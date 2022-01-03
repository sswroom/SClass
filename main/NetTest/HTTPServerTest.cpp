#include "Stdafx.h"
#include "Core/Core.h"
#include "IO/ConsoleWriter.h"
#include "Net/DefaultSSLEngine.h"
#include "Net/OSSocketFactory.h"
#include "Net/WebServer/HTTPDirectoryHandler.h"
#include "Net/WebServer/HTTPFormParser.h"
#include "Net/WebServer/WebListener.h"
#include "Net/WebServer/WebStandardHandler.h"
#include "Net/WebServer/WebServiceHandler.h"
#include "Text/StringBuilderUTF8.h"
#include "Text/XML.h"

IO::ConsoleWriter *console;

class MyHandler : public Net::WebServer::WebServiceHandler
{
private:
	Data::ArrayList<Net::WebServer::IWebResponse*> *sseConns;
	Sync::Mutex *sseMut;

	static void __stdcall SSEDisconnect(Net::WebServer::IWebResponse *resp, void *userObj)
	{
		MyHandler *me = (MyHandler*)userObj;
		Sync::MutexUsage mutUsage(me->sseMut);
		me->sseConns->Remove(resp);
		console->WriteLine((const UTF8Char*)"Disconnected");
	}

	static Bool __stdcall SSEHandler(Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp, const UTF8Char *subReq, WebServiceHandler *me)
	{
		MyHandler *myHdlr = (MyHandler*)me;
		resp->AddDefHeaders(req);
		if (resp->ResponseSSE(30000, SSEDisconnect, myHdlr))
		{
			Sync::MutexUsage mutUsage(myHdlr->sseMut);
			myHdlr->sseConns->Add(resp);
			
			resp->SSESend((const UTF8Char*)"INIT", 0);
			resp->SSESend((const UTF8Char*)"Test", (const UTF8Char*)"ABC");
			return true;
		}
		return false;
	}
public:
	MyHandler()
	{
		NEW_CLASS(this->sseConns, Data::ArrayList<Net::WebServer::IWebResponse*>());
		NEW_CLASS(this->sseMut, Sync::Mutex());
		this->AddService((const UTF8Char*)"/sse", Net::WebServer::IWebRequest::RequestMethod::HTTP_GET, SSEHandler);
	}

	virtual ~MyHandler()
	{
		DEL_CLASS(this->sseConns);
		DEL_CLASS(this->sseMut);
	}
};

Int32 MyMain(Core::IProgControl *progCtrl)
{
	Bool useSSL = false;
	Net::WebServer::WebListener *svr;
	Net::SocketFactory *sockf;
	Net::SSLEngine *ssl;
	Net::WebServer::WebStandardHandler *hdlr;
	Text::StringBuilderUTF8 sb;
	UInt16 port = 0;
	Bool succ = true;
	NEW_CLASS(console, IO::ConsoleWriter());

	UOSInt argc;
	UTF8Char **argv = progCtrl->GetCommandLines(progCtrl, &argc);
	if (argc >= 2)
	{
		if (!Text::StrToUInt16(argv[1], &port))
		{
			console->WriteLine((const UTF8Char*)"Error in parsing port number, use default port");
			port = 0;
		}
	}

	if (port == 0)
	{
		if (useSSL)
		{
			port = 443;
		}
		else
		{
			port = 80;
		}
	}

	NEW_CLASS(sockf, Net::OSSocketFactory(true));
	if (useSSL)
	{
		ssl = Net::DefaultSSLEngine::Create(sockf, true);
		if (ssl == 0 || !ssl->SetServerCerts((const UTF8Char*)"test.crt", (const UTF8Char*)"test.key"))
		{
			UTF8Char sbuff[512];
			console->WriteLine((const UTF8Char*)"Error in initializing SSL");
			ssl->GetErrorDetail(sbuff);
			console->WriteLine(sbuff);
			succ = false;
		}
	}
	else
	{
		ssl = 0;
	}

	if (succ)
	{
		sb.ClearStr();
		sb.AppendC(UTF8STRC("Listening to port "));
		sb.AppendU16(port);
		console->WriteLine(sb.ToString());
		MyHandler *myHdlr;
		NEW_CLASS(hdlr, Net::WebServer::HTTPDirectoryHandler((const UTF8Char*)".", true, 0, true));
		NEW_CLASS(myHdlr, MyHandler());
		hdlr->HandlePath((const UTF8Char*)"/api", myHdlr, true);
		NEW_CLASS(svr, Net::WebServer::WebListener(sockf, ssl, hdlr, port, 120, 4, (const UTF8Char*)"sswr/1.0", false, true));
		if (!svr->IsError())
		{
			progCtrl->WaitForExit(progCtrl);
		}
		else
		{
			console->WriteLine((const UTF8Char*)"Error in listening port");
		}
		DEL_CLASS(svr);
		hdlr->Release();
	}
	SDEL_CLASS(ssl);
	DEL_CLASS(sockf);
	DEL_CLASS(console);
	return 0;
}
