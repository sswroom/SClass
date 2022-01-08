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
#include "Text/StringBuilderUTF.h"
#include "Text/StringBuilderUTF8.h"

namespace Net
{
	class HTTPClient : public IO::Stream
	{
	protected:
		Net::SocketFactory *sockf;
		Manage::HiResClock *clk;

		Net::SocketUtil::AddressInfo svrAddr;
		Bool canWrite;
		Bool hasForm;
		Text::StringBuilderUTF8 *formSb;

		UInt64 contLeng;
		Net::WebStatus::StatusCode respStatus;
		Data::ArrayList<Text::String*> *headers;

		Bool kaConn;
		const UTF8Char *url;

		HTTPClient(Net::SocketFactory *sockf, Bool kaConn);
	public:
		virtual ~HTTPClient();

		virtual Bool IsError() = 0;

		virtual Bool Connect(const UTF8Char *url, const Char *method, Double *timeDNS, Double *timeConn, Bool defHeaders) = 0;
		virtual void AddHeaderC(const UTF8Char *name, UOSInt nameLen, const UTF8Char *value, UOSInt valueLen) = 0;
		virtual void EndRequest(Double *timeReq, Double *timeResp) = 0;
		virtual void SetTimeout(Int32 ms) = 0;

		virtual Bool IsSecureConn() = 0;
		virtual Crypto::Cert::Certificate *GetServerCert() = 0;

		Bool FormBegin();
		Bool FormAdd(const UTF8Char *name, const UTF8Char *value);
		void AddTimeHeader(const UTF8Char *name, UOSInt nameLen, Data::DateTime *dt);
		void AddContentType(const UTF8Char *contType, UOSInt len);
		void AddContentLength(UOSInt leng);

		UOSInt GetRespHeaderCnt();
		UTF8Char *GetRespHeader(UOSInt index, UTF8Char *buff);
		UTF8Char *GetRespHeader(const UTF8Char *name, UTF8Char *valueBuff);
		Bool GetRespHeader(const UTF8Char *name, Text::StringBuilderUTF *sb);
		Text::String *GetRespHeader(UOSInt index);
		UInt64 GetContentLength();
		UInt32 GetContentCodePage();
		Bool GetLastModified(Data::DateTime *dt);

		const UTF8Char *GetURL();
		Net::WebStatus::StatusCode GetRespStatus();
		const Net::SocketUtil::AddressInfo *GetSvrAddr();
		Double GetTotalTime();

		static void ParseDateStr(Data::DateTime *dt, const UTF8Char *dateStr);
		static UTF8Char *Date2Str(UTF8Char *sbuff, Data::DateTime *dt);
		static Net::HTTPClient *CreateClient(Net::SocketFactory *sockf, Net::SSLEngine *ssl, const UTF8Char *userAgent, Bool kaConn, Bool isSecure);
		static Net::HTTPClient *CreateConnect(Net::SocketFactory *sockf, Net::SSLEngine *ssl, const UTF8Char *url, const Char *method, Bool kaConn);
		static Bool IsHTTPURL(const UTF8Char *url);
		static void PrepareSSL(Net::SSLEngine *ssl);
	};
}
#endif
