#include "Stdafx.h"
#include "MyMemory.h"
#include "Crypto/Encrypt/Base64.h"
#include "Net/Email/SMTPServer.h"
#include "Text/Encoding.h"
#include "Text/MyString.h"
#include "Text/StringBuilderUTF8.h"

void __stdcall Net::Email::SMTPServer::ClientReady(Net::TCPClient *cli, void *userObj)
{
	Net::Email::SMTPServer *me = (Net::Email::SMTPServer*)userObj;
	MailStatus *cliStatus;
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

	if (me->connType != Net::Email::SMTPConn::CT_STARTTLS || !cli->IsSSL())
	{
		Text::StringBuilderUTF8 sb;
		sb.Append(me->domain);
		sb.AppendC(UTF8STRC(" ESMTP"));
	//	sb.Append(me->serverName);
		me->WriteMessage(cli, 220, sb.ToString());
	}
}

void __stdcall Net::Email::SMTPServer::ConnHdlr(Socket *s, void *userObj)
{
	Net::Email::SMTPServer *me = (Net::Email::SMTPServer*)userObj;
	Net::TCPClient *cli;
	if (me->connType == Net::Email::SMTPConn::CT_SSL)
	{
		me->ssl->ServerInit(s, ClientReady, me);
	}
	else
	{
		NEW_CLASS(cli, Net::TCPClient(me->sockf, s));
		ClientReady(cli, me);
	}
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
			if (i + 1 < cliStatus->buffSize && cliStatus->buff[i + 1] == '\n')
			{
				me->ParseCmd(cli, cliStatus, &cliStatus->buff[j], i - j, Text::LineBreakType::CRLF);
				j = i + 2;
				i++;
			}
			else
			{
				me->ParseCmd(cli, cliStatus, &cliStatus->buff[j], i - j, Text::LineBreakType::CR);
				j = i + 1;
			}
		}
		else if (cliStatus->buff[i] == '\n')
		{
			cliStatus->buff[i] = 0;
			me->ParseCmd(cli, cliStatus, &cliStatus->buff[j], i - j, Text::LineBreakType::LF);
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
		j = Text::StrIndexOfChar(&msg[i], '\r');
		if (j != INVALID_INDEX)
		{
			sb.AppendI32(statusCode);
			sb.AppendC(UTF8STRC("-"));
			sb.AppendC(&msg[i], (UOSInt)j);
			sb.AppendC(UTF8STRC("\r\n"));
			i += j + 2;
		}
		else
		{
			sb.AppendI32(statusCode);
			sb.AppendC(UTF8STRC(" "));
			sb.AppendSlow(&msg[i]);
			sb.AppendC(UTF8STRC("\r\n"));
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
		j = Text::StrIndexOfChar(&msg[i], '\r');
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

void Net::Email::SMTPServer::ParseCmd(Net::TCPClient *cli, Net::Email::SMTPServer::MailStatus *cliStatus, const UTF8Char *cmd, UOSInt cmdLen, Text::LineBreakType lbt)
{
	if (cliStatus->loginMode)
	{
		if (cliStatus->loginMode == 1)
		{
			Bool succ = false;
			UInt8 *decBuff = MemAlloc(UInt8, cmdLen);
			const UTF8Char *userName;
			const UTF8Char *pwd;
			Crypto::Encrypt::Base64 b64;
			cmdLen = b64.Decrypt(cmd, cmdLen, decBuff, 0);
			decBuff[cmdLen] = 0;
			userName = (const UTF8Char*)&decBuff[1];
			pwd = userName;
			while (*pwd++);
			if (pwd < (const UTF8Char*)(decBuff + cmdLen))
			{
				succ = this->loginHdlr(this->mailObj, userName, pwd);
			}
			MemFree(decBuff);
			cliStatus->loginMode = 0;
			if (succ)
			{
				WriteMessage(cli, 235, (const UTF8Char*)"2.7.0 Authentication successful");
				cliStatus->login = true;
				SDEL_TEXT(cliStatus->userName);
				cliStatus->userName = Text::StrCopyNew(userName);
			}
			else
			{
				WriteMessage(cli, 535, (const UTF8Char*)"authentication failed (#5.7.1)");
			}
		}
		else if (cliStatus->loginMode == 2)
		{
			UInt8 *decBuff = MemAlloc(UInt8, cmdLen);
			Crypto::Encrypt::Base64 b64;
			cmdLen = b64.Decrypt(cmd, cmdLen, decBuff, 0);
			decBuff[cmdLen] = 0;
			SDEL_TEXT(cliStatus->userName);
			cliStatus->userName = Text::StrCopyNewC(decBuff, cmdLen);
			MemFree(decBuff);
			cliStatus->loginMode = 3;
			WriteMessage(cli, 334, (const UTF8Char*)"UGFzc3dvcmQ6");
		}
		else if (cliStatus->loginMode == 3)
		{
			Bool succ = false;
			UInt8 *decBuff = MemAlloc(UInt8, cmdLen);
			Crypto::Encrypt::Base64 b64;
			cmdLen = b64.Decrypt(cmd, cmdLen, decBuff, 0);
			decBuff[cmdLen] = 0;
			succ = this->loginHdlr(this->mailObj, cliStatus->userName, (UTF8Char*)decBuff);
			MemFree(decBuff);
			cliStatus->loginMode = 0;
			if (succ)
			{
				WriteMessage(cli, 235, (const UTF8Char*)"2.7.0 Authentication successful");
				cliStatus->login = true;
			}
			else
			{
				WriteMessage(cli, 535, (const UTF8Char*)"authentication failed (#5.7.1)");
			}
		}
		else
		{
			WriteMessage(cli, 535, (const UTF8Char*)"authentication failed (#5.7.1)");
		}
	}
	else if (cliStatus->dataMode)
	{
		if (Text::StrEqualsC(cmd, cmdLen, UTF8STRC(".")))
		{
			UTF8Char sbuff[256];
			UTF8Char *sptr;
			cliStatus->dataMode = false;
			if ((sptr = this->mailHdlr(sbuff, this->mailObj, cli, cliStatus)) != 0)
			{
				Text::StringBuilderUTF8 sb;
				sb.AppendC(UTF8STRC("Ok: queued as "));
				sb.AppendP(sbuff, sptr);
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
				NEW_CLASS(cliStatus->dataStm, IO::MemoryStream(UTF8STRC("Net.SMTPServer.ParseCmd")));
			}
			else
			{
				if (cliStatus->lastLBT == Text::LineBreakType::CRLF)
				{
					cliStatus->dataStm->Write(UTF8STRC("\r\n"));
				}
				else if (cliStatus->lastLBT == Text::LineBreakType::CR)
				{
					cliStatus->dataStm->Write(UTF8STRC("\r"));
				}
				else if (cliStatus->lastLBT == Text::LineBreakType::LF)
				{
					cliStatus->dataStm->Write(UTF8STRC("\n"));
				}
			}
			cliStatus->dataStm->Write(cmd, cmdLen);
			cliStatus->lastLBT = lbt;
		}
	}
	else if (Text::StrEqualsC(cmd, cmdLen, UTF8STRC("DATA")))
	{
		if (cliStatus->cliName != 0 && cliStatus->mailFrom != 0 && cliStatus->rcptTo->GetCount() > 0)
		{
			cliStatus->dataMode = true;
			cliStatus->lastLBT = Text::LineBreakType::None;
			WriteMessage(cli, 354, (const UTF8Char*)"End data with <CR><LF>.<CR><LF>");
		}
	}
	else if (Text::StrEqualsC(cmd, cmdLen, UTF8STRC("QUIT")))
	{
		WriteMessage(cli, 221, (const UTF8Char*)"Bye");
		cli->ShutdownSend();
	}
	else if (Text::StrEqualsC(cmd, cmdLen, UTF8STRC("STARTTLS")))
	{
		if (!cli->IsSSL() && this->connType == Net::Email::SMTPConn::CT_STARTTLS)
		{
			WriteMessage(cli, 220, (const UTF8Char*)"Go ahead");
			this->ssl->ServerInit(cli->RemoveSocket(), ClientReady, this);;
		}
	}
	else if (Text::StrStartsWithC(cmd, cmdLen, UTF8STRC("HELO ")))
	{
		if (cliStatus->cliName)
		{
			Text::StrDelNew(cliStatus->cliName);
		}
		cliStatus->cliName = Text::StrCopyNewC(&cmd[5], cmdLen - 5);
		Text::StringBuilderUTF8 sb;
		sb.Append(this->domain);
		sb.AppendC(UTF8STRC(" Hello "));
		sb.AppendSlow(cliStatus->cliName);
		WriteMessage(cli, 250, sb.ToString());
	}
	else if (Text::StrStartsWithC(cmd, cmdLen, UTF8STRC("EHLO ")))
	{
		if (cliStatus->cliName)
		{
			Text::StrDelNew(cliStatus->cliName);
		}
		cliStatus->cliName = Text::StrCopyNewC(&cmd[5], cmdLen - 5);
		Text::StringBuilderUTF8 sb;
		sb.Append(this->domain);
		sb.AppendC(UTF8STRC(" Hello "));
		sb.AppendSlow(cliStatus->cliName);
		sb.AppendC(UTF8STRC("\r\nHELP"));
		sb.AppendC(UTF8STRC("\r\n8BITMIME"));
		if (this->connType == Net::Email::SMTPConn::CT_STARTTLS && !cli->IsSSL())
		{
			sb.AppendC(UTF8STRC("\r\nSTARTTLS"));
		}
		sb.AppendC(UTF8STRC("\r\nPIPELINING"));
		sb.AppendC(UTF8STRC("\r\nAUTH LOGIN PLAIN"));
		sb.AppendC(UTF8STRC("\r\nSIZE "));
		sb.AppendU32(this->maxMailSize);
		sb.AppendC(UTF8STRC("\r\nOK"));
		WriteMessage(cli, 250, sb.ToString());
	}
	else if (Text::StrStartsWithC(cmd, cmdLen, UTF8STRC("MAIL FROM:")))
	{
		if (cliStatus->mailFrom)
		{
			Text::StrDelNew(cliStatus->mailFrom);
		}
		cliStatus->mailFrom = Text::StrCopyNew((const UTF8Char*)cmd);
		WriteMessage(cli, 250, (const UTF8Char*)"Ok");
	}
	else if (Text::StrStartsWithC(cmd, cmdLen, UTF8STRC("RCPT TO:")))
	{
		cliStatus->rcptTo->Add(Text::StrCopyNew((const UTF8Char*)cmd));
		WriteMessage(cli, 250, (const UTF8Char*)"Ok");
	}
	else if (Text::StrStartsWithC(cmd, cmdLen, UTF8STRC("AUTH ")))
	{
		if (Text::StrEqualsC(&cmd[5], cmdLen - 5, UTF8STRC("PLAIN")))
		{
			cliStatus->loginMode = 1;
			WriteMessage(cli, 334, (const UTF8Char*)"");
		}
		else if (Text::StrEqualsC(&cmd[5], cmdLen - 5, UTF8STRC("LOGIN")))
		{
			cliStatus->loginMode = 2;
			WriteMessage(cli, 334, (const UTF8Char*)"VXNlcm5hbWU6");
		}
		else if (Text::StrEqualsC(&cmd[5], cmdLen - 5, UTF8STRC("CRAM-MD5")))
		{
			WriteMessage(cli, 504, (const UTF8Char*)"Unrecognized authentication type.");
		}
		else if (Text::StrStartsWithICaseC(&cmd[5], cmdLen - 5, UTF8STRC("PLAIN ")))
		{
			Bool succ = false;
			UInt8 *decBuff = MemAlloc(UInt8, cmdLen - 10);
			const UTF8Char *userName;
			const UTF8Char *pwd;
			Crypto::Encrypt::Base64 b64;
			cmdLen = b64.Decrypt((UInt8*)&cmd[11], cmdLen - 11, decBuff, 0);
			decBuff[cmdLen] = 0;
			userName = (const UTF8Char*)&decBuff[1];
			pwd = userName;
			while (*pwd++);
			if (pwd < (const UTF8Char*)(decBuff + cmdLen))
			{
				succ = this->loginHdlr(this->mailObj, userName, pwd);
			}
			MemFree(decBuff);
			if (succ)
			{
				WriteMessage(cli, 235, (const UTF8Char*)"2.7.0 Authentication successful");
				cliStatus->login = true;
				SDEL_TEXT(cliStatus->userName);
				cliStatus->userName = Text::StrCopyNew(userName);
			}
			else
			{
				WriteMessage(cli, 535, (const UTF8Char*)"authentication failed (#5.7.1)");
			}
		}
		else if (Text::StrStartsWithICaseC(&cmd[5], cmdLen - 5, UTF8STRC("LOGIN ")))
		{
			UInt8 *decBuff = MemAlloc(UInt8, cmdLen - 10);
			Crypto::Encrypt::Base64 b64;
			cmdLen = b64.Decrypt((UInt8*)&cmd[11], cmdLen - 11, decBuff, 0);
			decBuff[cmdLen] = 0;
			SDEL_TEXT(cliStatus->userName);
			cliStatus->userName = Text::StrCopyNew((UTF8Char*)&decBuff[1]);
			cliStatus->loginMode = 3;
			MemFree(decBuff);
			WriteMessage(cli, 334, (const UTF8Char*)"UGFzc3dvcmQ6");
		}
	}
	else
	{
		cliStatus->dataMode = false;
	}
	
}

Net::Email::SMTPServer::SMTPServer(Net::SocketFactory *sockf, Net::SSLEngine *ssl, UInt16 port, Net::Email::SMTPConn::ConnType connType, IO::LogTool *log, const UTF8Char *domain, const UTF8Char *serverName, MailHandler mailHdlr, LoginHandler loginHdlr, void *userObj)
{
	this->sockf = sockf;
	this->ssl = ssl;
	this->connType = connType;
	this->log = log;
	this->domain = Text::String::NewNotNull(domain);
	this->serverName = Text::StrCopyNew(serverName);
	this->mailHdlr = mailHdlr;
	this->mailObj = userObj;
	this->loginHdlr = loginHdlr;
	this->maxMailSize = 104857600;
	NEW_CLASS(this->cliMgr, Net::TCPClientMgr(60, ClientEvent, ClientData, this, 4, ClientTimeout));
	NEW_CLASS(this->svr, Net::TCPServer(this->sockf, port, log, ConnHdlr, this, 0));
	NEW_CLASS(this->rawLog, IO::FileStream(CSTR("SMTPLog.dat"), IO::FileMode::Append, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal));
}

Net::Email::SMTPServer::~SMTPServer()
{
	DEL_CLASS(this->svr);
	DEL_CLASS(this->cliMgr);
	DEL_CLASS(this->rawLog);
	this->domain->Release();
	Text::StrDelNew(this->serverName);
}

Bool Net::Email::SMTPServer::IsError()
{
	return this->svr->IsV4Error();
}

UInt16 Net::Email::SMTPServer::GetDefaultPort(Net::Email::SMTPConn::ConnType connType)
{
	switch (connType)
	{
	case Net::Email::SMTPConn::CT_PLAIN:
		return 25;
	case Net::Email::SMTPConn::CT_STARTTLS:
		return 587;
	case Net::Email::SMTPConn::CT_SSL:
		return 465;
	default:
		return 25;
	}
}
