#ifndef _SM_IO_PROTOHDLR_PROTOREVGEOHANDLER
#define _SM_IO_PROTOHDLR_PROTOREVGEOHANDLER
#include "Crypto/Hash/HashCalc.h"
#include "IO/IProtocolHandler.h"
#include "Sync/Mutex.h"

namespace IO
{
	namespace ProtoHdlr
	{
		class ProtoRevGeoHandler : public IO::IProtocolHandler
		{
		private:
			NotNullPtr<IO::IProtocolHandler::DataListener> listener;
			NotNullPtr<Crypto::Hash::HashCalc> crc;

		public:
			ProtoRevGeoHandler(NotNullPtr<IO::IProtocolHandler::DataListener> listener);
			virtual ~ProtoRevGeoHandler();

			virtual void *CreateStreamData(NotNullPtr<IO::Stream> stm);
			virtual void DeleteStreamData(NotNullPtr<IO::Stream> stm, void *stmData);
			virtual UOSInt ParseProtocol(NotNullPtr<IO::Stream> stm, void *stmObj, void *stmData, const Data::ByteArrayR &buff); // return unprocessed size
			virtual UOSInt BuildPacket(UInt8 *buff, Int32 cmdType, Int32 seqId, const UInt8 *cmd, UOSInt cmdSize, void *stmData);
		};
	}
}
#endif
