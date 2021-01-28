#ifndef _SM_IO_PROTOHDLR_PROTOREVGEOHANDLER
#define _SM_IO_PROTOHDLR_PROTOREVGEOHANDLER
#include "Crypto/Hash/CRC32R.h"
#include "IO/IProtocolHandler.h"
#include "Sync/Mutex.h"

namespace IO
{
	namespace ProtoHdlr
	{
		class ProtoRevGeoHandler : public IO::IProtocolHandler
		{
		private:
			IO::IProtocolHandler::DataListener *listener;
			Crypto::Hash::CRC32R *crc;
			Sync::Mutex *crcMut;

		public:
			ProtoRevGeoHandler(IO::IProtocolHandler::DataListener *listener);
			virtual ~ProtoRevGeoHandler();

			virtual void *CreateStreamData(IO::Stream *stm);
			virtual void DeleteStreamData(IO::Stream *stm, void *stmData);
			virtual UOSInt ParseProtocol(IO::Stream *stm, void *stmObj, void *stmData, const UInt8 *buff, UOSInt buffSize); // return unprocessed size
			virtual UOSInt BuildPacket(UInt8 *buff, Int32 cmdType, Int32 seqId, const UInt8 *cmd, UOSInt cmdSize, void *stmData);
		};
	}
}
#endif
