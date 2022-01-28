#ifndef _SM_MANAGE_CPUDB
#define _SM_MANAGE_CPUDB
#include "IO/Stream.h"
#include "Manage/CPUVendor.h"
#include "Manage/ThreadContext.h"
#include "Text/CString.h"

namespace Manage
{
	class CPUDB
	{
	public:
		typedef struct
		{
			Int32 familyId;
			Int32 modelId;
			Int32 steppingId;
			Int32 tcc;
			Int32 spdDef;
			Int32 spdOC;
			Int32 nCore;
			Int32 nThread;
			Double tdp;
			Int32 nm;
			Int32 launchYear;
			Int32 launchQuarter;
			Text::CString brandName;
			Text::CString partNum;
		} CPUSpecX86;

		typedef struct
		{
			Text::CString model;
			Manage::ThreadContext::ContextType contextType;
			Manage::CPUVendor::CPU_BRAND brand;
			const Char *name;
			Double tdp;
			Int32 nm;
			const Char *core1Type;
			Int32 core1Count;
			Int32 core1BaseFreqMHz;
			Int32 core1MaxFreqMHz;
			Int32 core1L1ICacheKB;
			Int32 core1L1DCacheKB;
			const Char *core2Type;
			Int32 core2Count;
			Int32 core2BaseFreqMHz;
			Int32 core2MaxFreqMHz;
			Int32 core2L1ICacheKB;
			Int32 core2L1DCacheKB;
			const Char *core3Type;
			Int32 core3Count;
			Int32 core3BaseFreqMHz;
			Int32 core3MaxFreqMHz;
			Int32 core3L1ICacheKB;
			Int32 core3L1DCacheKB;
			Int32 l2CacheKB;
			Int32 l3CacheKB;
			const Char *gpu;
		} CPUSpec;

		typedef struct
		{
			Text::CString dispName;
			Text::CString model;
		} CPUMapping;

	private:
		static CPUSpecX86 intelCPU[];
		static CPUSpecX86 amdCPU[];
		static CPUSpec cpuList[];
		static CPUMapping cpuX86Map[];

	public:
		static CPUSpecX86 *GetCPUSpecX86(Manage::CPUVendor::CPU_BRAND brand, Int32 familyId, Int32 modelId, Int32 steppingId);
		static const CPUSpec *GetCPUSpec(Text::CString model);
		static Text::CString X86CPUNameToModel(Text::CString x86CPUName);
		static Text::CString ParseCPUInfo(IO::Stream *stm); // return model
	};
}

#endif
