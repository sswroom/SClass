#ifndef _SM_IO_MEMORYSTREAM
#define _SM_IO_MEMORYSTREAM
#include "Data/ByteBuffer.h"
#include "IO/SeekableStream.h"

namespace IO
{
	class MemoryStream : public SeekableStream
	{
	private:
		Data::ByteBuffer buff;
		UOSInt currPtr;
		UOSInt currSize;
	public:
		MemoryStream();
		MemoryStream(UOSInt capacity);
		virtual ~MemoryStream();

		UnsafeArray<UInt8> GetBuff();
		UnsafeArray<UInt8> GetBuff(OutParam<UOSInt> buffSize);
		Data::ByteArray GetArray() const;

		virtual Bool IsDown() const;
		virtual UOSInt Read(const Data::ByteArray &buff);
		virtual UOSInt Write(Data::ByteArrayR buff);

		virtual Int32 Flush();
		virtual void Close();
		virtual Bool Recover();
		virtual StreamType GetStreamType() const;
		virtual UInt64 SeekFromBeginning(UInt64 position);
		virtual UInt64 SeekFromCurrent(Int64 position);
		virtual UInt64 SeekFromEnd(Int64 position);
		virtual UInt64 GetPosition();
		virtual UInt64 GetLength();

		void Clear();
	};
}
#endif
