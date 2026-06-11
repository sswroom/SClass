#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/FileStream.h"
#include "IO/SystemInfo.h"
#include "Manage/Process.h"
#include "Text/MyString.h"
#include "Text/StringBuilderUTF8.h"
#include "Text/UTF8Reader.h"
#include <sys/sysinfo.h>

struct IO::SystemInfo::ClassData
{
	Optional<Text::String> platformName;
	Optional<Text::String> platformSN;
} SystemData;

IO::SystemInfo::SystemInfo()
{
	Text::StringBuilderUTF8 sb;
	IntOS i;
	Text::PString u8arr[2];
	NN<ClassData> data = MemAllocNN(ClassData);
	data->platformName = nullptr;
	data->platformSN = nullptr;
	this->clsData = data;

	sb.ClearStr();
	Manage::Process::ExecuteProcess(CSTR("rdb_get -L"), sb);
	u8arr[1] = sb;

	while (true)
	{
		i = Text::StrSplitLineP(u8arr, 2, u8arr[1]);
		if (u8arr[0].StartsWith(UTF8STRC("system.product.model ")))
		{
			OPTSTR_DEL(data->platformName);
			data->platformName = Text::String::New(&u8arr[0].v[21], u8arr[0].leng - 21);
		}
		else if (u8arr[0].StartsWith(UTF8STRC("systeminfo.serialnumber ")))
		{
			OPTSTR_DEL(data->platformSN);
			data->platformSN = Text::String::New(&u8arr[0].v[24], u8arr[0].leng - 24);
		}

		if (i != 2)
		{
			break;
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

