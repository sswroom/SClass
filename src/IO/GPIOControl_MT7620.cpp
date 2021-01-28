#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/GPIOControl.h"
#include "IO/PhysicalMem.h"
#include "Text/MyString.h"

// MTK MT7620
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
	clsData->gpioPtr = (volatile UInt32 *)clsData->mem->GetPointer() + 0x600;
	clsData->sysCtlPtr = (volatile UInt32 *)clsData->mem->GetPointer() + 0x000;
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
	return 73;
}

Bool IO::GPIOControl::IsPinHigh(UOSInt pinNum)
{
	if (pinNum < 24)
	{
		return (((ClassData*)this->clsData)->gpioPtr[8] & (1 << pinNum)) != 0;
	}
	else if (pinNum < 40)
	{
		return (((ClassData*)this->clsData)->gpioPtr[18] & (1 << (pinNum - 24))) != 0;
	}
	else if (pinNum < 72)
	{
		return (((ClassData*)this->clsData)->gpioPtr[28] & (1 << (pinNum - 40))) != 0;
	}
	else if (pinNum == 72)
	{
		return (((ClassData*)this->clsData)->gpioPtr[38] & 1) != 0;
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
		return (((ClassData*)this->clsData)->gpioPtr[9] & (1 << pinNum)) != 0;
	}
	else if (pinNum < 40)
	{
		return (((ClassData*)this->clsData)->gpioPtr[19] & (1 << (pinNum - 24))) != 0;
	}
	else if (pinNum < 72)
	{
		return (((ClassData*)this->clsData)->gpioPtr[29] & (1 << (pinNum - 40))) != 0;
	}
	else if (pinNum == 72)
	{
		return (((ClassData*)this->clsData)->gpioPtr[39] & 1) != 0;
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
		mode = (((ClassData*)this->clsData)->sysCtlPtr[18] >> 0) & 1;
		isGPIO = (mode == 1);
		break;
	case 3: //SPI
	case 4:
	case 5:
	case 6:
	case 37: //SPI
	case 38:
	case 39:
		mode = (((ClassData*)this->clsData)->sysCtlPtr[18] >> 11) & 3;
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
		mode = (((ClassData*)this->clsData)->sysCtlPtr[18] >> 2) & 7;
		isGPIO = (mode == 7) || (pinNum >= 7 && pinNum <= 10 && mode == 4) || (pinNum >= 11 && pinNum <= 14 && mode >= 5);
		break;
	case 15: //UARTL
	case 16:
		mode = (((ClassData*)this->clsData)->sysCtlPtr[18] >> 5) & 1;
		isGPIO = (mode == 1);
		break;
	case 17: //WDT_RST
		mode = (((ClassData*)this->clsData)->sysCtlPtr[18] >> 21) & 3;
		isGPIO = (mode == 2);
		break;
	case 18: //PA_PE
	case 19:
	case 20:
	case 21:
		mode = (((ClassData*)this->clsData)->sysCtlPtr[18] >> 20) & 1;
		isGPIO = (mode == 1);
		break;
	case 22: //MDIO
	case 23:
		mode = (((ClassData*)this->clsData)->sysCtlPtr[18] >> 7) & 3;
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
		mode = (((ClassData*)this->clsData)->sysCtlPtr[18] >> 9) & 1;
		isGPIO = (mode == 1);
		break;
	case 36: //PERST_N
		mode = (((ClassData*)this->clsData)->sysCtlPtr[18] >> 15) & 3;
		isGPIO = (mode == 2);
		break;
	case 40: //SW_PHY_LED/JTAG
	case 41:
	case 42:
	case 43:
	case 44:
		mode = (((ClassData*)this->clsData)->sysCtlPtr[18] >> 15) & 1;
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
		mode = (((ClassData*)this->clsData)->sysCtlPtr[18] >> 15) & 3;
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
		mode = (((ClassData*)this->clsData)->sysCtlPtr[18] >> 10) & 1;
		isGPIO = (mode == 1);
		break;
	case 72: //WLED_N
		mode = (((ClassData*)this->clsData)->sysCtlPtr[18] >> 13) & 1;
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
			((ClassData*)this->clsData)->gpioPtr[9] |= (1 << pinNum);
		}
		else if (pinNum < 40)
		{
			((ClassData*)this->clsData)->gpioPtr[19] |= (1 << (pinNum - 24));
		}
		else if (pinNum < 72)
		{
			((ClassData*)this->clsData)->gpioPtr[29] |= (1 << (pinNum - 40));
		}
		else if (pinNum == 72)
		{
			((ClassData*)this->clsData)->gpioPtr[39] |= 1;
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
			((ClassData*)this->clsData)->gpioPtr[9] &= ~(1 << pinNum);
		}
		else if (pinNum < 40)
		{
			((ClassData*)this->clsData)->gpioPtr[19] &= ~(1 << (pinNum - 24));
		}
		else if (pinNum < 72)
		{
			((ClassData*)this->clsData)->gpioPtr[29] &= ~(1 << (pinNum - 40));
		}
		else if (pinNum == 72)
		{
			((ClassData*)this->clsData)->gpioPtr[39] &= ~1;
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
			((ClassData*)this->clsData)->gpioPtr[11] |= (1 << pinNum);
		}
		else if (pinNum < 40)
		{
			((ClassData*)this->clsData)->gpioPtr[21] |= (1 << (pinNum - 24));
		}
		else if (pinNum < 72)
		{
			((ClassData*)this->clsData)->gpioPtr[31] |= (1 << (pinNum - 40));
		}
		else if (pinNum == 72)
		{
			((ClassData*)this->clsData)->gpioPtr[41] |= 1;
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
			((ClassData*)this->clsData)->gpioPtr[12] |= (1 << pinNum);
		}
		else if (pinNum < 40)
		{
			((ClassData*)this->clsData)->gpioPtr[22] |= (1 << (pinNum - 24));
		}
		else if (pinNum < 72)
		{
			((ClassData*)this->clsData)->gpioPtr[32] |= (1 << (pinNum - 40));
		}
		else if (pinNum == 72)
		{
			((ClassData*)this->clsData)->gpioPtr[42] |= 1;
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

const Char *GPIOControl_Func0[] = {
	"GPO0",
	"I2C_SD",
	"I2C_SCLK",
	"SPI_CS0",
	"SPI_CLK",
	"SPI_MOSI",
	"SPI_MISO",
	"RTS_N",
	"TXD",
	"CTS_N",

	"RXD",
	"DTR_N",
	"DCD_N",
	"DSR_N",
	"RIN",
	"TXD2",
	"RXD2",
	"WDT_RST_N",
	"ANT_TRNB",
	"ANT_TRN",

	"PA_PE_G0",
	"PA_PE_G1",
	"MDIO",
	"MDC",
	"GE1_TXD0",
	"GE1_TXD1",
	"GE1_TXD2",
	"GE1_TXD3",
	"GE1_TXEN",
	"GE1_TXCLK",

	"GE1_RXD0",
	"GE1_RXD1",
	"GE1_RXD2",
	"GE1_RXD3",
	"GE1_RXDV",
	"GE1_RXCLK",
	"PERST_N",
	"SPI_CS1",
	"SPI_HOLD",
	"SPI_WP",

	"EPHY_LED0_N_JTDO",
	"EPHY_LED1_N_JTDI",
	"EPHY_LED2_N_JTMS",
	"EPHY_LED3_N_JTCLK",
	"EPHY_LED4_N_JTRST_N",
	"ND_CS_N",
	"ND_WE_N",
	"ND_RE_N",
	"ND_WP",
	"ND_RB_N",

	"ND_CLE",
	"ND_ALE",
	"ND_D0",
	"ND_D1",
	"ND_D2",
	"ND_D3",
	"ND_D4",
	"ND_D5",
	"ND_D6",
	"ND_D7",

	"GE2_TXD0",
	"GE2_TXD1",
	"GE2_TXD2",
	"GE2_TXD3",
	"GE2_TXEN",
	"GE2_TXCLK",
	"GE2_RXD0",
	"GE2_RXD1",
	"GE2_RXD2",
	"GE2_RXD3",

	"GE2_RXDV",
	"GE2_RXCLK",
	"WLAN_LED_N",
};

const Char *GPIOControl_Func1[] = {
	"Unknown",
	"Input",
	"Input",
	"Input",
	"Input",
	"Input",
	"Input",
	"RTS_N",
	"TXD",
	"CTS_N",

	"RXD",
	"PCMFS",
	"PCMCLK",
	"PCMDRX",
	"PCMDTX",
	"Input",
	"Input",
	"REFCLK0_OUT",
	"Input",
	"Input",

	"Input",
	"Input",
	"REFCLK1_OUT",
	"REFCLK0_OUT",
	"Input",
	"Input",
	"Input",
	"Input",
	"Input",
	"Input",

	"Input",
	"Input",
	"Input",
	"Input",
	"Input",
	"Input",
	"REFCLK0_OUT",
	"Input",
	"Input",
	"Input",

	"Input",
	"Input",
	"Input",
	"Input",
	"Input",
	"Input",
	"Input",
	"BT_ACT",
	"SD_WP",
	"SD_CLK",

	"SD_CARD_DETECT",
	"SD_CMD",
	"SD_D0",
	"SD_D1",
	"SD_D2",
	"SD_D3",
	"BT_STAT",
	"BT_AUX",
	"BT_WACT",
	"BT_ANT",

	"Input",
	"Input",
	"Input",
	"Input",
	"Input",
	"Input",
	"Input",
	"Input",
	"Input",
	"Input",

	"Input",
	"Input",
	"Input",
};

const Char *GPIOControl_Func2[] = {
	"Unknown",
	"Input",
	"Input",
	"SPI_CS0",
	"SPI_CLK",
	"SPI_MOSI",
	"SPI_MISO",
	"I2SCLK",
	"I2SWS",
	"I2SSDO",

	"I2SSDI",
	"PCMFS",
	"PCMCLK",
	"PCMDRX",
	"PCMDTX",
	"Unknown",
	"Unknown",
	"Input",
	"Unknown",
	"Unknown",

	"Unknown",
	"Unknown",
	"Input",
	"Input",
	"Unknown",
	"Unknown",
	"Unknown",
	"Unknown",
	"Unknown",
	"Unknown",

	"Unknown",
	"Unknown",
	"Unknown",
	"Unknown",
	"Unknown",
	"Unknown",
	"Input",
	"REFCLK0_OUT/Input",
	"Input",
	"Input",

	"Unknown",
	"Unknown",
	"Unknown",
	"Unknown",
	"Unknown",
	"Input",
	"Input",
	"Input",
	"Input",
	"Input",

	"Input",
	"Input",
	"Input",
	"Input",
	"Input",
	"Input",
	"Input",
	"Input",
	"Input",
	"Input",

	"Unknown",
	"Unknown",
	"Unknown",
	"Unknown",
	"Unknown",
	"Unknown",
	"Unknown",
	"Unknown",
	"Unknown",
	"Unknown",

	"Unknown",
	"Unknown",
	"Unknown",
};

const UTF8Char *IO::GPIOControl::PinModeGetName(UOSInt pinNum, UOSInt pinState)
{
	if (pinNum >= 73 || pinState > 8)
	{
		return (const UTF8Char*)"Unknown";
	}
	if (pinState == 8)
	{
		return (const UTF8Char*)"Output";
	}
	if (pinState == 0)
	{
		return (const UTF8Char*)GPIOControl_Func0[pinNum];
	}
	else if (pinState == 1)
	{
		return (const UTF8Char*)GPIOControl_Func1[pinNum];
	}
	else if (pinState == 2)
	{
		return (const UTF8Char*)GPIOControl_Func2[pinNum];
	}
	else
	{
		if (pinNum >= 7 && pinNum <= 14)
		{
			static const Char *pinName3[] = {
				"I2SCLK",
				"I2SWS",
				"I2SSDO",
				"I2SSDI",
				"RTS_N",
				"TXD",
				"CTS_N",
				"RXD",
			};
			static const Char *pinName4[] = {
				"Input",
				"Input",
				"Input",
				"Input",
				"PCMFS",
				"PCMCLK",
				"PCMDRX",
				"PCMDTX",
			};
			static const Char *pinName5[] = {
				"RTS_N",
				"TXD",
				"CTS_N",
				"RXD",
				"Input",
				"Input",
				"Input",
				"Input",
			};
			static const Char *pinName6[] = {
				"I2SCLK",
				"I2SWS",
				"I2SSDO",
				"I2SSDI",
				"Input",
				"Input",
				"Input",
				"Input",
			};
			if (pinState == 3)
			{
				return (const UTF8Char*)pinName3[pinNum - 7];
			}
			else if (pinState == 4)
			{
				return (const UTF8Char*)pinName4[pinNum - 7];
			}
			else if (pinState == 5)
			{
				return (const UTF8Char*)pinName5[pinNum - 7];
			}
			else if (pinState == 6)
			{
				return (const UTF8Char*)pinName6[pinNum - 7];
			}
			else if (pinState == 7)
			{
				return (const UTF8Char*)"Input";
			}
		}
		else if ((pinNum >= 3 && pinNum <= 6) || (pinNum >= 37 && pinNum <= 39))
		{
			if (pinState == 3)
			{
				return (const UTF8Char *)"Input";
			}
		}
	}
	return (const UTF8Char*)"Unknown";
}
