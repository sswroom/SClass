#ifndef _SM_SSWR_SERVERMONITOR_SERVERMONITORSESSION
#define _SM_SSWR_SERVERMONITOR_SERVERMONITORSESSION
#include "Net/WebServer/WebSession.h"
#include "Net/WebServer/WebSessionManager.h"
#include "SSWR/ServerMonitor/ServerMonitorData.h"

namespace SSWR
{
	namespace ServerMonitor
	{
		class ServerMonitorSession
		{
		private:
			Optional<Net::WebServer::WebSession> sess;

			public:
				ServerMonitorSession(Optional<Net::WebServer::WebSession> sess);
				~ServerMonitorSession();

				void EndUse();
				Bool IsValid() const;

				UserRole GetUserRole() const;
				void SetUserRole(UserRole userRole);

				static ServerMonitorSession CreateSession(NN<Net::WebServer::WebSessionManager> sessMgr, NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp, UserRole userRole);
				static Bool DeleteSession(NN<Net::WebServer::WebSession> sess);
		};
	}
}
#endif