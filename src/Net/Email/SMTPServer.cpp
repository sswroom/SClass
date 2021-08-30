#include "Stdafx.h"
#include "MyMemory.h"
#include "Crypto/Encrypt/Base64.h"
#include "Net/Email/SMTPServer.h"
#include "Text/Encoding.h"
#include "Text/MyString.h"
#include "Text/StringBuilderUTF8.h"

void __stdcall Net::Email::SMTPServer::ConnHdlr(Socket *s, void *userObj)
{
	Net::Email::SMTPServer *me = (Net::Email::SMTPServer*)userObj;
	Net::TCPClient *cli;
	MailStatus *cliStatus;
	NEW_CLASS(cli, Net::TCPClient(me->sockf, s));
	cliStatus = MemAlloc(MailStatus, 1);
	cliStatus->buff = MemAlloc(UInt8, 4096);
	cliStatus->buffSize = 0;
	cliStatus->cliName = 0;
	cliStatus->mailFrom = 0;
	NEW_CLASS(cliStatus->rcptTo, Data::ArrayList<const UTF8Char *>());
	cliStatus->dataStm = 0;
	cliStatus->dataMode = false;
	cliStatus->loginMode = 0;
	cliStatus->login = false;
	cliStatus->userName = 0;
	me->cliMgr->AddClient(cli, cliStatus);

	Text::StringBuilderUTF8 sb;
	sb.Append(me->domain);
	sb.Append((const UTF8Char *)" ESMTP");
//	sb.Append(me->serverName);
	me->WriteMessage(cli, 220, sb.ToString());
}

void __stdcall Net::Email::SMTPServer::ClientEvent(Net::TCPClient *cli, void *userObj, void *cliData, Net::TCPClientMgr::TCPEventType evtType)
{
	if (evtType == Net::TCPClientMgr::TCP_EVENT_DISCONNECT)
	{
		MailStatus *cliStatus;
		UOSInt i;
		cliStatus = (MailStatus*)cliData;
		MemFree(cliStatus->buff);
		if (cliStatus->cliName)
		{
			Text::StrDelNew(cliStatus->cliName);
		}
		if (cliStatus->mailFrom)
		{
			Text::StrDelNew(cliStatus->mailFrom);
		}
		i = cliStatus->rcptTo->GetCount();
		while (i-- > 0)
		{
			Text::StrDelNew(cliStatus->rcptTo->GetItem(i));
		}
		DEL_CLASS(cliStatus->rcptTo);
		if (cliStatus->dataStm)
		{
			DEL_CLASS(cliStatus->dataStm);
		}
		MemFree(cliStatus);
		DEL_CLASS(cli);
	}
	else if (evtType == Net::TCPClientMgr::TCP_EVENT_HASDATA)
	{
	}
}

void __stdcall Net::Email::SMTPServer::ClientData(Net::TCPClient *cli, void *userObj, void *cliData, const UInt8 *buff, UOSInt size)
{
	Net::Email::SMTPServer *me = (Net::Email::SMTPServer*)userObj;
	Net::Email::SMTPServer::MailStatus *cliStatus;
	cliStatus = (Net::Email::SMTPServer::MailStatus*)cliData;
	if (me->rawLog)
	{
		me->rawLog->Write(buff, size);
	}
	if (size > 4096)
	{
		MemCopyNO(cliStatus->buff, &buff[size - 4096], 2048);
		cliStatus->buffSize = 4096;
	}
	else if (cliStatus->buffSize + size > 4096)
	{
		MemCopyO(cliStatus->buff, &cliStatus->buff[cliStatus->buffSize - 4096 + size], 4096 - size);
		MemCopyNO(&cliStatus->buff[4096 - size], buff, size);
		cliStatus->buffSize = 4096;
	}
	else
	{
		MemCopyNO(&cliStatus->buff[cliStatus->buffSize], buff, size);
		cliStatus->buffSize += size;
	}
	UOSInt i;
	UOSInt j;
	j = 0;
	i = 0;
	while (i < cliStatus->buffSize)
	{
		if (cliStatus->buff[i] == '\r')
		{
			cliStatus->buff[i] = 0;
			if (j < cliStatus->buffSize && cliStatus->buff[j] == '\n')
			{
				me->ParseCmd(cli, cliStatus, (Char*)&cliStatus->buff[j], Text::LBT_CRLF);
				j = i + 1;
				j++;
				i++;
			}
			else
			{
				me->ParseCmd(cli, cliStatus, (Char*)&cliStatus->buff[j], Text::LBT_CR);
				j = i + 1;
			}
		}
		else if (cliStatus->buff[i] == '\n')
		{
			cliStatus->buff[i] = 0;
			me->ParseCmd(cli, cliStatus, (Char*)&cliStatus->buff[j], Text::LBT_LF);
			j = i + 1;
		}
		i++;
	}
	if (j >= cliStatus->buffSize)
	{
		cliStatus->buffSize = 0;
	}
	else if (j > 0)
	{
		MemCopyO(cliStatus->buff, &cliStatus->buff[j], cliStatus->buffSize - j);
		cliStatus->buffSize -= j;
	}
}

