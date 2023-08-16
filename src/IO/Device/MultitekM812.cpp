#include "Stdafx.h"
#include "IO/Device/MultitekM812.h"

IO::Device::MultitekM812::MultitekM812(NotNullPtr<IO::MODBUSMaster> modbus, UInt8 addr) : IO::MODBUSDevice(modbus, addr)
{
}

IO::Device::MultitekM812::~MultitekM812()
{
}

Bool IO::Device::MultitekM812::ReadVoltsL2_L1(OutParam<Double> volt)
{
	return this->ReadInputFloat(0x0000, volt);
}

Bool IO::Device::MultitekM812::ReadVoltsL3_L2(OutParam<Double> volt)
{
	return this->ReadInputFloat(0x0002, volt);
}

Bool IO::Device::MultitekM812::ReadVoltsL1_L3(OutParam<Double> volt)
{
	return this->ReadInputFloat(0x0004, volt);
}

Bool IO::Device::MultitekM812::ReadSysVoltsL1(OutParam<Double> volt)
{
	return this->ReadInputFloat(0x0006, volt);
}

Bool IO::Device::MultitekM812::ReadSysVoltsL2(OutParam<Double> volt)
{
	return this->ReadInputFloat(0x0008, volt);
}

Bool IO::Device::MultitekM812::ReadSysVoltsL3(OutParam<Double> volt)
{
	return this->ReadInputFloat(0x000A, volt);
}

Bool IO::Device::MultitekM812::ReadAmpsL1(OutParam<Double> amps)
{
	return this->ReadInputFloat(0x000C, amps);
}

Bool IO::Device::MultitekM812::ReadAmpsL2(OutParam<Double> amps)
{
	return this->ReadInputFloat(0x000E, amps);
}

Bool IO::Device::MultitekM812::ReadAmpsL3(OutParam<Double> amps)
{
	return this->ReadInputFloat(0x0010, amps);
}

Bool IO::Device::MultitekM812::ReadActivePower(OutParam<Double> watt)
{
	return this->ReadInputFloat(0x0012, watt);
}

Bool IO::Device::MultitekM812::ReadApparentPower(OutParam<Double> voltAmps)
{
	return this->ReadInputFloat(0x0014, voltAmps);
}

Bool IO::Device::MultitekM812::ReadReactivePower(OutParam<Double> var)
{
	return this->ReadInputFloat(0x0016, var);
}

Bool IO::Device::MultitekM812::ReadPowerFactor(OutParam<Double> ratio)
{
	return this->ReadInputFloat(0x0018, ratio);
}

Bool IO::Device::MultitekM812::ReadTotalActiveEnergy(OutParam<Double> wh)
{
	return this->ReadInputFloat(0x001A, wh);
}

Bool IO::Device::MultitekM812::ReadTotalReactiveEnergy(OutParam<Double> varh)
{
	return this->ReadInputFloat(0x001C, varh);
}

Bool IO::Device::MultitekM812::ReadFrequency(OutParam<Double> hz)
{
	return this->ReadInputFloat(0x001E, hz);
}

Bool IO::Device::MultitekM812::ReadActivePowerL1(OutParam<Double> watt)
{
	return this->ReadInputFloat(0x0020, watt);
}

Bool IO::Device::MultitekM812::ReadActivePowerL2(OutParam<Double> watt)
{
	return this->ReadInputFloat(0x0022, watt);
}

Bool IO::Device::MultitekM812::ReadActivePowerL3(OutParam<Double> watt)
{
	return this->ReadInputFloat(0x0024, watt);
}

Bool IO::Device::MultitekM812::ReadReactivePowerL1(OutParam<Double> var)
{
	return this->ReadInputFloat(0x0026, var);
}

Bool IO::Device::MultitekM812::ReadReactivePowerL2(OutParam<Double> var)
{
	return this->ReadInputFloat(0x0028, var);
}

Bool IO::Device::MultitekM812::ReadReactivePowerL3(OutParam<Double> var)
{
	return this->ReadInputFloat(0x002A, var);
}

Bool IO::Device::MultitekM812::ReadSysWDemand(OutParam<Double> watt)
{
	return this->ReadInputFloat(0x002C, watt);
}

Bool IO::Device::MultitekM812::ReadSysVADemand(OutParam<Double> voltAmps)
{
	return this->ReadInputFloat(0x002E, voltAmps);
}

Bool IO::Device::MultitekM812::ReadSysADemand(OutParam<Double> amps)
{
	return this->ReadInputFloat(0x0030, amps);
}

Bool IO::Device::MultitekM812::ReadNeutralCurrent(OutParam<Double> amps)
{
	return this->ReadInputFloat(0x0032, amps);
}

Bool IO::Device::MultitekM812::ReadMaxWDemand(OutParam<Double> watt)
{
	return this->ReadInputFloat(0x0034, watt);
}

Bool IO::Device::MultitekM812::ReadMaxVADemand(OutParam<Double> voltAmps)
{
	return this->ReadInputFloat(0x0036, voltAmps);
}

Bool IO::Device::MultitekM812::ReadMaxADemand(OutParam<Double> amps)
{
	return this->ReadInputFloat(0x0038, amps);
}

Bool IO::Device::MultitekM812::ReadTHD_V1(OutParam<Double> volt)
{
	return this->ReadInputFloat(0x003A, volt);
}

Bool IO::Device::MultitekM812::ReadTHD_V2(OutParam<Double> volt)
{
	return this->ReadInputFloat(0x003C, volt);
}

Bool IO::Device::MultitekM812::ReadTHD_V3(OutParam<Double> volt)
{
	return this->ReadInputFloat(0x003E, volt);
}

Bool IO::Device::MultitekM812::ReadTHD_I1(OutParam<Double> amps)
{
	return this->ReadInputFloat(0x0040, amps);
}

Bool IO::Device::MultitekM812::ReadTHD_I2(OutParam<Double> amps)
{
	return this->ReadInputFloat(0x0042, amps);
}

Bool IO::Device::MultitekM812::ReadTHD_I3(OutParam<Double> amps)
{
	return this->ReadInputFloat(0x0044, amps);
}

Bool IO::Device::MultitekM812::ReadSysV(OutParam<Double> volts)
{
	return this->ReadHoldingFloat(0x0000, volts);
}

Bool IO::Device::MultitekM812::ReadSysI(OutParam<Double> amps)
{
	return this->ReadHoldingFloat(0x0002, amps);
}

Bool IO::Device::MultitekM812::ReadSysType(OutParam<Int32> typ)
{
	return this->ReadHoldingI32(0x0004, typ);
}

Bool IO::Device::MultitekM812::ReadSysPower(OutParam<Double> watt)
{
	return this->ReadHoldingFloat(0x0006, watt);
}

Bool IO::Device::MultitekM812::ReadDemandTime(OutParam<Int32> timeMin)
{
	return this->ReadHoldingI32(0x0008, timeMin);
}

Bool IO::Device::MultitekM812::ReadDemandPeriod(OutParam<Int32> period)
{
	return this->ReadHoldingI32(0x000A, period);
}

Bool IO::Device::MultitekM812::ReadRelayDivisor(OutParam<Int32> divisor)
{
	return this->ReadHoldingI32(0x000C, divisor);
}

Bool IO::Device::MultitekM812::GetRelayPulseWidth(OutParam<Int32> width) //n * 20ms
{
	return this->ReadHoldingI32(0x000E, width);
}
