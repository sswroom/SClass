#include "Stdafx.h"
#include "IO/Device/SDM120M.h"
#include "Math/Math.h"
#include "Math/Unit/Angle.h"
#include "Math/Unit/ApparentPower.h"
#include "Math/Unit/ElectricCurrent.h"
#include "Math/Unit/ElectricPotential.h"
#include "Math/Unit/Energy.h"
#include "Math/Unit/Frequency.h"
#include "Math/Unit/Power.h"
#include "Math/Unit/Ratio.h"
#include "Math/Unit/ReactiveEnergy.h"
#include "Math/Unit/ReactivePower.h"

IO::Device::SDM120M::SDM120M(IO::MODBUSMaster *modbus, UInt8 addr) : IO::MODBUSDevice(modbus, addr)
{
}

IO::Device::SDM120M::~SDM120M()
{
}

Bool IO::Device::SDM120M::ReadVoltage(Double *volt)
{
	return this->ReadInputFloat(0x0000, volt);
}

Bool IO::Device::SDM120M::ReadCurrent(Double *amps)
{
	return this->ReadInputFloat(0x0006, amps);
}

Bool IO::Device::SDM120M::ReadActivePower(Double *watt)
{
	return this->ReadInputFloat(0x000C, watt);
}

Bool IO::Device::SDM120M::ReadApparentPower(Double *voltAmps)
{
	return this->ReadInputFloat(0x0012, voltAmps);
}

Bool IO::Device::SDM120M::ReadReactivePower(Double *var)
{
	return this->ReadInputFloat(0x0018, var);
}

Bool IO::Device::SDM120M::ReadPowerFactor(Double *ratio)
{
	return this->ReadInputFloat(0x001E, ratio);
}

Bool IO::Device::SDM120M::ReadPhaseAngle(Double *degree)
{
	return this->ReadInputFloat(0x0024, degree);
}

Bool IO::Device::SDM120M::ReadFrequency(Double *hz)
{
	return this->ReadInputFloat(0x0046, hz);
}

Bool IO::Device::SDM120M::ReadImportActiveEnergy(Double *kwh)
{
	return this->ReadInputFloat(0x0048, kwh);
}

Bool IO::Device::SDM120M::ReadExportActiveEnergy(Double *kwh)
{
	return this->ReadInputFloat(0x004A, kwh);
}

Bool IO::Device::SDM120M::ReadImportReactiveEnergy(Double *kvarh)
{
	return this->ReadInputFloat(0x004C, kvarh);
}

Bool IO::Device::SDM120M::ReadExportReactiveEnergy(Double *kvarh)
{
	return this->ReadInputFloat(0x004E, kvarh);
}

Bool IO::Device::SDM120M::ReadTotalActiveEnergy(Double *kwh)
{
	return this->ReadInputFloat(0x0156, kwh);
}

Bool IO::Device::SDM120M::ReadTotalReactiveEnergy(Double *kvarh)
{
	return this->ReadInputFloat(0x0158, kvarh);
}

Bool IO::Device::SDM120M::ReadRelayPulseWidth(Int32 *ms)
{
	Double dval;
	if (this->ReadHoldingFloat(0x000A, &dval))
	{
		*ms = Math::Double2Int32(dval);
		return true;
	}
	return false;
}

Bool IO::Device::SDM120M::ReadParityStop(Int32 *parity)
{
	Double dval;
	if (this->ReadHoldingFloat(0x0012, &dval))
	{
		*parity = Math::Double2Int32(dval);
		return true;
	}
	return false;
}

Bool IO::Device::SDM120M::ReadNetworkNode(Int32 *addr)
{
	Double dval;
	if (this->ReadHoldingFloat(0x0014, &dval))
	{
		*addr = Math::Double2Int32(dval);
		return true;
	}
	return false;
}

Bool IO::Device::SDM120M::ReadBaudRate(Int32 *baudRate)
{
	Double brId = 0;
	if (this->ReadHoldingFloat(0x001C, &brId))
	{
		switch (Math::Double2Int32(brId))
		{
		case 0:
			*baudRate = 2400;
			break;
		case 1:
			*baudRate = 4800;
			break;
		case 2:
			*baudRate = 9600;
			break;
		case 3:
			*baudRate = 19200;
			break;
		case 4:
			*baudRate = 38400;
			break;
		default:
			*baudRate = 0;
			break;
		}
		return true;
	}
	return false;
}

Bool IO::Device::SDM120M::SetRelayPulseWidth(Single ms)
{
	return this->WriteHoldingF32(0x2, ms);
}

