#ifndef _SM_IO_PROTOHDLR_PROTOMQTTHANDLER
#define _SM_IO_PROTOHDLR_PROTOMQTTHANDLER
#include "IO/IProtocolHandler.h"
#include "Text/StringBuilderUTF8.h"

namespace IO
{
	namespace ProtoHdlr
	{
		class ProtoMQTTHandler : public IO::IProtocolHandler
		{
		private:
			NN<IO::IProtocolHandler::DataListener> listener;

		public:
			ProtoMQTTHandler(NN<IO::IProtocolHandler::DataListener> listener);
			virtual ~ProtoMQTTHandler();

			virtual AnyType CreateStreamData(NN<IO::Stream> stm);
			virtual void DeleteStreamData(NN<IO::Stream> stm, AnyType stmData);
			virtual UOSInt ParseProtocol(NN<IO::Stream> stm, AnyType stmObj, AnyType stmData, const Data::ByteArrayR &buff); // return unprocessed size
			virtual UOSInt BuildPacket(UInt8 *buff, Int32 cmdType, Int32 seqId, const UInt8 *cmd, UOSInt cmdSize, AnyType stmData);

			Bool ParseUTF8Str(const UTF8Char *buff, UOSInt *index, UOSInt buffSize, NN<Text::StringBuilderUTF8> sb);
		};
	}
}
#endif
