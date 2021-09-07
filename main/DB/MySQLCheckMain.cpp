#include "Stdafx.h"
#include "Core/Core.h"
#include "DB/MySQLMaintance.h"
#include "IO/ConsoleWriter.h"
#include "IO/FileStream.h"
#include "IO/IniFile.h"
#include "IO/Path.h"
#include "Net/DefaultSSLEngine.h"
#include "Net/OSSocketFactory.h"
#include "Net/Email/SMTPClient.h"

Int32 MyMain(Core::IProgControl *progCtrl)
{
	IO::ConsoleWriter console;
	IO::ConfigFile *cfg = IO::IniFile::ParseProgConfig(0);
	if (cfg == 0)
	{
		console.WriteLine((const UTF8Char*)"Config file not found\r\n");
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
		if (cfg->GetValue((const UTF8Char*)cptr) == 0)
		{
			sb.ClearStr();
			sb.Append((const UTF8Char*)"Config ");
			sb.Append((const UTF8Char*)cptr);
			sb.Append((const UTF8Char*)" not found");
			console.WriteLine(sb.ToString());
			DEL_CLASS(cfg);
			return 2;
		}
		i++;
	}

	const UTF8Char *mysqlServer = cfg->GetValue((const UTF8Char*)"MySQLServer");
	const UTF8Char *mysqlPort = cfg->GetValue((const UTF8Char*)"MySQLPort");
	const UTF8Char *mysqlUser = cfg->GetValue((const UTF8Char*)"MySQLUser");
	const UTF8Char *mysqlPassword = cfg->GetValue((const UTF8Char*)"MySQLPassword");
	const UTF8Char *mysqlSchemas = cfg->GetValue((const UTF8Char*)"MySQLSchemas");
	const UTF8Char *smtpHost = cfg->GetValue((const UTF8Char*)"SMTPHost");
	const UTF8Char *smtpPort = cfg->GetValue((const UTF8Char*)"SMTPPort");
	const UTF8Char *smtpType = cfg->GetValue((const UTF8Char*)"SMTPType");
	const UTF8Char *smtpFrom = cfg->GetValue((const UTF8Char*)"SMTPFrom");
	const UTF8Char *smtpTo = cfg->GetValue((const UTF8Char*)"SMTPTo");
	const UTF8Char *smtpUser = cfg->GetValue((const UTF8Char*)"SMTPUser");
	const UTF8Char *smtpPassword = cfg->GetValue((const UTF8Char*)"SMTPPassword");
	Net::SocketFactory *sockf;
	Net::MySQLTCPClient *cli;
	DB::MySQLMaintance *mysql;
	Net::SocketUtil::AddressInfo addr;
	UInt16 port;
	UInt16 smtpIPort;
	Net::Email::SMTPConn::ConnType connType;
	UTF8Char *sarr[2];
	if (Text::StrEqualsICase(smtpType, (const UTF8Char*)"PLAIN"))
	{
		connType = Net::Email::SMTPConn::CT_PLAIN;
	}
	else if (Text::StrEqualsICase(smtpType, (const UTF8Char*)"SSL"))
	{
		connType = Net::Email::SMTPConn::CT_SSL;
	}
	else if (Text::StrEqualsICase(smtpType, (const UTF8Char*)"STARTTLS"))
	{
		connType = Net::Email::SMTPConn::CT_STARTTLS;
	}
	else
	{
		DEL_CLASS(cfg);
		console.WriteLine((const UTF8Char*)"SMTPType must be PLAIN/SSL/STARTTLS");
		return 3;
	}

	Int32 retNum = 0;
	NEW_CLASS(sockf, Net::OSSocketFactory(false));
	if (!sockf->DNSResolveIP(mysqlServer, &addr))
	{
		console.WriteLine((const UTF8Char*)"MySQLServer cannot be resolved");
		return 4;
	}
	else if (!Text::StrToUInt16(mysqlPort, &port))
	{
		console.WriteLine((const UTF8Char*)"MySQLPort is not valid");
		return 5;
	}
	else if (!Text::StrToUInt16(smtpPort, &smtpIPort))
	{
		console.WriteLine((const UTF8Char*)"SMTPPort is not valid");
		return 6;
	}
	else
	{
		NEW_CLASS(cli, Net::MySQLTCPClient(sockf, &addr, port, mysqlUser, mysqlPassword, 0));
		if (cli->IsError())
		{
			console.WriteLine((const UTF8Char*)"Error in connecting to MySQL server");
			DEL_CLASS(cli);
			retNum = 7;
		}
		else
		{
			Text::StringBuilderUTF8 sbMsg;
			sbMsg.Append((const UTF8Char*)"ServerName = ");
			sbMsg.Append(cfg->GetValue((const UTF8Char*)"ServerName"));
			sbMsg.Append((const UTF8Char*)"\r\n");
			sbMsg.Append((const UTF8Char*)"MySQL Check detail:\r\n");
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
			sb.Append((const UTF8Char*)".log");
			IO::FileStream *fs;
			Text::UTF8Writer *writer;
			NEW_CLASS(fs, IO::FileStream(sb.ToString(), IO::FileStream::FILE_MODE_APPEND, IO::FileStream::FILE_SHARE_DENY_WRITE, IO::FileStream::BT_NORMAL));
			NEW_CLASS(writer, Text::UTF8Writer(fs));

			Net::SSLEngine *ssl = Net::DefaultSSLEngine::Create(sockf, true);
			Net::Email::SMTPClient *smtp;
			Net::Email::EmailMessage *msg;
			Data::DateTime currTime;
			currTime.SetCurrTime();
			NEW_CLASS(smtp, Net::Email::SMTPClient(sockf, ssl, smtpHost, smtpIPort, connType, writer));
			if (smtpUser[0] && smtpPassword[0])
			{
				smtp->SetPlainAuth(smtpUser, smtpPassword);
			}
			NEW_CLASS(msg, Net::Email::EmailMessage());
			sb.ClearStr();
			Net::Email::EmailMessage::GenerateMessageID(&sb, smtpFrom);
			msg->SetMessageId(sb.ToString());
			msg->SetFrom(0, smtpFrom);
			msg->AddTo(0, smtpTo);
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