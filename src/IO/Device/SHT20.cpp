#include "Stdafx.h"
#include "Data/ByteTool.h"
#include "IO/Device/SHT20.h"

IO::Device::SHT20::SHT20(IO::MODBUSMaster *modbus, UInt8 addr) : IO::MODBUSDevice(modbus, addr)
{
}

IO::Device::SHT20::~SHT20()
{
}

Bool IO::Device::SHT20::ReadTempRH(Double *temp, Double *rh)
{
	UInt8 buff[4];
	if (this->ReadInputBuff(0, 2, buff))
	{
		*temp = ReadMInt16(buff) * 0.1;
		*rh = ReadMInt16(&buff[2]) * 0.1;
		return true;
	}
	else
	{
		return false;
	}
}

Bool IO::Device::SHT20::ReadTemp(Double *temp)
{
	Int32 tmpV;
	if (this->ReadInputI16(0, &tmpV))
	{
		*temp = tmpV * 0.1;
		return true;
	}
	else
	{
		return false;
	}
}

Bool IO::Device::SHT20::ReadRH(Double *rh)
{
	Int32 tmpV;
	if (this->ReadInputI16(1, &tmpV))
	{
		*rh = tmpV * 0.1;
		return true;
	}
	else
	{
		return false;
	}
}

Bool IO::Device::SHT20::ReadBaudRate(Int32 *baudRate)
{
	return this->ReadHoldingI16(0, baudRate);
}

Bool IO::Device::SHT20::ReadParity(Int32 *parity)
{
	return this->ReadHoldingI16(1, parity);
}

Bool IO::Device::SHT20::ReadId(Int32 *id)
{
	return this->ReadHoldingI16(2, id);
}

Bool IO::Device::SHT20::ReadIdValid(Int32 *idValid)
{
	return this->ReadHoldingI16(3, idValid);
}

Bool IO::Device::SHT20::SetId(UInt8 id)
{
	return this->WriteHoldingU16(3, 1) && this->WriteHoldingU16(2, id);
}
