#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/GPIOControl.h"
#include "IO/PhysicalMem.h"
#include "Sync/Interlocked.h"
#include "Sync/ThreadUtil.h"
#include "Text/MyString.h"

// BCM2709 (BCM2836 BCM2837)
#define IO_BASE_ADDR 0x3F000000
#define BLOCKSIZE 4096

struct IO::GPIOControl::ClassData
{
	IO::PhysicalMem *mem;
	volatile UInt32 *memPtr;
};

IO::GPIOControl::GPIOControl()
{
	ClassData *clsData = MemAlloc(ClassData, 1);
	NEW_CLASS(clsData->mem, IO::PhysicalMem(IO_BASE_ADDR + 0x200000, BLOCKSIZE));
	clsData->memPtr = (volatile UInt32 *)clsData->mem->GetPointer();
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
	return 54;
}

Bool IO::GPIOControl::IsPinHigh(UOSInt pinNum)
{
	if (pinNum >= 54)
	{
		return false;
	}
	if (pinNum < 32)
	{
		return (this->clsData->memPtr[13] & (1 << pinNum)) != 0;
	}
	else
	{
		return (this->clsData->memPtr[14] & (1 << (pinNum - 32))) != 0;
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
		return ((this->clsData->memPtr[0] >> (3 * pinNum)) & 7);
	}
	else if (pinNum < 20)
	{
		return ((this->clsData->memPtr[1] >> (3 * (pinNum - 10))) & 7);
	}
	else if (pinNum < 30)
	{
		return ((this->clsData->memPtr[2] >> (3 * (pinNum - 20))) & 7);
	}
	else if (pinNum < 40)
	{
		return ((this->clsData->memPtr[3] >> (3 * (pinNum - 30))) & 7);
	}
	else if (pinNum < 50)
	{
		return ((this->clsData->memPtr[4] >> (3 * (pinNum - 40))) & 7);
	}
	else if (pinNum < 54)
	{
		return ((this->clsData->memPtr[5] >> (3 * (pinNum - 50))) & 7);
	}

	return false;
}

