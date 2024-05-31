#include "Stdafx.h"
#include "MyMemory.h"
#include "Net/WebServer/WebListener.h"
#include "Net/WebServer/WebConnection.h"
#include "Sync/Interlocked.h"
#include "Sync/MutexUsage.h"
#include "Text/MyString.h"
#include "Text/MyStringFloat.h"

void __stdcall Net::WebServer::WebListener::ClientReady(NN<Net::TCPClient> cli, AnyType userObj)
{
	NN<Net::WebServer::WebListener> me;
	if (userObj.GetOpt<Net::WebServer::WebListener>().SetTo(me))
	{
		UOSInt i = me->nextCli;
		NN<Net::TCPClientMgr> cliMgr;
		if (me->cliMgrs.GetItem(i).SetTo(cliMgr))
		{
			Net::WebServer::WebConnection *conn;
			NEW_CLASS(conn, Net::WebServer::WebConnection(me->sockf, me->ssl, cli, me, me->hdlr, me->allowProxy, me->keepAlive));
			conn->SetLogWriter(cliMgr->GetLogWriter());
			conn->SetSendLogger(OnDataSent, userObj);
			cliMgr->AddClient(cli, conn);
		}
		else
		{
			cli.Delete();
		}
		me->nextCli = (i + 1) % me->cliMgrs.GetCount();
	}
}

void __stdcall Net::WebServer::WebListener::ConnHdlr(NN<Socket> s, AnyType userObj)
{
	NN<Net::WebServer::WebListener> me = userObj.GetNN<Net::WebServer::WebListener>();
	NN<Net::TCPClient> cli;
	NN<Net::SSLEngine> ssl;
	if (me->ssl.SetTo(ssl))
	{
		ssl->ServerInit(s, ClientReady, me);
	}
	else
	{
		NEW_CLASSNN(cli, Net::TCPClient(me->sockf, s));
		ClientReady(cli, me);
	}
	Interlocked_IncrementU32(&me->status.connCnt);
}

void __stdcall Net::WebServer::WebListener::ClientEvent(NN<Net::TCPClient> cli, AnyType userObj, AnyType cliData, Net::TCPClientMgr::TCPEventType evtType)
{
//	Net::WebServer::WebListener *me = (Net::WebServer::WebListener*)userObj;
	if (evtType == Net::TCPClientMgr::TCP_EVENT_DISCONNECT)
	{
		NN<Net::WebServer::WebConnection> conn = cliData.GetNN<Net::WebServer::WebConnection>();
		conn.Delete();
	}
	else if (evtType == Net::TCPClientMgr::TCP_EVENT_SHUTDOWN)
	{
		NN<Net::WebServer::WebConnection> conn = cliData.GetNN<Net::WebServer::WebConnection>();
		conn->ProxyShutdown();
	}
}

void __stdcall Net::WebServer::WebListener::ClientData(NN<Net::TCPClient> cli, AnyType userObj, AnyType cliData, const Data::ByteArrayR &buff)
{
	NN<Net::WebServer::WebListener> me = userObj.GetNN<Net::WebServer::WebListener>();
	NN<Net::WebServer::WebConnection> conn = cliData.GetNN<Net::WebServer::WebConnection>();
	conn->ReceivedData(buff);
	Interlocked_AddU64(&me->status.totalRead, buff.GetSize());
}

void __stdcall Net::WebServer::WebListener::ClientTimeout(NN<Net::TCPClient> cli, AnyType userObj, AnyType cliData)
{
	NN<Net::WebServer::WebListener> me = userObj.GetNN<Net::WebServer::WebListener>();
	NN<Net::WebServer::WebConnection> conn = cliData.GetNN<Net::WebServer::WebConnection>();
	conn->ProcessTimeout();
	if (me->timeoutHdlr)
	{
		me->timeoutHdlr(me->timeoutObj, conn->GetRequestURL());
	}
}

void __stdcall Net::WebServer::WebListener::ProxyClientEvent(NN<Net::TCPClient> cli, AnyType userObj, AnyType cliData, Net::TCPClientMgr::TCPEventType evtType)
{
//	NN<Net::WebServer::WebListener> me = userObj.GetNN<Net::WebServer::WebListener>();
	if (evtType == Net::TCPClientMgr::TCP_EVENT_DISCONNECT)
	{
		NN<Net::WebServer::WebConnection> conn = cliData.GetNN<Net::WebServer::WebConnection>();
		conn->EndProxyConn();
	}
	else if (evtType == Net::TCPClientMgr::TCP_EVENT_SHUTDOWN)
	{
		NN<Net::WebServer::WebConnection> conn = cliData.GetNN<Net::WebServer::WebConnection>();
		conn->ShutdownSend();
	}
}

void __stdcall Net::WebServer::WebListener::ProxyClientData(NN<Net::TCPClient> cli, AnyType userObj, AnyType cliData, const Data::ByteArrayR &buff)
{
//	NN<Net::WebServer::WebListener> me = userObj.GetNN<Net::WebServer::WebListener>();
	NN<Net::WebServer::WebConnection> conn = cliData.GetNN<Net::WebServer::WebConnection>();
	conn->ProxyData(buff);
}

