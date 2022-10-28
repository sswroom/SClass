#include "Stdafx.h"
#include "IO/JavaProperties.h"
#include "IO/Path.h"
#include "Manage/Computer.h"
#include "Manage/ComputerUser.h"
#include "Manage/Process.h"
#include "Net/Spring/SpringBootApplication.h"

Net::Spring::SpringBootApplication::SpringBootApplication(Text::CString appName) : consoleLog(&console)
{
	this->log.AddLogHandler(&this->consoleLog, IO::ILogHandler::LogLevel::Raw);
	this->cfg = IO::JavaProperties::ParseAppProp();
	this->activeProfile = 0;
	if (this->cfg)
	{
		Text::String *s = this->cfg->GetValue(CSTR("spring.profiles.active"));
		if (s)
		{
			this->activeProfile = s->Clone();
		}
	}
	if (this->activeProfile == 0)
	{
		this->activeProfile = Text::String::New(UTF8STRC("default"));
	}
	Text::StringBuilderUTF8 sb;
	UTF8Char sbuff[512];
	UTF8Char *sptr;
	UTF8Char *sptr2;
	Text::PString sarr[2];
	UOSInt i;
	sb.Append(this->activeProfile);
	sptr = IO::Path::GetProcessFileName(sbuff);
	sptr = IO::Path::AppendPath(sbuff, sptr, CSTR("application-"));
	i = 2;
	sarr[1] = sb;
	while (i == 2)
	{
		i = Text::StrSplitTrimP(sarr, 2, sarr[1], ',');
		sptr2 = sarr[0].ConcatTo(sptr);
		sptr2 = Text::StrConcatC(sptr2, UTF8STRC(".properties"));
		IO::ConfigFile *cfg = IO::JavaProperties::Parse(CSTRP(sbuff, sptr2));
		if (cfg)
		{
			this->cfg->MergeConfig(cfg);
			DEL_CLASS(cfg);
		}
	}

	sb.ClearStr();
	sb.AppendC(UTF8STRC("Starting "));
	sb.Append(appName);
	sb.AppendC(UTF8STRC(" on "));
	sptr = Manage::Computer::GetHostName(sbuff);
	if (sptr)
	{
		sb.AppendP(sbuff, sptr);
	}
	sb.AppendC(UTF8STRC(" with PID "));
	sb.AppendUOSInt(Manage::Process::GetCurrProcId());
	sb.AppendC(UTF8STRC(" ("));
	IO::Path::GetProcessFileName(&sb);
	sb.AppendC(UTF8STRC(" started by "));
	sptr = Manage::ComputerUser::GetProcessUser(sbuff);
	if (sptr)
	{
		sb.AppendP(sbuff, sptr);
	}
	sb.AppendC(UTF8STRC(" in "));
	sptr = IO::Path::GetCurrDirectory(sbuff);
	sb.AppendP(sbuff, sptr);
	sb.AppendUTF8Char(')');
	this->log.LogMessage(sb.ToCString(), IO::ILogHandler::LogLevel::Action);

	sb.ClearStr();
	sb.AppendC(UTF8STRC("The following profiles are active: "));
	sb.Append(this->activeProfile);
	this->log.LogMessage(sb.ToCString(), IO::ILogHandler::LogLevel::Action);
}

Net::Spring::SpringBootApplication::~SpringBootApplication()
{
	SDEL_CLASS(this->cfg);
	SDEL_STRING(this->activeProfile);
}