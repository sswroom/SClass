#ifndef _SM_NET_DEFAULTSSLENGINE
#define _SM_NET_DEFAULTSSLENGINE
#include "Net/SSLEngine.h"

namespace Net
{
	class DefaultSSLEngine
	{
	public:
		static Net::SSLEngine *Create(Net::SocketFactory *sockf);
	};
}
#endif