void __stdcall Net::Email::SMTPServer::ClientTimeout(Net::TCPClient *cli, void *userObj, void *cliData)
{
}

UOSInt Net::Email::SMTPServer::WriteMessage(Net::TCPClient *cli, Int32 statusCode, const UTF8Char *msg)
{
	Text::StringBuilderUTF8 sb;
	UOSInt i = 0;
	UOSInt j;
	while (true)
	{
		j = Text::StrIndexOf(&msg[i], '\r');
		if (j != INVALID_INDEX)
		{
			sb.AppendI32(statusCode);
			sb.Append((const UTF8Char *)"-");
			sb.AppendC(&msg[i], (UOSInt)j);
			sb.Append((const UTF8Char *)"\r\n");
			i += j + 2;
		}
		else
		{
			sb.AppendI32(statusCode);
			sb.Append((const UTF8Char *)" ");
			sb.Append(&msg[i]);
			sb.Append((const UTF8Char *)"\r\n");
			break;
		}
	}


	UOSInt buffSize;
	buffSize = cli->Write(sb.ToString(), sb.GetLength());
	if (this->rawLog)
	{
		this->rawLog->Write(sb.ToString(), buffSize);
	}
	return buffSize;
}

/*OSInt Net::SMTPServer::WriteMessage(Net::TCPClient *cli, Int32 statusCode, const Char *msg)
{
	Text::StringBuilderC sb;
	UOSInt i = 0;
	UOSInt j;
	while (true)
	{
		j = Text::StrIndexOf(&msg[i], '\r');
		if (j != INVALID_INDEX)
		{
			sb.Append(statusCode);
			sb.Append("-");
			sb.Append(&msg[i], j);
			sb.Append("\r\n");
			i = j + 2;
		}
		else
		{
			sb.Append(statusCode);
			sb.Append(" ");
			sb.Append(&msg[i]);
			sb.Append("\r\n");
			break;
		}
	}


	OSInt strLen = sb.GetLength();
	strLen = cli->Write((UInt8*)sb.ToString(), strLen);
	return strLen;
}*/

