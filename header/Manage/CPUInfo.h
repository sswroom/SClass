#ifndef _SM_MANAGE_CPUINFO
#define _SM_MANAGE_CPUINFO
#include "Data/ArrayList.h"
#include "Manage/CPUVendor.h"
#include "Text/StringBuilderUTF.h"

namespace Manage
{
	class CPUInfo
	{
	protected:
		Manage::CPUVendor::CPU_BRAND brand;
		Int32 familyId;
		Int32 model;
		Int32 steppingId;

		UOSInt infoCnt;
		void *clsData;
	public:
		CPUInfo();
		virtual ~CPUInfo();

		Manage::CPUVendor::CPU_BRAND GetBrand();
		Int32 GetFamilyId();
		Int32 GetModelId();
		Int32 GetStepping();

		Bool SupportIntelDTS(); //Digital Thermal Sensor

		UOSInt GetInfoCnt();
		Bool GetInfoName(UOSInt index, Text::StringBuilderUTF *sb);
		Bool GetInfoValue(UOSInt index, Text::StringBuilderUTF *sb);
		UOSInt GetCacheInfoList(Data::ArrayList<const UTF8Char*> *infoList);
		void GetFeatureFlags(Int32 *flag1, Int32 *flag2);

		UTF8Char *GetCPUName(UTF8Char *sbuff);
		Bool GetCPURatio(Int32 *ratio);
		Bool GetCPUTurboRatio(Int32 *ratio);
		Bool GetCPUTCC(Double *temp);

		static const UTF8Char *GetCacheInfo(Manage::CPUVendor::CPU_BRAND brand, UInt8 descType);
		static const UTF8Char *GetFeatureShortName(UOSInt index);
		static const UTF8Char *GetFeatureName(UOSInt index);
		static const UTF8Char *GetFeatureDesc(UOSInt index);
		static void AppendNameInfo10(UInt32 ecxv, UInt32 edxv, Text::StringBuilderUTF *sb);
		static void AppendNameInfo11(UInt32 ecxv, UInt32 edxv, Text::StringBuilderUTF *sb);
	};
}

#endif
