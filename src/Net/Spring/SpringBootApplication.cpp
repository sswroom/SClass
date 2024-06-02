#include "Stdafx.h"
#include "IO/JavaProperties.h"
#include "IO/Path.h"
#include "Manage/Computer.h"
#include "Manage/ComputerUser.h"
#include "Manage/Process.h"
#include "Net/Spring/SpringBootApplication.h"

Net::Spring::SpringBootApplication::SpringBootApplication(Text::CStringNN appName) : consoleLog(console)
{
	this->log.AddLogHandler(this->consoleLog, IO::LogHandler::LogLevel::Raw);
	this->cfg = IO::JavaProperties::ParseAppProp();
	this->activeProfile = Text::String::New(UTF8STRC("default"));
	NN<IO::ConfigFile> cfg;
	if (this->cfg.SetTo(cfg))
	{
		NN<Text::String> s;
		if (cfg->GetValue(CSTR("spring.profiles.active")).SetTo(s))
		{
			this->activeProfile->Release();
			this->activeProfile = s->Clone();
		}
	}
	Text::StringBuilderUTF8 sb;
	UTF8Char sbuff[512];
	UnsafeArray<UTF8Char> sptr;
	UnsafeArray<UTF8Char> sptr2;
	Text::PString sarr[2];
	UOSInt i;
	sb.Append(this->activeProfile);
	sptr = IO::Path::GetProcessFileName(sbuff).Or(sbuff);
	sptr = IO::Path::AppendPath(sbuff, sptr, CSTR("application-"));
	i = 2;
	sarr[1] = sb;
	while (i == 2)
	{
		i = Text::StrSplitTrimP(sarr, 2, sarr[1], ',');
		sptr2 = sarr[0].ConcatTo(sptr);
		sptr2 = Text::StrConcatC(sptr2, UTF8STRC(".properties"));
		NN<IO::ConfigFile> nncfg;
		if (this->cfg.SetTo(nncfg) && IO::JavaProperties::Parse(CSTRP(sbuff, sptr2)).SetTo(cfg))
		{
			nncfg->MergeConfig(cfg);
			cfg.Delete();
		}
	}

	sb.ClearStr();
	sb.AppendC(UTF8STRC("Starting "));
	sb.Append(appName);
	sb.AppendC(UTF8STRC(" on "));
	if (Manage::Computer::GetHostName(sbuff).SetTo(sptr))
	{
		sb.AppendP(sbuff, sptr);
	}
	sb.AppendC(UTF8STRC(" with PID "));
	sb.AppendUOSInt(Manage::Process::GetCurrProcId());
	sb.AppendC(UTF8STRC(" ("));
	IO::Path::GetProcessFileName(sb);
	sb.AppendC(UTF8STRC(" started by "));
	if (Manage::ComputerUser::GetProcessUser(sbuff).SetTo(sptr))
	{
		sb.AppendP(sbuff, sptr);
	}
	sb.AppendC(UTF8STRC(" in "));
	sptr = IO::Path::GetCurrDirectory(sbuff).Or(sbuff);
	sb.AppendP(sbuff, sptr);
	sb.AppendUTF8Char(')');
	this->log.LogMessage(sb.ToCString(), IO::LogHandler::LogLevel::Action);

	sb.ClearStr();
	sb.AppendC(UTF8STRC("The following profiles are active: "));
	sb.Append(this->activeProfile);
	this->log.LogMessage(sb.ToCString(), IO::LogHandler::LogLevel::Action);
}

Net::Spring::SpringBootApplication::~SpringBootApplication()
{
	this->cfg.Delete();
	this->activeProfile->Release();
}