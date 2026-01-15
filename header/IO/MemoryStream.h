#ifndef _SM_IO_MEMORYSTREAM
#define _SM_IO_MEMORYSTREAM
#include "Data/ByteBuffer.h"
#include "IO/SeekableStream.h"
#include "Text/CString.h"

namespace IO
{
	class MemoryStream : public SeekableStream
	{
	private:
		Data::ByteBuffer buff;
		UIntOS currPtr;
		UIntOS currSize;
	public:
		MemoryStream();
		MemoryStream(UIntOS capacity);
		virtual ~MemoryStream();

		UnsafeArray<UInt8> GetBuff();
		UnsafeArray<UInt8> GetBuff(OutParam<UIntOS> buffSize);
		Data::ByteArray GetArray() const;
		Text::CStringNN ToCString();

		virtual Bool IsDown() const;
		virtual UIntOS Read(const Data::ByteArray &buff);
		virtual UIntOS Write(Data::ByteArrayR buff);

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
