#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ArrayListUInt32.h"
#include "Net/Email/MailController.h"
#include "Net/Email/POP3Server.h"
#include "Text/Encoding.h"
#include "Text/MyString.h"
#include "Text/StringBuilderUTF8.h"
#include "Text/UTF8Reader.h"

#define VERBOSE
#if defined(VERBOSE)
#include <stdio.h>
#endif

void __stdcall Net::Email::POP3Server::ConnReady(NotNullPtr<Net::TCPClient> cli, AnyType userObj)
{
	NotNullPtr<Net::Email::POP3Server> me = userObj.GetNN<Net::Email::POP3Server>();
#if defined(VERBOSE)
	printf("POP3Server: %lld, ConnReady\r\n", cli->GetCliId());
#endif
	MailStatus *cliStatus;
	NEW_CLASS(cliStatus, MailStatus());
	cliStatus->buffSize = 0;
	cliStatus->cliName = 0;
	cliStatus->userName = 0;
	cliStatus->userId = 0;
	cliStatus->dataStm = 0;
	cliStatus->dataMode = false;
	me->cliMgr.AddClient(cli, cliStatus);

	me->WriteMessage(cli, true, me->greeting->ToCString());
}

void __stdcall Net::Email::POP3Server::ConnHdlr(Socket *s, AnyType userObj)
{
	NotNullPtr<Net::Email::POP3Server> me = userObj.GetNN<Net::Email::POP3Server>();
	NotNullPtr<Net::SSLEngine> ssl;
	if (me->sslConn && me->ssl.SetTo(ssl))
	{
		ssl->ServerInit(s, ConnReady, me);
	}
	else
	{
		NotNullPtr<Net::TCPClient> cli;
		NEW_CLASSNN(cli, Net::TCPClient(me->sockf, s));
		ConnReady(cli, me);
	}
}

void __stdcall Net::Email::POP3Server::ClientEvent(NotNullPtr<Net::TCPClient> cli, AnyType userObj, AnyType cliData, Net::TCPClientMgr::TCPEventType evtType)
{
	if (evtType == Net::TCPClientMgr::TCP_EVENT_DISCONNECT)
	{
		NotNullPtr<MailStatus> cliStatus = cliData.GetNN<MailStatus>();
		UOSInt i;
		if (cliStatus->cliName)
		{
			Text::StrDelNew(cliStatus->cliName);
		}
		SDEL_STRING(cliStatus->userName);
		i = cliStatus->rcptTo.GetCount();
		while (i-- > 0)
		{
			Text::StrDelNew(cliStatus->rcptTo.GetItem(i));
		}
		if (cliStatus->dataStm)
		{
			DEL_CLASS(cliStatus->dataStm);
		}
		cliStatus.Delete();
		cli.Delete();
	}
	else if (evtType == Net::TCPClientMgr::TCP_EVENT_HASDATA)
	{
	}
}

