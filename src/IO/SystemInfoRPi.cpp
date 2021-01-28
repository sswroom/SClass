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

typedef struct
{
	const UTF8Char *platformName;
} SystemData;

IO::SystemInfo::SystemInfo()
{
	IO::FileStream *fs;
	IO::StreamReader *reader;
	Text::StringBuilderUTF8 sb;
	OSInt i;
	SystemData *data = MemAlloc(SystemData, 1);
	data->platformName = 0;
	this->clsData = data;
	NEW_CLASS(fs, IO::FileStream((const UTF8Char*)"/sys/firmware/devicetree/base/model", IO::FileStream::FILE_MODE_READONLY, IO::FileStream::FILE_SHARE_DENY_NONE, IO::FileStream::BT_NORMAL));
	if (!fs->IsError())
	{
		NEW_CLASS(reader, IO::StreamReader(fs, 65001));
		sb.ClearStr();
		while (reader->ReadLine(&sb, 512))
		{
		}
		DEL_CLASS(reader);
		data->platformName = Text::StrCopyNew(sb.ToString());
	}
	DEL_CLASS(fs);
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

