#ifndef _SM_NET_HTTPCLIENT
#define _SM_NET_HTTPCLIENT
#include "Crypto/Cert/Certificate.h"
#include "Data/ArrayListStringNN.h"
#include "Data/DateTime.h"
#include "IO/MemoryStream.h"
#include "IO/Stream.h"
#include "Manage/HiResClock.h"
#include "Net/SocketFactory.h"
#include "Net/SSLEngine.h"
#include "Net/TCPClientFactory.h"
#include "Net/WebStatus.h"
#include "Net/WebUtil.h"
#include "Text/CString.h"
#include "Text/StringBuilderUTF8.h"

namespace Net
{
	class HTTPClient : public IO::Stream
	{
	protected:
		NN<Net::TCPClientFactory> clif;
		Manage::HiResClock clk;

		Net::SocketUtil::AddressInfo svrAddr;
		Bool canWrite;
		Optional<Text::StringBuilderUTF8> sbForm;

		UInt64 contLeng;
		Net::WebStatus::StatusCode respStatus;
		Data::ArrayListStringNN headers;
		UOSInt hdrLen;

		Bool kaConn;
		NN<Text::String> url;
		Optional<Text::String> forceHost;
		UInt64 totalUpload;
		UInt64 totalDownload;

		HTTPClient(NN<Net::TCPClientFactory> clif, Bool kaConn);
	public:
		virtual ~HTTPClient();

		virtual Bool IsError() const = 0;

		virtual Bool IsDown() const;
		virtual Bool Connect(Text::CStringNN url, Net::WebUtil::RequestMethod method, OptOut<Double> timeDNS, OptOut<Double> timeConn, Bool defHeaders) = 0;
		virtual void AddHeaderC(Text::CStringNN name, Text::CString value) = 0;
		virtual void EndRequest(OptOut<Double> timeReq, OptOut<Double> timeResp) = 0;
		virtual void SetTimeout(Data::Duration timeout) = 0;
		virtual IO::StreamType GetStreamType() const;

		virtual Bool IsSecureConn() const = 0;
		virtual Bool SetClientCert(NN<Crypto::Cert::X509Cert> cert, NN<Crypto::Cert::X509File> key) = 0;
		virtual Optional<const Data::ReadingListNN<Crypto::Cert::Certificate>> GetServerCerts() = 0;

		Bool FormBegin();
		Bool FormAdd(Text::CStringNN name, Text::CString value);
		void AddTimeHeader(Text::CStringNN name, NN<Data::DateTime> dt);
		void AddTimeHeader(Text::CStringNN name, Data::Timestamp ts);
		void AddContentType(Text::CStringNN contType);
		void AddContentLength(UInt64 leng);
		Bool WriteContent(Text::CStringNN contType, Text::CStringNN content);

		void ForceHostName(Text::CStringNN hostName);
		UOSInt GetRespHeaderCnt() const;
		UnsafeArrayOpt<UTF8Char> GetRespHeader(UOSInt index, UnsafeArray<UTF8Char> buff);
		UnsafeArrayOpt<UTF8Char> GetRespHeader(Text::CStringNN name, UnsafeArray<UTF8Char> valueBuff);
		Bool GetRespHeader(Text::CStringNN name, NN<Text::StringBuilderUTF8> sb);
		Text::CString GetRespHeader(Text::CStringNN name);
		Optional<Text::String> GetRespHeader(UOSInt index) const;
		Data::ArrayIterator<NN<Text::String>> RespHeaderIterator() const;
		UInt64 GetContentLength();
		UInt32 GetContentCodePage();
		void GetContentFileName(NN<Text::StringBuilderUTF8> sb);
		Bool GetLastModified(NN<Data::DateTime> dt);
		Bool GetLastModified(OutParam<Data::Timestamp> ts);
		Bool GetServerDate(NN<Data::DateTime> dt);
		Text::CString GetTransferEncoding();
		Text::CString GetContentType();

		Optional<Text::String> GetURL();
		Net::WebStatus::StatusCode GetRespStatus();
		NN<const Net::SocketUtil::AddressInfo> GetSvrAddr();
		Double GetTotalTime();
		UOSInt GetHdrLen();
		UInt64 GetTotalUpload();
		UInt64 GetTotalDownload();
		Bool ReadAllContent(NN<IO::Stream> outStm, UOSInt buffSize, UInt64 maxSize);
		Bool ReadAllContent(NN<Text::StringBuilderUTF8> sb, UOSInt buffSize, UInt64 maxSize);

		static void ParseDateStr(NN<Data::DateTime> dt, Text::CStringNN dateStr);
		static Data::Timestamp ParseDateStr(Text::CStringNN dateStr);
		static NN<Net::HTTPClient> CreateClient(NN<Net::TCPClientFactory> clif, Optional<Net::SSLEngine> ssl, Text::CString userAgent, Bool kaConn, Bool isSecure);
		static NN<Net::HTTPClient> CreateConnect(NN<Net::TCPClientFactory> clif, Optional<Net::SSLEngine> ssl, Text::CStringNN url, Net::WebUtil::RequestMethod method, Bool kaConn);
		static NN<Net::HTTPClient> CreateGet(NN<Net::TCPClientFactory> clif, Optional<Net::SSLEngine> ssl, Text::CStringNN url, Bool kaConn);
		static Bool IsHTTPURL(Text::CStringNN url);
		static void PrepareSSL(Optional<Net::SSLEngine> ssl);
		static Bool LoadContent(NN<Net::TCPClientFactory> clif, Optional<Net::SSLEngine> ssl, Text::CStringNN url, NN<IO::Stream> stm, UInt64 maxSize);
		static Bool LoadContent(NN<Net::TCPClientFactory> clif, Optional<Net::SSLEngine> ssl, Text::CStringNN url, NN<Text::StringBuilderUTF8> sb, UInt64 maxSize);
	};
}
#endif
