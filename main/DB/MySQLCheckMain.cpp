#include "Stdafx.h"
#include "Core/Core.h"
#include "DB/MySQLMaintance.h"
#include "IO/ConsoleWriter.h"
#include "IO/FileStream.h"
#include "IO/IniFile.h"
#include "IO/LogWriter.h"
#include "IO/Path.h"
#include "Net/OSSocketFactory.h"
#include "Net/SSLEngineFactory.h"
#include "Net/Email/SMTPClient.h"

Int32 MyMain(NotNullPtr<Core::IProgControl> progCtrl)
{
	IO::ConsoleWriter console;
	IO::ConfigFile *cfg = IO::IniFile::ParseProgConfig(0);
	if (cfg == 0)
	{
		console.WriteLineC(UTF8STRC("Config file not found\r\n"));
		return 1;
	}
	static const Char *cfgList[] = {
		"ServerName",
		"MySQLServer",
		"MySQLPort",
		"MySQLUser",
		"MySQLPassword",
		"MySQLSchemas",
		"SMTPHost",
		"SMTPPort",
		"SMTPType",
		"SMTPFrom",
		"SMTPTo",
		"SMTPUser",
		"SMTPPassword"
	};
	Text::StringBuilderUTF8 sb;
	const Char *cptr;
	UOSInt i = 0;
	UOSInt j = sizeof(cfgList) / sizeof(cfgList[0]);
	while (i < j)
	{
		cptr = cfgList[i];
		UOSInt len = Text::StrCharCnt(cptr);
		if (cfg->GetValue({(const UTF8Char*)cptr, len}) == 0)
		{
			sb.ClearStr();
			sb.AppendC(UTF8STRC("Config "));
			sb.AppendC((const UTF8Char*)cptr, len);
			sb.AppendC(UTF8STRC(" not found"));
			console.WriteLineC(sb.ToString(), sb.GetLength());
			DEL_CLASS(cfg);
			return 2;
		}
		i++;
	}

	Text::String *mysqlServer = cfg->GetValue(CSTR("MySQLServer"));
	Text::String *mysqlPort = cfg->GetValue(CSTR("MySQLPort"));
	Text::String *mysqlUser = cfg->GetValue(CSTR("MySQLUser"));
	Text::String *mysqlPassword = cfg->GetValue(CSTR("MySQLPassword"));
	Text::String *mysqlSchemas = cfg->GetValue(CSTR("MySQLSchemas"));
	Text::String *smtpHost = cfg->GetValue(CSTR("SMTPHost"));
	Text::String *smtpPort = cfg->GetValue(CSTR("SMTPPort"));
	Text::String *smtpType = cfg->GetValue(CSTR("SMTPType"));
	Text::String *smtpFrom = cfg->GetValue(CSTR("SMTPFrom"));
	Text::String *smtpTo = cfg->GetValue(CSTR("SMTPTo"));
	Text::String *smtpUser = cfg->GetValue(CSTR("SMTPUser"));
	Text::String *smtpPassword = cfg->GetValue(CSTR("SMTPPassword"));
	Net::MySQLTCPClient *cli;
	DB::MySQLMaintance *mysql;
	Net::SocketUtil::AddressInfo addr;
	UInt16 port;
	UInt16 smtpIPort;
	Net::Email::SMTPConn::ConnType connType;
	UTF8Char *sarr[2];
	if (smtpType->EqualsICase(UTF8STRC("PLAIN")))
	{
		connType = Net::Email::SMTPConn::ConnType::Plain;
	}
	else if (smtpType->EqualsICase(UTF8STRC("SSL")))
	{
		connType = Net::Email::SMTPConn::ConnType::SSL;
	}
	else if (smtpType->Equals(UTF8STRC("STARTTLS")))
	{
		connType = Net::Email::SMTPConn::ConnType::STARTTLS;
	}
	else
	{
		DEL_CLASS(cfg);
		console.WriteLineC(UTF8STRC("SMTPType must be PLAIN/SSL/STARTTLS"));
		return 3;
	}

	Int32 retNum = 0;
	Net::OSSocketFactory sockf(false);
	if (!sockf.DNSResolveIP(mysqlServer->ToCString(), addr))
	{
		console.WriteLineC(UTF8STRC("MySQLServer cannot be resolved"));
		return 4;
	}
	else if (!mysqlPort->ToUInt16(port))
	{
		console.WriteLineC(UTF8STRC("MySQLPort is not valid"));
		return 5;
	}
	else if (!smtpPort->ToUInt16(smtpIPort))
	{
		console.WriteLineC(UTF8STRC("SMTPPort is not valid"));
		return 6;
	}
	else
	{
		NEW_CLASS(cli, Net::MySQLTCPClient(sockf, addr, port, Text::String::OrEmpty(mysqlUser), Text::String::OrEmpty(mysqlPassword), 0));
		if (cli->IsError())
		{
			console.WriteLineC(UTF8STRC("Error in connecting to MySQL server"));
			DEL_CLASS(cli);
			retNum = 7;
		}
		else
		{
			Text::StringBuilderUTF8 sbMsg;
			sbMsg.AppendC(UTF8STRC("ServerName = "));
			sbMsg.Append(cfg->GetValue(CSTR("ServerName")));
			sbMsg.AppendC(UTF8STRC("\r\n"));
			sbMsg.AppendC(UTF8STRC("MySQL Check detail:\r\n"));
			NEW_CLASS(mysql, DB::MySQLMaintance(cli, true));
			sb.ClearStr();
			sb.Append(mysqlSchemas);
			sarr[1] = sb.v;
			while (true)
			{
				i = Text::StrSplit(sarr, 2, sarr[1], ',');
				mysql->RepairSchema(sarr[0], sbMsg);
				if (i != 2)
				{
					break;
				}
			}
			DEL_CLASS(mysql);

			sb.ClearStr();
			IO::Path::GetProcessFileName(sb);
			sb.AppendC(UTF8STRC(".log"));
			IO::LogTool log;
			log.AddFileLog(sb.ToCString(), IO::LogHandler::LogType::SingleFile, IO::LogHandler::LogGroup::NoGroup, IO::LogHandler::LogLevel::Raw, "yyyy-MM-dd HH:mm:ss.fff", true);
			IO::LogWriter *writer;
			NEW_CLASS(writer, IO::LogWriter(log, IO::LogHandler::LogLevel::Command));

			Net::SSLEngine *ssl = Net::SSLEngineFactory::Create(sockf, true);
			Net::Email::SMTPClient *smtp;
			Data::DateTime currTime;
			currTime.SetCurrTime();
			NEW_CLASS(smtp, Net::Email::SMTPClient(sockf, ssl, smtpHost->ToCString(), smtpIPort, connType, writer, 30000));
			if (smtpUser->v[0] && smtpPassword->v[0])
			{
				smtp->SetPlainAuth(smtpUser->ToCString(), smtpPassword->ToCString());
			}
			Net::Email::EmailMessage msg;
			sb.ClearStr();
			Net::Email::EmailMessage::GenerateMessageID(sb, smtpFrom->ToCString());
			msg.SetMessageId(sb.ToCString());
			msg.SetFrom(CSTR_NULL, smtpFrom->ToCString());
			msg.AddTo(CSTR_NULL, smtpTo->ToCString());
			msg.SetSentDate(currTime);
			msg.SetSubject(CSTR("MySQL Check Report"));
			msg.SetContent(sbMsg.ToCString(), CSTR("text/plain"));
			if (!smtp->Send(msg))
			{
				retNum = 8;
			}
			DEL_CLASS(smtp);
			SDEL_CLASS(ssl);
			DEL_CLASS(writer);
		}
	}
	DEL_CLASS(cfg);
	return retNum;
}