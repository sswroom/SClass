#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ByteTool.h"
#include "Data/DateTime.h"
#include "Net/NTPClient.h"
#include "Net/NTPServer.h"
#include "Sync/Thread.h"
#include "Text/MyString.h"
#include "Text/StringBuilderUTF8.h"

void __stdcall Net::NTPServer::PacketHdlr(const Net::SocketUtil::AddressInfo *addr, UInt16 port, const UInt8 *buff, UOSInt dataSize, void *userData)
{
	Net::NTPServer *me = (Net::NTPServer*)userData;
	UInt8 repBuff[48];
	if (dataSize >= 48)
	{
		UInt8 vn = (buff[0] >> 3) & 7;
		UInt8 mode = buff[0] & 7;
		if (mode == 3)
		{
			Data::DateTime dt;
			Int64 currTime;
			dt.SetCurrTimeUTC();
			currTime = dt.ToTicks();
			repBuff[0] = (UInt8)((vn << 3) | 4);
			repBuff[1] = 2; //Secondary Server
			repBuff[2] = buff[2]; //Poll
			repBuff[3] = 3;
			WriteMInt32(&repBuff[4], 0); //Root Delay
			WriteMInt32(&repBuff[8], 0); //Root Dispersion
			WriteMInt32(&repBuff[12], (Int32)(currTime & 0xffffffff)); //Reference ID
			WriteTime(&repBuff[16], me->refTime); //Reference Timestamp
			WriteNInt64(&repBuff[24], ReadNInt64(&buff[40])); //Origin Timestamp
			WriteTime(&repBuff[32], currTime + me->timeDiff);
			WriteTime(&repBuff[40], currTime + me->timeDiff);
			me->svr->SendTo(addr, port, repBuff, 48);

			if (me->log)
			{
				Text::StringBuilderUTF8 sb;
				UTF8Char sbuff[64];
				UTF8Char *sptr;
				sb.AppendC(UTF8STRC("NTP: Client "));
				sptr = Net::SocketUtil::GetAddrName(sbuff, addr, port);
				sb.AppendC(sbuff, (UOSInt)(sptr - sbuff));
				dt.AddMS((OSInt)me->timeDiff);
				dt.ToLocalTime();
				sb.AppendC(UTF8STRC(" reply time as "));
				sptr = dt.ToString(sbuff, "yyyy-MM-dd HH:mm:ss.fff");
				sb.AppendC(sbuff, (UOSInt)(sptr - sbuff));
				me->log->LogMessageC(sb.ToString(), sb.GetLength(), IO::ILogHandler::LOG_LEVEL_COMMAND);
			}
		}
	}
}

UInt32 __stdcall Net::NTPServer::CheckThread(void *userObj)
{
	Net::NTPServer *me = (Net::NTPServer*)userObj;
	Net::SocketUtil::AddressInfo addr;
	Text::StringBuilderUTF8 *sb;
	Data::DateTime *dt;
	UTF8Char sbuff[32];
	UTF8Char *sptr;
	UOSInt tsLen;
	me->threadRunning = true;
	tsLen = Text::StrCharCnt(me->timeServer);
	NEW_CLASS(dt, Data::DateTime());
	NEW_CLASS(sb, Text::StringBuilderUTF8());
	while (!me->threadToStop)
	{
		if (me->sockf->DNSResolveIP(me->timeServer, tsLen, &addr))
		{
			if (me->cli->GetServerTime(&addr, Net::NTPClient::GetDefaultPort(), dt))
			{
				if (dt->SetAsComputerTime())
				{
					me->refTime = dt->ToTicks();
					me->timeDiff = 0;
					sb->ClearStr();
					sb->AppendC(UTF8STRC("NTP: Time updated from Time Server as "));
					dt->ToLocalTime();
					sptr = dt->ToString(sbuff, "yyyy-MM-dd HH:mm:ss.fff");
					sb->AppendC(sbuff, (UOSInt)(sptr - sbuff));
					me->log->LogMessageC(sb->ToString(), sb->GetLength(), IO::ILogHandler::LOG_LEVEL_ACTION);
				}
				else
				{
					me->refTime = dt->ToTicks();
					sb->ClearStr();
					sb->AppendC(UTF8STRC("NTP: Time update to "));
					dt->ToLocalTime();
					sptr = dt->ToString(sbuff, "yyyy-MM-dd HH:mm:ss.fff");
					sb->AppendC(sbuff, (UOSInt)(sptr - sbuff));
					sb->AppendC(UTF8STRC(" failed"));
					me->log->LogMessageC(sb->ToString(), sb->GetLength(), IO::ILogHandler::LOG_LEVEL_ERROR);
					dt->SetCurrTimeUTC();
					me->timeDiff = me->refTime - dt->ToTicks();
				}
			}
			else
			{
				if (me->log)
				{
					me->log->LogMessageC(UTF8STRC("NTP: Requesting to Time Server"), IO::ILogHandler::LOG_LEVEL_ERROR);
				}
			}
		}
		else
		{
			if (me->log)
			{
				me->log->LogMessageC(UTF8STRC("NTP: Error in resolving time server"), IO::ILogHandler::LOG_LEVEL_ERROR);
			}
		}
		me->evt->Wait(60000);
	}
	DEL_CLASS(sb);
	DEL_CLASS(dt);
	me->threadRunning = false;
	return 0;
}

