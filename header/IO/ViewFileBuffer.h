#ifndef _SM_IO_VIEWFILEBUFFER
#define _SM_IO_VIEWFILEBUFFER
#include "IO/FileBuffer.h"

namespace IO
{
	class ViewFileBuffer : public IO::FileBuffer
	{
	private:
		UnsafeArrayOpt<UInt8> filePtr;
		void *fileHandle;
		void *mapHandle;

	public:
		ViewFileBuffer(const UTF8Char *fileName);
		virtual ~ViewFileBuffer();

		virtual UnsafeArrayOpt<UInt8> GetPointer();
		virtual UInt64 GetLength();

	private:
		void FreePointer();
	};
}
#endif
