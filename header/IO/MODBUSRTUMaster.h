#ifndef _SM_IO_MODBUSRTUMASTER
#define _SM_IO_MODBUSRTUMASTER
#include "AnyType.h"
#include "Crypto/Hash/CRC16R.h"
#include "Data/FastMapNN.h"
#include "IO/MODBUSMaster.h"
#include "IO/Stream.h"
#include "Manage/HiResClock.h"
#include "Sync/Mutex.h"

namespace IO
{
	class MODBUSRTUMaster : public IO::MODBUSMaster
	{
	public:
		typedef struct
		{
			ReadResultFunc readFunc;
			SetResultFunc setFunc;
			AnyType userObj;
		} AddrResultCb;
		
	private:
		NN<IO::Stream> stm;
		Bool threadRunning;
		Bool threadToStop;
		Manage::HiResClock clk;
		Sync::Mutex stmMut;
		Crypto::Hash::CRC16R crc;
		Sync::Mutex crcMut;
		Data::FastMapNN<Int32, AddrResultCb> cbMap;

		static UInt32 __stdcall ThreadProc(AnyType userObj);
		void CalcCRC(UnsafeArray<UInt8> rtu, UOSInt rtuSize); // size include CRC
		Bool IsCRCValid(UnsafeArray<UInt8> rtu, UOSInt rtuSize);
	public:
		MODBUSRTUMaster(NN<IO::Stream> stm);
		virtual ~MODBUSRTUMaster();

		virtual Bool ReadCoils(UInt8 devAddr, UInt16 coilAddr, UInt16 coilCnt); //Output
		virtual Bool ReadInputs(UInt8 devAddr, UInt16 inputAddr, UInt16 inputCnt);
		virtual Bool ReadHoldingRegisters(UInt8 devAddr, UInt16 regAddr, UInt16 regCnt); //Output
		virtual Bool ReadInputRegisters(UInt8 devAddr, UInt16 regAddr, UInt16 regCnt);
		virtual Bool WriteCoil(UInt8 devAddr, UInt16 coilAddr, Bool isHigh);
		virtual Bool WriteHoldingRegister(UInt8 devAddr, UInt16 regAddr, UInt16 val);
		virtual Bool WriteHoldingRegisters(UInt8 devAddr, UInt16 regAddr, UInt16 cnt, UnsafeArray<UInt8> val);

		virtual void HandleReadResult(UInt8 addr, ReadResultFunc readFunc, SetResultFunc setFunc, AnyType userObj);
	};
}
#endif
