#include "Stdafx.h"
#include "IO/ED516Sim.h"

IO::ED516Sim::ED516Sim()
{
	UOSInt i = 16;
	while (i-- > 0)
	{
		this->diCounts[i] = 0;
		this->diValues[i] = false;
	}
}

IO::ED516Sim::~ED516Sim()
{
}

Text::CStringNN IO::ED516Sim::GetName()
{
	return CSTR("ED516");
}

UOSInt IO::ED516Sim::GetValueCount()
{
	return 32;
}

Text::CString IO::ED516Sim::GetValueName(UOSInt index)
{
	switch (index)
	{
	case 0:
		return CSTR("Din 0");
	case 1:
		return CSTR("Din 1");
	case 2:
		return CSTR("Din 2");
	case 3:
		return CSTR("Din 3");
	case 4:
		return CSTR("Din 4");
	case 5:
		return CSTR("Din 5");
	case 6:
		return CSTR("Din 6");
	case 7:
		return CSTR("Din 7");
	case 8:
		return CSTR("Din 8");
	case 9:
		return CSTR("Din 9");
	case 10:
		return CSTR("Din 10");
	case 11:
		return CSTR("Din 11");
	case 12:
		return CSTR("Din 12");
	case 13:
		return CSTR("Din 13");
	case 14:
		return CSTR("Din 14");
	case 15:
		return CSTR("Din 15");
	case 16:
		return CSTR("Din 0 Count");
	case 17:
		return CSTR("Din 1 Count");
	case 18:
		return CSTR("Din 2 Count");
	case 19:
		return CSTR("Din 3 Count");
	case 20:
		return CSTR("Din 4 Count");
	case 21:
		return CSTR("Din 5 Count");
	case 22:
		return CSTR("Din 6 Count");
	case 23:
		return CSTR("Din 7 Count");
	case 24:
		return CSTR("Din 8 Count");
	case 25:
		return CSTR("Din 9 Count");
	case 26:
		return CSTR("Din 10 Count");
	case 27:
		return CSTR("Din 11 Count");
	case 28:
		return CSTR("Din 12 Count");
	case 29:
		return CSTR("Din 13 Count");
	case 30:
		return CSTR("Din 14 Count");
	case 31:
		return CSTR("Din 15 Count");
	default:
		return CSTR_NULL;
	}
}

Bool IO::ED516Sim::GetValue(UOSInt index, NN<Text::StringBuilderUTF8> sb)
{
	if (index < 16)
	{
		sb->AppendU32(this->diValues[index]);
		return true;
	}
	else if (index < 32)
	{
		sb->AppendU16(this->diCounts[index - 16]);
		return true;
	}
	return false;
}

Bool IO::ED516Sim::ToggleValue(UOSInt index)
{
	if (index < 16)
	{
		if (this->diValues[index])
		{
			this->diValues[index] = false;
		}
		else
		{
			this->diValues[index] = true;
			this->diCounts[index]++;
		}
		return true;
	}
	else if (index < 32)
	{
		this->diCounts[index - 16] = 0;
		return true;
	}
	return false;
}

Bool IO::ED516Sim::ReadCoil(UInt16 coilAddr, Bool *value)
{
	if (coilAddr >= 0x20 && coilAddr <= 0x2F)
	{
		*value = this->diValues[coilAddr - 0x20];
		return true;
	}
	return false;
}

Bool IO::ED516Sim::ReadDescreteInput(UInt16 inputAddr, Bool *value)
{
	if (inputAddr < 16)
	{
		*value = this->diValues[inputAddr];
		return true;
	}
	return false;
}

Bool IO::ED516Sim::ReadHoldingRegister(UInt16 regAddr, UInt16 *value)
{
	if (regAddr < 8)
	{
		*value = this->diCounts[regAddr];
		return true;
	}
	return false;
}

Bool IO::ED516Sim::ReadInputRegister(UInt16 regAddr, UInt16 *value)
{
	if (regAddr < 16)
	{
		*value = this->diCounts[regAddr];
		return true;
	}
	else if (regAddr == 0x20)
	{
		*value = (UInt16)((this->diValues[0] << 0) |
			(this->diValues[1] << 1) |
			(this->diValues[2] << 2) |
			(this->diValues[3] << 3) |
			(this->diValues[4] << 4) |
			(this->diValues[5] << 5) |
			(this->diValues[6] << 6) |
			(this->diValues[7] << 7) |
			(this->diValues[8] << 8) |
			(this->diValues[9] << 9) |
			(this->diValues[10] << 10) |
			(this->diValues[11] << 11) |
			(this->diValues[12] << 12) |
			(this->diValues[13] << 13) |
			(this->diValues[14] << 14) |
			(this->diValues[15] << 15));
		return true;
	}
	return false;
}

Bool IO::ED516Sim::WriteCoil(UInt16 coilAddr, Bool isHigh)
{
	if (coilAddr >= 0x200 && coilAddr < 0x208)
	{
		this->diCounts[coilAddr - 0x200] = 0;
		return true;
	}
	return false;
}

Bool IO::ED516Sim::WriteHoldingRegister(UInt16 regAddr, UInt16 val)
{
	return false;
}
