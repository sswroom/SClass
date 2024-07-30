#ifndef _SM_NET_URL
#define _SM_NET_URL
#include "IO/ParsedObject.h"
#include "Net/SSLEngine.h"
#include "Net/TCPClientFactory.h"
#include "Text/CString.h"

namespace Net
{
	class URL
	{
	public:
		static Optional<IO::ParsedObject> OpenObject(Text::CStringNN url, Text::CString userAgent, NN<Net::TCPClientFactory> clif, Optional<Net::SSLEngine> ssl, Data::Duration timeout, NN<IO::LogTool> log);
	};
}
#endif
