#ifndef _SM_IO_PROTOHDLR_PROTOCORTEXHANDLER
#define _SM_IO_PROTOHDLR_PROTOCORTEXHANDLER
#include "IO/ProtocolHandler.h"

namespace IO
{
	namespace ProtoHdlr
	{
		class ProtoCortexHandler : public IO::ProtocolHandler
		{
		private:
			NN<IO::ProtocolHandler::DataListener> listener;

		public:
			ProtoCortexHandler(NN<IO::ProtocolHandler::DataListener> listener);
			virtual ~ProtoCortexHandler();

			virtual AnyType CreateStreamData(NN<IO::Stream> stm);
			virtual void DeleteStreamData(NN<IO::Stream> stm, AnyType stmData);
			virtual UOSInt ParseProtocol(NN<IO::Stream> stm, AnyType stmObj, AnyType stmData, const Data::ByteArrayR &buff); // return unprocessed size
			virtual UOSInt BuildPacket(UnsafeArray<UInt8> buff, Int32 cmdType, Int32 seqId, UnsafeArray<const UInt8> cmd, UOSInt cmdSize, AnyType stmData);
		};
	}
}
#endif
