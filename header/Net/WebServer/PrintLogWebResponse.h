#ifndef _SM_NET_WEBSERVER_PRINTLOGWEBRESPONSE
#define _SM_NET_WEBSERVER_PRINTLOGWEBRESPONSE
#include "IO/Writer.h"
#include "Net/WebServer/IWebResponse.h"

namespace Net
{
	namespace WebServer
	{
		class PrintLogWebResponse : public Net::WebServer::IWebResponse
		{
		private:
			NN<Net::WebServer::IWebResponse> resp;
			NN<IO::Writer> writer;
			Optional<Text::String> prefix;

		public:
			PrintLogWebResponse(NN<Net::WebServer::IWebResponse> resp, NN<IO::Writer> writer, Text::CString prefix);
			virtual ~PrintLogWebResponse();

			virtual void EnableWriteBuffer();
			virtual Bool SetStatusCode(Net::WebStatus::StatusCode code);
			virtual Int32 GetStatusCode();
			virtual Bool AddHeader(Text::CStringNN name, Text::CStringNN value);
			virtual Bool AddDefHeaders(NN<Net::WebServer::IWebRequest> req);
			virtual UInt64 GetRespLength();
			virtual void ShutdownSend();
			virtual Bool ResponseSSE(Data::Duration timeout, SSEDisconnectHandler hdlr, AnyType userObj);
			virtual Bool SSESend(const UTF8Char *eventName, const UTF8Char *data);
			virtual Bool SwitchProtocol(ProtocolHandler *protoHdlr);
			virtual Text::CString GetRespHeaders();

			virtual Bool IsDown() const;
			virtual UOSInt Read(const Data::ByteArray &buff);
			virtual UOSInt Write(const UInt8 *buff, UOSInt size);
			virtual Int32 Flush();
			virtual void Close();
			virtual Bool Recover();
			virtual IO::StreamType GetStreamType() const;
		};
	}
}
#endif
