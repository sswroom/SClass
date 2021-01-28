#ifndef _SM_IO_MEMFILEBUFFER
#define _SM_IO_MEMFILEBUFFER
#include "IO/IFileBuffer.h"

namespace IO
{
	class MemFileBuffer : public IO::IFileBuffer
	{
	private:
		UInt8 *filePtr;
		Int64 fileSize;

	public:
		MemFileBuffer(const UTF8Char *fileName);
		virtual ~MemFileBuffer();

		virtual UInt8 *GetPointer();
		virtual Int64 GetLength();
	};
};
#endif
