#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/Path.h"
#include "IO/SystemInfo.h"
#include "IO/UnixConfigFile.h"
#include "Text/MyString.h"
#include <stdio.h>
#include <sys/sysinfo.h>

typedef struct
{
	const UTF8Char *platformName;
} SystemData;

IO::SystemInfo::SystemInfo()
{
	SystemData *data = MemAlloc(SystemData, 1);
	data->platformName = 0;
	this->clsData = data;
	if (IO::Path::GetPathType((const UTF8Char*)"/etc/OrangePi_Camera.conf") == IO::Path::PT_FILE)
	{
		IO::ConfigFile *cfg = IO::UnixConfigFile::Parse((const UTF8Char*)"/etc/OrangePi_Camera.conf");
		if (cfg)
		{
			const UTF8Char *csptr = cfg->GetValue((const UTF8Char*)"Platform:");
			if (csptr)
			{
				data->platformName = Text::StrCopyNew(csptr);
			}
			DEL_CLASS(cfg);
		}
	}
	else if (IO::Path::GetPathType((const UTF8Char*)"/boot/orangepi/OrangePIH6.dtb") == IO::Path::PT_FILE)
	{
		data->platformName = Text::StrCopyNew((const UTF8Char*)"Orange PI One Plus");
	}
	else if (IO::Path::GetPathType((const UTF8Char*)"/boot/orangepi/OrangePI-A64.dtb") == IO::Path::PT_FILE)
	{
		data->platformName = Text::StrCopyNew((const UTF8Char*)"Orange PI Win Plus");
	}
}

IO::SystemInfo::~SystemInfo()
{
	SystemData *data = (SystemData*)this->clsData;
	SDEL_TEXT(data->platformName);
	MemFree(data);
}

UTF8Char *IO::SystemInfo::GetPlatformName(UTF8Char *sbuff)
{
	SystemData *data = (SystemData*)this->clsData;
	if (data->platformName)
	{
		return Text::StrConcat(sbuff, data->platformName);
	}
	return 0;
}

UTF8Char *IO::SystemInfo::GetPlatformSN(UTF8Char *sbuff)
{
	SystemData *info = (SystemData*)this->clsData;
	return 0;
}

Int64 IO::SystemInfo::GetTotalMemSize()
{
	struct sysinfo info;
	if (sysinfo(&info) == 0)
	{
		return info.totalram * (Int64)info.mem_unit;
	}
	return 0;
}

Int64 IO::SystemInfo::GetTotalUsableMemSize()
{
	struct sysinfo info;
	if (sysinfo(&info) == 0)
	{
		return info.totalram * (Int64)info.mem_unit;
	}
	return 0;
}

IO::SystemInfo::ChassisType IO::SystemInfo::GetChassisType()
{
	return IO::SystemInfo::CT_IOT_GATEWAY;
}

OSInt IO::SystemInfo::GetRAMInfo(Data::ArrayList<RAMInfo*> *ramList)
{
	return 0;
}

void IO::SystemInfo::FreeRAMInfo(Data::ArrayList<RAMInfo*> *ramList)
{
}

