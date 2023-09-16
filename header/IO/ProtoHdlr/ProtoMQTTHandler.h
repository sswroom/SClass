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
			NotNullPtr<IO::IProtocolHandler::DataListener> listener;

		public:
			ProtoMQTTHandler(NotNullPtr<IO::IProtocolHandler::DataListener> listener);
			virtual ~ProtoMQTTHandler();

			virtual void *CreateStreamData(NotNullPtr<IO::Stream> stm);
			virtual void DeleteStreamData(NotNullPtr<IO::Stream> stm, void *stmData);
			virtual UOSInt ParseProtocol(NotNullPtr<IO::Stream> stm, void *stmObj, void *stmData, const Data::ByteArrayR &buff); // return unprocessed size
			virtual UOSInt BuildPacket(UInt8 *buff, Int32 cmdType, Int32 seqId, const UInt8 *cmd, UOSInt cmdSize, void *stmData);

			Bool ParseUTF8Str(const UTF8Char *buff, UOSInt *index, UOSInt buffSize, NotNullPtr<Text::StringBuilderUTF8> sb);
		};
	}
}
#endif
