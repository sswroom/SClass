#include "Stdafx.h"
#include "MyMemory.h"
#include "Text/MyString.h"
#include "IO/StreamReader.h"
#include "Manage/HiResClock.h"
#include "Sync/Thread.h"
#include "Net/FTPConn.h"

UInt32 __stdcall Net::FTPConn::FTPThread(void *userObj)
{
	Net::FTPConn *me = (Net::FTPConn *)userObj;
	IO::StreamReader *reader;
	UTF8Char sbuff[2048];
	UTF8Char sbuff2[4];
	UTF8Char *sptr;
	Int32 msgCode;

	me->threadRunning = true;
	NEW_CLASS(reader, IO::StreamReader(me->cli, me->codePage));
	while (!me->threadToStop)
	{
		sptr = reader->ReadLine(sbuff, 2048);
		if (sptr == 0)
			break;

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
			if (msgCode == 230)
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
	me->statusChg = true;
	me->evt->Set();
	DEL_CLASS(reader);
	me->threadRunning = false;
	return 0;
}

Int32 Net::FTPConn::WaitForResult()
{
	Manage::HiResClock clk;
	while (!this->statusChg && clk.GetTimeDiff() < 30.0)
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

Net::FTPConn::FTPConn(const UTF8Char *host, UInt16 port, Net::SocketFactory *sockf, UInt32 codePage)
{
	this->codePage = codePage;
	this->threadRunning = false;
	this->threadToStop = false;
	this->logged = true;
	this->msgRet = 0;
	this->statusChg = false;
	NEW_CLASS(this->evt, Sync::Event(true, (const UTF8Char*)"Net.FTPConn.evt"));
	NEW_CLASS(this->cli, Net::TCPClient(sockf, host, port));
	NEW_CLASS(this->writer, IO::StreamWriter(this->cli, codePage));
	Sync::Thread::Create(FTPThread, this);
	WaitForResult();
}

Net::FTPConn::~FTPConn()
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

Bool Net::FTPConn::IsLogged()
{
	return this->logged;
}

Bool Net::FTPConn::SendUser(const UTF8Char *userName)
{
	UTF8Char sbuff[512];
	Text::StrConcat(Text::StrConcat(sbuff, (const UTF8Char*)"USER "), userName);
	this->statusChg = false;
	writer->WriteLine(sbuff);
	Int32 code = WaitForResult();
	return code == 331;
}

Bool Net::FTPConn::SendPassword(const UTF8Char *password)
{
	UTF8Char sbuff[512];
	Text::StrConcat(Text::StrConcat(sbuff, (const UTF8Char*)"PASS "), password);
	this->statusChg = false;
	writer->WriteLine(sbuff);
	Int32 code = WaitForResult();
	return code == 230;
}

Bool Net::FTPConn::ChangeDirectory(const UTF8Char *dir)
{
	UTF8Char sbuff[512];
	Text::StrConcat(Text::StrConcat(sbuff, (const UTF8Char*)"CWD "), dir);
	this->statusChg = false;
	writer->WriteLine(sbuff);
	Int32 code = WaitForResult();
	return code == 250;
}

Bool Net::FTPConn::MakeDirectory(const UTF8Char *dir)
{
	UTF8Char sbuff[512];
	Text::StrConcat(Text::StrConcat(sbuff, (const UTF8Char*)"MKD "), dir);
	this->statusChg = false;
	writer->WriteLine(sbuff);
	Int32 code = WaitForResult();
	return code == 257;
}

Bool Net::FTPConn::RemoveDirectory(const UTF8Char *dir)
{
	UTF8Char sbuff[512];
	Text::StrConcat(Text::StrConcat(sbuff, (const UTF8Char*)"RMD "), dir);
	this->statusChg = false;
	writer->WriteLine(sbuff);
	Int32 code = WaitForResult();
	return code == 250;
}

Bool Net::FTPConn::GetFileSize(const UTF8Char *fileName, UInt64 *fileSize)
{
	UTF8Char sbuff[512];
	Text::StrConcat(Text::StrConcat(sbuff, (const UTF8Char*)"SIZE "), fileName);
	this->msgRet = sbuff;
	this->statusChg = false;
	writer->WriteLine(sbuff);
	Int32 code = WaitForResult();
	if (code == 213)
	{
		if (fileSize)
		{
			*fileSize = Text::StrToUInt64(sbuff);
		}
		return true;
	}
	else
	{
		return false;
	}
}

Bool Net::FTPConn::GetFileModTime(const UTF8Char *fileName, Data::DateTime *modTime)
{
	UTF8Char sbuff[512];
	Text::StrConcat(Text::StrConcat(sbuff, (const UTF8Char*)"MDTM "), fileName);
	this->msgRet = sbuff;
	this->statusChg = false;
	writer->WriteLine(sbuff);
	Int32 code = WaitForResult();
	if (code == 213)
	{
		UInt16 year;
		Int32 month;
		Int32 day;
		Int32 hour;
		Int32 minute;
		Int32 second;
		second = Text::StrToInt32(&sbuff[12]);
		sbuff[12] = 0;
		minute = Text::StrToInt32(&sbuff[10]);
		sbuff[10] = 0;
		hour = Text::StrToInt32(&sbuff[8]);
		sbuff[8] = 0;
		day = Text::StrToInt32(&sbuff[6]);
		sbuff[6] = 0;
		month = Text::StrToInt32(&sbuff[4]);
		sbuff[4] = 0;
		Text::StrToUInt16S(&sbuff[0], &year, 0);
		modTime->ToUTCTime();
		modTime->SetValue(year, month, day, hour, minute, second, 0);
		return true;
	}
	else
	{
		return false;
	}
}

Bool Net::FTPConn::ToBinaryType()
{
	this->statusChg = false;
	writer->WriteLine((const UTF8Char *)"TYPE I");
	Int32 code = WaitForResult();
	return code == 200;
}

Bool Net::FTPConn::ToASCIIType()
{
	this->statusChg = false;
	writer->WriteLine((const UTF8Char *)"TYPE A");
	Int32 code = WaitForResult();
	return code == 200;
}

Bool Net::FTPConn::ToEBCDICType()
{
	this->statusChg = false;
	writer->WriteLine((const UTF8Char *)"TYPE E");
	Int32 code = WaitForResult();
	return code == 200;
}


Bool Net::FTPConn::ChangePassiveMode(UInt32 *ip, UInt16 *port)
{
	UTF8Char sbuff[512];
	UTF8Char *sptr;
	UTF8Char *sarr[7];
	UInt8 buff[6];
	UOSInt i;
	this->msgRet = sbuff;
	this->statusChg = false;
	writer->WriteLine((const UTF8Char *)"PASV");
	Int32 code = WaitForResult();
	if (code == 227)
	{
		if (!Text::StrStartsWith(sbuff, (const UTF8Char *)"Entering Passive Mode ("))
		{
			return false;
		}
		sptr = &sbuff[23];
		i = Text::StrIndexOf(sptr, (const UTF8Char *)")");
		if (i == INVALID_INDEX || i == 0)
			return false;
		sptr[i] = 0;
		if (Text::StrSplit(sarr, 7, sptr, ',') != 6)
			return false;
		i = 6;
		buff[0] = (UInt8)Text::StrToUInt32(sarr[0]);
		buff[1] = (UInt8)Text::StrToUInt32(sarr[1]);
		buff[2] = (UInt8)Text::StrToUInt32(sarr[2]);
		buff[3] = (UInt8)Text::StrToUInt32(sarr[3]);
		buff[4] = (UInt8)Text::StrToUInt32(sarr[5]);
		buff[5] = (UInt8)Text::StrToUInt32(sarr[4]);
		if (ip)
		{
			*ip = *(UInt32*)buff;
		}
		if (port)
		{
			*port = *(UInt16*)&buff[4];
		}
		return true;
	}
	else
	{
		return false;
	}
}

Bool Net::FTPConn::ChangeActiveMode(UInt32 ip, UInt16 port)
{
	UInt8 buff[6];
	UTF8Char sbuff[512];
	UTF8Char *sptr;
	*(UInt32*)&buff[0] = ip;
	*(UInt16*)&buff[4] = port;
	sptr = Text::StrConcat(sbuff, (const UTF8Char*)"PORT ");
	sptr = Text::StrUInt16(sptr, buff[0]);
	sptr = Text::StrConcat(sptr, (const UTF8Char*)",");
	sptr = Text::StrUInt16(sptr, buff[1]);
	sptr = Text::StrConcat(sptr, (const UTF8Char*)",");
	sptr = Text::StrUInt16(sptr, buff[2]);
	sptr = Text::StrConcat(sptr, (const UTF8Char*)",");
	sptr = Text::StrUInt16(sptr, buff[3]);
	sptr = Text::StrConcat(sptr, (const UTF8Char*)",");
	sptr = Text::StrUInt16(sptr, buff[5]);
	sptr = Text::StrConcat(sptr, (const UTF8Char*)",");
	sptr = Text::StrUInt16(sptr, buff[4]);
	this->statusChg = false;
	writer->WriteLine(sbuff);
	Int32 code = WaitForResult();
	return code == 200;
}

Bool Net::FTPConn::ResumeTransferPos(UInt64 pos)
{
	UTF8Char sbuff[64];
	Text::StrUInt64(Text::StrConcat(sbuff, (const UTF8Char*)"REST "), pos);
	this->statusChg = false;
	writer->WriteLine(sbuff);
	Int32 code = WaitForResult();
	return code == 200;
}

Bool Net::FTPConn::GetFile(const UTF8Char *fileName)
{
	UTF8Char sbuff[512];
	Text::StrConcat(Text::StrConcat(sbuff, (const UTF8Char*)"RETR "), fileName);
	this->statusChg = false;
	writer->WriteLine(sbuff);
	Int32 code = WaitForResult();
	return code == 150;
}

Bool Net::FTPConn::RenameFile(const UTF8Char *fromFile, const UTF8Char *toFile)
{
	UTF8Char sbuff[512];
	Text::StrConcat(Text::StrConcat(sbuff, (const UTF8Char *)"RNFR "), fromFile);
	this->statusChg = false;
	writer->WriteLine(sbuff);
	Int32 code = WaitForResult();
	if (code != 350)
		return false;
	Text::StrConcat(Text::StrConcat(sbuff, (const UTF8Char *)"RNTO "), toFile);
	this->statusChg = false;
	writer->WriteLine(sbuff);
	code = WaitForResult();
	return code == 250;
}
