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
	Text::String *platformName;
	Text::String *platformSN;
} SystemData;

IO::SystemInfo::SystemInfo()
{
	Text::StringBuilderUTF8 sb;
	OSInt i;
	Text::PString u8arr[2];
	ClassData *data = MemAlloc(ClassData, 1);
	data->platformName = 0;
	data->platformSN = 0;
	this->clsData = data;

	sb.ClearStr();
	Manage::Process::ExecuteProcess(CSTR("rdb_get -L"), sb);
	u8arr[1] = sb;

	while (true)
	{
		i = Text::StrSplitLineP(u8arr, 2, u8arr[1]);
		if (u8arr[0].StartsWith(UTF8STRC("system.product.model ")))
		{
			SDEL_STRING(data->platformName);
			data->platformName = Text::String::New(&u8arr[0].v[21], u8arr[0].leng - 21).Ptr();
		}
		else if (u8arr[0].StartsWith(UTF8STRC("systeminfo.serialnumber ")))
		{
			SDEL_STRING(data->platformSN);
			data->platformSN = Text::String::New(&u8arr[0].v[24], u8arr[0].leng - 24).Ptr();
		}

		if (i != 2)
		{
			break;
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
	return IO::SystemInfo::CT_IOT_GATEWAY;
}

UOSInt IO::SystemInfo::GetRAMInfo(Data::ArrayList<RAMInfo*> *ramList)
{
	return 0;
}

void IO::SystemInfo::FreeRAMInfo(Data::ArrayList<RAMInfo*> *ramList)
{
}