Bool IO::GPIOControl::SetPinOutput(UOSInt pinNum, Bool isOutput)
{
	UInt32 val = isOutput?1:0;
	switch (pinNum)
	{
	case 0:
		this->clsData->memPtr[0] = (this->clsData->memPtr[0] & ~7) | val;
		break;
	case 1:
		this->clsData->memPtr[0] = (this->clsData->memPtr[0] & ~(7 << 3)) | (val << 3);
		break;
	case 2:
		this->clsData->memPtr[0] = (this->clsData->memPtr[0] & ~(7 << 6)) | (val << 6);
		break;
	case 3:
		this->clsData->memPtr[0] = (this->clsData->memPtr[0] & ~(7 << 9)) | (val << 9);
		break;
	case 4:
		this->clsData->memPtr[0] = (this->clsData->memPtr[0] & ~(7 << 12)) | (val << 12);
		break;
	case 5:
		this->clsData->memPtr[0] = (this->clsData->memPtr[0] & ~(7 << 15)) | (val << 15);
		break;
	case 6:
		this->clsData->memPtr[0] = (this->clsData->memPtr[0] & ~(7 << 18)) | (val << 18);
		break;
	case 7:
		this->clsData->memPtr[0] = (this->clsData->memPtr[0] & ~(7 << 21)) | (val << 21);
		break;
	case 8:
		this->clsData->memPtr[0] = (this->clsData->memPtr[0] & ~(7 << 24)) | (val << 24);
		break;
	case 9:
		this->clsData->memPtr[0] = (this->clsData->memPtr[0] & ~(7 << 27)) | (val << 27);
		break;

	case 10:
		this->clsData->memPtr[1] = (this->clsData->memPtr[1] & ~7) | val;
		break;
	case 11:
		this->clsData->memPtr[1] = (this->clsData->memPtr[1] & ~(7 << 3)) | (val << 3);
		break;
	case 12:
		this->clsData->memPtr[1] = (this->clsData->memPtr[1] & ~(7 << 6)) | (val << 6);
		break;
	case 13:
		this->clsData->memPtr[1] = (this->clsData->memPtr[1] & ~(7 << 9)) | (val << 9);
		break;
	case 14:
		this->clsData->memPtr[1] = (this->clsData->memPtr[1] & ~(7 << 12)) | (val << 12);
		break;
	case 15:
		this->clsData->memPtr[1] = (this->clsData->memPtr[1] & ~(7 << 15)) | (val << 15);
		break;
	case 16:
		this->clsData->memPtr[1] = (this->clsData->memPtr[1] & ~(7 << 18)) | (val << 18);
		break;
	case 17:
		this->clsData->memPtr[1] = (this->clsData->memPtr[1] & ~(7 << 21)) | (val << 21);
		break;
	case 18:
		this->clsData->memPtr[1] = (this->clsData->memPtr[1] & ~(7 << 24)) | (val << 24);
		break;
	case 19:
		this->clsData->memPtr[1] = (this->clsData->memPtr[1] & ~(7 << 27)) | (val << 27);
		break;

	case 20:
		this->clsData->memPtr[2] = (this->clsData->memPtr[2] & ~7) | val;
		break;
	case 21:
		this->clsData->memPtr[2] = (this->clsData->memPtr[2] & ~(7 << 3)) | (val << 3);
		break;
	case 22:
		this->clsData->memPtr[2] = (this->clsData->memPtr[2] & ~(7 << 6)) | (val << 6);
		break;
	case 23:
		this->clsData->memPtr[2] = (this->clsData->memPtr[2] & ~(7 << 9)) | (val << 9);
		break;
	case 24:
		this->clsData->memPtr[2] = (this->clsData->memPtr[2] & ~(7 << 12)) | (val << 12);
		break;
	case 25:
		this->clsData->memPtr[2] = (this->clsData->memPtr[2] & ~(7 << 15)) | (val << 15);
		break;
	case 26:
		this->clsData->memPtr[2] = (this->clsData->memPtr[2] & ~(7 << 18)) | (val << 18);
		break;
	case 27:
		this->clsData->memPtr[2] = (this->clsData->memPtr[2] & ~(7 << 21)) | (val << 21);
		break;
	case 28:
		this->clsData->memPtr[2] = (this->clsData->memPtr[2] & ~(7 << 24)) | (val << 24);
		break;
	case 29:
		this->clsData->memPtr[2] = (this->clsData->memPtr[2] & ~(7 << 27)) | (val << 27);
		break;

	case 30:
		this->clsData->memPtr[3] = (this->clsData->memPtr[3] & ~7) | val;
		break;
	case 31:
		this->clsData->memPtr[3] = (this->clsData->memPtr[3] & ~(7 << 3)) | (val << 3);
		break;
	case 32:
		this->clsData->memPtr[3] = (this->clsData->memPtr[3] & ~(7 << 6)) | (val << 6);
		break;
	case 33:
		this->clsData->memPtr[3] = (this->clsData->memPtr[3] & ~(7 << 9)) | (val << 9);
		break;
	case 34:
		this->clsData->memPtr[3] = (this->clsData->memPtr[3] & ~(7 << 12)) | (val << 12);
		break;
	case 35:
		this->clsData->memPtr[3] = (this->clsData->memPtr[3] & ~(7 << 15)) | (val << 15);
		break;
	case 36:
		this->clsData->memPtr[3] = (this->clsData->memPtr[3] & ~(7 << 18)) | (val << 18);
		break;
	case 37:
		this->clsData->memPtr[3] = (this->clsData->memPtr[3] & ~(7 << 21)) | (val << 21);
		break;
	case 38:
		this->clsData->memPtr[3] = (this->clsData->memPtr[3] & ~(7 << 24)) | (val << 24);
		break;
	case 39:
		this->clsData->memPtr[3] = (this->clsData->memPtr[3] & ~(7 << 27)) | (val << 27);
		break;

	case 40:
		this->clsData->memPtr[4] = (this->clsData->memPtr[4] & ~7) | val;
		break;
	case 41:
		this->clsData->memPtr[4] = (this->clsData->memPtr[4] & ~(7 << 3)) | (val << 3);
		break;
	case 42:
		this->clsData->memPtr[4] = (this->clsData->memPtr[4] & ~(7 << 6)) | (val << 6);
		break;
	case 43:
		this->clsData->memPtr[4] = (this->clsData->memPtr[4] & ~(7 << 9)) | (val << 9);
		break;
	case 44:
		this->clsData->memPtr[4] = (this->clsData->memPtr[4] & ~(7 << 12)) | (val << 12);
		break;
	case 45:
		this->clsData->memPtr[4] = (this->clsData->memPtr[4] & ~(7 << 15)) | (val << 15);
		break;
	case 46:
		this->clsData->memPtr[4] = (this->clsData->memPtr[4] & ~(7 << 18)) | (val << 18);
		break;
	case 47:
		this->clsData->memPtr[4] = (this->clsData->memPtr[4] & ~(7 << 21)) | (val << 21);
		break;
	case 48:
		this->clsData->memPtr[4] = (this->clsData->memPtr[4] & ~(7 << 24)) | (val << 24);
		break;
	case 49:
		this->clsData->memPtr[4] = (this->clsData->memPtr[4] & ~(7 << 27)) | (val << 27);
		break;

	case 50:
		this->clsData->memPtr[5] = (this->clsData->memPtr[5] & ~7) | val;
		break;
	case 51:
		this->clsData->memPtr[5] = (this->clsData->memPtr[5] & ~(7 << 3)) | (val << 3);
		break;
	case 52:
		this->clsData->memPtr[5] = (this->clsData->memPtr[5] & ~(7 << 6)) | (val << 6);
		break;
	case 53:
		this->clsData->memPtr[5] = (this->clsData->memPtr[5] & ~(7 << 9)) | (val << 9);
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
			this->clsData->memPtr[7] = 1 << pinNum;
		}
		else
		{
			this->clsData->memPtr[8] = 1 << (pinNum - 32);
		}
	}
	else
	{
		if (pinNum < 32)
		{
			this->clsData->memPtr[10] = 1 << pinNum;
		}
		else
		{
			this->clsData->memPtr[11] = 1 << (pinNum - 32);
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
		this->clsData->memPtr[37] = 0;
	}
	else if (pt == IO::IOPin::PT_DOWN)
	{
		this->clsData->memPtr[37] = 1;
	}
	else if (pt == IO::IOPin::PT_UP)
	{
		this->clsData->memPtr[37] = 2;
	}
	else
	{
		return false;
	}
	Sync::SimpleThread::Sleepus(1);
	if (pinNum < 32)
	{
		this->clsData->memPtr[38] = (1 << pinNum);
	}
	else
	{
		this->clsData->memPtr[39] = (1 << (pinNum - 32));
	}
	Sync::SimpleThread::Sleepus(1);
	if (pinNum < 32)
	{
		this->clsData->memPtr[38] = 0;
	}
	else
	{
		this->clsData->memPtr[39] = 0;
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
			this->clsData->memPtr[25] |= (1 << pinNum);
		}
		else
		{
			this->clsData->memPtr[25] &= ~(1 << pinNum);
		}
	}
	else
	{
		if (enable)
		{
			this->clsData->memPtr[26] |= (1 << (pinNum - 32));
		}
		else
		{
			this->clsData->memPtr[26] &= ~(1 << (pinNum - 32));
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
			this->clsData->memPtr[28] |= (1 << pinNum);
		}
		else
		{
			this->clsData->memPtr[28] &= ~(1 << pinNum);
		}
	}
	else
	{
		if (enable)
		{
			this->clsData->memPtr[29] |= (1 << (pinNum - 32));
		}
		else
		{
			this->clsData->memPtr[29] &= ~(1 << (pinNum - 32));
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
			this->clsData->memPtr[19] |= (1 << pinNum);
		}
		else
		{
			this->clsData->memPtr[19] &= ~(1 << pinNum);
		}
	}
	else
	{
		if (enable)
		{
			this->clsData->memPtr[20] |= (1 << (pinNum - 32));
		}
		else
		{
			this->clsData->memPtr[20] &= ~(1 << (pinNum - 32));
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
			this->clsData->memPtr[22] |= (1 << pinNum);
		}
		else
		{
			this->clsData->memPtr[22] &= ~(1 << pinNum);
		}
	}
	else
	{
		if (enable)
		{
			this->clsData->memPtr[23] |= (1 << (pinNum - 32));
		}
		else
		{
			this->clsData->memPtr[23] &= ~(1 << (pinNum - 32));
		}
	}
}

