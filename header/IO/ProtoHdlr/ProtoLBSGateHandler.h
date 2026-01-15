#ifndef _SM_IO_PROTOHDLR_PROTOLBSGATEHANDLER
#define _SM_IO_PROTOHDLR_PROTOLBSGATEHANDLER
#include "Crypto/Hash/CRC32R.h"
#include "IO/ProtocolHandler.h"
#include "Sync/Mutex.h"

namespace IO
{
	namespace ProtoHdlr
	{
		class ProtoLBSGateHandler : public IO::ProtocolHandler
		{
		private:
			NN<IO::ProtocolHandler::DataListener> listener;
			Sync::Mutex crcMut;
			Crypto::Hash::CRC32R crc;

		public:
			ProtoLBSGateHandler(NN<IO::ProtocolHandler::DataListener> listener);
			virtual ~ProtoLBSGateHandler();

			virtual AnyType CreateStreamData(NN<IO::Stream> stm);
			virtual void DeleteStreamData(NN<IO::Stream> stm, AnyType stmData);
			virtual UIntOS ParseProtocol(NN<IO::Stream> stm, AnyType stmObj, AnyType stmData, const Data::ByteArrayR &buff); // return unprocessed size
			virtual UIntOS BuildPacket(UnsafeArray<UInt8> buff, Int32 cmdType, Int32 seqId, UnsafeArray<const UInt8> cmd, UIntOS cmdSize, AnyType stmData);
		};
	}
}
#endif
