#ifndef _SM_NET_ACMECLIENT
#define _SM_NET_ACMECLIENT
#include "Net/ACMEConn.h"

namespace Net
{
	class ACMEClient
	{
	private:
		NN<Net::ACMEConn> acme;
		Bool keyReady;
		Bool accReady;

	public:
		ACMEClient(NN<Net::TCPClientFactory> clif, Text::CStringNN serverHost, UInt16 port, Text::CStringNN keyFile);
		~ACMEClient();

		Bool IsError();
		Text::String *GetTermOfService();
		Text::String *GetWebsite();
		Text::String *GetAccountId();
		NN<Net::ACMEConn> GetConn();
	};
}
#endif
