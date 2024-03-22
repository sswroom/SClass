#ifndef _SM_NET_IRTPCONTROLLER
#define _SM_NET_IRTPCONTROLLER

namespace Net
{
	class RTPCliChannel;

	class IRTPController
	{
	public:
		virtual ~IRTPController(){};

		virtual Bool Init(NotNullPtr<Net::RTPCliChannel> rtpChannel) = 0;
		virtual Bool Play(NotNullPtr<Net::RTPCliChannel> rtpChannel) = 0;
		virtual Bool KeepAlive(NotNullPtr<Net::RTPCliChannel> rtpChannel) = 0;
		virtual Bool StopPlay(NotNullPtr<Net::RTPCliChannel> rtpChannel) = 0;
		virtual Bool Deinit(NotNullPtr<Net::RTPCliChannel> rtpChannel) = 0;

		virtual Net::IRTPController *Clone() const = 0;
	};
}
#endif
