#ifndef _SM_IO_MODBUSMASTER
#define _SM_IO_MODBUSMASTER

namespace IO
{
	class MODBUSMaster
	{
	public:
		typedef void (__stdcall *ReadResultFunc)(void *userObj, UInt8 funcCode, const UInt8 *result, OSInt resultSize);
		typedef void (__stdcall *SetResultFunc)(void *userObj, UInt8 funcCode, UInt16 startAddr, UInt16 count);

	public:
		virtual ~MODBUSMaster() {};

		virtual Bool ReadCoils(UInt8 devAddr, UInt16 coilAddr, UInt16 coilCnt) = 0; //Output
		virtual Bool ReadInputs(UInt8 devAddr, UInt16 inputAddr, UInt16 inputCnt) = 0;
		virtual Bool ReadHoldingRegisters(UInt8 devAddr, UInt16 regAddr, UInt16 regCnt) = 0; //Output
		virtual Bool ReadInputRegisters(UInt8 devAddr, UInt16 regAddr, UInt16 regCnt) = 0;
		virtual Bool WriteCoil(UInt8 devAddr, UInt16 coilAddr, Bool isHigh) = 0;
		virtual Bool WriteHoldingRegister(UInt8 devAddr, UInt16 regAddr, UInt16 val) = 0;
		virtual Bool WriteHoldingRegisters(UInt8 devAddr, UInt16 regAddr, UInt16 cnt, UInt8 *val) = 0;

		virtual void HandleReadResult(UInt8 addr, ReadResultFunc readFunc, SetResultFunc setFunc, void *userObj) = 0;
	};
}
#endif
