#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ArrayListStrUTF8.h"
#include "IO/FileStream.h"
#include "IO/IniFile.h"
#include "IO/Path.h"
#include "IO/Registry.h"
#include "Sync/Interlocked.h"
#include "Sync/Mutex.h"
#include "Sync/MutexUsage.h"
#include "Text/MyString.h"
#include "Text/MyStringW.h"

void *IO::Registry::thisRegistryFile = 0;
void *IO::Registry::allRegistryFile = 0;

struct Registry_File
{
	const UTF8Char *fileName;
	Sync::Mutex *mut;
	IO::ConfigFile *cfg;
	UInt32 useCnt;
	Bool modified;
	IO::Registry::RegistryUser usr;
};

struct Registry_Param
{
	Registry_File *reg;
	const UTF8Char *currCate;
};

struct IO::Registry::ClassData
{
	Registry_File *reg;
	const UTF8Char *cate;
};

void *IO::Registry::OpenUserType(RegistryUser usr)
{
	UTF8Char sbuff[512];
	UTF8Char *sptr;
	Registry_File *reg;
	switch (usr)
	{
	case IO::Registry::REG_USER_ALL:
		if (allRegistryFile)
		{
			Sync::Interlocked::Increment(&((Registry_File*)allRegistryFile)->useCnt);
			return allRegistryFile;
		}
		sptr = IO::Path::GetUserHome(sbuff);
		if (sptr == 0)
		{
			return 0;
		}
		sptr = Text::StrConcatC(sptr, UTF8STRC("/.sswr"));
		IO::Path::CreateDirectory(sbuff);
		sptr = Text::StrConcatC(sptr, UTF8STRC("/alluser.reg"));
		reg = MemAlloc(Registry_File, 1);
		allRegistryFile = reg;
		reg->useCnt = 1;
		reg->usr = usr;
		reg->fileName = Text::StrCopyNew(sbuff);
		NEW_CLASS(reg->mut, Sync::Mutex());
		reg->cfg = IO::IniFile::Parse(reg->fileName, 65001);
		reg->modified = false;
		return reg;
	case IO::Registry::REG_USER_THIS:
		if (thisRegistryFile)
		{
			Sync::Interlocked::Increment(&((Registry_File*)thisRegistryFile)->useCnt);
			return thisRegistryFile;
		}
		sptr = IO::Path::GetUserHome(sbuff);
		if (sptr == 0)
		{
			return 0;
		}
		sptr = Text::StrConcatC(sptr, UTF8STRC("/.sswr"));
		IO::Path::CreateDirectory(sbuff);
		sptr = Text::StrConcatC(sptr, UTF8STRC("/thisuser.reg"));

		reg = MemAlloc(Registry_File, 1);
		thisRegistryFile = reg;
		reg->useCnt = 1;
		reg->usr = usr;
		reg->fileName = Text::StrCopyNew(sbuff);
		NEW_CLASS(reg->mut, Sync::Mutex());
		reg->cfg = IO::IniFile::Parse(reg->fileName, 65001);
		reg->modified = false;
		return reg;
	default:
		return 0;
	}
}

void IO::Registry::CloseInternal(void *data)
{
	Registry_File *reg = (Registry_File*)data;
	if (Sync::Interlocked::Decrement(&reg->useCnt) == 0)
	{
		if (reg->modified)
		{
			IO::FileStream *fs;
			NEW_CLASS(fs, IO::FileStream(reg->fileName, IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal));
			IO::IniFile::SaveConfig(fs, 65001, reg->cfg);
			DEL_CLASS(fs);
			reg->modified = false;
		}
		DEL_CLASS(reg->mut);
		SDEL_CLASS(reg->cfg);
		Text::StrDelNew(reg->fileName);
		if (reg->usr == REG_USER_ALL)
		{
			allRegistryFile = 0;
		}
		else if (reg->usr == REG_USER_THIS)
		{
			thisRegistryFile = 0;
		}
		MemFree(reg);
	}
}

IO::Registry *IO::Registry::OpenSoftware(IO::Registry::RegistryUser usr, const WChar *compName, const WChar *appName)
{
	Registry_Param param; 
	param.reg = (Registry_File*)OpenUserType(usr);
	if (param.reg == 0)
	{
		return 0;
	}
	Text::StringBuilderUTF8 sb;
	sb.AppendC(UTF8STRC("Software\\"));
	const UTF8Char *csptr = Text::StrToUTF8New(compName);
	sb.Append(csptr);
	Text::StrDelNew(csptr);
	sb.AppendChar('\\', 1);
	csptr = Text::StrToUTF8New(appName);
	sb.Append(csptr);
	Text::StrDelNew(csptr);
	param.currCate = sb.ToString();
	IO::Registry *reg;
	NEW_CLASS(reg, IO::Registry(&param));
	return reg;
}

