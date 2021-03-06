#ifndef _SM_IO_PROTOHDLR_PROTOMAPSVRHANDLER
#define _SM_IO_PROTOHDLR_PROTOMAPSVRHANDLER
#include "Crypto/Hash/CRC32R.h"
#include "IO/IProtocolHandler.h"
#include "Sync/Mutex.h"

namespace IO
{
	namespace ProtoHdlr
	{
		class ProtoMapSvrHandler : public IO::IProtocolHandler
		{
		private:
			IO::IProtocolHandler::DataListener *listener;
			Crypto::Hash::CRC32R *crc;
			Sync::Mutex *crcMut;
		private:
			static UInt16 CalCheck(UInt8 *buff, Int32 sz);
		public:
			ProtoMapSvrHandler(IO::IProtocolHandler::DataListener *listener);
			virtual ~ProtoMapSvrHandler();

			virtual void *CreateStreamData(IO::Stream *stm);
			virtual void DeleteStreamData(IO::Stream *stm, void *stmData);
			virtual UOSInt ParseProtocol(IO::Stream *stm, void *stmObj, void *stmData, UInt8 *buff, UOSInt buffSize); // return unprocessed size
			virtual UOSInt BuildPacket(UInt8 *buff, Int32 cmdType, Int32 seqId, UInt8 *cmd, UOSInt cmdSize, void *stmData);
		};
	}
}
#endif
