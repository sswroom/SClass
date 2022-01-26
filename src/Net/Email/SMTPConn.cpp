#include "Stdafx.h"
#include "MyMemory.h"
#include "Manage/HiResClock.h"
#include "Net/Email/SMTPConn.h"
#include "Sync/Thread.h"
#include "Text/MyString.h"
#include "Text/StringBuilderUTF8.h"
#include "Text/UTF8Reader.h"
#include "Text/TextBinEnc/Base64Enc.h"

#include <stdio.h>

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
				me->logWriter->WriteLineC(UTF8STRC("Connection Closed"));
			}
			break;
		}

		if (me->logWriter)
		{
			me->logWriter->WriteLineC(sbuff, (UOSInt)(sptr - sbuff));
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
					me->msgRet = 0;
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
	this->maxSize = 0;
	this->authLogin = false;
	this->authPlain = false;
	UOSInt hostLen = Text::StrCharCnt(host);
	Net::SocketUtil::AddressInfo addr;
	addr.addrType = Net::AddrType::Unknown;
	sockf->DNSResolveIP(host, hostLen, &addr);
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
		if (this->logWriter)
		{
			this->logWriter->WriteStrC(buff, buffSize);
		}
		if (buffSize > 2 && Text::StrStartsWith(buff, (const UTF8Char*)"220 ") && buff[buffSize - 2] == '\r' && buff[buffSize - 1] == '\n')
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
			if (buffSize > 0 && Text::StrStartsWith(buff, (const UTF8Char*)"220 ") && buff[buffSize - 2] == '\r' && buff[buffSize - 1] == '\n')
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
		sb.AppendC(host, hostLen);
		sb.AppendC(UTF8STRC("("));
		sptr = Net::SocketUtil::GetAddrName(sbuff, &addr);
		sb.AppendC(sbuff, (UOSInt)(sptr - sbuff));
		sb.AppendC(UTF8STRC("):"));
		sb.AppendU16(port);
		this->logWriter->WriteLineC(sb.ToString(), sb.GetLength());
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
	UTF8Char *sptr;
	sptr = Text::StrConcat(Text::StrConcatC(sbuff, UTF8STRC("HELO ")), cliName);
	this->statusChg = false;
	if (this->logWriter)
	{
		this->logWriter->WriteLineC(sbuff, (UOSInt)(sptr - sbuff));
	}
	writer->WriteLineC(sbuff, (UOSInt)(sptr - sbuff));
	UInt32 code = WaitForResult();
	return code == 250;
}

Bool Net::Email::SMTPConn::SendEHlo(const UTF8Char *cliName)
{
	UTF8Char returnMsg[2048];
	UTF8Char sbuff[512];
	UTF8Char *sptr;
	sptr = Text::StrConcatC(Text::StrConcat(Text::StrConcatC(sbuff, UTF8STRC("EHLO ")), cliName), UTF8STRC("\r\n"));
	this->statusChg = false;
	returnMsg[0] = 0;
	this->msgRet = returnMsg;
	if (this->logWriter)
	{
		this->logWriter->WriteStrC(sbuff, (UOSInt)(sptr - sbuff));
	}
	writer->WriteStrC(sbuff, (UOSInt)(sptr - sbuff));
	UInt32 code = WaitForResult();
	if (code == 0)
	{
		this->statusChg = false;
		this->msgRet = returnMsg;
		writer->WriteStrC(sbuff, (UOSInt)(sptr - sbuff));
		code = WaitForResult();
	}
	if (code == 250)
	{
		UTF8Char *sarr[2];
		UTF8Char *sarr2[2];
		sarr[1] = returnMsg;
		UOSInt i;
		UOSInt j;
		while (true)
		{
			i = Text::StrSplitLine(sarr, 2, sarr[1]);
			if (Text::StrStartsWith(sarr[0], (const UTF8Char*)"SIZE "))
			{
				this->maxSize = Text::StrToUOSInt(sarr[0] + 5);
			}
			else if (Text::StrStartsWith(sarr[0], (const UTF8Char*)"AUTH "))
			{
				sarr2[1] = sarr[0] + 5;
				while (true)
				{
					j = Text::StrSplit(sarr2, 2, sarr2[1], ' ');
					if (Text::StrEquals(sarr2[0], (const UTF8Char*)"LOGIN"))
					{
						this->authLogin = true;
					}
					else if (Text::StrEquals(sarr2[0], (const UTF8Char*)"PLAIN"))
					{
						this->authPlain = true;
					}
					if (j != 2)
					{
						break;
					}
				}
			}
			if (i != 2)
			{
				break;
			}
		}
	}
	return code == 250;
}

