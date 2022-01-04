#ifndef _SM_NET_SSLENGINEFACTORY
#define _SM_NET_SSLENGINEFACTORY
#include "Net/SSLEngine.h"

namespace Net
{
	class SSLEngineFactory
	{
	public:
		static Net::SSLEngine *Create(Net::SocketFactory *sockf, Bool skipCertCheck);
	};
}
#endif
