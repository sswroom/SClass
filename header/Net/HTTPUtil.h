#ifndef _SM_NET_HTTPUTIL
#define _SM_NET_HTTPUTIL
#include "Net/HTTPClient.h"
#include "Net/TCPClientFactory.h"

namespace Net
{
	class HTTPUtil
	{
	public:
		static Optional<IO::Stream> DownloadAndOpen(NN<Net::TCPClientFactory> clif, Optional<Net::SSLEngine> ssl, Text::CStringNN url, Text::CStringNN fileName, UInt64 maxSize);
		static Bool DownloadFile(NN<Net::TCPClientFactory> clif, Optional<Net::SSLEngine> ssl, Text::CStringNN url, Text::CStringNN fileName, UInt64 maxSize);
	};
}
#endif
