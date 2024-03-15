#ifndef _SM_IO_MODBUSCONTROLLER
#define _SM_IO_MODBUSCONTROLLER
#include "Data/FastMap.h"
#include "IO/MODBUSMaster.h"
#include "Sync/Event.h"
#include "Sync/Mutex.h"

namespace IO
{
	class MODBUSController
	{
	public:
		typedef enum
		{
			DT_I32,
			DT_F32,
			DT_U16,
			DT_IU32, //UInt32 in Intel byte order
			DT_IU16, //UInt16 in Intel byte order
			DT_OS16, //One's complement
			DT_U8
		} DataType;
	private:
		IO::MODBUSMaster *modbus;
		Data::Duration timeout;
		Sync::Event cbEvt;
		Sync::Mutex reqMut;
		UInt8 *reqResult;
		UInt8 reqFuncCode;
		UOSInt reqResultSize;
		Bool reqHasResult;
		Data::FastMap<UInt8, UInt8> devMap;

		UInt16 reqSetStartAddr;
		UInt16 reqSetCount;

		static void __stdcall ReadResult(AnyType userObj, UInt8 funcCode, const UInt8 *result, UOSInt resultSize);
		static void __stdcall SetResult(AnyType userObj, UInt8 funcCode, UInt16 startAddr, UInt16 cnt);

		Bool ReadRegister(UInt8 devAddr, UInt32 regAddr, UInt8 *resBuff, UInt16 resSize);
		Bool WriteRegister(UInt8 devAddr, UInt32 regAddr, UInt8 *regBuff, UInt16 regSize);
	public:
		MODBUSController(IO::MODBUSMaster *modbus);
		~MODBUSController();

		void SetTimeout(Data::Duration timeout);
		Bool ReadRegisterI32(UInt8 devAddr, UInt32 regAddr, Int32 *outVal);
		Bool ReadRegisterII32(UInt8 devAddr, UInt32 regAddr, Int32 *outVal); //Intel byte order
		Bool ReadRegisterF32(UInt8 devAddr, UInt32 regAddr, Single *outVal);
		Bool ReadRegisterU16(UInt8 devAddr, UInt32 regAddr, UInt16 *outVal);
		Bool ReadRegisterIU16(UInt8 devAddr, UInt32 regAddr, UInt16 *outVal); //Intel byte order
		Bool ReadRegisterU8(UInt8 devAddr, UInt32 regAddr, UInt8 *outVal);
		Bool ReadRegisterU8Arr(UInt8 devAddr, UInt32 regAddr, UInt8 *outVal, UInt16 valCnt);
		Bool WriteRegisterBool(UInt8 devAddr, UInt32 regAddr, Bool val);
	};
}
#endif
