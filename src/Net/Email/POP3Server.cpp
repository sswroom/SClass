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
	NEW_CLASS(cliStatus->rcptTo, Data::ArrayList<const Char *>());
	cliStatus->dataStm = 0;
	cliStatus->dataMode = false;
	me->cliMgr->AddClient(cli, cliStatus);

	me->WriteMessage(cli, true, me->greeting);
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
			me->ParseCmd(cli, cliStatus, (Char*)&cliStatus->buff[j]);
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

UOSInt Net::Email::POP3Server::WriteMessage(Net::TCPClient *cli, Bool success, const UTF8Char *msg)
{
	Text::StringBuilderUTF8 sb;
	if (success)
	{
		sb.Append((const UTF8Char *)"+OK");
	}
	else
	{
		sb.Append((const UTF8Char *)"-ERR");
	}
	if (msg)
	{
		sb.Append((const UTF8Char *)" ");
		sb.Append(msg);
	}
	sb.Append((const UTF8Char *)"\r\n");


	UOSInt buffSize;
	buffSize = cli->Write(sb.ToString(), sb.GetLength());
	if (this->rawLog)
	{
		this->rawLog->Write(sb.ToString(), buffSize);
	}
	return buffSize;
}

UOSInt Net::Email::POP3Server::WriteRAW(Net::TCPClient *cli, const UTF8Char *msg)
{
	UOSInt strLen = Text::StrCharCnt(msg);
	UOSInt buffSize;
	buffSize = cli->Write(msg, strLen);
	if (this->rawLog)
	{
		this->rawLog->Write(msg, buffSize);
	}
	return buffSize;
}

