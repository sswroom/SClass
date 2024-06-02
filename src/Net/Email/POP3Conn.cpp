#include "Stdafx.h"
#include "MyMemory.h"
#include "Manage/HiResClock.h"
#include "Net/Email/POP3Conn.h"
#include "Sync/SimpleThread.h"
#include "Sync/ThreadUtil.h"
#include "Text/MyString.h"
#include "Text/StringBuilderUTF8.h"
#include "Text/UTF8Reader.h"
#include "Text/TextBinEnc/Base64Enc.h"

//#define VERBOSE
#ifdef VERBOSE
#include <stdio.h>
#endif

UInt32 __stdcall Net::Email::POP3Conn::RecvThread(AnyType userObj)
{
	NN<Net::Email::POP3Conn> me = userObj.GetNN<Net::Email::POP3Conn>();
	UTF8Char sbuff[2048];
	UnsafeArray<UTF8Char> sptr;
	UOSInt i;
	Bool dataMode = false;

	me->threadStarted = true;
	me->threadRunning = true;
	{
		Text::UTF8Reader reader(me->cli);
		while (!me->threadToStop)
		{
			if (!reader.ReadLine(sbuff, 2048).SetTo(sptr))
			{
				if (me->logWriter)
				{
					me->logWriter->WriteLine(CSTR("Connection Closed"));
				}
				break;
			}

			if (me->logWriter)
			{
				me->logWriter->WriteLine(CSTRP(sbuff, sptr));
			}
	#ifdef VERBOSE
			printf("POP3 Read: %s\r\n", sbuff);
	#endif
			if (dataMode)
			{
				if (Text::StrEqualsC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC(".")))
				{
					dataMode = false;
					me->msgData = 0;
					me->statusChg = true;
					me->evt.Set();
				}
				NN<Text::StringBuilderUTF8> sb;
				if (sb.Set(me->msgData))
				{
					me->msgData->AppendP(sbuff, sptr);
					reader.GetLastLineBreak(sb);					
				}
			}
			else
			{
				if (sbuff[0] == '+')
				{
					me->lastStatus = ResultStatus::Success;
				}
				else if (sbuff[0] == '-')
				{
					me->lastStatus = ResultStatus::Failed;
				}
				else
				{
					me->lastStatus = ResultStatus::TimedOut;
				}
				i = Text::StrIndexOfCharC(sbuff, (UOSInt)(sptr - sbuff), ' ');
				if (i != INVALID_INDEX)
				{
					UnsafeArray<UTF8Char> nnmsgRet;
					if (me->msgRet.SetTo(nnmsgRet))
					{
						me->msgRet = nnmsgRet = Text::StrConcatC(nnmsgRet, &sbuff[i + 1], (UOSInt)(sptr - &sbuff[i + 1]));
						nnmsgRet[0] = 0;
					}
				}
				if (me->msgToDataMode)
				{
					me->msgToDataMode = false;
					if (me->lastStatus == ResultStatus::Success)
					{
						dataMode = true;
					}
					else
					{
						me->statusChg = true;
						me->evt.Set();
					}
				}
				else
				{
					me->statusChg = true;
					me->evt.Set();
				}
			}
		}
		me->lastStatus = ResultStatus::TimedOut;
		me->statusChg = true;
		me->evt.Set();
	}
	me->threadRunning = false;
	return 0;
}

Net::Email::POP3Conn::ResultStatus Net::Email::POP3Conn::WaitForResult(OptOut<UnsafeArrayOpt<UTF8Char>> msgRetEnd)
{
	Manage::HiResClock clk;
	while (this->threadRunning && !this->statusChg && clk.GetTimeDiff() < 30.0)
	{
		this->evt.Wait(1000);
	}
	msgRetEnd.Set(this->msgRet);
	this->msgRet = 0;
	if (this->statusChg)
	{
		return this->lastStatus;
	}
	else
		return ResultStatus::TimedOut;
}

