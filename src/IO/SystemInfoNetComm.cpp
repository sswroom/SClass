#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/FileStream.h"
#include "IO/SystemInfo.h"
#include "Manage/Process.h"
#include "Text/MyString.h"
#include "Text/StringBuilderUTF8.h"
#include "Text/UTF8Reader.h"
#include <sys/sysinfo.h>

typedef struct
{
	const UTF8Char *platformName;
	const UTF8Char *platformSN;
} SystemData;

IO::SystemInfo::SystemInfo()
{
	Text::StringBuilderUTF8 sb;
	OSInt i;
	UTF8Char *u8arr[2];
	SystemData *data = MemAlloc(SystemData, 1);
	data->platformName = 0;
	data->platformSN = 0;
	this->clsData = data;

	sb.ClearStr();
	Manage::Process::ExecuteProcess((const UTF8Char*)"rdb_get -L", &sb);
	u8arr[1] = sb.ToString();

	while (true)
	{
		i = Text::StrSplitLine(u8arr, 2, u8arr[1]);
		if (Text::StrStartsWith(u8arr[0], (const UTF8Char*)"system.product.model "))
		{
			SDEL_TEXT(data->platformName);
			data->platformName = Text::StrCopyNew(&u8arr[0][21]);
		}
		else if (Text::StrStartsWith(u8arr[0], (const UTF8Char*)"systeminfo.serialnumber "))
		{
			SDEL_TEXT(data->platformSN);
			data->platformSN = Text::StrCopyNew(&u8arr[0][24]);
		}

		if (i != 2)
		{
			break;
		}
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

