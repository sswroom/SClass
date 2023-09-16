#ifndef _SM_IO_PROTOHDLR_PROTOGPSDEVINFOHANDLER
#define _SM_IO_PROTOHDLR_PROTOGPSDEVINFOHANDLER
#include "Crypto/Hash/CRC32R.h"
#include "IO/IProtocolHandler.h"
#include "Sync/Mutex.h"

namespace IO
{
	namespace ProtoHdlr
	{
		class ProtoGPSDevInfoHandler : public IO::IProtocolHandler
		{
		private:
			NotNullPtr<IO::IProtocolHandler::DataListener> listener;
			Sync::Mutex crcMut;
			Crypto::Hash::CRC32R crc;

		public:
			ProtoGPSDevInfoHandler(NotNullPtr<IO::IProtocolHandler::DataListener> listener);
			virtual ~ProtoGPSDevInfoHandler();

			virtual void *CreateStreamData(NotNullPtr<IO::Stream> stm);
			virtual void DeleteStreamData(NotNullPtr<IO::Stream> stm, void *stmData);
			virtual UOSInt ParseProtocol(NotNullPtr<IO::Stream> stm, void *stmObj, void *stmData, const Data::ByteArrayR &buff); // return unprocessed size
			virtual UOSInt BuildPacket(UInt8 *buff, Int32 cmdType, Int32 seqId, const UInt8 *cmd, UOSInt cmdSize, void *stmData);
		};
	}
}
#endif
