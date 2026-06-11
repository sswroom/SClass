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

struct IO::SystemInfo::ClassData
{
	Optional<Text::String> platformName;
	Optional<Text::String> platformSN;
};

IO::SystemInfo::SystemInfo()
{
	Text::StringBuilderUTF8 sb;
	UInt8 buff[256];
	NN<ClassData> data = MemAllocNN(ClassData);
	data->platformName = nullptr;
	data->platformSN = nullptr;
	this->clsData = data;

	Int64 flen;
	IO::FileStream fs(CSTR("/sys/class/i2c-dev/i2c-0/device/0-0056/eeprom"), IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
	if (!fs.IsError())
	{
		flen = fs.GetLength();
		if (flen >= 256 && (flen & 255) == 0)
		{
			fs.Read(BYTEARR(buff));
			if (buff[32])
			{
				OPTSTR_DEL(data->platformName);
				data->platformName = Text::String::NewNotNullSlow(&buff[32]);
			}
			if (buff[64])
			{
				OPTSTR_DEL(data->platformSN);
				data->platformSN = Text::String::NewNotNullSlow(&buff[64]);
			}
		}
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
	return IO::SystemInfo::CT_IOT_GATEWAY;
}

UIntOS IO::SystemInfo::GetRAMInfo(NN<Data::ArrayListNN<RAMInfo>> ramList)
{
	return 0;
}

void IO::SystemInfo::FreeRAMInfo(NN<Data::ArrayListNN<RAMInfo>> ramList)
{
}

