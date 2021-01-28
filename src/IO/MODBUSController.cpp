#include "Stdafx.h"
#include "Data/ByteTool.h"
#include "IO/MODBUSController.h"
#include "Math/Math.h"
#include "Sync/Thread.h"
#include <stdio.h>

void __stdcall IO::MODBUSController::ReadResult(void *userObj, UInt8 funcCode, const UInt8 *result, OSInt resultSize)
{
	IO::MODBUSController *me = (IO::MODBUSController*)userObj;
	if (me->reqResult && funcCode == me->reqFuncCode && resultSize == me->reqResultSize)
	{
		MemCopyNO(me->reqResult, result, resultSize);
		me->reqHasResult = true;
		me->cbEvt->Set();
	}
}

void __stdcall IO::MODBUSController::SetResult(void *userObj, UInt8 funcCode, UInt16 startAddr, UInt16 cnt)
{
//	IO::MODBUSController *me = (IO::MODBUSController*)userObj;
/*	if (funcCode == 5 || funcCode == 6 || funcCode == 15 || funcCode == 16)
	{
		if (me->reqSetStartAddr == startAddr)
		{
			me->reqHasResult = true;
			me->cbEvt->Set();
		}
	}*/
}

Bool IO::MODBUSController::ReadRegister(UInt8 devAddr, UInt32 regAddr, UInt8 *resBuff, UInt16 resSize)
{
	Bool succ;
	this->reqMut->Lock();
	if (!this->devMap->Get(devAddr))
	{
		this->devMap->Put(devAddr, 1);
		this->modbus->HandleReadResult(devAddr, ReadResult, SetResult, this);
	}
	this->reqMut->Unlock();

	if (regAddr >= 30000 && regAddr < 40000)
	{
		this->reqMut->Lock();
		this->reqHasResult = false;
		this->reqResult = resBuff;
		this->reqFuncCode = 4;
		this->reqResultSize = resSize;
		this->cbEvt->Clear();
		this->modbus->ReadInputRegisters(devAddr, (UInt16)(regAddr - 30001), resSize >> 1);
		this->cbEvt->Wait(this->timeout);
		this->reqResult = 0;
		succ = this->reqHasResult;
		this->reqMut->Unlock();
		return succ;
	}
	else if (regAddr >= 40000 && regAddr < 50000)
	{
		this->reqMut->Lock();
		this->reqHasResult = false;
		this->reqResult = resBuff;
		this->reqFuncCode = 3;
		this->reqResultSize = resSize;
		this->cbEvt->Clear();
		this->modbus->ReadHoldingRegisters(devAddr, (UInt16)(regAddr - 40001), resSize >> 1);
		this->cbEvt->Wait(this->timeout);
		this->reqResult = 0;
		succ = this->reqHasResult;
		this->reqMut->Unlock();
		return succ;
	}
	else if (regAddr >= 300000 && regAddr < 400000)
	{
		this->reqMut->Lock();
		this->reqHasResult = false;
		this->reqResult = resBuff;
		this->reqFuncCode = 4;
		this->reqResultSize = resSize;
		this->cbEvt->Clear();
		this->modbus->ReadInputRegisters(devAddr, (UInt16)(regAddr - 300001), resSize >> 1);
		this->cbEvt->Wait(this->timeout);
		this->reqResult = 0;
		succ = this->reqHasResult;
		this->reqMut->Unlock();
		return succ;
	}
	else if (regAddr >= 400000 && regAddr < 500000)
	{
		this->reqMut->Lock();
		this->reqHasResult = false;
		this->reqResult = resBuff;
		this->reqFuncCode = 3;
		this->reqResultSize = resSize;
		this->cbEvt->Clear();
		this->modbus->ReadHoldingRegisters(devAddr, (UInt16)(regAddr - 400001), resSize >> 1);
		this->cbEvt->Wait(this->timeout);
		this->reqResult = 0;
		succ = this->reqHasResult;
		this->reqMut->Unlock();
		return succ;
	}
	else
	{
		return false;
	}
}

IO::MODBUSController::MODBUSController(IO::MODBUSMaster *modbus)
{
	this->modbus = modbus;
	this->timeout = 200;
	NEW_CLASS(this->cbEvt, Sync::Event(true, (const UTF8Char*)"IO.MODBUSDevice.cbEvt"));
	NEW_CLASS(this->reqMut, Sync::Mutex());
	NEW_CLASS(this->devMap, Data::Integer32Map<UInt8>());
	this->reqResult = 0;
	this->reqSetStartAddr = 0;
	this->reqSetCount = 0;
}

IO::MODBUSController::~MODBUSController()
{
	Data::ArrayList<Int32> *devList = this->devMap->GetKeys();
	OSInt i = devList->GetCount();
	while (i-- > 0)
	{
		this->modbus->HandleReadResult((UInt8)devList->GetItem(i), 0, 0, 0);
	}
	DEL_CLASS(this->devMap);
	DEL_CLASS(this->cbEvt);
	DEL_CLASS(this->reqMut);
}

void IO::MODBUSController::SetTimeout(UOSInt timeout)
{
	this->timeout = timeout;
}

Bool IO::MODBUSController::ReadRegisterI32(UInt8 devAddr, UInt32 regAddr, Int32 *outVal)
{
	UInt8 resBuff[4];
	Bool succ = this->ReadRegister(devAddr, regAddr, resBuff, 4);
	if (succ)
	{
		*outVal = ReadMInt32(resBuff);
	}
	return succ;
}

Bool IO::MODBUSController::ReadRegisterII32(UInt8 devAddr, UInt32 regAddr, Int32 *outVal)
{
	UInt8 resBuff[4];
	Bool succ = this->ReadRegister(devAddr, regAddr, resBuff, 4);
	if (succ)
	{
		*outVal = ReadInt32(resBuff);
	}
	return succ;
}

Bool IO::MODBUSController::ReadRegisterF32(UInt8 devAddr, UInt32 regAddr, Single *outVal)
{
	UInt8 resBuff[4];
	Bool succ = this->ReadRegister(devAddr, regAddr, resBuff, 4);
	if (succ)
	{
		*outVal = ReadMFloat(resBuff);
	}
	return succ;
}

Bool IO::MODBUSController::ReadRegisterU16(UInt8 devAddr, UInt32 regAddr, UInt16 *outVal)
{
	UInt8 resBuff[2];
	Bool succ = this->ReadRegister(devAddr, regAddr, resBuff, 2);
	if (succ)
	{
		*outVal = ReadMUInt16(resBuff);
	}
	return succ;
}

Bool IO::MODBUSController::ReadRegisterIU16(UInt8 devAddr, UInt32 regAddr, UInt16 *outVal)
{
	UInt8 resBuff[2];
	Bool succ = this->ReadRegister(devAddr, regAddr, resBuff, 2);
	if (succ)
	{
		*outVal = ReadUInt16(resBuff);
	}
	return succ;
}
