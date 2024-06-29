#ifndef _SM_NET_WINSSLCLIENT
#define _SM_NET_WINSSLCLIENT
#include "Net/SSLClient.h"

namespace Net
{
	class WinSSLClient : public Net::SSLClient
	{
	protected:
		struct ClassData;
		struct ClassData *clsData;

	public:
		WinSSLClient(NN<Net::SocketFactory> sockf, NN<Socket> s, void *ctxt);
		virtual ~WinSSLClient();

		virtual UOSInt Read(const Data::ByteArray &buff);
		virtual UOSInt Write(Data::ByteArrayR buff);

		virtual Optional<IO::StreamReadReq> BeginRead(const Data::ByteArray &buff, NN<Sync::Event> evt);
		virtual UOSInt EndRead(NN<IO::StreamReadReq> reqData, Bool toWait, OutParam<Bool> incomplete);
		virtual void CancelRead(NN<IO::StreamReadReq> reqData);
		virtual Optional<IO::StreamWriteReq> BeginWrite(Data::ByteArrayR buff, NN<Sync::Event> evt);
		virtual UOSInt EndWrite(NN<IO::StreamWriteReq> reqData, Bool toWait);
		virtual void CancelWrite(NN<IO::StreamWriteReq> reqData);

		virtual Int32 Flush();
		virtual void Close();
		virtual Bool Recover();

		virtual Optional<Crypto::Cert::Certificate> GetRemoteCert();
		virtual Optional<const Data::ReadingListNN<Crypto::Cert::Certificate>> GetRemoteCerts();
	};
}
#endif
