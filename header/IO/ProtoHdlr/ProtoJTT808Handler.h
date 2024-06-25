#ifndef _SM_IO_PROTOHDLR_PROTOJTT808HANDLER
#define _SM_IO_PROTOHDLR_PROTOJTT808HANDLER
#include "IO/IProtocolHandler.h"
#include "Sync/Mutex.h"

namespace IO
{
	namespace ProtoHdlr
	{
		class ProtoJTT808Handler : public IO::IProtocolHandler
		{
		private:
			NN<IO::IProtocolHandler::DataListener> listener;
			UInt64 devId;

		public:
			ProtoJTT808Handler(NN<IO::IProtocolHandler::DataListener> listener, UInt64 devId);
			virtual ~ProtoJTT808Handler();

			virtual AnyType CreateStreamData(NN<IO::Stream> stm);
			virtual void DeleteStreamData(NN<IO::Stream> stm, AnyType stmData);
			virtual UOSInt ParseProtocol(NN<IO::Stream> stm, AnyType stmObj, AnyType stmData, const Data::ByteArrayR &buff); // return unprocessed size
			virtual UOSInt BuildPacket(UnsafeArray<UInt8> buff, Int32 cmdType, Int32 seqId, UnsafeArray<const UInt8> cmd, UOSInt cmdSize, AnyType stmData);

			UnsafeArray<const UInt8> GetPacketContent(UnsafeArray<const UInt8> packet, OutParam<UOSInt> contSize);
		};
	}
}
#endif
