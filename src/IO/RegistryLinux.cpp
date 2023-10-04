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
#include "Text/CString.h"
#include "Text/MyString.h"
#include "Text/MyStringW.h"

void *IO::Registry::thisRegistryFile = 0;
void *IO::Registry::allRegistryFile = 0;

struct Registry_File
{
	NotNullPtr<Text::String> fileName;
	Sync::Mutex mut;
	IO::ConfigFile *cfg;
	UInt32 useCnt;
	Bool modified;
	IO::Registry::RegistryUser usr;
};

struct Registry_Param
{
	Registry_File *reg;
	Text::CString currCate;
};

struct IO::Registry::ClassData
{
	Registry_File *reg;
	NotNullPtr<Text::String> cate;
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
			Sync::Interlocked::IncrementU32(((Registry_File*)allRegistryFile)->useCnt);
			return allRegistryFile;
		}
		sptr = IO::Path::GetUserHome(sbuff);
		if (sptr == 0)
		{
			return 0;
		}
		sptr = Text::StrConcatC(sptr, UTF8STRC("/.sswr"));
		IO::Path::CreateDirectory(CSTRP(sbuff, sptr));
		sptr = Text::StrConcatC(sptr, UTF8STRC("/alluser.reg"));
		NEW_CLASS(reg, Registry_File());
		allRegistryFile = reg;
		reg->useCnt = 1;
		reg->usr = usr;
		reg->fileName = Text::String::New(sbuff, (UOSInt)(sptr - sbuff));
		reg->cfg = IO::IniFile::Parse(reg->fileName->ToCString(), 65001);
		reg->modified = false;
		return reg;
	case IO::Registry::REG_USER_THIS:
		if (thisRegistryFile)
		{
			Sync::Interlocked::IncrementU32(((Registry_File*)thisRegistryFile)->useCnt);
			return thisRegistryFile;
		}
		sptr = IO::Path::GetUserHome(sbuff);
		if (sptr == 0)
		{
			return 0;
		}
		sptr = Text::StrConcatC(sptr, UTF8STRC("/.sswr"));
		IO::Path::CreateDirectory(CSTRP(sbuff, sptr));
		sptr = Text::StrConcatC(sptr, UTF8STRC("/thisuser.reg"));

		NEW_CLASS(reg, Registry_File());
		thisRegistryFile = reg;
		reg->useCnt = 1;
		reg->usr = usr;
		reg->fileName = Text::String::New(sbuff, (UOSInt)(sptr - sbuff));
		reg->cfg = IO::IniFile::Parse(reg->fileName->ToCString(), 65001);
		reg->modified = false;
		return reg;
	default:
		return 0;
	}
}