IO::Registry *IO::Registry::OpenSoftware(IO::Registry::RegistryUser usr, const WChar *compName)
{
	Registry_Param param; 
	param.reg = (Registry_File*)OpenUserType(usr);
	if (param.reg == 0)
	{
		return 0;
	}
	Text::StringBuilderUTF8 sb;
	sb.AppendC(UTF8STRC("Software\\"));
	const UTF8Char *csptr = Text::StrToUTF8New(compName);
	sb.Append(csptr);
	Text::StrDelNew(csptr);
	param.currCate = sb.ToString();
	IO::Registry *reg;
	NEW_CLASS(reg, IO::Registry(&param));
	return reg;
}


IO::Registry *IO::Registry::OpenLocalHardware()
{
	Registry_Param param; 
	param.reg = (Registry_File*)OpenUserType(REG_USER_ALL);
	if (param.reg == 0)
	{
		return 0;
	}
	param.currCate = (const UTF8Char*)"Hardware";
	IO::Registry *reg;
	NEW_CLASS(reg, IO::Registry(&param));
	return reg;
}

IO::Registry *IO::Registry::OpenLocalSoftware(const WChar *softwareName)
{
	Registry_Param param; 
	param.reg = (Registry_File*)OpenUserType(REG_USER_ALL);
	if (param.reg == 0)
	{
		return 0;
	}
	param.currCate = (const UTF8Char*)"Software";
	IO::Registry *reg;
	NEW_CLASS(reg, IO::Registry(&param));
	return reg;
}

void IO::Registry::CloseRegistry(IO::Registry *reg)
{
	DEL_CLASS(reg);
}

IO::Registry::Registry(void *hand)
{
	Registry_Param *param = (Registry_Param *)hand;
	this->clsData = MemAlloc(ClassData, 1);
	this->clsData->reg = param->reg;
	this->clsData->cate = Text::StrCopyNew(param->currCate);
}

IO::Registry::~Registry()
{
	this->CloseInternal(this->clsData->reg);
	Text::StrDelNew(this->clsData->cate);
	MemFree(this->clsData);
}

IO::Registry *IO::Registry::OpenSubReg(const WChar *name)
{
	Registry_Param param;
	param.reg = this->clsData->reg;
	Text::StringBuilderUTF8 sb;
	sb.Append(this->clsData->cate);
	sb.AppendChar('\\', 1);
	const UTF8Char *csptr = Text::StrToUTF8New(name);
	sb.Append(csptr);
	Text::StrDelNew(csptr);
	param.currCate = sb.ToString();
	Sync::Interlocked::Increment(&param.reg->useCnt);
	IO::Registry *reg;
	NEW_CLASS(reg, IO::Registry(&param));
	return reg;
}

WChar *IO::Registry::GetSubReg(WChar *buff, UOSInt index)
{
	Data::ArrayListStrUTF8 names;
	Data::ArrayList<Text::String*> cateList;
	Sync::MutexUsage mutUsage(this->clsData->reg->mut);
	if (this->clsData->reg->cfg == 0)
	{
		return 0;
	}
	Text::String *cate;
	this->clsData->reg->cfg->GetCateList(&cateList, false);
	WChar *ret = 0;
	Text::StringBuilderUTF8 sbSubReg;
	UOSInt thisCateLen = Text::StrCharCnt(this->clsData->cate);
	UOSInt i = 0;
	UOSInt j = cateList.GetCount();
	UOSInt k;
	while (i < j)
	{
		cate = cateList.GetItem(i);
		if (cate->StartsWith(this->clsData->cate) && cate->v[thisCateLen] == '\\')
		{
			k = Text::StrIndexOf(&cate->v[thisCateLen + 1], '\\');
			sbSubReg.ClearStr();
			if (k != INVALID_INDEX)
			{
				sbSubReg.AppendC(&cate->v[thisCateLen + 1], k);
			}
			else
			{
				sbSubReg.Append(&cate->v[thisCateLen + 1]);
			}
			if (names.SortedIndexOf(sbSubReg.ToString()) < 0)
			{
				if (names.GetCount() == index)
				{
					ret = Text::StrUTF8_WChar(buff, sbSubReg.ToString(), 0);
					break;
				}
				else
				{
					names.SortedInsert(Text::StrCopyNew(sbSubReg.ToString()));
				}
			}
		}
		i++;
	}
	LIST_FREE_FUNC(&names, Text::StrDelNew);
	return ret;
}

