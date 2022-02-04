#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ArrayListUInt32.h"
#include "Net/Email/MailController.h"
#include "Net/Email/POP3Server.h"
#include "Text/Encoding.h"
#include "Text/MyString.h"
#include "Text/StringBuilderUTF8.h"

void __stdcall Net::Email::POP3Server::ConnReady(Net::TCPClient *cli, void *userObj)
{
	Net::Email::POP3Server *me = (Net::Email::POP3Server*)userObj;
	MailStatus *cliStatus;
	cliStatus = MemAlloc(MailStatus, 1);
	cliStatus->buff = MemAlloc(UInt8, 2048);
	cliStatus->buffSize = 0;
	cliStatus->cliName = 0;
	cliStatus->userName = 0;
	cliStatus->userId = 0;
	NEW_CLASS(cliStatus->rcptTo, Data::ArrayList<const UTF8Char *>());
	cliStatus->dataStm = 0;
	cliStatus->dataMode = false;
	me->cliMgr->AddClient(cli, cliStatus);

	me->WriteMessage(cli, true, me->greeting->v, me->greeting->leng);
}

void __stdcall Net::Email::POP3Server::ConnHdlr(Socket *s, void *userObj)
{
	Net::Email::POP3Server *me = (Net::Email::POP3Server*)userObj;
	if (me->ssl)
	{
		me->ssl->ServerInit(s, ConnReady, me);
	}
	else
	{
		Net::TCPClient *cli;
		NEW_CLASS(cli, Net::TCPClient(me->sockf, s));
		ConnReady(cli, me);
	}
}

