#include "Stdafx.h"
#include "MyMemory.h"
#include "Crypto/Encrypt/Base64.h"
#include "Net/Email/SMTPServer.h"
#include "Text/Encoding.h"
#include "Text/MyString.h"
#include "Text/StringBuilderUTF8.h"

void __stdcall Net::Email::SMTPServer::ClientReady(NotNullPtr<Net::TCPClient> cli, void *userObj)
{
	Net::Email::SMTPServer *me = (Net::Email::SMTPServer*)userObj;
	MailStatus *cliStatus;
	NEW_CLASS(cliStatus, MailStatus());
	cliStatus->buffSize = 0;
	cliStatus->cliName = 0;
	cliStatus->mailFrom = 0;
	cliStatus->dataStm = 0;
	cliStatus->dataMode = false;
	cliStatus->loginMode = 0;
	cliStatus->login = false;
	cliStatus->userName = 0;
	me->cliMgr.AddClient(cli, cliStatus);

	if (me->connType != Net::Email::SMTPConn::ConnType::STARTTLS || !cli->IsSSL())
	{
		Text::StringBuilderUTF8 sb;
		sb.Append(me->domain);
		sb.AppendC(UTF8STRC(" ESMTP"));
	//	sb.Append(me->serverName);
		me->WriteMessage(cli, 220, sb.ToCString());
	}
}

void __stdcall Net::Email::SMTPServer::ConnHdlr(Socket *s, void *userObj)
{
	Net::Email::SMTPServer *me = (Net::Email::SMTPServer*)userObj;
	NotNullPtr<Net::TCPClient> cli;
	NotNullPtr<Net::SSLEngine> ssl;
	if (me->connType == Net::Email::SMTPConn::ConnType::SSL && me->ssl.SetTo(ssl))
	{
		ssl->ServerInit(s, ClientReady, me);
	}
	else
	{
		NEW_CLASSNN(cli, Net::TCPClient(me->sockf, s));
		ClientReady(cli, me);
	}
}

void __stdcall Net::Email::SMTPServer::ClientEvent(NotNullPtr<Net::TCPClient> cli, void *userObj, void *cliData, Net::TCPClientMgr::TCPEventType evtType)
{
	if (evtType == Net::TCPClientMgr::TCP_EVENT_DISCONNECT)
	{
		MailStatus *cliStatus;
		UOSInt i;
		cliStatus = (MailStatus*)cliData;
		SDEL_STRING(cliStatus->cliName);
		SDEL_STRING(cliStatus->mailFrom);
		i = cliStatus->rcptTo.GetCount();
		while (i-- > 0)
		{
			cliStatus->rcptTo.GetItem(i)->Release();
		}
		if (cliStatus->dataStm)
		{
			DEL_CLASS(cliStatus->dataStm);
		}
		DEL_CLASS(cliStatus);
		cli.Delete();
	}
	else if (evtType == Net::TCPClientMgr::TCP_EVENT_HASDATA)
	{
	}
}

