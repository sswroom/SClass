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
		OpenSSLClient(Net::SocketFactory *sockf, void *ssl, UInt32 *s);
		virtual ~OpenSSLClient();

		virtual UOSInt Read(UInt8 *buff, UOSInt size);
		virtual UOSInt Write(const UInt8 *buff, UOSInt size);

		virtual void *BeginRead(UInt8 *buff, UOSInt size, Sync::Event *evt);
		virtual UOSInt EndRead(void *reqData, Bool toWait, Bool *incomplete);
		virtual void CancelRead(void *reqData);
		virtual void *BeginWrite(const UInt8 *buff, UOSInt size, Sync::Event *evt);
		virtual UOSInt EndWrite(void *reqData, Bool toWait);
		virtual void CancelWrite(void *reqData);

		virtual Int32 Flush();
		virtual void Close();
		virtual Bool Recover();
		virtual Crypto::Cert::Certificate *GetRemoteCert();
	};
}
#endif