Net::Email::POP3Conn::POP3Conn(NN<Net::SocketFactory> sockf, Optional<Net::SSLEngine> ssl, Text::CStringNN host, UInt16 port, ConnType connType, IO::Writer *logWriter, Data::Duration timeout)
{
	this->threadStarted = false;
	this->threadRunning = false;
	this->threadToStop = false;
	this->msgRet = 0;
	this->msgToDataMode = false;
	this->statusChg = false;
	this->maxSize = 0;
	this->authLogin = false;
	this->authPlain = false;
	Net::SocketUtil::AddressInfo addr;
	addr.addrType = Net::AddrType::Unknown;
	sockf->DNSResolveIP(host, addr);
	this->logWriter = logWriter;
	NN<Net::SSLEngine> nnssl;
	if (connType == CT_SSL && ssl.SetTo(nnssl))
	{
		if (!Optional<Net::TCPClient>(nnssl->ClientConnect(host, port, 0, timeout)).SetTo(this->cli))
		{
			NEW_CLASSNN(this->cli, Net::TCPClient(sockf, addr, port, timeout));
		}
	}
	else if (connType == CT_STARTTLS && ssl.SetTo(nnssl))
	{
		UInt8 buff[1024];
		UOSInt buffSize;
		NEW_CLASSNN(this->cli, Net::TCPClient(sockf, addr, port, timeout));
		this->cli->SetTimeout(timeout);
		buffSize = this->cli->Read(BYTEARR(buff));
		if (this->logWriter)
		{
			this->logWriter->Write(Text::CStringNN(buff, buffSize));
		}
		if (buffSize > 2 && Text::StrStartsWithC(buff, buffSize, UTF8STRC("+OK ")) && buff[buffSize - 2] == '\r' && buff[buffSize - 1] == '\n')
		{
			if (this->logWriter)
			{
				this->logWriter->WriteLine(CSTR("STARTTLS"));
			}
			this->cli->Write((const UInt8*)"STARTTLS\r\n", 10);
			buffSize = this->cli->Read(BYTEARR(buff));
			if (this->logWriter)
			{
				this->logWriter->Write(Text::CStringNN(buff, buffSize));
			}
			if (buffSize > 0 && Text::StrStartsWithC(buff, buffSize, UTF8STRC("+OK ")) && buff[buffSize - 2] == '\r' && buff[buffSize - 1] == '\n')
			{
				if (this->logWriter)
				{
					this->logWriter->WriteLine(CSTR("SSL Handshake begin"));
				}
				NN<Socket> s;
				NN<Net::SSLClient> cli;
				if (this->cli->RemoveSocket().SetTo(s) && nnssl->ClientInit(s, host, 0).SetTo(cli))
				{
					if (this->logWriter)
					{
						this->logWriter->WriteLine(CSTR("SSL Handshake success"));
					}
					this->cli.Delete();
					this->cli = cli;
				}
				else
				{
					if (this->logWriter)
					{
						this->logWriter->WriteLine(CSTR("SSL Handshake failed"));
					}
				}
			}
			else
			{
				this->cli->Close();
			}
		}
		else
		{
			this->cli->Close();
		}
	}
	else
	{
		NEW_CLASSNN(this->cli, Net::TCPClient(sockf, addr, port, timeout));
	}
	this->cli->SetNoDelay(false);
	NEW_CLASS(this->writer, Text::UTF8Writer(this->cli));
	if (this->logWriter)
	{
		Text::StringBuilderUTF8 sb;
		UTF8Char sbuff[128];
		UnsafeArray<UTF8Char> sptr;
		sb.AppendC(UTF8STRC("Connect to "));
		sb.Append(host);
		sb.AppendC(UTF8STRC("("));
		sptr = Net::SocketUtil::GetAddrName(sbuff, addr).Or(sbuff);
		sb.AppendC(sbuff, (UOSInt)(sptr - sbuff));
		sb.AppendC(UTF8STRC("):"));
		sb.AppendU16(port);
		this->logWriter->WriteLine(sb.ToCString());
	}
	Sync::ThreadUtil::Create(RecvThread, this);
	while (!this->threadStarted)
	{
		Sync::SimpleThread::Sleep(10);
	}
	if (connType == CT_STARTTLS)
	{
		this->welcomeMsg = ResultStatus::Success;
		this->logWriter->WriteLine(CSTR("Connected"));
	}
	else
	{
		this->welcomeMsg = WaitForResult(0);
	}
}

Net::Email::POP3Conn::~POP3Conn()
{
	this->threadToStop = true;
	this->cli->Close();
	while (this->threadRunning)
	{
		Sync::SimpleThread::Sleep(10);
	}
	DEL_CLASS(this->writer);
	this->cli.Delete();
}

Bool Net::Email::POP3Conn::IsError()
{
	return this->welcomeMsg != ResultStatus::Success || this->cli->IsConnectError();
}

