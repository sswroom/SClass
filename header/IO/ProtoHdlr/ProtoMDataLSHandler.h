#ifndef _SM_IO_PROTOHDLR_PROTOMDATALSHANDLER
#define _SM_IO_PROTOHDLR_PROTOMDATALSHANDLER
#include "Crypto/Hash/HashCalc.h"
#include "IO/ProtocolHandler.h"
#include "Sync/Mutex.h"

namespace IO
{
	namespace ProtoHdlr
	{
		class ProtoMDataLSHandler : public IO::ProtocolHandler
		{
		private:
			NN<IO::ProtocolHandler::DataListener> listener;
			NN<Crypto::Hash::HashCalc> crc;

		public:
			ProtoMDataLSHandler(NN<IO::ProtocolHandler::DataListener> listener);
			virtual ~ProtoMDataLSHandler();

			virtual AnyType CreateStreamData(NN<IO::Stream> stm);
			virtual void DeleteStreamData(NN<IO::Stream> stm, AnyType stmData);
			virtual UOSInt ParseProtocol(NN<IO::Stream> stm, AnyType stmObj, AnyType stmData, const Data::ByteArrayR &buff); // return unprocessed size
			virtual UOSInt BuildPacket(UnsafeArray<UInt8> buff, Int32 cmdType, Int32 seqId, UnsafeArray<const UInt8> cmd, UOSInt cmdSize, AnyType stmData);
		};
	}
}
#endif
