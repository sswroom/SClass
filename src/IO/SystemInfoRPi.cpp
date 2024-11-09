#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/FileStream.h"
#include "IO/Path.h"
#include "IO/SystemInfo.h"
#include "IO/UnixConfigFile.h"
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
	ClassData *data = MemAlloc(ClassData, 1);
	data->platformName = 0;
	data->platformSN = 0;
	this->clsData = data;

	{
		IO::FileStream fs(CSTR("/sys/firmware/devicetree/base/model"), IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
		if (!fs.IsError())
		{
			Text::UTF8Reader reader(fs);
			sb.ClearStr();
			while (reader.ReadLine(sb, 512))
			{
			}
			data->platformName = Text::String::New(sb.ToCString()).Ptr();
		}
	}
	{
		IO::FileStream fs(CSTR("/sys/firmware/devicetree/base/serial-number"), IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
		if (!fs.IsError())
		{
			Text::UTF8Reader reader(fs);
			sb.ClearStr();
			while (reader.ReadLine(sb, 512))
			{
			}
			data->platformSN = Text::String::New(sb.ToCString()).Ptr();
		}
	}
}

IO::SystemInfo::~SystemInfo()
{
	SDEL_STRING(this->clsData->platformName);
	SDEL_STRING(this->clsData->platformSN);
	MemFree(this->clsData);
}

UnsafeArrayOpt<UTF8Char> IO::SystemInfo::GetPlatformName(UnsafeArray<UTF8Char> sbuff)
{
	if (this->clsData->platformName)
	{
		return this->clsData->platformName->ConcatTo(sbuff);
	}
	return 0;
}

UnsafeArrayOpt<UTF8Char> IO::SystemInfo::GetPlatformSN(UnsafeArray<UTF8Char> sbuff)
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
	return IO::SystemInfo::CT_IOT_GATEWAY;
}

UOSInt IO::SystemInfo::GetRAMInfo(NN<Data::ArrayListNN<RAMInfo>> ramList)
{
	return 0;
}

void IO::SystemInfo::FreeRAMInfo(NN<Data::ArrayListNN<RAMInfo>> ramList)
{
}

