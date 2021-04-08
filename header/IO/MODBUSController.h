#ifndef _SM_IO_MODBUSCONTROLLER
#define _SM_IO_MODBUSCONTROLLER
#include "Data/Int32Map.h"
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
			DT_IU32,
			DT_IU16,
			DT_OS16 //One's complement
		} DataType;
	private:
		IO::MODBUSMaster *modbus;
		UOSInt timeout;
		Sync::Event *cbEvt;
		Sync::Mutex *reqMut;
		UInt8 *reqResult;
		UInt8 reqFuncCode;
		OSInt reqResultSize;
		Bool reqHasResult;
		Data::Int32Map<UInt8> *devMap;

		UInt16 reqSetStartAddr;
		UInt16 reqSetCount;

		static void __stdcall ReadResult(void *userObj, UInt8 funcCode, const UInt8 *result, OSInt resultSize);
		static void __stdcall SetResult(void *userObj, UInt8 funcCode, UInt16 startAddr, UInt16 cnt);

		Bool ReadRegister(UInt8 devAddr, UInt32 regAddr, UInt8 *resBuff, UInt16 resSize);
	public:
		MODBUSController(IO::MODBUSMaster *modbus);
		~MODBUSController();

		void SetTimeout(UOSInt timeout);
		Bool ReadRegisterI32(UInt8 devAddr, UInt32 regAddr, Int32 *outVal);
		Bool ReadRegisterII32(UInt8 devAddr, UInt32 regAddr, Int32 *outVal); //Intel byte order
		Bool ReadRegisterF32(UInt8 devAddr, UInt32 regAddr, Single *outVal);
		Bool ReadRegisterU16(UInt8 devAddr, UInt32 regAddr, UInt16 *outVal);
		Bool ReadRegisterIU16(UInt8 devAddr, UInt32 regAddr, UInt16 *outVal); //Intel byte order
	};
}
#endif
