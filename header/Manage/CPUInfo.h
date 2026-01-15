#ifndef _SM_MANAGE_CPUINFO
#define _SM_MANAGE_CPUINFO
#include "Data/ArrayListArr.hpp"
#include "Manage/CPUVendor.h"
#include "Text/CString.h"
#include "Text/StringBuilderUTF8.h"

namespace Manage
{
	class CPUInfo
	{
	protected:
		Manage::CPUVendor::CPU_BRAND brand;
		Int32 familyId;
		Int32 model;
		Int32 steppingId;

		UIntOS infoCnt;
		void *clsData;
	public:
		CPUInfo();
		virtual ~CPUInfo();

		Manage::CPUVendor::CPU_BRAND GetBrand();
		Int32 GetFamilyId();
		Int32 GetModelId();
		Int32 GetStepping();

		Bool SupportIntelDTS(); //Digital Thermal Sensor

		UIntOS GetInfoCnt();
		Bool GetInfoName(UIntOS index, NN<Text::StringBuilderUTF8> sb);
		Bool GetInfoValue(UIntOS index, NN<Text::StringBuilderUTF8> sb);
		UIntOS GetCacheInfoList(NN<Data::ArrayListArr<const UTF8Char>> infoList);
		void GetFeatureFlags(OutParam<Int32> flag1, OutParam<Int32> flag2);

		UnsafeArrayOpt<UTF8Char> GetCPUName(UnsafeArray<UTF8Char> sbuff);
		Bool GetCPURatio(OutParam<Int32> ratio);
		Bool GetCPUTurboRatio(OutParam<Int32> ratio);
		Bool GetCPUTCC(OutParam<Double> temp);

		static Text::CStringNN GetCacheInfo(Manage::CPUVendor::CPU_BRAND brand, UInt8 descType);
		static Text::CStringNN GetFeatureShortName(UIntOS index);
		static Text::CStringNN GetFeatureName(UIntOS index);
		static Text::CStringNN GetFeatureDesc(UIntOS index);
		static void AppendNameInfo10(UInt32 ecxv, UInt32 edxv, NN<Text::StringBuilderUTF8> sb);
		static void AppendNameInfo11(UInt32 ecxv, UInt32 edxv, NN<Text::StringBuilderUTF8> sb);
	};
}

#endif
