#include "Stdafx.h"
#include "Core/Core.h"
#include "IO/ConsoleWriter.h"
#include "IO/IniFile.h"
#include "IO/LogTool.h"
#include "IO/Path.h"
#include "Net/OSSocketFactory.h"
#include "Net/WebServer/HTTPForwardHandler.h"
#include "Net/WebServer/WebListener.h"

IO::ConsoleWriter *console;
IO::LogTool *logger;

void __stdcall OnForwardRequest(void *userObj, NotNullPtr<Net::WebServer::IWebRequest> req, NotNullPtr<Net::WebServer::IWebResponse> resp)
{
	UTF8Char sbuff[128];
	UTF8Char *sptr;
	sptr = Net::SocketUtil::GetAddrName(sbuff, req->GetClientAddr(), req->GetClientPort());
	Text::StringBuilderUTF8 sb;

	sb.ClearStr();
	sb.AppendC(sbuff, (UOSInt)(sptr - sbuff));
	sb.AppendC(UTF8STRC(" Req "));
	Text::CString reqMeth = req->GetReqMethodStr();
	sb.AppendC(reqMeth.v, reqMeth.leng);
	sb.AppendUTF8Char(' ');
	sb.Append(req->GetRequestURI());
	logger->LogMessage(sb.ToCString(), IO::LogHandler::LogLevel::Action);

	Data::ArrayList<Text::String *> headerNames;
	Text::String *headerName;
	UOSInt i = 0;
	UOSInt j = req->GetHeaderNames(headerNames);
	while (i < j)
	{
		headerName = headerNames.GetItem(i);
		sb.ClearStr();
		sb.AppendC(sbuff, (UOSInt)(sptr - sbuff));
		sb.AppendC(UTF8STRC(" Req "));
		sb.Append(headerName);
		sb.AppendC(UTF8STRC(": "));
		sb.Append(req->GetSHeader(headerName->ToCString()));
		logger->LogMessage(sb.ToCString(), IO::LogHandler::LogLevel::Raw);
		i++;
	}
	sb.ClearStr();
	sb.AppendC(sbuff, (UOSInt)(sptr - sbuff));
	sb.AppendC(UTF8STRC(" Resp "));
	sb.AppendI32(resp->GetStatusCode());
	logger->LogMessage(sb.ToCString(), IO::LogHandler::LogLevel::Action);
	
	sb.ClearStr();
	sb.AppendC(sbuff, (UOSInt)(sptr - sbuff));
	sb.AppendC(UTF8STRC(" Resp "));
	sb.Append(resp->GetRespHeaders());
	logger->LogMessage(sb.ToCString(), IO::LogHandler::LogLevel::Raw);
}

Int32 MyMain(NotNullPtr<Core::IProgControl> progCtrl)
{
	UInt16 listenPort = 0;
	UInt16 forwardPort = 0;
	Text::String *s;
	UTF8Char sbuff[512];
	UTF8Char *sptr;

	NEW_CLASS(console, IO::ConsoleWriter());
	NEW_CLASS(logger, IO::LogTool());

	IO::ConfigFile *cfg = IO::IniFile::ParseProgConfig(0);
	if (cfg)
	{
		Bool succ = true;
		Net::OSSocketFactory sockf(false);
		if ((s = cfg->GetValue(CSTR("ListenPort"))) == 0 || !s->ToUInt16(listenPort))
		{
			console->WriteLineC(UTF8STRC("Config ListenPort is not valid"));
			succ = false;
		}
		if ((s = cfg->GetValue(CSTR("ForwardPort"))) == 0 || !s->ToUInt16(forwardPort))
		{
			console->WriteLineC(UTF8STRC("Config ForwardPort is not valid"));
			succ = false;
		}

		if (succ)
		{
			sptr = Text::StrConcatC(sbuff, UTF8STRC("http://127.0.0.1"));
			if (forwardPort != 80)
			{
				*sptr++ = ':';
				sptr = Text::StrUInt16(sptr, forwardPort);
			}
			NotNullPtr<Net::WebServer::HTTPForwardHandler> hdlr;
			Net::WebServer::WebListener *svr;
			NEW_CLASSNN(hdlr, Net::WebServer::HTTPForwardHandler(sockf, 0, CSTRP(sbuff, sptr), Net::WebServer::HTTPForwardHandler::ForwardType::Normal));
			sptr = IO::Path::GetProcessFileName(sbuff);
			sptr = IO::Path::AppendPath(sbuff, sptr, CSTR("FwdLog"));
			sptr = IO::Path::AppendPath(sbuff, sptr, CSTR("fwdLog"));
			logger->AddFileLog(CSTRP(sbuff, sptr), IO::LogHandler::LogType::PerDay, IO::LogHandler::LogGroup::PerMonth, IO::LogHandler::LogLevel::Raw, "yyyy-MM-dd HH:mm:ss.fff", false);
			hdlr->HandleForwardRequest(OnForwardRequest, 0);
			NEW_CLASS(svr, Net::WebServer::WebListener(sockf, 0, hdlr, listenPort, 120, 4, CSTR("sswr/1.0"), false, Net::WebServer::KeepAlive::Default, true));
			if (!svr->IsError())
			{
				console->WriteLineC(UTF8STRC("HTTP Forwarding started"));
				progCtrl->WaitForExit(progCtrl);
			}
			else
			{
				console->WriteLineC(UTF8STRC("Error in listening port"));
			}
			DEL_CLASS(svr);
			hdlr->Release();
		}
		DEL_CLASS(cfg);
	}
	DEL_CLASS(logger);
	DEL_CLASS(console);
	return 0;
}