Bool IO::GPIOControl::HasEvent(UOSInt pinNum)
{
	if (pinNum >= 54)
		return false;
	if (pinNum < 32)
	{
		return (this->clsData->memPtr[16] & (1 << pinNum)) != 0;
	}
	else
	{
		return (this->clsData->memPtr[17] & (1 << (pinNum - 32))) != 0;
	}
}

void IO::GPIOControl::ClearEvent(UOSInt pinNum)
{
	if (pinNum >= 54)
		return;
	if (pinNum < 32)
	{
		this->clsData->memPtr[16] &= ~(1 << pinNum);
	}
	else
	{
		this->clsData->memPtr[17] &= ~(1 << (pinNum - 32));
	}
}

static Text::CString GPIOControl_ALT0[] = {
	CSTR("SDA0"),
	CSTR("SCL0"),
	CSTR("SDA1"),
	CSTR("SCL1"),
	CSTR("GPCLK0"),
	CSTR("GPCLK1"),
	CSTR("GPCLK2"),
	CSTR("SPI0_CE1_N"),
	CSTR("SPI0_CE0_N"),
	CSTR("SPI0_MISO"),

	CSTR("SPI0_MOSI"),
	CSTR("SPI0_SCLK"),
	CSTR("PWM0"),
	CSTR("PWM1"),
	CSTR("TXD0"),
	CSTR("RXD0"),
	CSTR("Reserved"),
	CSTR("Reserved"),
	CSTR("PCM_CLK"),
	CSTR("PCM_FS"),

	CSTR("PCM_DIN"),
	CSTR("PCM_DOUT"),
	CSTR("Reserved"),
	CSTR("Reserved"),
	CSTR("Reserved"),
	CSTR("Reserved"),
	CSTR("Reserved"),
	CSTR("Reserved"),
	CSTR("SDA0"),
	CSTR("SCL0"),

	CSTR("Reserved"),
	CSTR("Reserved"),
	CSTR("GPCLK0"),
	CSTR("Reserved"),
	CSTR("GPCLK0"),
	CSTR("SPI0_CE1_N"),
	CSTR("SPI0_CE0_N"),
	CSTR("SPI0_MISO"),
	CSTR("SPI0_MOSI"),
	CSTR("SPI0_SCLK"),

	CSTR("PWM0"),
	CSTR("PWM1"),
	CSTR("GPCLK1"),
	CSTR("GPCLK2"),
	CSTR("GPCLK1"),
	CSTR("PWM1"),
	CSTR("Reserved"),
	CSTR("Reserved"),
	CSTR("Reserved"),
	CSTR("Reserved"),

	CSTR("Reserved"),
	CSTR("Reserved"),
	CSTR("Reserved"),
	CSTR("Reserved"),
};

