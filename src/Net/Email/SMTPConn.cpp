#include "Stdafx.h"
#include "MyMemory.h"
#include "Manage/HiResClock.h"
#include "Net/Email/SMTPConn.h"
#include "Sync/Thread.h"
#include "Text/MyString.h"
#include "Text/StringBuilderUTF8.h"
#include "Text/UTF8Reader.h"
#include "Text/TextBinEnc/Base64Enc.h"

//#define VERBOSE
#ifdef VERBOSE
#include <stdio.h>
#endif

UInt32 __stdcall Net::Email::SMTPConn::SMTPThread(void *userObj)
{
	Net::Email::SMTPConn *me = (Net::Email::SMTPConn *)userObj;
	UTF8Char sbuff[2048];
	UTF8Char sbuff2[4];
	UTF8Char *sptr;
	UInt32 msgCode;

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
			printf("SMTP Read: %s\r\n", sbuff);
	#endif
			if (sbuff[0] == ' ')
			{
				if (me->msgRet)
				{
					me->msgRet = Text::StrConcatC(me->msgRet, sbuff, (UOSInt)(sptr - sbuff));
					me->msgRet = reader.GetLastLineBreak(me->msgRet);
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
					//me->logged = true;
				}
				if (sbuff[3] == ' ')
				{
					if (me->msgRet)
					{
						me->msgRet = Text::StrConcatC(me->msgRet, &sbuff[4], (UOSInt)(sptr - &sbuff[4]));
						me->msgRet[0] = 0;
					}
					me->lastStatus = msgCode;
					me->statusChg = true;
					me->evt.Set();
				}
				else if (sbuff[3] == '-')
				{
					if (me->msgRet)
					{
						me->msgRet = Text::StrConcatC(me->msgRet, &sbuff[4], (UOSInt)(sptr - &sbuff[4]));
						me->msgRet = reader.GetLastLineBreak(me->msgRet);
					}
				}
			}
		}
		me->lastStatus = 0;
		me->statusChg = true;
		me->evt.Set();
	}
	me->threadRunning = false;
	return 0;
}

UInt32 Net::Email::SMTPConn::WaitForResult(UTF8Char **msgRetEnd)
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
		return 0;
}

