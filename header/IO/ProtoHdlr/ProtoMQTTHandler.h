#ifndef _SM_IO_PROTOHDLR_PROTOMQTTHANDLER
#define _SM_IO_PROTOHDLR_PROTOMQTTHANDLER
#include "IO/ProtocolHandler.h"
#include "Text/StringBuilderUTF8.h"

namespace IO
{
	namespace ProtoHdlr
	{
		class ProtoMQTTHandler : public IO::ProtocolHandler
		{
		private:
			NN<IO::ProtocolHandler::DataListener> listener;

		public:
			ProtoMQTTHandler(NN<IO::ProtocolHandler::DataListener> listener);
			virtual ~ProtoMQTTHandler();

			virtual AnyType CreateStreamData(NN<IO::Stream> stm);
			virtual void DeleteStreamData(NN<IO::Stream> stm, AnyType stmData);
			virtual UIntOS ParseProtocol(NN<IO::Stream> stm, AnyType stmObj, AnyType stmData, const Data::ByteArrayR &buff); // return unprocessed size
			virtual UIntOS BuildPacket(UnsafeArray<UInt8> buff, Int32 cmdType, Int32 seqId, UnsafeArray<const UInt8> cmd, UIntOS cmdSize, AnyType stmData);

			Bool ParseUTF8Str(UnsafeArray<const UTF8Char> buff, InOutParam<UIntOS> index, UIntOS buffSize, NN<Text::StringBuilderUTF8> sb);
		};
	}
}
#endif