void IO::Registry::SetValue(const WChar *name, Int32 value)
{
	Text::StringBuilderUTF8 sb;
	sb.AppendC(UTF8STRC("dword:"));
	sb.AppendHex32((UInt32)value);

	Sync::MutexUsage mutUsage(this->clsData->reg->mut);
	if (this->clsData->reg->cfg == 0)
	{
		NEW_CLASS(this->clsData->reg->cfg, IO::ConfigFile());
	}
	const UTF8Char *csptr = Text::StrToUTF8New(name);
	this->clsData->reg->cfg->SetValue(this->clsData->cate, csptr, sb.ToString());
	this->clsData->reg->modified = true;
	Text::StrDelNew(csptr);
}

void IO::Registry::SetValue(const WChar *name, const WChar *value)
{
	Text::StringBuilderUTF8 sb;
	sb.AppendC(UTF8STRC("sz:"));
	const UTF8Char *csptr = Text::StrToUTF8New(value);
	sb.Append(csptr);
	Text::StrDelNew(csptr);

	Sync::MutexUsage mutUsage(this->clsData->reg->mut);
	if (this->clsData->reg->cfg == 0)
	{
		NEW_CLASS(this->clsData->reg->cfg, IO::ConfigFile());
	}
	csptr = Text::StrToUTF8New(name);
	this->clsData->reg->cfg->SetValue(this->clsData->cate, csptr, sb.ToString());
	this->clsData->reg->modified = true;
	Text::StrDelNew(csptr);
}

void IO::Registry::DelValue(const WChar *name)
{
	Sync::MutexUsage mutUsage(this->clsData->reg->mut);
	if (this->clsData->reg->cfg == 0)
	{
		return;
	}
	const UTF8Char *csptr = Text::StrToUTF8New(name);
	this->clsData->reg->cfg->RemoveValue(this->clsData->cate, csptr);
	Text::StrDelNew(csptr);
}

Int32 IO::Registry::GetValueI32(const WChar *name)
{
	Sync::MutexUsage mutUsage(this->clsData->reg->mut);
	if (this->clsData->reg->cfg == 0)
	{
		return 0;
	}
	const UTF8Char *csptr = Text::StrToUTF8New(name);
	Text::String *csval = this->clsData->reg->cfg->GetValue(this->clsData->cate, csptr);
	Text::StrDelNew(csptr);
	if (csval && csval->StartsWith((const UTF8Char*)"dword:"))
	{
		return Text::StrHex2Int32C(csval->v + 6);
	}
	return 0;
}

WChar *IO::Registry::GetValueStr(const WChar *name, WChar *buff)
{
	Sync::MutexUsage mutUsage(this->clsData->reg->mut);
	if (this->clsData->reg->cfg == 0)
	{
		return 0;
	}
	const UTF8Char *csptr = Text::StrToUTF8New(name);
	Text::String *csval = this->clsData->reg->cfg->GetValue(this->clsData->cate, csptr);
	Text::StrDelNew(csptr);
	if (csval && csval->StartsWith((const UTF8Char*)"sz:"))
	{
		return Text::StrUTF8_WChar(buff, csval->v + 3, 0);
	}
	return 0;
}

Bool IO::Registry::GetValueI32(const WChar *name, Int32 *value)
{
	Sync::MutexUsage mutUsage(this->clsData->reg->mut);
	if (this->clsData->reg->cfg == 0)
	{
		return false;
	}
	const UTF8Char *csptr = Text::StrToUTF8New(name);
	Text::String *csval = this->clsData->reg->cfg->GetValue(this->clsData->cate, csptr);
	Text::StrDelNew(csptr);
	if (csval && csval->StartsWith((const UTF8Char*)"dword:"))
	{
		*value = Text::StrHex2Int32C(csval->v + 6);
		return true;
	}
	return false;
}

WChar *IO::Registry::GetName(WChar *nameBuff, UOSInt index)
{
	Sync::MutexUsage mutUsage(this->clsData->reg->mut);
	if (this->clsData->reg->cfg == 0)
	{
		return 0;
	}
	Data::ArrayList<Text::String*> keys;
	this->clsData->reg->cfg->GetKeys(this->clsData->cate, &keys);
	Text::String *key = keys.GetItem(index);
	if (key)
	{
		return Text::StrUTF8_WChar(nameBuff, key->v, 0);
	}
	return 0;
}
