#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/GPIOControl.h"
#include "IO/PhysicalMem.h"
#include "Text/MyString.h"

// MTK MT7628
#define IO_BASE_ADDR 0x10000000
#define BLOCKSIZE 4096

struct IO::GPIOControl::ClassData
{
	IO::PhysicalMem *mem;
	volatile UInt32 *gpioPtr;
	volatile UInt32 *sysCtlPtr;
};

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
	DEL_CLASS(this->clsData->mem);
	MemFree(this->clsData);
}

Bool IO::GPIOControl::IsError()
{
	return this->clsData->mem->IsError();
}

UOSInt IO::GPIOControl::GetPinCount()
{
	return 47; 
}

Bool IO::GPIOControl::IsPinHigh(UOSInt pinNum)
{
	if (pinNum < 32)
	{
		return (this->clsData->gpioPtr[8] & (1 << pinNum)) != 0;
	}
	else if (pinNum < 64)
	{
		return (this->clsData->gpioPtr[9] & (1 << (pinNum - 32))) != 0;
	}
	else if (pinNum < 96)
	{
		return (this->clsData->gpioPtr[10] & (1 << (pinNum - 64))) != 0;
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
		return (this->clsData->gpioPtr[0] & (1 << pinNum)) != 0;
	}
	else if (pinNum < 64)
	{
		return (this->clsData->gpioPtr[1] & (1 << (pinNum - 32))) != 0;
	}
	else if (pinNum < 96)
	{
		return (this->clsData->gpioPtr[2] & (1 << (pinNum - 64))) != 0;
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
		mode = (this->clsData->sysCtlPtr[18] >> 6) & 3; //7:6 I2S_MODE I2S GPIO mode
		break;
	case 4:
	case 5:
		mode = (this->clsData->sysCtlPtr[18] >> 20) & 3; //21:20 I2C_MODE I2C GPIO mode
		break;
	case 6:
		mode = (this->clsData->sysCtlPtr[18] >> 4) & 3; //5:4 SPI_CS1_MODE SPI CS1 GPIO mode
		break;
	case 7:
	case 8:
	case 9:
	case 10:
		mode = (this->clsData->sysCtlPtr[18] >> 12) & 3; //12 SPI_MODE SPI GPIO mode
		break;
	case 11:
		mode = (this->clsData->sysCtlPtr[18] >> 0) & 3; //1:0 GPIO_MODE GPIO mode
		break;
	case 12:
	case 13:
		mode = (this->clsData->sysCtlPtr[18] >> 8) & 3; //9:8 UART0_MODE UART0 GPIO mode
		break;
	case 14:
	case 15:
	case 16:
	case 17:
		mode = (this->clsData->sysCtlPtr[18] >> 2) & 3; //3:2 SPIS_MODE SPI Slave GPIO mode
		break;
	case 18:
		mode = (this->clsData->sysCtlPtr[18] >> 28) & 3; //29:28 PWM0_MODE PWM0 GPIO mode
		break;
	case 19:
		mode = (this->clsData->sysCtlPtr[18] >> 30) & 3; //31:30 PWM1_MODE PWM1 GPIO mode
		break;
	case 20:
	case 21:
		mode = (this->clsData->sysCtlPtr[18] >> 26) & 3; //27:26 UART2_MODE UART2 GPIO mode
		break;
	case 22:
	case 23:
	case 24:
	case 25:
	case 26:
	case 27:
	case 28:
	case 29:
		mode = (this->clsData->sysCtlPtr[18] >> 10) & 3; //11:10 SD_MODE SDXC GPIO mode
		break;
	case 30:
		mode = (this->clsData->sysCtlPtr[19] >> 26) & 3; //27:26 P4_LED_KN_MODE EPHY P4 LED GPIO mode
		break;
	case 31:
		mode = (this->clsData->sysCtlPtr[19] >> 24) & 3; //25:24 P3_LED_KN_MODE EPHY P3 LED GPIO mode
		break;
	case 32:
		mode = (this->clsData->sysCtlPtr[19] >> 22) & 3; //23:22 P2_LED_KN_MODE EPHY P2 LED GPIO mode
		break;
	case 33:
		mode = (this->clsData->sysCtlPtr[19] >> 20) & 3; //21:20 P1_LED_KN_MODE EPHY P1 LED GPIO mode
		break;
	case 34:
		mode = (this->clsData->sysCtlPtr[19] >> 18) & 3; //19:18 P0_LED_KN_MODE EPHY P0 LED GPIO mode
		break;
	case 35:
		mode = (this->clsData->sysCtlPtr[19] >> 16) & 3; //17:16 WLED_KN_MODE WLED GPIO mode
		break;
	case 36:
		mode = (this->clsData->sysCtlPtr[18] >> 16) & 3; //16 PERST_MODE PCIe RESET GPIO mode
		break;
	case 37:
		mode = (this->clsData->sysCtlPtr[18] >> 18) & 3; //18 REFCLK_MODE REFCLK GPIO mode
		break;
	case 38:
		mode = (this->clsData->sysCtlPtr[18] >> 14) & 3; //14 WDT_MODE Watch dog timeout GPIO mode
		break;
	case 39:
		mode = (this->clsData->sysCtlPtr[19] >> 10) & 3; //11:10 P4_LED_AN_MODE EPHY P4 LED GPIO mode
		break;
	case 40:
		mode = (this->clsData->sysCtlPtr[19] >> 8) & 3; //9:8 P3_LED_AN_MODE EPHY P3 LED GPIO mode
		break;
	case 41:
		mode = (this->clsData->sysCtlPtr[19] >> 6) & 3; //7:6 P2_LED_AN_MODE EPHY P2 LED GPIO mode
		break;
	case 42:
		mode = (this->clsData->sysCtlPtr[19] >> 4) & 3; //5:4 P1_LED_AN_MODE EPHY P1 LED GPIO mode
		break;
	case 43:
		mode = (this->clsData->sysCtlPtr[19] >> 2) & 3; //3:2 P0_LED_AN_MODE EPHY P0 LED GPIO mode
		break;
	case 44:
		mode = (this->clsData->sysCtlPtr[19] >> 0) & 3; //1:0 WLED_AN_MODE WLED GPIO mode
		break;
	case 45:
	case 46:
		mode = (this->clsData->sysCtlPtr[18] >> 24) & 3;  //25:24 UART1_MODE UART1 GPIO mode
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
			this->clsData->gpioPtr[0] |= (1 << pinNum);
		}
		else if (pinNum < 64)
		{
			this->clsData->gpioPtr[1] |= (1 << (pinNum - 32));
		}
		else if (pinNum < 96)
		{
			this->clsData->gpioPtr[2] |= (1 << (pinNum - 64));
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
			this->clsData->gpioPtr[0] &= ~(1 << pinNum);
		}
		else if (pinNum < 64)
		{
			this->clsData->gpioPtr[1] &= ~(1 << (pinNum - 32));
		}
		else if (pinNum < 96)
		{
			this->clsData->gpioPtr[2] &= ~(1 << (pinNum - 64));
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
			this->clsData->gpioPtr[12] = (1 << pinNum);
		}
		else if (pinNum < 64)
		{
			this->clsData->gpioPtr[13] = (1 << (pinNum - 32));
		}
		else if (pinNum < 96)
		{
			this->clsData->gpioPtr[14] = (1 << (pinNum - 64));
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
			this->clsData->gpioPtr[16] = (1 << pinNum);
		}
		else if (pinNum < 64)
		{
			this->clsData->gpioPtr[17] = (1 << (pinNum - 32));
		}
		else if (pinNum < 96)
		{
			this->clsData->gpioPtr[18] = (1 << (pinNum - 64));
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

static Text::CString GPIOControl_Func0[] = {
	CSTR("i2ssdi"),
	CSTR("i2ssdo"),
	CSTR("i2sws"),
	CSTR("i2sclk"),
	CSTR("i2c_sclk"),
	CSTR("i2c_sd"),
	CSTR("spi_cs1"),
	CSTR("spi_clk"),
	CSTR("spi_mosi"),
	CSTR("spi_miso"),

	CSTR("spi_cs0"),
	CSTR("gpio"),
	CSTR("txd0"),
	CSTR("rxd0"),
	CSTR("spis_cs"),
	CSTR("spis_clk"),
	CSTR("spis_miso"),
	CSTR("spis_mosi"),
	CSTR("pwm_ch0"),
	CSTR("pwm_ch1"),

	CSTR("txd2"),
	CSTR("rxd2"),
	CSTR("sd_wp"),
	CSTR("sd_cd"),
	CSTR("sd_d1"),
	CSTR("sd_d0"),
	CSTR("sd_clk"),
	CSTR("sd_cmd"),
	CSTR("sd_d3"),
	CSTR("sd_d2"),

	CSTR("ephy_led4_k"),
	CSTR("ephy_led3_k"),
	CSTR("ephy_led2_k"),
	CSTR("ephy_led1_k"),
	CSTR("ephy_led0_k"),
	CSTR("wled_k"),
	CSTR("perst_n"),
	CSTR("co_clko"),
	CSTR("wdt"),
	CSTR("ephy_led4_n"),

	CSTR("ephy_led3_n"),
	CSTR("ephy_led2_n"),
	CSTR("ephy_led1_n"),
	CSTR("ephy_led0_n"),
	CSTR("wled_n"),
	CSTR("txd1"),
	CSTR("rxd1"),
};

static Text::CString GPIOControl_Func2[] = {
	CSTR("pcmdrx"),
	CSTR("pcmdtx"),
	CSTR("pcmclk"),
	CSTR("pcmfs"),
	CSTR("sutif_txd"),
	CSTR("sutif_rxd"),
	CSTR("co_clko"),
	CSTR("Unknown"),
	CSTR("Unknown"),
	CSTR("Unknown"),

	CSTR("Unknown"),
	CSTR("co_clko"),
	CSTR("Unknown"),
	CSTR("Unknown"),
	CSTR("w_utif[0]"),
	CSTR("w_utif[1]"),
	CSTR("w_utif[2]"),
	CSTR("w_utif[3]"),
	CSTR("w_utif[4]"),
	CSTR("w_utif[5]"),

	CSTR("pwm_ch2"),
	CSTR("pwm_ch3"),
	CSTR("w_utif[10]"),
	CSTR("w_utif[11]"),
	CSTR("w_utif[12]"),
	CSTR("w_utif[13]"),
	CSTR("w_utif[14]"),
	CSTR("w_utif[15]"),
	CSTR("w_utif[16]"),
	CSTR("w_utif[17]"),

	CSTR("w_utif_k[6]"),
	CSTR("w_utif_k[7]"),
	CSTR("w_utif_k[8]"),
	CSTR("w_utif_k[9]"),
	CSTR("Unknown"),
	CSTR("Unknown"),
	CSTR("Unknown"),
	CSTR("Unknown"),
	CSTR("Unknown"),
	CSTR("w_utif_n[6]"),

	CSTR("w_utif_n[7]"),
	CSTR("w_utif_n[8]"),
	CSTR("w_utif_n[9]"),
	CSTR("Unknown"),
	CSTR("Unknown"),
	CSTR("pwm_ch0"),
	CSTR("pwm_ch1"),
};

static Text::CString GPIOControl_Func3[] = {
	CSTR("antsel[5]"),
	CSTR("antsel[4]"),
	CSTR("antsel[3]"),
	CSTR("antsel[2]"),
	CSTR("ext_bgclk"),
	CSTR("Unknown"),
	CSTR("Unknown"),
	CSTR("Unknown"),
	CSTR("Unknown"),
	CSTR("Unknown"),

	CSTR("Unknown"),
	CSTR("perst_n"),
	CSTR("Unknown"),
	CSTR("Unknown"),
	CSTR("pwm_ch0"),
	CSTR("pwm_ch1"),
	CSTR("txd2"),
	CSTR("rxd2"),
	CSTR("sd_d7"),
	CSTR("sd_d6"),

	CSTR("sd_d5"),
	CSTR("sd_d4"),
	CSTR("w_dbgin"),
	CSTR("w_dbgack"),
	CSTR("w_jtclk"),
	CSTR("w_jtdi"),
	CSTR("w_jtdo"),
	CSTR("dbg_uart_txd"),
	CSTR("w_jtms"),
	CSTR("w_jtrst_n"),

	CSTR("jtrstn_k"),
	CSTR("jtclk_k"),
	CSTR("jtms_k"),
	CSTR("jtdi_k"),
	CSTR("jtdo_k"),
	CSTR("Unknown"),
	CSTR("Unknown"),
	CSTR("Unknown"),
	CSTR("Unknown"),
	CSTR("jtrstn_n"),

	CSTR("jtclk_n"),
	CSTR("jtms_n"),
	CSTR("jtdi_n"),
	CSTR("jtdo_n"),
	CSTR("Unknown"),
	CSTR("antsel[1]"),
	CSTR("antsel[0]"),
};

Text::CString IO::GPIOControl::PinModeGetName(UOSInt pinNum, UOSInt pinState)
{
	if (pinNum >= 47)
	{
		return CSTR("Unknown");
	}
	if (pinState == 1)
	{
		return CSTR("Input");
	}
	else if (pinState == 4)
	{
		return CSTR("Output");
	}
	if (pinNum >= 47)
	{
		return CSTR("Unknown");
	}
	if (pinState == 0)
	{
		return GPIOControl_Func0[pinNum];	
	}
	else if (pinState == 2)
	{
		return GPIOControl_Func2[pinNum];	
	}
	else if (pinState == 3)
	{
		return GPIOControl_Func3[pinNum];	
	}
	else
	{
		return CSTR("Unknown");
	}
}
