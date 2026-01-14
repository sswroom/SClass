#ifndef _SM_NET_HTTPMYCLIENT
#define _SM_NET_HTTPMYCLIENT
#include "Data/ArrayListString.h"
#include "Data/DateTime.h"
#include "IO/MemoryStream.h"
#include "IO/Stream.h"
#include "Manage/HiResClock.h"
#include "Net/HTTPClient.h"
#include "Net/SSLEngine.h"
#include "Net/TCPClientFactory.h"
#include "Text/StringBuilderUTF8.h"

namespace Net
{
	class HTTPMyClient : public HTTPClient
	{
	private:
		struct ClassData;
		ClassData *clsData;
	protected:
		Optional<Net::SSLEngine> ssl;
		Optional<Net::TCPClient> cli;
		Optional<Text::String> cliHost;
		IO::MemoryStream reqMstm;
		NN<Text::String> userAgent;
		Data::ArrayListString reqHeaders;

		Bool writing;

		UInt64 contRead;
		Int32 contEnc;
		UOSInt chunkSizeLeft;

		UnsafeArray<UInt8> dataBuff;
		UOSInt buffSize;
		UOSInt buffOfst;
		Data::Duration timeout;

		UOSInt ReadRAWInternal(Data::ByteArray buff);
	public:
		HTTPMyClient(NN<Net::TCPClientFactory> clif, Optional<Net::SSLEngine> ssl, Text::CString userAgent, Bool kaConn);
		virtual ~HTTPMyClient();

		virtual Bool IsError() const;

		UOSInt ReadRAW(const Data::ByteArray &buff);
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
