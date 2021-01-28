#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/GPIOControl.h"
#include "IO/PhysicalMem.h"
#include "Sync/Interlocked.h"
#include "Sync/Thread.h"
#include "Text/MyString.h"

// BCM2708 (BCM2835)
#define IO_BASE_ADDR 0x20000000
#define BLOCKSIZE 4096

typedef struct
{
	IO::PhysicalMem *mem;
	volatile UInt32 *memPtr;
} ClassData;

IO::GPIOControl::GPIOControl()
{
	ClassData *clsData = MemAlloc(ClassData, 1);
	NEW_CLASS(clsData->mem, IO::PhysicalMem(IO_BASE_ADDR + 0x200000, BLOCKSIZE));
	clsData->memPtr = (volatile UInt32 *)clsData->mem->GetPointer();
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
	return 54;
}

Bool IO::GPIOControl::IsPinHigh(UOSInt pinNum)
{
	if (pinNum >= 54)
		return false;
	if (pinNum < 32)
	{
		return (((ClassData*)this->clsData)->memPtr[13] & (1 << pinNum)) != 0;
	}
	else
	{
		return (((ClassData*)this->clsData)->memPtr[14] & (1 << (pinNum - 32))) != 0;
	}
}

Bool IO::GPIOControl::IsPinOutput(UOSInt pinNum)
{
	return this->GetPinMode(pinNum) == 1;
}

UOSInt IO::GPIOControl::GetPinMode(UOSInt pinNum)
{
	if (pinNum < 10)
	{
		return ((((ClassData*)this->clsData)->memPtr[0] >> (3 * pinNum)) & 7);
	}
	else if (pinNum < 20)
	{
		return ((((ClassData*)this->clsData)->memPtr[1] >> (3 * (pinNum - 10))) & 7);
	}
	else if (pinNum < 30)
	{
		return ((((ClassData*)this->clsData)->memPtr[2] >> (3 * (pinNum - 20))) & 7);
	}
	else if (pinNum < 40)
	{
		return ((((ClassData*)this->clsData)->memPtr[3] >> (3 * (pinNum - 30))) & 7);
	}
	else if (pinNum < 50)
	{
		return ((((ClassData*)this->clsData)->memPtr[4] >> (3 * (pinNum - 40))) & 7);
	}
	else if (pinNum < 54)
	{
		return ((((ClassData*)this->clsData)->memPtr[5] >> (3 * (pinNum - 50))) & 7);
	}

	return false;
}

