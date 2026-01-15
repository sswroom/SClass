#include "Stdafx.h"
#include "Core/ByteTool_C.h"
#include "IO/MODBUSDevice.h"
#include "Math/Math_C.h"
#include "Sync/MutexUsage.h"
#include "Sync/ThreadUtil.h"

void __stdcall IO::MODBUSDevice::ReadResult(AnyType userObj, UInt8 funcCode, UnsafeArray<const UInt8> result, UOSInt resultSize)
{
	NN<IO::MODBUSDevice> me = userObj.GetNN<IO::MODBUSDevice>();
	UnsafeArray<UInt8> reqBResult;
	if (funcCode == 4)
	{
		if (me->reqBResult.SetTo(reqBResult))
		{
			reqBResult.CopyFromNO(result, resultSize);
			me->reqHasResult = true;
			me->cbEvt.Set();
		}
		else if (resultSize == 4)
		{
			if (me->reqDResult.IsNotNull())
			{
				me->reqDResult.SetNoCheck(ReadMFloat(&result[0]));
				me->reqHasResult = true;
				me->cbEvt.Set();
			}
			else if (me->reqIResult.IsNotNull())
			{
				me->reqIResult.SetNoCheck(ReadMInt32(&result[0]));
				me->reqHasResult = true;
				me->cbEvt.Set();
			}
		}
		else if (resultSize == 2)
		{
			if (me->reqIResult.IsNotNull())
			{
				me->reqIResult.SetNoCheck(ReadMUInt16(&result[0]));
				me->reqHasResult = true;
				me->cbEvt.Set();
			}
		}
	}
	else if (funcCode == 3)
	{
		if (me->reqBResult.SetTo(reqBResult))
		{
			reqBResult.CopyFromNO(result, resultSize);
			me->reqHasResult = true;
			me->cbEvt.Set();
		}
		else if (resultSize == 4)
		{
			if (me->reqDResult.IsNotNull())
			{
				me->reqDResult.SetNoCheck(ReadMFloat(&result[0]));
				me->reqHasResult = true;
				me->cbEvt.Set();
			}
			else if (me->reqIResult.IsNotNull())
			{
				me->reqIResult.SetNoCheck(ReadMInt32(&result[0]));
				me->reqHasResult = true;
				me->cbEvt.Set();
			}
		}
		else if (resultSize == 2)
		{
			if (me->reqIResult.IsNotNull())
			{
				me->reqIResult.SetNoCheck(ReadMUInt16(&result[0]));
				me->reqHasResult = true;
				me->cbEvt.Set();
			}
		}
	}
	else if (funcCode == 2)
	{
		if (me->reqIResult.IsNotNull())
		{
			if (resultSize == 1)
			{
				me->reqIResult.SetNoCheck(result[0]);
				me->reqHasResult = true;
				me->cbEvt.Set();
			}
			else if (resultSize == 2)
			{
				me->reqIResult.SetNoCheck(ReadUInt16(&result[0]));
				me->reqHasResult = true;
				me->cbEvt.Set();
			}
			else if (resultSize == 3)
			{
				me->reqIResult.SetNoCheck((Int32)ReadUInt24(&result[0]));
				me->reqHasResult = true;
				me->cbEvt.Set();
			}
			else if (resultSize == 4)
			{
				me->reqIResult.SetNoCheck(ReadInt32(&result[0]));
				me->reqHasResult = true;
				me->cbEvt.Set();
			}
		}
	}
	else if (funcCode == 1)
	{
		if (me->reqIResult.IsNotNull())
		{
			if (resultSize == 1)
			{
				me->reqIResult.SetNoCheck(result[0]);
				me->reqHasResult = true;
				me->cbEvt.Set();
			}
			else if (resultSize == 2)
			{
				me->reqIResult.SetNoCheck(ReadUInt16(&result[0]));
				me->reqHasResult = true;
				me->cbEvt.Set();
			}
			else if (resultSize == 3)
			{
				me->reqIResult.SetNoCheck((Int32)ReadUInt24(&result[0]));
				me->reqHasResult = true;
				me->cbEvt.Set();
			}
			else if (resultSize == 4)
			{
				me->reqIResult.SetNoCheck(ReadInt32(&result[0]));
				me->reqHasResult = true;
				me->cbEvt.Set();
			}
		}
	}
}

