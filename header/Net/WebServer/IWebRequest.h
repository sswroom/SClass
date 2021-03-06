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
			typedef enum
			{
				REQMETH_HTTP_GET,
				REQMETH_HTTP_POST,
				REQMETH_HTTP_PUT,
				REQMETH_HTTP_PATCH,
				REQMETH_HTTP_DELETE,
				REQMETH_HTTP_CONNECT,
				REQMETH_RTSP_DESCRIBE,
				REQMETH_RTSP_ANNOUNCE,
				REQMETH_RTSP_GET_PARAMETER,
				REQMETH_RTSP_OPTIONS,
				REQMETH_RTSP_PAUSE,
				REQMETH_RTSP_PLAY,
				REQMETH_RTSP_RECORD,
				REQMETH_RTSP_REDIRECT,
				REQMETH_RTSP_SETUP,
				REQMETH_RTSP_SET_PARAMETER,
				REQMETH_RTSP_TEARDOWN
			} RequestMethod;

			typedef enum
			{
				REQPROTO_HTTP1_0,
				REQPROTO_HTTP1_1,
				REQPROTO_RTSP1_0
			} RequestProtocol;

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

			virtual UTF8Char *GetHeader(UTF8Char *sbuff, const UTF8Char *name, UOSInt buffLen) = 0;
			virtual Bool GetHeader(Text::StringBuilderUTF *sb, const UTF8Char *name) = 0;
			virtual UOSInt GetHeaderNames(Data::ArrayList<const UTF8Char*> *names) = 0;
			Bool GetRefererDomain(Text::StringBuilderUTF *sb);
			Bool GetIfModifiedSince(Data::DateTime *dt);

			virtual const UTF8Char *GetRequestURI() = 0;
			UTF8Char *GetRequestPath(UTF8Char *sbuff, UOSInt maxLeng);
			UTF8Char *GetQueryString(UTF8Char *sbuff, UOSInt maxLeng);
			virtual RequestProtocol GetProtocol() = 0;
			virtual const UTF8Char *GetQueryValue(const UTF8Char *name) = 0;
			UTF8Char *GetQueryValueStr(const UTF8Char *name, UTF8Char *buff, UOSInt buffSize);
			Bool GetQueryValueI16(const UTF8Char *name, Int16 *val);
			Bool GetQueryValueU16(const UTF8Char *name, UInt16 *val);
			Bool GetQueryValueI32(const UTF8Char *name, Int32 *val);
			Bool GetQueryValueU32(const UTF8Char *name, UInt32 *val);
			Bool GetQueryValueI64(const UTF8Char *name, Int64 *val);
			Bool GetQueryValueF64(const UTF8Char *name, Double *val);
			virtual Bool HasQuery(const UTF8Char *name) = 0;
			virtual RequestMethod GetReqMethod() = 0;
			virtual void ParseHTTPForm() = 0;
			virtual const UTF8Char *GetHTTPFormStr(const UTF8Char *name) = 0;
			virtual const UInt8 *GetHTTPFormFile(const UTF8Char *formName, UOSInt index, UTF8Char *fileName, UOSInt fileNameBuffSize, UOSInt *fileSize) = 0;
			Bool GetHTTPFormInt16(const UTF8Char *name, Int16 *valOut);
			Bool GetHTTPFormUInt16(const UTF8Char *name, UInt16 *valOut);
			Bool GetHTTPFormInt32(const UTF8Char *name, Int32 *valOut);
			Bool GetHTTPFormUInt32(const UTF8Char *name, UInt32 *valOut);
			Bool GetHTTPFormInt64(const UTF8Char *name, Int64 *valOut);
			Bool GetHTTPFormUInt64(const UTF8Char *name, UInt64 *valOut);
			virtual void GetRequestURLBase(Text::StringBuilderUTF *sb) = 0;

			virtual const Net::SocketUtil::AddressInfo *GetClientAddr() = 0;
			virtual UInt16 GetClientPort() = 0;
			virtual const UInt8 *GetReqData(UOSInt *dataSize) = 0;

			const UTF8Char *GetReqMethodStr();
			Net::BrowserInfo::BrowserType GetBrowser();
			const UTF8Char *GetBrowserVer();
			Manage::OSInfo::OSType GetOS();
			const UTF8Char *GetOSVer();

			static const UTF8Char *RequestMethodGetName(RequestMethod reqMeth);
		};
	}
}
#endif
