#include "Stdafx.h"
#include "IO/FileStream.h"
#include "IO/SystemInfo.h"
#include "IO/UnixConfigFile.h"
#include "Manage/Process.h"
#include "Text/MyString.h"
#include "Text/StringBuilderUTF8.h"
#include "Text/UTF8Reader.h"
#include <stdio.h>
#include <sys/sysinfo.h>

struct IO::SystemInfo::ClassData
{
	Optional<Text::String> platformName;
};

IO::SystemInfo::SystemInfo()
{
	Text::StringBuilderUTF8 sb;
	UIntOS i;
	NN<ClassData> data = MemAllocNN(ClassData);
	data->platformName = nullptr;
	this->clsData = data;

	NN<IO::ConfigFile> cfg;
	if (!IO::UnixConfigFile::Parse(CSTR("/system/build.prop")).SetTo(cfg))
	{
		Text::PString u8arr[2];
		Text::PString u8arr2[2];
		sb.ClearStr();
		Manage::Process::ExecuteProcess(CSTR("getprop"), sb);
		u8arr[1] = sb;
		NEW_CLASSNN(cfg, IO::ConfigFile());
		while (1)
		{
			i = Text::StrSplitP(u8arr, 2, u8arr[1], '\n');
			if (Text::StrSplitTrimP(u8arr2, 2, u8arr[0], ':') == 2)
			{
				if (Text::StrEndsWithC(u8arr2[0].v, u8arr2[0].leng, UTF8STRC("]")) && Text::StrEndsWithC(u8arr2[1].v, u8arr2[1].leng, UTF8STRC("]")))
				{
					u8arr2[0].v[u8arr2[0].leng - 1] = 0;
					u8arr2[1].v[u8arr2[1].leng - 1] = 0;
					if (u8arr2[0].v[0] == '[' && u8arr2[1].v[0] == '[')
					{
						u8arr2[0].RemoveChars(1);
						u8arr2[1].RemoveChars(1);
						cfg->SetValue(CSTR(""), u8arr2[0].ToCString().Substring(1), u8arr2[1].ToCString().Substring(1));
					}
				}
			}
			if (i != 2)
				break;
		}
	}
	Optional<Text::String> brand = cfg->GetValue(CSTR("ro.product.brand"));
	Optional<Text::String> model = cfg->GetValue(CSTR("ro.product.model"));
	Optional<Text::String> nickname = cfg->GetValue(CSTR("ro.product.nickname"));
	NN<Text::String> s;
	NN<Text::String> s2;
	sb.ClearStr();
	if (nickname.SetTo(s))
	{
		sb.Append(s);
	}
	else if (brand.SetTo(s) && model.SetTo(s2))
	{
		if (s2->StartsWith(s))
		{
			sb.Append(s2);
		}
		else
		{
			sb.Append(s);
			sb.AppendC(UTF8STRC(" "));
			sb.Append(s2);
		}
	}
	else if (model.SetTo(s))
	{
		sb.Append(s);
	}
	data->platformName = Text::String::New(sb.ToString(), sb.GetLength());
	cfg.Delete();
}

IO::SystemInfo::~SystemInfo()
{
	OPTSTR_DEL(this->clsData->platformName);
	MemFreeNN(this->clsData);
}

UnsafeArrayOpt<UTF8Char> IO::SystemInfo::GetPlatformName(UnsafeArray<UTF8Char> sbuff)
{
	NN<Text::String> s;
	if (this->clsData->platformName.SetTo(s))
	{
		return s->ConcatTo(sbuff);
	}
	return nullptr;
}

UnsafeArrayOpt<UTF8Char> IO::SystemInfo::GetPlatformSN(UnsafeArray<UTF8Char> sbuff)
{
//	SystemData *data = (SystemData*)this->clsData;
	return nullptr;
}

UInt64 IO::SystemInfo::GetTotalMemSize()
{
	struct sysinfo info;
	if (sysinfo(&info) == 0)
	{
		return info.totalram * (UInt64)info.mem_unit;
	}
	return 0;
}

UInt64 IO::SystemInfo::GetTotalUsableMemSize()
{
	struct sysinfo info;
	if (sysinfo(&info) == 0)
	{
		return info.totalram * (UInt64)info.mem_unit;
	}
	return 0;
}

IO::SystemInfo::ChassisType IO::SystemInfo::GetChassisType()
{
	return IO::SystemInfo::CT_TABLET;
}

UIntOS IO::SystemInfo::GetRAMInfo(NN<Data::ArrayListNN<RAMInfo>> ramList)
{
	return 0;
}

void IO::SystemInfo::FreeRAMInfo(NN<Data::ArrayListNN<RAMInfo>> ramList)
{
}

