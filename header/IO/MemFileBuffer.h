#ifndef _SM_IO_MEMFILEBUFFER
#define _SM_IO_MEMFILEBUFFER
#include "IO/FileBuffer.h"
#include "Text/CString.h"

namespace IO
{
	class MemFileBuffer : public IO::FileBuffer
	{
	private:
		UnsafeArrayOpt<UInt8> filePtr;
		UInt64 fileSize;

	public:
		MemFileBuffer(Text::CStringNN fileName);
		virtual ~MemFileBuffer();

		virtual UnsafeArrayOpt<UInt8> GetPointer();
		virtual UInt64 GetLength();
	};
}
#endif
