#ifndef _SM_NET_IRTPCONTROLLER
#define _SM_NET_IRTPCONTROLLER

namespace Net
{
	class RTPCliChannel;

	class IRTPController
	{
	public:
		virtual ~IRTPController(){};

		virtual Bool Init(NN<Net::RTPCliChannel> rtpChannel) = 0;
		virtual Bool Play(NN<Net::RTPCliChannel> rtpChannel) = 0;
		virtual Bool KeepAlive(NN<Net::RTPCliChannel> rtpChannel) = 0;
		virtual Bool StopPlay(NN<Net::RTPCliChannel> rtpChannel) = 0;
		virtual Bool Deinit(NN<Net::RTPCliChannel> rtpChannel) = 0;

		virtual Net::IRTPController *Clone() const = 0;
	};
}
#endif
