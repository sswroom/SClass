#ifndef _SM_NET_HTTPOSCLIENT
#define _SM_NET_HTTPOSCLIENT
#include "Data/ArrayList.h"
#include "Data/DateTime.h"
#include "IO/MemoryStream.h"
#include "IO/Stream.h"
#include "Manage/HiResClock.h"
#include "Net/HTTPClient.h"
#include "Net/SocketFactory.h"
#include "Net/TCPClient.h"
#include "Text/StringBuilderUTF.h"
#include "Text/StringBuilderUTF8.h"

namespace Net
{
	class HTTPOSClient : public HTTPClient
	{
	public:
		struct ClassData;
	private:
		ClassData *clsData;
		const UTF8Char *cliHost;
		IO::MemoryStream *reqMstm;

		Bool writing;

		UInt64 contRead;

		UInt8 *dataBuff;
		UOSInt buffSize;

	public:
		HTTPOSClient(Net::SocketFactory *sockf, const UTF8Char *userAgent, Bool kaConn);
		virtual ~HTTPOSClient();

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
