#ifndef _SM_NET_WHOISCLIENT
#define _SM_NET_WHOISCLIENT
#include "Net/SocketFactory.h"
#include "Net/WhoisRecord.h"

namespace Net
{
	class WhoisClient
	{
	protected:
		UInt32 whoisIP;
		Net::SocketFactory *sockf;
		const Char *prefix;

	public:
		WhoisClient(Net::SocketFactory *sockf, UInt32 whoisIP, const Char *prefix);
		virtual ~WhoisClient();

		virtual WhoisRecord *RequestIP(UInt32 ip);

	protected:
		WhoisRecord *RequestIP(UInt32 ip, UInt32 whoisIP, const Char *prefix);
	};
};
#endif
