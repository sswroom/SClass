#ifndef _SM_NET_WHOISGLOBALCLIENT
#define _SM_NET_WHOISGLOBALCLIENT
#include "Net/WhoisClient.h"

namespace Net
{
	class WhoisGlobalClient : public WhoisClient
	{
	public:
		WhoisGlobalClient(NN<Net::SocketFactory> sockf);
		virtual ~WhoisGlobalClient();

		virtual NN<WhoisRecord> RequestIP(UInt32 ip, Data::Duration timeout);
	};
}
#endif
