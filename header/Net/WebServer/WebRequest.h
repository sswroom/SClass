#ifndef _SM_NET_WEBSERVER_WEBREQUEST
#define _SM_NET_WEBSERVER_WEBREQUEST
#include "Crypto/Cert/X509Cert.h"
#include "Data/ArrayList.hpp"
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
		class WebRequest
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
			WebRequest();
			virtual ~WebRequest();

			virtual Optional<Text::String> GetSHeader(Text::CStringNN name) const = 0;
			virtual UnsafeArrayOpt<UTF8Char> GetHeader(UnsafeArray<UTF8Char> sbuff, Text::CStringNN name, UOSInt buffLen) const = 0;
			virtual Bool GetHeaderC(NN<Text::StringBuilderUTF8> sb, Text::CStringNN name) const = 0;
			virtual UOSInt GetHeaderNames(NN<Data::ArrayListStringNN> names) const = 0;
			Bool GetRefererDomain(NN<Text::StringBuilderUTF8> sb) const;
			Bool GetOrigin(NN<Text::StringBuilderUTF8> sb) const;
			Bool GetIfModifiedSince(NN<Data::DateTime> dt) const;
			Bool GetCookie(Text::CStringNN name, NN<Text::StringBuilderUTF8> sb) const;
			Optional<Text::String> GetCookieAsNew(Text::CStringNN name) const;

			virtual NN<Text::String> GetRequestURI() const = 0;
			UnsafeArray<UTF8Char> GetRequestPath(UnsafeArray<UTF8Char> sbuff, UOSInt maxLeng);
			UnsafeArrayOpt<UTF8Char> GetQueryString(UnsafeArray<UTF8Char> sbuff, UOSInt maxLeng);
			virtual RequestProtocol GetProtocol() const = 0;
			virtual Optional<Text::String> GetQueryValue(Text::CStringNN name) = 0;
			UnsafeArrayOpt<UTF8Char> GetQueryValueStr(Text::CStringNN name, UnsafeArray<UTF8Char> buff, UOSInt buffSize);
			Bool GetQueryValueI16(Text::CStringNN name, OutParam<Int16> val);
			Bool GetQueryValueU16(Text::CStringNN name, OutParam<UInt16> val);
			Bool GetQueryValueI32(Text::CStringNN name, OutParam<Int32> val);
			Bool GetQueryValueU32(Text::CStringNN name, OutParam<UInt32> val);
			Bool GetQueryValueI64(Text::CStringNN name, OutParam<Int64> val);
			Bool GetQueryValueU64(Text::CStringNN name, OutParam<UInt64> val);
			Bool GetQueryValueF64(Text::CStringNN name, OutParam<Double> val);
			virtual Bool HasQuery(Text::CStringNN name) = 0;
			virtual Net::WebUtil::RequestMethod GetReqMethod() const = 0;
			virtual void ParseHTTPForm() = 0;
			virtual Optional<Text::String> GetHTTPFormStr(Text::CStringNN name) = 0;
			virtual UnsafeArrayOpt<const UInt8> GetHTTPFormFile(Text::CStringNN formName, UOSInt index, UnsafeArrayOpt<UTF8Char> fileName, UOSInt fileNameBuffSize, OptOut<UnsafeArray<UTF8Char>> fileNameEnd, OptOut<UOSInt> fileSize) = 0;
			Bool GetHTTPFormInt16(Text::CStringNN name, OutParam<Int16> valOut);
			Bool GetHTTPFormUInt16(Text::CStringNN name, OutParam<UInt16> valOut);
			Bool GetHTTPFormInt32(Text::CStringNN name, OutParam<Int32> valOut);
			Bool GetHTTPFormUInt32(Text::CStringNN name, OutParam<UInt32> valOut);
			Bool GetHTTPFormInt64(Text::CStringNN name, OutParam<Int64> valOut);
			Bool GetHTTPFormUInt64(Text::CStringNN name, OutParam<UInt64> valOut);
			Bool GetHTTPFormDouble(Text::CStringNN name, OutParam<Double> valOut);
			virtual void GetRequestURLBase(NN<Text::StringBuilderUTF8> sb) = 0;
			UnsafeArray<UTF8Char> BuildURLHost(UnsafeArray<UTF8Char> sbuff);

			virtual NN<const Net::SocketUtil::AddressInfo> GetClientAddr() const = 0;
			virtual NN<Net::NetConnection> GetNetConn() const = 0;
			virtual UInt16 GetClientPort() const = 0;
			virtual Bool IsSecure() const = 0;
			virtual Optional<Crypto::Cert::X509Cert> GetClientCert() = 0;
			virtual UnsafeArrayOpt<const UInt8> GetReqData(OutParam<UOSInt> dataSize) = 0;
			void GetRequestAddr(NN<Net::SocketUtil::AddressInfo> addr) const;

			Text::CStringNN GetReqMethodStr() const { return Net::WebUtil::RequestMethodGetName(this->GetReqMethod()); }
			Net::BrowserInfo::BrowserType GetBrowser() { if (!this->uaParsed) this->ParseUserAgent(); return this->reqBrowser; }
			Text::CString GetBrowserVer() { if (!this->uaParsed) this->ParseUserAgent(); return this->reqBrowserVer; }
			Manage::OSInfo::OSType GetOS() { if (!this->uaParsed) this->ParseUserAgent(); return this->reqOS; }
			Text::CString GetOSVer() { if (!this->uaParsed) this->ParseUserAgent(); return this->reqOSVer; }
			Text::CString GetDevModel() { if (!this->uaParsed) this->ParseUserAgent(); return this->reqDevModel; }

			static Text::CStringNN RequestProtocolGetName(RequestProtocol reqProto);
		};
	}
}
#endif
