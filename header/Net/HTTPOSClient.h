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
		HTTPOSClient(Net::SocketFactory *sockf, Text::CString userAgent, Bool kaConn);
		virtual ~HTTPOSClient();

		virtual Bool IsError() const;

		virtual UOSInt Read(UInt8 *buff, UOSInt size);
		virtual UOSInt Write(const UInt8 *buff, UOSInt size);

		virtual Int32 Flush();
		virtual void Close();
		virtual Bool Recover();

		virtual Bool Connect(Text::CString url, Net::WebUtil::RequestMethod method, Double *timeDNS, Double *timeConn, Bool defHeaders);
		virtual void AddHeaderC(Text::CString name, Text::CString value);
		virtual void EndRequest(Double *timeReq, Double *timeResp);
		virtual void SetTimeout(Int32 ms);

		virtual Bool IsSecureConn();
		virtual const Data::ReadingList<Crypto::Cert::Certificate *> *GetServerCerts();
	};
}
#endif
