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

void __stdcall Net::Email::POP3Server::ConnReady(NN<Net::TCPClient> cli, AnyType userObj)
{
	NN<Net::Email::POP3Server> me = userObj.GetNN<Net::Email::POP3Server>();
#if defined(VERBOSE)
	printf("POP3Server: %lld, ConnReady\r\n", cli->GetCliId());
#endif
	MailStatus *cliStatus;
	NEW_CLASS(cliStatus, MailStatus());
	cliStatus->buffSize = 0;
	cliStatus->cliName = nullptr;
	cliStatus->userName = nullptr;
	cliStatus->userId = 0;
	cliStatus->dataStm = nullptr;
	cliStatus->dataMode = false;
	me->cliMgr.AddClient(cli, cliStatus);

	me->WriteMessage(cli, true, me->greeting->ToCString());
}

void __stdcall Net::Email::POP3Server::ConnHdlr(NN<Socket> s, AnyType userObj)
{
	NN<Net::Email::POP3Server> me = userObj.GetNN<Net::Email::POP3Server>();
	NN<Net::SSLEngine> ssl;
	if (me->sslConn && me->ssl.SetTo(ssl))
	{
		ssl->ServerInit(s, ConnReady, me);
	}
	else
	{
		NN<Net::TCPClient> cli;
		NEW_CLASSNN(cli, Net::TCPClient(me->sockf, s));
		ConnReady(cli, me);
	}
}

void __stdcall Net::Email::POP3Server::ClientEvent(NN<Net::TCPClient> cli, AnyType userObj, AnyType cliData, Net::TCPClientMgr::TCPEventType evtType)
{
	if (evtType == Net::TCPClientMgr::TCP_EVENT_DISCONNECT)
	{
		NN<MailStatus> cliStatus = cliData.GetNN<MailStatus>();
		UIntOS i;
		UnsafeArray<const UTF8Char> cliName;
		if (cliStatus->cliName.SetTo(cliName))
		{
			Text::StrDelNew(cliName);
		}
		OPTSTR_DEL(cliStatus->userName);
		i = cliStatus->rcptTo.GetCount();
		while (i-- > 0)
		{
			Text::StrDelNew(cliStatus->rcptTo.GetItemNoCheck(i));
		}
		cliStatus->dataStm.Delete();
		cliStatus.Delete();
		cli.Delete();
	}
	else if (evtType == Net::TCPClientMgr::TCP_EVENT_HASDATA)
	{
	}
}

void __stdcall Net::Email::POP3Server::ClientData(NN<Net::TCPClient> cli, AnyType userObj, AnyType cliData, const Data::ByteArrayR &buff)
{
	NN<Net::Email::POP3Server> me = userObj.GetNN<Net::Email::POP3Server>();
	NN<MailStatus> cliStatus = cliData.GetNN<MailStatus>();
	NN<IO::FileStream> rawLog;
	if (me->rawLog.SetTo(rawLog))
	{
		rawLog->Write(buff);
	}
	if (buff.GetSize() > 2048)
	{
		MemCopyNO(cliStatus->buff, &buff[buff.GetSize() - 2048], 2048);
		cliStatus->buffSize = 2048;
	}
	else if (cliStatus->buffSize + buff.GetSize() > 2048)
	{
		MemCopyO(cliStatus->buff, &cliStatus->buff[cliStatus->buffSize - 2048 + buff.GetSize()], 2048 - buff.GetSize());
		MemCopyNO(&cliStatus->buff[2048 - buff.GetSize()], buff.Arr().Ptr(), buff.GetSize());
		cliStatus->buffSize = 2048;
	}
	else
	{
		MemCopyNO(&cliStatus->buff[cliStatus->buffSize], buff.Arr().Ptr(), buff.GetSize());
		cliStatus->buffSize += buff.GetSize();
	}
	UIntOS i;
	UIntOS j;
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

void __stdcall Net::Email::POP3Server::ClientTimeout(NN<Net::TCPClient> cli, AnyType userObj, AnyType cliData)
{
}

UIntOS Net::Email::POP3Server::WriteMessage(NN<Net::TCPClient> cli, Bool success, Text::CString msg)
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
	Text::CStringNN nnmsg;
	if (msg.SetTo(nnmsg))
	{
		sb.AppendC(UTF8STRC(" "));
		sb.Append(nnmsg);
	}
	sb.AppendC(UTF8STRC("\r\n"));
#if defined(VERBOSE)
	printf("%s", sb.ToPtr());
#endif

	UIntOS buffSize;
	buffSize = cli->Write(sb.ToByteArray());
	NN<IO::FileStream> rawLog;
	if (this->rawLog.SetTo(rawLog))
	{
		rawLog->Write(sb.ToByteArray().WithSize(buffSize));
	}
	return buffSize;
}

