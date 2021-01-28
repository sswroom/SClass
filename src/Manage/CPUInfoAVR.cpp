#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/FileStream.h"
#include "IO/Path.h"
#include "IO/StreamReader.h"
#include "Manage/CPUInfo.h"
#include "Sync/Thread.h"
#include "Text/MyString.h"
#include "Text/StringBuilderUTF8.h"
#include "Text/StringBuilderW.h"
#include "Text/UTF8Reader.h"

Manage::CPUInfo::CPUInfo()
{
	this->infoCnt = 0;
	this->brand = Manage::CPUVendor::CB_ATMEL;
	this->familyId = 0;
	this->model = 0;
	this->steppingId = 0;
	this->clsData = 0;
}

Manage::CPUVendor::CPU_BRAND Manage::CPUInfo::GetBrand()
{
	return this->brand;
}

Int32 Manage::CPUInfo::GetFamilyId()
{
	return this->familyId;
}

Int32 Manage::CPUInfo::GetModelId()
{
	return this->model;
}

Int32 Manage::CPUInfo::GetStepping()
{
	return this->steppingId;
}

Bool Manage::CPUInfo::SupportIntelDTS()
{
	return false;
}

Int32 Manage::CPUInfo::GetTCC()
{
	return 0;
}

Bool Manage::CPUInfo::GetInfoValue(OSInt index, Text::StringBuilderUTF *sb)
{
	return false;
}

OSInt Manage::CPUInfo::GetCacheInfoList(Data::ArrayList<const UTF8Char*> *infoList)
{
	return false;
}

void Manage::CPUInfo::GetFeatureFlags(Int32 *flag1, Int32 *flag2)
{
	*flag1 = 0;
	*flag2 = 0;
}

Manage::CPUInfo::~CPUInfo()
{
	if (this->clsData)
		Text::StrDelNew((const UTF8Char*)this->clsData);
}

OSInt Manage::CPUInfo::GetInfoCnt()
{
	return this->infoCnt;
}

Bool Manage::CPUInfo::GetInfoName(OSInt index, Text::StringBuilderUTF *sb)
{
	if (index < 0 || index >= this->infoCnt)
		return false;
	switch (index)
	{
	case 0:
		sb->Append((const UTF8Char*)"Vendor identification");
		return true;
	default:
		return false;
	}
	return false;
}

UTF8Char *Manage::CPUInfo::GetCPUName(UTF8Char *sbuff)
{
	if (this->clsData)
	{
		return Text::StrConcat(sbuff, (const UTF8Char*)this->clsData);
	}
	return 0;
}

Bool Manage::CPUInfo::GetCPUTemp(OSInt index, Double *temp)
{
	return false;
}

Bool Manage::CPUInfo::GetCPURatio(Int32 *ratio)
{
	return false;
}

Bool Manage::CPUInfo::GetCPUTurboRatio(Int32 *ratio)
{
	return false;
}

Bool Manage::CPUInfo::GetCPUTCC(Double *temp)
{
	return false;
}

const UTF8Char *Manage::CPUInfo::GetCacheInfo(Manage::CPUVendor::CPU_BRAND brand, UInt8 descType)
{
	return 0;
}

const UTF8Char *Manage::CPUInfo::GetFeatureShortName(OSInt index)
{
	return 0;
}

const UTF8Char *Manage::CPUInfo::GetFeatureName(OSInt index)
{
	return 0;
}

const UTF8Char *Manage::CPUInfo::GetFeatureDesc(OSInt index)
{
	return 0;
}

void Manage::CPUInfo::AppendNameInfo10(Int32 ecxv, Int32 edxv, Text::StringBuilderUTF *sb)
{
}

void Manage::CPUInfo::AppendNameInfo11(Int32 ecxv, Int32 edxv, Text::StringBuilderUTF *sb)
{
}
