#include "Stdafx.h"
#include "MyMemory.h"
#include "Manage/CPUVendor.h"

const UTF8Char *Manage::CPUVendor::GetBrandName(CPU_BRAND brand)
{
	switch (brand)
	{
	case CB_INTEL:
		return (const UTF8Char*)"Intel";
	case CB_AMD:
		return (const UTF8Char*)"AMD";
	case CB_BROADCOM:
		return (const UTF8Char*)"Broadcom";
	case CB_TI:
		return (const UTF8Char*)"TI";
	case CB_QUALCOMM:
		return (const UTF8Char*)"Qualcomm";
	case CB_ATHEROS:
		return (const UTF8Char*)"Atheros";
	case CB_ROCKCHIP:
		return (const UTF8Char*)"Rockchip";
	case CB_MEDIATEK:
		return (const UTF8Char*)"MediaTek";
	case CB_INGENIC:
		return (const UTF8Char*)"Ingenic";
	case CB_ALLWINNER:
		return (const UTF8Char*)"Allwinner";
	case CB_MARVELL:
		return (const UTF8Char*)"Marvell";
	case CB_AMLOGIC:
		return (const UTF8Char*)"Amlogic";
	case CB_ATMEL:
		return (const UTF8Char*)"ATmel";
	case CB_GRAINMEDIA:
		return (const UTF8Char*)"GrainMedia";
	case CB_REALTEK:
		return (const UTF8Char*)"Realtek";
	case CB_ANNAPURNA:
		return (const UTF8Char*)"Annapurna Labs";
	case CB_RDA:
		return (const UTF8Char*)"RDA";
	case CB_VIA:
		return (const UTF8Char*)"VIA";
	case CB_HISILICON:
		return (const UTF8Char*)"HiSilicon";
	case CB_SAMSUNG:
		return (const UTF8Char*)"Samsung";
	case CB_SPREADTRUM:
		return (const UTF8Char*)"Spreadtrum";
	case CB_NXP:
		return (const UTF8Char*)"NXP";
	default:
		return (const UTF8Char*)"Unknown";
	}
}
