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
		Net::SocketFactory *sockf;
		Net::WebServer::WebListener *listener;
		PushServerHandler *webHdlr;
		PushManager mgr;

	public:
		PushServer(Net::SocketFactory *sockf, Net::SSLEngine *ssl, UInt16 port, Text::CString fcmKey, IO::LogTool *log);
		~PushServer();

		Bool IsError();
	};
}
#endif