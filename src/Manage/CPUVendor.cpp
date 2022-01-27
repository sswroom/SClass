#include "Stdafx.h"
#include "MyMemory.h"
#include "Manage/CPUVendor.h"

Text::CString Manage::CPUVendor::GetBrandName(CPU_BRAND brand)
{
	switch (brand)
	{
	case CB_INTEL:
		return CSTR("Intel");
	case CB_AMD:
		return CSTR("AMD");
	case CB_BROADCOM:
		return CSTR("Broadcom");
	case CB_TI:
		return CSTR("TI");
	case CB_QUALCOMM:
		return CSTR("Qualcomm");
	case CB_ATHEROS:
		return CSTR("Atheros");
	case CB_ROCKCHIP:
		return CSTR("Rockchip");
	case CB_MEDIATEK:
		return CSTR("MediaTek");
	case CB_INGENIC:
		return CSTR("Ingenic");
	case CB_ALLWINNER:
		return CSTR("Allwinner");
	case CB_MARVELL:
		return CSTR("Marvell");
	case CB_AMLOGIC:
		return CSTR("Amlogic");
	case CB_ATMEL:
		return CSTR("ATmel");
	case CB_GRAINMEDIA:
		return CSTR("GrainMedia");
	case CB_REALTEK:
		return CSTR("Realtek");
	case CB_ANNAPURNA:
		return CSTR("Annapurna Labs");
	case CB_RDA:
		return CSTR("RDA");
	case CB_VIA:
		return CSTR("VIA");
	case CB_HISILICON:
		return CSTR("HiSilicon");
	case CB_SAMSUNG:
		return CSTR("Samsung");
	case CB_SPREADTRUM:
		return CSTR("Spreadtrum");
	case CB_NXP:
		return CSTR("NXP");
	case CB_UNKNOWN:
	default:
		return CSTR("Unknown");
	}
}
