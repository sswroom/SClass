#include "Stdafx.h"
#include "IO/ED527Sim.h"

IO::ED527Sim::ED527Sim()
{
	UOSInt i = 16;
	while (i-- > 0)
	{
		this->relays[i] = false;
	}
}

IO::ED527Sim::~ED527Sim()
{
}

Text::CString IO::ED527Sim::GetName()
{
	return CSTR("ED527");
}

UOSInt IO::ED527Sim::GetValueCount()
{
	return 16;
}

Text::CString IO::ED527Sim::GetValueName(UOSInt index)
{
	switch (index)
	{
	case 0:
		return CSTR("Relay 0");
	case 1:
		return CSTR("Relay 1");
	case 2:
		return CSTR("Relay 2");
	case 3:
		return CSTR("Relay 3");
	case 4:
		return CSTR("Relay 4");
	case 5:
		return CSTR("Relay 5");
	case 6:
		return CSTR("Relay 6");
	case 7:
		return CSTR("Relay 7");
	case 8:
		return CSTR("Relay 8");
	case 9:
		return CSTR("Relay 9");
	case 10:
		return CSTR("Relay 10");
	case 11:
		return CSTR("Relay 11");
	case 12:
		return CSTR("Relay 12");
	case 13:
		return CSTR("Relay 13");
	case 14:
		return CSTR("Relay 14");
	case 15:
		return CSTR("Relay 15");
	default:
		return CSTR_NULL;
	}
}

Bool IO::ED527Sim::GetValue(UOSInt index, NotNullPtr<Text::StringBuilderUTF8> sb)
{
	if (index < 16)
	{
		sb->AppendU32(this->relays[index]);
		return true;
	}
	return false;
}

Bool IO::ED527Sim::ToggleValue(UOSInt index)
{
	if (index < 16)
	{
		this->relays[index] = !this->relays[index];
		return true;
	}
	return false;
}


Bool IO::ED527Sim::ReadCoil(UInt16 coilAddr, Bool *value)
{
	if (coilAddr < 16)
	{
		*value = this->relays[coilAddr];
		return true;
	}
	return false;
}

Bool IO::ED527Sim::ReadDescreteInput(UInt16 inputAddr, Bool *value)
{
	return false;
}

Bool IO::ED527Sim::ReadHoldingRegister(UInt16 regAddr, UInt16 *value)
{
	if (regAddr == 0x20)
	{
		*value = (UInt16)((this->relays[0] << 0) |
			(this->relays[1] << 1) |
			(this->relays[2] << 2) |
			(this->relays[3] << 3) |
			(this->relays[4] << 4) |
			(this->relays[5] << 5) |
			(this->relays[6] << 6) |
			(this->relays[7] << 7) |
			(this->relays[8] << 8) |
			(this->relays[9] << 9) |
			(this->relays[10] << 10) |
			(this->relays[11] << 11) |
			(this->relays[12] << 12) |
			(this->relays[13] << 13) |
			(this->relays[14] << 14) |
			(this->relays[15] << 15));
		return true;
	}
	return false;
}

Bool IO::ED527Sim::ReadInputRegister(UInt16 regAddr, UInt16 *value)
{
	return false;
}

Bool IO::ED527Sim::WriteCoil(UInt16 coilAddr, Bool isHigh)
{
	if (coilAddr < 16)
	{
		this->relays[coilAddr] = isHigh;
		return true;
	}
	return false;
}

Bool IO::ED527Sim::WriteHoldingRegister(UInt16 regAddr, UInt16 val)
{
	if (regAddr == 0x20)
	{
		this->relays[0] = (val & 0x0001) != 0;
		this->relays[1] = (val & 0x0002) != 0;
		this->relays[2] = (val & 0x0004) != 0;
		this->relays[3] = (val & 0x0008) != 0;
		this->relays[4] = (val & 0x0010) != 0;
		this->relays[5] = (val & 0x0020) != 0;
		this->relays[6] = (val & 0x0040) != 0;
		this->relays[7] = (val & 0x0080) != 0;
		this->relays[8] = (val & 0x0100) != 0;
		this->relays[9] = (val & 0x0200) != 0;
		this->relays[10] = (val & 0x0400) != 0;
		this->relays[11] = (val & 0x0800) != 0;
		this->relays[12] = (val & 0x1000) != 0;
		this->relays[13] = (val & 0x2000) != 0;
		this->relays[14] = (val & 0x4000) != 0;
		this->relays[15] = (val & 0x8000) != 0;
		return true;
	}
	return false;
}
