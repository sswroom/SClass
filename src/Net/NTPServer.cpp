#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ByteTool.h"
#include "Data/DateTime.h"
#include "Net/NTPClient.h"
#include "Net/NTPServer.h"
#include "Sync/SimpleThread.h"
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
				me->log->LogMessage(sb.ToCString(), IO::LogHandler::LogLevel::Command);
			}
		}
	}
}

UInt32 __stdcall Net::NTPServer::CheckThread(void *userObj)
{
	Net::NTPServer *me = (Net::NTPServer*)userObj;
	Net::SocketUtil::AddressInfo addr;
	UTF8Char sbuff[32];
	UTF8Char *sptr;
	{
		me->threadRunning = true;
		Data::DateTime dt;
		Text::StringBuilderUTF8 sb;
		while (!me->threadToStop)
		{
			if (me->sockf->DNSResolveIP(me->timeServer->ToCString(), &addr))
			{
				if (me->cli->GetServerTime(&addr, Net::NTPClient::GetDefaultPort(), &dt))
				{
					if (dt.SetAsComputerTime())
					{
						me->refTime = dt.ToTicks();
						me->timeDiff = 0;
						sb.ClearStr();
						sb.AppendC(UTF8STRC("NTP: Time updated from Time Server as "));
						dt.ToLocalTime();
						sptr = dt.ToString(sbuff, "yyyy-MM-dd HH:mm:ss.fff");
						sb.AppendC(sbuff, (UOSInt)(sptr - sbuff));
						me->log->LogMessage(sb.ToCString(), IO::LogHandler::LogLevel::Action);
					}
					else
					{
						me->refTime = dt.ToTicks();
						sb.ClearStr();
						sb.AppendC(UTF8STRC("NTP: Time update to "));
						dt.ToLocalTime();
						sptr = dt.ToString(sbuff, "yyyy-MM-dd HH:mm:ss.fff");
						sb.AppendC(sbuff, (UOSInt)(sptr - sbuff));
						sb.AppendC(UTF8STRC(" failed"));
						me->log->LogMessage(sb.ToCString(), IO::LogHandler::LogLevel::Error);
						dt.SetCurrTimeUTC();
						me->timeDiff = me->refTime - dt.ToTicks();
					}
				}
				else
				{
					if (me->log)
					{
						me->log->LogMessage(CSTR("NTP: Requesting to Time Server"), IO::LogHandler::LogLevel::Error);
					}
				}
			}
			else
			{
				if (me->log)
				{
					me->log->LogMessage(CSTR("NTP: Error in resolving time server"), IO::LogHandler::LogLevel::Error);
				}
			}
			me->evt->Wait(60000);
		}
	}
	me->threadRunning = false;
	return 0;
}

void Net::NTPServer::InitServer(Net::SocketFactory *sockf, UInt16 port)
{
	Net::UDPServer *svr;
	NEW_CLASS(svr, Net::UDPServer(sockf, 0, port, CSTR_NULL, PacketHdlr, this, 0, CSTR_NULL, 2, false));
	if (svr->IsError())
	{
		DEL_CLASS(svr);
	}
	else
	{
		this->svr = svr;
	}
}

Net::NTPServer::NTPServer(Net::SocketFactory *sockf, UInt16 port, IO::LogTool *log, Text::CString timeServer)
{
	this->sockf = sockf;
	this->svr = 0;
	this->log = log;
	this->timeServer = Text::String::New(timeServer);
	this->refTime = 0;
	this->threadRunning = false;
	this->threadToStop = false;
	NEW_CLASS(this->evt, Sync::Event(true));
	this->cli = 0;
	InitServer(sockf, port);
	if (this->svr)
	{
		NEW_CLASS(this->cli, Net::NTPClient(this->sockf, 0));
		Sync::Thread::Create(CheckThread, this);
		while (!this->threadRunning)
		{
			Sync::SimpleThread::Sleep(1);
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
			Sync::SimpleThread::Sleep(1);
		}
	}
	DEL_CLASS(this->evt);
	SDEL_CLASS(this->cli);
	SDEL_STRING(this->timeServer);
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

Data::Timestamp Net::NTPServer::ReadTime(const UInt8 *buff)
{
	UInt32 v = ReadMUInt32(&buff[0]);
	UInt32 v2 = ReadMUInt32(&buff[4]);
	return Data::Timestamp(Data::TimeInstant(v - 2208988800LL, (UInt32)((v2 * 1000000000ULL) >> 32)), 0);
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

void Net::NTPServer::WriteTime(UInt8 *buff, const Data::Timestamp &time)
{
	Int64 timeSecs = time.inst.sec - 2208988800LL;
	WriteMInt32(&buff[0], (Int32)timeSecs);
	WriteMUInt32(&buff[4], (UInt32)((((UInt64)time.inst.nanosec) << 32) / 1000000000) + 1);
}

void Net::NTPServer::WriteTime(UInt8 *buff, Int64 timeTicks)
{
	Int64 timeDiff = timeTicks + 2208988800000LL;
	WriteMInt32(&buff[0], (Int32)(timeDiff / 1000));
	timeDiff = timeDiff % 1000;
	WriteMInt32(&buff[4], (MulDiv32(0x40000000, (Int32)timeDiff, 500) << 1) + 1);
}
