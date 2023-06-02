#ifndef _SM_IO_STREAMHANDLER
#define _SM_IO_STREAMHANDLER
#include "IO/Stream.h"

namespace IO
{
	class StreamHandler
	{
	public:
		virtual ~StreamHandler() {};

		virtual void *StreamCreated(IO::Stream *stm) = 0;
		virtual void StreamData(IO::Stream *stm, void *stmData, const UInt8 *buff, UOSInt size) = 0;
		virtual void StreamClosed(IO::Stream *stm, void *stmData) = 0;
	};
}
#endif
