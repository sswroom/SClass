#include "stdafx.h"
#include "MyMemory.h"
#include "IO/StreamReader.h"
#include "Manage/HiResClock.h"
#include "Net/SMTPConn.h"
#include "Sync/Thread.h"
#include "Text/MyString.h"

UInt32 __stdcall Net::SMTPConn::SMTPThread(void *userObj)
{
	Net::SMTPConn *me = (Net::SMTPConn *)userObj;
	IO::StreamReader *reader;
	WChar sbuff[2048];
	WChar sbuff2[4];
	WChar *sptr;
	Int32 msgCode;

	me->threadStarted = true;
	me->threadRunning = true;
	NEW_CLASS(reader, IO::StreamReader(me->cli, me->codePage));
	while (!me->threadToStop)
	{
		sptr = reader->ReadLine(sbuff, 2048);
		if (sptr == 0)
		{
			if (me->logWriter)
			{
				me->logWriter->WriteLine(L"Connection Closed");
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

Int32 Net::SMTPConn::WaitForResult()
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

Net::SMTPConn::SMTPConn(const WChar *host, UInt16 port, Net::SocketFactory *sockf, Int32 codePage, IO::Writer *logWriter)
{
	this->codePage = codePage;
	this->threadStarted = false;
	this->threadRunning = false;
	this->threadToStop = false;
	this->logged = false;
	this->msgRet = 0;
	this->statusChg = false;
	UInt32 ip = sockf->GetIPByHost(host);
	this->logWriter = logWriter;
	NEW_CLASS(this->evt, Sync::Event(true, L"Net.SMTPConn.evt"));
	NEW_CLASS(this->cli, Net::TCPClient(sockf, ip, port));
	NEW_CLASS(this->writer, IO::StreamWriter(this->cli, codePage));
	if (this->logWriter)
	{
		Text::StringBuilder sb;
		WChar sbuff[20];
		sb.Append(L"Connect to ");
		sb.Append(host);
		sb.Append(L"(");
		Net::SocketFactory::GetIPv4Name(sbuff, ip);
		sb.Append(sbuff);
		sb.Append(L"):");
		sb.Append(port);
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
	return this->initCode != 220 || this->cli->GetLastError() != 0;
}

Bool Net::SMTPConn::SendHelo(const WChar *cliName)
{
	WChar sbuff[512];
	Text::StrConcat(Text::StrConcat(sbuff, L"HELO "), cliName);
	this->statusChg = false;
	if (this->logWriter)
	{
		this->logWriter->WriteLine(sbuff);
	}
	writer->WriteLine(sbuff);
	Int32 code = WaitForResult();
	return code == 250;
}

Bool Net::SMTPConn::SendEHlo(const WChar *cliName)
{
	WChar sbuff[512];
	Text::StrConcat(Text::StrConcat(sbuff, L"EHLO "), cliName);
	this->statusChg = false;
	if (this->logWriter)
	{
		this->logWriter->WriteLine(sbuff);
	}
	writer->WriteLine(sbuff);
	Int32 code = WaitForResult();
	return code == 250;
}

Bool Net::SMTPConn::SendMailFrom(const WChar *fromEmail)
{
	WChar sbuff[512];
	Text::StrConcat(Text::StrConcat(Text::StrConcat(sbuff, L"MAIL FROM: <"), fromEmail), L">");
	this->statusChg = false;
	if (this->logWriter)
	{
		this->logWriter->WriteLine(sbuff);
	}
	writer->WriteLine(sbuff);
	Int32 code = WaitForResult();
	return code == 250;
}

Bool Net::SMTPConn::SendRcptTo(const WChar *toEmail)
{
	WChar sbuff[512];
	Text::StrConcat(Text::StrConcat(Text::StrConcat(sbuff, L"RCPT TO: <"), toEmail), L">");
	this->statusChg = false;
	if (this->logWriter)
	{
		this->logWriter->WriteLine(sbuff);
	}
	writer->WriteLine(sbuff);
	Int32 code = WaitForResult();
	return code == 250;
}

Bool Net::SMTPConn::SendQuit()
{
	this->statusChg = false;
	if (this->logWriter)
	{
		this->logWriter->WriteLine(L"QUIT");
	}
	writer->WriteLine(L"QUIT");
	Int32 code = WaitForResult();
	return code == 221;
}
