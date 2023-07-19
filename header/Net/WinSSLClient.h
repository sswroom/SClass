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
		WinSSLClient(NotNullPtr<Net::SocketFactory> sockf, Socket *s, void *ctxt);
		virtual ~WinSSLClient();

		virtual UOSInt Read(const Data::ByteArray &buff);
		virtual UOSInt Write(const UInt8 *buff, UOSInt size);

		virtual void *BeginRead(const Data::ByteArray &buff, Sync::Event *evt);
		virtual UOSInt EndRead(void *reqData, Bool toWait, Bool *incomplete);
		virtual void CancelRead(void *reqData);
		virtual void *BeginWrite(const UInt8 *buff, UOSInt size, Sync::Event *evt);
		virtual UOSInt EndWrite(void *reqData, Bool toWait);
		virtual void CancelWrite(void *reqData);

		virtual Int32 Flush();
		virtual void Close();
		virtual Bool Recover();

		virtual Crypto::Cert::Certificate *GetRemoteCert();
		virtual const Data::ReadingList<Crypto::Cert::Certificate *> *GetRemoteCerts();
	};
}
#endif
