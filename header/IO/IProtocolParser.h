#ifndef _SM_IO_IPROTOCOLPARSER
#define _SM_IO_IPROTOCOLPARSER
#include "IO/Stream.h"

namespace IO
{
	class IProtocolParser
	{
	public:
		virtual void *CreateStreamData(NotNullPtr<IO::Stream> stm) = 0;
		virtual void DeleteStreamData(NotNullPtr<IO::Stream> stm, void *stmData) = 0;
		virtual UOSInt ParseProtocol(NotNullPtr<IO::Stream> stm, void *stmObj, void *stmData, const Data::ByteArrayR &buff) = 0; // return unprocessed size
	};
}
#endif
