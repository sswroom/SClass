#include "Stdafx.h"
#include "Net/Email/AWSEmailClient.h"
#include "Net/Email/EmailSenderConfig.h"
#include "Net/Email/SMTPClient.h"

Optional<Net::Email::EmailSender> Net::Email::EmailSenderConfig::LoadFromConfig(NN<Net::TCPClientFactory> clif, Optional<Net::SSLEngine> ssl, NN<IO::ConfigFile> cfg, Text::CString cfgCategory, NN<IO::LogTool> log)
{
	Text::CStringNN category = cfgCategory.OrEmpty();
	NN<Text::String> s;
	if (!cfg->GetCateValue(category, CSTR("EmailSenderType")).SetTo(s))
	{
		log->LogMessage(CSTR("EmailSenderType not found"), IO::LogHandler::LogLevel::Error);
		return 0;
	}
	Text::StringBuilderUTF8 sb;
	if (s->Equals(CSTR("SMTP")))
	{
		Data::Duration timeout = 60000;
		NN<Text::String> host;
		UInt16 port;
		Net::Email::SMTPConn::ConnType connType;
		NN<Net::Email::SMTPClient> cli;
		if (!cfg->GetCateValue(category, CSTR("SMTPPort")).SetTo(s))
		{
			log->LogMessage(CSTR("SMTPPort not found"), IO::LogHandler::LogLevel::Error);
			return 0;
		}
		if (!s->ToUInt16(port) || port == 0 || port == 65535)
		{
			log->LogMessage(CSTR("SMTPPort not valid"), IO::LogHandler::LogLevel::Error);
			return 0;
		}
		if (!cfg->GetCateValue(category, CSTR("SMTPHost")).SetTo(host))
		{
			log->LogMessage(CSTR("SMTPHost not found"), IO::LogHandler::LogLevel::Error);
			return 0;
		}
		if (!cfg->GetCateValue(category, CSTR("SMTPType")).SetTo(s))
		{
			log->LogMessage(CSTR("SMTPType not found"), IO::LogHandler::LogLevel::Error);
			return 0;
		}
		if (s->Equals(CSTR("PLAIN")))
		{
			connType = Net::Email::SMTPConn::ConnType::Plain;
		}
		else if (s->Equals(CSTR("SSL")))
		{
			connType = Net::Email::SMTPConn::ConnType::SSL;
		}
		else if (s->Equals(CSTR("STARTTLS")))
		{
			connType = Net::Email::SMTPConn::ConnType::STARTTLS;
		}
		else
		{
			log->LogMessage(CSTR("SMTPType must be one of PLAIN, SSL or STARTTLS"), IO::LogHandler::LogLevel::Error);
			return 0;
		}
		if (cfg->GetCateValue(category, CSTR("SMTPTimeout")).SetTo(s))
		{
			Int64 ticks;
			if (!s->ToInt64(ticks) || ticks <= 0)
			{
				log->LogMessage(CSTR("SMTPTimeout must be positive integer"), IO::LogHandler::LogLevel::Error);
				return 0;
			}
			timeout = Data::Duration(ticks);
		}

		NEW_CLASSNN(cli, Net::Email::SMTPClient(clif, ssl, host->ToCString(), port, connType, Optional<IO::LogTool>(log), timeout));
		NN<Text::String> proxyHost;
		NN<Text::String> proxyPort;
		UInt16 proxyIPort;
		if (cfg->GetCateValue(category, CSTR("SMTPProxyHost")).SetTo(proxyHost) && cfg->GetCateValue(category, CSTR("SMTPProxyPort")).SetTo(proxyPort) && proxyPort->ToUInt16(proxyIPort))
		{
			Optional<Text::String> proxyUser = cfg->GetCateValue(category, CSTR("SMTPProxyUser"));
			Optional<Text::String> proxyPwd = cfg->GetCateValue(category, CSTR("SMTPProxyPwd"));
			clif->SetProxy(proxyHost->ToCString(), proxyIPort, OPTSTR_CSTR(proxyUser), OPTSTR_CSTR(proxyPwd));
		}
		NN<Text::String> user;
		NN<Text::String> password;
		if (cfg->GetCateValue(category, CSTR("SMTPUser")).SetTo(user) && cfg->GetCateValue(category, CSTR("SMTPPassword")).SetTo(password))
		{
			cli->SetPlainAuth(user->ToCString(), password->ToCString());
		}
		return cli;
	}
	else if (s->Equals(CSTR("AWS")))
	{
		if (!cfg->GetValue(CSTR("AWSRegion")).SetTo(s))
		{
			log->LogMessage(CSTR("AWSRegion not found"), IO::LogHandler::LogLevel::Error);
			return 0;
		}
		NN<Net::Email::AWSEmailClient> cli;
		NEW_CLASSNN(cli, Net::Email::AWSEmailClient(s->ToCString()));
		NN<Text::String> sport;
		UInt16 port;
		if (cfg->GetValue(CSTR("AWSProxyHost")).SetTo(s) && cfg->GetValue(CSTR("AWSProxyPort")).SetTo(sport) && sport->ToUInt16(port))
		{
			Optional<Text::String> proxyUser = cfg->GetValue(CSTR("AWSProxyUser"));
			Optional<Text::String> proxyPwd = cfg->GetValue(CSTR("AWSProxyPwd"));
			cli->SetProxy(s->ToCString(), port, OPTSTR_CSTR(proxyUser), OPTSTR_CSTR(proxyPwd));
		}
		return cli;
	}
	else
	{
		sb.ClearStr();
		sb.Append(CSTR("EmailSenderType not supported: "));
		sb.Append(s);
		log->LogMessage(sb.ToCString(), IO::LogHandler::LogLevel::Error);
		return 0;
	}
}
