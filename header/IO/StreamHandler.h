#ifndef _SM_IO_STREAMHANDLER
#define _SM_IO_STREAMHANDLER
#include "AnyType.h"
#include "IO/Stream.h"

namespace IO
{
	class StreamHandler
	{
	public:
		virtual ~StreamHandler() {};

		virtual AnyType StreamCreated(NN<IO::Stream> stm) = 0;
		virtual void StreamData(NN<IO::Stream> stm, AnyType stmData, const Data::ByteArrayR &buff) = 0;
		virtual void StreamClosed(NN<IO::Stream> stm, AnyType stmData) = 0;
	};
}
#endif
