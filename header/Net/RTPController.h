#ifndef _SM_NET_RTPCONTROLLER
#define _SM_NET_RTPCONTROLLER

namespace Net
{
	class RTPCliChannel;

	class RTPController
	{
	public:
		virtual ~RTPController(){};

		virtual Bool Init(NN<Net::RTPCliChannel> rtpChannel) = 0;
		virtual Bool Play(NN<Net::RTPCliChannel> rtpChannel) = 0;
		virtual Bool KeepAlive(NN<Net::RTPCliChannel> rtpChannel) = 0;
		virtual Bool StopPlay(NN<Net::RTPCliChannel> rtpChannel) = 0;
		virtual Bool Deinit(NN<Net::RTPCliChannel> rtpChannel) = 0;

		virtual Net::RTPController *Clone() const = 0;
	};
}
#endif
