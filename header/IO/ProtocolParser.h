#ifndef _SM_IO_PROTOCOLPARSER
#define _SM_IO_PROTOCOLPARSER
#include "AnyType.h"
#include "IO/Stream.h"

namespace IO
{
	class ProtocolParser
	{
	public:
		virtual AnyType CreateStreamData(NN<IO::Stream> stm) = 0;
		virtual void DeleteStreamData(NN<IO::Stream> stm, AnyType stmData) = 0;
		virtual UIntOS ParseProtocol(NN<IO::Stream> stm, AnyType stmObj, AnyType stmData, const Data::ByteArrayR &buff) = 0; // return unprocessed size
	};
}
#endif