Bool IO::GPIOControl::SetPinOutput(UOSInt pinNum, Bool isOutput)
{
	UInt32 val = isOutput?1:0;
	switch (pinNum)
	{
	case 0:
		((ClassData*)this->clsData)->memPtr[0] = (((ClassData*)this->clsData)->memPtr[0] & ~7) | val;
		break;
	case 1:
		((ClassData*)this->clsData)->memPtr[0] = (((ClassData*)this->clsData)->memPtr[0] & ~(7 << 3)) | (val << 3);
		break;
	case 2:
		((ClassData*)this->clsData)->memPtr[0] = (((ClassData*)this->clsData)->memPtr[0] & ~(7 << 6)) | (val << 6);
		break;
	case 3:
		((ClassData*)this->clsData)->memPtr[0] = (((ClassData*)this->clsData)->memPtr[0] & ~(7 << 9)) | (val << 9);
		break;
	case 4:
		((ClassData*)this->clsData)->memPtr[0] = (((ClassData*)this->clsData)->memPtr[0] & ~(7 << 12)) | (val << 12);
		break;
	case 5:
		((ClassData*)this->clsData)->memPtr[0] = (((ClassData*)this->clsData)->memPtr[0] & ~(7 << 15)) | (val << 15);
		break;
	case 6:
		((ClassData*)this->clsData)->memPtr[0] = (((ClassData*)this->clsData)->memPtr[0] & ~(7 << 18)) | (val << 18);
		break;
	case 7:
		((ClassData*)this->clsData)->memPtr[0] = (((ClassData*)this->clsData)->memPtr[0] & ~(7 << 21)) | (val << 21);
		break;
	case 8:
		((ClassData*)this->clsData)->memPtr[0] = (((ClassData*)this->clsData)->memPtr[0] & ~(7 << 24)) | (val << 24);
		break;
	case 9:
		((ClassData*)this->clsData)->memPtr[0] = (((ClassData*)this->clsData)->memPtr[0] & ~(7 << 27)) | (val << 27);
		break;

	case 10:
		((ClassData*)this->clsData)->memPtr[1] = (((ClassData*)this->clsData)->memPtr[1] & ~7) | val;
		break;
	case 11:
		((ClassData*)this->clsData)->memPtr[1] = (((ClassData*)this->clsData)->memPtr[1] & ~(7 << 3)) | (val << 3);
		break;
	case 12:
		((ClassData*)this->clsData)->memPtr[1] = (((ClassData*)this->clsData)->memPtr[1] & ~(7 << 6)) | (val << 6);
		break;
	case 13:
		((ClassData*)this->clsData)->memPtr[1] = (((ClassData*)this->clsData)->memPtr[1] & ~(7 << 9)) | (val << 9);
		break;
	case 14:
		((ClassData*)this->clsData)->memPtr[1] = (((ClassData*)this->clsData)->memPtr[1] & ~(7 << 12)) | (val << 12);
		break;
	case 15:
		((ClassData*)this->clsData)->memPtr[1] = (((ClassData*)this->clsData)->memPtr[1] & ~(7 << 15)) | (val << 15);
		break;
	case 16:
		((ClassData*)this->clsData)->memPtr[1] = (((ClassData*)this->clsData)->memPtr[1] & ~(7 << 18)) | (val << 18);
		break;
	case 17:
		((ClassData*)this->clsData)->memPtr[1] = (((ClassData*)this->clsData)->memPtr[1] & ~(7 << 21)) | (val << 21);
		break;
	case 18:
		((ClassData*)this->clsData)->memPtr[1] = (((ClassData*)this->clsData)->memPtr[1] & ~(7 << 24)) | (val << 24);
		break;
	case 19:
		((ClassData*)this->clsData)->memPtr[1] = (((ClassData*)this->clsData)->memPtr[1] & ~(7 << 27)) | (val << 27);
		break;

	case 20:
		((ClassData*)this->clsData)->memPtr[2] = (((ClassData*)this->clsData)->memPtr[2] & ~7) | val;
		break;
	case 21:
		((ClassData*)this->clsData)->memPtr[2] = (((ClassData*)this->clsData)->memPtr[2] & ~(7 << 3)) | (val << 3);
		break;
	case 22:
		((ClassData*)this->clsData)->memPtr[2] = (((ClassData*)this->clsData)->memPtr[2] & ~(7 << 6)) | (val << 6);
		break;
	case 23:
		((ClassData*)this->clsData)->memPtr[2] = (((ClassData*)this->clsData)->memPtr[2] & ~(7 << 9)) | (val << 9);
		break;
	case 24:
		((ClassData*)this->clsData)->memPtr[2] = (((ClassData*)this->clsData)->memPtr[2] & ~(7 << 12)) | (val << 12);
		break;
	case 25:
		((ClassData*)this->clsData)->memPtr[2] = (((ClassData*)this->clsData)->memPtr[2] & ~(7 << 15)) | (val << 15);
		break;
	case 26:
		((ClassData*)this->clsData)->memPtr[2] = (((ClassData*)this->clsData)->memPtr[2] & ~(7 << 18)) | (val << 18);
		break;
	case 27:
		((ClassData*)this->clsData)->memPtr[2] = (((ClassData*)this->clsData)->memPtr[2] & ~(7 << 21)) | (val << 21);
		break;
	case 28:
		((ClassData*)this->clsData)->memPtr[2] = (((ClassData*)this->clsData)->memPtr[2] & ~(7 << 24)) | (val << 24);
		break;
	case 29:
		((ClassData*)this->clsData)->memPtr[2] = (((ClassData*)this->clsData)->memPtr[2] & ~(7 << 27)) | (val << 27);
		break;

	case 30:
		((ClassData*)this->clsData)->memPtr[3] = (((ClassData*)this->clsData)->memPtr[3] & ~7) | val;
		break;
	case 31:
		((ClassData*)this->clsData)->memPtr[3] = (((ClassData*)this->clsData)->memPtr[3] & ~(7 << 3)) | (val << 3);
		break;
	case 32:
		((ClassData*)this->clsData)->memPtr[3] = (((ClassData*)this->clsData)->memPtr[3] & ~(7 << 6)) | (val << 6);
		break;
	case 33:
		((ClassData*)this->clsData)->memPtr[3] = (((ClassData*)this->clsData)->memPtr[3] & ~(7 << 9)) | (val << 9);
		break;
	case 34:
		((ClassData*)this->clsData)->memPtr[3] = (((ClassData*)this->clsData)->memPtr[3] & ~(7 << 12)) | (val << 12);
		break;
	case 35:
		((ClassData*)this->clsData)->memPtr[3] = (((ClassData*)this->clsData)->memPtr[3] & ~(7 << 15)) | (val << 15);
		break;
	case 36:
		((ClassData*)this->clsData)->memPtr[3] = (((ClassData*)this->clsData)->memPtr[3] & ~(7 << 18)) | (val << 18);
		break;
	case 37:
		((ClassData*)this->clsData)->memPtr[3] = (((ClassData*)this->clsData)->memPtr[3] & ~(7 << 21)) | (val << 21);
		break;
	case 38:
		((ClassData*)this->clsData)->memPtr[3] = (((ClassData*)this->clsData)->memPtr[3] & ~(7 << 24)) | (val << 24);
		break;
	case 39:
		((ClassData*)this->clsData)->memPtr[3] = (((ClassData*)this->clsData)->memPtr[3] & ~(7 << 27)) | (val << 27);
		break;

	case 40:
		((ClassData*)this->clsData)->memPtr[4] = (((ClassData*)this->clsData)->memPtr[4] & ~7) | val;
		break;
	case 41:
		((ClassData*)this->clsData)->memPtr[4] = (((ClassData*)this->clsData)->memPtr[4] & ~(7 << 3)) | (val << 3);
		break;
	case 42:
		((ClassData*)this->clsData)->memPtr[4] = (((ClassData*)this->clsData)->memPtr[4] & ~(7 << 6)) | (val << 6);
		break;
	case 43:
		((ClassData*)this->clsData)->memPtr[4] = (((ClassData*)this->clsData)->memPtr[4] & ~(7 << 9)) | (val << 9);
		break;
	case 44:
		((ClassData*)this->clsData)->memPtr[4] = (((ClassData*)this->clsData)->memPtr[4] & ~(7 << 12)) | (val << 12);
		break;
	case 45:
		((ClassData*)this->clsData)->memPtr[4] = (((ClassData*)this->clsData)->memPtr[4] & ~(7 << 15)) | (val << 15);
		break;
	case 46:
		((ClassData*)this->clsData)->memPtr[4] = (((ClassData*)this->clsData)->memPtr[4] & ~(7 << 18)) | (val << 18);
		break;
	case 47:
		((ClassData*)this->clsData)->memPtr[4] = (((ClassData*)this->clsData)->memPtr[4] & ~(7 << 21)) | (val << 21);
		break;
	case 48:
		((ClassData*)this->clsData)->memPtr[4] = (((ClassData*)this->clsData)->memPtr[4] & ~(7 << 24)) | (val << 24);
		break;
	case 49:
		((ClassData*)this->clsData)->memPtr[4] = (((ClassData*)this->clsData)->memPtr[4] & ~(7 << 27)) | (val << 27);
		break;

	case 50:
		((ClassData*)this->clsData)->memPtr[5] = (((ClassData*)this->clsData)->memPtr[5] & ~7) | val;
		break;
	case 51:
		((ClassData*)this->clsData)->memPtr[5] = (((ClassData*)this->clsData)->memPtr[5] & ~(7 << 3)) | (val << 3);
		break;
	case 52:
		((ClassData*)this->clsData)->memPtr[5] = (((ClassData*)this->clsData)->memPtr[5] & ~(7 << 6)) | (val << 6);
		break;
	case 53:
		((ClassData*)this->clsData)->memPtr[5] = (((ClassData*)this->clsData)->memPtr[5] & ~(7 << 9)) | (val << 9);
		break;
	default:
		return false;
	}
	return true;
}

