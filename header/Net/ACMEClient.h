#ifndef _SM_NET_ACMECLIENT
#define _SM_NET_ACMECLIENT
#include "Net/ACMEConn.h"

namespace Net
{
	class ACMEClient
	{
	private:
		Net::ACMEConn *acme;
		Bool keyReady;
		Bool accReady;

	public:
		ACMEClient(Net::SocketFactory *sockf, const UTF8Char *serverHost, UInt16 port, const UTF8Char *keyFile);
		~ACMEClient();

		Bool IsError();
		const UTF8Char *GetTermOfService();
		const UTF8Char *GetWebsite();
		const UTF8Char *GetAccountId();
		Net::ACMEConn *GetConn();
	};
}
#endif