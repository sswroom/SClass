#ifndef _SM_IO_STREAMHANDLER
#define _SM_IO_STREAMHANDLER
#include "IO/Stream.h"

namespace IO
{
	class StreamHandler
	{
	public:
		virtual ~StreamHandler() {};

		virtual void *StreamCreated(NotNullPtr<IO::Stream> stm) = 0;
		virtual void StreamData(NotNullPtr<IO::Stream> stm, void *stmData, const UInt8 *buff, UOSInt size) = 0;
		virtual void StreamClosed(NotNullPtr<IO::Stream> stm, void *stmData) = 0;
	};
}
#endif
