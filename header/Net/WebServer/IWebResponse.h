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
			IWebResponse(const UTF8Char *sourceName);
			virtual ~IWebResponse();

			virtual Bool SetStatusCode(Net::WebStatus::StatusCode code) = 0;
			virtual Int32 GetStatusCode() = 0;
			virtual Bool AddHeader(const UTF8Char *name, const UTF8Char *value) = 0;
			virtual Bool AddDefHeaders(Net::WebServer::IWebRequest *req) = 0;
			virtual Int64 GetRespLength() = 0;
			virtual void ShutdownSend() = 0;

			Bool ResponseError(Net::WebServer::IWebRequest *req, Net::WebStatus::StatusCode code);
			Bool RedirectURL(Net::WebServer::IWebRequest *req, const UTF8Char *url, OSInt cacheAge);
			Bool ResponseNotModified(Net::WebServer::IWebRequest *req, OSInt cacheAge);

			Bool AddCacheControl(OSInt cacheAge);
			Bool AddTimeHeader(const UTF8Char *name, Data::DateTime *dt);
			Bool AddContentDisposition(Bool isAttachment, const UTF8Char *attFileName, Net::BrowserInfo::BrowserType browser);
			Bool AddContentLength(Int64 contentLeng);
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
