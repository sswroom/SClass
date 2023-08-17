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

struct IO::SystemInfo::ClassData
{
	Text::String *platformName;
	Text::String *platformSN;
};

IO::SystemInfo::SystemInfo()
{
	Text::StringBuilderUTF8 sb;
	UOSInt i;

	ClassData *data = MemAlloc(ClassData, 1);
	data->platformName = 0;
	data->platformSN = 0;
	this->clsData = data;

	{
		IO::FileStream fs(CSTR("/proc/cpuinfo"), IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
		if (!fs.IsError())
		{
			Text::UTF8Reader reader(fs);
			sb.ClearStr();
			while (reader.ReadLine(sb, 512))
			{
				if (sb.StartsWith(UTF8STRC("machine")))
				{
					i = sb.IndexOf(UTF8STRC(": "));
					data->platformName = Text::String::New(sb.ToString() + i + 2, sb.GetLength() - i - 2).Ptr();
				}
				else if (sb.StartsWith(UTF8STRC("Serial")))
				{
					i = sb.IndexOf(UTF8STRC(": "));
					data->platformSN = Text::String::New(sb.ToString() + i + 2, sb.GetLength() - i - 2).Ptr();
				}
				sb.ClearStr();
			}
		}
	}

	if (data->platformName == 0) //Asus Routers
	{
		sb.ClearStr();
		Manage::Process::ExecuteProcess(CSTR("nvram get productid"), sb);
		if (sb.GetLength() > 0)
		{
			while (sb.EndsWith('\r') || sb.EndsWith('\n'))
			{
				sb.RemoveChars(1);
			}
			data->platformName = Text::String::New(sb.ToCString()).Ptr();
		}
	}

	if (data->platformName == 0) //Bivocom
	{
		IO::FileStream fs(CSTR("/etc/fw_model"), IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
		if (!fs.IsError())
		{
			Text::UTF8Reader reader(fs);
			sb.ClearStr();
			if (reader.ReadLine(sb, 512))
			{
				data->platformName = Text::String::New(sb.ToCString()).Ptr();
			}
		}
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
	return IO::SystemInfo::CT_PERIPHERAL;
}

UOSInt IO::SystemInfo::GetRAMInfo(Data::ArrayList<RAMInfo*> *ramList)
{
	return 0;
}

void IO::SystemInfo::FreeRAMInfo(Data::ArrayList<RAMInfo*> *ramList)
{
}

