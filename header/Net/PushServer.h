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
		NotNullPtr<Net::SocketFactory> sockf;
		Net::WebServer::WebListener *listener;
		NotNullPtr<PushServerHandler> webHdlr;
		PushManager mgr;

	public:
		PushServer(NotNullPtr<Net::SocketFactory> sockf, Net::SSLEngine *ssl, UInt16 port, Text::CString fcmKey, NotNullPtr<IO::LogTool> log);
		~PushServer();

		Bool IsError();
	};
}
#endif
