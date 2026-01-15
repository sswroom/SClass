#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/ByteTool_C.h"
#include "Map/ReverseGeocoderServer.h"
#include "Math/Math_C.h"
#include "Sync/MutexUsage.h"
#include "Sync/ThreadUtil.h"
#include "Text/Encoding.h"
#include "Text/MyString.h"

Net::TCPClient *Map::ReverseGeocoderServer::GetLatestClient(UIntOS retryCnt)
{
	Int64 currTime;
	Data::DateTime dt;
	Int64 maxTime = -1;
	UIntOS maxIndex = 0;
	NN<Net::TCPClient> cli;
	AnyType cliObj;
	NN<ClientStatus> stat;

	dt.SetCurrTimeUTC();
	currTime = dt.ToTicks();

	Sync::MutexUsage mutUsage;
	this->ctrl->UseGetCli(mutUsage);
	UIntOS i = this->ctrl->GetCliCount();
	if (i <= retryCnt)
	{
		return 0;
	}

	while (i-- > 0)
	{
		if (this->ctrl->GetClient(i, cliObj).SetTo(cli))
		{
			stat = cliObj.GetNN<ClientStatus>();
			if (currTime - stat->lastReqTime > maxTime)
			{
				maxTime = currTime - stat->lastReqTime;
				maxIndex = i;
			}
		}
	}
	if (maxTime >= 0)
	{
		if (this->ctrl->GetClient(maxIndex, cliObj).SetTo(cli))
		{
			stat = cliObj.GetNN<ClientStatus>();
			stat->lastReqTime = currTime;
		}
	}
	return 0;
}

Map::ReverseGeocoderServer::ReverseGeocoderServer(NN<Net::SocketFactory> sockf, NN<IO::LogTool> log, UInt16 port) : protocol(*this)
{
	NEW_CLASS(this->ctrl, Net::TCPServerController(sockf, log, port, CSTR("RGS: "), 4096, this, 8, 240, true));
}

Map::ReverseGeocoderServer::~ReverseGeocoderServer()
{
	DEL_CLASS(this->ctrl);
}

UnsafeArrayOpt<UTF8Char> Map::ReverseGeocoderServer::SearchName(UnsafeArray<UTF8Char> buff, UIntOS buffSize, Math::Coord2DDbl pos, UInt32 lcid)
{
	UInt8 dataBuff[16];
	UInt8 dataBuff2[256];
	Bool sent;
	UIntOS retryCnt = 0;
	UnsafeArrayOpt<UTF8Char> sptr = nullptr;
	Sync::MutexUsage mutUsage(this->reqMut);
	this->reqBuff = buff;
	this->reqBuffSize = buffSize;
	this->reqLat = Double2Int32(pos.GetLat() * 200000.0);
	this->reqLon = Double2Int32(pos.GetLon() * 200000.0);
	this->reqLCID = lcid;
	this->reqResult = false;
	buff[0] = 0;

	Net::TCPClient *cli;
	while ((cli = GetLatestClient(retryCnt)) != 0)
	{
		sent = false;
		if (cli)
		{
			WriteInt32(&dataBuff[0], this->reqLat);
			WriteInt32(&dataBuff[4], this->reqLon);
			WriteUInt32(&dataBuff[8], this->reqLCID);
			cli->Write(Data::ByteArrayR(dataBuff2, this->protocol.BuildPacket(dataBuff2, 0, 0, dataBuff, 12, 0)));
			sent = true;
		}
		sptr = nullptr;
		if (sent)
		{
			IntOS i = 10;
			while (!this->reqResult)
			{
				this->reqEvt.Wait(1000);
				if (i-- <= 0)
					break;
			}
			if (this->reqResult)
			{
				sptr = &buff[Text::StrCharCnt(buff)];
			}
		}
		if (buff[0] != 0)
			break;
		this->reqResult = false;
		retryCnt++;
	}
	this->reqBuff = nullptr;
	mutUsage.EndUse();
	return sptr;
}

