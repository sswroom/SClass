#ifndef _SM_IO_IPROTOCOLPARSER
#define _SM_IO_IPROTOCOLPARSER
#include "AnyType.h"
#include "IO/Stream.h"

namespace IO
{
	class IProtocolParser
	{
	public:
		virtual AnyType CreateStreamData(NotNullPtr<IO::Stream> stm) = 0;
		virtual void DeleteStreamData(NotNullPtr<IO::Stream> stm, AnyType stmData) = 0;
		virtual UOSInt ParseProtocol(NotNullPtr<IO::Stream> stm, AnyType stmObj, AnyType stmData, const Data::ByteArrayR &buff) = 0; // return unprocessed size
	};
}
#endif
