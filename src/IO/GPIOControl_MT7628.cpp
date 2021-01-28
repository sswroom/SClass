#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/GPIOControl.h"
#include "IO/PhysicalMem.h"
#include "Text/MyString.h"

// MTK MT7628
#define IO_BASE_ADDR 0x10000000
#define BLOCKSIZE 4096

typedef struct
{
	IO::PhysicalMem *mem;
	volatile UInt32 *gpioPtr;
	volatile UInt32 *sysCtlPtr;
} ClassData;

IO::GPIOControl::GPIOControl()
{
	ClassData *clsData = MemAlloc(ClassData, 1);
	NEW_CLASS(clsData->mem, IO::PhysicalMem(IO_BASE_ADDR, BLOCKSIZE));
	clsData->gpioPtr = (volatile UInt32 *)(clsData->mem->GetPointer() + 0x600);
	clsData->sysCtlPtr = (volatile UInt32 *)(clsData->mem->GetPointer() + 0x000);
	this->clsData = clsData;
}

IO::GPIOControl::~GPIOControl()
{
	ClassData *clsData = (ClassData*)this->clsData;
	DEL_CLASS(clsData->mem);
	MemFree(clsData);
}

Bool IO::GPIOControl::IsError()
{
	return ((ClassData*)this->clsData)->mem->IsError();
}

UOSInt IO::GPIOControl::GetPinCount()
{
	return 47; 
}

Bool IO::GPIOControl::IsPinHigh(UOSInt pinNum)
{
	if (pinNum < 32)
	{
		return (((ClassData*)this->clsData)->gpioPtr[8] & (1 << pinNum)) != 0;
	}
	else if (pinNum < 64)
	{
		return (((ClassData*)this->clsData)->gpioPtr[9] & (1 << (pinNum - 32))) != 0;
	}
	else if (pinNum < 96)
	{
		return (((ClassData*)this->clsData)->gpioPtr[10] & (1 << (pinNum - 64))) != 0;
	}
	else
	{
		return false;
	}
	
}

Bool IO::GPIOControl::IsPinOutput(UOSInt pinNum)
{
	if (pinNum < 32)
	{
		return (((ClassData*)this->clsData)->gpioPtr[0] & (1 << pinNum)) != 0;
	}
	else if (pinNum < 64)
	{
		return (((ClassData*)this->clsData)->gpioPtr[1] & (1 << (pinNum - 32))) != 0;
	}
	else if (pinNum < 96)
	{
		return (((ClassData*)this->clsData)->gpioPtr[2] & (1 << (pinNum - 64))) != 0;
	}
	else
	{
		return false;
	}
}

