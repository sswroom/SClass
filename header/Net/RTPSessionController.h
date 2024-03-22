#ifndef _SM_NET_RTPSESSIONCONTROLLER
#define _SM_NET_RTPSESSIONCONTROLLER

namespace Net
{
	class RTPSessionController
	{
	public:
		virtual void SessionKA(Int32 sessId) = 0;
	};
}
#endif
