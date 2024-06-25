#ifndef _SM_IO_PROTOHDLR_PROTOGPSDEVINFOHANDLER
#define _SM_IO_PROTOHDLR_PROTOGPSDEVINFOHANDLER
#include "Crypto/Hash/CRC32R.h"
#include "IO/IProtocolHandler.h"
#include "Sync/Mutex.h"

namespace IO
{
	namespace ProtoHdlr
	{
		class ProtoGPSDevInfoHandler : public IO::IProtocolHandler
		{
		private:
			NN<IO::IProtocolHandler::DataListener> listener;
			Sync::Mutex crcMut;
			Crypto::Hash::CRC32R crc;

		public:
			ProtoGPSDevInfoHandler(NN<IO::IProtocolHandler::DataListener> listener);
			virtual ~ProtoGPSDevInfoHandler();

			virtual AnyType CreateStreamData(NN<IO::Stream> stm);
			virtual void DeleteStreamData(NN<IO::Stream> stm, AnyType stmData);
			virtual UOSInt ParseProtocol(NN<IO::Stream> stm, AnyType stmObj, AnyType stmData, const Data::ByteArrayR &buff); // return unprocessed size
			virtual UOSInt BuildPacket(UnsafeArray<UInt8> buff, Int32 cmdType, Int32 seqId, UnsafeArray<const UInt8> cmd, UOSInt cmdSize, AnyType stmData);
		};
	}
}
#endif
