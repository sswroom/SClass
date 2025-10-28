#include "Stdafx.h"
#include "IO/Device/ED588.h"
#include "Math/Math_C.h"
#include "Math/Unit/Count.h"

IO::Device::ED588::ED588(NN<IO::MODBUSMaster> modbus, UInt8 addr) : IO::MODBUSDevice(modbus, addr)
{
}

IO::Device::ED588::~ED588()
{
}

Bool IO::Device::ED588::IsDIHighByCoil(UInt16 diNum)
{
	if (diNum >= 8)
		return false;
	return this->ReadCoil((UInt16)(diNum + 0x20));
}

Bool IO::Device::ED588::IsDIHighByInput(UInt16 diNum)
{
	if (diNum >= 8)
		return false;
	return this->ReadDInput(diNum);
}

Bool IO::Device::ED588::IsDIHighByReg(UInt16 diNum)
{
	if (diNum >= 8)
		return false;
	Int32 val;
	if (!this->ReadInputI16((UInt16)(diNum + 0x20), val))
		return false;
	return val != 0;
}

UInt16 IO::Device::ED588::GetDICountByReg(UInt16 diNum)
{
	if (diNum >= 8)
		return 0;
	Int32 val;
	if (!this->ReadInputI16(diNum, val))
		return 0;
	return (UInt16)val;
}

Bool IO::Device::ED588::GetDICountByReg(UInt16 diNum, OutParam<Int32> val)
{
	if (diNum >= 8)
		return false;
	return this->ReadInputI16(diNum, val);
}

UInt16 IO::Device::ED588::GetDICountByHolding(UInt16 diNum)
{
	if (diNum >= 8)
		return 0;
	Int32 val;
	if (!this->ReadHoldingI16(diNum, val))
		return 0;
	return (UInt16)val;
}

Bool IO::Device::ED588::ClearDICount(UInt16 diNum)
{
	if (diNum >= 8)
		return false;
	return this->WriteDOutput((UInt16)(0x200 + diNum), true);
}

Bool IO::Device::ED588::IsRelayHigh(UInt16 index)
{
	if (index >= 8)
		return false;
	return this->ReadCoil(index);
}

Bool IO::Device::ED588::SetRelayState(UInt16 index, Bool isHigh)
{
	if (index >= 8)
		return false;
	return this->WriteDOutput(index, isHigh);
}

Bool IO::Device::ED588::GetOutputOverloadFlag(UInt16 diNum)
{
	if (diNum >= 8)
		return false;
	return this->ReadDInput((UInt16)(0x400 + diNum));
}

void IO::Device::ED588::GetDataEntries(UInt8 addr, MODBUSDataEntry dataHdlr, AnyType userObj)
{
	dataHdlr(userObj, CSTR("Din 0"),       addr, 10001, IO::MODBUSController::DT_U8,   Math::Unit::UnitBase::ValueType::Mapping,      0, 1);
	dataHdlr(userObj, CSTR("Din 1"),       addr, 10002, IO::MODBUSController::DT_U8,   Math::Unit::UnitBase::ValueType::Mapping,      0, 1);
	dataHdlr(userObj, CSTR("Din 2"),       addr, 10003, IO::MODBUSController::DT_U8,   Math::Unit::UnitBase::ValueType::Mapping,      0, 1);
	dataHdlr(userObj, CSTR("Din 3"),       addr, 10004, IO::MODBUSController::DT_U8,   Math::Unit::UnitBase::ValueType::Mapping,      0, 1);
	dataHdlr(userObj, CSTR("Din 4"),       addr, 10005, IO::MODBUSController::DT_U8,   Math::Unit::UnitBase::ValueType::Mapping,      0, 1);
	dataHdlr(userObj, CSTR("Din 5"),       addr, 10006, IO::MODBUSController::DT_U8,   Math::Unit::UnitBase::ValueType::Mapping,      0, 1);
	dataHdlr(userObj, CSTR("Din 6"),       addr, 10007, IO::MODBUSController::DT_U8,   Math::Unit::UnitBase::ValueType::Mapping,      0, 1);
	dataHdlr(userObj, CSTR("Din 7"),       addr, 10008, IO::MODBUSController::DT_U8,   Math::Unit::UnitBase::ValueType::Mapping,      0, 1);
	dataHdlr(userObj, CSTR("Din 0 Count"), addr, 40001, IO::MODBUSController::DT_U16,  Math::Unit::UnitBase::ValueType::Count,        Math::Unit::Count::CU_UNIT, 1);
	dataHdlr(userObj, CSTR("Din 1 Count"), addr, 40002, IO::MODBUSController::DT_U16,  Math::Unit::UnitBase::ValueType::Count,        Math::Unit::Count::CU_UNIT, 1);
	dataHdlr(userObj, CSTR("Din 2 Count"), addr, 40003, IO::MODBUSController::DT_U16,  Math::Unit::UnitBase::ValueType::Count,        Math::Unit::Count::CU_UNIT, 1);
	dataHdlr(userObj, CSTR("Din 3 Count"), addr, 40004, IO::MODBUSController::DT_U16,  Math::Unit::UnitBase::ValueType::Count,        Math::Unit::Count::CU_UNIT, 1);
	dataHdlr(userObj, CSTR("Din 4 Count"), addr, 40005, IO::MODBUSController::DT_U16,  Math::Unit::UnitBase::ValueType::Count,        Math::Unit::Count::CU_UNIT, 1);
	dataHdlr(userObj, CSTR("Din 5 Count"), addr, 40006, IO::MODBUSController::DT_U16,  Math::Unit::UnitBase::ValueType::Count,        Math::Unit::Count::CU_UNIT, 1);
	dataHdlr(userObj, CSTR("Din 6 Count"), addr, 40007, IO::MODBUSController::DT_U16,  Math::Unit::UnitBase::ValueType::Count,        Math::Unit::Count::CU_UNIT, 1);
	dataHdlr(userObj, CSTR("Din 7 Count"), addr, 40008, IO::MODBUSController::DT_U16,  Math::Unit::UnitBase::ValueType::Count,        Math::Unit::Count::CU_UNIT, 1);
	dataHdlr(userObj, CSTR("RL0"),         addr,     1, IO::MODBUSController::DT_U8,   Math::Unit::UnitBase::ValueType::Mapping,      0, 1);
	dataHdlr(userObj, CSTR("RL1"),         addr,     2, IO::MODBUSController::DT_U8,   Math::Unit::UnitBase::ValueType::Mapping,      0, 1);
	dataHdlr(userObj, CSTR("RL2"),         addr,     3, IO::MODBUSController::DT_U8,   Math::Unit::UnitBase::ValueType::Mapping,      0, 1);
	dataHdlr(userObj, CSTR("RL3"),         addr,     4, IO::MODBUSController::DT_U8,   Math::Unit::UnitBase::ValueType::Mapping,      0, 1);
	dataHdlr(userObj, CSTR("RL4"),         addr,     5, IO::MODBUSController::DT_U8,   Math::Unit::UnitBase::ValueType::Mapping,      0, 1);
	dataHdlr(userObj, CSTR("RL5"),         addr,     6, IO::MODBUSController::DT_U8,   Math::Unit::UnitBase::ValueType::Mapping,      0, 1);
	dataHdlr(userObj, CSTR("RL6"),         addr,     7, IO::MODBUSController::DT_U8,   Math::Unit::UnitBase::ValueType::Mapping,      0, 1);
	dataHdlr(userObj, CSTR("RL7"),         addr,     8, IO::MODBUSController::DT_U8,   Math::Unit::UnitBase::ValueType::Mapping,      0, 1);
}
