#include "Stdafx.h"
#include "MyMemory.h"
#include "Manage/HiResClock.h"
#include "Net/Email/SMTPConn.h"
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

UInt32 __stdcall Net::Email::SMTPConn::SMTPThread(AnyType userObj)
{
	NN<Net::Email::SMTPConn> me = userObj.GetNN<Net::Email::SMTPConn>();
	UTF8Char sbuff[2048];
	UTF8Char sbuff2[4];
	UnsafeArray<UTF8Char> sptr;
	UnsafeArray<UTF8Char> msgRet;
	UInt32 msgCode;
	NN<IO::Writer> lwriter;

	me->threadStarted = true;
	me->threadRunning = true;
	{
		Text::UTF8Reader reader(me->cli);
		while (!me->threadToStop)
		{
			if (!reader.ReadLine(sbuff, 2048).SetTo(sptr))
			{
				if (me->logWriter.SetTo(lwriter)) lwriter->WriteLine(CSTR("Connection Closed"));
				break;
			}

			if (me->logWriter.SetTo(lwriter)) lwriter->WriteLine(CSTRP(sbuff, sptr));
	#ifdef VERBOSE
			printf("SMTP Read: %s\r\n", sbuff);
	#endif
			if (sbuff[0] == ' ')
			{
				if (me->msgRet.SetTo(msgRet))
				{
					msgRet = Text::StrConcatC(msgRet, sbuff, (UOSInt)(sptr - sbuff));
					me->msgRet = reader.GetLastLineBreak(msgRet);
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
					if (me->msgRet.SetTo(msgRet))
					{
						msgRet = Text::StrConcatC(msgRet, &sbuff[4], (UOSInt)(sptr - &sbuff[4]));
						msgRet[0] = 0;
						me->msgRet = msgRet;
					}
					me->lastStatus = msgCode;
					me->statusChg = true;
					me->evt.Set();
				}
				else if (sbuff[3] == '-')
				{
					if (me->msgRet.SetTo(msgRet))
					{
						msgRet = Text::StrConcatC(msgRet, &sbuff[4], (UOSInt)(sptr - &sbuff[4]));
						me->msgRet = reader.GetLastLineBreak(msgRet);
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

UInt32 Net::Email::SMTPConn::WaitForResult(OptOut<UnsafeArrayOpt<UTF8Char>> msgRetEnd)
{
	Manage::HiResClock clk;
	while (this->threadRunning && !this->statusChg && clk.GetTimeDiff() < 30.0)
	{
		this->evt.Wait(1000);
	}
	msgRetEnd.Set(this->msgRet);
	this->msgRet = nullptr;
	if (this->statusChg)
	{
		return this->lastStatus;
	}
	else
		return 0;
}

Net::Email::SMTPConn::SMTPConn(NN<Net::TCPClientFactory> clif, Optional<Net::SSLEngine> ssl, Text::CStringNN host, UInt16 port, ConnType connType, Optional<IO::Writer> logWriter, Data::Duration timeout)
{
	this->threadStarted = false;
	this->threadRunning = false;
	this->threadToStop = false;
	this->msgRet = nullptr;
	this->statusChg = false;
	this->maxSize = 0;
	this->authLogin = false;
	this->authPlain = false;
	this->logWriter = logWriter;
	Net::SocketUtil::AddressInfo addr;
	addr.addrType = Net::AddrType::Unknown;
	clif->GetSocketFactory()->DNSResolveIP(host, addr);
	NN<IO::Writer> lwriter;
	if (connType == ConnType::SSL)
	{
		NN<Net::SSLEngine> nnssl;
		if (!ssl.SetTo(nnssl) || !Optional<Net::TCPClient>(nnssl->ClientConnect(host, port, 0, timeout)).SetTo(this->cli))
		{
			this->cli = clif->Create(host, port, timeout);
		}
	}
	else if (connType == ConnType::STARTTLS)
	{
		UInt8 buff[1024];
		UOSInt buffSize;
		this->cli = clif->Create(host, port, timeout);
		this->cli->SetTimeout(2000);
		buffSize = this->cli->Read(BYTEARR(buff));
		if (this->logWriter.SetTo(lwriter)) lwriter->Write(Text::CStringNN(buff, buffSize));
		if (buffSize > 2 && Text::StrStartsWithC(buff, buffSize, UTF8STRC("220 ")) && buff[buffSize - 2] == '\r' && buff[buffSize - 1] == '\n')
		{
			if (this->logWriter.SetTo(lwriter)) lwriter->WriteLine(CSTR("STARTTLS"));
			this->cli->Write(Data::ByteArrayR((const UInt8*)"STARTTLS\r\n", 10));
			buffSize = this->cli->Read(BYTEARR(buff));
			if (this->logWriter.SetTo(lwriter)) lwriter->Write(Text::CStringNN(buff, buffSize));
			if (buffSize > 0 && Text::StrStartsWithC(buff, buffSize, UTF8STRC("220 ")) && buff[buffSize - 2] == '\r' && buff[buffSize - 1] == '\n')
			{
				if (this->logWriter.SetTo(lwriter)) lwriter->WriteLine(CSTR("SSL Handshake begin"));
				NN<Socket> s;
				NN<Net::SSLClient> cli;
				NN<Net::SSLEngine> nnssl;
				if (this->cli->RemoveSocket().SetTo(s) && ssl.SetTo(nnssl) && nnssl->ClientInit(s, host, 0).SetTo(cli))
				{
					if (this->logWriter.SetTo(lwriter)) lwriter->WriteLine(CSTR("SSL Handshake success"));
					this->cli.Delete();
					this->cli = cli;
				}
				else
				{
					if (this->logWriter.SetTo(lwriter)) lwriter->WriteLine(CSTR("SSL Handshake failed"));
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
		this->cli = clif->Create(host, port, timeout);
	}
	this->cli->SetNoDelay(false);
	this->cli->SetTimeout(timeout);
	NEW_CLASSNN(this->writer, Text::UTF8Writer(this->cli));
	if (this->logWriter.SetTo(lwriter))
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
		lwriter->WriteLine(sb.ToCString());
	}
	Sync::ThreadUtil::Create(SMTPThread, this);
	while (!this->threadStarted)
	{
		Sync::SimpleThread::Sleep(10);
	}
	if (connType == ConnType::STARTTLS)
	{
		this->initCode = 220;
		if (this->logWriter.SetTo(lwriter)) lwriter->WriteLine(CSTR("Connected"));
	}
	else
	{
		this->initCode = WaitForResult(0);
	}
}

Net::Email::SMTPConn::~SMTPConn()
{
	this->threadToStop = true;
	this->cli->Close();
	while (this->threadRunning)
	{
		Sync::SimpleThread::Sleep(10);
	}
	this->writer.Delete();
	this->cli.Delete();
}

Bool Net::Email::SMTPConn::IsError()
{
	return this->initCode != 220 || this->cli->IsConnectError();
}

UOSInt Net::Email::SMTPConn::GetMaxSize()
{
	return this->maxSize;
}

Bool Net::Email::SMTPConn::SendHelo(Text::CStringNN cliName)
{
	NN<IO::Writer> lwriter;
	UTF8Char sbuff[512];
	UnsafeArray<UTF8Char> sptr;
	sptr = cliName.ConcatTo(Text::StrConcatC(sbuff, UTF8STRC("HELO ")));
	this->statusChg = false;
	if (this->logWriter.SetTo(lwriter)) lwriter->WriteLine(CSTRP(sbuff, sptr));
	writer->WriteLine(CSTRP(sbuff, sptr));
	UInt32 code = WaitForResult(0);
	return code == 250;
}

Bool Net::Email::SMTPConn::SendEHlo(Text::CStringNN cliName)
{
	NN<IO::Writer> lwriter;
	UTF8Char returnMsg[2048];
	UnsafeArrayOpt<UTF8Char> returnMsgEnd;
	UTF8Char sbuff[512];
	UnsafeArray<UTF8Char> sptr;
	sptr = Text::StrConcatC(cliName.ConcatTo(Text::StrConcatC(sbuff, UTF8STRC("EHLO "))), UTF8STRC("\r\n"));
	this->statusChg = false;
	returnMsg[0] = 0;
	this->msgRet = returnMsg;
	if (this->logWriter.SetTo(lwriter)) lwriter->Write(CSTRP(sbuff, sptr));
	writer->Write(CSTRP(sbuff, sptr));
	UInt32 code = WaitForResult(returnMsgEnd);
	if (code == 0)
	{
#ifdef VERBOSE
		printf("Retry sending EHlo\r\n");
#endif
		this->statusChg = false;
		this->msgRet = returnMsg;
		writer->Write(CSTRP(sbuff, sptr));
		code = WaitForResult(returnMsgEnd);
	}
#ifdef VERBOSE
	printf("EHlo reply = %d\r\n", code);
#endif
	if (code == 250)
	{
		Text::PString sarr[2];
		Text::PString sarr2[2];
		sarr[1].v = returnMsg;
		sarr[1].leng = (UOSInt)(returnMsgEnd.Ptr() - returnMsg);
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

Bool Net::Email::SMTPConn::SendAuth(Text::CStringNN userName, Text::CStringNN password)
{
	NN<IO::Writer> lwriter;
	if (this->authPlain)
	{
		UTF8Char pwdBuff[128];
		UnsafeArray<UTF8Char> sptr2 = pwdBuff;
		*sptr2++ = 0;
		sptr2 = userName.ConcatTo(sptr2) + 1;
		sptr2 = password.ConcatTo(sptr2);
		Text::TextBinEnc::Base64Enc b64;
		Text::StringBuilderUTF8 sbCmd;
		sbCmd.AppendC(UTF8STRC("AUTH PLAIN "));
		b64.EncodeBin(sbCmd, pwdBuff, (UOSInt)(sptr2 - pwdBuff));
		
		this->statusChg = false;
		if (this->logWriter.SetTo(lwriter)) lwriter->WriteLine(CSTR("[Authentication message hidden]"));
		writer->WriteLine(sbCmd.ToCString());
		UInt32 code = WaitForResult(0);
		return code == 235;
	}
	else if (this->authLogin)
	{
		UTF8Char retBuff[256];
		UnsafeArrayOpt<UTF8Char> retBuffEnd;
		Text::TextBinEnc::Base64Enc b64;
		Text::StringBuilderUTF8 sbCmd;
		UInt32 code;
		retBuff[0] = 0;
		this->statusChg = false;
		this->msgRet = retBuff;
		if (this->logWriter.SetTo(lwriter)) lwriter->WriteLine(CSTR("AUTH LOGIN"));
		writer->WriteLine(CSTR("AUTH LOGIN"));
		code = WaitForResult(retBuffEnd);
		if (code != 334 || !Text::StrEqualsC(retBuff, (UOSInt)(retBuffEnd.Ptr() - retBuff), UTF8STRC("VXNlcm5hbWU6")))
		{
#ifdef VERBOSE
			printf("Error in login1: code = %d, msgLen = %d, msg = %s\r\n", code, (UInt32)(retBuffEnd - retBuff), retBuff);
#endif
			return false;
		}
		sbCmd.ClearStr();
		b64.EncodeBin(sbCmd, userName.v.Ptr(), userName.leng);
		this->statusChg = false;
		this->msgRet = retBuff;
		if (this->logWriter.SetTo(lwriter)) lwriter->WriteLine(sbCmd.ToCString());
		writer->WriteLine(sbCmd.ToCString());
		code = WaitForResult(retBuffEnd);
		if (code != 334 || !Text::StrEqualsC(retBuff, (UOSInt)(retBuffEnd.Ptr() - retBuff), UTF8STRC("UGFzc3dvcmQ6")))
		{
#ifdef VERBOSE
			printf("Error in login2: code = %d, msgLen = %d, msg = %s\r\n", code, (UInt32)(retBuffEnd - retBuff), retBuff);
#endif
			return false;
		}
		sbCmd.ClearStr();
		b64.EncodeBin(sbCmd, password.v.Ptr(), password.leng);
		this->statusChg = false;
		this->msgRet = retBuff;
		if (this->logWriter.SetTo(lwriter)) lwriter->WriteLine(CSTR("[Password hidden]"));
		writer->WriteLine(sbCmd.ToCString());
		code = WaitForResult(0);
		return code == 235;
	}
	else
	{
		return false;
	}
}

Bool Net::Email::SMTPConn::SendMailFrom(Text::CStringNN fromEmail)
{
	NN<IO::Writer> lwriter;
	UTF8Char sbuff[512];
	UnsafeArray<UTF8Char> sptr;
	sptr = Text::StrConcatC(fromEmail.ConcatTo(Text::StrConcatC(sbuff, UTF8STRC("MAIL FROM: <"))), UTF8STRC(">"));
	this->statusChg = false;
	if (this->logWriter.SetTo(lwriter)) lwriter->WriteLine(CSTRP(sbuff, sptr));
	writer->WriteLine(CSTRP(sbuff, sptr));
	UInt32 code = WaitForResult(0);
	return code == 250;
}

Bool Net::Email::SMTPConn::SendRcptTo(Text::CStringNN toEmail)
{
	NN<IO::Writer> lwriter;
	UTF8Char sbuff[512];
	UnsafeArray<UTF8Char> sptr;
	sptr = Text::StrConcatC(toEmail.ConcatTo(Text::StrConcatC(sbuff, UTF8STRC("RCPT TO: <"))), UTF8STRC(">"));
	this->statusChg = false;
	if (this->logWriter.SetTo(lwriter)) lwriter->WriteLine(CSTRP(sbuff, sptr));
	writer->WriteLine(CSTRP(sbuff, sptr));
	UInt32 code = WaitForResult(0);
	return code == 250;
}

Bool Net::Email::SMTPConn::SendQuit()
{
	NN<IO::Writer> lwriter;
	this->statusChg = false;
	if (this->logWriter.SetTo(lwriter)) lwriter->WriteLine(CSTR("QUIT"));
	writer->WriteLine(CSTR("QUIT"));
	UInt32 code = WaitForResult(0);
	return code == 221;
}

Bool Net::Email::SMTPConn::SendData(UnsafeArray<const UTF8Char> buff, UOSInt buffSize)
{
	NN<IO::Writer> lwriter;
	this->statusChg = false;
	if (this->logWriter.SetTo(lwriter)) lwriter->WriteLine(CSTR("DATA"));
	writer->WriteLine(CSTR("DATA"));
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
		writeSize = this->cli->Write(Data::ByteArrayR(buff, buffSize));
		if (writeSize == 0)
		{
			if (this->logWriter.SetTo(lwriter)) lwriter->WriteLine(CSTR("Error in writing to SMTP Server"));
			break;
		}
		buff += writeSize;
		buffSize -= writeSize;
		totalSize += writeSize;
	}
	if (buffSize == 0)
	{
		UTF8Char sbuff[64];
		UnsafeArray<UTF8Char> sptr;
		sptr = Text::StrConcatC(sbuff, UTF8STRC("Write "));
		sptr = Text::StrUOSInt(sptr, totalSize);
		sptr = Text::StrConcatC(sptr, UTF8STRC(" bytes"));
		if (this->logWriter.SetTo(lwriter)) lwriter->WriteLine(CSTRP(sbuff, sptr));
	}
	this->cli->Write(Data::ByteArrayR((const UInt8*)"\r\n.\r\n", 5));
	code = WaitForResult(0);
	return code == 250;
}
