#include "Stdafx.h"
#include "MyMemory.h"
#include "Manage/HiResClock.h"
#include "Net/Email/POP3Conn.h"
#include "Sync/Thread.h"
#include "Text/MyString.h"
#include "Text/StringBuilderUTF8.h"
#include "Text/UTF8Reader.h"
#include "Text/TextBinEnc/Base64Enc.h"

//#define VERBOSE
#ifdef VERBOSE
#include <stdio.h>
#endif

UInt32 __stdcall Net::Email::POP3Conn::RecvThread(void *userObj)
{
	Net::Email::POP3Conn *me = (Net::Email::POP3Conn *)userObj;
	UTF8Char sbuff[2048];
	UTF8Char *sptr;
	UOSInt i;
	Bool dataMode = false;

	me->threadStarted = true;
	me->threadRunning = true;
	{
		Text::UTF8Reader reader(me->cli);
		while (!me->threadToStop)
		{
			sptr = reader.ReadLine(sbuff, 2048);
			if (sptr == 0)
			{
				if (me->logWriter)
				{
					me->logWriter->WriteLineC(UTF8STRC("Connection Closed"));
				}
				break;
			}

			if (me->logWriter)
			{
				me->logWriter->WriteLineC(sbuff, (UOSInt)(sptr - sbuff));
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
				if (me->msgData)
				{
					me->msgData->AppendP(sbuff, sptr);
					reader.GetLastLineBreak(me->msgData);					
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
					if (me->msgRet)
					{
						me->msgRet = Text::StrConcatC(me->msgRet, &sbuff[i + 1], (UOSInt)(sptr - &sbuff[i + 1]));
						me->msgRet[0] = 0;
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

Net::Email::POP3Conn::ResultStatus Net::Email::POP3Conn::WaitForResult(UTF8Char **msgRetEnd)
{
	Manage::HiResClock clk;
	while (this->threadRunning && !this->statusChg && clk.GetTimeDiff() < 30.0)
	{
		this->evt.Wait(1000);
	}
	if (msgRetEnd)
	{
		*msgRetEnd = this->msgRet;
	}
	this->msgRet = 0;
	if (this->statusChg)
	{
		return this->lastStatus;
	}
	else
		return ResultStatus::TimedOut;
}

Net::Email::POP3Conn::POP3Conn(Net::SocketFactory *sockf, Net::SSLEngine *ssl, Text::CString host, UInt16 port, ConnType connType, IO::Writer *logWriter)
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
	sockf->DNSResolveIP(host, &addr);
	this->logWriter = logWriter;
	if (connType == CT_SSL)
	{
		Net::SSLEngine::ErrorType err;
		this->cli = ssl->Connect(host, port, &err);
	}
	else if (connType == CT_STARTTLS)
	{
		UInt8 buff[1024];
		UOSInt buffSize;
		NEW_CLASS(this->cli, Net::TCPClient(sockf, &addr, port));
		this->cli->SetTimeout(2000);
		buffSize = this->cli->Read(buff, 1024);
		if (this->logWriter)
		{
			this->logWriter->WriteStrC(buff, buffSize);
		}
		if (buffSize > 2 && Text::StrStartsWithC(buff, buffSize, UTF8STRC("+OK ")) && buff[buffSize - 2] == '\r' && buff[buffSize - 1] == '\n')
		{
			if (this->logWriter)
			{
				this->logWriter->WriteLineC(UTF8STRC("STARTTLS"));
			}
			this->cli->Write((const UInt8*)"STARTTLS\r\n", 10);
			buffSize = this->cli->Read(buff, 1024);
			if (this->logWriter)
			{
				this->logWriter->WriteStrC(buff, buffSize);
			}
			if (buffSize > 0 && Text::StrStartsWithC(buff, buffSize, UTF8STRC("+OK ")) && buff[buffSize - 2] == '\r' && buff[buffSize - 1] == '\n')
			{
				if (this->logWriter)
				{
					this->logWriter->WriteLineC(UTF8STRC("SSL Handshake begin"));
				}
				Socket *s = this->cli->RemoveSocket();
				Net::SSLEngine::ErrorType err;
				Net::TCPClient *cli = ssl->ClientInit(s, host, &err);
				if (cli)
				{
					if (this->logWriter)
					{
						this->logWriter->WriteLineC(UTF8STRC("SSL Handshake success"));
					}
					DEL_CLASS(this->cli);
					this->cli = cli;
				}
				else
				{
					if (this->logWriter)
					{
						this->logWriter->WriteLineC(UTF8STRC("SSL Handshake failed"));
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
		NEW_CLASS(this->cli, Net::TCPClient(sockf, &addr, port));
	}
	this->cli->SetNoDelay(false);
	NEW_CLASS(this->writer, Text::UTF8Writer(this->cli));
	if (this->logWriter)
	{
		Text::StringBuilderUTF8 sb;
		UTF8Char sbuff[128];
		UTF8Char *sptr;
		sb.AppendC(UTF8STRC("Connect to "));
		sb.Append(host);
		sb.AppendC(UTF8STRC("("));
		sptr = Net::SocketUtil::GetAddrName(sbuff, &addr);
		sb.AppendC(sbuff, (UOSInt)(sptr - sbuff));
		sb.AppendC(UTF8STRC("):"));
		sb.AppendU16(port);
		this->logWriter->WriteLineC(sb.ToString(), sb.GetLength());
	}
	Sync::Thread::Create(RecvThread, this);
	while (!this->threadStarted)
	{
		Sync::Thread::Sleep(10);
	}
	if (connType == CT_STARTTLS)
	{
		this->welcomeMsg = ResultStatus::Success;
		this->logWriter->WriteLineC(UTF8STRC("Connected"));
	}
	else
	{
		this->welcomeMsg = WaitForResult(0);
	}
}

Net::Email::POP3Conn::~POP3Conn()
{
	this->threadToStop = true;
	if (cli)
	{

	}
	this->cli->Close();
	while (this->threadRunning)
	{
		Sync::Thread::Sleep(10);
	}
	DEL_CLASS(this->writer);
	DEL_CLASS(this->cli);
}

Bool Net::Email::POP3Conn::IsError()
{
	return this->welcomeMsg != ResultStatus::Success || this->cli->IsConnectError();
}

Net::Email::POP3Conn::ResultStatus Net::Email::POP3Conn::SendUser(Text::CString username)
{
	UTF8Char sbuff[512];
	UTF8Char *sptr;
	sptr = username.ConcatTo(Text::StrConcatC(sbuff, UTF8STRC("USER ")));
	this->statusChg = false;
	if (this->logWriter)
	{
		this->logWriter->WriteLineC(sbuff, (UOSInt)(sptr - sbuff));
	}
	writer->WriteLineC(sbuff, (UOSInt)(sptr - sbuff));
	return WaitForResult(0);
}

Net::Email::POP3Conn::ResultStatus Net::Email::POP3Conn::SendPass(Text::CString password)
{
	UTF8Char sbuff[512];
	UTF8Char *sptr;
	sptr = password.ConcatTo(Text::StrConcatC(sbuff, UTF8STRC("PASS ")));
	this->statusChg = false;
	if (this->logWriter)
	{
		this->logWriter->WriteLineC(sbuff, (UOSInt)(sptr - sbuff));
	}
	writer->WriteLineC(sbuff, (UOSInt)(sptr - sbuff));
	return WaitForResult(0);
}

Net::Email::POP3Conn::ResultStatus Net::Email::POP3Conn::SendNoop()
{
	this->statusChg = false;
	if (this->logWriter)
	{
		this->logWriter->WriteLineC(UTF8STRC("NOOP"));
	}
	writer->WriteLineC(UTF8STRC("NOOP"));
	return WaitForResult(0);
}

Net::Email::POP3Conn::ResultStatus Net::Email::POP3Conn::SendStat(UOSInt *msgCount, UOSInt *msgSize)
{
	UTF8Char returnMsg[2048];
	UTF8Char *returnMsgEnd;
	this->statusChg = false;
	returnMsg[0] = 0;
	this->msgRet = returnMsg;
	if (this->logWriter)
	{
		this->logWriter->WriteLineC(UTF8STRC("STAT"));
	}
	writer->WriteLineC(UTF8STRC("STAT"));
	ResultStatus status = WaitForResult(&returnMsgEnd);
	if (status == ResultStatus::Success)
	{
		UOSInt i = Text::StrIndexOfCharC(returnMsg, (UOSInt)(returnMsgEnd - returnMsg), ' ');
		if (i != INVALID_INDEX)
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

Net::Email::POP3Conn::ResultStatus Net::Email::POP3Conn::SendRetr(UOSInt msgIndex, Text::StringBuilderUTF8 *msgBuff)
{
	UTF8Char sbuff[512];
	UTF8Char *sptr;
	sptr = Text::StrUOSInt(Text::StrConcatC(sbuff, UTF8STRC("RETR ")), msgIndex + 1);
	this->statusChg = false;
	this->msgToDataMode = true;
	this->msgData = msgBuff;
	if (this->logWriter)
	{
		this->logWriter->WriteLineC(sbuff, (UOSInt)(sptr - sbuff));
	}
	writer->WriteLineC(sbuff, (UOSInt)(sptr - sbuff));
	return WaitForResult(0);
}

Net::Email::POP3Conn::ResultStatus Net::Email::POP3Conn::SendDele(UOSInt msgIndex)
{
	UTF8Char sbuff[512];
	UTF8Char *sptr;
	sptr = Text::StrUOSInt(Text::StrConcatC(sbuff, UTF8STRC("DELE ")), msgIndex + 1);
	this->statusChg = false;
	if (this->logWriter)
	{
		this->logWriter->WriteLineC(sbuff, (UOSInt)(sptr - sbuff));
	}
	writer->WriteLineC(sbuff, (UOSInt)(sptr - sbuff));
	return WaitForResult(0);
}

Net::Email::POP3Conn::ResultStatus Net::Email::POP3Conn::SendQuit()
{
	this->statusChg = false;
	if (this->logWriter)
	{
		this->logWriter->WriteLineC(UTF8STRC("QUIT"));
	}
	writer->WriteLineC(UTF8STRC("QUIT"));
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