#ifndef _SM_NET_URL
#define _SM_NET_URL
#include "IO/ParsedObject.h"
#include "Net/SocketFactory.h"
#include "Net/SSLEngine.h"
#include "Text/CString.h"

namespace Net
{
	class URL
	{
	public:
		static IO::ParsedObject *OpenObject(Text::CStringNN url, Text::CString userAgent, NotNullPtr<Net::SocketFactory> sockf, Optional<Net::SSLEngine> ssl, Data::Duration timeout, NotNullPtr<IO::LogTool> log);
	};
}
#endif
