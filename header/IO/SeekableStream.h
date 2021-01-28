#ifndef _SM_IO_SEEKABLESTREAM
#define _SM_IO_SEEKABLESTREAM
#include "IO/Stream.h"

namespace IO
{
	class SeekableStream : public Stream
	{
	public:
		typedef enum
		{
			ST_BEGIN,
			ST_CURRENT,
			ST_END
		} SeekType;

		SeekableStream(const UTF8Char *sourceName) : IO::Stream(sourceName)
		{};

		virtual ~SeekableStream(){};

		virtual Bool CanSeek() {return true;}
		virtual UInt64 Seek(SeekType origin, Int64 position) = 0;
		virtual UInt64 GetPosition() = 0;
		virtual UInt64 GetLength() = 0;
	};
}
#endif
