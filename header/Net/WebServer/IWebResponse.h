#ifndef _SM_NET_WEBSERVER_IWEBRESPONSE
#define _SM_NET_WEBSERVER_IWEBRESPONSE
#include "Data/DateTime.h"
#include "IO/Stream.h"
#include "Net/WebStatus.h"
#include "Net/WebServer/IWebRequest.h"

namespace Net
{
	namespace WebServer
	{
		class IWebResponse : public IO::Stream
		{
		public:
			typedef void (__stdcall *SSEDisconnectHandler)(Net::WebServer::IWebResponse *resp, void *userObj);
		public:
			IWebResponse(Text::String *sourceName);
			IWebResponse(const UTF8Char *sourceName);
			virtual ~IWebResponse();

			virtual void EnableWriteBuffer() = 0;
			virtual Bool SetStatusCode(Net::WebStatus::StatusCode code) = 0;
			virtual Int32 GetStatusCode() = 0;
			virtual Bool AddHeader(const UTF8Char *name, const UTF8Char *value) = 0;
			virtual Bool AddDefHeaders(Net::WebServer::IWebRequest *req) = 0;
			virtual UInt64 GetRespLength() = 0;
			virtual void ShutdownSend() = 0;
			virtual Bool ResponseSSE(Int32 timeoutMS, SSEDisconnectHandler hdlr, void *userObj) = 0;
			virtual Bool SSESend(const UTF8Char *eventName, const UTF8Char *data) = 0;

			Bool ResponseError(Net::WebServer::IWebRequest *req, Net::WebStatus::StatusCode code);
			Bool RedirectURL(Net::WebServer::IWebRequest *req, const UTF8Char *url, OSInt cacheAge);
			Bool ResponseNotModified(Net::WebServer::IWebRequest *req, OSInt cacheAge);
			Bool ResponseText(const UTF8Char *txt);
			Bool ResponseText(const UTF8Char *txt, const UTF8Char *contentType);

			Bool AddCacheControl(OSInt cacheAge);
			Bool AddTimeHeader(const UTF8Char *name, Data::DateTime *dt);
			Bool AddContentDisposition(Bool isAttachment, const UTF8Char *attFileName, Net::BrowserInfo::BrowserType browser);
			Bool AddContentLength(UInt64 contentLeng);
			Bool AddContentType(const UTF8Char *contentType);
			Bool AddDate(Data::DateTime *dt);
			Bool AddExpireTime(Data::DateTime *dt);
			Bool AddLastModified(Data::DateTime *dt);
			Bool AddServer(const UTF8Char *server);
			static UTF8Char *ToTimeString(UTF8Char *buff, Data::DateTime *dt);
		};
	}
}
#endif
