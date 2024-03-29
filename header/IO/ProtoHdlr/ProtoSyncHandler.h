#ifndef _SM_IO_PROTOHDLR_PROTOSYNCHANDLER
#define _SM_IO_PROTOHDLR_PROTOSYNCHANDLER
#include "Crypto/Hash/CRC32RIEEE.h"
#include "IO/IProtocolHandler.h"
#include "Sync/Mutex.h"

namespace IO
{
	namespace ProtoHdlr
	{
		class ProtoSyncHandler : public IO::IProtocolHandler
		{
		private:
			NotNullPtr<IO::IProtocolHandler::DataListener> listener;
			Crypto::Hash::CRC32RIEEE crc;

		public:
			ProtoSyncHandler(NotNullPtr<IO::IProtocolHandler::DataListener> listener);
			virtual ~ProtoSyncHandler();

			virtual AnyType CreateStreamData(NotNullPtr<IO::Stream> stm);
			virtual void DeleteStreamData(NotNullPtr<IO::Stream> stm, AnyType stmData);
			virtual UOSInt ParseProtocol(NotNullPtr<IO::Stream> stm, AnyType stmObj, AnyType stmData, const Data::ByteArrayR &buff); // return unprocessed size
			virtual UOSInt BuildPacket(UInt8 *buff, Int32 cmdType, Int32 seqId, const UInt8 *cmd, UOSInt cmdSize, AnyType stmData);
		};
	}
}
#endif