Bool IO::GPIOControl::SetPinState(UOSInt pinNum, Bool isHigh)
{
	if (pinNum >= 54)
		return false;
	if (isHigh)
	{
		if (pinNum < 32)
		{
			((ClassData*)this->clsData)->memPtr[7] = 1 << pinNum;
		}
		else
		{
			((ClassData*)this->clsData)->memPtr[8] = 1 << (pinNum - 32);
		}
	}
	else
	{
		if (pinNum < 32)
		{
			((ClassData*)this->clsData)->memPtr[10] = 1 << pinNum;
		}
		else
		{
			((ClassData*)this->clsData)->memPtr[11] = 1 << (pinNum - 32);
		}
	}
	return true;
}

Bool IO::GPIOControl::SetPullType(UOSInt pinNum, IO::IOPin::PullType pt)
{
	if (pinNum >= 54)
		return false;
	if (pt == IO::IOPin::PT_DISABLE)
	{
		((ClassData*)this->clsData)->memPtr[37] = 0;
	}
	else if (pt == IO::IOPin::PT_DOWN)
	{
		((ClassData*)this->clsData)->memPtr[37] = 1;
	}
	else if (pt == IO::IOPin::PT_UP)
	{
		((ClassData*)this->clsData)->memPtr[37] = 2;
	}
	else
	{
		return false;
	}
	Sync::Thread::Sleepus(1);
	if (pinNum < 32)
	{
		((ClassData*)this->clsData)->memPtr[38] = (1 << pinNum);
	}
	else
	{
		((ClassData*)this->clsData)->memPtr[39] = (1 << (pinNum - 32));
	}
	Sync::Thread::Sleepus(1);
	if (pinNum < 32)
	{
		((ClassData*)this->clsData)->memPtr[38] = 0;
	}
	else
	{
		((ClassData*)this->clsData)->memPtr[39] = 0;
	}
	return true;
}

