#include "Stdafx.h"
#include "MyMemory.h"
#include "Net/WebServer/WebListener.h"
#include "Net/WebServer/WebConnection.h"
#include "Sync/Interlocked.h"
#include "Sync/MutexUsage.h"
#include "Text/MyString.h"
#include "Text/MyStringFloat.h"

void __stdcall Net::WebServer::WebListener::ClientReady(NotNullPtr<Net::TCPClient> cli, void *userObj)
{
	NotNullPtr<Net::WebServer::WebListener> me;
	if (me.Set((Net::WebServer::WebListener*)userObj))
	{
		UOSInt i = me->nextCli;
		Net::WebServer::WebConnection *conn;
		NEW_CLASS(conn, Net::WebServer::WebConnection(me->sockf, me->ssl, cli, me, me->hdlr, me->allowProxy, me->keepAlive));
		conn->SetLogWriter(me->cliMgrs.GetItem(i)->GetLogWriter());
		conn->SetSendLogger(OnDataSent, userObj);
		me->cliMgrs.GetItem(i)->AddClient(cli, conn);
		me->nextCli = (i + 1) % me->cliMgrs.GetCount();
	}
}

void __stdcall Net::WebServer::WebListener::ConnHdlr(Socket *s, void *userObj)
{
	Net::WebServer::WebListener *me = (Net::WebServer::WebListener*)userObj;
	NotNullPtr<Net::TCPClient> cli;
	if (me->ssl)
	{
		me->ssl->ServerInit(s, ClientReady, me);
	}
	else
	{
		NEW_CLASSNN(cli, Net::TCPClient(me->sockf, s));
		ClientReady(cli, me);
	}
	Interlocked_IncrementU32(&me->status.connCnt);
}

void __stdcall Net::WebServer::WebListener::ClientEvent(NotNullPtr<Net::TCPClient> cli, void *userObj, void *cliData, Net::TCPClientMgr::TCPEventType evtType)
{
//	Net::WebServer::WebListener *me = (Net::WebServer::WebListener*)userObj;
	if (evtType == Net::TCPClientMgr::TCP_EVENT_DISCONNECT)
	{
		Net::WebServer::WebConnection *conn = (Net::WebServer::WebConnection*)cliData;
		DEL_CLASS(conn);
	}
	else if (evtType == Net::TCPClientMgr::TCP_EVENT_SHUTDOWN)
	{
		Net::WebServer::WebConnection *conn = (Net::WebServer::WebConnection*)cliData;
		conn->ProxyShutdown();
	}
}

void __stdcall Net::WebServer::WebListener::ClientData(NotNullPtr<Net::TCPClient> cli, void *userObj, void *cliData, const Data::ByteArrayR &buff)
{
	Net::WebServer::WebListener *me = (Net::WebServer::WebListener*)userObj;
	Net::WebServer::WebConnection *conn = (Net::WebServer::WebConnection*)cliData;
	conn->ReceivedData(buff);
	Interlocked_AddU64(&me->status.totalRead, buff.GetSize());
}

void __stdcall Net::WebServer::WebListener::ClientTimeout(NotNullPtr<Net::TCPClient> cli, void *userObj, void *cliData)
{
	Net::WebServer::WebListener *me = (Net::WebServer::WebListener *)userObj;
	Net::WebServer::WebConnection *conn = (Net::WebServer::WebConnection*)cliData;
	conn->ProcessTimeout();
	if (me->timeoutHdlr)
	{
		me->timeoutHdlr(me->timeoutObj, conn->GetRequestURL());
	}
}

void __stdcall Net::WebServer::WebListener::ProxyClientEvent(NotNullPtr<Net::TCPClient> cli, void *userObj, void *cliData, Net::TCPClientMgr::TCPEventType evtType)
{
//	Net::WebServer::WebListener *me = (Net::WebServer::WebListener*)userObj;
	if (evtType == Net::TCPClientMgr::TCP_EVENT_DISCONNECT)
	{
		Net::WebServer::WebConnection *conn = (Net::WebServer::WebConnection*)cliData;
		conn->EndProxyConn();
	}
	else if (evtType == Net::TCPClientMgr::TCP_EVENT_SHUTDOWN)
	{
		Net::WebServer::WebConnection *conn = (Net::WebServer::WebConnection*)cliData;
		conn->ShutdownSend();
	}
}

