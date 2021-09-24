#ifndef _SM_NET_HTTPMYCLIENT
#define _SM_NET_HTTPMYCLIENT
#include "Data/ArrayList.h"
#include "Data/ArrayListStrUTF8.h"
#include "Data/DateTime.h"
#include "IO/MemoryStream.h"
#include "IO/Stream.h"
#include "Manage/HiResClock.h"
#include "Net/HTTPClient.h"
#include "Net/SSLEngine.h"
#include "Net/TCPClient.h"
#include "Text/StringBuilderUTF.h"
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
		const UTF8Char *cliHost;
		IO::MemoryStream *reqMstm;
		const UTF8Char *userAgent;
		Data::ArrayListStrUTF8 *reqHeaders;

		Bool writing;

		UInt64 contRead;
		Int32 contEnc;
		UOSInt chunkSizeLeft;

		UInt8 *dataBuff;
		UOSInt buffSize;
		UOSInt buffOfst;
		Int32 timeOutMS;

	public:
		HTTPMyClient(Net::SocketFactory *sockf, Net::SSLEngine *ssl, const UTF8Char *userAgent, Bool kaConn);
		virtual ~HTTPMyClient();

		virtual Bool IsError();

		virtual UOSInt Read(UInt8 *buff, UOSInt size);
		virtual UOSInt Write(const UInt8 *buff, UOSInt size);

		virtual Int32 Flush();
		virtual void Close();
		virtual Bool Recover();

		virtual Bool Connect(const UTF8Char *url, const Char *method, Double *timeDNS, Double *timeConn, Bool defHeaders);
		virtual void AddHeader(const UTF8Char *name, const UTF8Char *value);
		virtual void EndRequest(Double *timeReq, Double *timeResp);
		virtual void SetTimeout(Int32 ms);

		virtual Bool IsSecureConn();
		virtual Crypto::Cert::Certificate *GetServerCert();
	};
}
#endif
