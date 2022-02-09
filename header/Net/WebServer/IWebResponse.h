#ifndef _SM_NET_WEBSERVER_IWEBRESPONSE
#define _SM_NET_WEBSERVER_IWEBRESPONSE
#include "Data/DateTime.h"
#include "IO/Stream.h"
#include "Net/WebStatus.h"
#include "Net/WebServer/IWebRequest.h"
#include "Text/CString.h"
#include "Text/TextBinEnc/URIEncoding.h"

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
			IWebResponse(Text::CString sourceName);
			virtual ~IWebResponse();

			virtual void EnableWriteBuffer() = 0;
			virtual Bool SetStatusCode(Net::WebStatus::StatusCode code) = 0;
			virtual Int32 GetStatusCode() = 0;
			virtual Bool AddHeaderC(const UTF8Char *name, UOSInt nameLen, const UTF8Char *value, UOSInt valueLen) = 0;
			virtual Bool AddDefHeaders(Net::WebServer::IWebRequest *req) = 0;
			virtual UInt64 GetRespLength() = 0;
			virtual void ShutdownSend() = 0;
			virtual Bool ResponseSSE(Int32 timeoutMS, SSEDisconnectHandler hdlr, void *userObj) = 0;
			virtual Bool SSESend(const UTF8Char *eventName, const UTF8Char *data) = 0;
			virtual Text::CString GetRespHeaders() = 0;

			Bool ResponseError(Net::WebServer::IWebRequest *req, Net::WebStatus::StatusCode code);
			Bool RedirectURL(Net::WebServer::IWebRequest *req, const UTF8Char *url, UOSInt urlLen, OSInt cacheAge);
			Bool ResponseNotModified(Net::WebServer::IWebRequest *req, OSInt cacheAge);
			Bool ResponseText(const UTF8Char *txt);
			Bool ResponseText(const UTF8Char *txt, const UTF8Char *contentType);

			Bool AddHeaderS(const UTF8Char *name, UOSInt nameLen, Text::String *value)
			{
				return AddHeaderC(name, nameLen, value->v, value->leng);
			}

			Bool AddCacheControl(OSInt cacheAge)
			{
				if (cacheAge == -2)
				{

				}
				else if (cacheAge == -1)
				{
					return this->AddHeaderC(UTF8STRC("Cache-Control"), UTF8STRC("private"));
				}
				else if (cacheAge == 0)
				{
					return this->AddHeaderC(UTF8STRC("Cache-Control"), UTF8STRC("no-cache"));
				}
				else
				{
					UTF8Char sbuff[256];
					UTF8Char *sptr;
					sptr = Text::StrOSInt(Text::StrConcatC(sbuff, UTF8STRC("private; max-age=")), cacheAge);
					return this->AddHeaderC(UTF8STRC("Cache-Control"), sbuff, (UOSInt)(sptr - sbuff));
				}
				return true;
			}

			Bool AddTimeHeader(const UTF8Char *name, UOSInt nameLen, Data::DateTime *dt)
			{
				UTF8Char sbuff[256];
				UTF8Char *sptr = ToTimeString(sbuff, dt);
				return this->AddHeaderC(name, nameLen, sbuff, (UOSInt)(sptr - sbuff));
			}

			Bool AddContentDisposition(Bool isAttachment, const UTF8Char *attFileName, Net::BrowserInfo::BrowserType browser)
			{
				UTF8Char sbuff[512];
				UTF8Char *sptr;
				if (isAttachment)
				{
					sptr = Text::StrConcatC(sbuff, UTF8STRC("attachment"));
				}
				else
				{
					sptr = Text::StrConcatC(sbuff, UTF8STRC("inline"));
				}
				if (attFileName)
				{
					sptr = Text::StrConcatC(sptr, UTF8STRC("; filename=\""));
					if (browser == Net::BrowserInfo::BT_IE)
					{
						sptr = Text::TextBinEnc::URIEncoding::URIEncode(sptr, attFileName);
					}
					else
					{
						sptr = Text::TextBinEnc::URIEncoding::URIEncode(sptr, attFileName);
					}
					sptr = Text::StrConcatC(sptr, UTF8STRC("\""));
				}
				return this->AddHeaderC(UTF8STRC("Content-Disposition"), sbuff, (UOSInt)(sptr - sbuff));
			}

			Bool AddContentLength(UInt64 contentLeng)
			{
				UTF8Char sbuff[22];
				UTF8Char *sptr = Text::StrUInt64(sbuff, contentLeng);
				return this->AddHeaderC(UTF8STRC("Content-Length"), sbuff, (UOSInt)(sptr - sbuff));
			}

			Bool AddContentType(const UTF8Char *contentType, UOSInt len)
			{
				return this->AddHeaderC(UTF8STRC("Content-Type"), contentType, len);
			}

			Bool AddDate(Data::DateTime *dt)
			{
				return this->AddTimeHeader(UTF8STRC("Date"), dt);
			}

			Bool AddExpireTime(Data::DateTime *dt)
			{
				if (dt == 0)
				{
					return this->AddHeaderC(UTF8STRC("Expires"), UTF8STRC("0"));
				}
				else
				{
					return this->AddTimeHeader(UTF8STRC("Expires"), dt);
				}
			}

			Bool AddLastModified(Data::DateTime *dt)
			{
				return this->AddTimeHeader(UTF8STRC("Last-Modified"), dt);
			}

			Bool AddServer(const UTF8Char *server, UOSInt len)
			{
				return this->AddHeaderC(UTF8STRC("Server"), server, len);
			}

			static UTF8Char *ToTimeString(UTF8Char *buff, Data::DateTime *dt);
		};
	}
}
#endif
