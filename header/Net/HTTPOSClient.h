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
		UnsafeArrayOpt<const UTF8Char> cliHost;
		IO::MemoryStream *reqMstm;

		Bool writing;

		UInt64 contRead;

		UInt8 *dataBuff;
		UOSInt buffSize;

	public:
		HTTPOSClient(NN<Net::SocketFactory> sockf, Text::CString userAgent, Bool kaConn);
		virtual ~HTTPOSClient();

		virtual Bool IsError() const;

		virtual UOSInt Read(const Data::ByteArray &buff);
		virtual UOSInt Write(Data::ByteArrayR buff);

		virtual Int32 Flush();
		virtual void Close();
		virtual Bool Recover();

		virtual Bool Connect(Text::CStringNN url, Net::WebUtil::RequestMethod method, OptOut<Double> timeDNS, OptOut<Double> timeConn, Bool defHeaders);
		virtual void AddHeaderC(Text::CStringNN name, Text::CString value);
		virtual void EndRequest(OptOut<Double> timeReq, OptOut<Double> timeResp);
		virtual void SetTimeout(Data::Duration timeout);

		virtual Bool IsSecureConn() const;
		virtual Bool SetClientCert(NN<Crypto::Cert::X509Cert> cert, NN<Crypto::Cert::X509File> key);
		virtual Optional<const Data::ReadingListNN<Crypto::Cert::Certificate>> GetServerCerts();
	};
}
#endif