static Text::CString GPIOControl_ALT1[] = {
	CSTR("SA5"),
	CSTR("SA4"),
	CSTR("SA3"),
	CSTR("SA2"),
	CSTR("SA1"),
	CSTR("SA0"),
	CSTR("SOE_N/SE"),
	CSTR("SWE_N/SRW_N"),
	CSTR("SD0"),
	CSTR("SD1"),

	CSTR("SD2"),
	CSTR("SD3"),
	CSTR("SD4"),
	CSTR("SD5"),
	CSTR("SD6"),
	CSTR("SD7"),
	CSTR("SD8"),
	CSTR("SD9"),
	CSTR("SD10"),
	CSTR("SD11"),

	CSTR("SD12"),
	CSTR("SD13"),
	CSTR("SD14"),
	CSTR("SD15"),
	CSTR("SD16"),
	CSTR("SD17"),
	CSTR("Reserved"),
	CSTR("Reserved"),
	CSTR("SA5"),
	CSTR("SA4"),

	CSTR("SA3"),
	CSTR("SA2"),
	CSTR("SA1"),
	CSTR("SA0"),
	CSTR("SOE_N/SE"),
	CSTR("SWE_N/SRW_N"),
	CSTR("SD0"),
	CSTR("SD1"),
	CSTR("SD2"),
	CSTR("SD3"),

	CSTR("SD4"),
	CSTR("SD5"),
	CSTR("SD6"),
	CSTR("SD7"),
	CSTR("SDA0"),
	CSTR("SCL0"),
	CSTR("Unknown"),
	CSTR("Unknown"),
	CSTR("Unknown"),
	CSTR("Unknown"),

	CSTR("Unknown"),
	CSTR("Unknown"),
	CSTR("Unknown"),
	CSTR("Unknown"),
};

