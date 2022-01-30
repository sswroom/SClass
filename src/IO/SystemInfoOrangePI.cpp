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
	Text::String *platformName;
};

IO::SystemInfo::SystemInfo()
{
	ClassData *data = MemAlloc(ClassData, 1);
	data->platformName = 0;
	this->clsData = data;
	if (IO::Path::GetPathType(UTF8STRC("/etc/OrangePi_Camera.conf")) == IO::Path::PathType::File)
	{
		IO::ConfigFile *cfg = IO::UnixConfigFile::Parse((const UTF8Char*)"/etc/OrangePi_Camera.conf");
		if (cfg)
		{
			Text::String *s = cfg->GetValue(UTF8STRC("Platform:"));
			if (s)
			{
				data->platformName = Text::String::New(s->v, s->leng);
			}
			DEL_CLASS(cfg);
		}
	}
	else if (IO::Path::GetPathType(UTF8STRC("/boot/orangepi/OrangePIH6.dtb")) == IO::Path::PathType::File)
	{
		data->platformName = Text::String::New(UTF8STRC("Orange PI One Plus"));
	}
	else if (IO::Path::GetPathType(UTF8STRC("/boot/orangepi/OrangePI-A64.dtb")) == IO::Path::PathType::File)
	{
		data->platformName = Text::String::New(UTF8STRC("Orange PI Win Plus"));
	}
}

IO::SystemInfo::~SystemInfo()
{
	SDEL_STRING(this->clsData->platformName);
	MemFree(this->clsData);
}

UTF8Char *IO::SystemInfo::GetPlatformName(UTF8Char *sbuff)
{
	if (this->clsData->platformName)
	{
		return this->clsData->platformName->ConcatTo(sbuff);
	}
	return 0;
}

UTF8Char *IO::SystemInfo::GetPlatformSN(UTF8Char *sbuff)
{
	return 0;
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

UOSInt IO::SystemInfo::GetRAMInfo(Data::ArrayList<RAMInfo*> *ramList)
{
	return 0;
}

void IO::SystemInfo::FreeRAMInfo(Data::ArrayList<RAMInfo*> *ramList)
{
}