void IO::GPIOControl::SetEventOnHigh(UOSInt pinNum, Bool enable)
{
	if (pinNum >= 54)
		return;
	if (pinNum < 32)
	{
		if (enable)
		{
			((ClassData*)this->clsData)->memPtr[25] |= (1 << pinNum);
		}
		else
		{
			((ClassData*)this->clsData)->memPtr[25] &= ~(1 << pinNum);
		}
	}
	else
	{
		if (enable)
		{
			((ClassData*)this->clsData)->memPtr[26] |= (1 << (pinNum - 32));
		}
		else
		{
			((ClassData*)this->clsData)->memPtr[26] &= ~(1 << (pinNum - 32));
		}
	}
}

void IO::GPIOControl::SetEventOnLow(UOSInt pinNum, Bool enable)
{
	if (pinNum >= 54)
		return;
	if (pinNum < 32)
	{
		if (enable)
		{
			((ClassData*)this->clsData)->memPtr[28] |= (1 << pinNum);
		}
		else
		{
			((ClassData*)this->clsData)->memPtr[28] &= ~(1 << pinNum);
		}
	}
	else
	{
		if (enable)
		{
			((ClassData*)this->clsData)->memPtr[29] |= (1 << (pinNum - 32));
		}
		else
		{
			((ClassData*)this->clsData)->memPtr[29] &= ~(1 << (pinNum - 32));
		}
	}
}

void IO::GPIOControl::SetEventOnRaise(UOSInt pinNum, Bool enable)
{
	if (pinNum >= 54)
		return;
	if (pinNum < 32)
	{
		if (enable)
		{
			((ClassData*)this->clsData)->memPtr[19] |= (1 << pinNum);
		}
		else
		{
			((ClassData*)this->clsData)->memPtr[19] &= ~(1 << pinNum);
		}
	}
	else
	{
		if (enable)
		{
			((ClassData*)this->clsData)->memPtr[20] |= (1 << (pinNum - 32));
		}
		else
		{
			((ClassData*)this->clsData)->memPtr[20] &= ~(1 << (pinNum - 32));
		}
	}
}

