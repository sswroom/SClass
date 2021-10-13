#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/FileStream.h"
#include "IO/SystemInfo.h"
#include "Manage/Process.h"
#include "Text/MyString.h"
#include "Text/StringBuilderUTF8.h"
#include "Text/UTF8Reader.h"
#include <sys/sysinfo.h>
#include <stdio.h>

typedef struct
{
	const UTF8Char *platformName;
	const UTF8Char *platformSN;
} SystemData;

IO::SystemInfo::SystemInfo()
{
	Text::StringBuilderUTF8 sb;
	UInt8 buff[256];
	SystemData *data = MemAlloc(SystemData, 1);
	data->platformName = 0;
	data->platformSN = 0;
	this->clsData = data;

	Int64 flen;
	IO::FileStream *fs;
	NEW_CLASS(fs, IO::FileStream((const UTF8Char*)"/sys/class/i2c-dev/i2c-0/device/0-0056/eeprom", IO::FileStream::FileMode::ReadOnly, IO::FileStream::FileShare::DenyNone, IO::FileStream::BufferType::Normal));
	if (!fs->IsError())
	{
		flen = fs->GetLength();
		if (flen >= 256 && (flen & 255) == 0)
		{
			fs->Read(buff, 256);
			if (buff[32])
			{
				SDEL_TEXT(data->platformName);
				data->platformName = Text::StrCopyNew(&buff[32]);
			}
			if (buff[64])
			{
				SDEL_TEXT(data->platformSN);
				data->platformSN = Text::StrCopyNew(&buff[64]);
			}
		}
	}
	SDEL_CLASS(fs);
}

IO::SystemInfo::~SystemInfo()
{
	SystemData *data = (SystemData*)this->clsData;
	SDEL_TEXT(data->platformName);
	SDEL_TEXT(data->platformSN);
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
	SystemData *data = (SystemData*)this->clsData;
	if (data->platformSN)
	{
		return Text::StrConcat(sbuff, data->platformSN);
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
	return IO::SystemInfo::CT_IOT_GATEWAY;
}

UOSInt IO::SystemInfo::GetRAMInfo(Data::ArrayList<RAMInfo*> *ramList)
{
	return 0;
}

void IO::SystemInfo::FreeRAMInfo(Data::ArrayList<RAMInfo*> *ramList)
{
}

