#ifndef _SM_IO_IFILEBUFFER
#define _SM_IO_IFILEBUFFER

namespace IO
{
	class IFileBuffer
	{
	public:
		virtual ~IFileBuffer(){};
		virtual UInt8 *GetPointer() = 0;
		virtual UInt64 GetLength() = 0;
	};
}
#endif
