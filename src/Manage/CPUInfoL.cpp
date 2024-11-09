#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/X86Util.h"
#include "IO/FileStream.h"
#include "Manage/CPUInfo.h"
#include "Text/MyString.h"
#include "Text/StringBuilderUTF8.h"
#include "Text/UTF8Reader.h"
#if defined(__APPLE__)
#include <sys/sysctl.h>
#endif
Manage::CPUInfo::CPUInfo()
{
	this->infoCnt = 0;
	this->brand = Manage::CPUVendor::CB_UNKNOWN;
	this->familyId = 0;
	this->model = 0;
	this->steppingId = 0;
	this->clsData = 0;

#if defined(__APPLE__)
	UTF8Char sbuff[256];
	size_t size = sizeof(sbuff);
	if (sysctlbyname("machdep.cpu.brand_string", sbuff, &size, 0, 0) == 0)
	{
		this->clsData = (void*)Text::StrCopyNewC(sbuff, size).Ptr();
		if (Text::StrStartsWithC(sbuff, (UOSInt)size, UTF8STRC("Apple")))
		{
			this->brand = Manage::CPUVendor::CB_APPLE;
		}
		else if (Text::StrStartsWithC(sbuff, (UOSInt)size, UTF8STRC("Intel")))
		{
			this->brand = Manage::CPUVendor::CB_INTEL;
		}
	}
#else
	UOSInt i;
	OSInt sysType = 0;

	IO::FileStream fs(CSTR("/proc/cpuinfo"), IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
	if (!fs.IsError())
	{
//		Int32 cpuPart = 0;
		Text::StringBuilderUTF8 sb;
		Text::UTF8Reader reader(fs);
		while (reader.ReadLine(sb, 512))
		{
			if (sb.StartsWith(UTF8STRC("Hardware"))) //ARM
			{
				i = sb.IndexOf(UTF8STRC(": "));
				if (i != INVALID_INDEX && sysType <= 1)
				{
					if (this->clsData)
						Text::StrDelNew((const UTF8Char*)this->clsData);
					this->clsData = (void*)Text::StrCopyNew(sb.ToString() + i + 2).Ptr();
					sysType = 2;
				}
				if (sb.IndexOf(UTF8STRC(": BCM")) != INVALID_INDEX)
				{
					this->brand = Manage::CPUVendor::CB_BROADCOM;
				}
				else if (sb.IndexOf(UTF8STRC(" AM33")) != INVALID_INDEX)
				{
					this->brand = Manage::CPUVendor::CB_TI;
				}
				else if (sb.IndexOf(UTF8STRC(" MSM")) != INVALID_INDEX)
				{
					this->brand = Manage::CPUVendor::CB_QUALCOMM;
				}
				else if (sb.IndexOf(UTF8STRC(" RK30board")) != INVALID_INDEX)
				{
					this->brand = Manage::CPUVendor::CB_ROCKCHIP;
				}
				else if (sb.IndexOf(UTF8STRC(" Amlogic")) != INVALID_INDEX)
				{
					this->brand = Manage::CPUVendor::CB_AMLOGIC;
				}
				else if (sb.IndexOf(UTF8STRC(" Qualcomm ")) != INVALID_INDEX)
				{
					this->brand = Manage::CPUVendor::CB_QUALCOMM;
				}
				else if (sb.IndexOf(UTF8STRC(" Annapurna Labs Alpine")) != INVALID_INDEX)
				{
					this->brand = Manage::CPUVendor::CB_ANNAPURNA;
				}
				else if (sb.IndexOf(UTF8STRC(" Feroceon")) != INVALID_INDEX)
				{
					this->brand = Manage::CPUVendor::CB_MARVELL;
				}
				else if (sb.IndexOf(UTF8STRC(" rda")) != INVALID_INDEX)
				{
					this->brand = Manage::CPUVendor::CB_RDA;
				}
				else if (sb.IndexOf(UTF8STRC(" Atmel")) != INVALID_INDEX)
				{
					this->brand = Manage::CPUVendor::CB_ATMEL;
				}
			}
			else if (sb.StartsWith(UTF8STRC("system type"))) //MIPS
			{
				if (sb.IndexOf(UTF8STRC(": Qualcomm Atheros")) != INVALID_INDEX)
				{
					this->brand = Manage::CPUVendor::CB_ATHEROS;
				}
				i = sb.IndexOf(UTF8STRC(": "));
				if (this->clsData)
					Text::StrDelNew((const UTF8Char*)this->clsData);
				this->clsData = (void*)Text::StrCopyNew(sb.ToString() + i + 2).Ptr();
				sysType = 3;
			}
			else if (sb.StartsWith(UTF8STRC("CPU architecture")))
			{
				i = sb.IndexOf(UTF8STRC(": "));
				this->familyId = Text::StrToInt32(sb.ToString() + i + 2);
			}
			else if (sb.StartsWith(UTF8STRC("CPU variant")))
			{
				i = sb.IndexOf(UTF8STRC(": "));
				this->model = Text::StrToInt32(sb.ToString() + i + 2);
			}
			else if (sb.StartsWith(UTF8STRC("CPU revision")))
			{
				i = sb.IndexOf(UTF8STRC(": "));
				this->steppingId = Text::StrToInt32(sb.ToString() + i + 2);
			}
			else if (sb.StartsWith(UTF8STRC("model name"))) //x86
			{
				i = sb.IndexOf(UTF8STRC(": "));
				if (i != INVALID_INDEX && sysType <= 0)
				{
					if (this->clsData)
						Text::StrDelNew((const UTF8Char*)this->clsData);
					this->clsData = (void*)Text::StrCopyNew(sb.ToString() + i + 2).Ptr();
					sysType = 1;
				}
			}
			else if (sb.StartsWith(UTF8STRC("CPU part	:")))
			{
//				cpuPart = Text::StrToInt32(sb.ToString() + 11);
			}
			sb.ClearStr();
		}
	}
#endif
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

UOSInt Manage::CPUInfo::GetCacheInfoList(NN<Data::ArrayListArr<const UTF8Char>> infoList)
{
	return false;
}

void Manage::CPUInfo::GetFeatureFlags(OutParam<Int32> flag1, OutParam<Int32> flag2)
{
	flag1.Set(0);
	flag2.Set(0);
}

Manage::CPUInfo::~CPUInfo()
{
	if (this->clsData)
		Text::StrDelNew((const UTF8Char*)this->clsData);
	this->clsData = 0;
}

UOSInt Manage::CPUInfo::GetInfoCnt()
{
	return this->infoCnt;
}

Bool Manage::CPUInfo::GetInfoName(UOSInt index, NN<Text::StringBuilderUTF8> sb)
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

UnsafeArrayOpt<UTF8Char> Manage::CPUInfo::GetCPUName(UnsafeArray<UTF8Char> sbuff)
{
	if (this->clsData)
	{
		return Text::StrConcat(sbuff, (const UTF8Char*)this->clsData);
	}
	return 0;
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
