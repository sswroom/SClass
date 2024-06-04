#ifndef _SM_NET_PUSHSERVER
#define _SM_NET_PUSHSERVER
#include "Net/PushManager.h"
#include "Net/WebServer/WebListener.h"

namespace Net
{
	class PushServerHandler;

	class PushServer
	{
	private:
		NN<Net::SocketFactory> sockf;
		Net::WebServer::WebListener *listener;
		NN<PushServerHandler> webHdlr;
		PushManager mgr;

	public:
		PushServer(NN<Net::SocketFactory> sockf, Optional<Net::SSLEngine> ssl, UInt16 port, Text::CStringNN fcmKey, NN<IO::LogTool> log);
		~PushServer();

		Bool IsError();
	};
}
#endif
