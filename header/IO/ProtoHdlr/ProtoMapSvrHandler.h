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
			NN<IO::IProtocolHandler::DataListener> listener;
			Crypto::Hash::CRC32R crc;
			Sync::Mutex crcMut;
		private:
			static UInt16 CalCheck(const UInt8 *buff, Int32 sz);
		public:
			ProtoMapSvrHandler(NN<IO::IProtocolHandler::DataListener> listener);
			virtual ~ProtoMapSvrHandler();

			virtual void *CreateStreamData(NN<IO::Stream> stm);
			virtual void DeleteStreamData(NN<IO::Stream> stm, void *stmData);
			virtual UOSInt ParseProtocol(NN<IO::Stream> stm, void *stmObj, void *stmData, const Data::ByteArrayR &buff); // return unprocessed size
			virtual UOSInt BuildPacket(UInt8 *buff, Int32 cmdType, Int32 seqId, const UInt8 *cmd, UOSInt cmdSize, void *stmData);
		};
	}
}
#endif