Bool Net::Email::SMTPConn::SendAuth(const UTF8Char *userName, const UTF8Char *password)
{
	if (this->authPlain)
	{
		UTF8Char pwdBuff[128];
		UTF8Char *sptr2 = pwdBuff;
		*sptr2++ = 0;
		sptr2 = Text::StrConcat(sptr2, userName) + 1;
		sptr2 = Text::StrConcat(sptr2, password);
		Text::TextBinEnc::Base64Enc b64;
		Text::StringBuilderUTF8 sbCmd;
		sbCmd.AppendC(UTF8STRC("AUTH PLAIN "));
		b64.EncodeBin(&sbCmd, pwdBuff, (UOSInt)(sptr2 - pwdBuff));
		
		this->statusChg = false;
		if (this->logWriter)
		{
			this->logWriter->WriteLineC(sbCmd.ToString(), sbCmd.GetLength());
		}
		writer->WriteLineC(sbCmd.ToString(), sbCmd.GetLength());
		UInt32 code = WaitForResult();
		return code == 235;
	}
	else if (this->authLogin)
	{
		UTF8Char retBuff[256];
		Text::TextBinEnc::Base64Enc b64;
		Text::StringBuilderUTF8 sbCmd;
		UInt32 code;
		retBuff[0] = 0;
		this->statusChg = false;
		this->msgRet = retBuff;
		if (this->logWriter)
		{
			this->logWriter->WriteLineC(UTF8STRC("AUTH LOGIN"));
		}
		writer->WriteLineC(UTF8STRC("AUTH LOGIN"));
		code = WaitForResult();
		if (code != 334 || !Text::StrEquals(retBuff, (const UTF8Char*)"VXNlcm5hbWU6"))
		{
			return false;
		}
		sbCmd.ClearStr();
		b64.EncodeBin(&sbCmd, userName, Text::StrCharCnt(userName));
		this->statusChg = false;
		this->msgRet = retBuff;
		if (this->logWriter)
		{
			this->logWriter->WriteLineC(sbCmd.ToString(), sbCmd.GetLength());
		}
		writer->WriteLineC(sbCmd.ToString(), sbCmd.GetLength());
		code = WaitForResult();
		if (code != 334 || !Text::StrEquals(retBuff, (const UTF8Char*)"UGFzc3dvcmQ6"))
		{
			return false;
		}
		sbCmd.ClearStr();
		b64.EncodeBin(&sbCmd, password, Text::StrCharCnt(password));
		this->statusChg = false;
		this->msgRet = retBuff;
		if (this->logWriter)
		{
			this->logWriter->WriteLineC(sbCmd.ToString(), sbCmd.GetLength());
		}
		writer->WriteLineC(sbCmd.ToString(), sbCmd.GetLength());
		code = WaitForResult();
		return code == 235;
	}
	else
	{
		return false;
	}
}

Bool Net::Email::SMTPConn::SendMailFrom(const UTF8Char *fromEmail)
{
	UTF8Char sbuff[512];
	UTF8Char *sptr;
	sptr = Text::StrConcatC(Text::StrConcat(Text::StrConcatC(sbuff, UTF8STRC("MAIL FROM: <")), fromEmail), UTF8STRC(">"));
	this->statusChg = false;
	if (this->logWriter)
	{
		this->logWriter->WriteLineC(sbuff, (UOSInt)(sptr - sbuff));
	}
	writer->WriteLineC(sbuff, (UOSInt)(sptr - sbuff));
	UInt32 code = WaitForResult();
	return code == 250;
}

Bool Net::Email::SMTPConn::SendRcptTo(const UTF8Char *toEmail)
{
	UTF8Char sbuff[512];
	UTF8Char *sptr;
	sptr = Text::StrConcatC(Text::StrConcat(Text::StrConcatC(sbuff, UTF8STRC("RCPT TO: <")), toEmail), UTF8STRC(">"));
	this->statusChg = false;
	if (this->logWriter)
	{
		this->logWriter->WriteLineC(sbuff, (UOSInt)(sptr - sbuff));
	}
	writer->WriteLineC(sbuff, (UOSInt)(sptr - sbuff));
	UInt32 code = WaitForResult();
	return code == 250;
}

Bool Net::Email::SMTPConn::SendQuit()
{
	this->statusChg = false;
	if (this->logWriter)
	{
		this->logWriter->WriteLineC(UTF8STRC("QUIT"));
	}
	writer->WriteLineC(UTF8STRC("QUIT"));
	UInt32 code = WaitForResult();
	return code == 221;
}

Bool Net::Email::SMTPConn::SendData(const UTF8Char *buff, UOSInt buffSize)
{
	this->statusChg = false;
	if (this->logWriter)
	{
		this->logWriter->WriteLineC(UTF8STRC("DATA"));
	}
	writer->WriteLineC(UTF8STRC("DATA"));
	UInt32 code = WaitForResult();
	if (code != 354)
	{
		return false;
	}
	this->statusChg = false;
	this->cli->Write(buff, buffSize);
	this->cli->Write((const UInt8*)"\r\n.\r\n", 5);
	code = WaitForResult();
	return code == 250;
	
}
