#ifndef _SM_NET_OPENSSLCLIENT
#define _SM_NET_OPENSSLCLIENT
#include "Net/SSLClient.h"

namespace Net
{
	class OpenSSLClient : public Net::SSLClient
	{
	protected:
		struct ClassData;
		struct ClassData *clsData;

		UInt32 GetLastErrorCode();
	public:
		OpenSSLClient(NN<Net::SocketFactory> sockf, void *ssl, NN<Socket> s);
		virtual ~OpenSSLClient();

		virtual UIntOS Read(const Data::ByteArray &buff);
		virtual UIntOS Write(Data::ByteArrayR buff);

		virtual Optional<IO::StreamReadReq> BeginRead(const Data::ByteArray &buff, NN<Sync::Event> evt);
		virtual UIntOS EndRead(NN<IO::StreamReadReq> reqData, Bool toWait, OutParam<Bool> incomplete);
		virtual void CancelRead(NN<IO::StreamReadReq> reqData);
		virtual Optional<IO::StreamWriteReq> BeginWrite(Data::ByteArrayR buff, NN<Sync::Event> evt);
		virtual UIntOS EndWrite(NN<IO::StreamWriteReq> reqData, Bool toWait);
		virtual void CancelWrite(NN<IO::StreamWriteReq> reqData);

		virtual Int32 Flush();
		virtual void Close();
		virtual Bool Recover();
		virtual void ShutdownSend();
		virtual Optional<Crypto::Cert::Certificate> GetRemoteCert();
		virtual Optional<const Data::ReadingListNN<Crypto::Cert::Certificate>> GetRemoteCerts();
	};
}
#endif
