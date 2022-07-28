#include "Stdafx.h"
#include "IO/Device/ED516.h"
#include "Math/Math.h"
#include "Math/Unit/Count.h"

IO::Device::ED516::ED516(IO::MODBUSMaster *modbus, UInt8 addr) : IO::MODBUSDevice(modbus, addr)
{
}

IO::Device::ED516::~ED516()
{
}

Bool IO::Device::ED516::IsDIHighByCoil(UInt16 diNum)
{
	if (diNum >= 16)
		return false;
	return this->ReadCoil((UInt16)(diNum + 0x20));
}

Bool IO::Device::ED516::IsDIHighByInput(UInt16 diNum)
{
	if (diNum >= 16)
		return false;
	return this->ReadDInput(diNum);
}

Bool IO::Device::ED516::IsDIHighByReg(UInt16 diNum)
{
	if (diNum >= 16)
		return false;
	Int32 val;
	if (!this->ReadInputI16((UInt16)(diNum + 0x20), &val))
		return false;
	return val != 0;
}

UInt16 IO::Device::ED516::GetDICountByReg(UInt16 diNum)
{
	if (diNum >= 16)
		return 0;
	Int32 val;
	if (!this->ReadInputI16(diNum, &val))
		return 0;
	return (UInt16)val;
}

Bool IO::Device::ED516::GetDICountByReg(UInt16 diNum, Int32 *val)
{
	if (diNum >= 16)
		return false;
	return this->ReadInputI16(diNum, val);
}

UInt16 IO::Device::ED516::GetDICountByHolding(UInt16 diNum)
{
	if (diNum >= 16)
		return 0;
	Int32 val;
	if (!this->ReadHoldingI16(diNum, &val))
		return 0;
	return (UInt16)val;
}

Bool IO::Device::ED516::ClearDICount(UInt16 diNum)
{
	if (diNum >= 16)
		return false;
	return this->WriteDOutput((UInt16)(0x200 + diNum), true);
}