UOSInt IO::GPIOControl::GetPinMode(UOSInt pinNum)
{
	if (pinNum >= 47)
	{
		return 0;
	}
	if (pinNum >= 47)
	{
		return this->IsPinOutput(pinNum)?4:1;
	}
	UOSInt mode = 1;
	switch (pinNum)
	{
	case 0:
	case 1:
	case 2:
	case 3:
		mode = (((ClassData*)this->clsData)->sysCtlPtr[18] >> 6) & 3; //7:6 I2S_MODE I2S GPIO mode
		break;
	case 4:
	case 5:
		mode = (((ClassData*)this->clsData)->sysCtlPtr[18] >> 20) & 3; //21:20 I2C_MODE I2C GPIO mode
		break;
	case 6:
		mode = (((ClassData*)this->clsData)->sysCtlPtr[18] >> 4) & 3; //5:4 SPI_CS1_MODE SPI CS1 GPIO mode
		break;
	case 7:
	case 8:
	case 9:
	case 10:
		mode = (((ClassData*)this->clsData)->sysCtlPtr[18] >> 12) & 3; //12 SPI_MODE SPI GPIO mode
		break;
	case 11:
		mode = (((ClassData*)this->clsData)->sysCtlPtr[18] >> 0) & 3; //1:0 GPIO_MODE GPIO mode
		break;
	case 12:
	case 13:
		mode = (((ClassData*)this->clsData)->sysCtlPtr[18] >> 8) & 3; //9:8 UART0_MODE UART0 GPIO mode
		break;
	case 14:
	case 15:
	case 16:
	case 17:
		mode = (((ClassData*)this->clsData)->sysCtlPtr[18] >> 2) & 3; //3:2 SPIS_MODE SPI Slave GPIO mode
		break;
	case 18:
		mode = (((ClassData*)this->clsData)->sysCtlPtr[18] >> 28) & 3; //29:28 PWM0_MODE PWM0 GPIO mode
		break;
	case 19:
		mode = (((ClassData*)this->clsData)->sysCtlPtr[18] >> 30) & 3; //31:30 PWM1_MODE PWM1 GPIO mode
		break;
	case 20:
	case 21:
		mode = (((ClassData*)this->clsData)->sysCtlPtr[18] >> 26) & 3; //27:26 UART2_MODE UART2 GPIO mode
		break;
	case 22:
	case 23:
	case 24:
	case 25:
	case 26:
	case 27:
	case 28:
	case 29:
		mode = (((ClassData*)this->clsData)->sysCtlPtr[18] >> 10) & 3; //11:10 SD_MODE SDXC GPIO mode
		break;
	case 30:
		mode = (((ClassData*)this->clsData)->sysCtlPtr[19] >> 26) & 3; //27:26 P4_LED_KN_MODE EPHY P4 LED GPIO mode
		break;
	case 31:
		mode = (((ClassData*)this->clsData)->sysCtlPtr[19] >> 24) & 3; //25:24 P3_LED_KN_MODE EPHY P3 LED GPIO mode
		break;
	case 32:
		mode = (((ClassData*)this->clsData)->sysCtlPtr[19] >> 22) & 3; //23:22 P2_LED_KN_MODE EPHY P2 LED GPIO mode
		break;
	case 33:
		mode = (((ClassData*)this->clsData)->sysCtlPtr[19] >> 20) & 3; //21:20 P1_LED_KN_MODE EPHY P1 LED GPIO mode
		break;
	case 34:
		mode = (((ClassData*)this->clsData)->sysCtlPtr[19] >> 18) & 3; //19:18 P0_LED_KN_MODE EPHY P0 LED GPIO mode
		break;
	case 35:
		mode = (((ClassData*)this->clsData)->sysCtlPtr[19] >> 16) & 3; //17:16 WLED_KN_MODE WLED GPIO mode
		break;
	case 36:
		mode = (((ClassData*)this->clsData)->sysCtlPtr[18] >> 16) & 3; //16 PERST_MODE PCIe RESET GPIO mode
		break;
	case 37:
		mode = (((ClassData*)this->clsData)->sysCtlPtr[18] >> 18) & 3; //18 REFCLK_MODE REFCLK GPIO mode
		break;
	case 38:
		mode = (((ClassData*)this->clsData)->sysCtlPtr[18] >> 14) & 3; //14 WDT_MODE Watch dog timeout GPIO mode
		break;
	case 39:
		mode = (((ClassData*)this->clsData)->sysCtlPtr[19] >> 10) & 3; //11:10 P4_LED_AN_MODE EPHY P4 LED GPIO mode
		break;
	case 40:
		mode = (((ClassData*)this->clsData)->sysCtlPtr[19] >> 8) & 3; //9:8 P3_LED_AN_MODE EPHY P3 LED GPIO mode
		break;
	case 41:
		mode = (((ClassData*)this->clsData)->sysCtlPtr[19] >> 6) & 3; //7:6 P2_LED_AN_MODE EPHY P2 LED GPIO mode
		break;
	case 42:
		mode = (((ClassData*)this->clsData)->sysCtlPtr[19] >> 4) & 3; //5:4 P1_LED_AN_MODE EPHY P1 LED GPIO mode
		break;
	case 43:
		mode = (((ClassData*)this->clsData)->sysCtlPtr[19] >> 2) & 3; //3:2 P0_LED_AN_MODE EPHY P0 LED GPIO mode
		break;
	case 44:
		mode = (((ClassData*)this->clsData)->sysCtlPtr[19] >> 0) & 3; //1:0 WLED_AN_MODE WLED GPIO mode
		break;
	case 45:
	case 46:
		mode = (((ClassData*)this->clsData)->sysCtlPtr[18] >> 24) & 3;  //25:24 UART1_MODE UART1 GPIO mode
		break;
	}
	if (mode == 1)
	{
		if (this->IsPinOutput(pinNum))
		{
			mode = 4;
		}
	}
	return mode;
}

