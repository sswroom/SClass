#include "Stdafx.h"
#include "SSWR/ServerMonitor/ServerMonitorSMTPAlerter.h"

SSWR::ServerMonitor::ServerMonitorSMTPAlerter::ServerMonitorSMTPAlerter(NN<Net::TCPClientFactory> clif, Optional<Net::SSLEngine> ssl,  NN<IO::LogTool> log, Text::CStringNN setting, Text::CStringNN target)
{
	this->clif = clif;
	this->ssl = ssl;

	Text::StringBuilderUTF8 sb;
	sb.Append(setting);
	Text::PString sarr[6];
	if (Text::StrSplitP(sarr, 6, sb, '|') != 6)
	{
		log->LogMessage(CSTR("Invalid setting format"), IO::LogHandler::LogLevel::Error);
		this->cli = nullptr;
		this->fromEmail = Text::String::NewEmpty();
		this->toEmails = Text::String::NewEmpty();
		return;
	}
	Net::Email::SMTPConn::ConnType connType;
	if (sarr[0].Equals(CSTR("ssl")))
	{
		connType = Net::Email::SMTPConn::ConnType::SSL;
	}
	else if (sarr[0].Equals(CSTR("starttls")))
	{
		connType = Net::Email::SMTPConn::ConnType::STARTTLS;
	}
	else if (sarr[0].Equals(CSTR("plain")))
	{
		connType = Net::Email::SMTPConn::ConnType::Plain;
	}
	else
	{
		log->LogMessage(CSTR("Invalid connection type"), IO::LogHandler::LogLevel::Error);
		this->cli = nullptr;
		this->fromEmail = Text::String::NewEmpty();
		this->toEmails = Text::String::NewEmpty();
		return;
	}
	UInt16 port;
	if (!sarr[2].ToUInt16(port))
	{
		log->LogMessage(CSTR("Invalid port number"), IO::LogHandler::LogLevel::Error);
		this->cli = nullptr;
		this->fromEmail = Text::String::NewEmpty();
		this->toEmails = Text::String::NewEmpty();
		return;
	}
	NN<Net::Email::SMTPClient> cli;
	NEW_CLASSNN(cli, Net::Email::SMTPClient(clif, ssl, sarr[1].ToCString(), port, connType, Optional<IO::LogTool>(log), Data::Duration::FromSecNS(30, 0)));
	cli->SetPlainAuth(sarr[3].ToCString(), sarr[4].ToCString());
	this->cli = cli;
	this->fromEmail = Text::String::New(sarr[5].ToCString());
	this->toEmails = Text::String::New(target);
}

SSWR::ServerMonitor::ServerMonitorSMTPAlerter::ServerMonitorSMTPAlerter(NN<Net::TCPClientFactory> clif, Optional<Net::SSLEngine> ssl, NN<IO::LogTool> log, Text::CStringNN smtpHost, UInt16 smtpPort, Net::Email::SMTPConn::ConnType connType, Text::CStringNN smtpUser, Text::CStringNN smtpPassword, Text::CStringNN fromEmail, Text::CStringNN toEmails)
{
	this->clif = clif;
	this->ssl = ssl;

	NN<Net::Email::SMTPClient> cli;
	NEW_CLASSNN(cli, Net::Email::SMTPClient(clif, ssl, smtpHost, smtpPort, connType, Optional<IO::LogTool>(log), Data::Duration::FromSecNS(30, 0)));
	if (true)
	{
		cli->SetPlainAuth(smtpUser, smtpPassword);
	}
	this->cli = cli;

	this->fromEmail = Text::String::New(fromEmail);
	this->toEmails = Text::String::New(toEmails);
}

SSWR::ServerMonitor::ServerMonitorSMTPAlerter::~ServerMonitorSMTPAlerter()
{
	this->cli.Delete();
	this->fromEmail->Release();
	this->toEmails->Release();
}

Bool SSWR::ServerMonitor::ServerMonitorSMTPAlerter::HasError() const
{
	return this->cli.IsNull();
}

Bool SSWR::ServerMonitor::ServerMonitorSMTPAlerter::Send(Text::CStringNN serverName)
{
	NN<Net::Email::SMTPClient> cli;
	if (!this->cli.SetTo(cli))
		return false;

	NN<Net::Email::EmailMessage> msg;
	NEW_CLASSNN(msg, Net::Email::EmailMessage());
	msg->SetFrom(nullptr, this->fromEmail->ToCString());
	msg->AddToList(this->toEmails->ToCString());
	Text::StringBuilderUTF8 sb;
	sb.Append(serverName);
	sb.Append(CSTR(" is down"));
	msg->SetSubject(sb.ToCString());
	sb.ClearStr();
	sb.Append(serverName);
	sb.Append(CSTR(" is detected as down by ServerMonitor."));
	msg->SetContent(sb.ToCString(), CSTR("text/plain; charset=utf-8"));
	msg->SetSentDate(Data::Timestamp::Now());
	sb.ClearStr();
	Net::Email::EmailMessage::GenerateMessageID(sb, this->fromEmail->ToCString());
	msg->SetMessageId(sb.ToCString());
	return cli->Send(msg);
}

Bool SSWR::ServerMonitor::ServerMonitorSMTPAlerter::BuildSetting(NN<Text::StringBuilderUTF8> setting)
{
	NN<Net::Email::SMTPClient> cli;
	if (!this->cli.SetTo(cli))
	{
		return false;
	}	Net::Email::SMTPConn::ConnType connType = cli->GetConnType();
	if (connType == Net::Email::SMTPConn::ConnType::SSL)
	{
		setting->Append(CSTR("ssl|"));
	}
	else if (connType == Net::Email::SMTPConn::ConnType::STARTTLS)
	{
		setting->Append(CSTR("starttls|"));
	}
	else
	{
		setting->Append(CSTR("plain|"));
	}
	setting->Append(cli->GetHost());
	setting->Append(CSTR("|"));
	setting->AppendU16(cli->GetPort());
	setting->Append(CSTR("|"));
	setting->AppendOpt(cli->GetAuthUser());
	setting->Append(CSTR("|"));
	setting->AppendOpt(cli->GetAuthPassword());
	setting->Append(CSTR("|"));
	setting->Append(this->fromEmail);
	return true;
}

Bool SSWR::ServerMonitor::ServerMonitorSMTPAlerter::BuildTarget(NN<Text::StringBuilderUTF8> target)
{
	target->Append(this->toEmails);
	return true;
}