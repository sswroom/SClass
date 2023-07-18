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
		NotNullPtr<Net::SocketFactory> sockf;
		Text::String *prefix;

	public:
		WhoisClient(NotNullPtr<Net::SocketFactory> sockf, UInt32 whoisIP, Text::CString prefix);
		virtual ~WhoisClient();

		virtual WhoisRecord *RequestIP(UInt32 ip, Data::Duration timeout);

	protected:
		WhoisRecord *RequestIP(UInt32 ip, UInt32 whoisIP, Text::CString prefix, Data::Duration timeout);
	};
};
#endif
