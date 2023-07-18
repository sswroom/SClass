#ifndef _SM_NET_HTTPSERVERMONITOR
#define _SM_NET_HTTPSERVERMONITOR
#include "Net/SSLEngine.h"
#include "Sync/Event.h"
#include "Text/String.h"

namespace Net
{
	class HTTPServerMonitor
	{
	private:
		NotNullPtr<Net::SocketFactory> sockf;
		Net::SSLEngine *ssl;
		NotNullPtr<Text::String> url;
		Bool currOnline;
		Bool threadRunning;
		Bool threadToStop;
		Sync::Event threadEvt;

		static UInt32 __stdcall ThreadProc(void *userObj);
		Bool CheckOnline();
	public:
		HTTPServerMonitor(NotNullPtr<Net::SocketFactory> sockf, Net::SSLEngine *ssl, NotNullPtr<Text::String> url);
		HTTPServerMonitor(NotNullPtr<Net::SocketFactory> sockf, Net::SSLEngine *ssl, Text::CString url);
		~HTTPServerMonitor();

		Bool IsOnline();
	};
}
#endif