void __stdcall Net::WebServer::WebListener::ProxyClientData(NotNullPtr<Net::TCPClient> cli, void *userObj, void *cliData, const Data::ByteArrayR &buff)
{
//	Net::WebServer::WebListener *me = (Net::WebServer::WebListener*)userObj;
	Net::WebServer::WebConnection *conn = (Net::WebServer::WebConnection*)cliData;
	conn->ProxyData(buff);
}

void __stdcall Net::WebServer::WebListener::ProxyTimeout(NotNullPtr<Net::TCPClient> cli, void *userObj, void *cliData)
{

}

void __stdcall Net::WebServer::WebListener::OnDataSent(void *userObj, UOSInt buffSize)
{
	Net::WebServer::WebListener *me = (Net::WebServer::WebListener*)userObj;
	Interlocked_AddU64(&me->status.totalWrite, buffSize);
}

Net::WebServer::WebListener::WebListener(NotNullPtr<Net::SocketFactory> sockf, Net::SSLEngine *ssl, NotNullPtr<IWebHandler> hdlr, UInt16 port, Int32 timeoutSeconds, UOSInt mgrCnt, UOSInt workerCnt, Text::CString svrName, Bool allowProxy, KeepAlive keepAlive, Bool autoStart)
{
	this->hdlr = hdlr;
	UOSInt i = mgrCnt;
	if (i == 0)
		i = 1;
	while (i-- > 0)
	{
		NotNullPtr<Net::TCPClientMgr> mgr;
		NEW_CLASSNN(mgr, Net::TCPClientMgr(timeoutSeconds, ClientEvent, ClientData, this, workerCnt, ClientTimeout));
		this->cliMgrs.Add(mgr);
	}
	this->nextCli = 0;

	this->sockf = sockf;
	this->accLog = 0;
	this->reqLog = 0;
	this->ssl = ssl;
	if (this->ssl)
	{
		this->ssl->ServerAddALPNSupport(CSTR("http/1.1"));
	}
	this->allowProxy = allowProxy;
	this->keepAlive = keepAlive;
	this->timeoutHdlr = 0;
	this->timeoutObj = 0;
	this->proxyCliMgr = 0;
	if (svrName.leng > 0)
	{
		this->svrName = Text::String::New(svrName);
	}
	else
	{
		this->svrName = Text::String::New(UTF8STRC("sswr/1.0"));
	}
	this->status.connCnt = 0;
	this->status.currConn = 0;
	this->status.reqCnt = 0;
	this->status.totalRead = 0;
	this->status.totalWrite = 0;
	NEW_CLASSNN(this->svr, Net::TCPServer(sockf, port, this->log, ConnHdlr, this, CSTR("Web: "), autoStart));
	if (this->allowProxy)
	{
		NEW_CLASS(this->proxyCliMgr, Net::TCPClientMgr(240, ProxyClientEvent, ProxyClientData, this, workerCnt, ProxyTimeout));
	}
}

Net::WebServer::WebListener::~WebListener()
{
	this->svr.Delete();
	UOSInt i = this->cliMgrs.GetCount();
	while (i-- > 0)
	{
		Net::TCPClientMgr *mgr = this->cliMgrs.GetItem(i);
		DEL_CLASS(mgr);
	}
	SDEL_CLASS(this->proxyCliMgr);
	this->svrName->Release();
}

Bool Net::WebServer::WebListener::Start()
{
	return this->svr->Start();
}

Bool Net::WebServer::WebListener::IsError()
{
	return this->svr->IsV4Error();
}

NotNullPtr<Text::String> Net::WebServer::WebListener::GetServerName() const
{
	return this->svrName;
}

void Net::WebServer::WebListener::SetClientLog(Text::CStringNN logFile)
{
	UOSInt i = this->cliMgrs.GetCount();
	while (i-- > 0)
	{
		this->cliMgrs.GetItem(i)->SetLogFile(logFile);
	}
}

void Net::WebServer::WebListener::SetAccessLog(IO::LogTool *accLog, IO::LogHandler::LogLevel accLogLev)
{
	Sync::MutexUsage mutUsage(this->accLogMut);
	this->accLog = accLog;
	this->accLogLev = accLogLev;
}

void Net::WebServer::WebListener::SetRequestLog(Net::WebServer::IReqLogger *reqLog)
{
	Sync::MutexUsage mutUsage(this->accLogMut);
	this->reqLog = reqLog;
}

