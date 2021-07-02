#include "Stdafx.h"
#include "IO/FileStream.h"
#include "IO/SystemInfo.h"
#include "IO/UnixConfigFile.h"
#include "Manage/Process.h"
#include "Text/MyString.h"
#include "Text/StringBuilderUTF8.h"
#include "Text/UTF8Reader.h"
#include <stdio.h>
#include <sys/sysinfo.h>

typedef struct
{
	const UTF8Char *platformName;
} SystemData;

IO::SystemInfo::SystemInfo()
{
	Text::StringBuilderUTF8 sb;
	OSInt i;
	SystemData *data = MemAlloc(SystemData, 1);
	data->platformName = 0;
	this->clsData = data;

	IO::ConfigFile *cfg = IO::UnixConfigFile::Parse((const UTF8Char*)"/system/build.prop");
	if (cfg == 0)
	{
		UTF8Char *u8arr[2];
		UTF8Char *u8arr2[2];
		sb.ClearStr();
		Manage::Process::ExecuteProcess((const UTF8Char*)"getprop", &sb);
		u8arr[1] = sb.ToString();
		NEW_CLASS(cfg, IO::ConfigFile());
		while (1)
		{
			i = Text::StrSplit(u8arr, 2, u8arr[1], '\n');
			if (Text::StrSplitTrim(u8arr2, 2, u8arr[0], ':') == 2)
			{
				if (Text::StrEndsWith(u8arr2[0], (const UTF8Char*)"]") && Text::StrEndsWith(u8arr2[1], (const UTF8Char*)"]"))
				{
					u8arr2[0][Text::StrCharCnt(u8arr2[0]) - 1] = 0;
					u8arr2[1][Text::StrCharCnt(u8arr2[1]) - 1] = 0;
					if (u8arr2[0][0] == '[' && u8arr2[1][0] == '[')
					{
						cfg->SetValue(0, &u8arr2[0][1], &u8arr2[1][1]);
					}
				}
			}
			if (i != 2)
				break;
		}
	}
	if (cfg)
	{
		const UTF8Char *brand = cfg->GetValue((const UTF8Char*)"ro.product.brand");
		const UTF8Char *model = cfg->GetValue((const UTF8Char*)"ro.product.model");
		const UTF8Char *nickname = cfg->GetValue((const UTF8Char*)"ro.product.nickname");
		sb.ClearStr();
		if (nickname)
		{
			sb.Append(nickname);
		}
		else if (brand && model)
		{
			if (Text::StrStartsWith(model, brand))
			{
				sb.Append(model);
			}
			else
			{
				sb.Append(brand);
				sb.Append((const UTF8Char*)" ");
				sb.Append(model);
			}
		}
		else if (model)
		{
			sb.Append(model);
		}
		data->platformName = Text::StrCopyNew(sb.ToString());
		DEL_CLASS(cfg);
	}
	else
	{
		//wprintf(L"Error in loading /system/build.prop\r\n");
	}
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
//	SystemData *data = (SystemData*)this->clsData;
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
	return IO::SystemInfo::CT_TABLET;
}

UOSInt IO::SystemInfo::GetRAMInfo(Data::ArrayList<RAMInfo*> *ramList)
{
	return 0;
}

void IO::SystemInfo::FreeRAMInfo(Data::ArrayList<RAMInfo*> *ramList)
{
}