Bool IO::Device::SDM120M::SetNetworkNode(UInt8 id)
{
	return this->WriteHoldingF32(0x14, id);
}

Bool IO::Device::SDM120M::SetBaudRate(Int32 baudRate)
{
	Single brId = 0;
	switch (baudRate)
	{
	case 2400:
		brId = 0;
		break;
	case 4800:
		brId = 1;
		break;
	case 9600:
		brId = 2;
		break;
	case 19200:
		brId = 3;
		break;
	case 38400:
		brId = 4;
		break;
	default:
		return false;
	}
	return this->WriteHoldingF32(0x1C, brId);
}

void IO::Device::SDM120M::GetDataEntries(UInt8 addr, MODBUSDataEntry dataHdlr, void *userObj)
{
	dataHdlr(userObj, (const UTF8Char*)"Voltage",                addr, 30001, IO::MODBUSController::DT_F32, Math::Unit::UnitBase::VT_ELECTRIC_POTENTIAL, Math::Unit::ElectricPotential::EPU_VOLT, 1);
	dataHdlr(userObj, (const UTF8Char*)"Current",                addr, 30007, IO::MODBUSController::DT_F32, Math::Unit::UnitBase::VT_ELECTRIC_CURRENT,   Math::Unit::ElectricCurrent::ECU_AMPERE, 1);
	dataHdlr(userObj, (const UTF8Char*)"Active Power",           addr, 30013, IO::MODBUSController::DT_F32, Math::Unit::UnitBase::VT_POWER,              Math::Unit::Power::PU_WATT, 1);
	dataHdlr(userObj, (const UTF8Char*)"Apparent Power",         addr, 30019, IO::MODBUSController::DT_F32, Math::Unit::UnitBase::VT_APPARENT_POWER,     Math::Unit::ApparentPower::APU_VOLTAMPERE, 1);
	dataHdlr(userObj, (const UTF8Char*)"Reactive Power",         addr, 30025, IO::MODBUSController::DT_F32, Math::Unit::UnitBase::VT_REACTIVE_POWER,     Math::Unit::ReactivePower::RPU_VAR, 1);
	dataHdlr(userObj, (const UTF8Char*)"Power Factor",           addr, 30031, IO::MODBUSController::DT_F32, Math::Unit::UnitBase::VT_RATIO,              Math::Unit::Ratio::RU_RATIO, 1);
	dataHdlr(userObj, (const UTF8Char*)"Phase Angle",            addr, 30037, IO::MODBUSController::DT_F32, Math::Unit::UnitBase::VT_ANGLE,              Math::Unit::Angle::AU_DEGREE, 1);
	dataHdlr(userObj, (const UTF8Char*)"Frequency",              addr, 30071, IO::MODBUSController::DT_F32, Math::Unit::UnitBase::VT_FREQUENCY,          Math::Unit::Frequency::FU_HERTZ, 1);
	dataHdlr(userObj, (const UTF8Char*)"Import Active Energy",   addr, 30073, IO::MODBUSController::DT_F32, Math::Unit::UnitBase::VT_ENERGY,             Math::Unit::Energy::EU_KILOWATTHOUR, 1);
	dataHdlr(userObj, (const UTF8Char*)"Export Active Energy",   addr, 30075, IO::MODBUSController::DT_F32, Math::Unit::UnitBase::VT_ENERGY,             Math::Unit::Energy::EU_KILOWATTHOUR, 1);
	dataHdlr(userObj, (const UTF8Char*)"Import Reactive Energy", addr, 30077, IO::MODBUSController::DT_F32, Math::Unit::UnitBase::VT_REACTIVE_ENERGY,    Math::Unit::ReactiveEnergy::REU_KVARH, 1);
	dataHdlr(userObj, (const UTF8Char*)"Export Reactive Energy", addr, 30079, IO::MODBUSController::DT_F32, Math::Unit::UnitBase::VT_REACTIVE_ENERGY,    Math::Unit::ReactiveEnergy::REU_KVARH, 1);
	dataHdlr(userObj, (const UTF8Char*)"Total Active Energy",    addr, 30343, IO::MODBUSController::DT_F32, Math::Unit::UnitBase::VT_ENERGY,             Math::Unit::Energy::EU_KILOWATTHOUR, 1);
	dataHdlr(userObj, (const UTF8Char*)"Total Reactive Energy",  addr, 30345, IO::MODBUSController::DT_F32, Math::Unit::UnitBase::VT_REACTIVE_ENERGY,    Math::Unit::ReactiveEnergy::REU_KVARH, 1);
}
