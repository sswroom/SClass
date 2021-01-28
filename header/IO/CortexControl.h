#ifndef _SM_IO_CORTEXCONTROL
#define _SM_IO_CORTEXCONTROL
#include "IO/IProtocolHandler.h"
#include "IO/IWriter.h"
#include "IO/ProtoCortexHandler.h"
#include "IO/Stream.h"
#include "Sync/Mutex.h"

namespace IO
{
	class CortexControl : public IO::IProtocolHandler::DataListener
	{
	private:
		IO::Stream *stm;
		IO::ProtoCortexHandler *protoHdlr;

		IO::IWriter *errWriter;
		
		Sync::Mutex *sendMut;
		Sync::Event *sendEvt;
		Int32 sendType;
		Bool sendHasResult;
		Int32 sendResult;

		Bool recvRunning;
		Bool recvToStop;

		static UInt32 __stdcall RecvThread(void *userObj);
	public:
		CortexControl(Int32 portNum, IO::IWriter *errWriter);
		virtual ~CortexControl();

		Bool IsError();

		virtual void DataParsed(IO::Stream *stm, void *stmObj, Int32 cmdType, Int32 seqId, UInt8 *cmd, OSInt cmdSize);
		virtual void DataSkipped(IO::Stream *stm, void *stmObj, UInt8 *buff, OSInt buffSize);
		
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
