#ifndef _SM_IO_PROTOHDLR_PROTOTRAMWAYV1HANDLER
#define _SM_IO_PROTOHDLR_PROTOTRAMWAYV1HANDLER
#include "IO/IProtocolHandler.h"
#include "Sync/Mutex.h"

namespace IO
{
	namespace ProtoHdlr
	{
		class ProtoTramwayV1Handler : public IO::IProtocolHandler
		{
		private:
			typedef struct
			{
				UInt8 *packetBuff;
				UOSInt buffSize;
			} ProtocolStatus;
		private:
			NotNullPtr<IO::IProtocolHandler::DataListener> listener;

		public:
			ProtoTramwayV1Handler(NotNullPtr<IO::IProtocolHandler::DataListener> listener);
			virtual ~ProtoTramwayV1Handler();

			virtual void *CreateStreamData(NotNullPtr<IO::Stream> stm);
			virtual void DeleteStreamData(NotNullPtr<IO::Stream> stm, void *stmData);
			virtual UOSInt ParseProtocol(NotNullPtr<IO::Stream> stm, void *stmObj, void *stmData, const Data::ByteArrayR &buff); // return unprocessed size
			virtual UOSInt BuildPacket(UInt8 *buff, Int32 cmdType, Int32 seqId, const UInt8 *cmd, UOSInt cmdSize, void *stmData);
		};
	}
}
#endif