void __stdcall Net::Email::SMTPServer::ClientData(NotNullPtr<Net::TCPClient> cli, void *userObj, void *cliData, const Data::ByteArrayR &srcBuff)
{
	Net::Email::SMTPServer *me = (Net::Email::SMTPServer*)userObj;
	NotNullPtr<Net::Email::SMTPServer::MailStatus> cliStatus;
	Data::ByteArrayR buff = srcBuff;
	if (!cliStatus.Set((Net::Email::SMTPServer::MailStatus*)cliData))
		return;
	while (buff.GetSize() > 0)
	{
		if (cliStatus->buffSize + buff.GetSize() > 4096)
		{
			if (me->rawLog)
			{
				me->rawLog->Write(buff.Ptr(), 4096 - cliStatus->buffSize);
			}
			MemCopyNO(&cliStatus->buff[cliStatus->buffSize], buff.Ptr(), 4096 - cliStatus->buffSize);
			buff += 4096 - cliStatus->buffSize;
			cliStatus->buffSize = 4096;
		}
		else
		{
			if (me->rawLog)
			{
				me->rawLog->Write(buff.Ptr(), buff.GetSize());
			}
			MemCopyNO(&cliStatus->buff[cliStatus->buffSize], buff.Ptr(), buff.GetSize());
			cliStatus->buffSize += buff.GetSize();
			buff += buff.GetSize();
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
		else if (cliStatus->buffSize == 4096)
		{
			if (cliStatus->dataMode)
			{
				me->ParseCmd(cli, cliStatus, &cliStatus->buff[0], 4095, Text::LineBreakType::None);
			}
			cliStatus->buff[0] = cliStatus->buff[4095];
			cliStatus->buffSize = 1;
		}
	}
}

void __stdcall Net::Email::SMTPServer::ClientTimeout(NotNullPtr<Net::TCPClient> cli, void *userObj, void *cliData)
{
}

UOSInt Net::Email::SMTPServer::WriteMessage(NotNullPtr<Net::TCPClient> cli, Int32 statusCode, Text::CString msg)
{
	Text::StringBuilderUTF8 sb;
	UOSInt i = 0;
	UOSInt j;
	while (true)
	{
		j = Text::StrIndexOfCharC(&msg.v[i], msg.leng - i, '\r');
		if (j != INVALID_INDEX)
		{
			sb.AppendI32(statusCode);
			sb.AppendC(UTF8STRC("-"));
			sb.AppendC(&msg.v[i], (UOSInt)j);
			sb.AppendC(UTF8STRC("\r\n"));
			i += j + 2;
		}
		else
		{
			sb.AppendI32(statusCode);
			sb.AppendC(UTF8STRC(" "));
			sb.AppendC(&msg.v[i], msg.leng - i);
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

void Net::Email::SMTPServer::ParseCmd(NotNullPtr<Net::TCPClient> cli, NotNullPtr<Net::Email::SMTPServer::MailStatus> cliStatus, const UTF8Char *cmd, UOSInt cmdLen, Text::LineBreakType lbt)
{
	if (cliStatus->loginMode)
	{
		if (cliStatus->loginMode == 1)
		{
			Bool succ = false;
			UInt8 *decBuff = MemAlloc(UInt8, cmdLen);
			Text::CString userName;
			Text::CString pwd;
			Crypto::Encrypt::Base64 b64;
			cmdLen = b64.Decrypt(cmd, cmdLen, decBuff, 0);
			decBuff[cmdLen] = 0;
			userName.v = (const UTF8Char*)&decBuff[1];
			userName.leng = Text::StrCharCnt(userName.v);
			pwd.v = userName.v + userName.leng + 1;
			if (pwd.v < (const UTF8Char*)(decBuff + cmdLen))
			{
				pwd.leng = (UOSInt)(decBuff + cmdLen - pwd.v);
				succ = this->loginHdlr(this->mailObj, userName, pwd);
			}
			MemFree(decBuff);
			cliStatus->loginMode = 0;
			if (succ)
			{
				WriteMessage(cli, 235, CSTR("2.7.0 Authentication successful"));
				cliStatus->login = true;
				SDEL_STRING(cliStatus->userName);
				cliStatus->userName = Text::String::New(userName).Ptr();
			}
			else
			{
				WriteMessage(cli, 535, CSTR("authentication failed (#5.7.1)"));
			}
		}
		else if (cliStatus->loginMode == 2)
		{
			UInt8 *decBuff = MemAlloc(UInt8, cmdLen);
			Crypto::Encrypt::Base64 b64;
			cmdLen = b64.Decrypt(cmd, cmdLen, decBuff, 0);
			decBuff[cmdLen] = 0;
			SDEL_STRING(cliStatus->userName);
			cliStatus->userName = Text::String::New(decBuff, cmdLen).Ptr();
			MemFree(decBuff);
			cliStatus->loginMode = 3;
			WriteMessage(cli, 334, CSTR("UGFzc3dvcmQ6"));
		}
		else if (cliStatus->loginMode == 3)
		{
			Bool succ = false;
			UInt8 *decBuff = MemAlloc(UInt8, cmdLen);
			Crypto::Encrypt::Base64 b64;
			cmdLen = b64.Decrypt(cmd, cmdLen, decBuff, 0);
			decBuff[cmdLen] = 0;
			succ = this->loginHdlr(this->mailObj, cliStatus->userName->ToCString(), {decBuff, cmdLen});
			MemFree(decBuff);
			cliStatus->loginMode = 0;
			if (succ)
			{
				WriteMessage(cli, 235, CSTR("2.7.0 Authentication successful"));
				cliStatus->login = true;
			}
			else
			{
				WriteMessage(cli, 535, CSTR("authentication failed (#5.7.1)"));
			}
		}
		else
		{
			WriteMessage(cli, 535, CSTR("authentication failed (#5.7.1)"));
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
				WriteMessage(cli, 250, sb.ToCString());
			}
			else
			{
			}
		}
		else
		{
			if (cliStatus->dataStm == 0)
			{
				NEW_CLASS(cliStatus->dataStm, IO::MemoryStream());
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
		if (cliStatus->cliName != 0 && cliStatus->mailFrom != 0 && cliStatus->rcptTo.GetCount() > 0)
		{
			cliStatus->dataMode = true;
			cliStatus->lastLBT = Text::LineBreakType::None;
			WriteMessage(cli, 354, CSTR("End data with <CR><LF>.<CR><LF>"));
		}
	}
	else if (Text::StrEqualsC(cmd, cmdLen, UTF8STRC("QUIT")))
	{
		WriteMessage(cli, 221, CSTR("Bye"));
		cli->ShutdownSend();
	}
	else if (Text::StrEqualsC(cmd, cmdLen, UTF8STRC("STARTTLS")))
	{
		NotNullPtr<Net::SSLEngine> ssl;
		if (!cli->IsSSL() && this->connType == Net::Email::SMTPConn::ConnType::STARTTLS && this->ssl.SetTo(ssl))
		{
			WriteMessage(cli, 220, CSTR("Go ahead"));
			ssl->ServerInit(cli->RemoveSocket(), ClientReady, this);;
		}
	}
	else if (Text::StrStartsWithC(cmd, cmdLen, UTF8STRC("HELO ")))
	{
		SDEL_STRING(cliStatus->cliName);
		cliStatus->cliName = Text::String::New(&cmd[5], cmdLen - 5).Ptr();
		Text::StringBuilderUTF8 sb;
		sb.Append(this->domain);
		sb.AppendC(UTF8STRC(" Hello "));
		sb.Append(cliStatus->cliName);
		WriteMessage(cli, 250, sb.ToCString());
	}
	else if (Text::StrStartsWithC(cmd, cmdLen, UTF8STRC("EHLO ")))
	{
		SDEL_STRING(cliStatus->cliName);
		cliStatus->cliName = Text::String::New(&cmd[5], cmdLen - 5).Ptr();
		Text::StringBuilderUTF8 sb;
		sb.Append(this->domain);
		sb.AppendC(UTF8STRC(" Hello "));
		sb.Append(cliStatus->cliName);
		sb.AppendC(UTF8STRC("\r\nHELP"));
		sb.AppendC(UTF8STRC("\r\n8BITMIME"));
		if (this->connType == Net::Email::SMTPConn::ConnType::STARTTLS && !cli->IsSSL())
		{
			sb.AppendC(UTF8STRC("\r\nSTARTTLS"));
		}
		sb.AppendC(UTF8STRC("\r\nPIPELINING"));
		sb.AppendC(UTF8STRC("\r\nAUTH LOGIN PLAIN"));
		sb.AppendC(UTF8STRC("\r\nSIZE "));
		sb.AppendU32(this->maxMailSize);
		sb.AppendC(UTF8STRC("\r\nOK"));
		WriteMessage(cli, 250, sb.ToCString());
	}
	else if (Text::StrStartsWithC(cmd, cmdLen, UTF8STRC("MAIL FROM:")))
	{
		SDEL_STRING(cliStatus->mailFrom);
		cliStatus->mailFrom = Text::String::New(cmd, cmdLen).Ptr();
		WriteMessage(cli, 250, CSTR("Ok"));
	}
	else if (Text::StrStartsWithC(cmd, cmdLen, UTF8STRC("RCPT TO:")))
	{
		cliStatus->rcptTo.Add(Text::String::New(cmd, cmdLen).Ptr());
		WriteMessage(cli, 250, CSTR("Ok"));
	}
	else if (Text::StrStartsWithC(cmd, cmdLen, UTF8STRC("AUTH ")))
	{
		if (Text::StrEqualsC(&cmd[5], cmdLen - 5, UTF8STRC("PLAIN")))
		{
			cliStatus->loginMode = 1;
			WriteMessage(cli, 334, CSTR(""));
		}
		else if (Text::StrEqualsC(&cmd[5], cmdLen - 5, UTF8STRC("LOGIN")))
		{
			cliStatus->loginMode = 2;
			WriteMessage(cli, 334, CSTR("VXNlcm5hbWU6"));
		}
		else if (Text::StrEqualsC(&cmd[5], cmdLen - 5, UTF8STRC("CRAM-MD5")))
		{
			WriteMessage(cli, 504, CSTR("Unrecognized authentication type."));
		}
		else if (Text::StrStartsWithICaseC(&cmd[5], cmdLen - 5, UTF8STRC("PLAIN ")))
		{
			Bool succ = false;
			UInt8 *decBuff = MemAlloc(UInt8, cmdLen - 10);
			Text::CString userName;
			Text::CString pwd;
			Crypto::Encrypt::Base64 b64;
			cmdLen = b64.Decrypt((UInt8*)&cmd[11], cmdLen - 11, decBuff, 0);
			decBuff[cmdLen] = 0;
			userName.v = &decBuff[1];
			userName.leng = Text::StrCharCnt(userName.v);
			pwd.v = userName.v + userName.leng + 1;
			if (pwd.v < (const UTF8Char*)(decBuff + cmdLen))
			{
				pwd.leng = (UOSInt)(decBuff + cmdLen - pwd.v);
				succ = this->loginHdlr(this->mailObj, userName, pwd);
			}
			MemFree(decBuff);
			if (succ)
			{
				WriteMessage(cli, 235, CSTR("2.7.0 Authentication successful"));
				cliStatus->login = true;
				SDEL_STRING(cliStatus->userName);
				cliStatus->userName = Text::String::New(userName).Ptr();
			}
			else
			{
				WriteMessage(cli, 535, CSTR("authentication failed (#5.7.1)"));
			}
		}
		else if (Text::StrStartsWithICaseC(&cmd[5], cmdLen - 5, UTF8STRC("LOGIN ")))
		{
			UInt8 *decBuff = MemAlloc(UInt8, cmdLen - 10);
			Crypto::Encrypt::Base64 b64;
			cmdLen = b64.Decrypt((UInt8*)&cmd[11], cmdLen - 11, decBuff, 0);
			decBuff[cmdLen] = 0;
			SDEL_STRING(cliStatus->userName);
			cliStatus->userName = Text::String::New((UTF8Char*)&decBuff[1], cmdLen - 1).Ptr();
			cliStatus->loginMode = 3;
			MemFree(decBuff);
			WriteMessage(cli, 334, CSTR("UGFzc3dvcmQ6"));
		}
	}
	else
	{
		cliStatus->dataMode = false;
	}
	
}

Net::Email::SMTPServer::SMTPServer(NotNullPtr<Net::SocketFactory> sockf, Optional<Net::SSLEngine> ssl, UInt16 port, Net::Email::SMTPConn::ConnType connType, NotNullPtr<IO::LogTool> log, Text::CStringNN domain, Text::CStringNN serverName, MailHandler mailHdlr, LoginHandler loginHdlr, void *userObj, Bool autoStart) : cliMgr(60, ClientEvent, ClientData, this, 4, ClientTimeout)
{
	this->sockf = sockf;
	this->ssl = ssl;
	this->connType = connType;
	this->log = log;
	this->domain = Text::String::New(domain);
	this->serverName = Text::String::New(serverName);
	this->mailHdlr = mailHdlr;
	this->mailObj = userObj;
	this->loginHdlr = loginHdlr;
	this->maxMailSize = 104857600;
	NEW_CLASS(this->svr, Net::TCPServer(this->sockf, 0, port, log, ConnHdlr, this, CSTR_NULL, autoStart));
	NEW_CLASS(this->rawLog, IO::FileStream(CSTR("SMTPLog.dat"), IO::FileMode::Append, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal));
}

Net::Email::SMTPServer::~SMTPServer()
{
	DEL_CLASS(this->svr);
	this->cliMgr.CloseAll();
	DEL_CLASS(this->rawLog);
	this->domain->Release();
	this->serverName->Release();
}

Bool Net::Email::SMTPServer::Start()
{
	return this->svr->Start();
}

Bool Net::Email::SMTPServer::IsError()
{
	return this->svr->IsV4Error();
}

UInt16 Net::Email::SMTPServer::GetDefaultPort(Net::Email::SMTPConn::ConnType connType)
{
	switch (connType)
	{
	case Net::Email::SMTPConn::ConnType::Plain:
		return 25;
	case Net::Email::SMTPConn::ConnType::STARTTLS:
		return 587;
	case Net::Email::SMTPConn::ConnType::SSL:
		return 465;
	default:
		return 25;
	}
}
