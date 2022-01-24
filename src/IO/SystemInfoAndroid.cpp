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
		Text::PString u8arr[2];
		Text::PString u8arr2[2];
		sb.ClearStr();
		Manage::Process::ExecuteProcess(UTF8STRC("getprop"), &sb);
		u8arr[1].v = sb.ToString();
		u8arr[1].len = sb.GetLength();
		NEW_CLASS(cfg, IO::ConfigFile());
		while (1)
		{
			i = Text::StrSplitP(u8arr, 2, u8arr[1].v, u8arr[1].len, '\n');
			if (Text::StrSplitTrimP(u8arr2, 2, u8arr[0].v, u8arr[0].len, ':') == 2)
			{
				if (Text::StrEndsWithC(u8arr2[0].v, u8arr2[0].len, UTF8STRC("]")) && Text::StrEndsWithC(u8arr2[1].v, u8arr2[1].len, UTF8STRC("]")))
				{
					u8arr2[0].v[u8arr2[0].len - 1] = 0;
					u8arr2[1].v[u8arr2[1].len - 1] = 0;
					if (u8arr2[0].v[0] == '[' && u8arr2[1].v[0] == '[')
					{
						cfg->SetValue(0, 0, &u8arr2[0].v[1], u8arr2[0].len - 2, &u8arr2[1].v[1], u8arr2[1].len - 2);
					}
				}
			}
			if (i != 2)
				break;
		}
	}
	if (cfg)
	{
		Text::String *brand = cfg->GetValue(UTF8STRC("ro.product.brand"));
		Text::String *model = cfg->GetValue(UTF8STRC("ro.product.model"));
		Text::String *nickname = cfg->GetValue(UTF8STRC("ro.product.nickname"));
		sb.ClearStr();
		if (nickname)
		{
			sb.Append(nickname);
		}
		else if (brand && model)
		{
			if (model->StartsWith(brand))
			{
				sb.Append(model);
			}
			else
			{
				sb.Append(brand);
				sb.AppendC(UTF8STRC(" "));
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

