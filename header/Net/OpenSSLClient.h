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
		OpenSSLClient(NotNullPtr<Net::SocketFactory> sockf, void *ssl, Socket *s);
		virtual ~OpenSSLClient();

		virtual UOSInt Read(const Data::ByteArray &buff);
		virtual UOSInt Write(const UInt8 *buff, UOSInt size);

		virtual void *BeginRead(const Data::ByteArray &buff, Sync::Event *evt);
		virtual UOSInt EndRead(void *reqData, Bool toWait, OutParam<Bool> incomplete);
		virtual void CancelRead(void *reqData);
		virtual void *BeginWrite(const UInt8 *buff, UOSInt size, Sync::Event *evt);
		virtual UOSInt EndWrite(void *reqData, Bool toWait);
		virtual void CancelWrite(void *reqData);

		virtual Int32 Flush();
		virtual void Close();
		virtual Bool Recover();
		virtual void ShutdownSend();
		virtual Crypto::Cert::Certificate *GetRemoteCert();
		virtual const Data::ReadingList<Crypto::Cert::Certificate *> *GetRemoteCerts();
	};
}
#endif
