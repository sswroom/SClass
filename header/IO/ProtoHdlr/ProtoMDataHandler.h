#ifndef _SM_IO_PROTOHDLR_PROTOMDATAHANDLER
#define _SM_IO_PROTOHDLR_PROTOMDATAHANDLER
#include "Crypto/Hash/HashCalc.h"
#include "IO/IProtocolHandler.h"
#include "Sync/Mutex.h"

namespace IO
{
	namespace ProtoHdlr
	{
		class ProtoMDataHandler : public IO::IProtocolHandler
		{
		private:
			NotNullPtr<IO::IProtocolHandler::DataListener> listener;
			NotNullPtr<Crypto::Hash::HashCalc> crc;

		public:
			ProtoMDataHandler(NotNullPtr<IO::IProtocolHandler::DataListener> listener);
			virtual ~ProtoMDataHandler();

			virtual AnyType CreateStreamData(NotNullPtr<IO::Stream> stm);
			virtual void DeleteStreamData(NotNullPtr<IO::Stream> stm, AnyType stmData);
			virtual UOSInt ParseProtocol(NotNullPtr<IO::Stream> stm, AnyType stmObj, AnyType stmData, const Data::ByteArrayR &buff); // return unprocessed size
			virtual UOSInt BuildPacket(UInt8 *buff, Int32 cmdType, Int32 seqId, const UInt8 *cmd, UOSInt cmdSize, AnyType stmData);
		};
	}
}
#endif
