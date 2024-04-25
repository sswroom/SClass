#include "Stdafx.h"
#include "IO/Device/BivocomTD210.h"

IO::Device::BivocomTD210::BivocomTD210(NN<IO::MODBUSMaster> modbus, UInt8 addr) : IO::MODBUSDevice(modbus, addr)
{
	this->SetTimeout(5000);
}

IO::Device::BivocomTD210::~BivocomTD210()
{
}

Bool IO::Device::BivocomTD210::ReadIOs(Int32 *io)
{
	return this->ReadDInputs(0, 3, io);
}

Bool IO::Device::BivocomTD210::SetOutput1(Bool isHigh)
{
	return this->WriteDOutput(0, isHigh);
}

Bool IO::Device::BivocomTD210::SetOutput2(Bool isHigh)
{
	return this->WriteDOutput(1, isHigh);
}

Bool IO::Device::BivocomTD210::SetOutput3(Bool isHigh)
{
	return this->WriteDOutput(2, isHigh);
}

Bool IO::Device::BivocomTD210::ReadADC1(OutParam<Int32> adc)
{
	return this->ReadInputI16(0, adc);
}

Bool IO::Device::BivocomTD210::ReadADC2(OutParam<Int32> adc)
{
	return this->ReadInputI16(1, adc);
}