UIntOS Net::Email::POP3Server::WriteRAW(NN<Net::TCPClient> cli, UnsafeArray<const UTF8Char> msg, UIntOS msgLen)
{
	UIntOS buffSize;
	buffSize = cli->Write(Data::ByteArrayR(msg, msgLen));
	NN<IO::FileStream> rawLog;
	if (this->rawLog.SetTo(rawLog))
	{
		rawLog->Write(Data::ByteArrayR(msg, buffSize));
	}
	return buffSize;
}

void Net::Email::POP3Server::ParseCmd(NN<Net::TCPClient> cli, NN<MailStatus> cliStatus, UnsafeArray<const UTF8Char> cmd, UIntOS cmdLen)
{
	NN<IO::FileStream> rawLog;
#if defined(VERBOSE)
	printf("%s\r\n", cmd.Ptr());
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
		printf("%s", sb.ToPtr());
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
		WriteMessage(cli, true, nullptr);
		cli->ShutdownSend();
	}
	else if (Text::StrEqualsC(cmd, cmdLen, UTF8STRC("STLS")))
	{
		NN<Net::SSLEngine> ssl;
		if (this->sslConn || cli->IsSSL())
		{
			WriteMessage(cli, false, CSTR("Command not permitted when TLS active"));
		}
		else if (this->ssl.SetTo(ssl))
		{
			WriteMessage(cli, true, CSTR("Begin TLS negotiation now."));
			NN<Socket> s;
			if (cli->RemoveSocket().SetTo(s))
			{
				ssl->ServerInit(s, ConnReady, this);
			}
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
			UIntOS mailCnt;
			UIntOS mailSize;
			mailCnt = this->mailCtrl->GetMessageStat(cliStatus->userId, mailSize);
			Text::StringBuilderUTF8 sb;
			sb.AppendUIntOS(mailCnt);
			sb.AppendC(UTF8STRC(" "));
			sb.AppendUIntOS(mailSize);
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
			UIntOS i;
			UIntOS j;
			UInt32 id;
			if (this->mailCtrl->GetUnreadList(cliStatus->userId, unreadList))
			{
				Text::StringBuilderUTF8 sb;
				Net::Email::MailController::MessageInfo info;
				WriteMessage(cli, true, CSTR("messages follow"));
				i = 0;
				j = unreadList.GetCount();
				while (i < j)
				{
					id = unreadList.GetItem(i);
					if (this->mailCtrl->GetMessageInfo(cliStatus->userId, id, info))
					{
						sb.AppendU32(id + 1);
						sb.AppendC(UTF8STRC(" "));
						sb.AppendUIntOS(info.size);
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
				if (this->mailCtrl->GetMessageInfo(cliStatus->userId, msgIndex - 1, info))
				{
					Text::StringBuilderUTF8 sb;
					sb.AppendU32(msgIndex);
					sb.AppendC(UTF8STRC(" "));
					sb.AppendUIntOS(info.size);
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
				if (this->mailCtrl->GetMessageContent(cliStatus->userId, msgIndex - 1, mstm))
				{
					WriteMessage(cli, true, CSTR("Content follows"));
					UIntOS buffSize;
					UnsafeArray<UInt8> buff;
					buff = mstm.GetBuff(buffSize);
					cli->Write(Data::ByteArrayR(buff, buffSize));
					if (this->rawLog.SetTo(rawLog))
					{
						rawLog->Write(Data::ByteArrayR(buff, buffSize));
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
			else if (Text::StrSplitWSP(sarr, 3, Text::PString(sbuff, (UIntOS)(Text::StrConcatC(sbuff, &cmd[4], cmdLen - 4) - sbuff))) != 2)
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
				if (this->mailCtrl->GetMessageContent(cliStatus->userId, msgIndex - 1, mstm))
				{
					WriteMessage(cli, true, CSTR("Content follows"));
					Text::StringBuilderUTF8 sb;
					mstm.SeekFromBeginning(0);
					Text::UTF8Reader reader(mstm);
					while (reader.ReadLine(sb, 1024))
					{
						sb.AppendC(UTF8STRC("\r\n"));
						cli->Write(sb.ToByteArray());
						if (this->rawLog.SetTo(rawLog))
						{
							rawLog->Write(sb.ToByteArray());
						}
						sb.ClearStr();
					}
					cli->Write(CSTR("\r\n").ToByteArray());
					if (this->rawLog.SetTo(rawLog))
					{
						rawLog->Write(CSTR("\r\n").ToByteArray());
					}
					while (lineNum > 0 && reader.ReadLine(sb, 1024))
					{
						sb.AppendC(UTF8STRC("\r\n"));
						cli->Write(sb.ToByteArray());
						if (this->rawLog.SetTo(rawLog))
						{
							rawLog->Write(sb.ToByteArray());
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
			UIntOS i;
			UIntOS j;
			UInt32 id;
			if (this->mailCtrl->GetUnreadList(cliStatus->userId, unreadList))
			{
				Text::StringBuilderUTF8 sb;
				Net::Email::MailController::MessageInfo info;
				WriteMessage(cli, true, CSTR("unique-id listing follows"));
				i = 0;
				j = unreadList.GetCount();
				while (i < j)
				{
					id = unreadList.GetItem(i);
					if (this->mailCtrl->GetMessageInfo(cliStatus->userId, id, info))
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
				if (this->mailCtrl->GetMessageInfo(cliStatus->userId, msgIndex - 1, info))
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
		OPTSTR_DEL(cliStatus->userName);
		cliStatus->userName = Text::String::New(&cmd[5], cmdLen - 5).Ptr();
		WriteMessage(cli, true, nullptr);
	}
	else if (Text::StrStartsWithC(cmd, cmdLen, UTF8STRC("PASS ")))
	{
		NN<Text::String> userName;
		Bool succ = false;
		if (cliStatus->userName.SetTo(userName))
		{
			Int32 userId;
			if (this->mailCtrl->Login(userName->ToCString(), Text::CStringNN(&cmd[5], cmdLen - 5), userId))
			{
				succ = true;
				cliStatus->userId = userId;
			}
		}
		if (succ)
		{
			WriteMessage(cli, true, nullptr);
		}
		else
		{
			WriteMessage(cli, false, CSTR("authorization failed  Check your server settings."));
		}
	}
	else if (Text::StrEqualsC(cmd, cmdLen, UTF8STRC("NOOP")))
	{
		WriteMessage(cli, true, nullptr);
	}
	else
	{
		WriteMessage(cli, false, CSTR("Unknown command"));
	}
}

Net::Email::POP3Server::POP3Server(NN<Net::SocketFactory> sockf, Optional<Net::SSLEngine> ssl, Bool sslConn, UInt16 port, NN<IO::LogTool> log, Text::CStringNN greeting, NN<Net::Email::MailController> mailCtrl, Bool autoStart) : cliMgr(60, ClientEvent, ClientData, this, 4, ClientTimeout)
{
	this->sockf = sockf;
	this->ssl = ssl;
	this->sslConn = sslConn;
	this->log = log;
	this->greeting = Text::String::New(greeting);
	this->mailCtrl = mailCtrl;
	NEW_CLASSNN(this->svr, Net::TCPServer(this->sockf, nullptr, port, log, ConnHdlr, this, nullptr, autoStart));
	NEW_CLASSOPT(this->rawLog, IO::FileStream(CSTR("POP3Log.dat"), IO::FileMode::Append, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal));
}

Net::Email::POP3Server::~POP3Server()
{
	this->svr.Delete();
	this->cliMgr.CloseAll();
	this->rawLog.Delete();
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
