#ifndef _SM_NET_WHOISCLIENT
#define _SM_NET_WHOISCLIENT
#include "Net/SocketFactory.h"
#include "Net/WhoisRecord.h"
#include "Text/String.h"

namespace Net
{
	class WhoisClient
	{
	protected:
		UInt32 whoisIP;
		NN<Net::SocketFactory> sockf;
		Optional<Text::String> prefix;

	public:
		WhoisClient(NN<Net::SocketFactory> sockf, UInt32 whoisIP, Text::CString prefix);
		virtual ~WhoisClient();

		virtual NN<WhoisRecord> RequestIP(UInt32 ip, Data::Duration timeout);

	protected:
		NN<WhoisRecord> RequestIP(UInt32 ip, UInt32 whoisIP, Text::CString prefix, Data::Duration timeout);
	};
};
#endif
