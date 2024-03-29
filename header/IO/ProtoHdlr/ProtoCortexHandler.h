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
			NotNullPtr<IO::IProtocolHandler::DataListener> listener;

		public:
			ProtoCortexHandler(NotNullPtr<IO::IProtocolHandler::DataListener> listener);
			virtual ~ProtoCortexHandler();

			virtual AnyType CreateStreamData(NotNullPtr<IO::Stream> stm);
			virtual void DeleteStreamData(NotNullPtr<IO::Stream> stm, AnyType stmData);
			virtual UOSInt ParseProtocol(NotNullPtr<IO::Stream> stm, AnyType stmObj, AnyType stmData, const Data::ByteArrayR &buff); // return unprocessed size
			virtual UOSInt BuildPacket(UInt8 *buff, Int32 cmdType, Int32 seqId, const UInt8 *cmd, UOSInt cmdSize, AnyType stmData);
		};
	}
}
#endif
