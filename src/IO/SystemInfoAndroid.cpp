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

struct IO::SystemInfo::ClassData
{
	Text::String *platformName;
};

IO::SystemInfo::SystemInfo()
{
	Text::StringBuilderUTF8 sb;
	OSInt i;
	ClassData *data = MemAlloc(ClassData, 1);
	data->platformName = 0;
	this->clsData = data;

	IO::ConfigFile *cfg = IO::UnixConfigFile::Parse(CSTR("/system/build.prop"));
	if (cfg == 0)
	{
		Text::PString u8arr[2];
		Text::PString u8arr2[2];
		sb.ClearStr();
		Manage::Process::ExecuteProcess(CSTR("getprop"), &sb);
		u8arr[1] = sb;
		NEW_CLASS(cfg, IO::ConfigFile());
		while (1)
		{
			i = Text::StrSplitP(u8arr, 2, u8arr[1], '\n');
			if (Text::StrSplitTrimP(u8arr2, 2, u8arr[0], ':') == 2)
			{
				if (Text::StrEndsWithC(u8arr2[0].v, u8arr2[0].leng, UTF8STRC("]")) && Text::StrEndsWithC(u8arr2[1].v, u8arr2[1].leng, UTF8STRC("]")))
				{
					u8arr2[0].v[u8arr2[0].leng - 1] = 0;
					u8arr2[1].v[u8arr2[1].leng - 1] = 0;
					if (u8arr2[0].v[0] == '[' && u8arr2[1].v[0] == '[')
					{
						u8arr2[0].RemoveChars(1);
						u8arr2[1].RemoveChars(1);
						cfg->SetValue(CSTR_NULL, u8arr2[0].ToCString().Substring(1), u8arr2[1].ToCString().Substring(1));
					}
				}
			}
			if (i != 2)
				break;
		}
	}
	if (cfg)
	{
		Text::String *brand = cfg->GetValue(CSTR("ro.product.brand"));
		Text::String *model = cfg->GetValue(CSTR("ro.product.model"));
		Text::String *nickname = cfg->GetValue(CSTR("ro.product.nickname"));
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
		data->platformName = Text::String::New(sb.ToString(), sb.GetLength());
		DEL_CLASS(cfg);
	}
	else
	{
		//wprintf(L"Error in loading /system/build.prop\r\n");
	}
}

IO::SystemInfo::~SystemInfo()
{
	SDEL_STRING(this->clsData->platformName);
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

