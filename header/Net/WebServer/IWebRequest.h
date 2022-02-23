#ifndef _SM_NET_WEBSERVER_IWEBREQUEST
#define _SM_NET_WEBSERVER_IWEBREQUEST
#include "Data/ArrayList.h"
#include "Manage/OSInfo.h"
#include "Net/BrowserInfo.h"
#include "Net/SocketFactory.h"
#include "Net/WebUtil.h"
#include "Text/CString.h"
#include "Text/String.h"
#include "Text/StringBuilderUTF8.h"

namespace Net
{
	namespace WebServer
	{
		class IWebRequest
		{
		public:
			enum class RequestProtocol
			{
				HTTP1_0,
				HTTP1_1,
				RTSP1_0,
				HTTP2_0
			};

			static UTF8Char PARAM_SEPERATOR;

		private:
			Bool uaParsed;
			Net::BrowserInfo::BrowserType reqBrowser;
			Text::CString reqBrowserVer;
			Manage::OSInfo::OSType reqOS;
			Text::CString reqOSVer;

			void ParseUserAgent();
		public:
			IWebRequest();
			virtual ~IWebRequest();

			virtual Text::String *GetSHeader(const UTF8Char *name, UOSInt nameLen) = 0;
			virtual UTF8Char *GetHeader(UTF8Char *sbuff, Text::CString name, UOSInt buffLen) = 0;
			virtual Bool GetHeaderC(Text::StringBuilderUTF8 *sb, const UTF8Char *name, UOSInt nameLen) = 0;
			virtual UOSInt GetHeaderNames(Data::ArrayList<Text::String*> *names) = 0;
			Bool GetRefererDomain(Text::StringBuilderUTF8 *sb);
			Bool GetIfModifiedSince(Data::DateTime *dt);
			Bool GetCookie(Text::CString name, Text::StringBuilderUTF8 *sb);
			Text::String *GetCookieAsNew(Text::CString name);

			virtual Text::String *GetRequestURI() = 0;
			UTF8Char *GetRequestPath(UTF8Char *sbuff, UOSInt maxLeng);
			UTF8Char *GetQueryString(UTF8Char *sbuff, UOSInt maxLeng);
			virtual RequestProtocol GetProtocol() = 0;
			virtual Text::String *GetQueryValue(const UTF8Char *name, UOSInt nameLen) = 0;
			UTF8Char *GetQueryValueStr(const UTF8Char *name, UOSInt nameLen, UTF8Char *buff, UOSInt buffSize);
			Bool GetQueryValueI16(const UTF8Char *name, UOSInt nameLen, Int16 *val);
			Bool GetQueryValueU16(const UTF8Char *name, UOSInt nameLen, UInt16 *val);
			Bool GetQueryValueI32(const UTF8Char *name, UOSInt nameLen, Int32 *val);
			Bool GetQueryValueU32(const UTF8Char *name, UOSInt nameLen, UInt32 *val);
			Bool GetQueryValueI64(const UTF8Char *name, UOSInt nameLen, Int64 *val);
			Bool GetQueryValueF64(const UTF8Char *name, UOSInt nameLen, Double *val);
			virtual Bool HasQuery(const UTF8Char *name, UOSInt nameLen) = 0;
			virtual Net::WebUtil::RequestMethod GetReqMethod() = 0;
			virtual void ParseHTTPForm() = 0;
			virtual Text::String *GetHTTPFormStr(const UTF8Char *name, UOSInt nameLen) = 0;
			virtual const UInt8 *GetHTTPFormFile(Text::CString formName, UOSInt index, UTF8Char *fileName, UOSInt fileNameBuffSize, UOSInt *fileSize) = 0;
			Bool GetHTTPFormInt16(const UTF8Char *name, UOSInt nameLen, Int16 *valOut);
			Bool GetHTTPFormUInt16(const UTF8Char *name, UOSInt nameLen, UInt16 *valOut);
			Bool GetHTTPFormInt32(const UTF8Char *name, UOSInt nameLen, Int32 *valOut);
			Bool GetHTTPFormUInt32(const UTF8Char *name, UOSInt nameLen, UInt32 *valOut);
			Bool GetHTTPFormInt64(const UTF8Char *name, UOSInt nameLen, Int64 *valOut);
			Bool GetHTTPFormUInt64(const UTF8Char *name, UOSInt nameLen, UInt64 *valOut);
			virtual void GetRequestURLBase(Text::StringBuilderUTF8 *sb) = 0;

			virtual const Net::SocketUtil::AddressInfo *GetClientAddr() = 0;
			virtual UInt16 GetClientPort() = 0;
			virtual Bool IsSecure() = 0;
			virtual const UInt8 *GetReqData(UOSInt *dataSize) = 0;

			Text::CString GetReqMethodStr();
			Net::BrowserInfo::BrowserType GetBrowser();
			Text::CString GetBrowserVer();
			Manage::OSInfo::OSType GetOS();
			Text::CString GetOSVer();

			static Text::CString RequestProtocolGetName(RequestProtocol reqProto);
		};
	}
}
#endif