void __stdcall Net::Email::POP3Server::ClientEvent(Net::TCPClient *cli, void *userObj, void *cliData, Net::TCPClientMgr::TCPEventType evtType)
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
		if (cliStatus->userName)
		{
			Text::StrDelNew(cliStatus->userName);
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

void __stdcall Net::Email::POP3Server::ClientData(Net::TCPClient *cli, void *userObj, void *cliData, const UInt8 *buff, UOSInt size)
{
	Net::Email::POP3Server *me = (Net::Email::POP3Server*)userObj;
	MailStatus *cliStatus;
	cliStatus = (MailStatus*)cliData;
	if (me->rawLog)
	{
		me->rawLog->Write(buff, size);
	}
	if (size > 2048)
	{
		MemCopyNO(cliStatus->buff, &buff[size - 2048], 2048);
		cliStatus->buffSize = 2048;
	}
	else if (cliStatus->buffSize + size > 2048)
	{
		MemCopyO(cliStatus->buff, &cliStatus->buff[cliStatus->buffSize - 2048 + size], 2048 - size);
		MemCopyNO(&cliStatus->buff[2048 - size], buff, size);
		cliStatus->buffSize = 2048;
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

void __stdcall Net::Email::POP3Server::ClientTimeout(Net::TCPClient *cli, void *userObj, void *cliData)
{
}

UOSInt Net::Email::POP3Server::WriteMessage(Net::TCPClient *cli, Bool success, const UTF8Char *msg, UOSInt msgLen)
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
	if (msg)
	{
		sb.AppendC(UTF8STRC(" "));
		sb.AppendC(msg, msgLen);
	}
	sb.AppendC(UTF8STRC("\r\n"));


	UOSInt buffSize;
	buffSize = cli->Write(sb.ToString(), sb.GetLength());
	if (this->rawLog)
	{
		this->rawLog->Write(sb.ToString(), buffSize);
	}
	return buffSize;
}

UOSInt Net::Email::POP3Server::WriteRAW(Net::TCPClient *cli, const UTF8Char *msg, UOSInt msgLen)
{
	UOSInt buffSize;
	buffSize = cli->Write(msg, msgLen);
	if (this->rawLog)
	{
		this->rawLog->Write(msg, buffSize);
	}
	return buffSize;
}

void Net::Email::POP3Server::ParseCmd(Net::TCPClient *cli, MailStatus *cliStatus, const UTF8Char *cmd, UOSInt cmdLen)
{
	if (Text::StrEqualsC(cmd, cmdLen, UTF8STRC("CAPA")))
	{
		if (cliStatus->userId == 0)
		{
			WriteMessage(cli, false, UTF8STRC("authorization first"));
		}
	}
	else if (Text::StrEqualsC(cmd, cmdLen, UTF8STRC("AUTH")))
	{
		if (cliStatus->userId == 0)
		{
			WriteMessage(cli, false, UTF8STRC("authorization first"));
		}
	}
	else if (Text::StrEqualsC(cmd, cmdLen, UTF8STRC("QUIT")))
	{
		WriteMessage(cli, true, 0, 0);
		cli->ShutdownSend();
	}
	else if (Text::StrEqualsC(cmd, cmdLen, UTF8STRC("STAT")))
	{
		if (cliStatus->userId == 0)
		{
			WriteMessage(cli, false, UTF8STRC("authorization first"));
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
			WriteMessage(cli, true, sb.ToString(), sb.GetLength());
		}
	}
	else if (Text::StrEqualsC(cmd, cmdLen, UTF8STRC("LIST")))
	{
		if (cliStatus->userId == 0)
		{
			WriteMessage(cli, false, UTF8STRC("authorization first"));
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
				WriteMessage(cli, true, UTF8STRC("messages follow"));
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
				WriteMessage(cli, false, UTF8STRC("authorization first"));
			}
		}
	}
	else if (Text::StrStartsWithC(cmd, cmdLen, UTF8STRC("LIST ")))
	{
		if (cliStatus->userId == 0)
		{
			WriteMessage(cli, false, UTF8STRC("authorization first"));
		}
		else
		{
			UInt32 msgIndex;
			msgIndex = Text::StrToUInt32(&cmd[5]);
			if (msgIndex <= 0)
			{
				WriteMessage(cli, false, UTF8STRC("authorization first"));
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
					WriteMessage(cli, true, sb.ToString(), sb.GetLength());
				}
				else
				{
					WriteMessage(cli, false, UTF8STRC("no such message"));
				}
			}
		}
	}
	else if (Text::StrStartsWithC(cmd, cmdLen, UTF8STRC("RETR ")))
	{
		if (cliStatus->userId == 0)
		{
			WriteMessage(cli, false, UTF8STRC("authorization first"));
		}
		else
		{
			UInt32 msgIndex;
			msgIndex = Text::StrToUInt32(&cmd[5]);
			if (msgIndex <= 0)
			{
				WriteMessage(cli, false, UTF8STRC("authorization first"));
			}
			else
			{
				IO::MemoryStream mstm(UTF8STRC("Net.POP3Server.ParseCmd.mstm"));
				if (this->mailCtrl->GetMessageContent(cliStatus->userId, msgIndex - 1, &mstm))
				{
					WriteMessage(cli, true, UTF8STRC("Content follows"));
					UOSInt buffSize;
					UInt8 *buff;
					buff = mstm.GetBuff(&buffSize);
					cli->Write(buff, buffSize);
					if (this->rawLog)
					{
						this->rawLog->Write(buff, buffSize);
					}
					WriteRAW(cli, UTF8STRC("\r\n.\r\n"));
				}
				else
				{
					WriteMessage(cli, false, UTF8STRC("no such message"));
				}
			}
		}
	}
	else if (Text::StrStartsWithC(cmd, cmdLen, UTF8STRC("DELE ")))
	{
		if (cliStatus->userId == 0)
		{
			WriteMessage(cli, false, UTF8STRC("authorization first"));
		}
		else
		{
			UInt32 msgIndex;
			msgIndex = Text::StrToUInt32(&cmd[5]);
			if (msgIndex <= 0)
			{
				WriteMessage(cli, false, UTF8STRC("authorization first"));
			}
			else
			{
				IO::MemoryStream mstm(UTF8STRC("Net.POP3Server.ParseCmd.mstm"));
				Int32 ret = this->mailCtrl->RemoveMessage(cliStatus->userId, msgIndex - 1);
				if (ret == 1)
				{
					WriteMessage(cli, true, UTF8STRC("message deleted"));
				}
				else if (ret == 2)
				{
					WriteMessage(cli, false, UTF8STRC("message already deleted"));
				}
				else
				{
					WriteMessage(cli, false, UTF8STRC("no such message"));
				}
			}
		}
	}
	else if (Text::StrEqualsC(cmd, cmdLen, UTF8STRC("UIDL")))
	{
		if (cliStatus->userId == 0)
		{
			WriteMessage(cli, false, UTF8STRC("authorization first"));
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
				WriteMessage(cli, true, UTF8STRC("unique-id listing follows"));
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
				WriteMessage(cli, false, UTF8STRC("authorization first"));
			}
		}
	}
	else if (Text::StrStartsWithC(cmd, cmdLen, UTF8STRC("UIDL ")))
	{
		if (cliStatus->userId == 0)
		{
			WriteMessage(cli, false, UTF8STRC("authorization first"));
		}
		else
		{
			UInt32 msgIndex;
			msgIndex = Text::StrToUInt32(&cmd[5]);
			if (msgIndex <= 0)
			{
				WriteMessage(cli, false, UTF8STRC("authorization first"));
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
					WriteMessage(cli, true, sb.ToString(), sb.GetLength());
				}
				else
				{
					WriteMessage(cli, false, UTF8STRC("no such message"));
				}
			}
		}
	}
	else if (Text::StrStartsWithC(cmd, cmdLen, UTF8STRC("USER ")))
	{
		if (cliStatus->userName)
		{
			Text::StrDelNew(cliStatus->userName);
		}
		cliStatus->userName = Text::StrCopyNewC(&cmd[5], cmdLen - 5);
		WriteMessage(cli, true, 0, 0);
	}
	else if (Text::StrStartsWithC(cmd, cmdLen, UTF8STRC("PASS ")))
	{
		Bool succ = false;
		if (cliStatus->userName)
		{
			Int32 userId;
			if (this->mailCtrl->Login(cliStatus->userName, &cmd[5], &userId))
			{
				succ = true;
				cliStatus->userId = userId;
			}
		}
		if (succ)
		{
			WriteMessage(cli, true, 0, 0);
		}
		else
		{
			WriteMessage(cli, false, UTF8STRC("authorization failed  Check your server settings."));
		}
	}
	else
	{
		WriteMessage(cli, false, UTF8STRC("Unknown command"));
	}
}

Net::Email::POP3Server::POP3Server(Net::SocketFactory *sockf, Net::SSLEngine *ssl, UInt16 port, IO::LogTool *log, const UTF8Char *greeting, Net::Email::MailController *mailCtrl)
{
	this->sockf = sockf;
	this->ssl = ssl;
	this->log = log;
	this->greeting = Text::String::NewNotNull(greeting);
	this->mailCtrl = mailCtrl;
	NEW_CLASS(this->cliMgr, Net::TCPClientMgr(60, ClientEvent, ClientData, this, 4, ClientTimeout));
	NEW_CLASS(this->svr, Net::TCPServer(this->sockf, port, log, ConnHdlr, this, 0));
	NEW_CLASS(this->rawLog, IO::FileStream(CSTR("POP3Log.dat"), IO::FileMode::Append, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal));
}

Net::Email::POP3Server::~POP3Server()
{
	DEL_CLASS(this->svr);
	DEL_CLASS(this->cliMgr);
	DEL_CLASS(this->rawLog);
	this->greeting->Release();
}

Bool Net::Email::POP3Server::IsError()
{
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
