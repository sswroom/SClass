#include "Stdafx.h"
#include "Core/Core.h"
#include "IO/ConsoleWriter.h"
#include "Net/OSSocketFactory.h"
#include "Net/SSLEngineFactory.h"
#include "Net/WebServer/HTTPDirectoryHandler.h"
#include "Net/WebServer/HTTPFormParser.h"
#include "Net/WebServer/WebListener.h"
#include "Net/WebServer/WebStandardHandler.h"
#include "Net/WebServer/WebServiceHandler.h"
#include "Text/StringBuilderUTF8.h"
#include "Text/XML.h"

#define USESSL

IO::ConsoleWriter *console;

class MyHandler : public Net::WebServer::WebServiceHandler
{
private:
	Data::ArrayListNN<Net::WebServer::IWebResponse> sseConns;
	Sync::Mutex sseMut;

	static void __stdcall SSEDisconnect(NotNullPtr<Net::WebServer::IWebResponse> resp, AnyType userObj)
	{
		NotNullPtr<MyHandler> me = userObj.GetNN<MyHandler>();
		Sync::MutexUsage mutUsage(me->sseMut);
		me->sseConns.Remove(resp);
		console->WriteLineC(UTF8STRC("Disconnected"));
	}

	static Bool __stdcall SSEHandler(NotNullPtr<Net::WebServer::IWebRequest> req, NotNullPtr<Net::WebServer::IWebResponse> resp, Text::CStringNN subReq, NotNullPtr<WebServiceHandler> me)
	{
		NotNullPtr<MyHandler> myHdlr = NotNullPtr<MyHandler>::ConvertFrom(me);
		resp->AddDefHeaders(req);
		if (resp->ResponseSSE(30000, SSEDisconnect, myHdlr))
		{
			Sync::MutexUsage mutUsage(myHdlr->sseMut);
			myHdlr->sseConns.Add(resp);
			
			resp->SSESend((const UTF8Char*)"INIT", 0);
			resp->SSESend((const UTF8Char*)"Test", (const UTF8Char*)"ABC");
			return true;
		}
		return false;
	}
public:
	MyHandler()
	{
		this->AddService(CSTR("/sse"), Net::WebUtil::RequestMethod::HTTP_GET, SSEHandler);
	}

	virtual ~MyHandler()
	{
	}
};

Int32 MyMain(NotNullPtr<Core::IProgControl> progCtrl)
{
	UTF8Char sbuff[512];
	UTF8Char *sptr;
	Net::WebServer::WebListener *svr;
	Optional<Net::SSLEngine> ssl;
	Text::StringBuilderUTF8 sb;
	UInt16 port = 0;
	Bool succ = true;
	NEW_CLASS(console, IO::ConsoleWriter());

	UOSInt argc;
	UTF8Char **argv = progCtrl->GetCommandLines(progCtrl, argc);
	if (argc >= 2)
	{
		if (!Text::StrToUInt16(argv[1], port))
		{
			console->WriteLineC(UTF8STRC("Error in parsing port number, use default port"));
			port = 0;
		}
	}

	if (port == 0)
	{
#if defined(USESSL)
		port = 443;
#else
		port = 80;
#endif
	}

	Net::OSSocketFactory sockf(true);
#if defined(USESSL)
	ssl = Net::SSLEngineFactory::Create(sockf, true);
	NotNullPtr<Net::SSLEngine> nnssl;
	if (!ssl.SetTo(nnssl))
	{
		console->WriteLineC(UTF8STRC("Error in initializing SSL engine"));
		succ = false;
	}
	else if (!nnssl->ServerSetCerts(CSTR("test.crt"), CSTR("test.key")))
	{
		console->WriteLineC(UTF8STRC("Error in initializing SSL"));
		sptr = nnssl->GetErrorDetail(sbuff);
		console->WriteLineC(sbuff, (UOSInt)(sptr - sbuff));
		succ = false;
	}
#else
	ssl = 0;
#endif

	if (succ)
	{
		sb.ClearStr();
		sb.AppendC(UTF8STRC("Listening to port "));
		sb.AppendU16(port);
		console->WriteLineC(sb.ToString(), sb.GetLength());
		NotNullPtr<Net::WebServer::WebStandardHandler> hdlr;
		NotNullPtr<MyHandler> myHdlr;
		NEW_CLASSNN(hdlr, Net::WebServer::HTTPDirectoryHandler(CSTR("wwwroot"), true, 0, true));
		NEW_CLASSNN(myHdlr, MyHandler());
		hdlr->HandlePath(CSTR("/api"), myHdlr, true);
		NEW_CLASS(svr, Net::WebServer::WebListener(sockf, ssl, hdlr, port, 120, 1, 4, CSTR("sswr/1.0"), false, Net::WebServer::KeepAlive::Default, true));
		if (!svr->IsError())
		{
			progCtrl->WaitForExit(progCtrl);
		}
		else
		{
			console->WriteLineC(UTF8STRC("Error in listening port"));
		}
		DEL_CLASS(svr);
		hdlr.Delete();
	}
	ssl.Delete();
	DEL_CLASS(console);
	return 0;
}