static Text::CString GPIOControl_ALT2[] = {
	CSTR("Reserved"),
	CSTR("Reserved"),
	CSTR("Reserved"),
	CSTR("Reserved"),
	CSTR("Reserved"),
	CSTR("Reserved"),
	CSTR("Reserved"),
	CSTR("Reserved"),
	CSTR("Reserved"),
	CSTR("Reserved"),

	CSTR("Reserved"),
	CSTR("Reserved"),
	CSTR("Reserved"),
	CSTR("Reserved"),
	CSTR("Reserved"),
	CSTR("Reserved"),
	CSTR("Reserved"),
	CSTR("Reserved"),
	CSTR("Reserved"),
	CSTR("Reserved"),

	CSTR("Reserved"),
	CSTR("Reserved"),
	CSTR("Reserved"),
	CSTR("Reserved"),
	CSTR("Reserved"),
	CSTR("Reserved"),
	CSTR("Reserved"),
	CSTR("Reserved"),
	CSTR("PCM_CLK"),
	CSTR("PCM_FS"),

	CSTR("PCM_DIN"),
	CSTR("PCM_DOUT"),
	CSTR("Reserved"),
	CSTR("Reserved"),
	CSTR("Reserved"),
	CSTR("Unknown"),
	CSTR("TXD0"),
	CSTR("RXD0"),
	CSTR("RTS0"),
	CSTR("CTS0"),

	CSTR("Unknown"),
	CSTR("Reserved"),
	CSTR("Reserved"),
	CSTR("Reserved"),
	CSTR("SDA1"),
	CSTR("SCL1"),
	CSTR("Unknown"),
	CSTR("Unknown"),
	CSTR("Unknown"),
	CSTR("Unknown"),

	CSTR("Unknown"),
	CSTR("Unknown"),
	CSTR("Unknown"),
	CSTR("Unknown"),
};

static Text::CString GPIOControl_ALT3[] = {
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
	CSTR("Unknown"),
	CSTR("Unknown"),
	CSTR("Unknown"),
	CSTR("CTS0"),
	CSTR("RTS0"),
	CSTR("BSCSL SDA/MOSI"),
	CSTR("BSCSL SCL/SCLK"),

	CSTR("BSCSL/MISO"),
	CSTR("BSCSL/CE_N"),
	CSTR("SD1_CLK"),
	CSTR("SD1_CMD"),
	CSTR("SD1_DAT0"),
	CSTR("SD1_DAT1"),
	CSTR("SD1_DAT2"),
	CSTR("SD1_DAT3"),
	CSTR("Reserved"),
	CSTR("Reserved"),

	CSTR("CTS0"),
	CSTR("RTS0"),
	CSTR("TXD0"),
	CSTR("RXD0"),
	CSTR("Reserved"),
	CSTR("Reserved"),
	CSTR("Reserved"),
	CSTR("Reserved"),
	CSTR("Reserved"),
	CSTR("Reserved"),

	CSTR("Reserved"),
	CSTR("Reserved"),
	CSTR("Reserved"),
	CSTR("Reserved"),
	CSTR("Reserved"),
	CSTR("Reserved"),
	CSTR("Unknown"),
	CSTR("Unknown"),
	CSTR("Unknown"),
	CSTR("Unknown"),

	CSTR("Unknown"),
	CSTR("Unknown"),
	CSTR("Unknown"),
	CSTR("Unknown"),
};

