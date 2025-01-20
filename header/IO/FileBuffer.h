#ifndef _SM_IO_FILEBUFFER
#define _SM_IO_FILEBUFFER

namespace IO
{
	class FileBuffer
	{
	public:
		virtual ~FileBuffer(){};
		virtual UnsafeArrayOpt<UInt8> GetPointer() = 0;
		virtual UInt64 GetLength() = 0;
	};
}
#endif