void Net::Email::SMTPServer::ParseCmd(Net::TCPClient *cli, Net::Email::SMTPServer::MailStatus *cliStatus, Char *cmd, Text::LineBreakType lbt)
{
	if (cliStatus->loginMode)
	{
		if (cliStatus->loginMode == 1)
		{
			Bool succ = false;
			UOSInt len = Text::StrCharCnt(cmd);
			UInt8 *decBuff = MemAlloc(UInt8, len);
			const UTF8Char *userName;
			const UTF8Char *pwd;
			Crypto::Encrypt::Base64 b64;
			len = b64.Decrypt((UInt8*)cmd, len, decBuff, 0);
			decBuff[len] = 0;
			userName = (const UTF8Char*)&decBuff[1];
			pwd = userName;
			while (*pwd++);
			if (pwd < (const UTF8Char*)(decBuff + len))
			{
				succ = this->loginHdlr(this->mailObj, userName, pwd);
			}
			MemFree(decBuff);
			cliStatus->loginMode = 0;
			if (succ)
			{
				WriteMessage(cli, 235, (const UTF8Char *)"2.7.0 Authentication successful");
				cliStatus->login = true;
				SDEL_TEXT(cliStatus->userName);
				cliStatus->userName = Text::StrCopyNew(userName);
			}
			else
			{
				WriteMessage(cli, 535, (const UTF8Char *)"authentication failed (#5.7.1)");
			}
		}
		else if (cliStatus->loginMode == 2)
		{
			UOSInt len = Text::StrCharCnt(cmd);
			UInt8 *decBuff = MemAlloc(UInt8, len);
			Crypto::Encrypt::Base64 b64;
			len = b64.Decrypt((UInt8*)cmd, len, decBuff, 0);
			decBuff[len] = 0;
			SDEL_TEXT(cliStatus->userName);
			cliStatus->userName = Text::StrCopyNew((const UTF8Char*)decBuff);
			MemFree(decBuff);
			cliStatus->loginMode = 3;
			WriteMessage(cli, 334, (const UTF8Char *)"UGFzc3dvcmQ6");
		}
		else if (cliStatus->loginMode == 3)
		{
			Bool succ = false;
			UOSInt len = Text::StrCharCnt(cmd);
			UInt8 *decBuff = MemAlloc(UInt8, len);
			Crypto::Encrypt::Base64 b64;
			len = b64.Decrypt((UInt8*)cmd, len, decBuff, 0);
			decBuff[len] = 0;
			succ = this->loginHdlr(this->mailObj, cliStatus->userName, (UTF8Char*)decBuff);
			MemFree(decBuff);
			cliStatus->loginMode = 0;
			if (succ)
			{
				WriteMessage(cli, 235, (const UTF8Char *)"2.7.0 Authentication successful");
				cliStatus->login = true;
			}
			else
			{
				WriteMessage(cli, 535, (const UTF8Char *)"authentication failed (#5.7.1)");
			}
		}
		else
		{
			WriteMessage(cli, 535, (const UTF8Char *)"authentication failed (#5.7.1)");
		}
	}
	else if (cliStatus->dataMode)
	{
		if (Text::StrEquals(cmd, "."))
		{
			UTF8Char sbuff[256];
			cliStatus->dataMode = false;
			if (this->mailHdlr(sbuff, this->mailObj, cli, cliStatus))
			{
				Text::StringBuilderUTF8 sb;
				sb.Append((const UTF8Char *)"Ok: queued as ");
				sb.Append(sbuff);
				WriteMessage(cli, 250, sb.ToString());
			}
			else
			{
			}
		}
		else
		{
			if (cliStatus->dataStm == 0)
			{
				NEW_CLASS(cliStatus->dataStm, IO::MemoryStream((const UTF8Char*)"Net.SMTPServer.ParseCmd"));
			}
			else
			{
				if (cliStatus->lastLBT == Text::LBT_CRLF)
				{
					cliStatus->dataStm->Write((UInt8*)"\r\n", 2);
				}
				else if (cliStatus->lastLBT == Text::LBT_CR)
				{
					cliStatus->dataStm->Write((UInt8*)"\r", 1);
				}
				else if (cliStatus->lastLBT == Text::LBT_LF)
				{
					cliStatus->dataStm->Write((UInt8*)"\n", 1);
				}
			}
			cliStatus->dataStm->Write((UInt8*)cmd, Text::StrCharCnt(cmd));
			cliStatus->lastLBT = lbt;
		}
	}
	else if (Text::StrEquals(cmd, "DATA"))
	{
		if (cliStatus->cliName != 0 && cliStatus->mailFrom != 0 && cliStatus->rcptTo->GetCount() > 0)
		{
			cliStatus->dataMode = true;
			cliStatus->lastLBT = Text::LBT_NONE;
			WriteMessage(cli, 354, (const UTF8Char *)"End data with <CR><LF>.<CR><LF>");
		}
	}
	else if (Text::StrEquals(cmd, "QUIT"))
	{
		WriteMessage(cli, 221, (const UTF8Char *)"Bye");
		cli->ShutdownSend();
	}
	else if (Text::StrStartsWith(cmd, "HELO "))
	{
		if (cliStatus->cliName)
		{
			Text::StrDelNew(cliStatus->cliName);
		}
		cliStatus->cliName = Text::StrCopyNew(&cmd[5]);
		Text::StringBuilderUTF8 sb;
		sb.Append(this->domain);
		sb.Append((const UTF8Char *)" Hello ");
		sb.Append((UTF8Char*)cliStatus->cliName);
		WriteMessage(cli, 250, sb.ToString());
	}
	else if (Text::StrStartsWith(cmd, "EHLO "))
	{
		if (cliStatus->cliName)
		{
			Text::StrDelNew(cliStatus->cliName);
		}
		cliStatus->cliName = Text::StrCopyNew(&cmd[5]);
		Text::StringBuilderUTF8 sb;
		sb.Append(this->domain);
		sb.Append((const UTF8Char *)" Hello ");
		sb.Append((UTF8Char*)cliStatus->cliName);
		sb.Append((const UTF8Char *)"\r\nHELP");
		sb.Append((const UTF8Char *)"\r\n8BITMIME");
		sb.Append((const UTF8Char *)"\r\nPIPELINING");
		sb.Append((const UTF8Char *)"\r\nAUTH LOGIN PLAIN");
		sb.Append((const UTF8Char *)"\r\nSIZE ");
		sb.AppendI32(this->maxMailSize);
		sb.Append((const UTF8Char *)"\r\nOK");
		WriteMessage(cli, 250, sb.ToString());
	}
	else if (Text::StrStartsWith(cmd, "MAIL FROM:"))
	{
		if (cliStatus->mailFrom)
		{
			Text::StrDelNew(cliStatus->mailFrom);
		}
		cliStatus->mailFrom = Text::StrCopyNew((const UTF8Char*)cmd);
		WriteMessage(cli, 250, (const UTF8Char *)"Ok");
	}
	else if (Text::StrStartsWith(cmd, "RCPT TO:"))
	{
		cliStatus->rcptTo->Add(Text::StrCopyNew((const UTF8Char*)cmd));
		WriteMessage(cli, 250, (const UTF8Char *)"Ok");
	}
	else if (Text::StrStartsWith(cmd, "AUTH "))
	{
		if (Text::StrEquals(&cmd[5], "PLAIN"))
		{
			cliStatus->loginMode = 1;
			WriteMessage(cli, 334, (const UTF8Char *)"");
		}
		else if (Text::StrEquals(&cmd[5], "LOGIN"))
		{
			cliStatus->loginMode = 2;
			WriteMessage(cli, 334, (const UTF8Char *)"VXNlcm5hbWU6");
		}
		else if (Text::StrEquals(&cmd[5], "CRAM-MD5"))
		{
			WriteMessage(cli, 504, (const UTF8Char *)"Unrecognized authentication type.");
		}
		else if (Text::StrStartsWithICase(&cmd[5], "PLAIN "))
		{
			Bool succ = false;
			UOSInt len = Text::StrCharCnt(cmd);
			UInt8 *decBuff = MemAlloc(UInt8, len - 10);
			const UTF8Char *userName;
			const UTF8Char *pwd;
			Crypto::Encrypt::Base64 b64;
			len = b64.Decrypt((UInt8*)&cmd[11], len - 11, decBuff, 0);
			decBuff[len] = 0;
			userName = (const UTF8Char*)&decBuff[1];
			pwd = userName;
			while (*pwd++);
			if (pwd < (const UTF8Char*)(decBuff + len))
			{
				succ = this->loginHdlr(this->mailObj, userName, pwd);
			}
			MemFree(decBuff);
			if (succ)
			{
				WriteMessage(cli, 235, (const UTF8Char *)"2.7.0 Authentication successful");
				cliStatus->login = true;
				SDEL_TEXT(cliStatus->userName);
				cliStatus->userName = Text::StrCopyNew(userName);
			}
			else
			{
				WriteMessage(cli, 535, (const UTF8Char *)"authentication failed (#5.7.1)");
			}
		}
		else if (Text::StrStartsWithICase(&cmd[5], "LOGIN "))
		{
			UOSInt len = Text::StrCharCnt(cmd);
			UInt8 *decBuff = MemAlloc(UInt8, len - 10);
			Crypto::Encrypt::Base64 b64;
			len = b64.Decrypt((UInt8*)&cmd[11], len - 11, decBuff, 0);
			decBuff[len] = 0;
			SDEL_TEXT(cliStatus->userName);
			cliStatus->userName = Text::StrCopyNew((UTF8Char*)&decBuff[1]);
			cliStatus->loginMode = 3;
			MemFree(decBuff);
			WriteMessage(cli, 334, (const UTF8Char *)"UGFzc3dvcmQ6");
		}
	}
	else
	{
		cliStatus->dataMode = false;
	}
	
}

