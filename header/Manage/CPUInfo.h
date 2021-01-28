#ifndef _SM_MANAGE_CPUINFO
#define _SM_MANAGE_CPUINFO
#include "Data/ArrayList.h"
#include "Manage/CPUVendor.h"
#include "Text/StringBuilderUTF.h"

namespace Manage
{
	class CPUInfo
	{
	public:
		typedef enum
		{
			IT_X86,
			IT_SSE41,
			IT_AVX,
			IT_AVX2
		} InstructionType;
	protected:
		Manage::CPUVendor::CPU_BRAND brand;
		Int32 familyId;
		Int32 model;
		Int32 steppingId;

		OSInt infoCnt;
		void *clsData;
	public:
		CPUInfo();
		virtual ~CPUInfo();

		Bool HasInstruction(InstructionType instType);
		Manage::CPUVendor::CPU_BRAND GetBrand();
		Int32 GetFamilyId();
		Int32 GetModelId();
		Int32 GetStepping();

		Bool SupportIntelDTS(); //Digital Thermal Sensor

		OSInt GetInfoCnt();
		Bool GetInfoName(OSInt index, Text::StringBuilderUTF *sb);
		Bool GetInfoValue(OSInt index, Text::StringBuilderUTF *sb);
		OSInt GetCacheInfoList(Data::ArrayList<const UTF8Char*> *infoList);
		void GetFeatureFlags(Int32 *flag1, Int32 *flag2);

		UTF8Char *GetCPUName(UTF8Char *sbuff);
		Bool GetCPURatio(Int32 *ratio);
		Bool GetCPUTurboRatio(Int32 *ratio);
		Bool GetCPUTCC(Double *temp);

		static const UTF8Char *GetCacheInfo(Manage::CPUVendor::CPU_BRAND brand, UInt8 descType);
		static const UTF8Char *GetFeatureShortName(OSInt index);
		static const UTF8Char *GetFeatureName(OSInt index);
		static const UTF8Char *GetFeatureDesc(OSInt index);
		static void AppendNameInfo10(Int32 ecxv, Int32 edxv, Text::StringBuilderUTF *sb);
		static void AppendNameInfo11(Int32 ecxv, Int32 edxv, Text::StringBuilderUTF *sb);
	};
}

#endif
