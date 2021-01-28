#ifndef _SM_NET_IRTPCONTROLLER
#define _SM_NET_IRTPCONTROLLER

namespace Net
{
	class RTPCliChannel;

	class IRTPController
	{
	public:
		virtual ~IRTPController(){};

		virtual Bool Init(Net::RTPCliChannel *rtpChannel) = 0;
		virtual Bool Play(Net::RTPCliChannel *rtpChannel) = 0;
		virtual Bool KeepAlive(Net::RTPCliChannel *rtpChannel) = 0;
		virtual Bool StopPlay(Net::RTPCliChannel *rtpChannel) = 0;
		virtual Bool Deinit(Net::RTPCliChannel *rtpChannel) = 0;

		virtual Net::IRTPController *Clone() = 0;
	};
};
#endif
