#include "Stdafx.h"
#include "Data/ByteTool.h"
#include "IO/Device/TH10SB.h"

IO::Device::TH10SB::TH10SB(NotNullPtr<IO::MODBUSMaster> modbus, UInt8 addr) : IO::MODBUSDevice(modbus, addr)
{
}

IO::Device::TH10SB::~TH10SB()
{
}

Bool IO::Device::TH10SB::ReadTempRH(OutParam<Double> temp, OutParam<Double> rh)
{
	UInt8 buff[4];
	if (this->ReadInputBuff(0, 2, buff))
	{
		temp.Set(ReadMInt16(buff) * 0.1);
		rh.Set(ReadMInt16(&buff[2]) * 0.1);
		return true;
	}
	else
	{
		return false;
	}
}

Bool IO::Device::TH10SB::ReadTemp(OutParam<Double> temp)
{
	Int32 tmpV;
	if (this->ReadInputI16(0, tmpV))
	{
		temp.Set(tmpV * 0.1);
		return true;
	}
	else
	{
		return false;
	}
}

Bool IO::Device::TH10SB::ReadRH(OutParam<Double> rh)
{
	Int32 tmpV;
	if (this->ReadInputI16(1, tmpV))
	{
		rh.Set(tmpV * 0.1);
		return true;
	}
	else
	{
		return false;
	}
}

Bool IO::Device::TH10SB::ReadId(OutParam<Int32> id)
{
	return this->ReadHoldingI16(100, id);
}

Bool IO::Device::TH10SB::ReadBaudRate(OutParam<Int32> baudRate)
{
	Int32 br;
	Bool ret = this->ReadHoldingI16(101, br);
	if (ret)
	{
		switch (br)
		{
		case 0:
			baudRate.Set(1200);
			break;
		case 1:
			baudRate.Set(2400);
			break;
		case 2:
			baudRate.Set(4800);
			break;
		case 3:
			baudRate.Set(9600);
			break;
		case 4:
			baudRate.Set(19200);
			break;
		default:
			baudRate.Set(0);
			break;
		}
	}
	return ret;
}

Bool IO::Device::TH10SB::SetId(UInt8 id)
{
	return this->WriteHoldingU16(100, id);
}

Bool IO::Device::TH10SB::SetBaudRate(Int32 baudRate)
{
	switch(baudRate)
	{
	case 1200:
		return this->WriteHoldingU16(101, 0);
	case 2400:
		return this->WriteHoldingU16(101, 1);
	case 4800:
		return this->WriteHoldingU16(101, 2);
	case 9600:
		return this->WriteHoldingU16(101, 3);
	case 19200:
		return this->WriteHoldingU16(101, 4);
	default:
		return false;
	}
}
