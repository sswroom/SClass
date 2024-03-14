#ifndef _SM_IO_PROTOHDLR_PROTOJTT808HANDLER
#define _SM_IO_PROTOHDLR_PROTOJTT808HANDLER
#include "IO/IProtocolHandler.h"
#include "Sync/Mutex.h"

namespace IO
{
	namespace ProtoHdlr
	{
		class ProtoJTT808Handler : public IO::IProtocolHandler
		{
		private:
			NotNullPtr<IO::IProtocolHandler::DataListener> listener;
			UInt64 devId;

		public:
			ProtoJTT808Handler(NotNullPtr<IO::IProtocolHandler::DataListener> listener, UInt64 devId);
			virtual ~ProtoJTT808Handler();

			virtual AnyType CreateStreamData(NotNullPtr<IO::Stream> stm);
			virtual void DeleteStreamData(NotNullPtr<IO::Stream> stm, AnyType stmData);
			virtual UOSInt ParseProtocol(NotNullPtr<IO::Stream> stm, AnyType stmObj, AnyType stmData, const Data::ByteArrayR &buff); // return unprocessed size
			virtual UOSInt BuildPacket(UInt8 *buff, Int32 cmdType, Int32 seqId, const UInt8 *cmd, UOSInt cmdSize, AnyType stmData);

			const UInt8 *GetPacketContent(const UInt8 *packet, UOSInt *contSize);
		};
	}
}
#endif
