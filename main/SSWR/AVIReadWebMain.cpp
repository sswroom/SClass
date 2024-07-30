#include "Stdafx.h"
#include "Core/Core.h"
#include "IO/ConsoleWriter.h"
#include "IO/Path.h"
#include "Net/OSSocketFactory.h"
#include "Net/WebServer/NodeModuleHandler.h"
#include "Net/WebServer/WebListener.h"

Int32 MyMain(NN<Core::IProgControl> progCtrl)
{
	UTF8Char sbuff[512];
	UnsafeArray<UTF8Char> sptr;
	Net::OSSocketFactory sockf(true);
	Net::TCPClientFactory clif(sockf);
	Net::WebServer::WebStandardHandler baseHdlr;
	NN<Net::WebServer::HTTPDirectoryHandler> hdlr;
	NN<Net::WebServer::NodeModuleHandler> hdlr2;
	sptr = IO::Path::GetRealPath(sbuff, UTF8STRC("~/Progs/SClass/js/aviread"));
	NEW_CLASSNN(hdlr, Net::WebServer::HTTPDirectoryHandler(CSTRP(sbuff, sptr), true, 0, false));
	baseHdlr.HandlePath(CSTR("/aviread"), hdlr, true);
	sptr = IO::Path::GetRealPath(sbuff, UTF8STRC("~/Progs/VCClass/Release/SMonitorSvr/node_modules"));
	NEW_CLASSNN(hdlr2, Net::WebServer::NodeModuleHandler(CSTRP(sbuff, sptr), 0));
	baseHdlr.HandlePath(CSTR("/js"), hdlr2, true);
	sptr = IO::Path::GetRealPath(sbuff, UTF8STRC("~/Progs/SClass/js/sswr"));
	NEW_CLASSNN(hdlr, Net::WebServer::HTTPDirectoryHandler(CSTRP(sbuff, sptr), true, 0, false));
	hdlr2->HandlePath(CSTR("/@sswroom/sswr"), hdlr, true);
	{
		IO::ConsoleWriter console;
		Net::WebServer::WebListener listener(clif, 0, baseHdlr, 5080, 60, 2, 8, CSTR("AVIReadWeb/1.0"), false, Net::WebServer::KeepAlive::Default, true);
		if (!listener.IsError())
		{
			console.WriteLine(CSTR("AVIReadWeb started"));
			progCtrl->WaitForExit(progCtrl);
		}
		else
		{
			console.WriteLine(CSTR("Error in listening to port 5080"));
		}
	}
	return 0;
}