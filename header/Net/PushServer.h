#ifndef _SM_NET_PUSHSERVER
#define _SM_NET_PUSHSERVER
#include "Net/PushManager.h"
#include "Net/Google/GoogleServiceAccount.h"
#include "Net/WebServer/WebListener.h"

namespace Net
{
	class PushServerHandler;

	class PushServer
	{
	private:
		NN<Net::TCPClientFactory> clif;
		Optional<Net::WebServer::WebListener> listener;
		NN<PushServerHandler> webHdlr;
		PushManager mgr;

	public:
		PushServer(NN<Net::TCPClientFactory> clif, Optional<Net::SSLEngine> ssl, UInt16 port, NN<Net::Google::GoogleServiceAccount> serviceAccount, NN<IO::LogTool> log);
		~PushServer();

		Bool IsError();
	};
}
#endif
