#ifndef _SM_IO_PROTOHDLR_PROTOMAPSVRHANDLER
#define _SM_IO_PROTOHDLR_PROTOMAPSVRHANDLER
#include "Crypto/Hash/CRC32R.h"
#include "IO/ProtocolHandler.h"
#include "Sync/Mutex.h"

namespace IO
{
	namespace ProtoHdlr
	{
		class ProtoMapSvrHandler : public IO::ProtocolHandler
		{
		private:
			NN<IO::ProtocolHandler::DataListener> listener;
			Crypto::Hash::CRC32R crc;
			Sync::Mutex crcMut;
		private:
			static UInt16 CalCheck(UnsafeArray<const UInt8> buff, Int32 sz);
		public:
			ProtoMapSvrHandler(NN<IO::ProtocolHandler::DataListener> listener);
			virtual ~ProtoMapSvrHandler();

			virtual AnyType CreateStreamData(NN<IO::Stream> stm);
			virtual void DeleteStreamData(NN<IO::Stream> stm, AnyType stmData);
			virtual UOSInt ParseProtocol(NN<IO::Stream> stm, AnyType stmObj, AnyType stmData, const Data::ByteArrayR &buff); // return unprocessed size
			virtual UOSInt BuildPacket(UnsafeArray<UInt8> buff, Int32 cmdType, Int32 seqId, UnsafeArray<const UInt8> cmd, UOSInt cmdSize, AnyType stmData);
		};
	}
}
#endif
