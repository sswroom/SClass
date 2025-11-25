#ifndef _SM_IO_RS232GPIO
#define _SM_IO_RS232GPIO
#include "AnyType.h"
#include "Data/ArrayList.hpp"
#include "IO/GPIOControl.h"
#include "IO/Stream.h"
#include "Sync/Event.h"
#include "Sync/Mutex.h"

#if defined(CPU_AVR)
#define RS232GPIO_BUFFSIZE 64
#else
#define RS232GPIO_BUFFSIZE 1024
#endif
namespace IO
{
	class RS232GPIO : public IO::Stream
	{
	private:
		NN<IO::GPIOControl> gpio;
		UOSInt rxdPin;
		UOSInt txdPin;
		UInt8 readBuff[RS232GPIO_BUFFSIZE];
		UOSInt readBuffStart;
		UOSInt readBuffEnd;
		UInt32 baudRate;
		Int64 readStartTime;
		UOSInt readBit;
		UInt8 readVal;

		Bool reading;
		Bool running;
		Bool toStop;

		static UInt32 __stdcall ReadThread(AnyType userObj);
		static void __stdcall IntHdlr(AnyType userObj);
	public:
		RS232GPIO(NN<IO::GPIOControl> gpio, UOSInt rxdPin, UOSInt txdPin, UInt32 baudRate);
		virtual ~RS232GPIO();

		virtual Bool IsDown() const;
		virtual UOSInt Read(const Data::ByteArray &buff);
		virtual UOSInt Write(Data::ByteArrayR buff);
		Bool HasData();

		virtual Optional<StreamReadReq> BeginRead(const Data::ByteArray &buff, NN<Sync::Event> evt);
		virtual UOSInt EndRead(NN<StreamReadReq> reqData, Bool toWait, OutParam<Bool> incomplete);
		virtual void CancelRead(NN<StreamReadReq> reqData);
		virtual Optional<StreamWriteReq> BeginWrite(Data::ByteArrayR buff, NN<Sync::Event> evt);
		virtual UOSInt EndWrite(NN<StreamWriteReq> reqData, Bool toWait);
		virtual void CancelWrite(NN<StreamWriteReq> reqData);

		virtual Int32 Flush();
		virtual void Close();
		virtual Bool Recover();
		virtual IO::StreamType GetStreamType() const;
	};
}
#endif