void __stdcall IO::MODBUSDevice::SetResult(AnyType userObj, UInt8 funcCode, UInt16 startAddr, UInt16 cnt)
{
	NN<IO::MODBUSDevice> me = userObj.GetNN<IO::MODBUSDevice>();
	if (funcCode == 5 || funcCode == 6 || funcCode == 15 || funcCode == 16)
	{
		if (me->reqSetStartAddr == startAddr)
		{
			me->reqHasResult = true;
			me->cbEvt.Set();
		}
	}
}

void IO::MODBUSDevice::SetTimeout(Data::Duration timeout)
{
	this->timeout = timeout;
}

Bool IO::MODBUSDevice::ReadInputI16(UInt16 addr, OutParam<Int32> outVal)
{
	Bool succ;
	Sync::MutexUsage mutUsage(this->reqMut);
	this->reqHasResult = false;
	this->reqIResult = outVal;
	this->cbEvt.Clear();
	this->modbus->ReadInputRegisters(this->addr, addr, 1);
	this->cbEvt.Wait(this->timeout);
	this->reqIResult = nullptr;
	succ = this->reqHasResult;
	mutUsage.EndUse();
	return succ;
}

Bool IO::MODBUSDevice::ReadInputFloat(UInt16 addr, OutParam<Double> outVal)
{
	Sync::MutexUsage mutUsage(this->reqMut);
	this->reqHasResult = false;
	this->reqDResult = outVal;
	this->cbEvt.Clear();
	this->modbus->ReadInputRegisters(this->addr, addr, 2);
	this->cbEvt.Wait(this->timeout);
	this->reqDResult = nullptr;
	return this->reqHasResult;
}

Bool IO::MODBUSDevice::ReadInputBuff(UInt16 addr, UInt16 regCnt, UnsafeArray<UInt8> buff)
{
	Sync::MutexUsage mutUsage(this->reqMut);
	this->reqHasResult = false;
	this->reqBResult = buff;
	this->cbEvt.Clear();
	this->modbus->ReadInputRegisters(this->addr, addr, regCnt);
	this->cbEvt.Wait(this->timeout);
	this->reqBResult = nullptr;
	return this->reqHasResult;
}


Bool IO::MODBUSDevice::ReadHoldingI16(UInt16 addr, OutParam<Int32> outVal)
{
	Sync::MutexUsage mutUsage(this->reqMut);
	this->reqHasResult = false;
	this->reqIResult = outVal;
	this->cbEvt.Clear();
	this->modbus->ReadHoldingRegisters(this->addr, addr, 1);
	this->cbEvt.Wait(this->timeout);
	this->reqIResult = nullptr;
	return this->reqHasResult;
}

Bool IO::MODBUSDevice::ReadHoldingI32(UInt16 addr, OutParam<Int32> outVal)
{
	Sync::MutexUsage mutUsage(this->reqMut);
	this->reqHasResult = false;
	this->reqIResult = outVal;
	this->cbEvt.Clear();
	this->modbus->ReadHoldingRegisters(this->addr, addr, 2);
	this->cbEvt.Wait(this->timeout);
	this->reqIResult = nullptr;
	return this->reqHasResult;
}

Bool IO::MODBUSDevice::ReadHoldingFloat(UInt16 addr, OutParam<Double> outVal)
{
	Sync::MutexUsage mutUsage(this->reqMut);
	this->reqHasResult = false;
	this->reqDResult = outVal;
	this->cbEvt.Clear();
	this->modbus->ReadHoldingRegisters(this->addr, addr, 2);
	this->cbEvt.Wait(this->timeout);
	this->reqDResult = nullptr;
	return this->reqHasResult;
}

Bool IO::MODBUSDevice::WriteHoldingU16(UInt16 addr, UInt16 val)
{
	Sync::MutexUsage mutUsage(this->reqMut);
	this->reqHasResult = false;
	this->reqSetStartAddr = addr;
	this->cbEvt.Clear();
	this->modbus->WriteHoldingRegister(this->addr, addr, val);
	this->cbEvt.Wait(this->timeout);
	this->reqSetStartAddr = 0;
	return this->reqHasResult;
}