UnsafeArrayOpt<UTF8Char> Map::ReverseGeocoderServer::CacheName(UnsafeArray<UTF8Char> buff, UIntOS buffSize, Math::Coord2DDbl pos, UInt32 lcid)
{
	UInt8 dataBuff[16];
	UInt8 dataBuff2[256];
	Bool sent;
	UIntOS retryCnt = 0;
	UnsafeArrayOpt<UTF8Char> sptr = nullptr;
	Sync::MutexUsage mutUsage(this->reqMut);
	this->reqBuff = buff;
	this->reqBuffSize = buffSize;
	this->reqLat = Double2Int32(pos.GetLat() * 200000.0);
	this->reqLon = Double2Int32(pos.GetLon() * 200000.0);
	this->reqLCID = lcid;
	this->reqResult = false;
	buff[0] = 0;

	Net::TCPClient *cli;
	while ((cli = GetLatestClient(retryCnt)) != 0)
	{
		sent = false;
		if (cli)
		{
			WriteInt32(&dataBuff[0], this->reqLat);
			WriteInt32(&dataBuff[4], this->reqLon);
			WriteUInt32(&dataBuff[8], this->reqLCID);
			cli->Write(Data::ByteArrayR(dataBuff2, this->protocol.BuildPacket(dataBuff2, 2, 0, dataBuff, 12, 0)));
			sent = true;
		}
		sptr = nullptr;
		if (sent)
		{
			IntOS i = 10;
			while (!this->reqResult)
			{
				this->reqEvt.Wait(1000);
				if (i-- <= 0)
					break;
			}
			if (this->reqResult)
			{
				sptr = &buff[Text::StrCharCnt(buff)];
			}
		}
		if (buff[0] != 0)
			break;
		this->reqResult = false;
		retryCnt++;
	}
	this->reqBuff = nullptr;
	mutUsage.EndUse();
	return sptr;}

AnyType Map::ReverseGeocoderServer::NewConn(NN<Net::TCPClient> cli)
{
	ClientStatus *stat = MemAlloc(ClientStatus, 1);
	stat->cliData = this->protocol.CreateStreamData(cli);
	stat->lastReqTime = 0;
	return stat;
}

void Map::ReverseGeocoderServer::EndConn(NN<Net::TCPClient> cli, AnyType cliObj)
{
	NN<ClientStatus> stat = cliObj.GetNN<ClientStatus>();
	this->protocol.DeleteStreamData(cli, stat->cliData);
	MemFree(stat.Ptr());
}

UIntOS Map::ReverseGeocoderServer::ReceivedData(NN<Net::TCPClient> cli, AnyType cliObj, const Data::ByteArrayR &buff)
{
	NN<ClientStatus> stat = cliObj.GetNN<ClientStatus>();
	return this->protocol.ParseProtocol(cli, cliObj, stat->cliData, buff);
}

void Map::ReverseGeocoderServer::DataParsed(NN<IO::Stream> stm, AnyType cliObj, Int32 cmdType, Int32 seqId, UnsafeArray<const UInt8> cmd, UIntOS cmdSize)
{
	UnsafeArray<UTF8Char> reqBuff;
	if (cmdType == 1)
	{
		if (this->reqBuff.SetTo(reqBuff) && ReadInt32(&cmd[0]) == this->reqLat && ReadInt32(&cmd[4]) == this->reqLon && ReadUInt32(&cmd[8]) == this->reqLCID)
		{
			UIntOS strSize;
			if ((cmd[12] & 0x80) != 0)
			{
				strSize = (UIntOS)(((cmd[12] & 0x7f) << 8) | cmd[13]);
				cmd += 14;
			}
			else
			{
				strSize = cmd[12];
				cmd += 13;
			}
			if (strSize > 0)
			{
				if (this->reqBuffSize > strSize)
				{
					Text::StrConcatC(reqBuff, cmd, strSize);
				}
				else
				{
					Text::StrConcatC(reqBuff, cmd, this->reqBuffSize - 1);
				}
			}
			this->reqResult = true;
			this->reqEvt.Set();
		}
	}
	else if (cmdType == 3)
	{
		if (this->reqBuff.SetTo(reqBuff) && ReadInt32(&cmd[0]) == this->reqLat && ReadInt32(&cmd[4]) == this->reqLon && ReadUInt32(&cmd[8]) == this->reqLCID)
		{
			UIntOS strSize;
			if ((cmd[12] & 0x80) != 0)
			{
				strSize = (UIntOS)(((cmd[12] & 0x7f) << 8) | cmd[13]);
				cmd += 14;
			}
			else
			{
				strSize = cmd[12];
				cmd += 13;
			}
			if (strSize > 0)
			{
				if (this->reqBuffSize > strSize)
				{
					Text::StrConcatC(reqBuff, cmd, strSize);
				}
				else
				{
					Text::StrConcatC(reqBuff, cmd, this->reqBuffSize - 1);
				}
			}
			this->reqResult = true;
			this->reqEvt.Set();
		}
	}
	else if (cmdType == 4)
	{
		UInt8 buff[16];
		stm->Write(Data::ByteArrayR(buff, this->protocol.BuildPacket(buff, 4, 0, buff, 0, 0)));
	}
}

void Map::ReverseGeocoderServer::DataSkipped(NN<IO::Stream> stm, AnyType cliObj, UnsafeArray<const UInt8> buff, UIntOS buffSize)
{
}

Bool Map::ReverseGeocoderServer::IsError()
{
	return this->ctrl->IsError();
}
