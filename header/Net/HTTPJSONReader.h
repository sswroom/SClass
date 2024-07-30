#ifndef _SM_NET_HTTPJSONREADER
#define _SM_NET_HTTPJSONREADER
#include "Net/SSLEngine.h"
#include "Text/JSON.h"

namespace Net
{
	class HTTPJSONReader
	{
	public:
		static Optional<Text::JSONBase> Read(NN<Net::TCPClientFactory> clif, Optional<Net::SSLEngine> ssl, Text::CStringNN url);
	};
}
#endif