void Net::WebServer::WebListener::LogAccess(NotNullPtr<Net::WebServer::IWebRequest> req, NotNullPtr<Net::WebServer::IWebResponse> resp, Double time)
{
	UTF8Char sbuff[128];
	UTF8Char *sptr;
	Text::CString cstr;
	Interlocked_IncrementU32(&this->status.reqCnt);
	Sync::MutexUsage accLogMutUsage(this->accLogMut);
	if (this->reqLog)
	{
		this->reqLog->LogRequest(req);
	}
	if (this->accLog)
	{
		Text::StringBuilderUTF8 sb;
		sptr = Net::SocketUtil::GetAddrName(sbuff, req->GetClientAddr(), req->GetClientPort());
		if (sptr == 0)
			sb.AppendC(UTF8STRC("?"));
		else
			sb.AppendC(sbuff, (UOSInt)(sptr - sbuff));
		sb.AppendC(UTF8STRC(" "));
		Text::CString reqMeth = req->GetReqMethodStr();
		sb.AppendC(reqMeth.v, reqMeth.leng);
		sb.AppendC(UTF8STRC(" "));
		sb.Append(req->GetRequestURI());
		sb.AppendC(UTF8STRC(" \""));
		if (req->GetBrowser() == Net::BrowserInfo::BT_UNKNOWN)
		{
			if (!req->GetHeaderC(sb, CSTR("User-Agent")))
			{
				sb.AppendC(UTF8STRC("Unk Browser"));
			}
		}
		else
		{
			sb.Append(Net::BrowserInfo::GetName(req->GetBrowser()));
		}
		cstr = req->GetBrowserVer();
		if (cstr.leng > 0)
		{
			sb.AppendC(UTF8STRC(" "));
			sb.Append(cstr);
		}
		sb.AppendC(UTF8STRC("\""));

		sb.AppendC(UTF8STRC(" \""));
		Manage::OSInfo::GetCommonName(sb, req->GetOS(), req->GetOSVer());
		sb.AppendC(UTF8STRC("\""));

		sb.AppendC(UTF8STRC(" "));
		sb.AppendI32(resp->GetStatusCode());
		sb.AppendC(UTF8STRC(" "));
		sb.AppendU64(resp->GetRespLength());
		sb.AppendC(UTF8STRC(" "));
		sb.AppendDouble(time);

		this->accLog->LogMessage(sb.ToCString(), this->accLogLev);
	}
}

void Net::WebServer::WebListener::LogMessageC(Net::WebServer::IWebRequest *req, const UTF8Char *msg, UOSInt msgLen)
{
	UTF8Char sbuff[32];
	Sync::MutexUsage mutUsage(this->accLogMut);
	if (this->accLog)
	{
		if (req)
		{
			Text::StringBuilderUTF8 sb;
			UTF8Char *sptr = Net::SocketUtil::GetAddrName(sbuff, req->GetClientAddr(), req->GetClientPort());
			sb.AppendC(sbuff, (UOSInt)(sptr - sbuff));
			sb.AppendC(UTF8STRC(" "));
			sb.AppendC(msg, msgLen);
			this->accLog->LogMessage(sb.ToCString(), this->accLogLev);
		}
		else
		{
			this->accLog->LogMessage({msg, msgLen}, this->accLogLev);
		}
	}
}

void Net::WebServer::WebListener::AddProxyConn(Net::WebServer::WebConnection *conn, NotNullPtr<Net::TCPClient> proxyCli)
{
	if (this->proxyCliMgr)
	{
		this->proxyCliMgr->AddClient(proxyCli, conn);
	}
	else
	{
		conn->EndProxyConn();
	}
}

void Net::WebServer::WebListener::HandleTimeout(TimeoutHandler hdlr, void *userObj)
{
	this->timeoutObj = userObj;
	this->timeoutHdlr = hdlr;
}

void Net::WebServer::WebListener::ExtendTimeout(NotNullPtr<Net::TCPClient> cli)
{
	UOSInt i = this->cliMgrs.GetCount();
	while (i-- > 0)
	{
		this->cliMgrs.GetItem(i)->ExtendTimeout(cli);
	}
}

void Net::WebServer::WebListener::GetStatus(SERVER_STATUS *status)
{
	MemCopyNO(status, &this->status, sizeof(SERVER_STATUS));
	status->currConn = (UInt32)this->GetClientCount();
}

UOSInt Net::WebServer::WebListener::GetClientCount() const
{
	UOSInt cnt = 0;
	UOSInt i = this->cliMgrs.GetCount();
	while (i-- > 0)
	{
		cnt += this->cliMgrs.GetItem(i)->GetClientCount();
	}
	return cnt;
}
