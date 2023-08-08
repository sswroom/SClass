#ifndef _SM_IO_MEMFILEBUFFER
#define _SM_IO_MEMFILEBUFFER
#include "IO/IFileBuffer.h"
#include "Text/CString.h"

namespace IO
{
	class MemFileBuffer : public IO::IFileBuffer
	{
	private:
		UInt8 *filePtr;
		UInt64 fileSize;

	public:
		MemFileBuffer(Text::CStringNN fileName);
		virtual ~MemFileBuffer();

		virtual UInt8 *GetPointer();
		virtual UInt64 GetLength();
	};
}
#endif