void IO::GPIOControl::SetEventOnFall(UOSInt pinNum, Bool enable)
{
	if (pinNum >= 54)
		return;
	if (pinNum < 32)
	{
		if (enable)
		{
			((ClassData*)this->clsData)->memPtr[22] |= (1 << pinNum);
		}
		else
		{
			((ClassData*)this->clsData)->memPtr[22] &= ~(1 << pinNum);
		}
	}
	else
	{
		if (enable)
		{
			((ClassData*)this->clsData)->memPtr[23] |= (1 << (pinNum - 32));
		}
		else
		{
			((ClassData*)this->clsData)->memPtr[23] &= ~(1 << (pinNum - 32));
		}
	}
}

Bool IO::GPIOControl::HasEvent(UOSInt pinNum)
{
	if (pinNum >= 54)
		return false;
	if (pinNum < 32)
	{
		return (((ClassData*)this->clsData)->memPtr[16] & (1 << pinNum)) != 0;
	}
	else
	{
		return (((ClassData*)this->clsData)->memPtr[17] & (1 << (pinNum - 32))) != 0;
	}
}

void IO::GPIOControl::ClearEvent(UOSInt pinNum)
{
	if (pinNum >= 54)
		return;
	if (pinNum < 32)
	{
		((ClassData*)this->clsData)->memPtr[16] &= ~(1 << pinNum);
	}
	else
	{
		((ClassData*)this->clsData)->memPtr[17] &= ~(1 << (pinNum - 32));
	}
}

static const Char *GPIOControl_ALT0[] = {
	"SDA0",
	"SCL0",
	"SDA1",
	"SCL1",
	"GPCLK0",
	"GPCLK1",
	"GPCLK2",
	"SPI0_CE1_N",
	"SPI0_CE0_N",
	"SPI0_MISO",

	"SPI0_MOSI",
	"SPI0_SCLK",
	"PWM0",
	"PWM1",
	"TXD0",
	"RXD0",
	"Reserved",
	"Reserved",
	"PCM_CLK",
	"PCM_FS",

	"PCM_DIN",
	"PCM_DOUT",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"SDA0",
	"SCL0",

	"Reserved",
	"Reserved",
	"GPCLK0",
	"Reserved",
	"GPCLK0",
	"SPI0_CE1_N",
	"SPI0_CE0_N",
	"SPI0_MISO",
	"SPI0_MOSI",
	"SPI0_SCLK",

	"PWM0",
	"PWM1",
	"GPCLK1",
	"GPCLK2",
	"GPCLK1",
	"PWM1",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",

	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved"
};

static const Char *GPIOControl_ALT1[] = {
	"SA5",
	"SA4",
	"SA3",
	"SA2",
	"SA1",
	"SA0",
	"SOE_N/SE",
	"SWE_N/SRW_N",
	"SD0",
	"SD1",

	"SD2",
	"SD3",
	"SD4",
	"SD5",
	"SD6",
	"SD7",
	"SD8",
	"SD9",
	"SD10",
	"SD11",

	"SD12",
	"SD13",
	"SD14",
	"SD15",
	"SD16",
	"SD17",
	"Reserved",
	"Reserved",
	"SA5",
	"SA4",

	"SA3",
	"SA2",
	"SA1",
	"SA0",
	"SOE_N/SE",
	"SWE_N/SRW_N",
	"SD0",
	"SD1",
	"SD2",
	"SD3",

	"SD4",
	"SD5",
	"SD6",
	"SD7",
	"SDA0",
	"SCL0",
	"Unknown",
	"Unknown",
	"Unknown",
	"Unknown",

	"Unknown",
	"Unknown",
	"Unknown",
	"Unknown"
};

static const Char *GPIOControl_ALT2[] = {
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",

	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",

	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"PCM_CLK",
	"PCM_FS",

	"PCM_DIN",
	"PCM_DOUT",
	"Reserved",
	"Reserved",
	"Reserved",
	"Unknown",
	"TXD0",
	"RXD0",
	"RTS0",
	"CTS0",

	"Unknown",
	"Reserved",
	"Reserved",
	"Reserved",
	"SDA1",
	"SCL1",
	"Unknown",
	"Unknown",
	"Unknown",
	"Unknown",

	"Unknown",
	"Unknown",
	"Unknown",
	"Unknown"
};

