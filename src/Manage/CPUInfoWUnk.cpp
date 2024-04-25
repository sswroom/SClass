#include "Stdafx.h"
#if defined(CPU_X86_32) ||  defined(CPU_X86_64)
#include "CPUInfox86.cpp"

#undef __cpuid
#undef __cpuidex
#include <intrin.h>
extern "C" void CPUInfo_cpuid(Int32 *cpuInfo, Int32 func, Int32 subfunc)
{
	__cpuidex(cpuInfo, func, subfunc);
}
#else
#include "Stdafx.h"
#include "Manage/CPUInfo.h"

Manage::CPUInfo::CPUInfo()
{
	OSInt sysType = 0;

	this->infoCnt = 0;
	this->brand = Manage::CPUVendor::CB_UNKNOWN;
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

Bool Manage::CPUInfo::GetInfoValue(UOSInt index, NN<Text::StringBuilderUTF8> sb)
{
	return false;
}

UOSInt Manage::CPUInfo::GetCacheInfoList(Data::ArrayList<const UTF8Char*> *infoList)
{
	return 0;
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

UOSInt Manage::CPUInfo::GetInfoCnt()
{
	return this->infoCnt;
}

Bool Manage::CPUInfo::GetInfoName(UOSInt index, NN<Text::StringBuilderUTF8> sb)
{
	if (index >= this->infoCnt)
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

UTF8Char *Manage::CPUInfo::GetCPUName(UTF8Char *sbuff)
{
	if (this->clsData)
	{
		return Text::StrConcat(sbuff, (const UTF8Char*)this->clsData);
	}
	return 0;
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

Text::CStringNN Manage::CPUInfo::GetCacheInfo(Manage::CPUVendor::CPU_BRAND brand, UInt8 descType)
{
	return CSTR("Unknown");
}

Text::CStringNN Manage::CPUInfo::GetFeatureShortName(UOSInt index)
{
	return CSTR("Unknown");
}

Text::CStringNN Manage::CPUInfo::GetFeatureName(UOSInt index)
{
	return CSTR("Unknown");
}

Text::CStringNN Manage::CPUInfo::GetFeatureDesc(UOSInt index)
{
	return CSTR("Unknown");
}

void Manage::CPUInfo::AppendNameInfo10(UInt32 ecxv, UInt32 edxv, NN<Text::StringBuilderUTF8> sb)
{
}

void Manage::CPUInfo::AppendNameInfo11(UInt32 ecxv, UInt32 edxv, NN<Text::StringBuilderUTF8> sb)
{
}
#endif
