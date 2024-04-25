#ifndef _SM_IO_PROTOHDLR_PROTOCORTEXHANDLER
#define _SM_IO_PROTOHDLR_PROTOCORTEXHANDLER
#include "IO/IProtocolHandler.h"

namespace IO
{
	namespace ProtoHdlr
	{
		class ProtoCortexHandler : public IO::IProtocolHandler
		{
		private:
			NN<IO::IProtocolHandler::DataListener> listener;

		public:
			ProtoCortexHandler(NN<IO::IProtocolHandler::DataListener> listener);
			virtual ~ProtoCortexHandler();

			virtual AnyType CreateStreamData(NN<IO::Stream> stm);
			virtual void DeleteStreamData(NN<IO::Stream> stm, AnyType stmData);
			virtual UOSInt ParseProtocol(NN<IO::Stream> stm, AnyType stmObj, AnyType stmData, const Data::ByteArrayR &buff); // return unprocessed size
			virtual UOSInt BuildPacket(UInt8 *buff, Int32 cmdType, Int32 seqId, const UInt8 *cmd, UOSInt cmdSize, AnyType stmData);
		};
	}
}
#endif
