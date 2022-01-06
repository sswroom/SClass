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

void __stdcall OnForwardRequest(void *userObj, Net::WebServer::IWebRequest *req, Net::WebServer::IWebResponse *resp)
{
	UTF8Char sbuff[128];
	UTF8Char *sptr;
	sptr = Net::SocketUtil::GetAddrName(sbuff, req->GetClientAddr(), req->GetClientPort());
	Text::StringBuilderUTF8 sb;

	sb.ClearStr();
	sb.AppendC(sbuff, (UOSInt)(sptr - sbuff));
	sb.AppendC(UTF8STRC(" Req "));
	sb.Append((const UTF8Char*)req->GetReqMethodStr());
	sb.AppendChar(' ', 1);
	sb.Append(req->GetRequestURI());
	logger->LogMessageC(sb.ToString(), sb.GetLength(), IO::ILogHandler::LOG_LEVEL_ACTION);

	Data::ArrayList<Text::String *> headerNames;
	Text::String *headerName;
	UOSInt i = 0;
	UOSInt j = req->GetHeaderNames(&headerNames);
	while (i < j)
	{
		headerName = headerNames.GetItem(i);
		sb.ClearStr();
		sb.AppendC(sbuff, (UOSInt)(sptr - sbuff));
		sb.AppendC(UTF8STRC(" Req "));
		sb.Append(headerName);
		sb.AppendC(UTF8STRC(": "));
		sb.Append(req->GetSHeader(headerName->v));
		logger->LogMessageC(sb.ToString(), sb.GetLength(), IO::ILogHandler::LOG_LEVEL_RAW);
		i++;
	}
	sb.ClearStr();
	sb.AppendC(sbuff, (UOSInt)(sptr - sbuff));
	sb.AppendC(UTF8STRC(" Resp "));
	sb.AppendI32(resp->GetStatusCode());
	logger->LogMessageC(sb.ToString(), sb.GetLength(), IO::ILogHandler::LOG_LEVEL_ACTION);
	
	sb.ClearStr();
	sb.AppendC(sbuff, (UOSInt)(sptr - sbuff));
	sb.AppendC(UTF8STRC(" Resp "));
	sb.Append(resp->GetRespHeaders());
	logger->LogMessageC(sb.ToString(), sb.GetLength(), IO::ILogHandler::LOG_LEVEL_RAW);
}

Int32 MyMain(Core::IProgControl *progCtrl)
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
		Net::SocketFactory *sockf;
		Bool succ = true;
		NEW_CLASS(sockf, Net::OSSocketFactory(false));
		if ((s = cfg->GetValue((const UTF8Char*)"ListenPort")) == 0 || !s->ToUInt16(&listenPort))
		{
			console->WriteLineC(UTF8STRC("Config ListenPort is not valid"));
			succ = false;
		}
		if ((s = cfg->GetValue((const UTF8Char*)"ForwardPort")) == 0 || !s->ToUInt16(&forwardPort))
		{
			console->WriteLineC(UTF8STRC("Config ForwardPort is not valid"));
			succ = false;
		}

		if (succ)
		{
			sptr = Text::StrConcat(sbuff, (const UTF8Char*)"http://127.0.0.1");
			if (forwardPort != 80)
			{
				*sptr++ = ':';
				Text::StrUInt16(sptr, forwardPort);
			}
			Net::WebServer::HTTPForwardHandler *hdlr;
			Net::WebServer::WebListener *svr;
			NEW_CLASS(hdlr, Net::WebServer::HTTPForwardHandler(sockf, 0, sbuff, Net::WebServer::HTTPForwardHandler::ForwardType::Normal));
			IO::Path::GetProcessFileName(sbuff);
			IO::Path::AppendPath(sbuff, (const UTF8Char*)"FwdLog");
			IO::Path::AppendPath(sbuff, (const UTF8Char*)"fwdLog");
			logger->AddFileLog(sbuff, IO::ILogHandler::LOG_TYPE_PER_DAY, IO::ILogHandler::LOG_GROUP_TYPE_PER_MONTH, IO::ILogHandler::LOG_LEVEL_RAW, "yyyy-MM-dd HH:mm:ss.fff", false);
			hdlr->HandleForwardRequest(OnForwardRequest, 0);
			NEW_CLASS(svr, Net::WebServer::WebListener(sockf, 0, hdlr, listenPort, 120, 4, (const UTF8Char*)"sswr/1.0", false, true));
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
		DEL_CLASS(sockf);
		DEL_CLASS(cfg);
	}
	DEL_CLASS(logger);
	DEL_CLASS(console);
	return 0;
}
