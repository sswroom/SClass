#ifndef _SM_NET_FAILOVERCHANNEL
#define _SM_NET_FAILOVERCHANNEL

namespace Net
{
	class FailoverChannel
	{
	public:
		virtual ~FailoverChannel() {};

		virtual Bool ChannelFailure() = 0;
	};
}
#endif
