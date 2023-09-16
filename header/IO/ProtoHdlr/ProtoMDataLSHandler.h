#ifndef _SM_IO_PROTOHDLR_PROTOMDATALSHANDLER
#define _SM_IO_PROTOHDLR_PROTOMDATALSHANDLER
#include "Crypto/Hash/HashCalc.h"
#include "IO/IProtocolHandler.h"
#include "Sync/Mutex.h"

namespace IO
{
	namespace ProtoHdlr
	{
		class ProtoMDataLSHandler : public IO::IProtocolHandler
		{
		private:
			NotNullPtr<IO::IProtocolHandler::DataListener> listener;
			NotNullPtr<Crypto::Hash::HashCalc> crc;

		public:
			ProtoMDataLSHandler(NotNullPtr<IO::IProtocolHandler::DataListener> listener);
			virtual ~ProtoMDataLSHandler();

			virtual void *CreateStreamData(NotNullPtr<IO::Stream> stm);
			virtual void DeleteStreamData(NotNullPtr<IO::Stream> stm, void *stmData);
			virtual UOSInt ParseProtocol(NotNullPtr<IO::Stream> stm, void *stmObj, void *stmData, const Data::ByteArrayR &buff); // return unprocessed size
			virtual UOSInt BuildPacket(UInt8 *buff, Int32 cmdType, Int32 seqId, const UInt8 *cmd, UOSInt cmdSize, void *stmData);
		};
	}
}
#endif
