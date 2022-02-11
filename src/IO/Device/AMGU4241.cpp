#include "Stdafx.h"
#include "IO/Device/AMGU4241.h"
#include "Math/Math.h"
#include "Math/Unit/Count.h"
#include "Math/Unit/Temperature.h"

IO::Device::AMGU4241::AMGU4241(IO::MODBUSMaster *modbus, UInt8 addr) : IO::MODBUSDevice(modbus, addr)
{
}

IO::Device::AMGU4241::~AMGU4241()
{
}

Bool IO::Device::AMGU4241::ReadPeopleCount(Int32 *count)
{
	return this->ReadHoldingI16(0, count);
}

Bool IO::Device::AMGU4241::ReadTemperature(Double *temp)
{
	Int32 iVal;
	if (this->ReadHoldingI16(6, &iVal))
	{
		if (iVal & 0x8000)
		{
			*temp = -(iVal & 0x7fff) / 256.0;
		}
		else
		{
			*temp = (iVal & 0x7fff) / 256.0;
		}
		return true;
	}
	return false;
}

void IO::Device::AMGU4241::GetDataEntries(UInt8 addr, MODBUSDataEntry dataHdlr, void *userObj)
{
	dataHdlr(userObj, CSTR("Number of people"),       addr, 40001, IO::MODBUSController::DT_U16,  Math::Unit::UnitBase::VT_COUNT,              Math::Unit::Count::CU_UNIT, 1);
	dataHdlr(userObj, CSTR("Human Coordinates"),      addr, 40002, IO::MODBUSController::DT_IU32, Math::Unit::UnitBase::VT_COUNT,              Math::Unit::Count::CU_UNIT, 0);
	dataHdlr(userObj, CSTR("Exist/absence data"),     addr, 40004, IO::MODBUSController::DT_IU16, Math::Unit::UnitBase::VT_COUNT,              Math::Unit::Count::CU_UNIT, 0);
	dataHdlr(userObj, CSTR("Moving directions"),      addr, 40005, IO::MODBUSController::DT_IU32, Math::Unit::UnitBase::VT_COUNT,              Math::Unit::Count::CU_UNIT, 0);
	dataHdlr(userObj, CSTR("Temperature data"),       addr, 40007, IO::MODBUSController::DT_OS16, Math::Unit::UnitBase::VT_TEMPERATURE,        Math::Unit::Temperature::TU_CELSIUS, 256);
}
