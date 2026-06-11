#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/Path.h"
#include "IO/SystemInfo.h"
#include "IO/UnixConfigFile.h"
#include "Text/MyString.h"
#include "Text/String.h"
#include <stdio.h>
#include <sys/sysinfo.h>

struct IO::SystemInfo::ClassData
{
	Optional<Text::String> platformName;
};

IO::SystemInfo::SystemInfo()
{
	NN<ClassData> data = MemAllocNN(ClassData);
	data->platformName = nullptr;
	this->clsData = data;
	if (IO::Path::GetPathType(CSTR("/etc/OrangePi_Camera.conf")) == IO::Path::PathType::File)
	{
		NN<IO::ConfigFile> cfg;
		if (IO::UnixConfigFile::Parse(CSTR("/etc/OrangePi_Camera.conf")).SetTo(cfg))
		{
			NN<Text::String> s;
			if (cfg->GetValue(CSTR("Platform:")).SetTo(s))
			{
				data->platformName = Text::String::New(s->v, s->leng);
			}
			cfg.Delete();
		}
	}
	else if (IO::Path::GetPathType(CSTR("/boot/orangepi/OrangePIH6.dtb")) == IO::Path::PathType::File)
	{
		data->platformName = Text::String::New(CSTR("Orange PI One Plus")).Ptr();
	}
	else if (IO::Path::GetPathType(CSTR("/boot/orangepi/OrangePI-A64.dtb")) == IO::Path::PathType::File)
	{
		data->platformName = Text::String::New(CSTR("Orange PI Win Plus")).Ptr();
	}
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
	return IO::SystemInfo::CT_IOT_GATEWAY;
}

UIntOS IO::SystemInfo::GetRAMInfo(NN<Data::ArrayListNN<RAMInfo>> ramList)
{
	return 0;
}

void IO::SystemInfo::FreeRAMInfo(NN<Data::ArrayListNN<RAMInfo>> ramList)
{
}

