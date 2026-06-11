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
	Optional<Text::String> platformName;
	Optional<Text::String> platformSN;
};

IO::SystemInfo::SystemInfo()
{
	Text::StringBuilderUTF8 sb;
	UIntOS i;

	NN<ClassData> data = MemAllocNN(ClassData);
	data->platformName = nullptr;
	data->platformSN = nullptr;
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

	if (data->platformName.IsNull()) //Asus Routers
	{
		sb.ClearStr();
		Manage::Process::ExecuteProcess(CSTR("nvram get productid"), sb);
		if (sb.GetLength() > 0)
		{
			while (sb.EndsWith('\r') || sb.EndsWith('\n'))
			{
				sb.RemoveChars(1);
			}
			data->platformName = Text::String::New(sb.ToCString());
		}
	}

	if (data->platformName.IsNull()) //Bivocom
	{
		IO::FileStream fs(CSTR("/etc/fw_model"), IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
		if (!fs.IsError())
		{
			Text::UTF8Reader reader(fs);
			sb.ClearStr();
			if (reader.ReadLine(sb, 512))
			{
				data->platformName = Text::String::New(sb.ToCString());
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
	return IO::SystemInfo::CT_PERIPHERAL;
}

UIntOS IO::SystemInfo::GetRAMInfo(NN<Data::ArrayListNN<RAMInfo>> ramList)
{
	return 0;
}

void IO::SystemInfo::FreeRAMInfo(NN<Data::ArrayListNN<RAMInfo>> ramList)
{
}

