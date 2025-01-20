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

		IO::Writer *errWriter;
		
		Sync::Mutex sendMut;
		Sync::Event sendEvt;
		Int32 sendType;
		Bool sendHasResult;
		Int32 sendResult;

		Bool recvRunning;
		Bool recvToStop;

		static UInt32 __stdcall RecvThread(AnyType userObj);
	public:
		CortexControl(UOSInt portNum, IO::Writer *errWriter);
		virtual ~CortexControl();

		Bool IsError();

		virtual void DataParsed(NN<IO::Stream> stm, AnyType stmObj, Int32 cmdType, Int32 seqId, UnsafeArray<const UInt8> cmd, UOSInt cmdSize);
		virtual void DataSkipped(NN<IO::Stream> stm, AnyType stmObj, UnsafeArray<const UInt8> buff, UOSInt buffSize);
		
		Bool GetFWVersion(Int32 *majorVer, Int32 *minorVer);
		Bool ReadDIO(Int32 *dioValues); //bit 0-9 = IN1-10, bit10-11 = OUT1-2
		Bool WriteDIO(Int32 outVal, Int32 outMask); // bit0-1 = OUT1-2
		Bool ReadVin(Int32 *voltage);
		Bool ReadVBatt(Int32 *voltage);
		Bool ReadOdometerCounter(Int32 *odoCount);
		Bool ResetOdometerCounter();
		Bool ReadEnvBrightness(Int32 *brightness); //in LUX
		Bool ReadTemperature(Int32 *temperature); //degree Celsius
		Bool PowerOff();
		Bool HDACodecPower(Bool turnOn);
		Bool SetWatchdogTimeout(UInt8 timeout);
	};
}
#endif
