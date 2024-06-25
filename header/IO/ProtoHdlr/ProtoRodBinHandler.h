#ifndef _SM_IO_PROTOHDLR_PROTORODBINHANDLER
#define _SM_IO_PROTOHDLR_PROTORODBINHANDLER
#include "IO/IProtocolHandler.h"
#include "Sync/Mutex.h"

namespace IO
{
	namespace ProtoHdlr
	{
		class ProtoRodBinHandler : public IO::IProtocolHandler
		{
		private:
			NN<IO::IProtocolHandler::DataListener> listener;

		public:
			ProtoRodBinHandler(NN<IO::IProtocolHandler::DataListener> listener);
			virtual ~ProtoRodBinHandler();

			virtual AnyType CreateStreamData(NN<IO::Stream> stm);
			virtual void DeleteStreamData(NN<IO::Stream> stm, AnyType stmData);
			virtual UOSInt ParseProtocol(NN<IO::Stream> stm, AnyType stmObj, AnyType stmData, const Data::ByteArrayR &buff); // return unprocessed size
			virtual UOSInt BuildPacket(UnsafeArray<UInt8> buff, Int32 cmdType, Int32 seqId, UnsafeArray<const UInt8> cmd, UOSInt cmdSize, AnyType stmData);

			UInt16 CalCheck(UnsafeArray<const UInt8> buff, UOSInt buffSize);
		};
	}
}
#endif