static Text::CString GPIOControl_ALT4[] = {
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
	CSTR("Unknown"),
	CSTR("Unknown"),
	CSTR("Unknown"),
	CSTR("SPI1_CE2_N"),
	CSTR("SPI1_CE1_N"),
	CSTR("SPI1_CE0_N"),
	CSTR("SPI1_MISO"),

	CSTR("SPI1_MOSI"),
	CSTR("SPI1_SCLK"),
	CSTR("ARM_TRST"),
	CSTR("ARM_RTCK"),
	CSTR("ARM_TDO"),
	CSTR("ARM_TCK"),
	CSTR("ARM_TDI"),
	CSTR("ARM_TMS"),
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

	CSTR("SPI2_MISO"),
	CSTR("SPI2_MOSI"),
	CSTR("SPI2_SCLK"),
	CSTR("SPI2_CE0_N"),
	CSTR("SPI2_CE1_N"),
	CSTR("SPI2_CE2_N"),
	CSTR("Unknown"),
	CSTR("Unknown"),
	CSTR("Unknown"),
	CSTR("Unknown"),

	CSTR("Unknown"),
	CSTR("Unknown"),
	CSTR("Unknown"),
	CSTR("Unknown"),
};

static Text::CString GPIOControl_ALT5[] = {
	CSTR("Unknown"),
	CSTR("Unknown"),
	CSTR("Unknown"),
	CSTR("Unknown"),
	CSTR("ARM_TDI"),
	CSTR("ARM_TDO"),
	CSTR("ARM_RTCK"),
	CSTR("Unknown"),
	CSTR("Unknown"),
	CSTR("Unknown"),

	CSTR("Unknown"),
	CSTR("Unknown"),
	CSTR("ARM_TMS"),
	CSTR("ARM_TCK"),
	CSTR("TXD1"),
	CSTR("RXD1"),
	CSTR("CTS1"),
	CSTR("RTS1"),
	CSTR("PWM0"),
	CSTR("PWM1"),

	CSTR("GPCLK0"),
	CSTR("GPCLK1"),
	CSTR("Unknown"),
	CSTR("Unknown"),
	CSTR("Unknown"),
	CSTR("Unknown"),
	CSTR("Unknown"),
	CSTR("Unknown"),
	CSTR("Unknown"),
	CSTR("Unknown"),

	CSTR("CTS1"),
	CSTR("RTS1"),
	CSTR("TXD1"),
	CSTR("RXD1"),
	CSTR("Unknown"),
	CSTR("Unknown"),
	CSTR("Unknown"),
	CSTR("Unknown"),
	CSTR("Unknown"),
	CSTR("Unknown"),

	CSTR("TXD1"),
	CSTR("RXD1"),
	CSTR("RTS1"),
	CSTR("CTS1"),
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

Text::CString IO::GPIOControl::PinModeGetName(UOSInt pinNum, UOSInt pinMode)
{
	if (pinMode == 0)
	{
		return CSTR("Input");
	}
	else if (pinMode == 1)
	{
		return CSTR("Output");
	}
	if (pinNum >= 54)
	{
		return CSTR("Unknown");
	}
	if (pinMode == 4)
	{
		return GPIOControl_ALT0[pinNum];
	}
	else if (pinMode == 5)
	{
		return GPIOControl_ALT1[pinNum];
	}
	else if (pinMode == 6)
	{
		return GPIOControl_ALT2[pinNum];
	}
	else if (pinMode == 7)
	{
		return GPIOControl_ALT3[pinNum];
	}
	else if (pinMode == 3)
	{
		return GPIOControl_ALT4[pinNum];
	}
	else if (pinMode == 2)
	{
		return GPIOControl_ALT5[pinNum];
	}
	return CSTR("Unknown");
}
