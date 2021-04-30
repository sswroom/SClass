#ifndef _SM_IO_MODBUSDEVICE
#define _SM_IO_MODBUSDEVICE
#include "IO/MODBUSController.h"
#include "IO/MODBUSMaster.h"
#include "Math/Unit/UnitBase.h"

namespace IO
{
	class MODBUSDevice
	{
	public:
		typedef void (__stdcall *MODBUSDataEntry)(void *userObj, const UTF8Char *name, UInt8 devAddr, UInt32 regAddr, IO::MODBUSController::DataType dt, Math::Unit::UnitBase::ValueType vt, Int32 unit, Int32 denorm);
	private:
		IO::MODBUSMaster *modbus;
		UOSInt timeout;
		UInt8 addr;
		Sync::Event *cbEvt;
		Sync::Mutex *reqMut;
		Double *reqDResult;
		Int32 *reqIResult;
		UInt8 *reqBResult;
		Bool reqHasResult;
		UInt16 reqSetStartAddr;
		UInt16 reqSetCount;
		static void __stdcall ReadResult(void *userObj, UInt8 funcCode, const UInt8 *result, UOSInt resultSize);
		static void __stdcall SetResult(void *userObj, UInt8 funcCode, UInt16 startAddr, UInt16 cnt);

	protected:
		void SetTimeout(UOSInt timeout);

		Bool ReadInputI16(UInt16 addr, Int32 *outVal);
		Bool ReadInputFloat(UInt16 addr, Double *outVal);
		Bool ReadInputBuff(UInt16 addr, UInt16 regCnt, UInt8 *buff);
		Bool ReadHoldingI16(UInt16 addr, Int32 *outVal);
		Bool ReadHoldingI32(UInt16 addr, Int32 *outVal);
		Bool ReadHoldingFloat(UInt16 addr, Double *outVal);
		Bool WriteHoldingU16(UInt16 addr, UInt16 val);
		Bool WriteHoldingsU16(UInt16 addr, UInt16 cnt, UInt16 *vals);
		Bool WriteHoldingI32(UInt16 addr, Int32 val);
		Bool WriteHoldingF32(UInt16 addr, Single val);
		Bool ReadDInputs(UInt16 addr, UInt16 cnt, Int32 *outVal);
		Bool WriteDOutput(UInt16 addr, Bool isHigh);
	public:
		MODBUSDevice(IO::MODBUSMaster *modbus, UInt8 addr);
		virtual ~MODBUSDevice();
	};
}
#endif
