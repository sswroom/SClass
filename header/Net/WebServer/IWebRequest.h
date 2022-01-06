#ifndef _SM_NET_WEBSERVER_IWEBREQUEST
#define _SM_NET_WEBSERVER_IWEBREQUEST
#include "Data/ArrayList.h"
#include "Manage/OSInfo.h"
#include "Net/BrowserInfo.h"
#include "Net/SocketFactory.h"
#include "Text/StringBuilderUTF.h"

namespace Net
{
	namespace WebServer
	{
		class IWebRequest
		{
		public:
			enum class RequestMethod
			{
				HTTP_GET,
				HTTP_POST,
				HTTP_PUT,
				HTTP_PATCH,
				HTTP_DELETE,
				HTTP_CONNECT,
				RTSP_DESCRIBE,
				RTSP_ANNOUNCE,
				RTSP_GET_PARAMETER,
				RTSP_OPTIONS,
				RTSP_PAUSE,
				RTSP_PLAY,
				RTSP_RECORD,
				RTSP_REDIRECT,
				RTSP_SETUP,
				RTSP_SET_PARAMETER,
				RTSP_TEARDOWN
			};

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
			const UTF8Char *reqBrowserVer;
			Manage::OSInfo::OSType reqOS;
			const UTF8Char *reqOSVer;

			void ParseUserAgent();
		public:
			IWebRequest();
			virtual ~IWebRequest();

			virtual Text::String *GetSHeader(const UTF8Char *name) = 0;
			virtual UTF8Char *GetHeader(UTF8Char *sbuff, const UTF8Char *name, UOSInt buffLen) = 0;
			virtual Bool GetHeader(Text::StringBuilderUTF *sb, const UTF8Char *name) = 0;
			virtual UOSInt GetHeaderNames(Data::ArrayList<Text::String*> *names) = 0;
			Bool GetRefererDomain(Text::StringBuilderUTF *sb);
			Bool GetIfModifiedSince(Data::DateTime *dt);

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
			virtual RequestMethod GetReqMethod() = 0;
			virtual void ParseHTTPForm() = 0;
			virtual Text::String *GetHTTPFormStr(const UTF8Char *name, UOSInt nameLen) = 0;
			virtual const UInt8 *GetHTTPFormFile(const UTF8Char *formName, UOSInt index, UTF8Char *fileName, UOSInt fileNameBuffSize, UOSInt *fileSize) = 0;
			Bool GetHTTPFormInt16(const UTF8Char *name, UOSInt nameLen, Int16 *valOut);
			Bool GetHTTPFormUInt16(const UTF8Char *name, UOSInt nameLen, UInt16 *valOut);
			Bool GetHTTPFormInt32(const UTF8Char *name, UOSInt nameLen, Int32 *valOut);
			Bool GetHTTPFormUInt32(const UTF8Char *name, UOSInt nameLen, UInt32 *valOut);
			Bool GetHTTPFormInt64(const UTF8Char *name, UOSInt nameLen, Int64 *valOut);
			Bool GetHTTPFormUInt64(const UTF8Char *name, UOSInt nameLen, UInt64 *valOut);
			virtual void GetRequestURLBase(Text::StringBuilderUTF *sb) = 0;

			virtual const Net::SocketUtil::AddressInfo *GetClientAddr() = 0;
			virtual UInt16 GetClientPort() = 0;
			virtual Bool IsSecure() = 0;
			virtual const UInt8 *GetReqData(UOSInt *dataSize) = 0;

			const Char *GetReqMethodStr();
			Net::BrowserInfo::BrowserType GetBrowser();
			const UTF8Char *GetBrowserVer();
			Manage::OSInfo::OSType GetOS();
			const UTF8Char *GetOSVer();

			static const Char *RequestMethodGetName(RequestMethod reqMeth);
			static const Char *RequestProtocolGetName(RequestProtocol reqProto);
		};
	}
}
#endif
