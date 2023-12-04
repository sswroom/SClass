#ifndef _SM_NET_HTTPCLIENT
#define _SM_NET_HTTPCLIENT
#include "Crypto/Cert/Certificate.h"
#include "Data/ArrayList.h"
#include "Data/ArrayListNN.h"
#include "Data/DateTime.h"
#include "IO/MemoryStream.h"
#include "IO/Stream.h"
#include "Manage/HiResClock.h"
#include "Net/SocketFactory.h"
#include "Net/SSLEngine.h"
#include "Net/TCPClient.h"
#include "Net/WebStatus.h"
#include "Net/WebUtil.h"
#include "Text/CString.h"
#include "Text/StringBuilderUTF8.h"

namespace Net
{
	class HTTPClient : public IO::Stream
	{
	protected:
		NotNullPtr<Net::SocketFactory> sockf;
		Manage::HiResClock clk;

		Net::SocketUtil::AddressInfo svrAddr;
		Bool canWrite;
		Bool hasForm;
		Text::StringBuilderUTF8 *formSb;

		UInt64 contLeng;
		Net::WebStatus::StatusCode respStatus;
		Data::ArrayListNN<Text::String> headers;
		UOSInt hdrLen;

		Bool kaConn;
		NotNullPtr<Text::String> url;
		UInt64 totalUpload;
		UInt64 totalDownload;

		HTTPClient(NotNullPtr<Net::SocketFactory> sockf, Bool kaConn);
	public:
		virtual ~HTTPClient();

		virtual Bool IsError() const = 0;

		virtual Bool IsDown() const;
		virtual Bool Connect(Text::CStringNN url, Net::WebUtil::RequestMethod method, Double *timeDNS, Double *timeConn, Bool defHeaders) = 0;
		virtual void AddHeaderC(Text::CStringNN name, Text::CString value) = 0;
		virtual void EndRequest(Double *timeReq, Double *timeResp) = 0;
		virtual void SetTimeout(Data::Duration timeout) = 0;
		virtual IO::StreamType GetStreamType() const;

		virtual Bool IsSecureConn() const = 0;
		virtual Bool SetClientCert(NotNullPtr<Crypto::Cert::X509Cert> cert, NotNullPtr<Crypto::Cert::X509File> key) = 0;
		virtual const Data::ReadingList<Crypto::Cert::Certificate *> *GetServerCerts() = 0;

		Bool FormBegin();
		Bool FormAdd(Text::CStringNN name, Text::CString value);
		void AddTimeHeader(Text::CStringNN name, NotNullPtr<Data::DateTime> dt);
		void AddTimeHeader(Text::CStringNN name, Data::Timestamp ts);
		void AddContentType(Text::CStringNN contType);
		void AddContentLength(UInt64 leng);
		Bool WriteContent(Text::CStringNN contType, Text::CStringNN content);

		UOSInt GetRespHeaderCnt();
		UTF8Char *GetRespHeader(UOSInt index, UTF8Char *buff);
		UTF8Char *GetRespHeader(Text::CStringNN name, UTF8Char *valueBuff);
		Bool GetRespHeader(Text::CStringNN name, NotNullPtr<Text::StringBuilderUTF8> sb);
		Text::CString GetRespHeader(Text::CStringNN name);
		Text::String *GetRespHeader(UOSInt index);
		UInt64 GetContentLength();
		UInt32 GetContentCodePage();
		Bool GetLastModified(NotNullPtr<Data::DateTime> dt);
		Bool GetLastModified(OutParam<Data::Timestamp> ts);
		Bool GetServerDate(NotNullPtr<Data::DateTime> dt);
		Text::CString GetTransferEncoding();
		Text::CString GetContentType();

		Text::String *GetURL();
		Net::WebStatus::StatusCode GetRespStatus();
		const Net::SocketUtil::AddressInfo *GetSvrAddr();
		Double GetTotalTime();
		UOSInt GetHdrLen();
		UInt64 GetTotalUpload();
		UInt64 GetTotalDownload();
		Bool ReadAllContent(NotNullPtr<IO::Stream> outStm, UOSInt buffSize, UInt64 maxSize);
		Bool ReadAllContent(NotNullPtr<Text::StringBuilderUTF8> sb, UOSInt buffSize, UInt64 maxSize);

		static void ParseDateStr(NotNullPtr<Data::DateTime> dt, Text::CStringNN dateStr);
		static Data::Timestamp ParseDateStr(Text::CStringNN dateStr);
		static NotNullPtr<Net::HTTPClient> CreateClient(NotNullPtr<Net::SocketFactory> sockf, Optional<Net::SSLEngine> ssl, Text::CString userAgent, Bool kaConn, Bool isSecure);
		static NotNullPtr<Net::HTTPClient> CreateConnect(NotNullPtr<Net::SocketFactory> sockf, Optional<Net::SSLEngine> ssl, Text::CStringNN url, Net::WebUtil::RequestMethod method, Bool kaConn);
		static Bool IsHTTPURL(Text::CStringNN url);
		static void PrepareSSL(Optional<Net::SSLEngine> ssl);
		static Bool LoadContent(NotNullPtr<Net::SocketFactory> sockf, Optional<Net::SSLEngine> ssl, Text::CStringNN url, NotNullPtr<IO::Stream> stm, UInt64 maxSize);
		static Bool LoadContent(NotNullPtr<Net::SocketFactory> sockf, Optional<Net::SSLEngine> ssl, Text::CStringNN url, NotNullPtr<Text::StringBuilderUTF8> sb, UInt64 maxSize);
	};
}
#endif