Net::Email::SMTPServer::SMTPServer(Net::SocketFactory *sockf, UInt16 port, IO::LogTool *log, const UTF8Char *domain, const UTF8Char *serverName, MailHandler mailHdlr, LoginHandler loginHdlr, void *userObj)
{
	this->sockf = sockf;
	this->log = log;
	this->domain = Text::StrCopyNew(domain);
	this->serverName = Text::StrCopyNew(serverName);
	this->mailHdlr = mailHdlr;
	this->mailObj = userObj;
	this->loginHdlr = loginHdlr;
	this->maxMailSize = 104857600;
	NEW_CLASS(this->cliMgr, Net::TCPClientMgr(60, ClientEvent, ClientData, this, 4, ClientTimeout));
	NEW_CLASS(this->svr, Net::TCPServer(this->sockf, port, log, ConnHdlr, this, 0));
	NEW_CLASS(this->rawLog, IO::FileStream((const UTF8Char*)"SMTPLog.dat", IO::FileStream::FILE_MODE_APPEND, IO::FileStream::FILE_SHARE_DENY_NONE, IO::FileStream::BT_NORMAL));
}

Net::Email::SMTPServer::~SMTPServer()
{
	DEL_CLASS(this->svr);
	DEL_CLASS(this->cliMgr);
	DEL_CLASS(this->rawLog);
	Text::StrDelNew(this->domain);
	Text::StrDelNew(this->serverName);
}

Bool Net::Email::SMTPServer::IsError()
{
	return this->svr->IsV4Error();
}

UInt16 Net::Email::SMTPServer::GetDefaultPort()
{
	return 25;
}
