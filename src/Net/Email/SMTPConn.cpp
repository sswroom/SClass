#include "Stdafx.h"
#include "MyMemory.h"
#include "Manage/HiResClock.h"
#include "Net/Email/SMTPConn.h"
#include "Sync/Thread.h"
#include "Text/MyString.h"
#include "Text/StringBuilderUTF8.h"
#include "Text/UTF8Reader.h"

UInt32 __stdcall Net::Email::SMTPConn::SMTPThread(void *userObj)
{
	Net::Email::SMTPConn *me = (Net::Email::SMTPConn *)userObj;
	Text::UTF8Reader *reader;
	UTF8Char sbuff[2048];
	UTF8Char sbuff2[4];
	UTF8Char *sptr;
	UInt32 msgCode;

	me->threadStarted = true;
	me->threadRunning = true;
	NEW_CLASS(reader, Text::UTF8Reader(me->cli));
	while (!me->threadToStop)
	{
		sptr = reader->ReadLine(sbuff, 2048);
		if (sptr == 0)
		{
			if (me->logWriter)
			{
				me->logWriter->WriteLine((const UTF8Char*)"Connection Closed");
			}
			break;
		}

		if (me->logWriter)
		{
			me->logWriter->WriteLine(sbuff);
		}
		if (sbuff[0] == ' ')
		{
			if (me->msgRet)
			{
				me->msgRet = Text::StrConcat(me->msgRet, sbuff);
				me->msgRet = reader->GetLastLineBreak(me->msgRet);
			}
		}
		else
		{
			sbuff2[0] = sbuff[0];
			sbuff2[1] = sbuff[1];
			sbuff2[2] = sbuff[2];
			sbuff2[3] = 0;
			msgCode = Text::StrToUInt32(sbuff2);
			if (msgCode == 235)
			{
				me->logged = true;
			}
			if (sbuff[3] == ' ')
			{
				if (me->msgRet)
				{
					me->msgRet = Text::StrConcat(me->msgRet, &sbuff[4]);
				}
				me->lastStatus = msgCode;
				me->statusChg = true;
				me->evt->Set();
			}
			else if (sbuff[3] == '-')
			{
				if (me->msgRet)
				{
					me->msgRet = Text::StrConcat(me->msgRet, &sbuff[4]);
					me->msgRet = reader->GetLastLineBreak(me->msgRet);
				}
			}
		}
	}
	me->lastStatus = 0;
	me->statusChg = true;
	me->evt->Set();
	DEL_CLASS(reader);
	me->threadRunning = false;
	return 0;
}

UInt32 Net::Email::SMTPConn::WaitForResult()
{
	Manage::HiResClock clk;
	while (this->threadRunning && !this->statusChg && clk.GetTimeDiff() < 30.0)
	{
		this->evt->Wait(1000);
	}
	this->msgRet = 0;
	if (this->statusChg)
	{
		return this->lastStatus;
	}
	else
		return 0;
}

