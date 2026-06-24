#include "Stdafx.h"
#include "MyMemory.h"
#include "Manage/CPUInfo.h"
#include "Text/MyString.h"

Manage::CPUInfo::CPUInfo()
{
	this->infoCnt = 0;
	this->brand = Manage::CPUVendor::CB_ATMEL;
	this->familyId = 0;
	this->model = 0;
	this->steppingId = 0;
	this->clsData = 0;
}

Manage::CPUInfo::~CPUInfo()
{
	if (this->clsData)
		Text::StrDelNew((const UTF8Char*)this->clsData);
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

UIntOS Manage::CPUInfo::GetInfoCnt()
{
	return this->infoCnt;
}

Bool Manage::CPUInfo::GetInfoName(UIntOS index, NN<Text::StringBuilderUTF8> sb)
{
	if (index < 0 || index >= this->infoCnt)
		return false;
	switch (index)
	{
	case 0:
		sb->AppendC(UTF8STRC("Vendor identification"));
		return true;
	default:
		return false;
	}
	return false;
}

Bool Manage::CPUInfo::GetInfoValue(UIntOS index, NN<Text::StringBuilderUTF8> sb)
{
	return false;
}

UIntOS Manage::CPUInfo::GetCacheInfoList(NN<Data::ArrayListArr<const UTF8Char>> infoList)
{
	return false;
}

void Manage::CPUInfo::GetFeatureFlags(OutParam<Int32> flag1, OutParam<Int32> flag2)
{
	flag1.Set(0);
	flag2.Set(0);
}

UnsafeArrayOpt<UTF8Char> Manage::CPUInfo::GetCPUName(UnsafeArray<UTF8Char> sbuff)
{
	if (this->clsData)
	{
		return Text::StrConcat(sbuff, (const UTF8Char*)this->clsData);
	}
	return nullptr;
}

Bool Manage::CPUInfo::GetCPURatio(OutParam<Int32> ratio)
{
	return false;
}

Bool Manage::CPUInfo::GetCPUTurboRatio(OutParam<Int32> ratio)
{
	return false;
}

Bool Manage::CPUInfo::GetCPUTCC(OutParam<Double> temp)
{
	return false;
}

Text::CStringNN Manage::CPUInfo::GetCacheInfo(Manage::CPUVendor::CPU_BRAND brand, UInt8 descType)
{
	return CSTR("Unknown");
}

Text::CStringNN Manage::CPUInfo::GetFeatureShortName(UIntOS index)
{
	return CSTR("Unknown");
}

Text::CStringNN Manage::CPUInfo::GetFeatureName(UIntOS index)
{
	return CSTR("Unknown");
}

Text::CStringNN Manage::CPUInfo::GetFeatureDesc(UIntOS index)
{
	return CSTR("Unknown");
}

void Manage::CPUInfo::AppendNameInfo10(UInt32 ecxv, UInt32 edxv, NN<Text::StringBuilderUTF8> sb)
{
}

void Manage::CPUInfo::AppendNameInfo11(UInt32 ecxv, UInt32 edxv, NN<Text::StringBuilderUTF8> sb)
{
}
