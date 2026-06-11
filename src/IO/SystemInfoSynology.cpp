#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/FileStream.h"
#include "IO/Path.h"
#include "IO/StreamReader.h"
#include "IO/SystemInfo.h"
#include "IO/UnixConfigFile.h"
#include "Text/MyString.h"
#include "Text/StringBuilderUTF8.h"
#include <stdio.h>
#include <sys/sysinfo.h>

struct IO::SystemInfo::ClassData
{
	Optional<Text::String> platformName;
	Optional<Text::String> platformSN;
};

IO::SystemInfo::SystemInfo()
{
	IO::FileStream *fs;
	IO::StreamReader *reader;
	IntOS i;
	NN<ClassData> data = MemAllocNN(ClassData);
	data->platformName = nullptr;
	data->platformSN = nullptr;
	this->clsData = data;
	NN<IO::ConfigFile> cfg;
	if (IO::UnixConfigFile::Parse(CSTR("/etc/synoinfo.conf")).SetTo(cfg))
	{
		NN<Text::String> s;
		if (cfg->GetValue(CSTR("unique")).SetTo(s))
		{
			data->platformName = Text::String::New(s->ToCString());
		}
		cfg.Delete();
	}
}

IO::SystemInfo::~SystemInfo()
{
	OPTSTR_DEL(this->clsData->platformName);
	OPTSTR_DEL(this->clsData->platformSN);
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
	NN<Text::String> s;
	if (this->clsData->platformSN.SetTo(s))
	{
		return s->ConcatTo(sbuff);
	}
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
	return IO::SystemInfo::CT_RAID;
}

UIntOS IO::SystemInfo::GetRAMInfo(NN<Data::ArrayListNN<RAMInfo>> ramList)
{
	return 0;
}

void IO::SystemInfo::FreeRAMInfo(NN<Data::ArrayListNN<RAMInfo>> ramList)
{
}