Bool IO::GPIOControl::SetPinOutput(UOSInt pinNum, Bool isOutput)
{
	if (isOutput)
	{
		if (pinNum < 32)
		{
			((ClassData*)this->clsData)->gpioPtr[0] |= (1 << pinNum);
		}
		else if (pinNum < 64)
		{
			((ClassData*)this->clsData)->gpioPtr[1] |= (1 << (pinNum - 32));
		}
		else if (pinNum < 96)
		{
			((ClassData*)this->clsData)->gpioPtr[2] |= (1 << (pinNum - 64));
		}
		else
		{
			return false;
		}
	}
	else
	{
		if (pinNum < 32)
		{
			((ClassData*)this->clsData)->gpioPtr[0] &= ~(1 << pinNum);
		}
		else if (pinNum < 64)
		{
			((ClassData*)this->clsData)->gpioPtr[1] &= ~(1 << (pinNum - 32));
		}
		else if (pinNum < 96)
		{
			((ClassData*)this->clsData)->gpioPtr[2] &= ~(1 << (pinNum - 64));
		}
		else
		{
			return false;
		}
	}
	return true;
}

Bool IO::GPIOControl::SetPinState(UOSInt pinNum, Bool isHigh)
{
	if (isHigh)
	{
		if (pinNum < 32)
		{
			((ClassData*)this->clsData)->gpioPtr[12] |= (1 << pinNum);
		}
		else if (pinNum < 64)
		{
			((ClassData*)this->clsData)->gpioPtr[13] |= (1 << (pinNum - 32));
		}
		else if (pinNum < 96)
		{
			((ClassData*)this->clsData)->gpioPtr[14] |= (1 << (pinNum - 64));
		}
		else
		{
			return false;
		}
	}
	else
	{
		if (pinNum < 32)
		{
			((ClassData*)this->clsData)->gpioPtr[16] |= (1 << pinNum);
		}
		else if (pinNum < 64)
		{
			((ClassData*)this->clsData)->gpioPtr[17] |= (1 << (pinNum - 32));
		}
		else if (pinNum < 96)
		{
			((ClassData*)this->clsData)->gpioPtr[18] |= (1 << (pinNum - 64));
		}
		else
		{
			return false;
		}
	}
	return true;
}

Bool IO::GPIOControl::SetPullType(UOSInt pinNum, IO::IOPin::PullType pt)
{
	return false;
}

void IO::GPIOControl::SetEventOnHigh(UOSInt pinNum, Bool enable)
{
}

void IO::GPIOControl::SetEventOnLow(UOSInt pinNum, Bool enable)
{
}

void IO::GPIOControl::SetEventOnRaise(UOSInt pinNum, Bool enable)
{
}

void IO::GPIOControl::SetEventOnFall(UOSInt pinNum, Bool enable)
{
}

Bool IO::GPIOControl::HasEvent(UOSInt pinNum)
{
	return false;
}

void IO::GPIOControl::ClearEvent(UOSInt pinNum)
{
}

