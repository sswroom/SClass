#ifndef _SM_IO_PROTOHDLR_PROTOJMVL01HANDLER
#define _SM_IO_PROTOHDLR_PROTOJMVL01HANDLER
#include "Crypto/Hash/CRC16R.h"
#include "IO/IProtocolHandler.h"
#include "Sync/Mutex.h"

namespace IO
{
	namespace ProtoHdlr
	{
		class ProtoJMVL01Handler : public IO::IProtocolHandler
		{
		private:
			NN<IO::IProtocolHandler::DataListener> listener;
			UInt64 devId;
			Crypto::Hash::CRC16R crc;

		public:
			ProtoJMVL01Handler(NN<IO::IProtocolHandler::DataListener> listener, UInt64 devId);
			virtual ~ProtoJMVL01Handler();

			virtual AnyType CreateStreamData(NN<IO::Stream> stm);
			virtual void DeleteStreamData(NN<IO::Stream> stm, AnyType stmData);
			virtual UOSInt ParseProtocol(NN<IO::Stream> stm, AnyType stmObj, AnyType stmData, const Data::ByteArrayR &buff); // return unprocessed size
			virtual UOSInt BuildPacket(UInt8 *buff, Int32 cmdType, Int32 seqId, const UInt8 *cmd, UOSInt cmdSize, AnyType stmData);
		};
	}
}
#endif