void Net::Email::POP3Server::ParseCmd(Net::TCPClient *cli, MailStatus *cliStatus, Char *cmd)
{
	if (Text::StrEquals(cmd, "CAPA"))
	{
		if (cliStatus->userId == 0)
		{
			WriteMessage(cli, false, (const UTF8Char *)"authorization first");
		}
	}
	else if (Text::StrEquals(cmd, "AUTH"))
	{
		if (cliStatus->userId == 0)
		{
			WriteMessage(cli, false, (const UTF8Char *)"authorization first");
		}
	}
	else if (Text::StrEquals(cmd, "QUIT"))
	{
		WriteMessage(cli, true, 0);
		cli->ShutdownSend();
	}
	else if (Text::StrEquals(cmd, "STAT"))
	{
		if (cliStatus->userId == 0)
		{
			WriteMessage(cli, false, (const UTF8Char *)"authorization first");
		}
		else
		{
			UOSInt mailCnt;
			UOSInt mailSize;
			mailCnt = this->mailCtrl->GetMessageStat(cliStatus->userId, &mailSize);
			Text::StringBuilderUTF8 sb;
			sb.AppendUOSInt(mailCnt);
			sb.Append((const UTF8Char *)" ");
			sb.AppendUOSInt(mailSize);
			WriteMessage(cli, true, sb.ToString());
		}
	}
	else if (Text::StrEquals(cmd, "LIST"))
	{
		if (cliStatus->userId == 0)
		{
			WriteMessage(cli, false, (const UTF8Char *)"authorization first");
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
				WriteMessage(cli, true, (const UTF8Char *)"messages follow");
				i = 0;
				j = unreadList.GetCount();
				while (i < j)
				{
					id = unreadList.GetItem(i);
					if (this->mailCtrl->GetMessageInfo(cliStatus->userId, id, &info))
					{
						sb.AppendU32(id + 1);
						sb.Append((const UTF8Char *)" ");
						sb.AppendUOSInt(info.size);
						sb.Append((const UTF8Char *)"\r\n");
					}
					i++;
				}
				sb.Append((const UTF8Char *)".\r\n");
				WriteRAW(cli, sb.ToString());
			}
			else
			{
				WriteMessage(cli, false, (const UTF8Char *)"authorization first");
			}
		}
	}
	else if (Text::StrStartsWith(cmd, "LIST "))
	{
		if (cliStatus->userId == 0)
		{
			WriteMessage(cli, false, (const UTF8Char *)"authorization first");
		}
		else
		{
			UInt32 msgIndex;
			msgIndex = Text::StrToUInt32(&cmd[5]);
			if (msgIndex <= 0)
			{
				WriteMessage(cli, false, (const UTF8Char *)"authorization first");
			}
			else
			{
				Net::Email::MailController::MessageInfo info;
				if (this->mailCtrl->GetMessageInfo(cliStatus->userId, msgIndex - 1, &info))
				{
					Text::StringBuilderUTF8 sb;
					sb.AppendU32(msgIndex);
					sb.Append((const UTF8Char *)" ");
					sb.AppendUOSInt(info.size);
					WriteMessage(cli, true, sb.ToString());
				}
				else
				{
					WriteMessage(cli, false, (const UTF8Char *)"no such message");
				}
			}
		}
	}
	else if (Text::StrStartsWith(cmd, "RETR "))
	{
		if (cliStatus->userId == 0)
		{
			WriteMessage(cli, false, (const UTF8Char *)"authorization first");
		}
		else
		{
			UInt32 msgIndex;
			msgIndex = Text::StrToUInt32(&cmd[5]);
			if (msgIndex <= 0)
			{
				WriteMessage(cli, false, (const UTF8Char *)"authorization first");
			}
			else
			{
				IO::MemoryStream mstm((const UTF8Char*)"Net.POP3Server.ParseCmd.mstm");
				if (this->mailCtrl->GetMessageContent(cliStatus->userId, msgIndex - 1, &mstm))
				{
					WriteMessage(cli, true, (const UTF8Char *)"Content follows");
					UOSInt buffSize;
					UInt8 *buff;
					buff = mstm.GetBuff(&buffSize);
					cli->Write(buff, buffSize);
					if (this->rawLog)
					{
						this->rawLog->Write(buff, buffSize);
					}
					WriteRAW(cli, (const UTF8Char *)"\r\n.\r\n");
				}
				else
				{
					WriteMessage(cli, false, (const UTF8Char *)"no such message");
				}
			}
		}
	}
	else if (Text::StrStartsWith(cmd, "DELE "))
	{
		if (cliStatus->userId == 0)
		{
			WriteMessage(cli, false, (const UTF8Char *)"authorization first");
		}
		else
		{
			UInt32 msgIndex;
			msgIndex = Text::StrToUInt32(&cmd[5]);
			if (msgIndex <= 0)
			{
				WriteMessage(cli, false, (const UTF8Char *)"authorization first");
			}
			else
			{
				IO::MemoryStream mstm((const UTF8Char*)"Net.POP3Server.ParseCmd.mstm");
				Int32 ret = this->mailCtrl->RemoveMessage(cliStatus->userId, msgIndex - 1);
				if (ret == 1)
				{
					WriteMessage(cli, true, (const UTF8Char *)"message deleted");
				}
				else if (ret == 2)
				{
					WriteMessage(cli, false, (const UTF8Char *)"message already deleted");
				}
				else
				{
					WriteMessage(cli, false, (const UTF8Char *)"no such message");
				}
			}
		}
	}
	else if (Text::StrEquals(cmd, "UIDL"))
	{
		if (cliStatus->userId == 0)
		{
			WriteMessage(cli, false, (const UTF8Char *)"authorization first");
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
				WriteMessage(cli, true, (const UTF8Char *)"unique-id listing follows");
				i = 0;
				j = unreadList.GetCount();
				while (i < j)
				{
					id = unreadList.GetItem(i);
					if (this->mailCtrl->GetMessageInfo(cliStatus->userId, id, &info))
					{
						sb.AppendU32(id + 1);
						sb.Append((const UTF8Char *)" ");
						sb.Append(info.uid);
						sb.Append((const UTF8Char *)"\r\n");
					}
					i++;
				}
				sb.Append((const UTF8Char *)".\r\n");
				WriteRAW(cli, sb.ToString());
			}
			else
			{
				WriteMessage(cli, false, (const UTF8Char *)"authorization first");
			}
		}
	}
	else if (Text::StrStartsWith(cmd, "UIDL "))
	{
		if (cliStatus->userId == 0)
		{
			WriteMessage(cli, false, (const UTF8Char *)"authorization first");
		}
		else
		{
			UInt32 msgIndex;
			msgIndex = Text::StrToUInt32(&cmd[5]);
			if (msgIndex <= 0)
			{
				WriteMessage(cli, false, (const UTF8Char *)"authorization first");
			}
			else
			{
				Net::Email::MailController::MessageInfo info;
				if (this->mailCtrl->GetMessageInfo(cliStatus->userId, msgIndex - 1, &info))
				{
					Text::StringBuilderUTF8 sb;
					sb.AppendU32(msgIndex);
					sb.Append((const UTF8Char *)" ");
					sb.Append(info.uid);
					WriteMessage(cli, true, sb.ToString());
				}
				else
				{
					WriteMessage(cli, false, (const UTF8Char *)"no such message");
				}
			}
		}
	}
	else if (Text::StrStartsWith(cmd, "USER "))
	{
		if (cliStatus->userName)
		{
			Text::StrDelNew(cliStatus->userName);
		}
		cliStatus->userName = Text::StrCopyNew(&cmd[5]);
		WriteMessage(cli, true, 0);
	}
	else if (Text::StrStartsWith(cmd, "PASS "))
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
			WriteMessage(cli, true, 0);
		}
		else
		{
			WriteMessage(cli, false, (const UTF8Char *)"authorization failed  Check your server settings.");
		}
	}
	else
	{
		WriteMessage(cli, false, (const UTF8Char *)"Unknown command");
	}
}

Net::Email::POP3Server::POP3Server(Net::SocketFactory *sockf, Net::SSLEngine *ssl, UInt16 port, IO::LogTool *log, const UTF8Char *greeting, Net::Email::MailController *mailCtrl)
{
	this->sockf = sockf;
	this->ssl = ssl;
	this->log = log;
	this->greeting = Text::StrCopyNew(greeting);
	this->mailCtrl = mailCtrl;
	NEW_CLASS(this->cliMgr, Net::TCPClientMgr(60, ClientEvent, ClientData, this, 4, ClientTimeout));
	NEW_CLASS(this->svr, Net::TCPServer(this->sockf, port, log, ConnHdlr, this, 0));
	NEW_CLASS(this->rawLog, IO::FileStream((const UTF8Char *)"POP3Log.dat", IO::FileMode::Append, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal));
}

Net::Email::POP3Server::~POP3Server()
{
	DEL_CLASS(this->svr);
	DEL_CLASS(this->cliMgr);
	DEL_CLASS(this->rawLog);
	Text::StrDelNew(this->greeting);
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
