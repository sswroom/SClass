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
			NotNullPtr<Net::WebServer::IWebResponse> resp;
			NotNullPtr<IO::Writer> writer;
			Text::String *prefix;

		public:
			PrintLogWebResponse(NotNullPtr<Net::WebServer::IWebResponse> resp, NotNullPtr<IO::Writer> writer, Text::CString prefix);
			virtual ~PrintLogWebResponse();

			virtual void EnableWriteBuffer();
			virtual Bool SetStatusCode(Net::WebStatus::StatusCode code);
			virtual Int32 GetStatusCode();
			virtual Bool AddHeader(Text::CString name, Text::CString value);
			virtual Bool AddDefHeaders(NotNullPtr<Net::WebServer::IWebRequest> req);
			virtual UInt64 GetRespLength();
			virtual void ShutdownSend();
			virtual Bool ResponseSSE(Data::Duration timeout, SSEDisconnectHandler hdlr, void *userObj);
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
