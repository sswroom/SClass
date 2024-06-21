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

		virtual void *BeginRead(const Data::ByteArray &buff, Sync::Event *evt);
		virtual UOSInt EndRead(void *reqData, Bool toWait, OutParam<Bool> incomplete);
		virtual void CancelRead(void *reqData);
		virtual void *BeginWrite(Data::ByteArrayR buff, Sync::Event *evt);
		virtual UOSInt EndWrite(void *reqData, Bool toWait);
		virtual void CancelWrite(void *reqData);

		virtual Int32 Flush();
		virtual void Close();
		virtual Bool Recover();

		virtual Optional<Crypto::Cert::Certificate> GetRemoteCert();
		virtual Optional<const Data::ReadingListNN<Crypto::Cert::Certificate>> GetRemoteCerts();
	};
}
#endif
