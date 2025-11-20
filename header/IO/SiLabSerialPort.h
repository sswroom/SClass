#ifndef _SM_IO_SILABSERIALPORT
#define _SM_IO_SILABSERIALPORT
#include "Data/ArrayList.hpp"
#include "Sync/Event.h"
#include "Sync/Mutex.h"
#include "IO/Stream.h"

namespace IO
{
	class SiLabDriver;
	class SiLabSerialPort : public IO::Stream
	{
	private:
		SiLabDriver *driver;
		void *handle;
		Sync::Event rdEvt;
		Sync::Mutex rdMut;
		Bool reading;

	public:
		SiLabSerialPort(void *handle, IO::SiLabDriver *driver);
		virtual ~SiLabSerialPort();

		virtual Bool IsDown() const;
		virtual UOSInt Read(const Data::ByteArray &buff);
		virtual UOSInt Write(Data::ByteArrayR buff);

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
