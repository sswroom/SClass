#ifndef _SM_IO_PROTOHDLR_PROTOSMONHANDLER
#define _SM_IO_PROTOHDLR_PROTOSMONHANDLER
#include "Crypto/Hash/HashCalc.h"
#include "IO/IProtocolHandler.h"
#include "Sync/Mutex.h"

namespace IO
{
	namespace ProtoHdlr
	{
		class ProtoSMonHandler : public IO::IProtocolHandler
		{
		private:
			NN<IO::IProtocolHandler::DataListener> listener;
			NN<Crypto::Hash::HashCalc> crc;

		public:
			ProtoSMonHandler(NN<IO::IProtocolHandler::DataListener> listener);
			virtual ~ProtoSMonHandler();

			virtual AnyType CreateStreamData(NN<IO::Stream> stm);
			virtual void DeleteStreamData(NN<IO::Stream> stm, AnyType stmData);
			virtual UOSInt ParseProtocol(NN<IO::Stream> stm, AnyType stmObj, AnyType stmData, const Data::ByteArrayR &buff); // return unprocessed size
			virtual UOSInt BuildPacket(UInt8 *buff, Int32 cmdType, Int32 seqId, const UInt8 *cmd, UOSInt cmdSize, AnyType stmData);
		};
	}
}
#endif