Net::Email::POP3Conn::ResultStatus Net::Email::POP3Conn::SendUser(Text::CStringNN username)
{
	UTF8Char sbuff[512];
	UnsafeArray<UTF8Char> sptr;
	sptr = username.ConcatTo(Text::StrConcatC(sbuff, UTF8STRC("USER ")));
	this->statusChg = false;
	if (this->logWriter)
	{
		this->logWriter->WriteLine(CSTRP(sbuff, sptr));
	}
	writer->WriteLine(CSTRP(sbuff, sptr));
	return WaitForResult(0);
}

Net::Email::POP3Conn::ResultStatus Net::Email::POP3Conn::SendPass(Text::CStringNN password)
{
	UTF8Char sbuff[512];
	UnsafeArray<UTF8Char> sptr;
	sptr = password.ConcatTo(Text::StrConcatC(sbuff, UTF8STRC("PASS ")));
	this->statusChg = false;
	if (this->logWriter)
	{
		this->logWriter->WriteLine(CSTRP(sbuff, sptr));
	}
	writer->WriteLine(CSTRP(sbuff, sptr));
	return WaitForResult(0);
}

Net::Email::POP3Conn::ResultStatus Net::Email::POP3Conn::SendNoop()
{
	this->statusChg = false;
	if (this->logWriter)
	{
		this->logWriter->WriteLine(CSTR("NOOP"));
	}
	writer->WriteLine(CSTR("NOOP"));
	return WaitForResult(0);
}

Net::Email::POP3Conn::ResultStatus Net::Email::POP3Conn::SendStat(UOSInt *msgCount, UOSInt *msgSize)
{
	UTF8Char returnMsg[2048];
	UnsafeArrayOpt<UTF8Char> returnMsgEnd;
	UnsafeArray<UTF8Char> nnreturnMsgEnd;
	this->statusChg = false;
	returnMsg[0] = 0;
	this->msgRet = returnMsg;
	if (this->logWriter)
	{
		this->logWriter->WriteLine(CSTR("STAT"));
	}
	writer->WriteLine(CSTR("STAT"));
	ResultStatus status = WaitForResult(returnMsgEnd);
	if (status == ResultStatus::Success)
	{
		UOSInt i;
		if (returnMsgEnd.SetTo(nnreturnMsgEnd) && (i = Text::StrIndexOfCharC(returnMsg, (UOSInt)(nnreturnMsgEnd - returnMsg), ' ')) != INVALID_INDEX)
		{
			returnMsg[i] = 0;
			*msgCount = Text::StrToUOSInt(returnMsg);
			*msgSize = Text::StrToUOSInt(&returnMsg[i + 1]);
		}
		else
		{
			*msgCount = 0;
			*msgSize = 0;
		}
	}
	return status;
}

Net::Email::POP3Conn::ResultStatus Net::Email::POP3Conn::SendRetr(UOSInt msgIndex, NN<Text::StringBuilderUTF8> msgBuff)
{
	UTF8Char sbuff[512];
	UnsafeArray<UTF8Char> sptr;
	sptr = Text::StrUOSInt(Text::StrConcatC(sbuff, UTF8STRC("RETR ")), msgIndex + 1);
	this->statusChg = false;
	this->msgToDataMode = true;
	this->msgData = msgBuff.Ptr();
	if (this->logWriter)
	{
		this->logWriter->WriteLine(CSTRP(sbuff, sptr));
	}
	writer->WriteLine(CSTRP(sbuff, sptr));
	return WaitForResult(0);
}

Net::Email::POP3Conn::ResultStatus Net::Email::POP3Conn::SendDele(UOSInt msgIndex)
{
	UTF8Char sbuff[512];
	UnsafeArray<UTF8Char> sptr;
	sptr = Text::StrUOSInt(Text::StrConcatC(sbuff, UTF8STRC("DELE ")), msgIndex + 1);
	this->statusChg = false;
	if (this->logWriter)
	{
		this->logWriter->WriteLine(CSTRP(sbuff, sptr));
	}
	writer->WriteLine(CSTRP(sbuff, sptr));
	return WaitForResult(0);
}

Net::Email::POP3Conn::ResultStatus Net::Email::POP3Conn::SendQuit()
{
	this->statusChg = false;
	if (this->logWriter)
	{
		this->logWriter->WriteLine(CSTR("QUIT"));
	}
	writer->WriteLine(CSTR("QUIT"));
	return WaitForResult(0);
}

UInt16 Net::Email::POP3Conn::GetDefaultPort(ConnType connType)
{
	switch (connType)
	{
	default:
	case ConnType::CT_PLAIN:
	case ConnType::CT_STARTTLS:
		return 110;
	case ConnType::CT_SSL:
		return 995;
	}
}
