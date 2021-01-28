#ifndef _SM_IO_IPROTOCOLPARSER
#define _SM_IO_IPROTOCOLPARSER
#include "IO/Stream.h"

namespace IO
{
	class IProtocolParser
	{
	public:
		virtual void *CreateStreamData(IO::Stream *stm) = 0;
		virtual void DeleteStreamData(IO::Stream *stm, void *stmData) = 0;
		virtual UOSInt ParseProtocol(IO::Stream *stm, void *stmObj, void *stmData, const UInt8 *buff, UOSInt buffSize) = 0; // return unprocessed size
	};
}
#endif