static const Char *GPIOControl_Func0[] = {
	"i2ssdi",
	"i2ssdo",
	"i2sws",
	"i2sclk",
	"i2c_sclk",
	"i2c_sd",
	"spi_cs1",
	"spi_clk",
	"spi_mosi",
	"spi_miso",

	"spi_cs0",
	"gpio",
	"txd0",
	"rxd0",
	"spis_cs",
	"spis_clk",
	"spis_miso",
	"spis_mosi",
	"pwm_ch0",
	"pwm_ch1",

	"txd2",
	"rxd2",
	"sd_wp",
	"sd_cd",
	"sd_d1",
	"sd_d0",
	"sd_clk",
	"sd_cmd",
	"sd_d3",
	"sd_d2",

	"ephy_led4_k",
	"ephy_led3_k",
	"ephy_led2_k",
	"ephy_led1_k",
	"ephy_led0_k",
	"wled_k",
	"perst_n",
	"co_clko",
	"wdt",
	"ephy_led4_n",

	"ephy_led3_n",
	"ephy_led2_n",
	"ephy_led1_n",
	"ephy_led0_n",
	"wled_n",
	"txd1",
	"rxd1",
};

static const Char *GPIOControl_Func2[] = {
	"pcmdrx",
	"pcmdtx",
	"pcmclk",
	"pcmfs",
	"sutif_txd",
	"sutif_rxd",
	"co_clko",
	"Unknown",
	"Unknown",
	"Unknown",

	"Unknown",
	"co_clko",
	"Unknown",
	"Unknown",
	"w_utif[0]",
	"w_utif[1]",
	"w_utif[2]",
	"w_utif[3]",
	"w_utif[4]",
	"w_utif[5]",

	"pwm_ch2",
	"pwm_ch3",
	"w_utif[10]",
	"w_utif[11]",
	"w_utif[12]",
	"w_utif[13]",
	"w_utif[14]",
	"w_utif[15]",
	"w_utif[16]",
	"w_utif[17]",

	"w_utif_k[6]",
	"w_utif_k[7]",
	"w_utif_k[8]",
	"w_utif_k[9]",
	"Unknown",
	"Unknown",
	"Unknown",
	"Unknown",
	"Unknown",
	"w_utif_n[6]",

	"w_utif_n[7]",
	"w_utif_n[8]",
	"w_utif_n[9]",
	"Unknown",
	"Unknown",
	"pwm_ch0",
	"pwm_ch1",
};

static const Char *GPIOControl_Func3[] = {
	"antsel[5]",
	"antsel[4]",
	"antsel[3]",
	"antsel[2]",
	"ext_bgclk",
	"Unknown",
	"Unknown",
	"Unknown",
	"Unknown",
	"Unknown",

	"Unknown",
	"perst_n",
	"Unknown",
	"Unknown",
	"pwm_ch0",
	"pwm_ch1",
	"txd2",
	"rxd2",
	"sd_d7",
	"sd_d6",

	"sd_d5",
	"sd_d4",
	"w_dbgin",
	"w_dbgack",
	"w_jtclk",
	"w_jtdi",
	"w_jtdo",
	"dbg_uart_txd",
	"w_jtms",
	"w_jtrst_n",

	"jtrstn_k",
	"jtclk_k",
	"jtms_k",
	"jtdi_k",
	"jtdo_k",
	"Unknown",
	"Unknown",
	"Unknown",
	"Unknown",
	"jtrstn_n",

	"jtclk_n",
	"jtms_n",
	"jtdi_n",
	"jtdo_n",
	"Unknown",
	"antsel[1]",
	"antsel[0]",
};

const UTF8Char *IO::GPIOControl::PinModeGetName(UOSInt pinNum, UOSInt pinState)
{
	if (pinNum >= 47)
	{
		return (const UTF8Char*)"Unknown";
	}
	if (pinState == 1)
	{
		return (const UTF8Char*)"Input";
	}
	else if (pinState == 4)
	{
		return (const UTF8Char*)"Output";
	}
	if (pinNum >= 47)
	{
		return (const UTF8Char*)"Unknown";
	}
	if (pinState == 0)
	{
		return (const UTF8Char*)GPIOControl_Func0[pinNum];	
	}
	else if (pinState == 2)
	{
		return (const UTF8Char*)GPIOControl_Func2[pinNum];	
	}
	else if (pinState == 3)
	{
		return (const UTF8Char*)GPIOControl_Func3[pinNum];	
	}
	else
	{
		return (const UTF8Char*)"Unknown";
	}
}
