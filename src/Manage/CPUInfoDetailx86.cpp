#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/FileStream.h"
#include "IO/Library.h"
#include "Manage/CPUDB.h"
#include "Manage/CPUInfoDetail.h"
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

Manage::CPUInfoDetail::CPUInfoDetail()
{
	UTF8Char u8buff[256];
	UTF8Char *sptr;
	this->cpuModel = CSTR_NULL;
	if ((sptr = this->GetCPUName(u8buff)) != 0)
	{
		this->cpuModel = Manage::CPUDB::X86CPUNameToModel({u8buff, (UOSInt)(sptr - u8buff)});
	}
	if (this->cpuModel.v == 0)
	{
		Manage::CPUDB::CPUSpecX86 *cpuSpec = Manage::CPUDB::GetCPUSpecX86(this->brand, this->familyId, this->model, this->steppingId);
		if (cpuSpec)
		{
			this->cpuModel = cpuSpec->partNum;
		}
	}
}


Manage::CPUInfoDetail::~CPUInfoDetail()
{
}

Text::CString Manage::CPUInfoDetail::GetCPUModel()
{
	return this->cpuModel;
}

Int32 Manage::CPUInfoDetail::GetTCC()
{
	Manage::CPUDB::CPUSpecX86 *cpu = Manage::CPUDB::GetCPUSpecX86(this->brand, this->familyId, this->model, this->steppingId);
	if (cpu)
	{
		return cpu->tcc;
	}
	else
	{
		return 0;
	}
}

#if defined(WIN32) || defined(_WIN64)
Bool Manage::CPUInfoDetail::GetCPUTemp(UOSInt index, Double *temp)
{
	InfoData *info = (InfoData*)this->clsData;
	if (info->winRing0 == 0)
		return false;

	if (this->brand == Manage::CPUVendor::CB_INTEL && this->SupportIntelDTS())
	{
		if (index == 0)
		{
			UInt32 eax;
			UInt32 edx;
			Double tcc = this->GetTCC();
			if (tcc == 0)
			{
				eax = 0;
				info->Rdmsr(0x1a2, &eax, &edx);
				tcc = (eax >> 16) & 0xff;
			}
			eax = 0;
			info->Rdmsr(0x19c, &eax, &edx);
			if (eax == 0)
			{
				return false;
			}
			else
			{
				Int32 t = (eax & 0x7f0000) >> 16;
				*temp = tcc - t;
				return true;
			}
		}
		return false;
	}
	return false;
}
#else

Bool Manage::CPUInfoDetail::GetCPUTemp(UOSInt index, Double *temp)
{
//	InfoData *info = (InfoData*)this->clsData;
	const UTF8Char *path = 0;
	switch (index)
	{
	case 0:
		path = (const UTF8Char*)"/sys/class/hwmon/hwmon0/temp1_input";
		break;
	case 1:
		path = (const UTF8Char*)"/sys/class/hwmon/hwmon1/temp2_input";
		break;
	}
	if (path == 0)
		return false;
	Bool succ = false;
	Int32 val;
	Text::UTF8Reader *reader;
	IO::FileStream *fs;
	NEW_CLASS(fs, IO::FileStream(path, IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal));
	if (!fs->IsError())
	{
		Text::StringBuilderUTF8 sb;
		NEW_CLASS(reader, Text::UTF8Reader(fs));
		if (reader->ReadLine(&sb, 512))
		{
			if (sb.ToInt32(&val))
			{
				succ = true;
				*temp = val * 0.001;
			}
		}
		DEL_CLASS(reader);
	}
	DEL_CLASS(fs);
	return succ;
}
#endif