#include "Stdafx.h"
#include "SSWR/ServerMonitor/ServerMonitorSession.h"

SSWR::ServerMonitor::ServerMonitorSession::ServerMonitorSession(Optional<Net::WebServer::WebSession> sess)
{
	this->sess = sess;
}

SSWR::ServerMonitor::ServerMonitorSession::~ServerMonitorSession()
{
	this->EndUse();
}

void SSWR::ServerMonitor::ServerMonitorSession::EndUse()
{
	NN<Net::WebServer::WebSession> sess;
	if (this->sess.SetTo(sess))
	{
		sess->EndUse();
		this->sess = nullptr;
	}
}

Bool SSWR::ServerMonitor::ServerMonitorSession::IsValid() const
{
	return this->GetUserRole() != UserRole::NotLogged;
}

SSWR::ServerMonitor::UserRole SSWR::ServerMonitor::ServerMonitorSession::GetUserRole() const
{
	NN<Net::WebServer::WebSession> sess;
	if (!this->sess.SetTo(sess))
	{
		return UserRole::NotLogged;
	}
	return (UserRole)sess->GetValueInt32(CSTR("UserRole"));
}

void SSWR::ServerMonitor::ServerMonitorSession::SetUserRole(UserRole userRole)
{
	NN<Net::WebServer::WebSession> sess;
	if (this->sess.SetTo(sess))
	{
		sess->SetValueInt32(CSTR("UserRole"), (Int32)userRole);
	}
}

SSWR::ServerMonitor::ServerMonitorSession SSWR::ServerMonitor::ServerMonitorSession::CreateSession(NN<Net::WebServer::WebSessionManager> sessMgr, NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp, UserRole userRole)
{
	ServerMonitorSession sess(sessMgr->CreateSession(req, resp));
	sess.SetUserRole(userRole);
	return sess;
}

Bool SSWR::ServerMonitor::ServerMonitorSession::DeleteSession(NN<Net::WebServer::WebSession> sess)
{
	return true;
}
