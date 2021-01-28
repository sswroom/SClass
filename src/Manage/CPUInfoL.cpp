#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/FileStream.h"
#include "Manage/CPUInfo.h"
#include "Text/MyString.h"
#include "Text/StringBuilderUTF8.h"
#include "Text/UTF8Reader.h"

#if defined(CPU_X86_32) || defined(CPU_X86_64)
#if defined(HAS_GCCASM32) || defined(HAS_GCCASM64)
static inline void native_cpuid(Int32 *eax, Int32 *ebx, Int32 *ecx, Int32 *edx)
{
	asm volatile("cpuid": "=a" (*eax), "=b" (*ebx), "=c" (*ecx), "=d" (*edx) : "0" (*eax), "2" (*ecx));
}

static inline void __cpuid(Int32 *cpuInfo, Int32 func)
{
	cpuInfo[0] = func;
	cpuInfo[2] = 0;
	native_cpuid(&cpuInfo[0], &cpuInfo[1], &cpuInfo[2], &cpuInfo[3]);
}

static inline void __cpuidex(Int32 *cpuInfo, Int32 func, Int32 subfunc)
{
	cpuInfo[0] = func;
	cpuInfo[2] = subfunc;
	native_cpuid(&cpuInfo[0], &cpuInfo[1], &cpuInfo[2], &cpuInfo[3]);
}
#else
extern "C"
{
	void CPUInfo_cpuid(Int32 *cpuInfo, Int32 func, Int32 subfunc);
}
#define __cpuid(cpuInfo, func) CPUInfo_cpuid(cpuInfo, func, 0)
#define __cpuidex(cpuInfo, func, subfunc) CPUInfo_cpuid(cpuInfo, func, subfunc)
#endif
#endif

Manage::CPUInfo::CPUInfo()
{
	IO::FileStream *fs;
	Text::UTF8Reader *reader;
	OSInt i;
	OSInt sysType = 0;

	this->infoCnt = 0;
	this->brand = Manage::CPUVendor::CB_UNKNOWN;
	this->familyId = 0;
	this->model = 0;
	this->steppingId = 0;
	this->clsData = 0;

	NEW_CLASS(fs, IO::FileStream((const UTF8Char*)"/proc/cpuinfo", IO::FileStream::FILE_MODE_READONLY, IO::FileStream::FILE_SHARE_DENY_NONE, IO::FileStream::BT_NORMAL));
	if (!fs->IsError())
	{
		Int32 cpuPart = 0;
		Text::StringBuilderUTF8 sb;
		NEW_CLASS(reader, Text::UTF8Reader(fs));
		while (reader->ReadLine(&sb, 512))
		{
			if (sb.StartsWith((const UTF8Char*)"Hardware")) //ARM
			{
				i = sb.IndexOf((const UTF8Char*)": ");
				if (i >= 0 && sysType <= 1)
				{
					if (this->clsData)
						Text::StrDelNew((const UTF8Char*)this->clsData);
					this->clsData = (void*)Text::StrCopyNew(sb.ToString() + i + 2);
					sysType = 2;
				}
				if (sb.IndexOf((const UTF8Char*)": BCM") >= 0)
				{
					this->brand = Manage::CPUVendor::CB_BROADCOM;
				}
				else if (sb.IndexOf((const UTF8Char*)" AM33") >= 0)
				{
					this->brand = Manage::CPUVendor::CB_TI;
				}
				else if (sb.IndexOf((const UTF8Char*)" MSM") >= 0)
				{
					this->brand = Manage::CPUVendor::CB_QUALCOMM;
				}
				else if (sb.IndexOf((const UTF8Char*)" RK30board") >= 0)
				{
					this->brand = Manage::CPUVendor::CB_ROCKCHIP;
				}
				else if (sb.IndexOf((const UTF8Char*)" Amlogic") >= 0)
				{
					this->brand = Manage::CPUVendor::CB_AMLOGIC;
				}
				else if (sb.IndexOf((const UTF8Char*)" Qualcomm ") >= 0)
				{
					this->brand = Manage::CPUVendor::CB_QUALCOMM;
				}
				else if (sb.IndexOf((const UTF8Char*)" Annapurna Labs Alpine") >= 0)
				{
					this->brand = Manage::CPUVendor::CB_ANNAPURNA;
				}
				else if (sb.IndexOf((const UTF8Char*)" Feroceon") >= 0)
				{
					this->brand = Manage::CPUVendor::CB_MARVELL;
				}
				else if (sb.IndexOf((const UTF8Char*)" rda") >= 0)
				{
					this->brand = Manage::CPUVendor::CB_RDA;
				}
				else if (sb.IndexOf((const UTF8Char*)" Atmel") >= 0)
				{
					this->brand = Manage::CPUVendor::CB_ATMEL;
				}
			}
			else if (sb.StartsWith((const UTF8Char*)"system type")) //MIPS
			{
				if (sb.IndexOf((const UTF8Char*)": Qualcomm Atheros") >= 0)
				{
					this->brand = Manage::CPUVendor::CB_ATHEROS;
				}
				i = sb.IndexOf((const UTF8Char*)": ");
				if (this->clsData)
					Text::StrDelNew((const UTF8Char*)this->clsData);
				this->clsData = (void*)Text::StrCopyNew(sb.ToString() + i + 2);
				sysType = 3;
			}
			else if (sb.StartsWith((const UTF8Char*)"CPU architecture"))
			{
				i = sb.IndexOf((const UTF8Char*)": ");
				this->familyId = Text::StrToInt32(sb.ToString() + i + 2);
			}
			else if (sb.StartsWith((const UTF8Char*)"CPU variant"))
			{
				i = sb.IndexOf((const UTF8Char*)": ");
				this->model = Text::StrToInt32(sb.ToString() + i + 2);
			}
			else if (sb.StartsWith((const UTF8Char*)"CPU revision"))
			{
				i = sb.IndexOf((const UTF8Char*)": ");
				this->steppingId = Text::StrToInt32(sb.ToString() + i + 2);
			}
			else if (sb.StartsWith((const UTF8Char*)"model name")) //x86
			{
				i = sb.IndexOf((const UTF8Char*)": ");
				if (i >= 0 && sysType <= 0)
				{
					if (this->clsData)
						Text::StrDelNew((const UTF8Char*)this->clsData);
					this->clsData = (void*)Text::StrCopyNew(sb.ToString() + i + 2);
					sysType = 1;
				}
			}
			else if (sb.StartsWith((const UTF8Char*)"CPU part	:"))
			{
				cpuPart = Text::StrToInt32(sb.ToString() + 11);
			}
			sb.ClearStr();
		}
		DEL_CLASS(reader);
	}
	DEL_CLASS(fs);
}


Bool Manage::CPUInfo::HasInstruction(InstructionType instType)
{
#if defined(CPU_X86_32) || defined(CPU_X86_64)
	Int32 cpuInfo[4];
#endif
	switch (instType)
	{
#if defined(CPU_X86_32) || defined(CPU_X86_64)
	case IT_X86:
		return true;
	case IT_SSE41:
		__cpuid(cpuInfo, 1);
		return (cpuInfo[2] & 0x80000) != 0;
	case IT_AVX:
		__cpuid(cpuInfo, 1);
		return (cpuInfo[2] & 0x10000000) != 0;
	case IT_AVX2:
		__cpuidex(cpuInfo, 7, 0);
		return (cpuInfo[1] & 0x20) != 0;
#endif
	default:
		return false;
	}
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
