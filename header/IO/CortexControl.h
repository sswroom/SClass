#ifndef _SM_IO_CORTEXCONTROL
#define _SM_IO_CORTEXCONTROL
#include "AnyType.h"
#include "IO/ProtocolHandler.h"
#include "IO/SerialPort.h"
#include "IO/Writer.h"
#include "IO/ProtoHdlr/ProtoCortexHandler.h"
#include "Sync/Mutex.h"

namespace IO
{
	class CortexControl : public IO::ProtocolHandler::DataListener
	{
	private:
		NN<IO::SerialPort> stm;
		IO::ProtoHdlr::ProtoCortexHandler protoHdlr;

		Optional<IO::Writer> errWriter;
		
		Sync::Mutex sendMut;
		Sync::Event sendEvt;
		Int32 sendType;
		Bool sendHasResult;
		Int32 sendResult;

		Bool recvRunning;
		Bool recvToStop;

		static UInt32 __stdcall RecvThread(AnyType userObj);
	public:
		CortexControl(UIntOS portNum, Optional<IO::Writer> errWriter);
		virtual ~CortexControl();

		Bool IsError();

		virtual void DataParsed(NN<IO::Stream> stm, AnyType stmObj, Int32 cmdType, Int32 seqId, UnsafeArray<const UInt8> cmd, UIntOS cmdSize);
		virtual void DataSkipped(NN<IO::Stream> stm, AnyType stmObj, UnsafeArray<const UInt8> buff, UIntOS buffSize);
		
		Bool GetFWVersion(OutParam<Int32> majorVer, OutParam<Int32> minorVer);
		Bool ReadDIO(OutParam<Int32> dioValues); //bit 0-9 = IN1-10, bit10-11 = OUT1-2
		Bool WriteDIO(Int32 outVal, Int32 outMask); // bit0-1 = OUT1-2
		Bool ReadVin(OutParam<Int32> voltage);
		Bool ReadVBatt(OutParam<Int32> voltage);
		Bool ReadOdometerCounter(OutParam<Int32> odoCount);
		Bool ResetOdometerCounter();
		Bool ReadEnvBrightness(OutParam<Int32> brightness); //in LUX
		Bool ReadTemperature(OutParam<Int32> temperature); //degree Celsius
		Bool PowerOff();
		Bool HDACodecPower(Bool turnOn);
		Bool SetWatchdogTimeout(UInt8 timeout);
	};
}
#endif
