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
		NN<Net::TCPClientFactory> clif;
		Optional<Net::SSLEngine> ssl;
		NN<Text::String> url;
		Bool currOnline;
		Bool threadRunning;
		Bool threadToStop;
		Sync::Event threadEvt;

		static UInt32 __stdcall ThreadProc(AnyType userObj);
		Bool CheckOnline();
	public:
		HTTPServerMonitor(NN<Net::TCPClientFactory> clif, Optional<Net::SSLEngine> ssl, NN<Text::String> url);
		HTTPServerMonitor(NN<Net::TCPClientFactory> clif, Optional<Net::SSLEngine> ssl, Text::CStringNN url);
		~HTTPServerMonitor();

		Bool IsOnline();
	};
}
#endif