Bool IO::MODBUSDevice::WriteHoldingsU16(UInt16 addr, UInt16 cnt, UnsafeArray<UInt16> val)
{
	UInt8 buff[256];
	OSInt i = 0;
	while (i < cnt)
	{
		WriteMInt16(&buff[i * 2], val[i]);
	}
	Sync::MutexUsage mutUsage(this->reqMut);
	this->reqHasResult = false;
	this->reqSetStartAddr = addr;
	this->cbEvt.Clear();
	this->modbus->WriteHoldingRegisters(this->addr, addr, cnt, buff);
	this->cbEvt.Wait(this->timeout);
	this->reqSetStartAddr = 0;
	return this->reqHasResult;
}

Bool IO::MODBUSDevice::WriteHoldingI32(UInt16 addr, Int32 val)
{
	UInt8 buff[4];
	WriteMInt32(buff, val);
	Sync::MutexUsage mutUsage(this->reqMut);
	this->reqHasResult = false;
	this->reqSetStartAddr = addr;
	this->cbEvt.Clear();
	this->modbus->WriteHoldingRegisters(this->addr, addr, 2, buff);
	this->cbEvt.Wait(this->timeout);
	this->reqSetStartAddr = 0;
	return this->reqHasResult;
}

Bool IO::MODBUSDevice::WriteHoldingF32(UInt16 addr, Single val)
{
	UInt8 buff[4];
	WriteMFloat(buff, val);
	Sync::MutexUsage mutUsage(this->reqMut);
	this->reqHasResult = false;
	this->reqSetStartAddr = addr;
	this->cbEvt.Clear();
	this->modbus->WriteHoldingRegisters(this->addr, addr, 2, buff);
	this->cbEvt.Wait(this->timeout);
	this->reqSetStartAddr = 0;
	return this->reqHasResult;
}

Bool IO::MODBUSDevice::ReadDInput(UInt16 addr)
{
	Int32 outVal = 0;
	Sync::MutexUsage mutUsage(this->reqMut);
	this->reqHasResult = false;
	this->reqIResult = OptOut<Int32>(outVal);
	this->cbEvt.Clear();
	this->modbus->ReadInputs(this->addr, addr, 1);
	this->cbEvt.Wait(this->timeout);
	this->reqIResult = nullptr;
	if (this->reqHasResult)
	{
		return outVal != 0;
	}
	else
	{
		return false;
	}
}

Bool IO::MODBUSDevice::ReadDInputs(UInt16 addr, UInt16 cnt, OutParam<Int32> outVal)
{
	Sync::MutexUsage mutUsage(this->reqMut);
	this->reqHasResult = false;
	this->reqIResult = outVal;
	this->cbEvt.Clear();
	this->modbus->ReadInputs(this->addr, addr, cnt);
	this->cbEvt.Wait(this->timeout);
	this->reqIResult = nullptr;
	return this->reqHasResult;
}

Bool IO::MODBUSDevice::ReadCoil(UInt16 addr)
{
	Int32 outVal = 0;
	Sync::MutexUsage mutUsage(this->reqMut);
	this->reqHasResult = false;
	this->reqIResult = OptOut<Int32>(outVal);
	this->cbEvt.Clear();
	this->modbus->ReadCoils(this->addr, addr, 1);
	this->cbEvt.Wait(this->timeout);
	this->reqIResult = nullptr;
	if (this->reqHasResult)
	{
		return outVal != 0;
	}
	else
	{
		return false;
	}
}

Bool IO::MODBUSDevice::WriteDOutput(UInt16 addr, Bool isHigh)
{
	Sync::MutexUsage mutUsage(this->reqMut);
	this->reqHasResult = false;
	this->reqSetStartAddr = addr;
	this->cbEvt.Clear();
	this->modbus->WriteCoil(this->addr, addr, isHigh);
	this->cbEvt.Wait(this->timeout);
	this->reqSetStartAddr = 0;
	return this->reqHasResult;
}

IO::MODBUSDevice::MODBUSDevice(NN<IO::MODBUSMaster> modbus, UInt8 addr)
{
	this->modbus = modbus;
	this->addr = addr;
	this->timeout = 200;
	this->reqDResult = nullptr;
	this->reqIResult = nullptr;
	this->reqBResult = nullptr;
	this->reqSetStartAddr = 0;
	this->reqSetCount = 0;
	this->modbus->HandleReadResult(this->addr, ReadResult, SetResult, this);
}

IO::MODBUSDevice::~MODBUSDevice()
{
	this->modbus->HandleReadResult(this->addr, 0, 0, 0);
}
