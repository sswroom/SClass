#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ByteTool.h"
#include "Map/ReverseGeocoderClient.h"
#include "Math/Math.h"
#include "Net/TCPClient.h"
#include "Sync/MutexUsage.h"
#include "Text/Encoding.h"
#include "Text/MyString.h"
#include "Sync/SimpleThread.h"
#include "Sync/ThreadUtil.h"

UInt32 __stdcall Map::ReverseGeocoderClient::ClientThread(AnyType userObj)
{
	NN<Map::ReverseGeocoderClient> me = userObj.GetNN<Map::ReverseGeocoderClient>();
	UInt8 *recvBuff;
	OSInt buffSize = 0;
	OSInt readSize;
	NN<Net::TCPClient> cli;

	me->cliRunning = true;
	recvBuff = MemAlloc(UInt8, 4096);
	while (!me->cliToStop)
	{
		if (!cli.Set(me->cli))
		{
			Sync::MutexUsage mutUsage(me->cliMut);
			NEW_CLASSNN(cli, Net::TCPClient(me->sockf, me->host->ToCString(), me->port, 15000));
			if (cli->IsConnectError())
			{
				cli.Delete();
				if (!me->cliToStop)
				{
					Sync::SimpleThread::Sleep(100);
				}
			}
			else
			{
				me->lastKASent.SetCurrTimeUTC();
				me->lastKARecv.SetCurrTimeUTC();
				me->errWriter->WriteLine(CSTR("ReverseGeocoder connected"));
				me->cli = cli.Ptr();
			}
		}
		else
		{
			readSize = cli->Read(Data::ByteArray(&recvBuff[buffSize], 4096 - buffSize));
			if (readSize <= 0)
			{
				Sync::MutexUsage mutUsage(me->cliMut);
				DEL_CLASS(me->cli);
				me->cli = 0;
				mutUsage.EndUse();
				me->errWriter->WriteLine(CSTR("ReverseGeocoder disconnected"));
			}
			else
			{
				buffSize += readSize;
				readSize = me->protocol.ParseProtocol(cli, me, 0, Data::ByteArrayR(recvBuff, buffSize));
				if (readSize >= 4096 || readSize <= 0)
				{
					buffSize = 0;
				}
				else if (readSize < buffSize)
				{
					MemCopyO(recvBuff, &recvBuff[buffSize - readSize], readSize);
					buffSize = readSize;
				}
			}
		}
	}
	{
		Sync::MutexUsage mutUsage(me->cliMut);
		if (me->cli)
		{
			DEL_CLASS(me->cli);
			me->cli = 0;
		}
	}
	MemFree(recvBuff);
	me->cliRunning = false;
	return 0;
}

UInt32 __stdcall Map::ReverseGeocoderClient::MonThread(AnyType userObj)
{
	Data::DateTime currTime;
	UInt8 kaBuff[16];
	NN<Map::ReverseGeocoderClient> me = userObj.GetNN<Map::ReverseGeocoderClient>();
	me->monRunning = true;
	while (!me->monToStop)
	{
		currTime.SetCurrTimeUTC();
		if (currTime.DiffMS(me->lastKARecv) > 600000)
		{
			Sync::MutexUsage mutUsage(me->cliMut);
			if (me->cli)
			{
				me->errWriter->WriteLine(CSTR("ReverseGeocoder timed out"));
				me->cli->Close();
			}
		}
		else if (currTime.DiffMS(me->lastKASent) > 180000)
		{
			Sync::MutexUsage mutUsage(me->cliMut);
			if (me->cli)
			{
				me->cli->Write(Data::ByteArrayR(kaBuff, me->protocol.BuildPacket(kaBuff, 4, 0, kaBuff, 0, 0)));
				me->lastKASent.SetCurrTimeUTC();
//				me->errWriter->WriteLine(CSTR("KA sent"));
			}
		}
		me->monEvt.Wait(1000);
	}
	me->monRunning = false;
	return 0;
}

