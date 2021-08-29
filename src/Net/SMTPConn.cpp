#include "Stdafx.h"
#include "MyMemory.h"
#include "Manage/HiResClock.h"
#include "Net/SMTPConn.h"
#include "Sync/Thread.h"
#include "Text/MyString.h"
#include "Text/StringBuilderUTF8.h"
#include "Text/UTF8Reader.h"

UInt32 __stdcall Net::SMTPConn::SMTPThread(void *userObj)
{
	Net::SMTPConn *me = (Net::SMTPConn *)userObj;
	Text::UTF8Reader *reader;
	UTF8Char sbuff[2048];
	UTF8Char sbuff2[4];
	UTF8Char *sptr;
	Int32 msgCode;

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
			msgCode = Text::StrToInt32(sbuff2);
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

UInt32 Net::SMTPConn::WaitForResult()
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

Net::SMTPConn::SMTPConn(Net::SocketFactory *sockf, Net::SSLEngine *ssl, const UTF8Char *host, UInt16 port, IO::Writer *logWriter)
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
	NEW_CLASS(this->cli, Net::TCPClient(sockf, &addr, port));
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
	this->initCode = WaitForResult();
}

Net::SMTPConn::~SMTPConn()
{
	this->threadToStop = true;
	this->cli->Close();
	while (this->threadRunning)
	{
		Sync::Thread::Sleep(10);
	}
	DEL_CLASS(this->writer);
	DEL_CLASS(this->cli);
	DEL_CLASS(this->evt);
}

Bool Net::SMTPConn::IsError()
{
	return this->initCode != 220 || this->cli->IsConnectError();
}

Bool Net::SMTPConn::SendHelo(const UTF8Char *cliName)
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

Bool Net::SMTPConn::SendEHlo(const UTF8Char *cliName)
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

Bool Net::SMTPConn::SendMailFrom(const UTF8Char *fromEmail)
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

Bool Net::SMTPConn::SendRcptTo(const UTF8Char *toEmail)
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

Bool Net::SMTPConn::SendQuit()
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

Bool Net::SMTPConn::SendData(const UTF8Char *buff, UOSInt buffSize)
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