void IO::Registry::CloseInternal(void *data)
{
	Registry_File *reg = (Registry_File*)data;
	if (Sync::Interlocked::DecrementU32(reg->useCnt) == 0)
	{
		if (reg->modified)
		{
			IO::FileStream fs(reg->fileName, IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
			IO::IniFile::SaveConfig(fs, 65001, reg->cfg);
			reg->modified = false;
		}
		SDEL_CLASS(reg->cfg);
		reg->fileName->Release();
		if (reg->usr == REG_USER_ALL)
		{
			allRegistryFile = 0;
		}
		else if (reg->usr == REG_USER_THIS)
		{
			thisRegistryFile = 0;
		}
		DEL_CLASS(reg);
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
	NotNullPtr<Text::String> s = Text::String::NewNotNull(compName);
	sb.Append(s);
	s->Release();
	sb.AppendUTF8Char('\\');
	s = Text::String::NewNotNull(appName);
	sb.Append(s);
	s->Release();
	param.currCate = {sb.ToString(), sb.GetLength()};
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
	NotNullPtr<Text::String> s = Text::String::NewNotNull(compName);
	sb.Append(s);
	s->Release();
	param.currCate = {sb.ToString(), sb.GetLength()};
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
	param.currCate = CSTR("Hardware");
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
	param.currCate = CSTR("Software");
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
	this->clsData->cate = Text::String::New(param->currCate.v, param->currCate.leng);
}

IO::Registry::~Registry()
{
	this->CloseInternal(this->clsData->reg);
	this->clsData->cate->Release();
	MemFree(this->clsData);
}

IO::Registry *IO::Registry::OpenSubReg(const WChar *name)
{
	Registry_Param param;
	param.reg = this->clsData->reg;
	Text::StringBuilderUTF8 sb;
	sb.Append(this->clsData->cate);
	sb.AppendUTF8Char('\\');
	NotNullPtr<Text::String> s = Text::String::NewNotNull(name);
	sb.Append(s);
	s->Release();
	param.currCate = {sb.ToString(), sb.GetLength()};
	Sync::Interlocked::IncrementU32(param.reg->useCnt);
	IO::Registry *reg;
	NEW_CLASS(reg, IO::Registry(&param));
	return reg;
}

WChar *IO::Registry::GetSubReg(WChar *buff, UOSInt index)
{
	Data::ArrayListStrUTF8 names;
	Data::ArrayListNN<Text::String> cateList;
	Sync::MutexUsage mutUsage(this->clsData->reg->mut);
	if (this->clsData->reg->cfg == 0)
	{
		return 0;
	}
	Text::String *cate;
	this->clsData->reg->cfg->GetCateList(&cateList, false);
	WChar *ret = 0;
	Text::StringBuilderUTF8 sbSubReg;
	UOSInt thisCateLen = this->clsData->cate->leng;
	UOSInt i = 0;
	UOSInt j = cateList.GetCount();
	UOSInt k;
	while (i < j)
	{
		cate = cateList.GetItem(i);
		if (cate->StartsWith(this->clsData->cate) && cate->v[thisCateLen] == '\\')
		{
			k = Text::StrIndexOfChar(&cate->v[thisCateLen + 1], '\\');
			sbSubReg.ClearStr();
			if (k != INVALID_INDEX)
			{
				sbSubReg.AppendC(&cate->v[thisCateLen + 1], k);
			}
			else
			{
				sbSubReg.AppendC(&cate->v[thisCateLen + 1], cate->leng - thisCateLen - 1);
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
					names.SortedInsert(Text::StrCopyNewC(sbSubReg.ToString(), sbSubReg.GetLength()).Ptr());
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
	NotNullPtr<Text::String> s = Text::String::NewNotNull(name);
	this->clsData->reg->cfg->SetValue(this->clsData->cate->ToCString(), s->ToCString(), sb.ToCString());
	this->clsData->reg->modified = true;
	s->Release();
}

void IO::Registry::SetValue(const WChar *name, const WChar *value)
{
	Text::StringBuilderUTF8 sb;
	sb.AppendC(UTF8STRC("sz:"));
	NotNullPtr<Text::String> s = Text::String::NewNotNull(value);
	sb.Append(s);
	s->Release();

	Sync::MutexUsage mutUsage(this->clsData->reg->mut);
	if (this->clsData->reg->cfg == 0)
	{
		NEW_CLASS(this->clsData->reg->cfg, IO::ConfigFile());
	}
	s = Text::String::NewNotNull(name);
	this->clsData->reg->cfg->SetValue(this->clsData->cate->ToCString(), s->ToCString(), sb.ToCString());
	this->clsData->reg->modified = true;
	s->Release();
}

void IO::Registry::DelValue(const WChar *name)
{
	Sync::MutexUsage mutUsage(this->clsData->reg->mut);
	if (this->clsData->reg->cfg == 0)
	{
		return;
	}
	NotNullPtr<Text::String> s = Text::String::NewNotNull(name);
	this->clsData->reg->cfg->RemoveValue(this->clsData->cate->ToCString(), s->ToCString());
	s->Release();
}

Int32 IO::Registry::GetValueI32(const WChar *name)
{
	Sync::MutexUsage mutUsage(this->clsData->reg->mut);
	if (this->clsData->reg->cfg == 0)
	{
		return 0;
	}
	NotNullPtr<Text::String> s = Text::String::NewNotNull(name);
	Text::String *csval = this->clsData->reg->cfg->GetCateValue(this->clsData->cate, s);
	s->Release();
	if (csval && csval->StartsWith(UTF8STRC("dword:")))
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
	NotNullPtr<Text::String> s = Text::String::NewNotNull(name);
	Text::String *csval = this->clsData->reg->cfg->GetCateValue(this->clsData->cate, s);
	s->Release();
	if (csval && csval->StartsWith(UTF8STRC("sz:")))
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
	NotNullPtr<Text::String> s = Text::String::NewNotNull(name);
	Text::String *csval = this->clsData->reg->cfg->GetCateValue(this->clsData->cate, s);
	s->Release();
	if (csval && csval->StartsWith(UTF8STRC("dword:")))
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
	Data::ArrayListNN<Text::String> keys;
	this->clsData->reg->cfg->GetKeys(this->clsData->cate->ToCString(), &keys);
	Text::String *key = keys.GetItem(index);
	if (key)
	{
		return Text::StrUTF8_WChar(nameBuff, key->v, 0);
	}
	return 0;
}