void IO::Device::ED516::GetDataEntries(UInt8 addr, MODBUSDataEntry dataHdlr, void *userObj)
{
	dataHdlr(userObj, CSTR("Din 0"),        addr, 10001, IO::MODBUSController::DT_U8,   Math::Unit::UnitBase::VT_MAPPING,      0, 1);
	dataHdlr(userObj, CSTR("Din 1"),        addr, 10002, IO::MODBUSController::DT_U8,   Math::Unit::UnitBase::VT_MAPPING,      0, 1);
	dataHdlr(userObj, CSTR("Din 2"),        addr, 10003, IO::MODBUSController::DT_U8,   Math::Unit::UnitBase::VT_MAPPING,      0, 1);
	dataHdlr(userObj, CSTR("Din 3"),        addr, 10004, IO::MODBUSController::DT_U8,   Math::Unit::UnitBase::VT_MAPPING,      0, 1);
	dataHdlr(userObj, CSTR("Din 4"),        addr, 10005, IO::MODBUSController::DT_U8,   Math::Unit::UnitBase::VT_MAPPING,      0, 1);
	dataHdlr(userObj, CSTR("Din 5"),        addr, 10006, IO::MODBUSController::DT_U8,   Math::Unit::UnitBase::VT_MAPPING,      0, 1);
	dataHdlr(userObj, CSTR("Din 6"),        addr, 10007, IO::MODBUSController::DT_U8,   Math::Unit::UnitBase::VT_MAPPING,      0, 1);
	dataHdlr(userObj, CSTR("Din 7"),        addr, 10008, IO::MODBUSController::DT_U8,   Math::Unit::UnitBase::VT_MAPPING,      0, 1);
	dataHdlr(userObj, CSTR("Din 8"),        addr, 10009, IO::MODBUSController::DT_U8,   Math::Unit::UnitBase::VT_MAPPING,      0, 1);
	dataHdlr(userObj, CSTR("Din 9"),        addr, 10010, IO::MODBUSController::DT_U8,   Math::Unit::UnitBase::VT_MAPPING,      0, 1);
	dataHdlr(userObj, CSTR("Din 10"),       addr, 10011, IO::MODBUSController::DT_U8,   Math::Unit::UnitBase::VT_MAPPING,      0, 1);
	dataHdlr(userObj, CSTR("Din 11"),       addr, 10012, IO::MODBUSController::DT_U8,   Math::Unit::UnitBase::VT_MAPPING,      0, 1);
	dataHdlr(userObj, CSTR("Din 12"),       addr, 10013, IO::MODBUSController::DT_U8,   Math::Unit::UnitBase::VT_MAPPING,      0, 1);
	dataHdlr(userObj, CSTR("Din 13"),       addr, 10014, IO::MODBUSController::DT_U8,   Math::Unit::UnitBase::VT_MAPPING,      0, 1);
	dataHdlr(userObj, CSTR("Din 14"),       addr, 10015, IO::MODBUSController::DT_U8,   Math::Unit::UnitBase::VT_MAPPING,      0, 1);
	dataHdlr(userObj, CSTR("Din 15"),       addr, 10016, IO::MODBUSController::DT_U8,   Math::Unit::UnitBase::VT_MAPPING,      0, 1);
	dataHdlr(userObj, CSTR("Din 0 Count"),  addr, 40001, IO::MODBUSController::DT_U16,  Math::Unit::UnitBase::VT_COUNT,        Math::Unit::Count::CU_UNIT, 1);
	dataHdlr(userObj, CSTR("Din 1 Count"),  addr, 40002, IO::MODBUSController::DT_U16,  Math::Unit::UnitBase::VT_COUNT,        Math::Unit::Count::CU_UNIT, 1);
	dataHdlr(userObj, CSTR("Din 2 Count"),  addr, 40003, IO::MODBUSController::DT_U16,  Math::Unit::UnitBase::VT_COUNT,        Math::Unit::Count::CU_UNIT, 1);
	dataHdlr(userObj, CSTR("Din 3 Count"),  addr, 40004, IO::MODBUSController::DT_U16,  Math::Unit::UnitBase::VT_COUNT,        Math::Unit::Count::CU_UNIT, 1);
	dataHdlr(userObj, CSTR("Din 4 Count"),  addr, 40005, IO::MODBUSController::DT_U16,  Math::Unit::UnitBase::VT_COUNT,        Math::Unit::Count::CU_UNIT, 1);
	dataHdlr(userObj, CSTR("Din 5 Count"),  addr, 40006, IO::MODBUSController::DT_U16,  Math::Unit::UnitBase::VT_COUNT,        Math::Unit::Count::CU_UNIT, 1);
	dataHdlr(userObj, CSTR("Din 6 Count"),  addr, 40007, IO::MODBUSController::DT_U16,  Math::Unit::UnitBase::VT_COUNT,        Math::Unit::Count::CU_UNIT, 1);
	dataHdlr(userObj, CSTR("Din 7 Count"),  addr, 40008, IO::MODBUSController::DT_U16,  Math::Unit::UnitBase::VT_COUNT,        Math::Unit::Count::CU_UNIT, 1);
	dataHdlr(userObj, CSTR("Din 8 Count"),  addr, 40009, IO::MODBUSController::DT_U16,  Math::Unit::UnitBase::VT_COUNT,        Math::Unit::Count::CU_UNIT, 1);
	dataHdlr(userObj, CSTR("Din 9 Count"),  addr, 40010, IO::MODBUSController::DT_U16,  Math::Unit::UnitBase::VT_COUNT,        Math::Unit::Count::CU_UNIT, 1);
	dataHdlr(userObj, CSTR("Din 10 Count"), addr, 40011, IO::MODBUSController::DT_U16,  Math::Unit::UnitBase::VT_COUNT,        Math::Unit::Count::CU_UNIT, 1);
	dataHdlr(userObj, CSTR("Din 11 Count"), addr, 40012, IO::MODBUSController::DT_U16,  Math::Unit::UnitBase::VT_COUNT,        Math::Unit::Count::CU_UNIT, 1);
	dataHdlr(userObj, CSTR("Din 12 Count"), addr, 40013, IO::MODBUSController::DT_U16,  Math::Unit::UnitBase::VT_COUNT,        Math::Unit::Count::CU_UNIT, 1);
	dataHdlr(userObj, CSTR("Din 13 Count"), addr, 40014, IO::MODBUSController::DT_U16,  Math::Unit::UnitBase::VT_COUNT,        Math::Unit::Count::CU_UNIT, 1);
	dataHdlr(userObj, CSTR("Din 14 Count"), addr, 40015, IO::MODBUSController::DT_U16,  Math::Unit::UnitBase::VT_COUNT,        Math::Unit::Count::CU_UNIT, 1);
	dataHdlr(userObj, CSTR("Din 15 Count"), addr, 40016, IO::MODBUSController::DT_U16,  Math::Unit::UnitBase::VT_COUNT,        Math::Unit::Count::CU_UNIT, 1);
}
