#ifndef _SM_IO_SILABSERIALPORT
#define _SM_IO_SILABSERIALPORT
#include "Data/ArrayList.h"
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

		virtual void *BeginRead(const Data::ByteArray &buff, Sync::Event *evt);
		virtual UOSInt EndRead(void *reqData, Bool toWait, OutParam<Bool> incomplete);
		virtual void CancelRead(void *reqData);
		virtual void *BeginWrite(Data::ByteArrayR buff, Sync::Event *evt);
		virtual UOSInt EndWrite(void *reqData, Bool toWait);
		virtual void CancelWrite(void *reqData);

		virtual Int32 Flush();
		virtual void Close();
		virtual Bool Recover();
		virtual IO::StreamType GetStreamType() const;
	};
}
#endif
