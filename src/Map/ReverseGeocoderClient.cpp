#include "Stdafx.h"
#include "MyMemory.h"
#include "Map/ReverseGeocoderClient.h"
#include "Math/Math.h"
#include "Net/TCPClient.h"
#include "Text/Encoding.h"
#include "Text/MyString.h"
#include "Sync/Thread.h"

UInt32 __stdcall Map::ReverseGeocoderClient::ClientThread(void *userObj)
{
	Map::ReverseGeocoderClient *me = (Map::ReverseGeocoderClient*)userObj;
	UInt8 *recvBuff;
	OSInt buffSize = 0;
	OSInt readSize;

	me->cliRunning = true;
	recvBuff = MemAlloc(UInt8, 4096);
	while (!me->cliToStop)
	{
		if (me->cli == 0)
		{
			me->cliMut->Lock();
			NEW_CLASS(me->cli, Net::TCPClient(me->sockf, me->host, me->port));
			if (me->cli->IsConnectError())
			{
				DEL_CLASS(me->cli);
				me->cli = 0;
			}
			else
			{
				me->lastKASent->SetCurrTimeUTC();
				me->lastKARecv->SetCurrTimeUTC();
				me->errWriter->WriteLineC(UTF8STRC("ReverseGeocoder connected");
			}
			me->cliMut->Unlock();
		}
		if (me->cli)
		{
			readSize = me->cli->Read(&recvBuff[buffSize], 4096 - buffSize);
			if (readSize <= 0)
			{
				me->cliMut->Lock();
				DEL_CLASS(me->cli);
				me->cli = 0;
				me->cliMut->Unlock();
				me->errWriter->WriteLineC(UTF8STRC("ReverseGeocoder disconnected");
			}
			else
			{
				buffSize += readSize;
				readSize = me->protocol->ParseProtocol(me->cli, me, 0, recvBuff, buffSize);
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
		else if (!me->cliToStop)
		{
			Sync::Thread::Sleep(100);
		}
	}
	me->cliMut->Lock();
	if (me->cli)
	{
		DEL_CLASS(me->cli);
		me->cli = 0;
	}
	me->cliMut->Unlock();
	MemFree(recvBuff);
	me->cliRunning = false;
	return 0;
}

UInt32 __stdcall Map::ReverseGeocoderClient::MonThread(void *userObj)
{
	Data::DateTime currTime;
	UInt8 kaBuff[16];
	Map::ReverseGeocoderClient *me = (Map::ReverseGeocoderClient*)userObj;
	me->monRunning = true;
	while (!me->monToStop)
	{
		currTime.SetCurrTimeUTC();
		if (currTime.DiffMS(me->lastKARecv) > 600000)
		{
			me->cliMut->Lock();
			if (me->cli)
			{
				me->errWriter->WriteLineC(UTF8STRC("ReverseGeocoder timed out");
				me->cli->Close();
			}
			me->cliMut->Unlock();
		}
		else if (currTime.DiffMS(me->lastKASent) > 180000)
		{
			me->cliMut->Lock();
			if (me->cli)
			{
				me->cli->Write(kaBuff, me->protocol->BuildPacket(kaBuff, 4, 0, 0, 0, 0));
				me->lastKASent->SetCurrTimeUTC();
//				me->errWriter->WriteLineC(UTF8STRC("KA sent"));
			}
			me->cliMut->Unlock();
		}
		me->monEvt->Wait(1000);
	}
	me->monRunning = false;
	return 0;
}

Map::ReverseGeocoderClient::ReverseGeocoderClient(Net::SocketFactory *sockf, const UTF8Char *host, UInt16 port, Map::IReverseGeocoder *revGeo, IO::Writer *errWriter)
{
	this->sockf = sockf;
	this->host = Text::StrCopyNew(host);
	this->port = port;
	this->revGeo = revGeo;
	this->cli = 0;
	this->cliRunning = false;
	this->cliToStop = false;
	this->monRunning = false;
	this->monToStop = false;
	this->errWriter = errWriter;
	NEW_CLASS(this->cliMut, Sync::Mutex());
	NEW_CLASS(this->monEvt, Sync::Event(true, (const UTF8Char*)"Map.ReverseGeocoderClient.monEvt"));
	NEW_CLASS(this->lastKASent, Data::DateTime());
	NEW_CLASS(this->lastKARecv, Data::DateTime());
	NEW_CLASS(this->protocol, IO::ProtoHdlr::ProtoRevGeoHandler(this));
	this->lastKASent->SetCurrTimeUTC();
	this->lastKARecv->SetCurrTimeUTC();
	Sync::Thread::Create(ClientThread, this);
	Sync::Thread::Create(MonThread, this);
}

Map::ReverseGeocoderClient::~ReverseGeocoderClient()
{
	this->cliToStop = true;
	this->monToStop = true;
	this->monEvt->Set();
	if (this->cli)
	{
		this->cli->Close();
	}

	while (this->cliRunning)
	{
		Sync::Thread::Sleep(10);
	}
	while (this->monRunning)
	{
		Sync::Thread::Sleep(10);
	}
	Text::StrDelNew(this->host);
	DEL_CLASS(this->monEvt);
	DEL_CLASS(this->cliMut);
	DEL_CLASS(this->lastKASent);
	DEL_CLASS(this->lastKARecv);
	DEL_CLASS(this->protocol);
}

void Map::ReverseGeocoderClient::DataParsed(IO::Stream *stm, void *stmObj, Int32 cmdType, Int32 seqId, const UInt8 *cmd, UOSInt cmdSize)
{
	UInt8 buff[512];
	UInt8 buff2[512];
	Map::ReverseGeocoderClient *me = (Map::ReverseGeocoderClient*)stmObj;
	UTF8Char sbuff[512];
	UTF8Char *sptr;
	Double lat;
	Double lon;
	Int32 lcid;
	OSInt strSize;

	if (cmdType == 0)
	{
		lat = *(Int32*)&cmd[0] / 200000.0;
		lon = *(Int32*)&cmd[4] / 200000.0;
		lcid = *(Int32*)&cmd[8];
		sbuff[0] = 0;
		sptr = me->revGeo->SearchName(sbuff, sizeof(sbuff), lat, lon, lcid);
		*(Int32*)&buff[0] = Math::Double2Int32(lat * 200000.0);
		*(Int32*)&buff[4] = Math::Double2Int32(lon * 200000.0);
		*(Int32*)&buff[8] = lcid;
		strSize = Text::StrCharCnt(sbuff);
		if (strSize >= 128)
		{
			buff[12] = (UInt8)(0x80 | (strSize >> 8));
			buff[13] = (UInt8)(strSize & 0xff);
			Text::StrConcat(&buff[14], sbuff);
			strSize = me->protocol->BuildPacket(buff2, 1, 0, buff, 14 + strSize, 0);
		}
		else
		{
			buff[12] = (UInt8)(strSize & 0xff);
			Text::StrConcat(&buff[13], sbuff);
			strSize = me->protocol->BuildPacket(buff2, 1, 0, buff, 13 + strSize, 0);
		}
		cli->Write(buff2, strSize);
	}
	else if (cmdType == 2)
	{
		lat = *(Int32*)&cmd[0] / 200000.0;
		lon = *(Int32*)&cmd[4] / 200000.0;
		lcid = *(Int32*)&cmd[8];
		sbuff[0] = 0;
		sptr = me->revGeo->CacheName(sbuff, sizeof(sbuff), lat, lon, lcid);
		strSize = Text::StrCharCnt(sbuff);
		if (strSize >= 128)
		{
			buff[12] = (UInt8)(0x80 | (strSize >> 8));
			buff[13] = (UInt8)(strSize & 0xff);
			Text::StrConcat(&buff[14], sbuff);
			strSize = me->protocol->BuildPacket(buff2, 3, 0, buff, 14 + strSize, 0);
		}
		else
		{
			buff[12] = (UInt8)(strSize & 0xff);
			Text::StrConcat(&buff[13], sbuff);
			strSize = me->protocol->BuildPacket(buff2, 3, 0, buff, 13 + strSize, 0);
		}
		cli->Write(buff2, strSize);
	}
	else if (cmdType == 4)
	{
//		this->errWriter->WriteLineC(UTF8STRC("KA Received");
		this->lastKARecv->SetCurrTimeUTC();
	}
}

void Map::ReverseGeocoderClient::DataSkipped(IO::Stream *stm, void *stmObj, const UInt8 *buff, UOSInt buffSize)
{
}