Net::Email::SMTPConn::SMTPConn(Net::SocketFactory *sockf, Net::SSLEngine *ssl, Text::CString host, UInt16 port, ConnType connType, IO::Writer *logWriter)
{
	this->threadStarted = false;
	this->threadRunning = false;
	this->threadToStop = false;
	this->msgRet = 0;
	this->statusChg = false;
	this->maxSize = 0;
	this->authLogin = false;
	this->authPlain = false;
	Net::SocketUtil::AddressInfo addr;
	addr.addrType = Net::AddrType::Unknown;
	sockf->DNSResolveIP(host, &addr);
	this->logWriter = logWriter;
	if (connType == ConnType::SSL)
	{
		Net::SSLEngine::ErrorType err;
		this->cli = ssl->ClientConnect(host, port, &err);
	}
	else if (connType == ConnType::STARTTLS)
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
		if (buffSize > 2 && Text::StrStartsWithC(buff, buffSize, UTF8STRC("220 ")) && buff[buffSize - 2] == '\r' && buff[buffSize - 1] == '\n')
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
			if (buffSize > 0 && Text::StrStartsWithC(buff, buffSize, UTF8STRC("220 ")) && buff[buffSize - 2] == '\r' && buff[buffSize - 1] == '\n')
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
	Sync::Thread::Create(SMTPThread, this);
	while (!this->threadStarted)
	{
		Sync::Thread::Sleep(10);
	}
	if (connType == ConnType::STARTTLS)
	{
		this->initCode = 220;
		this->logWriter->WriteLineC(UTF8STRC("Connected"));
	}
	else
	{
		this->initCode = WaitForResult(0);
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
}

Bool Net::Email::SMTPConn::IsError()
{
	return this->initCode != 220 || this->cli->IsConnectError();
}

UOSInt Net::Email::SMTPConn::GetMaxSize()
{
	return this->maxSize;
}

Bool Net::Email::SMTPConn::SendHelo(Text::CString cliName)
{
	UTF8Char sbuff[512];
	UTF8Char *sptr;
	sptr = cliName.ConcatTo(Text::StrConcatC(sbuff, UTF8STRC("HELO ")));
	this->statusChg = false;
	if (this->logWriter)
	{
		this->logWriter->WriteLineC(sbuff, (UOSInt)(sptr - sbuff));
	}
	writer->WriteLineC(sbuff, (UOSInt)(sptr - sbuff));
	UInt32 code = WaitForResult(0);
	return code == 250;
}

Bool Net::Email::SMTPConn::SendEHlo(Text::CString cliName)
{
	UTF8Char returnMsg[2048];
	UTF8Char *returnMsgEnd;
	UTF8Char sbuff[512];
	UTF8Char *sptr;
	sptr = Text::StrConcatC(cliName.ConcatTo(Text::StrConcatC(sbuff, UTF8STRC("EHLO "))), UTF8STRC("\r\n"));
	this->statusChg = false;
	returnMsg[0] = 0;
	this->msgRet = returnMsg;
	if (this->logWriter)
	{
		this->logWriter->WriteStrC(sbuff, (UOSInt)(sptr - sbuff));
	}
	writer->WriteStrC(sbuff, (UOSInt)(sptr - sbuff));
	UInt32 code = WaitForResult(&returnMsgEnd);
	if (code == 0)
	{
#ifdef VERBOSE
		printf("Retry sending EHlo\r\n");
#endif
		this->statusChg = false;
		this->msgRet = returnMsg;
		writer->WriteStrC(sbuff, (UOSInt)(sptr - sbuff));
		code = WaitForResult(&returnMsgEnd);
	}
#ifdef VERBOSE
	printf("EHlo reply = %d\r\n", code);
#endif
	if (code == 250)
	{
		Text::PString sarr[2];
		Text::PString sarr2[2];
		sarr[1].v = returnMsg;
		sarr[1].leng = (UOSInt)(returnMsgEnd - returnMsg);
		UOSInt i;
		UOSInt j;
		while (true)
		{
			i = Text::StrSplitLineP(sarr, 2, sarr[1]);
			if (sarr[0].StartsWith(UTF8STRC("SIZE ")))
			{
				this->maxSize = Text::StrToUOSInt(sarr[0].v + 5);
			}
			else if (sarr[0].StartsWith(UTF8STRC("AUTH ")))
			{
				sarr2[1] = sarr[0].Substring(5);
				while (true)
				{
					j = Text::StrSplitP(sarr2, 2, sarr2[1], ' ');
					if (sarr2[0].Equals(UTF8STRC("LOGIN")))
					{
						this->authLogin = true;
					}
					else if (sarr2[0].Equals(UTF8STRC("PLAIN")))
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

Bool Net::Email::SMTPConn::SendAuth(Text::CString userName, Text::CString password)
{
	if (this->authPlain)
	{
		UTF8Char pwdBuff[128];
		UTF8Char *sptr2 = pwdBuff;
		*sptr2++ = 0;
		sptr2 = userName.ConcatTo(sptr2) + 1;
		sptr2 = password.ConcatTo(sptr2);
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
		UInt32 code = WaitForResult(0);
		return code == 235;
	}
	else if (this->authLogin)
	{
		UTF8Char retBuff[256];
		UTF8Char *retBuffEnd;
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
		code = WaitForResult(&retBuffEnd);
		if (code != 334 || !Text::StrEqualsC(retBuff, (UOSInt)(retBuffEnd - retBuff), UTF8STRC("VXNlcm5hbWU6")))
		{
#ifdef VERBOSE
			printf("Error in login1: code = %d, msgLen = %d, msg = %s\r\n", code, (UInt32)(retBuffEnd - retBuff), retBuff);
#endif
			return false;
		}
		sbCmd.ClearStr();
		b64.EncodeBin(&sbCmd, userName.v, userName.leng);
		this->statusChg = false;
		this->msgRet = retBuff;
		if (this->logWriter)
		{
			this->logWriter->WriteLineC(sbCmd.ToString(), sbCmd.GetLength());
		}
		writer->WriteLineC(sbCmd.ToString(), sbCmd.GetLength());
		code = WaitForResult(&retBuffEnd);
		if (code != 334 || !Text::StrEqualsC(retBuff, (UOSInt)(retBuffEnd - retBuff), UTF8STRC("UGFzc3dvcmQ6")))
		{
#ifdef VERBOSE
			printf("Error in login2: code = %d, msgLen = %d, msg = %s\r\n", code, (UInt32)(retBuffEnd - retBuff), retBuff);
#endif
			return false;
		}
		sbCmd.ClearStr();
		b64.EncodeBin(&sbCmd, password.v, password.leng);
		this->statusChg = false;
		this->msgRet = retBuff;
		if (this->logWriter)
		{
			this->logWriter->WriteLineC(sbCmd.ToString(), sbCmd.GetLength());
		}
		writer->WriteLineC(sbCmd.ToString(), sbCmd.GetLength());
		code = WaitForResult(0);
		return code == 235;
	}
	else
	{
		return false;
	}
}

Bool Net::Email::SMTPConn::SendMailFrom(Text::CString fromEmail)
{
	UTF8Char sbuff[512];
	UTF8Char *sptr;
	sptr = Text::StrConcatC(fromEmail.ConcatTo(Text::StrConcatC(sbuff, UTF8STRC("MAIL FROM: <"))), UTF8STRC(">"));
	this->statusChg = false;
	if (this->logWriter)
	{
		this->logWriter->WriteLineC(sbuff, (UOSInt)(sptr - sbuff));
	}
	writer->WriteLineC(sbuff, (UOSInt)(sptr - sbuff));
	UInt32 code = WaitForResult(0);
	return code == 250;
}

Bool Net::Email::SMTPConn::SendRcptTo(Text::CString toEmail)
{
	UTF8Char sbuff[512];
	UTF8Char *sptr;
	sptr = Text::StrConcatC(toEmail.ConcatTo(Text::StrConcatC(sbuff, UTF8STRC("RCPT TO: <"))), UTF8STRC(">"));
	this->statusChg = false;
	if (this->logWriter)
	{
		this->logWriter->WriteLineC(sbuff, (UOSInt)(sptr - sbuff));
	}
	writer->WriteLineC(sbuff, (UOSInt)(sptr - sbuff));
	UInt32 code = WaitForResult(0);
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
	UInt32 code = WaitForResult(0);
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
	UInt32 code = WaitForResult(0);
	if (code != 354)
	{
		return false;
	}
	this->statusChg = false;
	UOSInt totalSize = 0;
	UOSInt writeSize;
	while (buffSize > 0)
	{
		writeSize = this->cli->Write(buff, buffSize);
		if (writeSize == 0)
		{
			this->logWriter->WriteLineC(UTF8STRC("Error in writing to SMTP Server"));
			break;
		}
		buff += writeSize;
		buffSize -= writeSize;
		totalSize += writeSize;
	}
	if (buffSize == 0)
	{
		UTF8Char sbuff[64];
		UTF8Char *sptr;
		sptr = Text::StrConcatC(sbuff, UTF8STRC("Write "));
		sptr = Text::StrUOSInt(sptr, totalSize);
		sptr = Text::StrConcatC(sptr, UTF8STRC(" bytes"));
		this->logWriter->WriteLineC(sbuff, (UOSInt)(sptr - sbuff));
	}
	this->cli->Write((const UInt8*)"\r\n.\r\n", 5);
	code = WaitForResult(0);
	return code == 250;
	
}
