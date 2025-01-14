#include "Stdafx.h"
#include "Data/ByteTool.h"
#include "IO/MODBUSController.h"
#include "Math/Math.h"
#include "Sync/MutexUsage.h"
#include "Sync/ThreadUtil.h"

void __stdcall IO::MODBUSController::ReadResult(AnyType userObj, UInt8 funcCode, UnsafeArray<const UInt8> result, UOSInt resultSize)
{
	NN<IO::MODBUSController> me = userObj.GetNN<IO::MODBUSController>();
	UnsafeArray<UInt8> reqResult;
	if (me->reqResult.SetTo(reqResult) && funcCode == me->reqFuncCode && resultSize == me->reqResultSize)
	{
		MemCopyNO(reqResult.Ptr(), result.Ptr(), resultSize);
		me->reqHasResult = true;
		me->cbEvt.Set();
	}
}

void __stdcall IO::MODBUSController::SetResult(AnyType userObj, UInt8 funcCode, UInt16 startAddr, UInt16 cnt)
{
	NN<IO::MODBUSController> me = userObj.GetNN<IO::MODBUSController>();
	if (me->reqResult.NotNull() && funcCode == me->reqFuncCode && me->reqSetStartAddr == startAddr)
	{
		me->reqHasResult = true;
		me->cbEvt.Set();
	}
}

Bool IO::MODBUSController::ReadRegister(UInt8 devAddr, UInt32 regAddr, UnsafeArray<UInt8> resBuff, UInt16 resSize)
{
	Bool succ;
	Sync::MutexUsage mutUsage(this->reqMut);
	if (!this->devMap.Get(devAddr))
	{
		this->devMap.Put(devAddr, 1);
		this->modbus->HandleReadResult(devAddr, ReadResult, SetResult, this);
	}
	mutUsage.EndUse();

	if (regAddr > 0 && regAddr < 10000)
	{
		mutUsage.BeginUse();
		this->reqHasResult = false;
		this->reqResult = resBuff;
		this->reqFuncCode = 1;
		this->reqResultSize = resSize;
		this->cbEvt.Clear();
		this->modbus->ReadCoils(devAddr, (UInt16)(regAddr - 1), resSize);
		this->cbEvt.Wait(this->timeout);
		this->reqResult = 0;
		succ = this->reqHasResult;
		mutUsage.EndUse();
		return succ;
	}
	else if (regAddr > 10000 && regAddr < 20000)
	{
		mutUsage.BeginUse();
		this->reqHasResult = false;
		this->reqResult = resBuff;
		this->reqFuncCode = 2;
		this->reqResultSize = resSize;
		this->cbEvt.Clear();
		this->modbus->ReadInputs(devAddr, (UInt16)(regAddr - 10001), resSize);
		this->cbEvt.Wait(this->timeout);
		this->reqResult = 0;
		succ = this->reqHasResult;
		mutUsage.EndUse();
		return succ;
	}
	else if (regAddr >= 30000 && regAddr < 40000)
	{
		mutUsage.BeginUse();
		this->reqHasResult = false;
		this->reqResult = resBuff;
		this->reqFuncCode = 4;
		this->reqResultSize = resSize;
		this->cbEvt.Clear();
		this->modbus->ReadInputRegisters(devAddr, (UInt16)(regAddr - 30001), (UInt16)(resSize >> 1));
		this->cbEvt.Wait(this->timeout);
		this->reqResult = 0;
		succ = this->reqHasResult;
		mutUsage.EndUse();
		return succ;
	}
	else if (regAddr >= 40000 && regAddr < 50000)
	{
		mutUsage.BeginUse();
		this->reqHasResult = false;
		this->reqResult = resBuff;
		this->reqFuncCode = 3;
		this->reqResultSize = resSize;
		this->cbEvt.Clear();
		this->modbus->ReadHoldingRegisters(devAddr, (UInt16)(regAddr - 40001), (UInt16)(resSize >> 1));
		this->cbEvt.Wait(this->timeout);
		this->reqResult = 0;
		succ = this->reqHasResult;
		mutUsage.EndUse();
		return succ;
	}
	else if (regAddr >= 300000 && regAddr < 400000)
	{
		mutUsage.BeginUse();
		this->reqHasResult = false;
		this->reqResult = resBuff;
		this->reqFuncCode = 4;
		this->reqResultSize = resSize;
		this->cbEvt.Clear();
		this->modbus->ReadInputRegisters(devAddr, (UInt16)(regAddr - 300001), (UInt16)(resSize >> 1));
		this->cbEvt.Wait(this->timeout);
		this->reqResult = 0;
		succ = this->reqHasResult;
		mutUsage.EndUse();
		return succ;
	}
	else if (regAddr >= 400000 && regAddr < 500000)
	{
		mutUsage.BeginUse();
		this->reqHasResult = false;
		this->reqResult = resBuff;
		this->reqFuncCode = 3;
		this->reqResultSize = resSize;
		this->cbEvt.Clear();
		this->modbus->ReadHoldingRegisters(devAddr, (UInt16)(regAddr - 400001), (UInt16)(resSize >> 1));
		this->cbEvt.Wait(this->timeout);
		this->reqResult = 0;
		succ = this->reqHasResult;
		mutUsage.EndUse();
		return succ;
	}
	else
	{
		return false;
	}
}

