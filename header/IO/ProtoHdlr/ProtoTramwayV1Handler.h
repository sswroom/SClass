#ifndef _SM_IO_PROTOHDLR_PROTOTRAMWAYV1HANDLER
#define _SM_IO_PROTOHDLR_PROTOTRAMWAYV1HANDLER
#include "IO/ProtocolHandler.h"
#include "Sync/Mutex.h"

namespace IO
{
	namespace ProtoHdlr
	{
		class ProtoTramwayV1Handler : public IO::ProtocolHandler
		{
		private:
			typedef struct
			{
				UnsafeArray<UInt8> packetBuff;
				UIntOS buffSize;
			} ProtocolStatus;
		private:
			NN<IO::ProtocolHandler::DataListener> listener;

		public:
			ProtoTramwayV1Handler(NN<IO::ProtocolHandler::DataListener> listener);
			virtual ~ProtoTramwayV1Handler();

			virtual AnyType CreateStreamData(NN<IO::Stream> stm);
			virtual void DeleteStreamData(NN<IO::Stream> stm, AnyType stmData);
			virtual UIntOS ParseProtocol(NN<IO::Stream> stm, AnyType stmObj, AnyType stmData, const Data::ByteArrayR &buff); // return unprocessed size
			virtual UIntOS BuildPacket(UnsafeArray<UInt8> buff, Int32 cmdType, Int32 seqId, UnsafeArray<const UInt8> cmd, UIntOS cmdSize, AnyType stmData);
		};
	}
}
#endif
