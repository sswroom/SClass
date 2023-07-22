#ifndef _SM_MANAGE_CPUBRAND
#define _SM_MANAGE_CPUBRAND
#include "Text/CString.h"

namespace Manage
{
	class CPUVendor
	{
	public:
		typedef enum
		{
			CB_UNKNOWN,
			CB_INTEL,
			CB_AMD,
			CB_BROADCOM,
			CB_TI,
			CB_QUALCOMM,
			CB_ATHEROS, //Included in Qualcomm
			CB_ROCKCHIP,
			CB_MEDIATEK,
			CB_INGENIC,
			CB_ALLWINNER,
			CB_MARVELL,
			CB_AMLOGIC,
			CB_ATMEL,
			CB_GRAINMEDIA,
			CB_REALTEK,
			CB_ANNAPURNA, //Annapurna Labs
			CB_RDA,
			CB_VIA,
			CB_HISILICON,
			CB_SAMSUNG,
			CB_SPREADTRUM,
			CB_NXP,
			CB_APPLE
		} CPU_BRAND;

		static Text::CString GetBrandName(CPU_BRAND brand);
	};
}

#endif
