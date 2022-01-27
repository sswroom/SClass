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
			Net::WebServer::IWebResponse *resp;
			IO::Writer *writer;
			Text::String *prefix;

		public:
			PrintLogWebResponse(Net::WebServer::IWebResponse *resp, IO::Writer *writer, const UTF8Char *prefix);
			virtual ~PrintLogWebResponse();

			virtual void EnableWriteBuffer();
			virtual Bool SetStatusCode(Net::WebStatus::StatusCode code);
			virtual Int32 GetStatusCode();
			virtual Bool AddHeaderC(const UTF8Char *name, UOSInt nameLen, const UTF8Char *value, UOSInt valueLen);
			virtual Bool AddDefHeaders(Net::WebServer::IWebRequest *req);
			virtual UInt64 GetRespLength();
			virtual void ShutdownSend();
			virtual Bool ResponseSSE(Int32 timeoutMS, SSEDisconnectHandler hdlr, void *userObj);
			virtual Bool SSESend(const UTF8Char *eventName, const UTF8Char *data);
			virtual Text::CString GetRespHeaders();

			virtual UOSInt Read(UInt8 *buff, UOSInt size);
			virtual UOSInt Write(const UInt8 *buff, UOSInt size);
			virtual Int32 Flush();
			virtual void Close();
			virtual Bool Recover();
		};
	}
}
#endif
