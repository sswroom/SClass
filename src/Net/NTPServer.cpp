#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ByteTool.h"
#include "Data/DateTime.h"
#include "Net/NTPClient.h"
#include "Net/NTPServer.h"
#include "Sync/SimpleThread.h"
#include "Sync/ThreadUtil.h"
#include "Text/MyString.h"
#include "Text/StringBuilderUTF8.h"

void __stdcall Net::NTPServer::PacketHdlr(NN<const Net::SocketUtil::AddressInfo> addr, UInt16 port, Data::ByteArrayR data, AnyType userData)
{
	NN<Net::NTPServer> me = userData.GetNN<Net::NTPServer>();
	UInt8 repBuff[48];
	if (data.GetSize() >= 48)
	{
		UInt8 vn = (data[0] >> 3) & 7;
		UInt8 mode = data[0] & 7;
		if (mode == 3)
		{
			Data::DateTime dt;
			Int64 currTime;
			dt.SetCurrTimeUTC();
			currTime = dt.ToTicks();
			repBuff[0] = (UInt8)((vn << 3) | 4);
			repBuff[1] = 2; //Secondary Server
			repBuff[2] = data[2]; //Poll
			repBuff[3] = 3;
			WriteMInt32(&repBuff[4], 0); //Root Delay
			WriteMInt32(&repBuff[8], 0); //Root Dispersion
			WriteMInt32(&repBuff[12], (Int32)(currTime & 0xffffffff)); //Reference ID
			WriteTime(&repBuff[16], me->refTime); //Reference Timestamp
			WriteNInt64(&repBuff[24], ReadNInt64(&data[40])); //Origin Timestamp
			WriteTime(&repBuff[32], currTime + me->timeDiff);
			WriteTime(&repBuff[40], currTime + me->timeDiff);
			me->svr->SendTo(addr, port, repBuff, 48);

			if (me->log->HasHandler())
			{
				Text::StringBuilderUTF8 sb;
				UTF8Char sbuff[64];
				UnsafeArray<UTF8Char> sptr;
				sb.AppendC(UTF8STRC("NTP: Client "));
				sptr = Net::SocketUtil::GetAddrName(sbuff, addr, port).Or(sbuff);
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

void __stdcall Net::NTPServer::CheckThread(NN<Sync::Thread> thread)
{
	NN<Net::NTPServer> me = thread->GetUserObj().GetNN<Net::NTPServer>();
	Net::SocketUtil::AddressInfo addr;
	UTF8Char sbuff[32];
	UnsafeArray<UTF8Char> sptr;
	Data::DateTime dt;
	Text::StringBuilderUTF8 sb;
	while (!thread->IsStopping())
	{
		if (me->sockf->DNSResolveIP(me->timeServerHost->ToCString(), addr))
		{
			if (me->cli->GetServerTime(addr, Net::NTPClient::GetDefaultPort(), dt))
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
				me->log->LogMessage(CSTR("NTP: Requesting to Time Server"), IO::LogHandler::LogLevel::Error);
			}
		}
		else
		{
			me->log->LogMessage(CSTR("NTP: Error in resolving time server"), IO::LogHandler::LogLevel::Error);
		}
		thread->Wait(60000);
	}
}

void Net::NTPServer::InitServer(NN<Net::SocketFactory> sockf, UInt16 port)
{
	Net::UDPServer *svr;
	NEW_CLASS(svr, Net::UDPServer(sockf, 0, port, CSTR_NULL, PacketHdlr, this, this->log, CSTR_NULL, 2, false));
	if (svr->IsError())
	{
		DEL_CLASS(svr);
	}
	else
	{
		this->svr = svr;
	}
}

Net::NTPServer::NTPServer(NN<Net::SocketFactory> sockf, UInt16 port, NN<IO::LogTool> log, Text::CStringNN timeServerHost) : thread(CheckThread, this, CSTR("NTPServer"))
{
	this->sockf = sockf;
	this->svr = 0;
	this->log = log;
	this->timeServerHost = Text::String::New(timeServerHost);
	this->refTime = 0;
	this->cli = 0;
	InitServer(sockf, port);
	if (this->svr)
	{
		NEW_CLASS(this->cli, Net::NTPClient(this->sockf, 0, this->log));
		this->thread.Start();
	}
}

Net::NTPServer::~NTPServer()
{
	SDEL_CLASS(this->svr);
	this->thread.Stop();
	SDEL_CLASS(this->cli);
	this->timeServerHost->Release();
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
	return Data::Timestamp::FromNTPTime(ReadMUInt32(&buff[0]), ReadMUInt32(&buff[4]), 0);
}

void Net::NTPServer::WriteTime(UInt8 *buff, Data::DateTime *time)
{
	Int64 timeDiff;
	Data::DateTime dt;
	dt.ToUTCTime();
	dt.SetValue(1900, 1, 1, 0, 0, 0, 0);

	timeDiff = time->DiffMS(dt);
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