void __stdcall Net::WebServer::WebListener::ProxyTimeout(NN<Net::TCPClient> cli, AnyType userObj, AnyType cliData)
{

}

void __stdcall Net::WebServer::WebListener::OnDataSent(AnyType userObj, UOSInt buffSize)
{
	NN<Net::WebServer::WebListener> me = userObj.GetNN<Net::WebServer::WebListener>();
	Interlocked_AddU64(&me->status.totalWrite, buffSize);
}

Net::WebServer::WebListener::WebListener(NN<Net::SocketFactory> sockf, Optional<Net::SSLEngine> ssl, NN<IWebHandler> hdlr, UInt16 port, Int32 timeoutSeconds, UOSInt mgrCnt, UOSInt workerCnt, Text::CString svrName, Bool allowProxy, KeepAlive keepAlive, Bool autoStart)
{
	this->hdlr = hdlr;
	UOSInt i = mgrCnt;
	if (i == 0)
		i = 1;
	while (i-- > 0)
	{
		NN<Net::TCPClientMgr> mgr;
		NEW_CLASSNN(mgr, Net::TCPClientMgr(timeoutSeconds, ClientEvent, ClientData, this, workerCnt, ClientTimeout));
		this->cliMgrs.Add(mgr);
	}
	this->nextCli = 0;

	this->sockf = sockf;
	this->accLog = 0;
	this->reqLog = 0;
	this->ssl = ssl;
	NN<Net::SSLEngine> nnssl;
	if (this->ssl.SetTo(nnssl))
	{
		nnssl->ServerAddALPNSupport(CSTR("http/1.1"));
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
	NEW_CLASSNN(this->svr, Net::TCPServer(sockf, 0, port, this->log, ConnHdlr, this, CSTR("Web: "), autoStart));
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
		this->cliMgrs.GetItem(i).Delete();
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

NN<Text::String> Net::WebServer::WebListener::GetServerName() const
{
	return this->svrName;
}

UInt16 Net::WebServer::WebListener::GetListenPort()
{
	return this->svr->GetListenPort();
}

void Net::WebServer::WebListener::SetClientLog(Text::CStringNN logFile)
{
	Data::ArrayIterator<NN<Net::TCPClientMgr>> it = this->cliMgrs.Iterator(); 
	while (it.HasNext())
	{
		it.Next()->SetLogFile(logFile);
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

void Net::WebServer::WebListener::LogAccess(NN<Net::WebServer::IWebRequest> req, NN<Net::WebServer::IWebResponse> resp, Double time)
{
	UTF8Char sbuff[128];
	UnsafeArray<UTF8Char> sptr;
	Text::CStringNN cstr;
	Interlocked_IncrementU32(&this->status.reqCnt);
	Sync::MutexUsage accLogMutUsage(this->accLogMut);
	if (this->reqLog)
	{
		this->reqLog->LogRequest(req);
	}
	if (this->accLog)
	{
		Text::StringBuilderUTF8 sb;
		if (!Net::SocketUtil::GetAddrName(sbuff, req->GetClientAddr(), req->GetClientPort()).SetTo(sptr))
			sb.AppendC(UTF8STRC("?"));
		else
			sb.AppendC(sbuff, (UOSInt)(sptr - sbuff));
		sb.AppendC(UTF8STRC(" "));
		Text::CStringNN reqMeth = req->GetReqMethodStr();
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
		if (req->GetBrowserVer().SetTo(cstr) && cstr.leng > 0)
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

void Net::WebServer::WebListener::LogMessageC(Net::WebServer::IWebRequest *req, Text::CStringNN msg)
{
	UTF8Char sbuff[32];
	Sync::MutexUsage mutUsage(this->accLogMut);
	if (this->accLog)
	{
		if (req)
		{
			Text::StringBuilderUTF8 sb;
			UnsafeArray<UTF8Char> sptr = Net::SocketUtil::GetAddrName(sbuff, req->GetClientAddr(), req->GetClientPort()).Or(sbuff);
			sb.AppendC(sbuff, (UOSInt)(sptr - sbuff));
			sb.AppendC(UTF8STRC(" "));
			sb.Append(msg);
			this->accLog->LogMessage(sb.ToCString(), this->accLogLev);
		}
		else
		{
			this->accLog->LogMessage(msg, this->accLogLev);
		}
	}
}

void Net::WebServer::WebListener::AddProxyConn(Net::WebServer::WebConnection *conn, NN<Net::TCPClient> proxyCli)
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

void Net::WebServer::WebListener::HandleTimeout(TimeoutHandler hdlr, AnyType userObj)
{
	this->timeoutObj = userObj;
	this->timeoutHdlr = hdlr;
}

void Net::WebServer::WebListener::ExtendTimeout(NN<Net::TCPClient> cli)
{
	Data::ArrayIterator<NN<Net::TCPClientMgr>> it = this->cliMgrs.Iterator();
	while (it.HasNext())
	{
		it.Next()->ExtendTimeout(cli);
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
	Data::ArrayIterator<NN<Net::TCPClientMgr>> it = this->cliMgrs.Iterator();
	while (it.HasNext())
	{
		cnt += it.Next()->GetClientCount();
	}
	return cnt;
}
