#include "Stdafx.h"
#include "IO/ED538Sim.h"

IO::ED538Sim::ED538Sim()
{
	UOSInt i = 8;
	while (i-- > 0)
	{
		this->diCounts[i] = 0;
		this->diValues[i] = false;
	}
	i = 4;
	while (i-- > 0)
	{
		this->relays[i] = false;
	}
}

IO::ED538Sim::~ED538Sim()
{
}

Text::CString IO::ED538Sim::GetName()
{
	return CSTR("ED538");
}

UOSInt IO::ED538Sim::GetValueCount()
{
	return 20;
}

Text::CString IO::ED538Sim::GetValueName(UOSInt index)
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
	default:
		return CSTR_NULL;
	}
}

Bool IO::ED538Sim::GetValue(UOSInt index, NotNullPtr<Text::StringBuilderUTF8> sb)
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
	else if (index < 20)
	{
		sb->AppendU32(this->relays[index - 16]);
		return true;
	}
	return false;
}

Bool IO::ED538Sim::ToggleValue(UOSInt index)
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
	else if (index < 20)
	{
		this->relays[index - 16] = !this->relays[index - 16];
		return true;
	}
	return false;
}

Bool IO::ED538Sim::ReadCoil(UInt16 coilAddr, Bool *value)
{
	if (coilAddr < 4)
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

Bool IO::ED538Sim::ReadDescreteInput(UInt16 inputAddr, Bool *value)
{
	if (inputAddr < 8)
	{
		*value = this->diValues[inputAddr];
		return true;
	}
	return false;
}

Bool IO::ED538Sim::ReadHoldingRegister(UInt16 regAddr, UInt16 *value)
{
	if (regAddr < 8)
	{
		*value = this->diCounts[regAddr];
		return true;
	}
	else if (regAddr == 0x20)
	{
		*value = (UInt16)((this->relays[0] << 0) |
			(this->relays[1] << 1) |
			(this->relays[2] << 2) |
			(this->relays[3] << 3));
		return true;
	}
	return false;
}

Bool IO::ED538Sim::ReadInputRegister(UInt16 regAddr, UInt16 *value)
{
	if (regAddr < 8)
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
			(this->diValues[7] << 7));
		return true;
	}
	return false;
}

Bool IO::ED538Sim::WriteCoil(UInt16 coilAddr, Bool isHigh)
{
	if (coilAddr < 4)
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

Bool IO::ED538Sim::WriteHoldingRegister(UInt16 regAddr, UInt16 val)
{
	if (regAddr == 0x20)
	{
		this->relays[0] = (val & 0x0001) != 0;
		this->relays[1] = (val & 0x0002) != 0;
		this->relays[2] = (val & 0x0004) != 0;
		this->relays[3] = (val & 0x0008) != 0;
		return true;
	}
	return false;
}
