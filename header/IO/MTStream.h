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
		UIntOS cacheBuffSize;
		UIntOS cacheSize;
		Sync::Thread thread;

		static void __stdcall OutputThread(NN<Sync::Thread> thread);
	public:
		MTStream(NN<IO::Stream> outStm, UIntOS buffSize);
		virtual ~MTStream();

		virtual Bool IsDown() const;
		virtual UIntOS Read(const Data::ByteArray &buff);
		virtual UIntOS Write(Data::ByteArrayR size);

		virtual Int32 Flush();
		virtual void Close();
		virtual Bool Recover();
		virtual StreamType GetStreamType() const;
	};
}
#endif