Net::Email::SMTPConn::SMTPConn(Net::SocketFactory *sockf, Net::SSLEngine *ssl, const UTF8Char *host, UInt16 port, ConnType connType, IO::Writer *logWriter)
{
	this->threadStarted = false;
	this->threadRunning = false;
	this->threadToStop = false;
	this->logged = false;
	this->msgRet = 0;
	this->statusChg = false;
	Net::SocketUtil::AddressInfo addr;
	addr.addrType = Net::SocketUtil::AT_UNKNOWN;
	sockf->DNSResolveIP(host, &addr);
	this->logWriter = logWriter;
	NEW_CLASS(this->evt, Sync::Event(true, (const UTF8Char*)"Net.SMTPConn.evt"));
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
		if (buffSize > 2 && Text::StrStartsWith(buff, (const UTF8Char*)"220 ") && buff[buffSize - 2] == '\r' && buff[buffSize - 1] == '\n')
		{
			this->cli->Write((const UTF8Char*)"STARTTLS\r\n", 10);
			buffSize = this->cli->Read(buff, 1024);
			if (buffSize > 0 && Text::StrStartsWith(buff, (const UTF8Char*)"220 ") && buff[buffSize - 2] == '\r' && buff[buffSize - 1] == '\n')
			{
				Socket *s = this->cli->RemoveSocket();
				Net::SSLEngine::ErrorType err;
				Net::TCPClient *cli = ssl->ClientInit(s, host, &err);
				if (cli)
				{
					DEL_CLASS(this->cli);
					this->cli = cli;
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
	NEW_CLASS(this->writer, Text::UTF8Writer(this->cli));
	if (this->logWriter)
	{
		Text::StringBuilderUTF8 sb;
		UTF8Char sbuff[128];
		sb.Append((const UTF8Char*)"Connect to ");
		sb.Append(host);
		sb.Append((const UTF8Char*)"(");
		Net::SocketUtil::GetAddrName(sbuff, &addr);
		sb.Append(sbuff);
		sb.Append((const UTF8Char*)"):");
		sb.AppendU16(port);
		this->logWriter->WriteLine(sb.ToString());
	}
	Sync::Thread::Create(SMTPThread, this);
	while (!this->threadStarted)
	{
		Sync::Thread::Sleep(10);
	}
	if (connType == CT_STARTTLS)
	{
		this->initCode = 220;
	}
	else
	{
		this->initCode = WaitForResult();
	}
}

Net::Email::SMTPConn::~SMTPConn()
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
	DEL_CLASS(this->evt);
}

Bool Net::Email::SMTPConn::IsError()
{
	return this->initCode != 220 || this->cli->IsConnectError();
}

Bool Net::Email::SMTPConn::SendHelo(const UTF8Char *cliName)
{
	UTF8Char sbuff[512];
	Text::StrConcat(Text::StrConcat(sbuff, (const UTF8Char*)"HELO "), cliName);
	this->statusChg = false;
	if (this->logWriter)
	{
		this->logWriter->WriteLine(sbuff);
	}
	writer->WriteLine(sbuff);
	UInt32 code = WaitForResult();
	return code == 250;
}

Bool Net::Email::SMTPConn::SendEHlo(const UTF8Char *cliName)
{
	UTF8Char sbuff[512];
	Text::StrConcat(Text::StrConcat(sbuff, (const UTF8Char*)"EHLO "), cliName);
	this->statusChg = false;
	if (this->logWriter)
	{
		this->logWriter->WriteLine(sbuff);
	}
	writer->WriteLine(sbuff);
	UInt32 code = WaitForResult();
	return code == 250;
}

Bool Net::Email::SMTPConn::SendMailFrom(const UTF8Char *fromEmail)
{
	UTF8Char sbuff[512];
	Text::StrConcat(Text::StrConcat(Text::StrConcat(sbuff, (const UTF8Char*)"MAIL FROM: <"), fromEmail), (const UTF8Char*)">");
	this->statusChg = false;
	if (this->logWriter)
	{
		this->logWriter->WriteLine(sbuff);
	}
	writer->WriteLine(sbuff);
	UInt32 code = WaitForResult();
	return code == 250;
}

Bool Net::Email::SMTPConn::SendRcptTo(const UTF8Char *toEmail)
{
	UTF8Char sbuff[512];
	Text::StrConcat(Text::StrConcat(Text::StrConcat(sbuff, (const UTF8Char*)"RCPT TO: <"), toEmail), (const UTF8Char*)">");
	this->statusChg = false;
	if (this->logWriter)
	{
		this->logWriter->WriteLine(sbuff);
	}
	writer->WriteLine(sbuff);
	UInt32 code = WaitForResult();
	return code == 250;
}

Bool Net::Email::SMTPConn::SendQuit()
{
	this->statusChg = false;
	if (this->logWriter)
	{
		this->logWriter->WriteLine((const UTF8Char*)"QUIT");
	}
	writer->WriteLine((const UTF8Char*)"QUIT");
	UInt32 code = WaitForResult();
	return code == 221;
}

Bool Net::Email::SMTPConn::SendData(const UTF8Char *buff, UOSInt buffSize)
{
	this->statusChg = false;
	if (this->logWriter)
	{
		this->logWriter->WriteLine((const UTF8Char*)"DATA");
	}
	writer->WriteLine((const UTF8Char*)"DATA");
	UInt32 code = WaitForResult();
	if (code != 354)
	{
		return false;
	}
	this->cli->Write(buff, buffSize);
	this->cli->Write((const UInt8*)"\r\n.\r\n", 5);
	code = WaitForResult();
	return code == 250;
	
}
