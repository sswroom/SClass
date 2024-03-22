#ifndef _SM_NET_HTTPSERVERMONITOR
#define _SM_NET_HTTPSERVERMONITOR
#include "AnyType.h"
#include "Net/SSLEngine.h"
#include "Sync/Event.h"
#include "Text/String.h"

namespace Net
{
	class HTTPServerMonitor
	{
	private:
		NotNullPtr<Net::SocketFactory> sockf;
		Optional<Net::SSLEngine> ssl;
		NotNullPtr<Text::String> url;
		Bool currOnline;
		Bool threadRunning;
		Bool threadToStop;
		Sync::Event threadEvt;

		static UInt32 __stdcall ThreadProc(AnyType userObj);
		Bool CheckOnline();
	public:
		HTTPServerMonitor(NotNullPtr<Net::SocketFactory> sockf, Optional<Net::SSLEngine> ssl, NotNullPtr<Text::String> url);
		HTTPServerMonitor(NotNullPtr<Net::SocketFactory> sockf, Optional<Net::SSLEngine> ssl, Text::CString url);
		~HTTPServerMonitor();

		Bool IsOnline();
	};
}
#endif
