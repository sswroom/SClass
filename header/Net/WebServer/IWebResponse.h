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
		enum class SameSiteType
		{
			Unspecified,
			None,
			Lax,
			Strict
		};

		class ProtocolHandler
		{
		public:
			virtual void ProtocolData(const UInt8 *data, UOSInt dataSize) = 0;
			virtual void ConnectionClosed() = 0;
		};

		class IWebResponse : public IO::Stream
		{
		public:
			typedef void (__stdcall *SSEDisconnectHandler)(NotNullPtr<Net::WebServer::IWebResponse> resp, void *userObj);
		public:
			IWebResponse(NotNullPtr<Text::String> sourceName);
			IWebResponse(Text::CStringNN sourceName);
			virtual ~IWebResponse();

			virtual void EnableWriteBuffer() = 0;
			virtual Bool SetStatusCode(Net::WebStatus::StatusCode code) = 0;
			virtual Int32 GetStatusCode() = 0;
			virtual Bool AddHeader(Text::CStringNN name, Text::CStringNN value) = 0;
			virtual Bool AddDefHeaders(NotNullPtr<Net::WebServer::IWebRequest> req) = 0;
			virtual UInt64 GetRespLength() = 0;
			virtual void ShutdownSend() = 0;
			virtual Bool ResponseSSE(Data::Duration timeout, SSEDisconnectHandler hdlr, void *userObj) = 0;
			virtual Bool SSESend(const UTF8Char *eventName, const UTF8Char *data) = 0;
			virtual Bool SwitchProtocol(ProtocolHandler *protoHdlr) = 0;
			virtual Text::CString GetRespHeaders() = 0;

			Bool ResponseError(NotNullPtr<Net::WebServer::IWebRequest> req, Net::WebStatus::StatusCode code);
			Bool RedirectURL(NotNullPtr<Net::WebServer::IWebRequest> req, Text::CStringNN url, OSInt cacheAge);
			Bool VirtualRedirectURL(NotNullPtr<Net::WebServer::IWebRequest> req, Text::CStringNN url, OSInt cacheAge);
			Bool ResponseNotModified(NotNullPtr<Net::WebServer::IWebRequest> req, OSInt cacheAge);
			Bool ResponseText(Text::CStringNN txt);
			Bool ResponseText(Text::CStringNN txt, Text::CStringNN contentType);
			Bool ResponseJSONStr(NotNullPtr<Net::WebServer::IWebRequest> req, OSInt cacheAge, Text::CStringNN json);

			Bool AddHeaderS(Text::CStringNN name, NotNullPtr<Text::String> value)
			{
				return AddHeader(name, value->ToCString());
			}

			Bool AddCacheControl(OSInt cacheAge)
			{
				if (cacheAge < 0)
				{
					return this->AddHeader(CSTR("Cache-Control"), CSTR("public, max-age, immutable"));
				}
				else if (cacheAge == 0)
				{
					return this->AddHeader(CSTR("Cache-Control"), CSTR("no-cache, no-store, must-revalidate"));
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

			Bool AddTimeHeader(Text::CStringNN name, NotNullPtr<Data::DateTime> dt)
			{
				UTF8Char sbuff[256];
				UTF8Char *sptr = Net::WebUtil::Date2Str(sbuff, dt);
				return this->AddHeader(name, CSTRP(sbuff, sptr));
			}

			Bool AddTimeHeader(Text::CStringNN name, const Data::Timestamp &ts)
			{
				UTF8Char sbuff[256];
				UTF8Char *sptr = Net::WebUtil::Date2Str(sbuff, ts);
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

			Bool AddContentType(Text::CStringNN contentType)
			{
				return this->AddHeader(CSTR("Content-Type"), contentType);
			}

			Bool AddDate(NotNullPtr<Data::DateTime> dt)
			{
				return this->AddTimeHeader(CSTR("Date"), dt);
			}

			Bool AddExpireTime(Data::DateTime *dt)
			{
				NotNullPtr<Data::DateTime> nnDt;
				if (!nnDt.Set(dt))
				{
					return this->AddHeader(CSTR("Expires"), CSTR("0"));
				}
				else
				{
					return this->AddTimeHeader(CSTR("Expires"), nnDt);
				}
			}

			Bool AddLastModified(NotNullPtr<Data::DateTime> dt)
			{
				return this->AddTimeHeader(CSTR("Last-Modified"), dt);
			}

			Bool AddLastModified(const Data::Timestamp &ts)
			{
				return this->AddTimeHeader(CSTR("Last-Modified"), ts);
			}

			Bool AddServer(Text::CStringNN server)
			{
				return this->AddHeader(CSTR("Server"), server);
			}

			Bool AddAccessControlAllowOrigin(Text::CStringNN origin)
			{
				return this->AddHeader(CSTR("Access-Control-Allow-Origin"), origin);
			}

			Bool AddSetCookie(Text::CString name, Text::CString value, Text::CString path, Bool httpOnly, Bool secure, SameSiteType sameSite, Data::Timestamp expires)
			{
				Text::StringBuilderUTF8 sb;
				sb.Append(name);
				sb.AppendUTF8Char('=');
				sb.Append(value);
				if (path.leng > 0)
				{
					sb.AppendC(UTF8STRC("; Path="));
					sb.Append(path);
				}
				if (secure)
				{
					sb.AppendC(UTF8STRC("; Secure"));
				}
				if (httpOnly)
				{
					sb.AppendC(UTF8STRC("; HttpOnly"));
				}
				switch (sameSite)
				{
				case SameSiteType::None:
					sb.AppendC(UTF8STRC("; SameSite=None"));
					break;
				case SameSiteType::Lax:
					sb.AppendC(UTF8STRC("; SameSite=Lax"));
					break;
				case SameSiteType::Strict:
					sb.AppendC(UTF8STRC("; SameSite=Strict"));
					break;
				case SameSiteType::Unspecified:
				default:
					break;
				}
				if (!expires.IsNull())
				{
					sb.AppendC(UTF8STRC("; Expires="));
					Net::WebUtil::Date2Str(sb, expires);
				}
				return this->AddHeader(CSTR("Set-Cookie"), sb.ToCString());
			}
		};
	}
}
#endif
