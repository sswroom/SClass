#include "Stdafx.h"
#include "SSWR/ServerMonitor/ServerMonitorClient.h"
#include "SSWR/ServerMonitor/URLMonitorClient.h"

Optional<SSWR::ServerMonitor::ServerMonitorClient> SSWR::ServerMonitor::ServerMonitorClient::CreateClient(ServerType serverType, NN<Net::TCPClientFactory> clif, Optional<Net::SSLEngine> ssl, Text::CStringNN target, Int32 timeoutMS)
{
	switch (serverType)
	{
	case ServerType::URL:
		{
			NN<URLMonitorClient> client;
			NEW_CLASSNN(client, URLMonitorClient(clif, ssl, target, timeoutMS));
			if (client->HasError())
			{
				client.Delete();
				return nullptr;
			}
			else
			{
				return client;
			}
		}
	case ServerType::TCP:
	default:
		return nullptr;
	}
}