#ifndef _SM_NET_WEBSERVER_IWEBREQUEST
#define _SM_NET_WEBSERVER_IWEBREQUEST
#include "Crypto/Cert/X509Cert.h"
#include "Data/ArrayList.h"
#include "Manage/OSInfo.h"
#include "Net/BrowserInfo.h"
#include "Net/NetConnection.h"
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
			Text::CString reqDevModel;

			void ParseUserAgent();
		public:
			IWebRequest();
			virtual ~IWebRequest();

			virtual Optional<Text::String> GetSHeader(Text::CStringNN name) = 0;
			virtual UTF8Char *GetHeader(UTF8Char *sbuff, Text::CStringNN name, UOSInt buffLen) = 0;
			virtual Bool GetHeaderC(NotNullPtr<Text::StringBuilderUTF8> sb, Text::CStringNN name) = 0;
			virtual UOSInt GetHeaderNames(NotNullPtr<Data::ArrayListNN<Text::String>> names) = 0;
			Bool GetRefererDomain(NotNullPtr<Text::StringBuilderUTF8> sb);
			Bool GetIfModifiedSince(Data::DateTime *dt);
			Bool GetCookie(Text::CStringNN name, NotNullPtr<Text::StringBuilderUTF8> sb);
			Optional<Text::String> GetCookieAsNew(Text::CStringNN name);

			virtual NotNullPtr<Text::String> GetRequestURI() const = 0;
			UTF8Char *GetRequestPath(UTF8Char *sbuff, UOSInt maxLeng);
			UTF8Char *GetQueryString(UTF8Char *sbuff, UOSInt maxLeng);
			virtual RequestProtocol GetProtocol() const = 0;
			virtual Optional<Text::String> GetQueryValue(Text::CStringNN name) = 0;
			UTF8Char *GetQueryValueStr(Text::CStringNN name, UTF8Char *buff, UOSInt buffSize);
			Bool GetQueryValueI16(Text::CStringNN name, OutParam<Int16> val);
			Bool GetQueryValueU16(Text::CStringNN name, OutParam<UInt16> val);
			Bool GetQueryValueI32(Text::CStringNN name, OutParam<Int32> val);
			Bool GetQueryValueU32(Text::CStringNN name, OutParam<UInt32> val);
			Bool GetQueryValueI64(Text::CStringNN name, OutParam<Int64> val);
			Bool GetQueryValueF64(Text::CStringNN name, OutParam<Double> val);
			virtual Bool HasQuery(Text::CStringNN name) = 0;
			virtual Net::WebUtil::RequestMethod GetReqMethod() const = 0;
			virtual void ParseHTTPForm() = 0;
			virtual Optional<Text::String> GetHTTPFormStr(Text::CStringNN name) = 0;
			virtual const UInt8 *GetHTTPFormFile(Text::CStringNN formName, UOSInt index, UTF8Char *fileName, UOSInt fileNameBuffSize, OptOut<UTF8Char*> fileNameEnd, OptOut<UOSInt> fileSize) = 0;
			Bool GetHTTPFormInt16(Text::CStringNN name, OutParam<Int16> valOut);
			Bool GetHTTPFormUInt16(Text::CStringNN name, OutParam<UInt16> valOut);
			Bool GetHTTPFormInt32(Text::CStringNN name, OutParam<Int32> valOut);
			Bool GetHTTPFormUInt32(Text::CStringNN name, OutParam<UInt32> valOut);
			Bool GetHTTPFormInt64(Text::CStringNN name, OutParam<Int64> valOut);
			Bool GetHTTPFormUInt64(Text::CStringNN name, OutParam<UInt64> valOut);
			Bool GetHTTPFormDouble(Text::CStringNN name, OutParam<Double> valOut);
			virtual void GetRequestURLBase(NotNullPtr<Text::StringBuilderUTF8> sb) = 0;
			UTF8Char *BuildURLHost(UTF8Char *sbuff);

			virtual NotNullPtr<const Net::SocketUtil::AddressInfo> GetClientAddr() const = 0;
			virtual NotNullPtr<Net::NetConnection> GetNetConn() const = 0;
			virtual UInt16 GetClientPort() const = 0;
			virtual Bool IsSecure() const = 0;
			virtual Crypto::Cert::X509Cert *GetClientCert() = 0;
			virtual const UInt8 *GetReqData(OutParam<UOSInt> dataSize) = 0;

			Text::CString GetReqMethodStr() const { return Net::WebUtil::RequestMethodGetName(this->GetReqMethod()); }
			Net::BrowserInfo::BrowserType GetBrowser() { if (!this->uaParsed) this->ParseUserAgent(); return this->reqBrowser; }
			Text::CString GetBrowserVer() { if (!this->uaParsed) this->ParseUserAgent(); return this->reqBrowserVer; }
			Manage::OSInfo::OSType GetOS() { if (!this->uaParsed) this->ParseUserAgent(); return this->reqOS; }
			Text::CString GetOSVer() { if (!this->uaParsed) this->ParseUserAgent(); return this->reqOSVer; }
			Text::CString GetDevModel() { if (!this->uaParsed) this->ParseUserAgent(); return this->reqDevModel; }

			static Text::CString RequestProtocolGetName(RequestProtocol reqProto);
		};
	}
}
#endif
