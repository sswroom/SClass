#ifndef _SM_IO_PROTOHDLR_PROTOLBSGATEHANDLER
#define _SM_IO_PROTOHDLR_PROTOLBSGATEHANDLER
#include "Crypto/Hash/CRC32R.h"
#include "IO/IProtocolHandler.h"
#include "Sync/Mutex.h"

namespace IO
{
	namespace ProtoHdlr
	{
		class ProtoLBSGateHandler : public IO::IProtocolHandler
		{
		private:
			NN<IO::IProtocolHandler::DataListener> listener;
			Sync::Mutex crcMut;
			Crypto::Hash::CRC32R crc;

		public:
			ProtoLBSGateHandler(NN<IO::IProtocolHandler::DataListener> listener);
			virtual ~ProtoLBSGateHandler();

			virtual void *CreateStreamData(NN<IO::Stream> stm);
			virtual void DeleteStreamData(NN<IO::Stream> stm, void *stmData);
			virtual UOSInt ParseProtocol(NN<IO::Stream> stm, void *stmObj, void *stmData, const Data::ByteArrayR &buff); // return unprocessed size
			virtual UOSInt BuildPacket(UInt8 *buff, Int32 cmdType, Int32 seqId, const UInt8 *cmd, UOSInt cmdSize, void *stmData);
		};
	}
}
#endif