void Net::NTPServer::InitServer(Net::SocketFactory *sockf, UInt16 port)
{
	Net::UDPServer *svr;
	NEW_CLASS(svr, Net::UDPServer(sockf, 0, port, 0, PacketHdlr, this, 0, 0, 2, false));
	if (svr->IsError())
	{
		DEL_CLASS(svr);
	}
	else
	{
		this->svr = svr;
	}
}

Net::NTPServer::NTPServer(Net::SocketFactory *sockf, UInt16 port, IO::LogTool *log, const UTF8Char *timeServer)
{
	this->sockf = sockf;
	this->svr = 0;
	this->log = log;
	this->timeServer = Text::StrCopyNew(timeServer);
	this->refTime = 0;
	this->threadRunning = false;
	this->threadToStop = false;
	NEW_CLASS(this->evt, Sync::Event(true, (const UTF8Char*)"Net.NTPServer.evt"));
	this->cli = 0;
	InitServer(sockf, port);
	if (this->svr)
	{
		NEW_CLASS(this->cli, Net::NTPClient(this->sockf, 0));
		Sync::Thread::Create(CheckThread, this);
		while (!this->threadRunning)
		{
			Sync::Thread::Sleep(1);
		}
	}
}

Net::NTPServer::~NTPServer()
{
	SDEL_CLASS(this->svr);
	if (this->threadRunning)
	{
		this->threadToStop = true;
		this->evt->Set();
		while (this->threadRunning)
		{
			Sync::Thread::Sleep(1);
		}
	}
	DEL_CLASS(this->evt);
	SDEL_CLASS(this->cli);
	SDEL_TEXT(this->timeServer);
}

Bool Net::NTPServer::IsError()
{
	return this->svr == 0;
}

void Net::NTPServer::ReadTime(const UInt8 *buff, Data::DateTime *time)
{
	time->ToUTCTime();
	time->SetValue(1900, 1, 1, 0, 0, 0, 0);
	UInt32 v = ReadMUInt32(&buff[0]);
	UInt32 v2 = ReadMUInt32(&buff[4]);

	time->AddMinute((OSInt)v / 60);
	time->AddMS((OSInt)((v % 60) * 1000 + ((v2 * 1000LL) >> 32)));
}

void Net::NTPServer::WriteTime(UInt8 *buff, Data::DateTime *time)
{
	Int64 timeDiff;
	Data::DateTime dt;
	dt.ToUTCTime();
	dt.SetValue(1900, 1, 1, 0, 0, 0, 0);

	timeDiff = time->DiffMS(&dt);
	WriteMInt32(&buff[0], (Int32)(timeDiff / 1000));
	timeDiff = timeDiff % 1000;
	WriteMInt32(&buff[4], (MulDiv32(0x40000000, (Int32)timeDiff, 500) << 1) + 1);
}

void Net::NTPServer::WriteTime(UInt8 *buff, Int64 timeTicks)
{
	Int64 timeDiff = timeTicks + 2208988800000LL;
	WriteMInt32(&buff[0], (Int32)(timeDiff / 1000));
	timeDiff = timeDiff % 1000;
	WriteMInt32(&buff[4], (MulDiv32(0x40000000, (Int32)timeDiff, 500) << 1) + 1);
}
