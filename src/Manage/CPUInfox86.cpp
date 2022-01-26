#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/X86Util.h"
#include "IO/FileStream.h"
#include "IO/Library.h"
#include "IO/Registry.h"
#include "Manage/CPUInfo.h"
#include "Text/MyStringW.h"
#include "Text/StringBuilderUTF8.h"
#include "Text/UTF8Reader.h"

#if defined(WIN32) || defined(_WIN64)
typedef void (__stdcall *NoRetFunc)();
typedef UInt32 (__stdcall *UInt32RetFunc)();
typedef void (__stdcall *RdmsrFunc)(UInt32 ecx, UInt32 *eax, UInt32 *edx);

typedef struct
{
	IO::Library *winRing0;
	NoRetFunc InitializeOls;
	UInt32RetFunc GetDllStatus;
	RdmsrFunc Rdmsr;
	NoRetFunc DeinitializeOls;
} InfoData;
#else
typedef struct
{
	const UTF8Char *cpuName;
} InfoData;
#if defined(__FreeBSD__) || defined(__APPLE__)
#include <sys/types.h>
#include <sys/sysctl.h>
#elif defined(__sun__)
#include <sys/types.h>
#include <sys/processor.h>
#endif
#endif

Manage::CPUInfo::CPUInfo()
{
	Int32 cpuInfo[4];

#if defined(WIN32) || defined(_WIN64)
	InfoData *info = MemAlloc(InfoData, 1);
	info->winRing0 = 0;
	info->InitializeOls = 0;
	info->GetDllStatus = 0;
	info->Rdmsr = 0;
	info->DeinitializeOls = 0;
#ifdef _WIN64
	NEW_CLASS(info->winRing0, IO::Library((const UTF8Char*)"WinRing0x64.dll"));
#else
	NEW_CLASS(info->winRing0, IO::Library((const UTF8Char*)"WinRing0.dll"));
#endif
	if (info->winRing0->IsError())
	{
		DEL_CLASS(info->winRing0);
		info->winRing0 = 0;
	}
	else
	{
		info->InitializeOls = (NoRetFunc)info->winRing0->GetFunc("InitializeOls");
		info->GetDllStatus = (UInt32RetFunc)info->winRing0->GetFunc("GetDllStatus");
		info->Rdmsr = (RdmsrFunc)info->winRing0->GetFunc("Rdmsr");
		info->DeinitializeOls = (NoRetFunc)info->winRing0->GetFunc("DeinitializeOls");
		if (info->InitializeOls == 0 || info->GetDllStatus == 0 || info->Rdmsr == 0 || info->DeinitializeOls == 0)
		{
			DEL_CLASS(info->winRing0);
			info->winRing0 = 0;
		}
		else
		{
			info->InitializeOls();
			if (info->GetDllStatus() != 0)
			{
				DEL_CLASS(info->winRing0);
				info->winRing0 = 0;
			}
		}
	}
	this->clsData = info;
#else
	InfoData *info = MemAlloc(InfoData, 1);
	info->cpuName = 0;
#if defined(__APPLE__)
	Text::StringBuilderUTF8 sb;
	Char cbuff[1024];
	int mib[2];
	size_t size = 1024;
	if (sysctlbyname("machdep.cpu.brand_string", cbuff, &size, 0, 0) == 0)
	{
		sb.ClearStr();
		sb.AppendC((const UTF8Char*)cbuff, size);
		info->cpuName = Text::StrCopyNew(sb.ToString());
	}
#elif defined(__FreeBSD__)
	Text::StringBuilderUTF8 sb;
	Char cbuff[1024];
	int mib[2];
	mib[0] = CTL_HW;
	mib[1] = HW_MODEL;
	size_t size = 1024;
	if (sysctl(mib, 2, cbuff, &size, 0, 0) == 0)
	{
		sb.ClearStr();
		sb.AppendC((const UTF8Char*)cbuff, size);
		info->cpuName = Text::StrCopyNew(sb.ToString());
	}
#elif defined(__sun__)
	processor_info_t pinfo;
	if (processor_info(getcpuid(), &pinfo) == 0)
	{
		info->cpuName = Text::StrCopyNew((const UTF8Char*)pinfo.pi_processor_type);
	}
#else
	IO::FileStream *fs;
	Text::UTF8Reader *reader;
	Text::StringBuilderUTF8 sb;
	UOSInt i;
	NEW_CLASS(fs, IO::FileStream((const UTF8Char*)"/proc/cpuinfo", IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal));
	if (!fs->IsError())
	{
		sb.ClearStr();
		NEW_CLASS(reader, Text::UTF8Reader(fs));
		while (reader->ReadLine(&sb, 512))
		{
			if (sb.StartsWith(UTF8STRC("model name")))
			{
				i = sb.IndexOf(UTF8STRC(": "));
				info->cpuName = Text::StrCopyNew(sb.ToString() + i + 2);
				break;
			}
			sb.ClearStr();
		}
		DEL_CLASS(reader);
	}
	DEL_CLASS(fs);
#endif
	this->clsData = info;
#endif

	Core::X86Util_cpuid(cpuInfo, 0);
	if (cpuInfo[0] <= 0)
	{
		this->infoCnt = 2;
	}
	else if (cpuInfo[0] <= 1)
	{
		this->infoCnt = 12;
	}
	else if (cpuInfo[0] <= 2)
	{
		this->infoCnt = 13;
	}
	else
	{
		this->infoCnt = 13;
	}
	if (cpuInfo[1] == 0x756e6547 && cpuInfo[3] == 0x49656e69 && cpuInfo[2] == 0x6c65746e)
	{
		this->brand = Manage::CPUVendor::CB_INTEL;
	}
	else if (cpuInfo[1] == 0x68747541 && cpuInfo[3] == 0x69746E65 && cpuInfo[2] == 0x444D4163)
	{
		this->brand = Manage::CPUVendor::CB_AMD;
		}
	else
	{
		this->brand = Manage::CPUVendor::CB_UNKNOWN;
	}

	Core::X86Util_cpuid(cpuInfo, 1);
	if ((cpuInfo[0] & 0xf00) == 0xf00)
	{
		this->familyId = ((cpuInfo[0] & 0xf00) >> 8) + ((cpuInfo[0] & 0xff00000) >> 20);
	}
	else
	{
		this->familyId = (cpuInfo[0] & 0xf00) >> 8;
	}
	this->steppingId = cpuInfo[0] & 0xf;
	if (this->familyId == 6 || this->familyId >= 0xf)
	{
		this->model = ((cpuInfo[0] & 0xf0000) >> 12) | ((cpuInfo[0] & 0xf0) >> 4);
	}
	else
	{
		this->model = ((cpuInfo[0] & 0xf0) >> 4);
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
	Int32 cpuInfo[4];
	if (this->brand == Manage::CPUVendor::CB_INTEL)
	{
		Core::X86Util_cpuid(cpuInfo, 6);
		return cpuInfo[0] & 1;
	}
	else
	{
		return false;
	}
}


Bool Manage::CPUInfo::GetInfoValue(UOSInt index, Text::StringBuilderUTF8 *sb)
{
	Int32 cpuInfo[4];
	switch (index)
	{
	case 0:
		{
			UTF8Char cbuff[13];
			Core::X86Util_cpuid(cpuInfo, 0);
			*(Int32*)&cbuff[0] = cpuInfo[1];
			*(Int32*)&cbuff[4] = cpuInfo[3];
			*(Int32*)&cbuff[8] = cpuInfo[2];
			cbuff[12] = 0;
			sb->AppendC(cbuff, 12);
		}
		return true;
	case 1:
		{
			Core::X86Util_cpuid(cpuInfo, 0);
			sb->AppendI32(cpuInfo[0]);
		}
		return true;
	case 2:
		{
			Core::X86Util_cpuid(cpuInfo, 0);
			if (this->brand == Manage::CPUVendor::CB_INTEL)
			{
				switch ((cpuInfo[0] >> 12) & 3)
				{
				case 0:
					sb->AppendC(UTF8STRC("0 (Original OEM Processor)"));
					break;
				case 1:
					sb->AppendC(UTF8STRC("1 (Intel OneDrive Processor)"));
					break;
				case 2:
					sb->AppendC(UTF8STRC("2 (Dual Processor)"));
					break;
				case 3:
					sb->AppendC(UTF8STRC("3 (Intel reserved)"));
					break;
				}
			}
			else
			{
				sb->AppendI32((cpuInfo[0] >> 12) & 3);
			}
		}
		return true;
	case 3:
		{
			Int32 v;
			Core::X86Util_cpuid(cpuInfo, 1);
			if ((cpuInfo[0] & 0xf00) == 0xf00)
			{
				v = ((cpuInfo[0] >> 20) & 0xff) + 15;
			}
			else
			{
				v = (cpuInfo[0] >> 8) & 15;
			}
			sb->AppendHex8((UInt8)v);
		}
		return true;
	case 4:
		{
			Int32 v;
			Int32 v2;
			Core::X86Util_cpuid(cpuInfo, 1);
			if ((cpuInfo[0] & 0xf0) == 0xf0)
			{
				v = ((cpuInfo[0] >> 12) & 15) | ((cpuInfo[0] >> 4) & 15);
			}
			else
			{
				if ((cpuInfo[0] & 0xf00) == 0xf00)
				{
					v2 = ((cpuInfo[0] >> 20) & 0xff) + 15;
				}
				else
				{
					v2 = (cpuInfo[0] >> 8) & 15;
				}
				if (v2 == 6 || v2 >= 15)
				{
					v = ((cpuInfo[0] >> 12) & 0xf0) | ((cpuInfo[0] >> 4) & 15);
				}
				else
				{
					v = (cpuInfo[0] >> 4) & 15;
				}
			}
			sb->AppendHex8((UInt8)v);
		}
		return true;
	case 5:
		{
			Core::X86Util_cpuid(cpuInfo, 1);
			sb->AppendI32(cpuInfo[0] & 15);
		}
		return true;
	case 6:
		{
			Core::X86Util_cpuid(cpuInfo, 1);
			sb->AppendI32(cpuInfo[1] & 255);
		}
		return true;
	case 7:
		{
			Core::X86Util_cpuid(cpuInfo, 1);
			sb->AppendI32((cpuInfo[1] >> 8) & 255);
		}
		return true;
	case 8:
		{
			Core::X86Util_cpuid(cpuInfo, 1);
			sb->AppendI32((cpuInfo[1] >> 16) & 255);
		}
		return true;
	case 9:
		{
			Core::X86Util_cpuid(cpuInfo, 1);
			sb->AppendI32((cpuInfo[1] >> 24) & 255);
		}
		return true;
	case 10:
		{
			Core::X86Util_cpuid(cpuInfo, 1);
			AppendNameInfo10((UInt32)cpuInfo[2], (UInt32)cpuInfo[3], sb);
		}
		return true;
	case 11:
		{
			Core::X86Util_cpuid(cpuInfo, 1);
			AppendNameInfo11((UInt32)cpuInfo[2], (UInt32)cpuInfo[3], sb);
		}
		return true;
	case 12:
		{
			UInt8 buff[16];
			OSInt i;
			Bool firstFound;
			Text::CString cstr;
			Core::X86Util_cpuid(cpuInfo, 2);
			*(Int32*)&buff[0] = cpuInfo[0];
			*(Int32*)&buff[4] = cpuInfo[3];
			*(Int32*)&buff[8] = cpuInfo[2];
			*(Int32*)&buff[12] = cpuInfo[1];
			firstFound = true;
			i = 1;
			while (i < 16)
			{
				cstr = GetCacheInfo(this->brand, buff[i]);
				if (cstr.v)
				{
					if (firstFound)
					{
						firstFound = false;
					}
					else
					{
						sb->AppendC(UTF8STRC(", "));
					}
					sb->Append(cstr);
				}
				i++;
			}
		}
		return true;
	default:
		return false;
	}
	return false;
}

UOSInt Manage::CPUInfo::GetCacheInfoList(Data::ArrayList<const UTF8Char*> *infoList)
{
	if (this->infoCnt <= 13)
		return 0;
	UInt8 buff[16];
	OSInt i;
	UOSInt retCnt = 0;
	Text::CString cstr;
	Int32 cpuInfo[4];
	Core::X86Util_cpuid(cpuInfo, 2);
	*(Int32*)&buff[0] = cpuInfo[0];
	*(Int32*)&buff[4] = cpuInfo[3];
	*(Int32*)&buff[8] = cpuInfo[2];
	*(Int32*)&buff[12] = cpuInfo[1];
	i = 1;
	while (i < 16)
	{
		cstr = GetCacheInfo(this->brand, buff[i]);
		if (cstr.v)
		{
			infoList->Add(cstr.v);
			retCnt++;
		}
		i++;
	}
	return retCnt;
}

void Manage::CPUInfo::GetFeatureFlags(Int32 *flag1, Int32 *flag2)
{
	Int32 cpuInfo[4];
	Core::X86Util_cpuid(cpuInfo, 1);
	*flag1 = cpuInfo[3];
	*flag2 = cpuInfo[2];
}

Manage::CPUInfo::~CPUInfo()
{
#if defined(WIN32) || defined(_WIN64)
	InfoData *info = (InfoData*)this->clsData;
	if (info->winRing0)
	{
		info->DeinitializeOls();
		DEL_CLASS(info->winRing0);
	}
	MemFree(info);
#else
	InfoData *info = (InfoData*)this->clsData;
	SDEL_TEXT(info->cpuName);
	MemFree(info);
#endif
}

UOSInt Manage::CPUInfo::GetInfoCnt()
{
	return this->infoCnt;
}

Bool Manage::CPUInfo::GetInfoName(UOSInt index, Text::StringBuilderUTF8 *sb)
{
	if (index >= this->infoCnt)
		return false;
	switch (index)
	{
	case 0:
		sb->AppendC(UTF8STRC("Vendor identification"));
		return true;
	case 1:
		sb->AppendC(UTF8STRC("Max CPUID Input"));
		return true;
	case 2:
		sb->AppendC(UTF8STRC("Processor Type"));
		return true;
	case 3:
		sb->AppendC(UTF8STRC("Processor Family ID"));
		return true;
	case 4:
		sb->AppendC(UTF8STRC("Processor Model ID"));
		return true;
	case 5:
		sb->AppendC(UTF8STRC("Processor Stepping"));
		return true;
	case 6:
		sb->AppendC(UTF8STRC("8 Bit Brand ID"));
		return true;
	case 7:
		sb->AppendC(UTF8STRC("CLFLUSH size"));
		return true;
	case 8:
		sb->AppendC(UTF8STRC("Logical Processor Count exist"));
		return true;
	case 9:
		sb->AppendC(UTF8STRC("Local APIC physical ID"));
		return true;
	case 10:
		sb->AppendC(UTF8STRC("Instruction Sets"));
		return true;
	case 11:
		sb->AppendC(UTF8STRC("CPU Features"));
		return true;
	case 12:
		sb->AppendC(UTF8STRC("TLB/Cache/Prefetch Information"));
		return true;
	default:
		return false;
	}
	return false;
}

#if defined(WIN32) || defined(_WIN64)
UTF8Char *Manage::CPUInfo::GetCPUName(UTF8Char *u8buff)
{
	UTF8Char *ret = 0;
	IO::Registry *reg = IO::Registry::OpenLocalHardware();
	IO::Registry *reg2;
	WChar sbuff[256];
	if (reg)
	{
		reg2 = reg->OpenSubReg(L"DESCRIPTION\\System\\CentralProcessor\\0");
		if (reg2)
		{
			WChar *sptr;
			sptr = reg2->GetValueStr(L"ProcessorNameString", sbuff);
			IO::Registry::CloseRegistry(reg2);
			if (sptr)
			{
				ret = Text::StrWChar_UTF8(u8buff, sbuff);
				ret = Text::StrTrim(u8buff);
			}
		}
		IO::Registry::CloseRegistry(reg);
	}
	return ret;
}

Bool Manage::CPUInfo::GetCPURatio(Int32 *ratio)
{
	InfoData *info = (InfoData*)this->clsData;
	if (info->winRing0 == 0)
		return false;

	if (this->brand == Manage::CPUVendor::CB_INTEL)
	{
		UInt32 eax;
		UInt32 edx;
		eax = 0;
		info->Rdmsr(0xce, &eax, &edx);

		*ratio = ((eax >> 8) & 0xFF);
		return true;
	}
	return false;
}

Bool Manage::CPUInfo::GetCPUTurboRatio(Int32 *ratio)
{
	InfoData *info = (InfoData*)this->clsData;
	if (info->winRing0 == 0)
		return false;

	if (this->brand == Manage::CPUVendor::CB_INTEL)
	{
		UInt32 eax;
		UInt32 edx;
		eax = 0;
		info->Rdmsr(0x1ad, &eax, &edx);

		*ratio = (eax & 0xFF);
		return true;
	}
	return false;
}

Bool Manage::CPUInfo::GetCPUTCC(Double *temp)
{
	InfoData *info = (InfoData*)this->clsData;
	if (info->winRing0 == 0)
		return false;

	if (this->brand == Manage::CPUVendor::CB_INTEL && this->SupportIntelDTS())
	{
		UInt32 eax;
		UInt32 edx;
		eax = 0;
		info->Rdmsr(0x1a2, &eax, &edx);
		*temp = (eax >> 16) & 0xff;
		return true;
	}

	return false;
}
#else
UTF8Char *Manage::CPUInfo::GetCPUName(UTF8Char *sbuff)
{
	InfoData *info = (InfoData*)this->clsData;
	if (info->cpuName)
	{
		return Text::StrConcat(sbuff, info->cpuName);
	}
	return 0;
}

Bool Manage::CPUInfo::GetCPURatio(Int32 *ratio)
{
//	InfoData *info = (InfoData*)this->clsData;
	return false;
}

Bool Manage::CPUInfo::GetCPUTurboRatio(Int32 *ratio)
{
//	InfoData *info = (InfoData*)this->clsData;
	return false;
}

Bool Manage::CPUInfo::GetCPUTCC(Double *temp)
{
//	InfoData *info = (InfoData*)this->clsData;
	return false;
}
#endif

Text::CString Manage::CPUInfo::GetCacheInfo(Manage::CPUVendor::CPU_BRAND brand, UInt8 descType)
{
	if (brand == Manage::CPUVendor::CB_INTEL)
	{
		switch (descType)
		{
		case 0x01:
			return {UTF8STRC("Instruction TLB: 4 KByte pages, 4-way set associative, 32 entries")};
		case 0x02:
			return {UTF8STRC("Instruction TLB: 4 MByte pages, fully associative, 2 entries")};
		case 0x03:
			return {UTF8STRC("Data TLB: 4 KByte pages, 4-way set associative, 64 entries")};
		case 0x04:
			return {UTF8STRC("Data TLB: 4 MByte pages, 4-way set associative, 8 entries")};
		case 0x05:
			return {UTF8STRC("Data TLB1: 4 MByte pages, 4-way set associative, 32 entries")};
		case 0x06:
			return {UTF8STRC("1st-level instruction cache: 8 KBytes, 4-way set associative, 32 byte line size")};
		case 0x08:
			return {UTF8STRC("1st-level instruction cache: 16 KBytes, 4-way set associative, 32 byte line size")};
		case 0x09:
			return {UTF8STRC("1st-level instruction cache: 32KBytes, 4-way set associative, 64 byte line size")};
		case 0x0A:
			return {UTF8STRC("1st-level data cache: 8 KBytes, 2-way set associative, 32 byte line size")};
		case 0x0B:
			return {UTF8STRC("Instruction TLB: 4 MByte pages, 4-way set associative, 4 entries")};
		case 0x0C:
			return {UTF8STRC("1st-level data cache: 16 KBytes, 4-way set associative, 32 byte line size")};
		case 0x0D:
			return {UTF8STRC("1st-level data cache: 16 KBytes, 4-way set associative, 64 byte line size")};
		case 0x0E:
			return {UTF8STRC("1st-level data cache: 24 KBytes, 6-way set associative, 64 byte line size")};
		case 0x1D:
			return {UTF8STRC("2nd-level cache: 128 KBytes, 2-way set associative, 64 byte line size")};
		case 0x21:
			return {UTF8STRC("2nd-level cache: 256 KBytes, 8-way set associative, 64 byte line size")};
		case 0x22:
			return {UTF8STRC("3rd-level cache: 512 KBytes, 4-way set associative, 64 byte line size, 2 lines per sector")};
		case 0x23:
			return {UTF8STRC("3rd-level cache: 1 MBytes, 8-way set associative, 64 byte line size, 2 lines per sector")};
		case 0x24:
			return {UTF8STRC("2nd-level cache: 1 MBytes, 16-way set associative, 64 byte line size")};
		case 0x25:
			return {UTF8STRC("3rd-level cache: 2 MBytes, 8-way set associative, 64 byte line size, 2 lines per sector")};
		case 0x29:
			return {UTF8STRC("3rd-level cache: 4 MBytes, 8-way set associative, 64 byte line size, 2 lines per sector")};
		case 0x2C:
			return {UTF8STRC("1st-level data cache: 32 KBytes, 8-way set associative, 64 byte line size")};
		case 0x30:
			return {UTF8STRC("1st-level instruction cache: 32 KBytes, 8-way set associative, 64 byte line size")};
		case 0x40:
			return {UTF8STRC("No 2nd-level cache or, if processor contains a valid 2nd-level cache, no 3rd-level cache")};
		case 0x41:
			return {UTF8STRC("2nd-level cache: 128 KBytes, 4-way set associative, 32 byte line size")};
		case 0x42:
			return {UTF8STRC("2nd-level cache: 256 KBytes, 4-way set associative, 32 byte line size")};
		case 0x43:
			return {UTF8STRC("2nd-level cache: 512 KBytes, 4-way set associative, 32 byte line size")};
		case 0x44:
			return {UTF8STRC("2nd-level cache: 1 MByte, 4-way set associative, 32 byte line size")};
		case 0x45:
			return {UTF8STRC("2nd-level cache: 2 MByte, 4-way set associative, 32 byte line size")};
		case 0x46:
			return {UTF8STRC("3rd-level cache: 4 MByte, 4-way set associative, 64 byte line size")};
		case 0x47:
			return {UTF8STRC("3rd-level cache: 8 MByte, 8-way set associative, 64 byte line size")};
		case 0x48:
			return {UTF8STRC("2nd-level cache: 3MByte, 12-way set associative, 64 byte line size")};
		case 0x49:
			return {UTF8STRC("2nd-level cache: 4 MByte, 16-way set associative, 64 byte line size")}; ////3rd-level cache: 4MB, 16-way set associative, 64-byte line size (Intel Xeon processor MP, Family 0FH, Model 06H)
		case 0x4A:
			return {UTF8STRC("3rd-level cache: 6MByte, 12-way set associative, 64 byte line size")};
		case 0x4B:
			return {UTF8STRC("3rd-level cache: 8MByte, 16-way set associative, 64 byte line size")};
		case 0x4C:
			return {UTF8STRC("3rd-level cache: 12MByte, 12-way set associative, 64 byte line size")};
		case 0x4D:
			return {UTF8STRC("3rd-level cache: 16MByte, 16-way set associative, 64 byte line size")};
		case 0x4E:
			return {UTF8STRC("2nd-level cache: 6MByte, 24-way set associative, 64 byte line size")};
		case 0x4F:
			return {UTF8STRC("Instruction TLB: 4 KByte pages, 32 entries")};
		case 0x50:
			return {UTF8STRC("Instruction TLB: 4 KByte and 2-MByte or 4-MByte pages, 64 entries")};
		case 0x51:
			return {UTF8STRC("Instruction TLB: 4 KByte and 2-MByte or 4-MByte pages, 128 entries")};
		case 0x52:
			return {UTF8STRC("Instruction TLB: 4 KByte and 2-MByte or 4-MByte pages, 256 entries")};
		case 0x55:
			return {UTF8STRC("Instruction TLB: 2-MByte or 4-MByte pages, fully associative, 7 entries")};
		case 0x56:
			return {UTF8STRC("Data TLB0: 4 MByte pages, 4-way set associative, 16 entries")};
		case 0x57:
			return {UTF8STRC("Data TLB0: 4 KByte pages, 4-way associative, 16 entries")};
		case 0x59:
			return {UTF8STRC("Data TLB0: 4 KByte pages, fully associative, 16 entries")};
		case 0x5A:
			return {UTF8STRC("Data TLB0: 2-MByte or 4 MByte pages, 4-way set associative, 32 entries")};
		case 0x5B:
			return {UTF8STRC("Data TLB: 4 KByte and 4 MByte pages, 64 entries")};
		case 0x5C:
			return {UTF8STRC("Data TLB: 4 KByte and 4 MByte pages,128 entries")};
		case 0x5D:
			return {UTF8STRC("Data TLB: 4 KByte and 4 MByte pages,256 entries")};
		case 0x60:
			return {UTF8STRC("1st-level data cache: 16 KByte, 8-way set associative, 64 byte line size")};
		case 0x61:
			return {UTF8STRC("Instruction TLB: 4 KByte pages, fully associative, 48 entries")};
		case 0x63:
			return {UTF8STRC("Data TLB: 1 GByte pages, 4-way set associative, 4 entries")};
		case 0x66:
			return {UTF8STRC("1st-level data cache: 8 KByte, 4-way set associative, 64 byte line size")};
		case 0x67:
			return {UTF8STRC("1st-level data cache: 16 KByte, 4-way set associative, 64 byte line size")};
		case 0x68:
			return {UTF8STRC("1st-level data cache: 32 KByte, 4-way set associative, 64 byte line size")};
		case 0x70:
			return {UTF8STRC("Trace cache: 12 K-μop, 8-way set associative")};
		case 0x71:
			return {UTF8STRC("Trace cache: 16 K-μop, 8-way set associative")};
		case 0x72:
			return {UTF8STRC("Trace cache: 32 K-μop, 8-way set associative")};
		case 0x76:
			return {UTF8STRC("Instruction TLB: 2M/4M pages, fully associative, 8 entries")};
		case 0x78:
			return {UTF8STRC("2nd-level cache: 1 MByte, 4-way set associative, 64byte line size")};
		case 0x79:
			return {UTF8STRC("2nd-level cache: 128 KByte, 8-way set associative, 64 byte line size, 2 lines per sector")};
		case 0x7A:
			return {UTF8STRC("2nd-level cache: 256 KByte, 8-way set associative, 64 byte line size, 2 lines per sector")};
		case 0x7B:
			return {UTF8STRC("2nd-level cache: 512 KByte, 8-way set associative, 64 byte line size, 2 lines per sector")};
		case 0x7C:
			return {UTF8STRC("2nd-level cache: 1 MByte, 8-way set associative, 64 byte line size, 2 lines per sector")};
		case 0x7D:
			return {UTF8STRC("2nd-level cache: 2 MByte, 8-way set associative, 64byte line size")};
		case 0x7F:
			return {UTF8STRC("2nd-level cache: 512 KByte, 2-way set associative, 64-byte line size")};
		case 0x80:
			return {UTF8STRC("2nd-level cache: 512 KByte, 8-way set associative, 64-byte line size")};
		case 0x82:
			return {UTF8STRC("2nd-level cache: 256 KByte, 8-way set associative, 32 byte line size")};
		case 0x83:
			return {UTF8STRC("2nd-level cache: 512 KByte, 8-way set associative, 32 byte line size")};
		case 0x84:
			return {UTF8STRC("2nd-level cache: 1 MByte, 8-way set associative, 32 byte line size")};
		case 0x85:
			return {UTF8STRC("2nd-level cache: 2 MByte, 8-way set associative, 32 byte line size")};
		case 0x86:
			return {UTF8STRC("2nd-level cache: 512 KByte, 4-way set associative, 64 byte line size")};
		case 0x87:
			return {UTF8STRC("2nd-level cache: 1 MByte, 8-way set associative, 64 byte line size")};
		case 0xA0:
			return {UTF8STRC("DTLB: 4k pages, fully associative, 32 entries")};
		case 0xB0:
			return {UTF8STRC("Instruction TLB: 4 KByte pages, 4-way set associative, 128 entries")};
		case 0xB1:
			return {UTF8STRC("Instruction TLB: 2M pages, 4-way, 8 entries or 4M pages, 4-way, 4 entries")};
		case 0xB2:
			return {UTF8STRC("Instruction TLB: 4KByte pages, 4-way set associative, 64 entries")};
		case 0xB3:
			return {UTF8STRC("Data TLB: 4 KByte pages, 4-way set associative, 128 entries")};
		case 0xB4:
			return {UTF8STRC("Data TLB1: 4 KByte pages, 4-way associative, 256 entries")};
		case 0xB5:
			return {UTF8STRC("Instruction TLB: 4KByte pages, 8-way set associative, 64 entries")};
		case 0xB6:
			return {UTF8STRC("Instruction TLB: 4KByte pages, 8-way set associative, 128 entries")};
		case 0xBA:
			return {UTF8STRC("Data TLB1: 4 KByte pages, 4-way associative, 64 entries")};
		case 0xC0:
			return {UTF8STRC("Data TLB: 4 KByte and 4 MByte pages, 4-way associative, 8 entries")};
		case 0xC1:
			return {UTF8STRC("Shared 2nd-Level TLB: 4 KByte/2MByte pages, 8-way associative, 1024 entries")};
		case 0xC2:
			return {UTF8STRC("DTLB: 4 KByte/2 MByte pages, 4-way associative, 16 entries")};
		case 0xC3:
			return {UTF8STRC("Shared 2nd-Level TLB: 4 KByte /2 MByte pages, 6-way associative, 1536 entries. Also 1GBbyte pages, 4-way, 16 entries.")};
		case 0xCA:
			return {UTF8STRC("Shared 2nd-Level TLB: 4 KByte pages, 4-way associative, 512 entries")};
		case 0xD0:
			return {UTF8STRC("3rd-level cache: 512 KByte, 4-way set associative, 64 byte line size")};
		case 0xD1:
			return {UTF8STRC("3rd-level cache: 1 MByte, 4-way set associative, 64 byte line size")};
		case 0xD2:
			return {UTF8STRC("3rd-level cache: 2 MByte, 4-way set associative, 64 byte line size")};
		case 0xD6:
			return {UTF8STRC("3rd-level cache: 1 MByte, 8-way set associative, 64 byte line size")};
		case 0xD7:
			return {UTF8STRC("3rd-level cache: 2 MByte, 8-way set associative, 64 byte line size")};
		case 0xD8:
			return {UTF8STRC("3rd-level cache: 4 MByte, 8-way set associative, 64 byte line size")};
		case 0xDC:
			return {UTF8STRC("3rd-level cache: 1.5 MByte, 12-way set associative, 64 byte line size")};
		case 0xDD:
			return {UTF8STRC("3rd-level cache: 3 MByte, 12-way set associative, 64 byte line size")};
		case 0xDE:
			return {UTF8STRC("3rd-level cache: 6 MByte, 12-way set associative, 64 byte line size")};
		case 0xE2:
			return {UTF8STRC("3rd-level cache: 2 MByte, 16-way set associative, 64 byte line size")};
		case 0xE3:
			return {UTF8STRC("3rd-level cache: 4 MByte, 16-way set associative, 64 byte line size")};
		case 0xE4:
			return {UTF8STRC("3rd-level cache: 8 MByte, 16-way set associative, 64 byte line size")};
		case 0xEA:
			return {UTF8STRC("3rd-level cache: 12MByte, 24-way set associative, 64 byte line size")};
		case 0xEB:
			return {UTF8STRC("3rd-level cache: 18MByte, 24-way set associative, 64 byte line size")};
		case 0xEC:
			return {UTF8STRC("3rd-level cache: 24MByte, 24-way set associative, 64 byte line size")};
		case 0xF0:
			return {UTF8STRC("64-Byte prefetching")};
		case 0xF1:
			return {UTF8STRC("128-Byte prefetching")};
		}
	}
	return {0, 0};
}

const UTF8Char *Manage::CPUInfo::GetFeatureShortName(UOSInt index)
{
	const UTF8Char *shortNames[64] = {
		(const UTF8Char*)"FPU",
		(const UTF8Char*)"VME",
		(const UTF8Char*)"DE",
		(const UTF8Char*)"PSE",
		(const UTF8Char*)"TSC",
		(const UTF8Char*)"MSR",
		(const UTF8Char*)"PAE",
		(const UTF8Char*)"MCE",

		(const UTF8Char*)"CX8",
		(const UTF8Char*)"APIC",
		(const UTF8Char*)"Reserved",
		(const UTF8Char*)"SEP",
		(const UTF8Char*)"MTRR",
		(const UTF8Char*)"PGE",
		(const UTF8Char*)"MCA",
		(const UTF8Char*)"CMOV",

		(const UTF8Char*)"PAT",
		(const UTF8Char*)"PSE-36",
		(const UTF8Char*)"PSN",
		(const UTF8Char*)"CLFSH",
		(const UTF8Char*)"Reserved",
		(const UTF8Char*)"DS",
		(const UTF8Char*)"ACPI",
		(const UTF8Char*)"MMX",

		(const UTF8Char*)"FXSR",
		(const UTF8Char*)"SSE",
		(const UTF8Char*)"SSE2",
		(const UTF8Char*)"SS",
		(const UTF8Char*)"HTT",
		(const UTF8Char*)"TM",
		(const UTF8Char*)"Reserved",
		(const UTF8Char*)"PBE",

		(const UTF8Char*)"SSE3",
		(const UTF8Char*)"PCLMULQDQ",
		(const UTF8Char*)"DTES64",
		(const UTF8Char*)"MONITOR",
		(const UTF8Char*)"DS-CPL",
		(const UTF8Char*)"VMX",
		(const UTF8Char*)"SMX",
		(const UTF8Char*)"EIST",

		(const UTF8Char*)"TM2",
		(const UTF8Char*)"SSSE3",
		(const UTF8Char*)"CNXT-ID",
		(const UTF8Char*)"SDBG",
		(const UTF8Char*)"FMA",
		(const UTF8Char*)"CMPXCHG16B",
		(const UTF8Char*)"xTPR Update Control",
		(const UTF8Char*)"PDCM",

		(const UTF8Char*)"Reserved",
		(const UTF8Char*)"PCID",
		(const UTF8Char*)"DCA",
		(const UTF8Char*)"SSE4.1",
		(const UTF8Char*)"SSE4.2",
		(const UTF8Char*)"x2APIC",
		(const UTF8Char*)"MOVBE",
		(const UTF8Char*)"POPCNT",

		(const UTF8Char*)"TSC-Deadline",
		(const UTF8Char*)"AESNI",
		(const UTF8Char*)"XSAVE",
		(const UTF8Char*)"OSXSAVE",
		(const UTF8Char*)"AVX",
		(const UTF8Char*)"F16C",
		(const UTF8Char*)"RDRAND",
		(const UTF8Char*)"Not Used"
	};
	if (index >= 64)
		return 0;
	return shortNames[index];
}

const UTF8Char *Manage::CPUInfo::GetFeatureName(UOSInt index)
{
	const UTF8Char *names[64] = {
		(const UTF8Char*)"Floating Point Unit On-Chip",
		(const UTF8Char*)"Virtual 8086 Mode Enhancements",
		(const UTF8Char*)"Debugging Extensions",
		(const UTF8Char*)"Page Size Extension",
		(const UTF8Char*)"Time Stamp Counter",
		(const UTF8Char*)"Model Specific Registers RDMSR and WRMSR Instructions",
		(const UTF8Char*)"Physical Address Extension",
		(const UTF8Char*)"Machine Check Exception",

		(const UTF8Char*)"CMPXCHG8B Instruction",
		(const UTF8Char*)"APIC On-Chip",
		(const UTF8Char*)"Reserved",
		(const UTF8Char*)"SYSENTER and SYSEXIT Instructions",
		(const UTF8Char*)"Memory Type Range Registers",
		(const UTF8Char*)"Page Global Bit",
		(const UTF8Char*)"Machine Check Architecture",
		(const UTF8Char*)"Conditional Move Instructions",

		(const UTF8Char*)"Page Attribute Table",
		(const UTF8Char*)"36-Bit Page Size Extension",
		(const UTF8Char*)"Processor Serial Number",
		(const UTF8Char*)"CLFLUSH Instruction",
		(const UTF8Char*)"Reserved",
		(const UTF8Char*)"Debug Store",
		(const UTF8Char*)"Thermal Monitor and Software Controlled Clock Facilities",
		(const UTF8Char*)"Intel MMX Technology",

		(const UTF8Char*)"FXSAVE and FXRSTOR Instructions",
		(const UTF8Char*)"SSE",
		(const UTF8Char*)"SSE2",
		(const UTF8Char*)"Self Snoop",
		(const UTF8Char*)"Max APIC IDs reserved field is Valid",
		(const UTF8Char*)"Thermal Monitor",
		(const UTF8Char*)"Reserved",
		(const UTF8Char*)"Pending Break Enable",

		(const UTF8Char*)"Streaming SIMD Extensions 3 (SSE3)",
		(const UTF8Char*)"PCLMULQDQ",
		(const UTF8Char*)"64-bit DS Area",
		(const UTF8Char*)"MONITOR/MWAIT",
		(const UTF8Char*)"CPL Qualified Debug Store",
		(const UTF8Char*)"Virtual Machine Extensions",
		(const UTF8Char*)"Safer Mode Extensions",
		(const UTF8Char*)"Enhanced Intel SpeedStepR technology",

		(const UTF8Char*)"Thermal Monitor 2",
		(const UTF8Char*)"Supplemental Streaming SIMD Extensions 3 (SSSE3)",
		(const UTF8Char*)"L1 Context ID",
		(const UTF8Char*)"SDBG",
		(const UTF8Char*)"FMA",
		(const UTF8Char*)"CMPXCHG16B Available",
		(const UTF8Char*)"xTPR Update Control",
		(const UTF8Char*)"Perfmon and Debug Capability",

		(const UTF8Char*)"Reserved",
		(const UTF8Char*)"Process-context identifiers",
		(const UTF8Char*)"DCA",
		(const UTF8Char*)"SSE4.1",
		(const UTF8Char*)"SSE4.2",
		(const UTF8Char*)"x2APIC",
		(const UTF8Char*)"MOVBE",
		(const UTF8Char*)"POPCNT",

		(const UTF8Char*)"TSC-Deadline",
		(const UTF8Char*)"AESNI",
		(const UTF8Char*)"XSAVE",
		(const UTF8Char*)"OSXSAVE",
		(const UTF8Char*)"AVX",
		(const UTF8Char*)"F16C",
		(const UTF8Char*)"RDRAND",
		(const UTF8Char*)"Not Used"
	};
	if (index >= 64)
		return 0;
	return names[index];
}

const UTF8Char *Manage::CPUInfo::GetFeatureDesc(UOSInt index)
{
	const UTF8Char *desc[64] = {
		(const UTF8Char*)"The processor contains an x87 FPU",
		(const UTF8Char*)"Virtual 8086 mode enhancements, including CR4.VME for controlling the feature, CR4.PVI for protected mode virtual interrupts, software interrupt indirection, expansion of the TSS with the software indirection bitmap, and EFLAGS.VIF and EFLAGS.VIP flags",
		(const UTF8Char*)"Support for I/O breakpoints, including CR4.DE for controlling the feature, and optional trapping of accesses to DR4 and DR5",
		(const UTF8Char*)"Large pages of size 4 MByte are supported, including CR4.PSE for controlling the feature, the defined dirty bit in PDE (Page Directory Entries), optional reserved bit trapping in CR3, PDEs, and PTEs",
		(const UTF8Char*)"The RDTSC instruction is supported, including CR4.TSD for controlling privilege",
		(const UTF8Char*)"The RDMSR and WRMSR instructions are supported. Some of the MSRs are implementation dependent",
		(const UTF8Char*)"Physical addresses greater than 32 bits are supported: extended page table entry formats, an extra level in the page translation tables is defined, 2-MByte pages are supported instead of 4 Mbyte pages if PAE bit is 1",
		(const UTF8Char*)"Exception 18 is defined for Machine Checks, including CR4.MCE for controlling the feature. This feature does not define the model-specific implementations of machine-check error logging, reporting, and processor shutdowns. Machine Check exception handlers may have to depend on processor version to do model specific processing of the exception, or test for the presence of the Machine Check feature",

		(const UTF8Char*)"The compare-and-exchange 8 bytes (64 bits) instruction is supported (implicitly locked and atomic)",
		(const UTF8Char*)"The processor contains an Advanced Programmable Interrupt Controller (APIC), responding to memory mapped commands in the physical address range FFFE0000H to FFFE0FFFH (by default - some processors permit the APIC to be relocated)",
		(const UTF8Char*)"Reserved",
		(const UTF8Char*)"The SYSENTER and SYSEXIT and associated MSRs are supported",
		(const UTF8Char*)"MTRRs are supported. The MTRRcap MSR contains feature bits that describe what memory types are supported, how many variable MTRRs are supported, and whether fixed MTRRs are supported",
		(const UTF8Char*)"The global bit is supported in paging-structure entries that map a page, indicating TLB entries that are common to different processes and need not be flushed. The CR4.PGE bit controls this feature",
		(const UTF8Char*)"The Machine Check Architecture, which provides a compatible mechanism for error reporting in P6 family, Pentium 4, Intel Xeon processors, and future processors, is supported. The MCG_CAP MSR contains feature bits describing how many banks of error reporting MSRs are supported",
		(const UTF8Char*)"The conditional move instruction CMOV is supported. In addition, if x87 FPU is present as indicated by the CPUID.FPU feature bit, then the FCOMI and FCMOV instructions are supported",

		(const UTF8Char*)"Page Attribute Table is supported. This feature augments the Memory Type Range Registers (MTRRs), allowing an operating system to specify attributes of memory accessed through a linear address on a 4KB granularity",
		(const UTF8Char*)"4-MByte pages addressing physical memory beyond 4 GBytes are supported with 32-bit paging. This feature indicates that upper bits of the physical address of a 4-MByte page are encoded in bits 20:13 of the page-directory entry. Such physical addresses are limited by MAXPHYADDR and may be up to 40 bits in size.",
		(const UTF8Char*)"The processor supports the 96-bit processor identification number feature and the feature is enabled",
		(const UTF8Char*)"CLFLUSH Instruction is supported",
		(const UTF8Char*)"Reserved",
		(const UTF8Char*)"The processor supports the ability to write debug information into a memory resident buffer. This feature is used by the branch trace store (BTS) and precise event-based sampling (PEBS) facilities",
		(const UTF8Char*)"The processor implements internal MSRs that allow processor temperature to be monitored and processor performance to be modulated in predefined duty cycles under software control",
		(const UTF8Char*)"The processor supports the Intel MMX technology",

		(const UTF8Char*)"The FXSAVE and FXRSTOR instructions are supported for fast save and restore of the floating point context. Presence of this bit also indicates that CR4.OSFXSR is available for an operating system to indicate that it supports the FXSAVE and FXRSTOR instructions",
		(const UTF8Char*)"The processor supports the SSE extensions",
		(const UTF8Char*)"The processor supports the SSE2 extensions",
		(const UTF8Char*)"The processor supports the management of conflicting memory types by performing a snoop of its own cache structure for transactions issued to the bus",
		(const UTF8Char*)"A value of 0 for HTT indicates there is only a single logical processor in the package and software should assume only a single APIC ID is reserved. A value of 1 for HTT indicates the value in CPUID.1.EBX[23:16] (the Maximum number of addressable IDs for logical processors in this package) is valid for the package",
		(const UTF8Char*)"The processor implements the thermal monitor automatic thermal control circuitry (TCC)",
		(const UTF8Char*)"Reserved",
		(const UTF8Char*)"The processor supports the use of the FERR#/PBE# pin when the processor is in the stop-clock state (STPCLK# is asserted) to signal the processor that an interrupt is pending and that the processor should return to normal operation to handle the interrupt. Bit 10 (PBE enable) in the IA32_MISC_ENABLE MSR enables this capability",

		(const UTF8Char*)"A value of 1 indicates the processor supports this technology",
		(const UTF8Char*)"A value of 1 indicates the processor supports the PCLMULQDQ instruction",
		(const UTF8Char*)"A value of 1 indicates the processor supports DS area using 64-bit layout",
		(const UTF8Char*)"A value of 1 indicates the processor supports this feature",
		(const UTF8Char*)"A value of 1 indicates the processor supports the extensions to the Debug Store feature to allow for branch message storage qualified by CPL",
		(const UTF8Char*)"A value of 1 indicates that the processor supports this technology",
		(const UTF8Char*)"A value of 1 indicates that the processor supports this technology",
		(const UTF8Char*)"A value of 1 indicates that the processor supports this technology",

		(const UTF8Char*)"A value of 1 indicates whether the processor supports this technology",
		(const UTF8Char*)"A value of 1 indicates the presence of the Supplemental Streaming SIMD Extensions 3 (SSSE3). A value of 0 indicates the instruction extensions are not present in the processor",
		(const UTF8Char*)"A value of 1 indicates the L1 data cache mode can be set to either adaptive mode or shared mode. A value of 0 indicates this feature is not supported. See definition of the IA32_MISC_ENABLE MSR Bit 24 (L1 Data Cache Context Mode) for details",
		(const UTF8Char*)"A value of 1 indicates the processor supports IA32_DEBUG_INTERFACE MSR for silicon debug",
		(const UTF8Char*)"A value of 1 indicates the processor supports FMA extensions using YMM state",
		(const UTF8Char*)"A value of 1 indicates that the feature is available",
		(const UTF8Char*)"A value of 1 indicates that the processor supports changing IA32_MISC_ENABLE[bit 23]",
		(const UTF8Char*)"A value of 1 indicates the processor supports the performance and debug feature indication MSR IA32_PERF_CAPABILITIES",

		(const UTF8Char*)"Reserved",
		(const UTF8Char*)"A value of 1 indicates that the processor supports PCIDs and that software may set CR4.PCIDE to 1",
		(const UTF8Char*)"A value of 1 indicates the processor supports the ability to prefetch data from a memory mapped device",
		(const UTF8Char*)"A value of 1 indicates that the processor supports SSE4.1",
		(const UTF8Char*)"A value of 1 indicates that the processor supports SSE4.2",
		(const UTF8Char*)"A value of 1 indicates that the processor supports x2APIC feature",
		(const UTF8Char*)"A value of 1 indicates that the processor supports MOVBE instruction",
		(const UTF8Char*)"A value of 1 indicates that the processor supports the POPCNT instruction",

		(const UTF8Char*)"A value of 1 indicates that the processor's local APIC timer supports one-shot operation using a TSC deadline value",
		(const UTF8Char*)"A value of 1 indicates that the processor supports the AESNI instruction extensions",
		(const UTF8Char*)"A value of 1 indicates that the processor supports the XSAVE/XRSTOR processor extended states feature, the XSETBV/XGETBV instructions, and XCR0",
		(const UTF8Char*)"A value of 1 indicates that the OS has set CR4.OSXSAVE[bit 18] to enable XSETBV/XGETBV instructions to access XCR0 and to support processor extended state management using XSAVE/XRSTOR",
		(const UTF8Char*)"A value of 1 indicates the processor supports the AVX instruction extensions",
		(const UTF8Char*)"A value of 1 indicates that processor supports 16-bit floating-point conversion instructions",
		(const UTF8Char*)"A value of 1 indicates that processor supports RDRAND instruction",
		(const UTF8Char*)"Always returns 0"
	};
	if (index >= 64)
		return 0;
	return desc[index];
}

void Manage::CPUInfo::AppendNameInfo10(UInt32 ecxv, UInt32 edxv, Text::StringBuilderUTF8 *sb)
{
	if (edxv & 1)
		sb->AppendC(UTF8STRC("FPU "));
	if (edxv & 0x10)
		sb->AppendC(UTF8STRC("TSC "));
	if (edxv & 0x20)
		sb->AppendC(UTF8STRC("MSR "));
	if (edxv & 0x100)
		sb->AppendC(UTF8STRC("CMPXCHG8B "));
	if (edxv & 0x800)
		sb->AppendC(UTF8STRC("SysEnterSysExit "));
	if (edxv & 0x8000)
		sb->AppendC(UTF8STRC("CMOV "));
	if (edxv & 0x80000)
		sb->AppendC(UTF8STRC("CFLSH "));
	if (edxv & 0x800000)
		sb->AppendC(UTF8STRC("MMX "));
	if (edxv & 0x1000000)
		sb->AppendC(UTF8STRC("FXSR "));
	if (edxv & 0x2000000)
		sb->AppendC(UTF8STRC("SSE "));
	if (edxv & 0x4000000)
		sb->AppendC(UTF8STRC("SSE2 "));
	if (ecxv & 0x1)
		sb->AppendC(UTF8STRC("SSE3 "));
	if (ecxv & 0x2)
		sb->AppendC(UTF8STRC("PCLMULQDQ "));
	if (ecxv & 0x8)
		sb->AppendC(UTF8STRC("MONITOR "));
	if (ecxv & 0x200)
		sb->AppendC(UTF8STRC("SSSE3 "));
	if (ecxv & 0x1000)
		sb->AppendC(UTF8STRC("FMA "));
	if (ecxv & 0x2000)
		sb->AppendC(UTF8STRC("CMPXCHG16B "));
	if (ecxv & 0x80000)
		sb->AppendC(UTF8STRC("SSE4.1 "));
	if (ecxv & 0x100000)
		sb->AppendC(UTF8STRC("SSE4.2 "));
	if (ecxv & 0x400000)
		sb->AppendC(UTF8STRC("MOVBE "));
	if (ecxv & 0x800000)
		sb->AppendC(UTF8STRC("POPCNT "));
	if (ecxv & 0x2000000)
		sb->AppendC(UTF8STRC("AESNI "));
	if (ecxv & 0x4000000)
		sb->AppendC(UTF8STRC("XSAVE "));
	if (ecxv & 0x8000000)
		sb->AppendC(UTF8STRC("OSXSAVE "));
	if (ecxv & 0x10000000)
		sb->AppendC(UTF8STRC("AVX "));
	if (ecxv & 0x20000000)
		sb->AppendC(UTF8STRC("F16C "));
	if (ecxv & 0x40000000)
		sb->AppendC(UTF8STRC("RDRAND "));
}

void Manage::CPUInfo::AppendNameInfo11(UInt32 ecxv, UInt32 edxv, Text::StringBuilderUTF8 *sb)
{
	if (edxv & 2)
		sb->AppendC(UTF8STRC("VME "));
	if (edxv & 4)
		sb->AppendC(UTF8STRC("DE "));
	if (edxv & 8)
		sb->AppendC(UTF8STRC("PSE "));
	if (edxv & 0x40)
		sb->AppendC(UTF8STRC("PAE "));
	if (edxv & 0x80)
		sb->AppendC(UTF8STRC("MCE "));
	if (edxv & 0x200)
		sb->AppendC(UTF8STRC("APIC "));
	if (edxv & 0x1000)
		sb->AppendC(UTF8STRC("MTRR "));
	if (edxv & 0x2000)
		sb->AppendC(UTF8STRC("PGE "));
	if (edxv & 0x4000)
		sb->AppendC(UTF8STRC("MCA "));
	if (edxv & 0x10000)
		sb->AppendC(UTF8STRC("PAT "));
	if (edxv & 0x20000)
		sb->AppendC(UTF8STRC("PSE36 "));
	if (edxv & 0x40000)
		sb->AppendC(UTF8STRC("PSN "));
	if (edxv & 0x200000)
		sb->AppendC(UTF8STRC("DS "));
	if (edxv & 0x400000)
		sb->AppendC(UTF8STRC("ACPI "));
	if (edxv & 0x8000000)
		sb->AppendC(UTF8STRC("SS "));
	if (edxv & 0x10000000)
		sb->AppendC(UTF8STRC("HTT "));
	if (edxv & 0x20000000)
		sb->AppendC(UTF8STRC("TM "));
	if (edxv & 0x80000000)
		sb->AppendC(UTF8STRC("PBE "));
	if (ecxv & 0x4)
		sb->AppendC(UTF8STRC("DTES64 "));
	if (ecxv & 0x10)
		sb->AppendC(UTF8STRC("DS-CPL "));
	if (ecxv & 0x20)
		sb->AppendC(UTF8STRC("VMX "));
	if (ecxv & 0x40)
		sb->AppendC(UTF8STRC("SMX "));
	if (ecxv & 0x80)
		sb->AppendC(UTF8STRC("EST "));
	if (ecxv & 0x100)
		sb->AppendC(UTF8STRC("TM2 "));
	if (ecxv & 0x400)
		sb->AppendC(UTF8STRC("CNXT-ID "));
	if (ecxv & 0x800)
		sb->AppendC(UTF8STRC("SDBG "));
	if (ecxv & 0x4000)
		sb->AppendC(UTF8STRC("xTPR_Update "));
	if (ecxv & 0x8000)
		sb->AppendC(UTF8STRC("PDCM "));
	if (ecxv & 0x20000)
		sb->AppendC(UTF8STRC("PCID "));
	if (ecxv & 0x40000)
		sb->AppendC(UTF8STRC("DCA "));
	if (ecxv & 0x200000)
		sb->AppendC(UTF8STRC("x2APIC "));
	if (ecxv & 0x1000000)
		sb->AppendC(UTF8STRC("TSC-Deadline "));
	if (ecxv & 0x80000000)
		sb->AppendC(UTF8STRC("RAZ "));
}
