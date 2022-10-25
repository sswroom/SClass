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
			virtual Bool AddHeader(Text::CString name, Text::CString value) = 0;
			virtual Bool AddDefHeaders(Net::WebServer::IWebRequest *req) = 0;
			virtual UInt64 GetRespLength() = 0;
			virtual void ShutdownSend() = 0;
			virtual Bool ResponseSSE(Int32 timeoutMS, SSEDisconnectHandler hdlr, void *userObj) = 0;
			virtual Bool SSESend(const UTF8Char *eventName, const UTF8Char *data) = 0;
			virtual Text::CString GetRespHeaders() = 0;

			Bool ResponseError(Net::WebServer::IWebRequest *req, Net::WebStatus::StatusCode code);
			Bool RedirectURL(Net::WebServer::IWebRequest *req, Text::CString url, OSInt cacheAge);
			Bool ResponseNotModified(Net::WebServer::IWebRequest *req, OSInt cacheAge);
			Bool ResponseText(Text::CString txt);
			Bool ResponseText(Text::CString txt, Text::CString contentType);
			Bool ResponseJSONStr(Net::WebServer::IWebRequest *req, OSInt cacheAge, Text::CString json);

			Bool AddHeaderS(Text::CString name, Text::String *value)
			{
				return AddHeader(name, value->ToCString());
			}

			Bool AddCacheControl(OSInt cacheAge)
			{
				if (cacheAge == -2)
				{
					return this->AddHeader(CSTR("Cache-Control"), CSTR("public"));
				}
				else if (cacheAge == -1)
				{
					return this->AddHeader(CSTR("Cache-Control"), CSTR("private"));
				}
				else if (cacheAge == 0)
				{
					return this->AddHeader(CSTR("Cache-Control"), CSTR("no-cache"));
				}
				else
				{
					UTF8Char sbuff[256];
					UTF8Char *sptr;
					sptr = Text::StrOSInt(Text::StrConcatC(sbuff, UTF8STRC("private; max-age=")), cacheAge);
					return this->AddHeader(CSTR("Cache-Control"), CSTRP(sbuff, sptr));
				}
				return true;
			}

			Bool AddTimeHeader(Text::CString name, Data::DateTime *dt)
			{
				UTF8Char sbuff[256];
				UTF8Char *sptr = ToTimeString(sbuff, dt);
				return this->AddHeader(name, CSTRP(sbuff, sptr));
			}

			Bool AddTimeHeader(Text::CString name, Data::Timestamp ts)
			{
				UTF8Char sbuff[256];
				UTF8Char *sptr = ToTimeString(sbuff, ts);
				return this->AddHeader(name, CSTRP(sbuff, sptr));
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
				return this->AddHeader(CSTR("Content-Disposition"), CSTRP(sbuff, sptr));
			}

			Bool AddContentLength(UInt64 contentLeng)
			{
				UTF8Char sbuff[22];
				UTF8Char *sptr = Text::StrUInt64(sbuff, contentLeng);
				return this->AddHeader(CSTR("Content-Length"), CSTRP(sbuff, sptr));
			}

			Bool AddContentType(Text::CString contentType)
			{
				return this->AddHeader(CSTR("Content-Type"), contentType);
			}

			Bool AddDate(Data::DateTime *dt)
			{
				return this->AddTimeHeader(CSTR("Date"), dt);
			}

			Bool AddExpireTime(Data::DateTime *dt)
			{
				if (dt == 0)
				{
					return this->AddHeader(CSTR("Expires"), CSTR("0"));
				}
				else
				{
					return this->AddTimeHeader(CSTR("Expires"), dt);
				}
			}

			Bool AddLastModified(Data::DateTime *dt)
			{
				return this->AddTimeHeader(CSTR("Last-Modified"), dt);
			}

			Bool AddLastModified(Data::Timestamp ts)
			{
				return this->AddTimeHeader(CSTR("Last-Modified"), ts);
			}

			Bool AddServer(Text::CString server)
			{
				return this->AddHeader(CSTR("Server"), server);
			}

			static UTF8Char *ToTimeString(UTF8Char *buff, Data::DateTime *dt);
			static UTF8Char *ToTimeString(UTF8Char *buff, Data::Timestamp ts);
		};
	}
}
#endif
