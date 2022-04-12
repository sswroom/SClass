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
			IO::IProtocolHandler::DataListener *listener;
			Crypto::Hash::CRC32RIEEE crc;

		public:
			ProtoSyncHandler(IO::IProtocolHandler::DataListener *listener);
			virtual ~ProtoSyncHandler();

			virtual void *CreateStreamData(IO::Stream *stm);
			virtual void DeleteStreamData(IO::Stream *stm, void *stmData);
			virtual UOSInt ParseProtocol(IO::Stream *stm, void *stmObj, void *stmData, const UInt8 *buff, UOSInt buffSize); // return unprocessed size
			virtual UOSInt BuildPacket(UInt8 *buff, Int32 cmdType, Int32 seqId, const UInt8 *cmd, UOSInt cmdSize, void *stmData);
		};
	}
}
#endif