void __stdcall Net::Email::POP3Server::ClientData(NotNullPtr<Net::TCPClient> cli, AnyType userObj, AnyType cliData, const Data::ByteArrayR &buff)
{
	NotNullPtr<Net::Email::POP3Server> me = userObj.GetNN<Net::Email::POP3Server>();
	NotNullPtr<MailStatus> cliStatus = cliData.GetNN<MailStatus>();
	if (me->rawLog)
	{
		me->rawLog->Write(buff.Ptr(), buff.GetSize());
	}
	if (buff.GetSize() > 2048)
	{
		MemCopyNO(cliStatus->buff, &buff[buff.GetSize() - 2048], 2048);
		cliStatus->buffSize = 2048;
	}
	else if (cliStatus->buffSize + buff.GetSize() > 2048)
	{
		MemCopyO(cliStatus->buff, &cliStatus->buff[cliStatus->buffSize - 2048 + buff.GetSize()], 2048 - buff.GetSize());
		MemCopyNO(&cliStatus->buff[2048 - buff.GetSize()], buff.Ptr(), buff.GetSize());
		cliStatus->buffSize = 2048;
	}
	else
	{
		MemCopyNO(&cliStatus->buff[cliStatus->buffSize], buff.Ptr(), buff.GetSize());
		cliStatus->buffSize += buff.GetSize();
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
			me->ParseCmd(cli, cliStatus, &cliStatus->buff[j], i - j);
			j = i + 1;
			if (j < cliStatus->buffSize && cliStatus->buff[j] == '\n')
			{
				j++;
				i++;
			}
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

void __stdcall Net::Email::POP3Server::ClientTimeout(NotNullPtr<Net::TCPClient> cli, AnyType userObj, AnyType cliData)
{
}

UOSInt Net::Email::POP3Server::WriteMessage(NotNullPtr<Net::TCPClient> cli, Bool success, Text::CString msg)
{
	Text::StringBuilderUTF8 sb;
	if (success)
	{
		sb.AppendC(UTF8STRC("+OK"));
	}
	else
	{
		sb.AppendC(UTF8STRC("-ERR"));
	}
	if (msg.v)
	{
		sb.AppendC(UTF8STRC(" "));
		sb.Append(msg);
	}
	sb.AppendC(UTF8STRC("\r\n"));
#if defined(VERBOSE)
	printf("%s", sb.ToString());
#endif

	UOSInt buffSize;
	buffSize = cli->Write(sb.ToString(), sb.GetLength());
	if (this->rawLog)
	{
		this->rawLog->Write(sb.ToString(), buffSize);
	}
	return buffSize;
}

UOSInt Net::Email::POP3Server::WriteRAW(NotNullPtr<Net::TCPClient> cli, const UTF8Char *msg, UOSInt msgLen)
{
	UOSInt buffSize;
	buffSize = cli->Write(msg, msgLen);
	if (this->rawLog)
	{
		this->rawLog->Write(msg, buffSize);
	}
	return buffSize;
}

void Net::Email::POP3Server::ParseCmd(NotNullPtr<Net::TCPClient> cli, NotNullPtr<MailStatus> cliStatus, const UTF8Char *cmd, UOSInt cmdLen)
{
#if defined(VERBOSE)
	printf("%s\r\n", cmd);
#endif
	if (Text::StrEqualsC(cmd, cmdLen, UTF8STRC("CAPA")))
	{
		WriteMessage(cli, true, CSTR("Capability list follows"));
		Text::StringBuilderUTF8 sb;
		sb.AppendC(UTF8STRC("CAPA\r\nTOP\r\nUSER\r\nUIDL\r\n"));
		if (!this->sslConn && !this->ssl.IsNull() && !cli->IsSSL())
		{
			sb.AppendC(UTF8STRC("STLS\r\n"));
		}
		sb.AppendC(UTF8STRC(".\r\n"));
#if defined(VERBOSE)
		printf("%s", sb.v);
#endif
		WriteRAW(cli, sb.v, sb.leng);
	}
	else if (Text::StrEqualsC(cmd, cmdLen, UTF8STRC("AUTH")))
	{
		if (cliStatus->userId == 0)
		{
			WriteMessage(cli, false, CSTR("authorization first"));
		}
	}
	else if (Text::StrEqualsC(cmd, cmdLen, UTF8STRC("QUIT")))
	{
		WriteMessage(cli, true, CSTR_NULL);
		cli->ShutdownSend();
	}
	else if (Text::StrEqualsC(cmd, cmdLen, UTF8STRC("STLS")))
	{
		NotNullPtr<Net::SSLEngine> ssl;
		if (this->sslConn || cli->IsSSL())
		{
			WriteMessage(cli, false, CSTR("Command not permitted when TLS active"));
		}
		else if (this->ssl.SetTo(ssl))
		{
			WriteMessage(cli, true, CSTR("Begin TLS negotiation now."));
			Socket *s = cli->RemoveSocket();
			ssl->ServerInit(s, ConnReady, this);
		}
		else
		{
			WriteMessage(cli, false, CSTR("Command not supported"));
		}
	}
	else if (Text::StrEqualsC(cmd, cmdLen, UTF8STRC("STAT")))
	{
		if (cliStatus->userId == 0)
		{
			WriteMessage(cli, false, CSTR("authorization first"));
		}
		else
		{
			UOSInt mailCnt;
			UOSInt mailSize;
			mailCnt = this->mailCtrl->GetMessageStat(cliStatus->userId, &mailSize);
			Text::StringBuilderUTF8 sb;
			sb.AppendUOSInt(mailCnt);
			sb.AppendC(UTF8STRC(" "));
			sb.AppendUOSInt(mailSize);
			WriteMessage(cli, true, sb.ToCString());
		}
	}
	else if (Text::StrEqualsC(cmd, cmdLen, UTF8STRC("LIST")))
	{
		if (cliStatus->userId == 0)
		{
			WriteMessage(cli, false, CSTR("authorization first"));
		}
		else
		{
			Data::ArrayListUInt32 unreadList;
			UOSInt i;
			UOSInt j;
			UInt32 id;
			if (this->mailCtrl->GetUnreadList(cliStatus->userId, &unreadList))
			{
				Text::StringBuilderUTF8 sb;
				Net::Email::MailController::MessageInfo info;
				WriteMessage(cli, true, CSTR("messages follow"));
				i = 0;
				j = unreadList.GetCount();
				while (i < j)
				{
					id = unreadList.GetItem(i);
					if (this->mailCtrl->GetMessageInfo(cliStatus->userId, id, &info))
					{
						sb.AppendU32(id + 1);
						sb.AppendC(UTF8STRC(" "));
						sb.AppendUOSInt(info.size);
						sb.AppendC(UTF8STRC("\r\n"));
					}
					i++;
				}
				sb.AppendC(UTF8STRC(".\r\n"));
				WriteRAW(cli, sb.ToString(), sb.GetLength());
			}
			else
			{
				WriteMessage(cli, false, CSTR("authorization first"));
			}
		}
	}
	else if (Text::StrStartsWithC(cmd, cmdLen, UTF8STRC("LIST ")))
	{
		if (cliStatus->userId == 0)
		{
			WriteMessage(cli, false, CSTR("authorization first"));
		}
		else
		{
			UInt32 msgIndex;
			msgIndex = Text::StrToUInt32(&cmd[5]);
			if (msgIndex <= 0)
			{
				WriteMessage(cli, false, CSTR("authorization first"));
			}
			else
			{
				Net::Email::MailController::MessageInfo info;
				if (this->mailCtrl->GetMessageInfo(cliStatus->userId, msgIndex - 1, &info))
				{
					Text::StringBuilderUTF8 sb;
					sb.AppendU32(msgIndex);
					sb.AppendC(UTF8STRC(" "));
					sb.AppendUOSInt(info.size);
					WriteMessage(cli, true, sb.ToCString());
				}
				else
				{
					WriteMessage(cli, false, CSTR("no such message"));
				}
			}
		}
	}
	else if (Text::StrStartsWithC(cmd, cmdLen, UTF8STRC("RETR ")))
	{
		if (cliStatus->userId == 0)
		{
			WriteMessage(cli, false, CSTR("authorization first"));
		}
		else
		{
			UInt32 msgIndex;
			msgIndex = Text::StrToUInt32(&cmd[5]);
			if (msgIndex <= 0)
			{
				WriteMessage(cli, false, CSTR("authorization first"));
			}
			else
			{
				IO::MemoryStream mstm;
				if (this->mailCtrl->GetMessageContent(cliStatus->userId, msgIndex - 1, &mstm))
				{
					WriteMessage(cli, true, CSTR("Content follows"));
					UOSInt buffSize;
					UInt8 *buff;
					buff = mstm.GetBuff(buffSize);
					cli->Write(buff, buffSize);
					if (this->rawLog)
					{
						this->rawLog->Write(buff, buffSize);
					}
					WriteRAW(cli, UTF8STRC("\r\n.\r\n"));
				}
				else
				{
					WriteMessage(cli, false, CSTR("no such message"));
				}
			}
		}
	}
	else if (Text::StrStartsWithC(cmd, cmdLen, UTF8STRC("TOP ")))
	{
		if (cliStatus->userId == 0)
		{
			WriteMessage(cli, false, CSTR("authorization first"));
		}
		else
		{
			UTF8Char sbuff[32];
			UInt32 msgIndex;
			UInt32 lineNum;
			Text::PString sarr[3];
			if (cmdLen >= 35)
			{
				WriteMessage(cli, false, CSTR("invalid param"));
			}
			else if (Text::StrSplitWSP(sarr, 3, Text::PString(sbuff, (UOSInt)(Text::StrConcatC(sbuff, &cmd[4], cmdLen - 4) - sbuff))) != 2)
			{
				WriteMessage(cli, false, CSTR("invalid param"));
			}
			else if (!sarr[0].ToUInt32(msgIndex) || !sarr[1].ToUInt32(lineNum))
			{
				WriteMessage(cli, false, CSTR("invalid param"));
			}
			else
			{
				IO::MemoryStream mstm;
				if (this->mailCtrl->GetMessageContent(cliStatus->userId, msgIndex - 1, &mstm))
				{
					WriteMessage(cli, true, CSTR("Content follows"));
					Text::StringBuilderUTF8 sb;
					mstm.SeekFromBeginning(0);
					Text::UTF8Reader reader(mstm);
					while (reader.ReadLine(sb, 1024))
					{
						sb.AppendC(UTF8STRC("\r\n"));
						cli->Write(sb.v, sb.leng);
						if (this->rawLog)
						{
							this->rawLog->Write(sb.v, sb.leng);
						}
						sb.ClearStr();
					}
					cli->Write(UTF8STRC("\r\n"));
					if (this->rawLog)
					{
						this->rawLog->Write(UTF8STRC("\r\n"));
					}
					while (lineNum > 0 && reader.ReadLine(sb, 1024))
					{
						sb.AppendC(UTF8STRC("\r\n"));
						cli->Write(sb.v, sb.leng);
						if (this->rawLog)
						{
							this->rawLog->Write(sb.v, sb.leng);
						}
						sb.ClearStr();
						lineNum--;
					}
					WriteRAW(cli, UTF8STRC("\r\n.\r\n"));
				}
				else
				{
					WriteMessage(cli, false, CSTR("no such message"));
				}
			}
		}
	}
	else if (Text::StrStartsWithC(cmd, cmdLen, UTF8STRC("DELE ")))
	{
		if (cliStatus->userId == 0)
		{
			WriteMessage(cli, false, CSTR("authorization first"));
		}
		else
		{
			UInt32 msgIndex;
			msgIndex = Text::StrToUInt32(&cmd[5]);
			if (msgIndex <= 0)
			{
				WriteMessage(cli, false, CSTR("authorization first"));
			}
			else
			{
				IO::MemoryStream mstm;
				Int32 ret = this->mailCtrl->RemoveMessage(cliStatus->userId, msgIndex - 1);
				if (ret == 1)
				{
					WriteMessage(cli, true, CSTR("message deleted"));
				}
				else if (ret == 2)
				{
					WriteMessage(cli, false, CSTR("message already deleted"));
				}
				else
				{
					WriteMessage(cli, false, CSTR("no such message"));
				}
			}
		}
	}
	else if (Text::StrEqualsC(cmd, cmdLen, UTF8STRC("UIDL")))
	{
		if (cliStatus->userId == 0)
		{
			WriteMessage(cli, false, CSTR("authorization first"));
		}
		else
		{
			Data::ArrayListUInt32 unreadList;
			UOSInt i;
			UOSInt j;
			UInt32 id;
			if (this->mailCtrl->GetUnreadList(cliStatus->userId, &unreadList))
			{
				Text::StringBuilderUTF8 sb;
				Net::Email::MailController::MessageInfo info;
				WriteMessage(cli, true, CSTR("unique-id listing follows"));
				i = 0;
				j = unreadList.GetCount();
				while (i < j)
				{
					id = unreadList.GetItem(i);
					if (this->mailCtrl->GetMessageInfo(cliStatus->userId, id, &info))
					{
						sb.AppendU32(id + 1);
						sb.AppendC(UTF8STRC(" "));
						sb.AppendSlow(info.uid);
						sb.AppendC(UTF8STRC("\r\n"));
					}
					i++;
				}
				sb.AppendC(UTF8STRC(".\r\n"));
				WriteRAW(cli, sb.ToString(), sb.GetLength());
			}
			else
			{
				WriteMessage(cli, false, CSTR("authorization first"));
			}
		}
	}
	else if (Text::StrStartsWithC(cmd, cmdLen, UTF8STRC("UIDL ")))
	{
		if (cliStatus->userId == 0)
		{
			WriteMessage(cli, false, CSTR("authorization first"));
		}
		else
		{
			UInt32 msgIndex;
			msgIndex = Text::StrToUInt32(&cmd[5]);
			if (msgIndex <= 0)
			{
				WriteMessage(cli, false, CSTR("authorization first"));
			}
			else
			{
				Net::Email::MailController::MessageInfo info;
				if (this->mailCtrl->GetMessageInfo(cliStatus->userId, msgIndex - 1, &info))
				{
					Text::StringBuilderUTF8 sb;
					sb.AppendU32(msgIndex);
					sb.AppendC(UTF8STRC(" "));
					sb.AppendSlow(info.uid);
					WriteMessage(cli, true, sb.ToCString());
				}
				else
				{
					WriteMessage(cli, false, CSTR("no such message"));
				}
			}
		}
	}
	else if (Text::StrStartsWithC(cmd, cmdLen, UTF8STRC("USER ")))
	{
		SDEL_STRING(cliStatus->userName);
		cliStatus->userName = Text::String::New(&cmd[5], cmdLen - 5).Ptr();
		WriteMessage(cli, true, CSTR_NULL);
	}
	else if (Text::StrStartsWithC(cmd, cmdLen, UTF8STRC("PASS ")))
	{
		Bool succ = false;
		if (cliStatus->userName)
		{
			Int32 userId;
			if (this->mailCtrl->Login(cliStatus->userName->ToCString(), Text::CStringNN(&cmd[5], cmdLen - 5), userId))
			{
				succ = true;
				cliStatus->userId = userId;
			}
		}
		if (succ)
		{
			WriteMessage(cli, true, CSTR_NULL);
		}
		else
		{
			WriteMessage(cli, false, CSTR("authorization failed  Check your server settings."));
		}
	}
	else if (Text::StrEqualsC(cmd, cmdLen, UTF8STRC("NOOP")))
	{
		WriteMessage(cli, true, CSTR_NULL);
	}
	else
	{
		WriteMessage(cli, false, CSTR("Unknown command"));
	}
}

Net::Email::POP3Server::POP3Server(NotNullPtr<Net::SocketFactory> sockf, Optional<Net::SSLEngine> ssl, Bool sslConn, UInt16 port, NotNullPtr<IO::LogTool> log, Text::CString greeting, NotNullPtr<Net::Email::MailController> mailCtrl, Bool autoStart) : cliMgr(60, ClientEvent, ClientData, this, 4, ClientTimeout)
{
	this->sockf = sockf;
	this->ssl = ssl;
	this->sslConn = sslConn;
	this->log = log;
	this->greeting = Text::String::New(greeting);
	this->mailCtrl = mailCtrl;
	NEW_CLASS(this->svr, Net::TCPServer(this->sockf, 0, port, log, ConnHdlr, this, CSTR_NULL, autoStart));
	NEW_CLASS(this->rawLog, IO::FileStream(CSTR("POP3Log.dat"), IO::FileMode::Append, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal));
}

Net::Email::POP3Server::~POP3Server()
{
	DEL_CLASS(this->svr);
	this->cliMgr.CloseAll();
	DEL_CLASS(this->rawLog);
	this->greeting->Release();
}

Bool Net::Email::POP3Server::Start()
{
	return this->svr->Start();
}

Bool Net::Email::POP3Server::IsError()
{
	if (this->sslConn && this->ssl.IsNull())
		return true;
	return this->svr->IsV4Error();
}

UInt16 Net::Email::POP3Server::GetDefaultPort(Bool ssl)
{
	if (ssl)
	{
		return 995;
	}
	else
	{
		return 110;
	}
}
