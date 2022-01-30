#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/GPIOControl.h"
#include "IO/PhysicalMem.h"
#include "Text/MyString.h"

// Freescale i.MX6UL
#define IO_BASE_ADDR 0x209C000
#define BLOCKSIZE 0x14000

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
	clsData->gpioPtr = (volatile UInt32 *)clsData->mem->GetPointer();
	clsData->sysCtlPtr = (volatile UInt32 *)clsData->mem->GetPointer();
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
	return 160;
}

Bool IO::GPIOControl::IsPinHigh(UOSInt pinNum)
{
	if (pinNum < 24)
	{
		return (this->clsData->gpioPtr[8] & (1 << pinNum)) != 0;
	}
	else if (pinNum < 40)
	{
		return (this->clsData->gpioPtr[18] & (1 << (pinNum - 24))) != 0;
	}
	else if (pinNum < 72)
	{
		return (this->clsData->gpioPtr[28] & (1 << (pinNum - 40))) != 0;
	}
	else if (pinNum == 72)
	{
		return (this->clsData->gpioPtr[38] & 1) != 0;
	}
	else
	{
		return false;
	}
	
}

Bool IO::GPIOControl::IsPinOutput(UOSInt pinNum)
{
	if (pinNum < 24)
	{
		return (this->clsData->gpioPtr[9] & (1 << pinNum)) != 0;
	}
	else if (pinNum < 40)
	{
		return (this->clsData->gpioPtr[19] & (1 << (pinNum - 24))) != 0;
	}
	else if (pinNum < 72)
	{
		return (this->clsData->gpioPtr[29] & (1 << (pinNum - 40))) != 0;
	}
	else if (pinNum == 72)
	{
		return (this->clsData->gpioPtr[39] & 1) != 0;
	}
	else
	{
		return false;
	}
}


UOSInt IO::GPIOControl::GetPinMode(UOSInt pinNum)
{
	if (pinNum >= 73)
	{
		return 0;
	}
	UInt16 mode = 0;
	Bool isGPIO = false;
	switch (pinNum)
	{
	case 0: //GPIO
		mode = 0;
		isGPIO = true;
		break;
	case 1: //I2C/SUTIF
	case 2:
		mode = (this->clsData->sysCtlPtr[18] >> 0) & 1;
		isGPIO = (mode == 1);
		break;
	case 3: //SPI
	case 4:
	case 5:
	case 6:
	case 37: //SPI
	case 38:
	case 39:
		mode = (this->clsData->sysCtlPtr[18] >> 11) & 3;
		isGPIO = (mode & 1) || pinNum == 38 || pinNum == 39 || (pinNum == 37 && mode == 3);
		break;
	case 7: //UARTF
	case 8:
	case 9:
	case 10:
	case 11:
	case 12:
	case 13:
	case 14:
		mode = (this->clsData->sysCtlPtr[18] >> 2) & 7;
		isGPIO = (mode == 7) || (pinNum >= 7 && pinNum <= 10 && mode == 4) || (pinNum >= 11 && pinNum <= 14 && mode >= 5);
		break;
	case 15: //UARTL
	case 16:
		mode = (this->clsData->sysCtlPtr[18] >> 5) & 1;
		isGPIO = (mode == 1);
		break;
	case 17: //WDT_RST
		mode = (this->clsData->sysCtlPtr[18] >> 21) & 3;
		isGPIO = (mode == 2);
		break;
	case 18: //PA_PE
	case 19:
	case 20:
	case 21:
		mode = (this->clsData->sysCtlPtr[18] >> 20) & 1;
		isGPIO = (mode == 1);
		break;
	case 22: //MDIO
	case 23:
		mode = (this->clsData->sysCtlPtr[18] >> 7) & 3;
		isGPIO = (mode == 2);
		break;
	case 24: //RGMII1
	case 25:
	case 26:
	case 27:
	case 28:
	case 29:
	case 30:
	case 31:
	case 32:
	case 33:
	case 34:
	case 35:
		mode = (this->clsData->sysCtlPtr[18] >> 9) & 1;
		isGPIO = (mode == 1);
		break;
	case 36: //PERST_N
		mode = (this->clsData->sysCtlPtr[18] >> 15) & 3;
		isGPIO = (mode == 2);
		break;
	case 40: //SW_PHY_LED/JTAG
	case 41:
	case 42:
	case 43:
	case 44:
		mode = (this->clsData->sysCtlPtr[18] >> 15) & 1;
		isGPIO = (mode == 1);
		break;
	case 45: //NAND
	case 46:
	case 47:
	case 48:
	case 49:
	case 50:
	case 51:
	case 52:
	case 53:
	case 54:
	case 55:
	case 56:
	case 57:
	case 58:
	case 59:
		mode = (this->clsData->sysCtlPtr[18] >> 15) & 3;
		isGPIO = (mode == 2);
		break;
	case 60: //RGMII2
	case 61:
	case 62:
	case 63:
	case 64:
	case 65:
	case 66:
	case 67:
	case 68:
	case 69:
	case 70:
	case 71:
		mode = (this->clsData->sysCtlPtr[18] >> 10) & 1;
		isGPIO = (mode == 1);
		break;
	case 72: //WLED_N
		mode = (this->clsData->sysCtlPtr[18] >> 13) & 1;
		isGPIO = (mode == 1);
		break;
	}
	if (isGPIO)
	{
		if (this->IsPinOutput(pinNum))
		{
			mode = 8;
		}
	}
	return mode;
}

