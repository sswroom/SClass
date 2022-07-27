#include "Stdafx.h"
#include "IO/ED588Sim.h"

IO::ED588Sim::ED588Sim()
{
	UOSInt i = 8;
	while (i-- > 0)
	{
		this->diCounts[i] = 0;
		this->diValues[i] = false;
		this->relays[i] = false;
	}
}

IO::ED588Sim::~ED588Sim()
{
}

Text::CString IO::ED588Sim::GetName()
{
	return CSTR("ED588");
}

UOSInt IO::ED588Sim::GetValueCount()
{
	return 24;
}

Text::CString IO::ED588Sim::GetValueName(UOSInt index)
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
		return CSTR("Din 0 Count");
	case 9:
		return CSTR("Din 1 Count");
	case 10:
		return CSTR("Din 2 Count");
	case 11:
		return CSTR("Din 3 Count");
	case 12:
		return CSTR("Din 4 Count");
	case 13:
		return CSTR("Din 5 Count");
	case 14:
		return CSTR("Din 6 Count");
	case 15:
		return CSTR("Din 7 Count");
	case 16:
		return CSTR("Relay 0");
	case 17:
		return CSTR("Relay 1");
	case 18:
		return CSTR("Relay 2");
	case 19:
		return CSTR("Relay 3");
	case 20:
		return CSTR("Relay 4");
	case 21:
		return CSTR("Relay 5");
	case 22:
		return CSTR("Relay 6");
	case 23:
		return CSTR("Relay 7");
	default:
		return CSTR_NULL;
	}
}

Bool IO::ED588Sim::GetValue(UOSInt index, Text::StringBuilderUTF8 *sb)
{
	if (index < 8)
	{
		sb->AppendU32(this->diValues[index]);
		return true;
	}
	else if (index < 16)
	{
		sb->AppendU16(this->diCounts[index - 8]);
		return true;
	}
	else if (index < 24)
	{
		sb->AppendU32(this->relays[index - 16]);
		return true;
	}
	return false;
}

Bool IO::ED588Sim::ToggleValue(UOSInt index)
{
	if (index < 8)
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
	else if (index < 16)
	{
		this->diCounts[index - 8] = 0;
		return true;
	}
	else if (index < 24)
	{
		this->relays[index - 16] = !this->relays[index - 16];
		return true;
	}
	return false;
}

Bool IO::ED588Sim::ReadCoil(UInt16 coilAddr, Bool *value)
{
	if (coilAddr < 8)
	{
		*value = this->relays[coilAddr];
		return true;
	}
	else if (coilAddr >= 0x20 && coilAddr <= 0x27)
	{
		*value = this->diValues[coilAddr - 0x20];
		return true;
	}
	return false;
}

Bool IO::ED588Sim::ReadDescreteInput(UInt16 inputAddr, Bool *value)
{
	if (inputAddr < 8)
	{
		*value = this->diValues[inputAddr];
		return true;
	}
	return false;
}

Bool IO::ED588Sim::ReadHoldingRegister(UInt16 regAddr, UInt16 *value)
{
	if (regAddr < 8)
	{
		*value = this->diCounts[regAddr];
		return true;
	}
	else if (regAddr == 0x20)
	{
		*value = (UInt16)((this->relays[0] << 15) |
			(this->relays[1] << 14) |
			(this->relays[2] << 13) |
			(this->relays[3] << 12) |
			(this->relays[4] << 11) |
			(this->relays[5] << 10) |
			(this->relays[6] << 9) |
			(this->relays[7] << 8));
		return true;
	}
	return false;
}

Bool IO::ED588Sim::ReadInputRegister(UInt16 regAddr, UInt16 *value)
{
	if (regAddr < 8)
	{
		*value = this->diCounts[regAddr];
		return true;
	}
	else if (regAddr == 0x20)
	{
		*value = (UInt16)((this->diValues[0] << 15) |
			(this->diValues[1] << 14) |
			(this->diValues[2] << 13) |
			(this->diValues[3] << 12) |
			(this->diValues[4] << 11) |
			(this->diValues[5] << 10) |
			(this->diValues[6] << 9) |
			(this->diValues[7] << 8));
		return true;
	}
	return false;
}

Bool IO::ED588Sim::WriteCoil(UInt16 coilAddr, Bool isHigh)
{
	if (coilAddr < 8)
	{
		this->relays[coilAddr] = isHigh;
		return true;
	}
	else if (coilAddr >= 0x200 && coilAddr < 0x208)
	{
		this->diCounts[coilAddr - 0x200] = 0;
		return true;
	}
	return false;
}

Bool IO::ED588Sim::WriteHoldingRegister(UInt16 regAddr, UInt16 val)
{
	if (regAddr == 0x20)
	{
		this->relays[0] = (val & 0x8000) != 0;
		this->relays[1] = (val & 0x4000) != 0;
		this->relays[2] = (val & 0x2000) != 0;
		this->relays[3] = (val & 0x1000) != 0;
		this->relays[4] = (val & 0x0800) != 0;
		this->relays[5] = (val & 0x0400) != 0;
		this->relays[6] = (val & 0x0200) != 0;
		this->relays[7] = (val & 0x0100) != 0;
		return true;
	}
	return false;
}
