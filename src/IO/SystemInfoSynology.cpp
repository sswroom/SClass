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
	Text::String *platformName;
	Text::String *platformSN;
};

IO::SystemInfo::SystemInfo()
{
	IO::FileStream *fs;
	IO::StreamReader *reader;
	OSInt i;
	ClassData *data = MemAlloc(ClassData, 1);
	data->platformName = 0;
	data->platformSN = 0;
	this->clsData = data;
	IO::ConfigFile *cfg = IO::UnixConfigFile::Parse(CSTR("/etc/synoinfo.conf"));
	if (cfg)
	{
		Text::String *s = cfg->GetValue(CSTR("unique"));
		if (s)
		{
			data->platformName = s->Clone().Ptr();
		}
		DEL_CLASS(cfg);
	}
}

IO::SystemInfo::~SystemInfo()
{
	SDEL_STRING(this->clsData->platformName);
	SDEL_STRING(this->clsData->platformSN);
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
	if (this->clsData->platformSN)
	{
		return this->clsData->platformSN->ConcatTo(sbuff);
	}
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
	return IO::SystemInfo::CT_RAID;
}

UOSInt IO::SystemInfo::GetRAMInfo(Data::ArrayList<RAMInfo*> *ramList)
{
	return 0;
}

void IO::SystemInfo::FreeRAMInfo(Data::ArrayList<RAMInfo*> *ramList)
{
}

