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

void __stdcall OnForwardRequest(AnyType userObj, NN<Net::WebServer::WebRequest> req, NN<Net::WebServer::WebResponse> resp)
{
	UTF8Char sbuff[128];
	UnsafeArray<UTF8Char> sptr;
	sptr = Net::SocketUtil::GetAddrName(sbuff, req->GetClientAddr(), req->GetClientPort()).Or(sbuff);
	Text::StringBuilderUTF8 sb;

	sb.ClearStr();
	sb.AppendC(sbuff, (UOSInt)(sptr - sbuff));
	sb.AppendC(UTF8STRC(" Req "));
	Text::CStringNN reqMeth = req->GetReqMethodStr();
	sb.AppendC(reqMeth.v, reqMeth.leng);
	sb.AppendUTF8Char(' ');
	sb.Append(req->GetRequestURI());
	logger->LogMessage(sb.ToCString(), IO::LogHandler::LogLevel::Action);

	Data::ArrayListStringNN headerNames;
	NN<Text::String> headerName;
	NN<Text::String> headerVal;
	req->GetHeaderNames(headerNames);
	Data::ArrayIterator<NN<Text::String>> it = headerNames.Iterator();
	while (it.HasNext())
	{
		headerName = it.Next();
		sb.ClearStr();
		sb.AppendC(sbuff, (UOSInt)(sptr - sbuff));
		sb.AppendC(UTF8STRC(" Req "));
		sb.Append(headerName);
		sb.AppendC(UTF8STRC(": "));
		if (req->GetSHeader(headerName->ToCString()).SetTo(headerVal))
			sb.Append(headerVal);
		logger->LogMessage(sb.ToCString(), IO::LogHandler::LogLevel::Raw);
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

Int32 MyMain(NN<Core::IProgControl> progCtrl)
{
	UInt16 listenPort = 0;
	UInt16 forwardPort = 0;
	NN<Text::String> s;
	UTF8Char sbuff[512];
	UnsafeArray<UTF8Char> sptr;

	NEW_CLASS(console, IO::ConsoleWriter());
	NEW_CLASS(logger, IO::LogTool());

	NN<IO::ConfigFile> cfg;
	if (IO::IniFile::ParseProgConfig(0).SetTo(cfg))
	{
		Bool succ = true;
		Net::OSSocketFactory sockf(false);
		Net::TCPClientFactory clif(sockf);
		if (!cfg->GetValue(CSTR("ListenPort")).SetTo(s) || !s->ToUInt16(listenPort))
		{
			console->WriteLine(CSTR("Config ListenPort is not valid"));
			succ = false;
		}
		if (!cfg->GetValue(CSTR("ForwardPort")).SetTo(s) || !s->ToUInt16(forwardPort))
		{
			console->WriteLine(CSTR("Config ForwardPort is not valid"));
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
			NN<Net::WebServer::HTTPForwardHandler> hdlr;
			Net::WebServer::WebListener *svr;
			NEW_CLASSNN(hdlr, Net::WebServer::HTTPForwardHandler(clif, 0, CSTRP(sbuff, sptr), Net::WebServer::HTTPForwardHandler::ForwardType::Normal));
			sptr = IO::Path::GetProcessFileName(sbuff).Or(sbuff);
			sptr = IO::Path::AppendPath(sbuff, sptr, CSTR("FwdLog"));
			sptr = IO::Path::AppendPath(sbuff, sptr, CSTR("fwdLog"));
			logger->AddFileLog(CSTRP(sbuff, sptr), IO::LogHandler::LogType::PerDay, IO::LogHandler::LogGroup::PerMonth, IO::LogHandler::LogLevel::Raw, "yyyy-MM-dd HH:mm:ss.fff", false);
			hdlr->HandleForwardRequest(OnForwardRequest, 0);
			NEW_CLASS(svr, Net::WebServer::WebListener(clif, 0, hdlr, listenPort, 120, 1, 4, CSTR("sswr/1.0"), false, Net::WebServer::KeepAlive::Default, true));
			if (!svr->IsError())
			{
				console->WriteLine(CSTR("HTTP Forwarding started"));
				progCtrl->WaitForExit(progCtrl);
			}
			else
			{
				console->WriteLine(CSTR("Error in listening port"));
			}
			DEL_CLASS(svr);
			hdlr.Delete();
		}
		cfg.Delete();
	}
	DEL_CLASS(logger);
	DEL_CLASS(console);
	return 0;
}
