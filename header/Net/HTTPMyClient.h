#ifndef _SM_NET_HTTPMYCLIENT
#define _SM_NET_HTTPMYCLIENT
#include "Data/ArrayList.h"
#include "Data/ArrayListString.h"
#include "Data/DateTime.h"
#include "IO/MemoryStream.h"
#include "IO/Stream.h"
#include "Manage/HiResClock.h"
#include "Net/HTTPClient.h"
#include "Net/SSLEngine.h"
#include "Net/TCPClient.h"
#include "Text/StringBuilderUTF8.h"

namespace Net
{
	class HTTPMyClient : public HTTPClient
	{
	private:
		struct ClassData;
		ClassData *clsData;
	protected:
		Net::SSLEngine *ssl;
		Net::TCPClient *cli;
		Text::String *cliHost;
		IO::MemoryStream *reqMstm;
		Text::String *userAgent;
		Data::ArrayListString *reqHeaders;

		Bool writing;

		UInt64 contRead;
		Int32 contEnc;
		UOSInt chunkSizeLeft;

		UInt8 *dataBuff;
		UOSInt buffSize;
		UOSInt buffOfst;
		Int32 timeOutMS;

	public:
		HTTPMyClient(Net::SocketFactory *sockf, Net::SSLEngine *ssl, const UTF8Char *userAgent, UOSInt uaLen, Bool kaConn);
		virtual ~HTTPMyClient();

		virtual Bool IsError();

		UOSInt ReadRAW(UInt8 *buff, UOSInt size);
		virtual UOSInt Read(UInt8 *buff, UOSInt size);
		virtual UOSInt Write(const UInt8 *buff, UOSInt size);

		virtual Int32 Flush();
		virtual void Close();
		virtual Bool Recover();

		virtual Bool Connect(const UTF8Char *url, UOSInt urlLen, Net::WebUtil::RequestMethod method, Double *timeDNS, Double *timeConn, Bool defHeaders);
		virtual void AddHeaderC(const UTF8Char *name, UOSInt nameLen, const UTF8Char *value, UOSInt valueLen);
		virtual void EndRequest(Double *timeReq, Double *timeResp);
		virtual void SetTimeout(Int32 ms);

		virtual Bool IsSecureConn();
		virtual Crypto::Cert::Certificate *GetServerCert();
	};
}
#endif
