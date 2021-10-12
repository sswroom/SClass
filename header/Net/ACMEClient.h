#ifndef _SM_NET_ACMECLIENT
#define _SM_NET_ACMECLIENT
#include "Net/SocketFactory.h"
#include "Net/SSLEngine.h"

namespace Net
{
	class ACMEClient
	{
	private:
		Net::SocketFactory *sockf;
		Net::SSLEngine *ssl;
		const UTF8Char *serverHost;
		UInt16 port;
		const UTF8Char *urlNewNonce;
		const UTF8Char *urlNewAccount;
		const UTF8Char *urlNewOrder;
		const UTF8Char *urlNewAuthz;
		const UTF8Char *urlRevokeCert;
		const UTF8Char *urlKeyChange;
		const UTF8Char *urlTermOfService;
		const UTF8Char *urlWebsite;

	public:
		ACMEClient(Net::SocketFactory *sockf, const UTF8Char *serverHost, UInt16 port);
		~ACMEClient();

		Bool IsError();
		const UTF8Char *GetTermOfService();
		const UTF8Char *GetWebsite();
	};
}
#endif
