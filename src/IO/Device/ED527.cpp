#include "Stdafx.h"
#include "IO/Device/ED527.h"
#include "Math/Math.h"
#include "Math/Unit/Count.h"

IO::Device::ED527::ED527(NotNullPtr<IO::MODBUSMaster> modbus, UInt8 addr) : IO::MODBUSDevice(modbus, addr)
{
}

IO::Device::ED527::~ED527()
{
}

Bool IO::Device::ED527::IsRelayHigh(UInt16 index)
{
	if (index >= 16)
		return false;
	return this->ReadCoil(index);
}

Bool IO::Device::ED527::SetRelayState(UInt16 index, Bool isHigh)
{
	if (index >= 16)
		return false;
	return this->WriteDOutput(index, isHigh);
}

Bool IO::Device::ED527::GetOutputOverloadFlag(UInt16 diNum)
{
	if (diNum >= 16)
		return false;
	return this->ReadDInput((UInt16)(0x400 + diNum));
}

void IO::Device::ED527::GetDataEntries(UInt8 addr, MODBUSDataEntry dataHdlr, AnyType userObj)
{
	dataHdlr(userObj, CSTR("RL0"),         addr,     1, IO::MODBUSController::DT_U8,   Math::Unit::UnitBase::ValueType::Mapping,      0, 1);
	dataHdlr(userObj, CSTR("RL1"),         addr,     2, IO::MODBUSController::DT_U8,   Math::Unit::UnitBase::ValueType::Mapping,      0, 1);
	dataHdlr(userObj, CSTR("RL2"),         addr,     3, IO::MODBUSController::DT_U8,   Math::Unit::UnitBase::ValueType::Mapping,      0, 1);
	dataHdlr(userObj, CSTR("RL3"),         addr,     4, IO::MODBUSController::DT_U8,   Math::Unit::UnitBase::ValueType::Mapping,      0, 1);
	dataHdlr(userObj, CSTR("RL4"),         addr,     5, IO::MODBUSController::DT_U8,   Math::Unit::UnitBase::ValueType::Mapping,      0, 1);
	dataHdlr(userObj, CSTR("RL5"),         addr,     6, IO::MODBUSController::DT_U8,   Math::Unit::UnitBase::ValueType::Mapping,      0, 1);
	dataHdlr(userObj, CSTR("RL6"),         addr,     7, IO::MODBUSController::DT_U8,   Math::Unit::UnitBase::ValueType::Mapping,      0, 1);
	dataHdlr(userObj, CSTR("RL7"),         addr,     8, IO::MODBUSController::DT_U8,   Math::Unit::UnitBase::ValueType::Mapping,      0, 1);
	dataHdlr(userObj, CSTR("RL8"),         addr,     9, IO::MODBUSController::DT_U8,   Math::Unit::UnitBase::ValueType::Mapping,      0, 1);
	dataHdlr(userObj, CSTR("RL9"),         addr,    10, IO::MODBUSController::DT_U8,   Math::Unit::UnitBase::ValueType::Mapping,      0, 1);
	dataHdlr(userObj, CSTR("RL10"),        addr,    11, IO::MODBUSController::DT_U8,   Math::Unit::UnitBase::ValueType::Mapping,      0, 1);
	dataHdlr(userObj, CSTR("RL11"),        addr,    12, IO::MODBUSController::DT_U8,   Math::Unit::UnitBase::ValueType::Mapping,      0, 1);
	dataHdlr(userObj, CSTR("RL12"),        addr,    13, IO::MODBUSController::DT_U8,   Math::Unit::UnitBase::ValueType::Mapping,      0, 1);
	dataHdlr(userObj, CSTR("RL13"),        addr,    14, IO::MODBUSController::DT_U8,   Math::Unit::UnitBase::ValueType::Mapping,      0, 1);
	dataHdlr(userObj, CSTR("RL14"),        addr,    15, IO::MODBUSController::DT_U8,   Math::Unit::UnitBase::ValueType::Mapping,      0, 1);
	dataHdlr(userObj, CSTR("RL15"),        addr,    16, IO::MODBUSController::DT_U8,   Math::Unit::UnitBase::ValueType::Mapping,      0, 1);
}
