#include "Stdafx.h"
#include "MyMemory.h"
#include "Net/WebServer/WebListener.h"
#include "Net/WebServer/WebConnection.h"
#include "Sync/Interlocked.h"
#include "Sync/MutexUsage.h"
#include "Text/MyString.h"
#include "Text/MyStringFloat.h"

void __stdcall Net::WebServer::WebListener::ClientReady(Net::TCPClient *cli, void *userObj)
{
	Net::WebServer::WebListener *me = (Net::WebServer::WebListener*)userObj;
	Net::WebServer::WebConnection *conn;
	NEW_CLASS(conn, Net::WebServer::WebConnection(me->sockf, me->ssl, cli, me, me->hdlr, me->allowProxy, me->keepAlive));
	conn->SetSendLogger(OnDataSent, me);
	me->cliMgr.AddClient(cli, conn);
}

void __stdcall Net::WebServer::WebListener::ConnHdlr(Socket *s, void *userObj)
{
	Net::WebServer::WebListener *me = (Net::WebServer::WebListener*)userObj;
	Net::TCPClient *cli;
	if (me->ssl)
	{
		me->ssl->ServerInit(s, ClientReady, me);
	}
	else
	{
		NEW_CLASS(cli, Net::TCPClient(me->sockf, s));
		ClientReady(cli, me);
	}
	Interlocked_IncrementU32(&me->status.connCnt);
}

void __stdcall Net::WebServer::WebListener::ClientEvent(Net::TCPClient *cli, void *userObj, void *cliData, Net::TCPClientMgr::TCPEventType evtType)
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

void __stdcall Net::WebServer::WebListener::ClientData(Net::TCPClient *cli, void *userObj, void *cliData, const UInt8 *buff, UOSInt size)
{
	Net::WebServer::WebListener *me = (Net::WebServer::WebListener*)userObj;
	Net::WebServer::WebConnection *conn = (Net::WebServer::WebConnection*)cliData;
	conn->ReceivedData(buff, size);
	Interlocked_AddU64(&me->status.totalRead, size);
}

void __stdcall Net::WebServer::WebListener::ClientTimeout(Net::TCPClient *cli, void *userObj, void *cliData)
{
	Net::WebServer::WebListener *me = (Net::WebServer::WebListener *)userObj;
	Net::WebServer::WebConnection *conn = (Net::WebServer::WebConnection*)cliData;
	conn->ProcessTimeout();
	if (me->timeoutHdlr)
	{
		me->timeoutHdlr(me->timeoutObj, conn->GetRequestURL());
	}
}

void __stdcall Net::WebServer::WebListener::ProxyClientEvent(Net::TCPClient *cli, void *userObj, void *cliData, Net::TCPClientMgr::TCPEventType evtType)
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

void __stdcall Net::WebServer::WebListener::ProxyClientData(Net::TCPClient *cli, void *userObj, void *cliData, const UInt8 *buff, UOSInt size)
{
//	Net::WebServer::WebListener *me = (Net::WebServer::WebListener*)userObj;
	Net::WebServer::WebConnection *conn = (Net::WebServer::WebConnection*)cliData;
	conn->ProxyData(buff, size);
}

void __stdcall Net::WebServer::WebListener::ProxyTimeout(Net::TCPClient *cli, void *userObj, void *cliData)
{

}

void __stdcall Net::WebServer::WebListener::OnDataSent(void *userObj, UOSInt buffSize)
{
	Net::WebServer::WebListener *me = (Net::WebServer::WebListener*)userObj;
	Interlocked_AddU64(&me->status.totalWrite, buffSize);
}

Net::WebServer::WebListener::WebListener(Net::SocketFactory *sockf, Net::SSLEngine *ssl, IWebHandler *hdlr, UInt16 port, Int32 timeoutSeconds, UOSInt workerCnt, Text::CString svrName, Bool allowProxy, KeepAlive keepAlive, Bool autoStart) : cliMgr(timeoutSeconds, ClientEvent, ClientData, this, workerCnt, ClientTimeout)
{
	this->hdlr = hdlr;

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
	NEW_CLASS(this->svr, Net::TCPServer(sockf, port, &this->log, ConnHdlr, this, CSTR("Web: "), autoStart));
	if (this->allowProxy)
	{
		NEW_CLASS(this->proxyCliMgr, Net::TCPClientMgr(240, ProxyClientEvent, ProxyClientData, this, workerCnt, ProxyTimeout));
	}
}

Net::WebServer::WebListener::~WebListener()
{
	DEL_CLASS(this->svr);
	this->cliMgr.CloseAll();
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

Text::String *Net::WebServer::WebListener::GetServerName()
{
	return this->svrName;
}

void Net::WebServer::WebListener::SetAccessLog(IO::LogTool *accLog, IO::ILogHandler::LogLevel accLogLev)
{
	Sync::MutexUsage mutUsage(&this->accLogMut);
	this->accLog = accLog;
	this->accLogLev = accLogLev;
}

void Net::WebServer::WebListener::SetRequestLog(Net::WebServer::IReqLogger *reqLog)
{
	Sync::MutexUsage mutUsage(&this->accLogMut);
	this->reqLog = reqLog;
}

void Net::WebServer::WebListener::LogAccess(Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp, Double time)
{
	UTF8Char sbuff[128];
	UTF8Char *sptr;
	Text::CString cstr;
	Interlocked_IncrementU32(&this->status.reqCnt);
	Sync::MutexUsage accLogMutUsage(&this->accLogMut);
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
			if (!req->GetHeaderC(&sb, CSTR("User-Agent")))
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
		Manage::OSInfo::GetCommonName(&sb, req->GetOS(), req->GetOSVer());
		sb.AppendC(UTF8STRC("\""));

		sb.AppendC(UTF8STRC(" "));
		sb.AppendI32(resp->GetStatusCode());
		sb.AppendC(UTF8STRC(" "));
		sb.AppendU64(resp->GetRespLength());
		sb.AppendC(UTF8STRC(" "));
		Text::SBAppendF64(&sb, time);

		this->accLog->LogMessage(sb.ToCString(), this->accLogLev);
	}
	accLogMutUsage.EndUse();
}

void Net::WebServer::WebListener::LogMessageC(Net::WebServer::IWebRequest *req, const UTF8Char *msg, UOSInt msgLen)
{
	UTF8Char sbuff[32];
	Sync::MutexUsage mutUsage(&this->accLogMut);
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

void Net::WebServer::WebListener::AddProxyConn(Net::WebServer::WebConnection *conn, Net::TCPClient *proxyCli)
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

void Net::WebServer::WebListener::ExtendTimeout(Net::TCPClient *cli)
{
	this->cliMgr.ExtendTimeout(cli);
}

void Net::WebServer::WebListener::GetStatus(SERVER_STATUS *status)
{
	MemCopyNO(status, &this->status, sizeof(SERVER_STATUS));
	status->currConn = (UInt32)this->cliMgr.GetClientCount();
}
