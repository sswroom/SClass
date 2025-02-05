#ifndef _SM_IO_MTSTREAM
#define _SM_IO_MTSTREAM
#include "IO/Stream.h"
#include "Sync/Mutex.h"
#include "Sync/Thread.h"

namespace IO
{
	class MTStream : public IO::Stream
	{
	private:
		NN<IO::Stream> outStm;
		Sync::Mutex cacheMut;
		UnsafeArray<UInt8> cacheBuff1;
		UnsafeArray<UInt8> cacheBuff2;
		UOSInt cacheBuffSize;
		UOSInt cacheSize;
		Sync::Thread thread;

		static void __stdcall OutputThread(NN<Sync::Thread> thread);
	public:
		MTStream(NN<IO::Stream> outStm, UOSInt buffSize);
		virtual ~MTStream();

		virtual Bool IsDown() const;
		virtual UOSInt Read(const Data::ByteArray &buff);
		virtual UOSInt Write(Data::ByteArrayR size);

		virtual Int32 Flush();
		virtual void Close();
		virtual Bool Recover();
		virtual StreamType GetStreamType() const;
	};
}
#endif
