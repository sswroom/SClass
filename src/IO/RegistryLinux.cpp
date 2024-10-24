#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ArrayListStringNN.h"
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
	NN<Text::String> fileName;
	Sync::Mutex mut;
	Optional<IO::ConfigFile> cfg;
	UInt32 useCnt;
	Bool modified;
	IO::Registry::RegistryUser usr;
};

struct Registry_Param
{
	Registry_File *reg;
	Text::CStringNN currCate;
};

struct IO::Registry::ClassData
{
	Registry_File *reg;
	NN<Text::String> cate;
};

void *IO::Registry::OpenUserType(RegistryUser usr)
{
	UTF8Char sbuff[512];
	UnsafeArray<UTF8Char> sptr;
	Registry_File *reg;
	switch (usr)
	{
	case IO::Registry::REG_USER_ALL:
		if (allRegistryFile)
		{
			Sync::Interlocked::IncrementU32(((Registry_File*)allRegistryFile)->useCnt);
			return allRegistryFile;
		}
		if (!IO::Path::GetUserHome(sbuff).SetTo(sptr))
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
		if (!IO::Path::GetUserHome(sbuff).SetTo(sptr))
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
	NN<IO::ConfigFile> cfg;
	if (Sync::Interlocked::DecrementU32(reg->useCnt) == 0)
	{
		if (reg->modified)
		{
			if (reg->cfg.SetTo(cfg))
			{
				IO::FileStream fs(reg->fileName, IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
				IO::IniFile::SaveConfig(fs, 65001, cfg);
			}
			reg->modified = false;
		}
		reg->cfg.Delete();
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

Optional<IO::Registry> IO::Registry::OpenSoftware(IO::Registry::RegistryUser usr, UnsafeArray<const WChar> compName, UnsafeArray<const WChar> appName)
{
	Registry_Param param; 
	param.reg = (Registry_File*)OpenUserType(usr);
	if (param.reg == 0)
	{
		return 0;
	}
	Text::StringBuilderUTF8 sb;
	sb.AppendC(UTF8STRC("Software\\"));
	NN<Text::String> s = Text::String::NewNotNull(compName);
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

Optional<IO::Registry> IO::Registry::OpenSoftware(IO::Registry::RegistryUser usr, UnsafeArray<const WChar> compName)
{
	Registry_Param param; 
	param.reg = (Registry_File*)OpenUserType(usr);
	if (param.reg == 0)
	{
		return 0;
	}
	Text::StringBuilderUTF8 sb;
	sb.AppendC(UTF8STRC("Software\\"));
	NN<Text::String> s = Text::String::NewNotNull(compName);
	sb.Append(s);
	s->Release();
	param.currCate = {sb.ToString(), sb.GetLength()};
	IO::Registry *reg;
	NEW_CLASS(reg, IO::Registry(&param));
	return reg;
}


Optional<IO::Registry> IO::Registry::OpenLocalHardware()
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

Optional<IO::Registry> IO::Registry::OpenLocalSoftware(UnsafeArray<const WChar> softwareName)
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

void IO::Registry::CloseRegistry(NN<IO::Registry> reg)
{
	IO::Registry *preg = reg.Ptr();
	DEL_CLASS(preg);
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

Optional<IO::Registry> IO::Registry::OpenSubReg(UnsafeArray<const WChar> name)
{
	Registry_Param param;
	param.reg = this->clsData->reg;
	Text::StringBuilderUTF8 sb;
	sb.Append(this->clsData->cate);
	sb.AppendUTF8Char('\\');
	NN<Text::String> s = Text::String::NewNotNull(name);
	sb.Append(s);
	s->Release();
	param.currCate = {sb.ToString(), sb.GetLength()};
	Sync::Interlocked::IncrementU32(param.reg->useCnt);
	IO::Registry *reg;
	NEW_CLASS(reg, IO::Registry(&param));
	return reg;
}

UnsafeArrayOpt<WChar> IO::Registry::GetSubReg(UnsafeArray<WChar> buff, UOSInt index)
{
	Data::ArrayListStrUTF8 names;
	Data::ArrayListStringNN cateList;
	Sync::MutexUsage mutUsage(this->clsData->reg->mut);
	NN<IO::ConfigFile> cfg;
	if (!this->clsData->reg->cfg.SetTo(cfg))
	{
		return 0;
	}
	NN<Text::String> cate;
	cfg->GetCateList(cateList, false);
	UnsafeArrayOpt<WChar> ret = 0;
	Text::StringBuilderUTF8 sbSubReg;
	UOSInt thisCateLen = this->clsData->cate->leng;
	Data::ArrayIterator<NN<Text::String>> it = cateList.Iterator();
	UOSInt k;
	while (it.HasNext())
	{
		cate = it.Next();
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
	}
	names.DeleteAll();
	return ret;
}

void IO::Registry::SetValue(UnsafeArray<const WChar> name, Int32 value)
{
	Text::StringBuilderUTF8 sb;
	sb.AppendC(UTF8STRC("dword:"));
	sb.AppendHex32((UInt32)value);

	Sync::MutexUsage mutUsage(this->clsData->reg->mut);
	NN<IO::ConfigFile> cfg;
	if (!this->clsData->reg->cfg.SetTo(cfg))
	{
		NEW_CLASSNN(cfg, IO::ConfigFile());
		this->clsData->reg->cfg = cfg;
	}
	NN<Text::String> s = Text::String::NewNotNull(name);
	cfg->SetValue(this->clsData->cate->ToCString(), s->ToCString(), sb.ToCString());
	this->clsData->reg->modified = true;
	s->Release();
}

void IO::Registry::SetValue(UnsafeArray<const WChar> name, UnsafeArrayOpt<const WChar> value)
{
	Text::StringBuilderUTF8 sb;
	sb.AppendC(UTF8STRC("sz:"));
	UnsafeArray<const WChar> nnvalue;
	NN<Text::String> s;
	if (value.SetTo(nnvalue))
	{
		s = Text::String::NewNotNull(nnvalue);
		sb.Append(s);
		s->Release();
	}

	Sync::MutexUsage mutUsage(this->clsData->reg->mut);
	NN<IO::ConfigFile> cfg;
	if (!this->clsData->reg->cfg.SetTo(cfg))
	{
		NEW_CLASSNN(cfg, IO::ConfigFile());
		this->clsData->reg->cfg = cfg;
	}
	s = Text::String::NewNotNull(name);
	cfg->SetValue(this->clsData->cate->ToCString(), s->ToCString(), sb.ToCString());
	this->clsData->reg->modified = true;
	s->Release();
}

void IO::Registry::DelValue(UnsafeArray<const WChar> name)
{
	NN<IO::ConfigFile> cfg;
	Sync::MutexUsage mutUsage(this->clsData->reg->mut);
	if (!this->clsData->reg->cfg.SetTo(cfg))
	{
		return;
	}
	NN<Text::String> s = Text::String::NewNotNull(name);
	cfg->RemoveValue(this->clsData->cate->ToCString(), s->ToCString());
	s->Release();
}

Int32 IO::Registry::GetValueI32(UnsafeArray<const WChar> name)
{
	NN<IO::ConfigFile> cfg;
	Sync::MutexUsage mutUsage(this->clsData->reg->mut);
	if (!this->clsData->reg->cfg.SetTo(cfg))
	{
		return 0;
	}
	NN<Text::String> s = Text::String::NewNotNull(name);
	NN<Text::String> csval;
	if (cfg->GetCateValue(this->clsData->cate, s).SetTo(csval) && csval->StartsWith(UTF8STRC("dword:")))
	{
		s->Release();
		return Text::StrHex2Int32C(UnsafeArray<const UInt8>(csval->v + 6));
	}
	s->Release();
	return 0;
}

UnsafeArrayOpt<WChar> IO::Registry::GetValueStr(UnsafeArray<const WChar> name, UnsafeArray<WChar> buff)
{
	NN<IO::ConfigFile> cfg;
	Sync::MutexUsage mutUsage(this->clsData->reg->mut);
	if (!this->clsData->reg->cfg.SetTo(cfg))
	{
		return 0;
	}
	NN<Text::String> s = Text::String::NewNotNull(name);
	NN<Text::String> csval;
	if (cfg->GetCateValue(this->clsData->cate, s).SetTo(csval) && csval->StartsWith(UTF8STRC("sz:")))
	{
		s->Release();
		return Text::StrUTF8_WChar(buff, csval->v + 3, 0);
	}
	s->Release();
	return 0;
}

Bool IO::Registry::GetValueI32(UnsafeArray<const WChar> name, OutParam<Int32> value)
{
	NN<IO::ConfigFile> cfg;
	Sync::MutexUsage mutUsage(this->clsData->reg->mut);
	if (!this->clsData->reg->cfg.SetTo(cfg))
	{
		return false;
	}
	NN<Text::String> s = Text::String::NewNotNull(name);
	NN<Text::String> csval;
	if (cfg->GetCateValue(this->clsData->cate, s).SetTo(csval) && csval->StartsWith(UTF8STRC("dword:")))
	{
		s->Release();
		value.Set(Text::StrHex2Int32C(UnsafeArray<const UInt8>(csval->v + 6)));
		return true;
	}
	s->Release();
	return false;
}

UnsafeArrayOpt<WChar> IO::Registry::GetName(UnsafeArray<WChar> nameBuff, UOSInt index)
{
	NN<IO::ConfigFile> cfg;
	Sync::MutexUsage mutUsage(this->clsData->reg->mut);
	if (!this->clsData->reg->cfg.SetTo(cfg))
	{
		return 0;
	}
	Data::ArrayListStringNN keys;
	cfg->GetKeys(this->clsData->cate->ToCString(), keys);
	NN<Text::String> key;
	if (keys.GetItem(index).SetTo(key))
	{
		return Text::StrUTF8_WChar(nameBuff, key->v, 0);
	}
	return 0;
}