Bool IO::GPIOControl::SetPinOutput(UOSInt pinNum, Bool isOutput)
{
	if (isOutput)
	{
		if (pinNum < 24)
		{
			this->clsData->gpioPtr[9] |= (1 << pinNum);
		}
		else if (pinNum < 40)
		{
			this->clsData->gpioPtr[19] |= (1 << (pinNum - 24));
		}
		else if (pinNum < 72)
		{
			this->clsData->gpioPtr[29] |= (1 << (pinNum - 40));
		}
		else if (pinNum == 72)
		{
			this->clsData->gpioPtr[39] |= 1;
		}
		else
		{
			return false;
		}
	}
	else
	{
		if (pinNum < 24)
		{
			this->clsData->gpioPtr[9] &= ~(1 << pinNum);
		}
		else if (pinNum < 40)
		{
			this->clsData->gpioPtr[19] &= ~(1 << (pinNum - 24));
		}
		else if (pinNum < 72)
		{
			this->clsData->gpioPtr[29] &= ~(1 << (pinNum - 40));
		}
		else if (pinNum == 72)
		{
			this->clsData->gpioPtr[39] &= ~1;
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
		if (pinNum < 24)
		{
			this->clsData->gpioPtr[11] |= (1 << pinNum);
		}
		else if (pinNum < 40)
		{
			this->clsData->gpioPtr[21] |= (1 << (pinNum - 24));
		}
		else if (pinNum < 72)
		{
			this->clsData->gpioPtr[31] |= (1 << (pinNum - 40));
		}
		else if (pinNum == 72)
		{
			this->clsData->gpioPtr[41] |= 1;
		}
		else
		{
			return false;
		}
	}
	else
	{
		if (pinNum < 24)
		{
			this->clsData->gpioPtr[12] |= (1 << pinNum);
		}
		else if (pinNum < 40)
		{
			this->clsData->gpioPtr[22] |= (1 << (pinNum - 24));
		}
		else if (pinNum < 72)
		{
			this->clsData->gpioPtr[32] |= (1 << (pinNum - 40));
		}
		else if (pinNum == 72)
		{
			this->clsData->gpioPtr[42] |= 1;
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

Text::CString GPIOControl_Func0[] = {
	CSTR("GPO0"),
	CSTR("I2C_SD"),
	CSTR("I2C_SCLK"),
	CSTR("SPI_CS0"),
	CSTR("SPI_CLK"),
	CSTR("SPI_MOSI"),
	CSTR("SPI_MISO"),
	CSTR("RTS_N"),
	CSTR("TXD"),
	CSTR("CTS_N"),

	CSTR("RXD"),
	CSTR("DTR_N"),
	CSTR("DCD_N"),
	CSTR("DSR_N"),
	CSTR("RIN"),
	CSTR("TXD2"),
	CSTR("RXD2"),
	CSTR("WDT_RST_N"),
	CSTR("ANT_TRNB"),
	CSTR("ANT_TRN"),

	CSTR("PA_PE_G0"),
	CSTR("PA_PE_G1"),
	CSTR("MDIO"),
	CSTR("MDC"),
	CSTR("GE1_TXD0"),
	CSTR("GE1_TXD1"),
	CSTR("GE1_TXD2"),
	CSTR("GE1_TXD3"),
	CSTR("GE1_TXEN"),
	CSTR("GE1_TXCLK"),

	CSTR("GE1_RXD0"),
	CSTR("GE1_RXD1"),
	CSTR("GE1_RXD2"),
	CSTR("GE1_RXD3"),
	CSTR("GE1_RXDV"),
	CSTR("GE1_RXCLK"),
	CSTR("PERST_N"),
	CSTR("SPI_CS1"),
	CSTR("SPI_HOLD"),
	CSTR("SPI_WP"),

	CSTR("EPHY_LED0_N_JTDO"),
	CSTR("EPHY_LED1_N_JTDI"),
	CSTR("EPHY_LED2_N_JTMS"),
	CSTR("EPHY_LED3_N_JTCLK"),
	CSTR("EPHY_LED4_N_JTRST_N"),
	CSTR("ND_CS_N"),
	CSTR("ND_WE_N"),
	CSTR("ND_RE_N"),
	CSTR("ND_WP"),
	CSTR("ND_RB_N"),

	CSTR("ND_CLE"),
	CSTR("ND_ALE"),
	CSTR("ND_D0"),
	CSTR("ND_D1"),
	CSTR("ND_D2"),
	CSTR("ND_D3"),
	CSTR("ND_D4"),
	CSTR("ND_D5"),
	CSTR("ND_D6"),
	CSTR("ND_D7"),

	CSTR("GE2_TXD0"),
	CSTR("GE2_TXD1"),
	CSTR("GE2_TXD2"),
	CSTR("GE2_TXD3"),
	CSTR("GE2_TXEN"),
	CSTR("GE2_TXCLK"),
	CSTR("GE2_RXD0"),
	CSTR("GE2_RXD1"),
	CSTR("GE2_RXD2"),
	CSTR("GE2_RXD3"),

	CSTR("GE2_RXDV"),
	CSTR("GE2_RXCLK"),
	CSTR("WLAN_LED_N"),
};

Text::CString GPIOControl_Func1[] = {
	CSTR("Unknown"),
	CSTR("Input"),
	CSTR("Input"),
	CSTR("Input"),
	CSTR("Input"),
	CSTR("Input"),
	CSTR("Input"),
	CSTR("RTS_N"),
	CSTR("TXD"),
	CSTR("CTS_N"),

	CSTR("RXD"),
	CSTR("PCMFS"),
	CSTR("PCMCLK"),
	CSTR("PCMDRX"),
	CSTR("PCMDTX"),
	CSTR("Input"),
	CSTR("Input"),
	CSTR("REFCLK0_OUT"),
	CSTR("Input"),
	CSTR("Input"),

	CSTR("Input"),
	CSTR("Input"),
	CSTR("REFCLK1_OUT"),
	CSTR("REFCLK0_OUT"),
	CSTR("Input"),
	CSTR("Input"),
	CSTR("Input"),
	CSTR("Input"),
	CSTR("Input"),
	CSTR("Input"),

	CSTR("Input"),
	CSTR("Input"),
	CSTR("Input"),
	CSTR("Input"),
	CSTR("Input"),
	CSTR("Input"),
	CSTR("REFCLK0_OUT"),
	CSTR("Input"),
	CSTR("Input"),
	CSTR("Input"),

	CSTR("Input"),
	CSTR("Input"),
	CSTR("Input"),
	CSTR("Input"),
	CSTR("Input"),
	CSTR("Input"),
	CSTR("Input"),
	CSTR("BT_ACT"),
	CSTR("SD_WP"),
	CSTR("SD_CLK"),

	CSTR("SD_CARD_DETECT"),
	CSTR("SD_CMD"),
	CSTR("SD_D0"),
	CSTR("SD_D1"),
	CSTR("SD_D2"),
	CSTR("SD_D3"),
	CSTR("BT_STAT"),
	CSTR("BT_AUX"),
	CSTR("BT_WACT"),
	CSTR("BT_ANT"),

	CSTR("Input"),
	CSTR("Input"),
	CSTR("Input"),
	CSTR("Input"),
	CSTR("Input"),
	CSTR("Input"),
	CSTR("Input"),
	CSTR("Input"),
	CSTR("Input"),
	CSTR("Input"),

	CSTR("Input"),
	CSTR("Input"),
	CSTR("Input"),
};

Text::CString GPIOControl_Func2[] = {
	CSTR("Unknown"),
	CSTR("Input"),
	CSTR("Input"),
	CSTR("SPI_CS0"),
	CSTR("SPI_CLK"),
	CSTR("SPI_MOSI"),
	CSTR("SPI_MISO"),
	CSTR("I2SCLK"),
	CSTR("I2SWS"),
	CSTR("I2SSDO"),

	CSTR("I2SSDI"),
	CSTR("PCMFS"),
	CSTR("PCMCLK"),
	CSTR("PCMDRX"),
	CSTR("PCMDTX"),
	CSTR("Unknown"),
	CSTR("Unknown"),
	CSTR("Input"),
	CSTR("Unknown"),
	CSTR("Unknown"),

	CSTR("Unknown"),
	CSTR("Unknown"),
	CSTR("Input"),
	CSTR("Input"),
	CSTR("Unknown"),
	CSTR("Unknown"),
	CSTR("Unknown"),
	CSTR("Unknown"),
	CSTR("Unknown"),
	CSTR("Unknown"),

	CSTR("Unknown"),
	CSTR("Unknown"),
	CSTR("Unknown"),
	CSTR("Unknown"),
	CSTR("Unknown"),
	CSTR("Unknown"),
	CSTR("Input"),
	CSTR("REFCLK0_OUT/Input"),
	CSTR("Input"),
	CSTR("Input"),

	CSTR("Unknown"),
	CSTR("Unknown"),
	CSTR("Unknown"),
	CSTR("Unknown"),
	CSTR("Unknown"),
	CSTR("Input"),
	CSTR("Input"),
	CSTR("Input"),
	CSTR("Input"),
	CSTR("Input"),

	CSTR("Input"),
	CSTR("Input"),
	CSTR("Input"),
	CSTR("Input"),
	CSTR("Input"),
	CSTR("Input"),
	CSTR("Input"),
	CSTR("Input"),
	CSTR("Input"),
	CSTR("Input"),

	CSTR("Unknown"),
	CSTR("Unknown"),
	CSTR("Unknown"),
	CSTR("Unknown"),
	CSTR("Unknown"),
	CSTR("Unknown"),
	CSTR("Unknown"),
	CSTR("Unknown"),
	CSTR("Unknown"),
	CSTR("Unknown"),

	CSTR("Unknown"),
	CSTR("Unknown"),
	CSTR("Unknown"),
};

Text::CString IO::GPIOControl::PinModeGetName(UOSInt pinNum, UOSInt pinState)
{
	if (pinNum >= 73 || pinState > 8)
	{
		return CSTR("Unknown");
	}
	if (pinState == 8)
	{
		return CSTR("Output");
	}
	if (pinState == 0)
	{
		return GPIOControl_Func0[pinNum];
	}
	else if (pinState == 1)
	{
		return GPIOControl_Func1[pinNum];
	}
	else if (pinState == 2)
	{
		return GPIOControl_Func2[pinNum];
	}
	else
	{
		if (pinNum >= 7 && pinNum <= 14)
		{
			static Text::CString pinName3[] = {
				CSTR("I2SCLK"),
				CSTR("I2SWS"),
				CSTR("I2SSDO"),
				CSTR("I2SSDI"),
				CSTR("RTS_N"),
				CSTR("TXD"),
				CSTR("CTS_N"),
				CSTR("RXD"),
			};
			static Text::CString pinName4[] = {
				CSTR("Input"),
				CSTR("Input"),
				CSTR("Input"),
				CSTR("Input"),
				CSTR("PCMFS"),
				CSTR("PCMCLK"),
				CSTR("PCMDRX"),
				CSTR("PCMDTX"),
			};
			static Text::CString pinName5[] = {
				CSTR("RTS_N"),
				CSTR("TXD"),
				CSTR("CTS_N"),
				CSTR("RXD"),
				CSTR("Input"),
				CSTR("Input"),
				CSTR("Input"),
				CSTR("Input"),
			};
			static Text::CString pinName6[] = {
				CSTR("I2SCLK"),
				CSTR("I2SWS"),
				CSTR("I2SSDO"),
				CSTR("I2SSDI"),
				CSTR("Input"),
				CSTR("Input"),
				CSTR("Input"),
				CSTR("Input"),
			};
			if (pinState == 3)
			{
				return pinName3[pinNum - 7];
			}
			else if (pinState == 4)
			{
				return pinName4[pinNum - 7];
			}
			else if (pinState == 5)
			{
				return pinName5[pinNum - 7];
			}
			else if (pinState == 6)
			{
				return pinName6[pinNum - 7];
			}
			else if (pinState == 7)
			{
				return CSTR("Input");
			}
		}
		else if ((pinNum >= 3 && pinNum <= 6) || (pinNum >= 37 && pinNum <= 39))
		{
			if (pinState == 3)
			{
				return CSTR("Input");
			}
		}
	}
	return CSTR("Unknown");
}