Bool IO::MODBUSController::WriteRegister(UInt8 devAddr, UInt32 regAddr, UnsafeArray<UInt8> regBuff, UInt16 regSize)
{
	Bool succ;
	Sync::MutexUsage mutUsage(this->reqMut);
	if (!this->devMap.Get(devAddr))
	{
		this->devMap.Put(devAddr, 1);
		this->modbus->HandleReadResult(devAddr, ReadResult, SetResult, this);
	}
	mutUsage.EndUse();

	if (regAddr > 0 && regAddr < 10000)
	{
		if (regSize == 1)
		{
			mutUsage.BeginUse();
			this->reqHasResult = false;
			this->reqResult = regBuff;
			this->reqFuncCode = 5;
			this->reqSetStartAddr = (UInt16)(regAddr - 1);
			this->cbEvt.Clear();
			this->modbus->WriteCoil(devAddr, this->reqSetStartAddr, regBuff[0] != 0);
			this->cbEvt.Wait(this->timeout);
			this->reqResult = 0;
			succ = this->reqHasResult;
			mutUsage.EndUse();
			return succ;
		}
		return false;
	}
	else if (regAddr > 10000 && regAddr < 20000)
	{
		return false;
	}
	else if (regAddr >= 30000 && regAddr < 40000)
	{
		return false;
	}
	else if (regAddr >= 40000 && regAddr < 50000)
	{
		mutUsage.BeginUse();
		this->reqHasResult = false;
		this->reqResult = regBuff;
		this->reqFuncCode = 6;
		this->reqSetStartAddr = (UInt16)(regAddr - 40001);
		this->cbEvt.Clear();
		this->modbus->WriteHoldingRegisters(devAddr, this->reqSetStartAddr, (UInt16)(regSize >> 1), regBuff);
		this->cbEvt.Wait(this->timeout);
		this->reqResult = 0;
		succ = this->reqHasResult;
		mutUsage.EndUse();
		return succ;
	}
	else if (regAddr >= 300000 && regAddr < 400000)
	{
		return false;
	}
	else if (regAddr >= 400000 && regAddr < 500000)
	{
		mutUsage.BeginUse();
		this->reqHasResult = false;
		this->reqResult = regBuff;
		this->reqFuncCode = 6;
		this->reqSetStartAddr = (UInt16)(regAddr - 400001);
		this->cbEvt.Clear();
		this->modbus->WriteHoldingRegisters(devAddr, this->reqSetStartAddr, (UInt16)(regSize >> 1), regBuff);
		this->cbEvt.Wait(this->timeout);
		this->reqResult = 0;
		succ = this->reqHasResult;
		mutUsage.EndUse();
		return succ;
	}
	else
	{
		return false;
	}
}

IO::MODBUSController::MODBUSController(NN<IO::MODBUSMaster> modbus)
{
	this->modbus = modbus;
	this->timeout = 200;
	this->reqResult = 0;
	this->reqSetStartAddr = 0;
	this->reqSetCount = 0;
}

IO::MODBUSController::~MODBUSController()
{
	UOSInt i = this->devMap.GetCount();
	while (i-- > 0)
	{
		this->modbus->HandleReadResult(this->devMap.GetKey(i), 0, 0, 0);
	}
}

void IO::MODBUSController::SetTimeout(Data::Duration timeout)
{
	this->timeout = timeout;
}

Bool IO::MODBUSController::ReadRegisterI32(UInt8 devAddr, UInt32 regAddr, OutParam<Int32> outVal)
{
	UInt8 resBuff[4];
	Bool succ = this->ReadRegister(devAddr, regAddr, resBuff, 4);
	if (succ)
	{
		outVal.Set(ReadMInt32(resBuff));
	}
	return succ;
}

Bool IO::MODBUSController::ReadRegisterII32(UInt8 devAddr, UInt32 regAddr, OutParam<Int32> outVal)
{
	UInt8 resBuff[4];
	Bool succ = this->ReadRegister(devAddr, regAddr, resBuff, 4);
	if (succ)
	{
		outVal.Set(ReadInt32(resBuff));
	}
	return succ;
}

Bool IO::MODBUSController::ReadRegisterF32(UInt8 devAddr, UInt32 regAddr, OutParam<Single> outVal)
{
	UInt8 resBuff[4];
	Bool succ = this->ReadRegister(devAddr, regAddr, resBuff, 4);
	if (succ)
	{
		outVal.Set(ReadMFloat(resBuff));
	}
	return succ;
}

Bool IO::MODBUSController::ReadRegisterU16(UInt8 devAddr, UInt32 regAddr, OutParam<UInt16> outVal)
{
	UInt8 resBuff[2];
	Bool succ = this->ReadRegister(devAddr, regAddr, resBuff, 2);
	if (succ)
	{
		outVal.Set(ReadMUInt16(resBuff));
	}
	return succ;
}

Bool IO::MODBUSController::ReadRegisterIU16(UInt8 devAddr, UInt32 regAddr, OutParam<UInt16> outVal)
{
	UInt8 resBuff[2];
	Bool succ = this->ReadRegister(devAddr, regAddr, resBuff, 2);
	if (succ)
	{
		outVal.Set(ReadUInt16(resBuff));
	}
	return succ;
}

Bool IO::MODBUSController::ReadRegisterU8(UInt8 devAddr, UInt32 regAddr, OutParam<UInt8> outVal)
{
	UInt8 tmpBuff;
	Bool succ = this->ReadRegister(devAddr, regAddr, &tmpBuff, 1);
	if (succ)
	{
		outVal.Set(tmpBuff);
	}
	return succ;
}

Bool IO::MODBUSController::ReadRegisterU8Arr(UInt8 devAddr, UInt32 regAddr, UnsafeArray<UInt8> outVal, UInt16 valCnt)
{
	return this->ReadRegister(devAddr, regAddr, outVal, valCnt);
}

Bool IO::MODBUSController::WriteRegisterBool(UInt8 devAddr, UInt32 regAddr, Bool val)
{
	UInt8 reg = val?1:0;
	return this->WriteRegister(devAddr, regAddr, &reg, 1);
}