Map::ReverseGeocoderClient::ReverseGeocoderClient(NN<Net::SocketFactory> sockf, Text::CStringNN host, UInt16 port, Map::ReverseGeocoder *revGeo, IO::Writer *errWriter) : protocol(*this)
{
	this->sockf = sockf;
	this->host = Text::String::New(host);
	this->port = port;
	this->revGeo = revGeo;
	this->cli = 0;
	this->cliRunning = false;
	this->cliToStop = false;
	this->monRunning = false;
	this->monToStop = false;
	this->errWriter = errWriter;
	this->lastKASent.SetCurrTimeUTC();
	this->lastKARecv.SetCurrTimeUTC();
	Sync::ThreadUtil::Create(ClientThread, this);
	Sync::ThreadUtil::Create(MonThread, this);
}

Map::ReverseGeocoderClient::~ReverseGeocoderClient()
{
	this->cliToStop = true;
	this->monToStop = true;
	this->monEvt.Set();
	if (this->cli)
	{
		this->cli->Close();
	}

	while (this->cliRunning)
	{
		Sync::SimpleThread::Sleep(10);
	}
	while (this->monRunning)
	{
		Sync::SimpleThread::Sleep(10);
	}
	this->host->Release();
}

void Map::ReverseGeocoderClient::DataParsed(NN<IO::Stream> stm, AnyType stmObj, Int32 cmdType, Int32 seqId, UnsafeArray<const UInt8> cmd, UOSInt cmdSize)
{
	UInt8 buff[512];
	UInt8 buff2[512];
	NN<Map::ReverseGeocoderClient> me = stmObj.GetNN<Map::ReverseGeocoderClient>();
	UTF8Char sbuff[512];
	UnsafeArray<UTF8Char> sptr;
	Double lat;
	Double lon;
	UInt32 lcid;
	OSInt strSize;

	if (cmdType == 0)
	{
		lat = ReadInt32(&cmd[0]) / 200000.0;
		lon = ReadInt32(&cmd[4]) / 200000.0;
		lcid = ReadUInt32(&cmd[8]);
		sbuff[0] = 0;
		sptr = me->revGeo->SearchName(sbuff, sizeof(sbuff), Math::Coord2DDbl(lon, lat), lcid).Or(sbuff);
		WriteInt32(&buff[0], Double2Int32(lat * 200000.0));
		WriteInt32(&buff[4], Double2Int32(lon * 200000.0));
		WriteUInt32(&buff[8], lcid);
		strSize = (UOSInt)(sptr - sbuff);
		if (strSize >= 128)
		{
			buff[12] = (UInt8)(0x80 | (strSize >> 8));
			buff[13] = (UInt8)(strSize & 0xff);
			Text::StrConcat(&buff[14], sbuff);
			strSize = me->protocol.BuildPacket(buff2, 1, 0, buff, 14 + strSize, 0);
		}
		else
		{
			buff[12] = (UInt8)(strSize & 0xff);
			Text::StrConcat(&buff[13], sbuff);
			strSize = me->protocol.BuildPacket(buff2, 1, 0, buff, 13 + strSize, 0);
		}
		cli->Write(Data::ByteArrayR(buff2, strSize));
	}
	else if (cmdType == 2)
	{
		lat = ReadInt32(&cmd[0]) / 200000.0;
		lon = ReadInt32(&cmd[4]) / 200000.0;
		lcid = ReadUInt32(&cmd[8]);
		sbuff[0] = 0;
		sptr = me->revGeo->CacheName(sbuff, sizeof(sbuff), Math::Coord2DDbl(lon, lat), lcid).Or(sbuff);
		strSize = (UOSInt)(sptr - sbuff);
		if (strSize >= 128)
		{
			buff[12] = (UInt8)(0x80 | (strSize >> 8));
			buff[13] = (UInt8)(strSize & 0xff);
			Text::StrConcat(&buff[14], sbuff);
			strSize = me->protocol.BuildPacket(buff2, 3, 0, buff, 14 + strSize, 0);
		}
		else
		{
			buff[12] = (UInt8)(strSize & 0xff);
			Text::StrConcat(&buff[13], sbuff);
			strSize = me->protocol.BuildPacket(buff2, 3, 0, buff, 13 + strSize, 0);
		}
		cli->Write(Data::ByteArrayR(buff2, strSize));
	}
	else if (cmdType == 4)
	{
//		this->errWriter->WriteLine(CSTR("KA Received");
		this->lastKARecv.SetCurrTimeUTC();
	}
}

void Map::ReverseGeocoderClient::DataSkipped(NN<IO::Stream> stm, AnyType stmObj, UnsafeArray<const UInt8> buff, UOSInt buffSize)
{
}
