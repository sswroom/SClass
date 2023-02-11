#ifndef _SM_NET_HTTPCLIENT
#define _SM_NET_HTTPCLIENT
#include "Crypto/Cert/Certificate.h"
#include "Data/ArrayList.h"
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
		Net::SocketFactory *sockf;
		Manage::HiResClock clk;

		Net::SocketUtil::AddressInfo svrAddr;
		Bool canWrite;
		Bool hasForm;
		Text::StringBuilderUTF8 *formSb;

		UInt64 contLeng;
		Net::WebStatus::StatusCode respStatus;
		Data::ArrayList<Text::String*> headers;
		UOSInt hdrLen;

		Bool kaConn;
		Text::String *url;
		UInt64 totalUpload;
		UInt64 totalDownload;

		HTTPClient(Net::SocketFactory *sockf, Bool kaConn);
	public:
		virtual ~HTTPClient();

		virtual Bool IsError() const = 0;

		virtual Bool IsDown() const;
		virtual Bool Connect(Text::CString url, Net::WebUtil::RequestMethod method, Double *timeDNS, Double *timeConn, Bool defHeaders) = 0;
		virtual void AddHeaderC(Text::CString name, Text::CString value) = 0;
		virtual void EndRequest(Double *timeReq, Double *timeResp) = 0;
		virtual void SetTimeout(Int32 ms) = 0;
		virtual IO::StreamType GetStreamType() const;

		virtual Bool IsSecureConn() = 0;
		virtual Bool SetClientCert(Crypto::Cert::X509Cert *cert, Crypto::Cert::X509File *key) = 0;
		virtual const Data::ReadingList<Crypto::Cert::Certificate *> *GetServerCerts() = 0;

		Bool FormBegin();
		Bool FormAdd(Text::CString name, Text::CString value);
		void AddTimeHeader(Text::CString name, Data::DateTime *dt);
		void AddContentType(Text::CString contType);
		void AddContentLength(UInt64 leng);

		UOSInt GetRespHeaderCnt();
		UTF8Char *GetRespHeader(UOSInt index, UTF8Char *buff);
		UTF8Char *GetRespHeader(Text::CString name, UTF8Char *valueBuff);
		Bool GetRespHeader(Text::CString name, Text::StringBuilderUTF8 *sb);
		Text::CString GetRespHeader(Text::CString name);
		Text::String *GetRespHeader(UOSInt index);
		UInt64 GetContentLength();
		UInt32 GetContentCodePage();
		Bool GetLastModified(Data::DateTime *dt);
		Bool GetServerDate(Data::DateTime *dt);
		Text::CString GetTransferEncoding();

		Text::String *GetURL();
		Net::WebStatus::StatusCode GetRespStatus();
		const Net::SocketUtil::AddressInfo *GetSvrAddr();
		Double GetTotalTime();
		UOSInt GetHdrLen();
		UInt64 GetTotalUpload();
		UInt64 GetTotalDownload();
		Bool ReadAllContent(IO::Stream *outStm, UOSInt buffSize, UInt64 maxSize);
		Bool ReadAllContent(Text::StringBuilderUTF8 *sb, UOSInt buffSize, UInt64 maxSize);

		static void ParseDateStr(Data::DateTime *dt, Text::CString dateStr);
		static Net::HTTPClient *CreateClient(Net::SocketFactory *sockf, Net::SSLEngine *ssl, Text::CString userAgent, Bool kaConn, Bool isSecure);
		static Net::HTTPClient *CreateConnect(Net::SocketFactory *sockf, Net::SSLEngine *ssl, Text::CString url, Net::WebUtil::RequestMethod method, Bool kaConn);
		static Bool IsHTTPURL(Text::CString url);
		static void PrepareSSL(Net::SSLEngine *ssl);
		static Bool LoadContent(Net::SocketFactory *sockf, Net::SSLEngine *ssl, Text::CString url, IO::Stream *stm, UInt64 maxSize);
		static Bool LoadContent(Net::SocketFactory *sockf, Net::SSLEngine *ssl, Text::CString url, Text::StringBuilderUTF8 *sb, UInt64 maxSize);
	};
}
#endif
