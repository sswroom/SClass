#include "Stdafx.h"
#include "IO/Device/ED538.h"
#include "Math/Math.h"
#include "Math/Unit/Count.h"
#include "Math/Unit/Temperature.h"

IO::Device::ED538::ED538(IO::MODBUSMaster *modbus, UInt8 addr) : IO::MODBUSDevice(modbus, addr)
{
}

IO::Device::ED538::~ED538()
{
}

Bool IO::Device::ED538::IsDIHighByCoil(UInt16 diNum)
{
	if (diNum >= 8)
		return false;
	return this->ReadCoil(diNum + 0x20);
}

Bool IO::Device::ED538::IsDIHighByInput(UInt16 diNum)
{
	if (diNum >= 8)
		return false;
	return this->ReadDInput(diNum);
}

Bool IO::Device::ED538::IsDIHighByReg(UInt16 diNum)
{
	if (diNum >= 8)
		return false;
	Int32 val;
	if (!this->ReadInputI16(diNum + 0x20, &val))
		return false;
	return val != 0;
}

UInt16 IO::Device::ED538::GetDICountByReg(UInt16 diNum)
{
	if (diNum >= 8)
		return 0;
	Int32 val;
	if (!this->ReadInputI16(diNum, &val))
		return 0;
	return (UInt16)val;
}

UInt16 IO::Device::ED538::GetDICountByHolding(UInt16 diNum)
{
	if (diNum >= 8)
		return 0;
	Int32 val;
	if (!this->ReadHoldingI16(diNum, &val))
		return 0;
	return (UInt16)val;
}

Bool IO::Device::ED538::ClearDICount(UInt16 diNum)
{
	if (diNum >= 8)
		return false;
	return this->WriteDOutput(0x200 + diNum, true);
}

Bool IO::Device::ED538::IsRelayHigh(UInt16 index)
{
	if (index >= 4)
		return false;
	return this->ReadCoil(index);
}

Bool IO::Device::ED538::SetRelayState(UInt16 index, Bool isHigh)
{
	if (index >= 4)
		return false;
	return this->WriteDOutput(index, isHigh);
}

Bool IO::Device::ED538::GetOutputOverloadFlag(UInt16 diNum)
{
	if (diNum >= 8)
		return false;
	return this->ReadDInput(0x400 + diNum);
}

void IO::Device::ED538::GetDataEntries(UInt8 addr, MODBUSDataEntry dataHdlr, void *userObj)
{
	dataHdlr(userObj, CSTR("Din 0"),       addr, 10001, IO::MODBUSController::DT_U8,   Math::Unit::UnitBase::VT_MAPPING,      0, 1);
	dataHdlr(userObj, CSTR("Din 1"),       addr, 10002, IO::MODBUSController::DT_U8,   Math::Unit::UnitBase::VT_MAPPING,      0, 1);
	dataHdlr(userObj, CSTR("Din 2"),       addr, 10003, IO::MODBUSController::DT_U8,   Math::Unit::UnitBase::VT_MAPPING,      0, 1);
	dataHdlr(userObj, CSTR("Din 3"),       addr, 10004, IO::MODBUSController::DT_U8,   Math::Unit::UnitBase::VT_MAPPING,      0, 1);
	dataHdlr(userObj, CSTR("Din 4"),       addr, 10005, IO::MODBUSController::DT_U8,   Math::Unit::UnitBase::VT_MAPPING,      0, 1);
	dataHdlr(userObj, CSTR("Din 5"),       addr, 10006, IO::MODBUSController::DT_U8,   Math::Unit::UnitBase::VT_MAPPING,      0, 1);
	dataHdlr(userObj, CSTR("Din 6"),       addr, 10007, IO::MODBUSController::DT_U8,   Math::Unit::UnitBase::VT_MAPPING,      0, 1);
	dataHdlr(userObj, CSTR("Din 7"),       addr, 10008, IO::MODBUSController::DT_U8,   Math::Unit::UnitBase::VT_MAPPING,      0, 1);
	dataHdlr(userObj, CSTR("Din 0 Count"), addr, 40001, IO::MODBUSController::DT_U16,  Math::Unit::UnitBase::VT_COUNT,        Math::Unit::Count::CU_UNIT, 1);
	dataHdlr(userObj, CSTR("Din 1 Count"), addr, 40002, IO::MODBUSController::DT_U16,  Math::Unit::UnitBase::VT_COUNT,        Math::Unit::Count::CU_UNIT, 1);
	dataHdlr(userObj, CSTR("Din 2 Count"), addr, 40003, IO::MODBUSController::DT_U16,  Math::Unit::UnitBase::VT_COUNT,        Math::Unit::Count::CU_UNIT, 1);
	dataHdlr(userObj, CSTR("Din 3 Count"), addr, 40004, IO::MODBUSController::DT_U16,  Math::Unit::UnitBase::VT_COUNT,        Math::Unit::Count::CU_UNIT, 1);
	dataHdlr(userObj, CSTR("Din 4 Count"), addr, 40005, IO::MODBUSController::DT_U16,  Math::Unit::UnitBase::VT_COUNT,        Math::Unit::Count::CU_UNIT, 1);
	dataHdlr(userObj, CSTR("Din 5 Count"), addr, 40006, IO::MODBUSController::DT_U16,  Math::Unit::UnitBase::VT_COUNT,        Math::Unit::Count::CU_UNIT, 1);
	dataHdlr(userObj, CSTR("Din 6 Count"), addr, 40007, IO::MODBUSController::DT_U16,  Math::Unit::UnitBase::VT_COUNT,        Math::Unit::Count::CU_UNIT, 1);
	dataHdlr(userObj, CSTR("Din 7 Count"), addr, 40008, IO::MODBUSController::DT_U16,  Math::Unit::UnitBase::VT_COUNT,        Math::Unit::Count::CU_UNIT, 1);
	dataHdlr(userObj, CSTR("RL0"),         addr, 00001, IO::MODBUSController::DT_U8,   Math::Unit::UnitBase::VT_MAPPING,      0, 1);
	dataHdlr(userObj, CSTR("RL1"),         addr, 00001, IO::MODBUSController::DT_U8,   Math::Unit::UnitBase::VT_MAPPING,      0, 1);
	dataHdlr(userObj, CSTR("RL2"),         addr, 00001, IO::MODBUSController::DT_U8,   Math::Unit::UnitBase::VT_MAPPING,      0, 1);
	dataHdlr(userObj, CSTR("RL3"),         addr, 00001, IO::MODBUSController::DT_U8,   Math::Unit::UnitBase::VT_MAPPING,      0, 1);
}
