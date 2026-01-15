#ifndef _SM_NET_WEBSERVER_PRINTLOGWEBRESPONSE
#define _SM_NET_WEBSERVER_PRINTLOGWEBRESPONSE
#include "IO/Writer.h"
#include "Net/WebServer/WebResponse.h"

namespace Net
{
	namespace WebServer
	{
		class PrintLogWebResponse : public Net::WebServer::WebResponse
		{
		private:
			NN<Net::WebServer::WebResponse> resp;
			NN<IO::Writer> writer;
			Optional<Text::String> prefix;

		public:
			PrintLogWebResponse(NN<Net::WebServer::WebResponse> resp, NN<IO::Writer> writer, Text::CString prefix);
			virtual ~PrintLogWebResponse();

			virtual void EnableWriteBuffer();
			virtual Bool SetStatusCode(Net::WebStatus::StatusCode code);
			virtual Int32 GetStatusCode();
			virtual Bool AddHeader(Text::CStringNN name, Text::CStringNN value);
			virtual Bool AddDefHeaders(NN<Net::WebServer::WebRequest> req);
			virtual UInt64 GetRespLength();
			virtual void ShutdownSend();
			virtual Bool ResponseSSE(Data::Duration timeout, SSEDisconnectHandler hdlr, AnyType userObj);
			virtual Bool SSESend(const UTF8Char *eventName, const UTF8Char *data);
			virtual Bool SwitchProtocol(Optional<ProtocolHandler> protoHdlr);
			virtual Text::CStringNN GetRespHeaders();

			virtual Bool IsDown() const;
			virtual UIntOS Read(const Data::ByteArray &buff);
			virtual UIntOS Write(Data::ByteArrayR buff);
			virtual Int32 Flush();
			virtual void Close();
			virtual Bool Recover();
			virtual IO::StreamType GetStreamType() const;
		};
	}
}
#endif
