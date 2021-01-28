#ifndef _SM_IO_PROTOHDLR_PROTOMQTTHANDLER
#define _SM_IO_PROTOHDLR_PROTOMQTTHANDLER
#include "IO/IProtocolHandler.h"
#include "Text/StringBuilderUTF.h"

namespace IO
{
	namespace ProtoHdlr
	{
		class ProtoMQTTHandler : public IO::IProtocolHandler
		{
		private:
			IO::IProtocolHandler::DataListener *listener;

		public:
			ProtoMQTTHandler(IO::IProtocolHandler::DataListener *listener);
			virtual ~ProtoMQTTHandler();

			virtual void *CreateStreamData(IO::Stream *stm);
			virtual void DeleteStreamData(IO::Stream *stm, void *stmData);
			virtual UOSInt ParseProtocol(IO::Stream *stm, void *stmObj, void *stmData, const UInt8 *buff, UOSInt buffSize); // return unprocessed size
			virtual UOSInt BuildPacket(UInt8 *buff, Int32 cmdType, Int32 seqId, const UInt8 *cmd, UOSInt cmdSize, void *stmData);

			Bool ParseUTF8Str(const UTF8Char *buff, UOSInt *index, UOSInt buffSize, Text::StringBuilderUTF *sb);
		};
	}
}
#endif
