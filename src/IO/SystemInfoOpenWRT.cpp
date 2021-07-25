#include "Stdafx.h"
#include "IO/FileStream.h"
#include "IO/Path.h"
#include "IO/SystemInfo.h"
#include "Manage/Process.h"
#include "Text/MyString.h"
#include "Text/StringBuilderUTF8.h"
#include "Text/UTF8Reader.h"
#include <stdio.h>
#include <sys/sysinfo.h>

typedef struct
{
	const UTF8Char *platformName;
	const UTF8Char *platformSN;
} SystemData;

IO::SystemInfo::SystemInfo()
{
	IO::FileStream *fs;
	Text::UTF8Reader *reader;
	Text::StringBuilderUTF8 sb;
	UOSInt i;

	SystemData *data = MemAlloc(SystemData, 1);
	data->platformName = 0;
	data->platformSN = 0;
	this->clsData = data;

	NEW_CLASS(fs, IO::FileStream((const UTF8Char*)"/proc/cpuinfo", IO::FileStream::FILE_MODE_READONLY, IO::FileStream::FILE_SHARE_DENY_NONE, IO::FileStream::BT_NORMAL));
	if (!fs->IsError())
	{
		NEW_CLASS(reader, Text::UTF8Reader(fs));
		sb.ClearStr();
		while (reader->ReadLine(&sb, 512))
		{
			if (sb.StartsWith((const UTF8Char*)"machine"))
			{
				i = sb.IndexOf((const UTF8Char*)": ");
				data->platformName = Text::StrCopyNew(sb.ToString() + i + 2);
			}
			else if (sb.StartsWith((const UTF8Char*)"Serial"))
			{
				i = sb.IndexOf((const UTF8Char*)": ");
				data->platformSN = Text::StrCopyNew(sb.ToString() + i + 2);
			}
			sb.ClearStr();
		}
		DEL_CLASS(reader);
	}
	DEL_CLASS(fs);

	if (data->platformName == 0) //Asus Routers
	{
		sb.ClearStr();
		Manage::Process::ExecuteProcess((const UTF8Char*)"nvram get productid", &sb);
		if (sb.GetLength() > 0)
		{
			while (sb.EndsWith('\r') || sb.EndsWith('\n'))
			{
				sb.RemoveChars(1);
			}
			data->platformName = Text::StrCopyNew(sb.ToString());
		}
	}

	if (data->platformName == 0) //Bivocom
	{
		NEW_CLASS(fs, IO::FileStream((const UTF8Char*)"/etc/fw_model", IO::FileStream::FILE_MODE_READONLY, IO::FileStream::FILE_SHARE_DENY_NONE, IO::FileStream::BT_NORMAL));
		if (!fs->IsError())
		{
			NEW_CLASS(reader, Text::UTF8Reader(fs));
			sb.ClearStr();
			if (reader->ReadLine(&sb, 512))
			{
				data->platformName = Text::StrCopyNew(sb.ToString());
			}
			DEL_CLASS(reader);
		}
		DEL_CLASS(fs);
	}
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
	return IO::SystemInfo::CT_PERIPHERAL;
}

UOSInt IO::SystemInfo::GetRAMInfo(Data::ArrayList<RAMInfo*> *ramList)
{
	return 0;
}

void IO::SystemInfo::FreeRAMInfo(Data::ArrayList<RAMInfo*> *ramList)
{
}

