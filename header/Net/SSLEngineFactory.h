#ifndef _SM_NET_SSLENGINEFACTORY
#define _SM_NET_SSLENGINEFACTORY
#include "Net/SSLEngine.h"

namespace Net
{
	class SSLEngineFactory
	{
	public:
		static Optional<Net::SSLEngine> Create(NN<Net::TCPClientFactory> clif, Bool skipCertCheck);
	};
}
#endif
