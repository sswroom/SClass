#include "Stdafx.h"
#include "Core/Core.h"
#include "DB/MySQLMaintance.h"
#include "IO/ConsoleWriter.h"
#include "IO/FileStream.h"
#include "IO/IniFile.h"
#include "IO/Path.h"
#include "Net/OSSocketFactory.h"
#include "Net/SSLEngineFactory.h"
#include "Net/Email/SMTPClient.h"

Int32 MyMain(Core::IProgControl *progCtrl)
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
		if (cfg->GetValue((const UTF8Char*)cptr, len) == 0)
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

	Text::String *mysqlServer = cfg->GetValue(UTF8STRC("MySQLServer"));
	Text::String *mysqlPort = cfg->GetValue(UTF8STRC("MySQLPort"));
	Text::String *mysqlUser = cfg->GetValue(UTF8STRC("MySQLUser"));
	Text::String *mysqlPassword = cfg->GetValue(UTF8STRC("MySQLPassword"));
	Text::String *mysqlSchemas = cfg->GetValue(UTF8STRC("MySQLSchemas"));
	Text::String *smtpHost = cfg->GetValue(UTF8STRC("SMTPHost"));
	Text::String *smtpPort = cfg->GetValue(UTF8STRC("SMTPPort"));
	Text::String *smtpType = cfg->GetValue(UTF8STRC("SMTPType"));
	Text::String *smtpFrom = cfg->GetValue(UTF8STRC("SMTPFrom"));
	Text::String *smtpTo = cfg->GetValue(UTF8STRC("SMTPTo"));
	Text::String *smtpUser = cfg->GetValue(UTF8STRC("SMTPUser"));
	Text::String *smtpPassword = cfg->GetValue(UTF8STRC("SMTPPassword"));
	Net::SocketFactory *sockf;
	Net::MySQLTCPClient *cli;
	DB::MySQLMaintance *mysql;
	Net::SocketUtil::AddressInfo addr;
	UInt16 port;
	UInt16 smtpIPort;
	Net::Email::SMTPConn::ConnType connType;
	UTF8Char *sarr[2];
	if (smtpType->EqualsICase(UTF8STRC("PLAIN")))
	{
		connType = Net::Email::SMTPConn::CT_PLAIN;
	}
	else if (smtpType->EqualsICase(UTF8STRC("SSL")))
	{
		connType = Net::Email::SMTPConn::CT_SSL;
	}
	else if (smtpType->Equals(UTF8STRC("STARTTLS")))
	{
		connType = Net::Email::SMTPConn::CT_STARTTLS;
	}
	else
	{
		DEL_CLASS(cfg);
		console.WriteLineC(UTF8STRC("SMTPType must be PLAIN/SSL/STARTTLS"));
		return 3;
	}

	Int32 retNum = 0;
	NEW_CLASS(sockf, Net::OSSocketFactory(false));
	if (!sockf->DNSResolveIP(mysqlServer->v, mysqlServer->leng, &addr))
	{
		console.WriteLineC(UTF8STRC("MySQLServer cannot be resolved"));
		return 4;
	}
	else if (!mysqlPort->ToUInt16(&port))
	{
		console.WriteLineC(UTF8STRC("MySQLPort is not valid"));
		return 5;
	}
	else if (!smtpPort->ToUInt16(&smtpIPort))
	{
		console.WriteLineC(UTF8STRC("SMTPPort is not valid"));
		return 6;
	}
	else
	{
		NEW_CLASS(cli, Net::MySQLTCPClient(sockf, &addr, port, mysqlUser, mysqlPassword, 0));
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
			sbMsg.Append(cfg->GetValue(UTF8STRC("ServerName")));
			sbMsg.AppendC(UTF8STRC("\r\n"));
			sbMsg.AppendC(UTF8STRC("MySQL Check detail:\r\n"));
			NEW_CLASS(mysql, DB::MySQLMaintance(cli, true));
			sb.ClearStr();
			sb.Append(mysqlSchemas);
			sarr[1] = sb.ToString();
			while (true)
			{
				i = Text::StrSplit(sarr, 2, sarr[1], ',');
				mysql->RepairSchema(sarr[0], &sbMsg);
				if (i != 2)
				{
					break;
				}
			}
			DEL_CLASS(mysql);

			sb.ClearStr();
			IO::Path::GetProcessFileName(&sb);
			sb.AppendC(UTF8STRC(".log"));
			IO::FileStream *fs;
			Text::UTF8Writer *writer;
			NEW_CLASS(fs, IO::FileStream(sb.ToCString(), IO::FileMode::Append, IO::FileShare::DenyWrite, IO::FileStream::BufferType::Normal));
			NEW_CLASS(writer, Text::UTF8Writer(fs));

			Net::SSLEngine *ssl = Net::SSLEngineFactory::Create(sockf, true);
			Net::Email::SMTPClient *smtp;
			Net::Email::EmailMessage *msg;
			Data::DateTime currTime;
			currTime.SetCurrTime();
			NEW_CLASS(smtp, Net::Email::SMTPClient(sockf, ssl, smtpHost->ToCString(), smtpIPort, connType, writer));
			if (smtpUser->v[0] && smtpPassword->v[0])
			{
				smtp->SetPlainAuth(smtpUser->v, smtpPassword->v);
			}
			NEW_CLASS(msg, Net::Email::EmailMessage());
			sb.ClearStr();
			Net::Email::EmailMessage::GenerateMessageID(&sb, smtpFrom->v);
			msg->SetMessageId(sb.ToString());
			msg->SetFrom(0, smtpFrom->v);
			msg->AddTo(0, smtpTo->v);
			msg->SetSentDate(&currTime);
			msg->SetSubject((const UTF8Char*)"MySQL Check Report");
			msg->SetContent(sbMsg.ToString(), "text/plain");
			if (!smtp->Send(msg))
			{
				retNum = 8;
			}
			DEL_CLASS(msg);
			DEL_CLASS(smtp);
			SDEL_CLASS(ssl);
			DEL_CLASS(writer);
			DEL_CLASS(fs);
		}
	}
	DEL_CLASS(cfg);
	DEL_CLASS(sockf);
	return retNum;
}