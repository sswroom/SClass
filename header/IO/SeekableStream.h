#ifndef _SM_IO_SEEKABLESTREAM
#define _SM_IO_SEEKABLESTREAM
#include "IO/Stream.h"

namespace IO
{
	class SeekableStream : public Stream
	{
	public:
		SeekableStream(Text::String *sourceName) : IO::Stream(sourceName)
		{};

		SeekableStream(const UTF8Char *sourceName) : IO::Stream(sourceName)
		{};

		virtual ~SeekableStream(){};

		virtual Bool CanSeek() {return true;}
		virtual UInt64 SeekFromBeginning(UInt64 position) = 0;
		virtual UInt64 SeekFromCurrent(Int64 position) = 0;
		virtual UInt64 SeekFromEnd(Int64 position) = 0;
		virtual UInt64 GetPosition() = 0;
		virtual UInt64 GetLength() = 0;
	};
}
#endif