static const Char *GPIOControl_ALT3[] = {
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
	"Unknown",
	"Unknown",
	"Unknown",
	"CTS0",
	"RTS0",
	"BSCSL SDA/MOSI",
	"BSCSL SCL/SCLK",

	"BSCSL/MISO",
	"BSCSL/CE_N",
	"SD1_CLK",
	"SD1_CMD",
	"SD1_DAT0",
	"SD1_DAT1",
	"SD1_DAT2",
	"SD1_DAT3",
	"Reserved",
	"Reserved",

	"CTS0",
	"RTS0",
	"TXD0",
	"RXD0",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",

	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Unknown",
	"Unknown",
	"Unknown",
	"Unknown",

	"Unknown",
	"Unknown",
	"Unknown",
	"Unknown"
};

static const Char *GPIOControl_ALT4[] = {
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
	"Unknown",
	"Unknown",
	"Unknown",
	"SPI1_CE2_N",
	"SPI1_CE1_N",
	"SPI1_CE0_N",
	"SPI1_MISO",

	"SPI1_MOSI",
	"SPI1_SCLK",
	"ARM_TRST",
	"ARM_RTCK",
	"ARM_TDO",
	"ARM_TCK",
	"ARM_TDI",
	"ARM_TMS",
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

	"SPI2_MISO",
	"SPI2_MOSI",
	"SPI2_SCLK",
	"SPI2_CE0_N",
	"SPI2_CE1_N",
	"SPI2_CE2_N",
	"Unknown",
	"Unknown",
	"Unknown",
	"Unknown",

	"Unknown",
	"Unknown",
	"Unknown",
	"Unknown"
};

static const Char *GPIOControl_ALT5[] = {
	"Unknown",
	"Unknown",
	"Unknown",
	"Unknown",
	"ARM_TDI",
	"ARM_TDO",
	"ARM_RTCK",
	"Unknown",
	"Unknown",
	"Unknown",

	"Unknown",
	"Unknown",
	"ARM_TMS",
	"ARM_TCK",
	"TXD1",
	"RXD1",
	"CTS1",
	"RTS1",
	"PWM0",
	"PWM1",

	"GPCLK0",
	"GPCLK1",
	"Unknown",
	"Unknown",
	"Unknown",
	"Unknown",
	"Unknown",
	"Unknown",
	"Unknown",
	"Unknown",

	"CTS1",
	"RTS1",
	"TXD1",
	"RXD1",
	"Unknown",
	"Unknown",
	"Unknown",
	"Unknown",
	"Unknown",
	"Unknown",

	"TXD1",
	"RXD1",
	"RTS1",
	"CTS1",
	"Unknown",
	"Unknown",
	"Unknown",
	"Unknown",
	"Unknown",
	"Unknown",

	"Unknown",
	"Unknown",
	"Unknown",
	"Unknown"
};

const UTF8Char *IO::GPIOControl::PinModeGetName(UOSInt pinNum, UOSInt pinMode)
{
	if (pinMode == 0)
	{
		return (const UTF8Char*)"Input";
	}
	else if (pinMode == 1)
	{
		return (const UTF8Char*)"Output";
	}
	if (pinNum >= 54)
	{
		return (const UTF8Char*)"Unknown";
	}
	if (pinMode == 4)
	{
		return (const UTF8Char*)GPIOControl_ALT0[pinNum];
	}
	else if (pinMode == 5)
	{
		return (const UTF8Char*)GPIOControl_ALT1[pinNum];
	}
	else if (pinMode == 6)
	{
		return (const UTF8Char*)GPIOControl_ALT2[pinNum];
	}
	else if (pinMode == 7)
	{
		return (const UTF8Char*)GPIOControl_ALT3[pinNum];
	}
	else if (pinMode == 3)
	{
		return (const UTF8Char*)GPIOControl_ALT4[pinNum];
	}
	else if (pinMode == 2)
	{
		return (const UTF8Char*)GPIOControl_ALT5[pinNum];
	}
	return (const UTF8Char*)"Unknown";
}
