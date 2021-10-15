#include "Stdafx.h"
#include "Crypto/Hash/CRC16.h"
#include "Crypto/Hash/MD5.h"
#include "Data/ByteTool.h"
#include "DB/DBReader.h"
#include "DB/MySQLConn.h"
#include "DB/ODBCConn.h"
#include "IO/FileStream.h"
#include "IO/IniFile.h"
#include "IO/MemoryStream.h"
#include "IO/Path.h"
#include "IO/StreamWriter.h"
#include "IO/ProtoHdlr/ProtoSMonHandler.h"
#include "Net/OSSocketFactory.h"
#include "Net/WebServer/HTTPDirectoryHandler.h"
#include "Parser/FullParserList.h"
#include "SSWR/Benchmark/BenchmarkWebHandler.h"
#include "SSWR/SMonitor/SMonitorSvrCore.h"
#include "SSWR/SMonitor/SMonitorWebHandler.h"
#include "Sync/Thread.h"
#include "Text/StringBuilderUTF8.h"
#include "Text/UTF8Reader.h"
#include "Text/UTF8Writer.h"

void __stdcall SSWR::SMonitor::SMonitorSvrCore::OnClientEvent(Net::TCPClient *cli, void *userObj, void *cliData, Net::TCPClientMgr::TCPEventType evtType)
{
	SSWR::SMonitor::SMonitorSvrCore *me = (SSWR::SMonitor::SMonitorSvrCore*)userObj;
	UTF8Char sbuff[32];
	ClientStatus *status = (ClientStatus*)cliData;
	switch (evtType)
	{
	case Net::TCPClientMgr::TCP_EVENT_DISCONNECT:
		{
			cli->GetRemoteName(sbuff);
			Text::StringBuilderUTF8 sb;
			sb.Append((const UTF8Char*)"CLI: Client disconnected: ");
			sb.Append(sbuff);
			me->log->LogMessage(sb.ToString(), IO::ILogHandler::LOG_LEVEL_ACTION);

			if (status->dev)
			{
				status->dev->mut->LockWrite();
				if (status->dev->stm == cli)
				{
					status->dev->stm = 0;
				}
				status->dev->mut->UnlockWrite();
			}
			MemFree(status->dataBuff);
			me->protoHdlr->DeleteStreamData(cli, status->stmData);
			MemFree(status);
			DEL_CLASS(cli);
		}
		break;
	case Net::TCPClientMgr::TCP_EVENT_CONNECT:
	case Net::TCPClientMgr::TCP_EVENT_HASDATA:
	case Net::TCPClientMgr::TCP_EVENT_SHUTDOWN:
	default:
		break;
	}
}

void __stdcall SSWR::SMonitor::SMonitorSvrCore::OnClientData(Net::TCPClient *cli, void *userObj, void *cliData, const UInt8 *buff, UOSInt size)
{
	SSWR::SMonitor::SMonitorSvrCore *me = (SSWR::SMonitor::SMonitorSvrCore*)userObj;
	ClientStatus *status = (ClientStatus*)cliData;
	MemCopyNO(&status->dataBuff[status->dataSize], buff, size);
	status->dataSize += size;

	UOSInt retSize = me->protoHdlr->ParseProtocol(cli, status, status->stmData, status->dataBuff, status->dataSize);
	if (retSize == 0)
	{
		status->dataSize = 0;
	}
	else if (retSize == status->dataSize)
	{
	}
	else
	{
		MemCopyO(status->dataBuff, &status->dataBuff[status->dataSize - retSize], retSize);
		status->dataSize = retSize;
	}
}

void __stdcall SSWR::SMonitor::SMonitorSvrCore::OnClientTimeout(Net::TCPClient *cli, void *userObj, void *cliData)
{
	SSWR::SMonitor::SMonitorSvrCore *me = (SSWR::SMonitor::SMonitorSvrCore*)userObj;
	UTF8Char sbuff[32];
	cli->GetRemoteName(sbuff);
	Text::StringBuilderUTF8 sb;
	sb.Append((const UTF8Char*)"CLI: Client process timeout: ");
	sb.Append(sbuff);
	me->log->LogMessage(sb.ToString(), IO::ILogHandler::LOG_LEVEL_ACTION);
}

void __stdcall SSWR::SMonitor::SMonitorSvrCore::OnServerConn(Socket *s, void *userObj)
{
	SSWR::SMonitor::SMonitorSvrCore *me = (SSWR::SMonitor::SMonitorSvrCore*)userObj;
	Net::TCPClient *cli;
	ClientStatus *status;
	NEW_CLASS(cli, Net::TCPClient(me->sockf, s));
	status = MemAlloc(ClientStatus, 1);
	status->cliId = 0;
	status->dataBuff = MemAlloc(UInt8, 4096);
	status->dataSize = 0;
	status->me = me;
	status->dev = 0;
	status->stmData = me->protoHdlr->CreateStreamData(cli);
	me->cliMgr->AddClient(cli, status);
}

UInt32 __stdcall SSWR::SMonitor::SMonitorSvrCore::CheckThread(void *userObj)
{
	SSWR::SMonitor::SMonitorSvrCore *me = (SSWR::SMonitor::SMonitorSvrCore*)userObj;
	Data::DateTime *currTime;
	Data::DateTime *lastStoreTime;
	Data::ArrayList<DeviceInfo *> *devList;
	Data::ArrayList<DevRecord2*> *recList;
	DeviceInfo *dev;
	Int64 t;
	UOSInt i;
	UOSInt j;

	me->checkRunning = true;
	NEW_CLASS(currTime, Data::DateTime());
	NEW_CLASS(lastStoreTime, Data::DateTime());
	NEW_CLASS(devList, Data::ArrayList<DeviceInfo*>());
	NEW_CLASS(recList, Data::ArrayList<DevRecord2*>());
	lastStoreTime->SetCurrTimeUTC();
	while (!me->checkToStop)
	{
		currTime->SetCurrTimeUTC();
		if (currTime->DiffMS(lastStoreTime) >= 300000)
		{
			lastStoreTime->SetValue(currTime);
			me->SaveDatas();
			
			if (me->uaLog->IsModified())
			{
				me->UserAgentStore();
			}

			if (me->refererLog->IsModified())
			{
				me->RefererStore();
			}
		}
		t = currTime->ToTicks();
		if (t >= me->currDate + 86400000)
		{
			me->devMut->LockRead();
			devList->AddAll(me->devMap->GetValues());
			me->devMut->UnlockRead();

			me->dateMut->LockWrite();
			me->currDate += 86400000;
			i = devList->GetCount();
			while (i-- > 0)
			{
				dev = devList->GetItem(i);
				dev->mut->LockWrite();
				recList->AddAll(dev->yesterdayRecs->GetValues());
				dev->yesterdayRecs->Clear();
				dev->yesterdayRecs->PutAll(dev->todayRecs);
				dev->todayRecs->Clear();
				j = recList->GetCount();
				if (j > 0)
				{
					dev->valUpdated = true;
				}
				dev->mut->UnlockWrite();

				while (j-- > 0)
				{
					MemFree(recList->GetItem(j));
				}
				recList->Clear();
			}
			me->dateMut->UnlockWrite();
			devList->Clear();
		}
		me->checkEvt->Wait(1000);
	}
	DEL_CLASS(recList);
	DEL_CLASS(devList);
	DEL_CLASS(lastStoreTime);
	DEL_CLASS(currTime);
	me->checkRunning = false;
	return 0;
}

void __stdcall SSWR::SMonitor::SMonitorSvrCore::OnDataUDPPacket(const Net::SocketUtil::AddressInfo *addr, UInt16 port, const UInt8 *buff, UOSInt dataSize, void *userData)
{
	SSWR::SMonitor::SMonitorSvrCore *me = (SSWR::SMonitor::SMonitorSvrCore*)userData;
	SSWR::SMonitor::ISMonitorCore::DeviceInfo *devInfo;
	if (dataSize >= 6 && buff[0] == 'S' && buff[1] == 'm')
	{
		UInt8 calcVal[2];
		me->dataCRC->Calc(buff, dataSize - 2, calcVal);
		if (calcVal[0] == (buff[dataSize - 2] ^ 0x12) && calcVal[1] == (buff[dataSize - 1] ^ 0x34))
		{
			switch (ReadUInt16(&buff[2]))
			{
			case 0: //Readings
				if (dataSize >= 42)
				{
					Int32 profileId = ReadInt32(&buff[4]);
					Int64 clientId = ReadInt64(&buff[8]);
					Int64 recTime = ReadInt64(&buff[16]);
					UInt32 digitalVals = ReadUInt32(&buff[24]);
//					Int32 reportInterval = ReadInt32(&buff[28]);
//					Int32 kaInterval = ReadInt32(&buff[32]);
					UInt32 nReading = buff[36];
					UInt32 nDigital = buff[37];
					UInt32 nOutput = buff[38];
					if (dataSize >= 42 + 16 * nReading)
					{
						me->UDPSendReadingRecv(addr, port, recTime);
						devInfo = me->DevGet(clientId, true);
						devInfo->udpAddr = *addr;
						devInfo->udpPort = port;

						me->DeviceRecvReading(devInfo, recTime, nDigital, nReading, nOutput, digitalVals, (ReadingInfo*)&buff[40], profileId, *(UInt32*)addr->addr, port);
					}
				}
				break;
			case 10:
				if (dataSize >= 34)
				{
					Int64 clientId = ReadInt64(&buff[4]);
					DeviceInfo *dev = me->DevGet(clientId, true);
					dev->mut->LockWrite();
					if (dev->photoBuff)
					{
						MemFree(dev->photoBuff);
					}
					if (dev->photoBuffRecv)
					{
						MemFree(dev->photoBuffRecv);
					}
					dev->photoTime = ReadInt64(&buff[12]);
					dev->photoSize = ReadUInt32(&buff[20]);
					dev->photoFmt = ReadInt32(&buff[24]);
					dev->photoPacketSize = ReadUInt32(&buff[28]);
					dev->photoBuff = MemAlloc(UInt8, dev->photoSize);
					dev->photoBuffRecv = MemAlloc(UInt8, (dev->photoSize / dev->photoPacketSize) + 1);
					MemClear(dev->photoBuffRecv, (dev->photoSize / dev->photoPacketSize) + 1);
					dev->photoOfst = 0;
					dev->photoSeq = 0;
					dev->mut->UnlockWrite();
					me->log->LogMessage((const UTF8Char*)"Received photo info", IO::ILogHandler::LOG_LEVEL_RAW);
				}
				break;
			case 12:
				if (dataSize >= 26)
				{
					Int64 clientId = ReadInt64(&buff[4]);
					DeviceInfo *dev = me->DevGet(clientId, false);
					if (dev)
					{
						Int64 photoTime = ReadInt64(&buff[12]);
						UInt32 seq = ReadUInt32(&buff[20]);
						dev->mut->LockWrite();
						if (dev->photoBuff && dev->photoTime == photoTime)
						{
							UOSInt currOfst = seq * dev->photoPacketSize;
							UOSInt currSize = dev->photoPacketSize;
							if (currOfst + currSize > dev->photoSize)
							{
								currSize = dev->photoSize - currOfst;
							}
							if (currOfst < dev->photoSize && currSize == dataSize - 26)
							{
								MemCopyNO(&dev->photoBuff[currOfst], &buff[24], dataSize - 26);
								dev->photoBuffRecv[seq] = 1;
								me->log->LogMessage((const UTF8Char*)"Received photo packet, success", IO::ILogHandler::LOG_LEVEL_RAW);
							}
							else
							{
								me->log->LogMessage((const UTF8Char*)"Received photo packet, size error", IO::ILogHandler::LOG_LEVEL_RAW);
							}
						}
						else
						{
							me->log->LogMessage((const UTF8Char*)"Received photo packet, photo not receiving", IO::ILogHandler::LOG_LEVEL_RAW);
						}
						dev->mut->UnlockWrite();
					}
					else
					{
						me->log->LogMessage((const UTF8Char*)"Received photo packet, device not found", IO::ILogHandler::LOG_LEVEL_RAW);
					}
				}
				break;
			case 14:
				if (dataSize >= 26)
				{
					Int64 clientId = ReadInt64(&buff[4]);
					DeviceInfo *dev = me->DevGet(clientId, false);
					if (dev)
					{
						Bool succ = false;
						Int64 photoTime = ReadInt64(&buff[12]);
						dev->mut->LockWrite();
						if (dev->photoBuff && dev->photoTime == photoTime)
						{
							UOSInt i = dev->photoSize / dev->photoPacketSize;
							if (i * dev->photoPacketSize < dev->photoSize)
							{
								i++;
							}
							succ = true;
							while (i-- > 0)
							{
								if (dev->photoBuffRecv[i] == 0)
								{
									succ = false;
									me->UDPSendPhotoPacket(addr, port, photoTime, (UInt32)i);
								}
							}
							if (succ)
							{
								me->SavePhoto(clientId, dev->photoTime, dev->photoFmt, dev->photoBuff, dev->photoSize);
								MemFree(dev->photoBuff);
								dev->photoBuff = 0;
								MemFree(dev->photoBuffRecv);
								dev->photoBuffRecv = 0;
							}
							succ = true;
						}
						dev->mut->UnlockWrite();
						if (succ)
						{
							me->UDPSendPhotoEnd(addr, port, photoTime);
						}
					}
				}
				break;
			case 16:
				if (dataSize > 14)
				{
					Int64 cliId = ReadInt64(&buff[4]);
					const UTF8Char *name = Text::StrCopyNewC(&buff[12], dataSize - 14);
					me->DeviceSetName(cliId, name);
					Text::StrDelNew(name);
				}
				break;
			case 18:
				if (dataSize > 14)
				{
					Int64 cliId = ReadInt64(&buff[4]);
					const UTF8Char *name = Text::StrCopyNewC(&buff[12], dataSize - 14);
					me->DeviceSetPlatform(cliId, name);
					Text::StrDelNew(name);
				}
				break;
			case 20:
				if (dataSize > 14)
				{
					Int64 cliId = ReadInt64(&buff[4]);
					const UTF8Char *name = Text::StrCopyNewC(&buff[12], dataSize - 14);
					me->DeviceSetCPUName(cliId, name);
					Text::StrDelNew(name);
				}
				break;
			case 22:
				if (dataSize > 22)
				{
					Int64 cliId = ReadInt64(&buff[4]);
					UInt32 index = ReadUInt32(&buff[12]);
					UInt16 sensorId = ReadUInt16(&buff[16]);
					UInt16 readingId = ReadUInt16(&buff[18]);
					const UTF8Char *name = Text::StrCopyNewC(&buff[20], dataSize - 22);
					me->DeviceSetReading(cliId, index, sensorId, readingId, name);
					Text::StrDelNew(name);
				}
				break;
			case 24:
				if (dataSize >= 22)
				{
					Int64 cliId = ReadInt64(&buff[4]);
					Int64 version = ReadInt64(&buff[12]);
					me->DeviceSetVersion(cliId, version);
				}
				break;
			default:
				{
					Text::StringBuilderUTF8 sb;
					sb.Append((const UTF8Char*)"Received unknown packet func ");
					sb.AppendU16(ReadUInt16(&buff[2]));
					sb.Append((const UTF8Char*)", size = ");
					sb.AppendUOSInt(dataSize);
					me->log->LogMessage(sb.ToString(), IO::ILogHandler::LOG_LEVEL_ERROR);
				}
				break;
			}
		}
	}
}

void SSWR::SMonitor::SMonitorSvrCore::DataParsed(IO::Stream *stm, void *stmObj, Int32 cmdType, Int32 seqId, const UInt8 *cmd, UOSInt cmdSize)
{
	DeviceInfo *dev;
	ClientStatus *status = (ClientStatus*)stmObj;
	switch (cmdType)
	{
	case 0:
		if (cmdSize >= 16)
		{
			Data::DateTime dt;
			dt.SetCurrTimeUTC();
			Int64 cliId = ReadInt64(&cmd[0]);
			Int64 cliTime = ReadInt64(&cmd[8]);
			this->devMut->LockRead();
			dev = this->devMap->Get(cliId);
			this->devMut->UnlockRead();
			if (dev)
			{
				if (status->dev != dev)
				{
					if (status->dev)
					{
						status->dev->mut->LockWrite();
						if (status->dev->stm == stm)
						{
							status->dev->stm = 0;
						}
						status->dev->mut->UnlockWrite();
					}

					status->dev = dev;
					status->dev->mut->LockWrite();
					if (status->dev->stm)
					{
						status->dev->stm->Close();
					}
					status->dev->stm = stm;
					status->dev->mut->UnlockWrite();
				}
				status->cliId = cliId;
				dev->lastKATime = dt.ToTicks();
				this->TCPSendLoginReply(stm, cliTime, dev->lastKATime, 0);
			}
			else
			{
				this->TCPSendLoginReply(stm, cliTime, dt.ToTicks(), 1);
			}
		}
		break;
	case 2:
		if (cmdSize >= 10 && cmdSize >= (UOSInt)(10 + cmd[8] + cmd[9]))
		{
			UTF8Char sbuff[256];
			UTF8Char sbuff2[256];
			Int64 cliId = ReadInt64(&cmd[0]);
			Text::StrConcatC(sbuff, (const UTF8Char*)&cmd[10], cmd[8]);
			Text::StrConcatC(sbuff2, (const UTF8Char*)&cmd[10 + cmd[8]], cmd[9]);

			this->devMut->LockRead();
			dev = this->devMap->Get(cliId);
			this->devMut->UnlockRead();
			if (dev == 0)
			{
				status->dev = this->DevAdd(cliId, sbuff, sbuff2);
				status->dev->stm = stm;
				status->cliId = cliId;
			}
		}
		break;
	case 4:
		if (status->dev && cmdSize >= 16)
		{
			if (cmdSize >= (UOSInt)(16 + cmd[12] * 16))
			{
				this->DeviceRecvReading(status->dev, ReadInt64(&cmd[0]), cmd[13], cmd[12], cmd[14], ReadUInt32(&cmd[8]), (ReadingInfo *)&cmd[16], 0, 0, 0);
			}
		}
		break;
	case 6:
		if (cmdSize >= 8)
		{
			Data::DateTime dt;
			dt.SetCurrTimeUTC();
			Int64 cliTime = ReadInt64(&cmd[0]);
			if (status->dev)
			{
				status->dev->lastKATime = dt.ToTicks();
				this->TCPSendKAReply(stm, cliTime, status->dev->lastKATime);
				this->DeviceKARecv(status->dev, status->dev->lastKATime);
			}
		}
		break;
	case 10:
		if (cmdSize >= 16)
		{
			if (status->dev)
			{
				status->dev->mut->LockWrite();
				if (status->dev->photoBuff)
				{
					MemFree(status->dev->photoBuff);
				}
				status->dev->photoTime = ReadInt64(&cmd[0]);
				status->dev->photoSize = ReadUInt32(&cmd[8]);
				status->dev->photoFmt = ReadInt32(&cmd[12]);
				status->dev->photoBuff = MemAlloc(UInt8, status->dev->photoSize);
				status->dev->photoOfst = 0;
				status->dev->photoSeq = 0;
				status->dev->mut->UnlockWrite();
			}
		}
		break;
	case 12:
		if (cmdSize >= 12)
		{
			if (status->dev)
			{
				status->dev->mut->LockWrite();
				if (status->dev->photoBuff && status->dev->photoTime == ReadInt64(&cmd[0]) && status->dev->photoSeq == ReadInt32(&cmd[8]))
				{
					if ((status->dev->photoOfst + cmdSize - 12) <= status->dev->photoSize)
					{
						MemCopyNO(&status->dev->photoBuff[status->dev->photoOfst], &cmd[12], cmdSize - 12);
						status->dev->photoOfst += cmdSize - 12;
						status->dev->photoSeq++;
					}
				}
				status->dev->mut->UnlockWrite();
			}
		}
		break;
	case 14:
		if (cmdSize >= 12)
		{
			if (status->dev)
			{
				Bool succ = false;
				status->dev->mut->LockWrite();
				if (status->dev->photoBuff && status->dev->photoTime == ReadInt64(&cmd[0]) && status->dev->photoOfst == status->dev->photoSize)
				{
					this->SavePhoto(status->cliId, status->dev->photoTime, status->dev->photoFmt, status->dev->photoBuff, status->dev->photoSize);
					MemFree(status->dev->photoBuff);
					status->dev->photoBuff = 0;
					succ = true;
				}
				status->dev->mut->UnlockWrite();
				if (succ)
				{
					this->TCPSendPhotoEnd(stm, ReadInt64(&cmd[0]));
				}
			}
		}
		break;
	case 24:
		if (cmdSize >= 8)
		{
			this->DeviceSetVersion(status->cliId, ReadInt64(&cmd[0]));
		}
		break;
	}
}

void SSWR::SMonitor::SMonitorSvrCore::DataSkipped(IO::Stream *stm, void *stmObj, const UInt8 *buff, UOSInt buffSize)
{
}

void SSWR::SMonitor::SMonitorSvrCore::TCPSendLoginReply(IO::Stream *stm, Int64 cliTime, Int64 svrTime, UInt8 status)
{
	UInt8 cmdBuff[17];
	UInt8 packetBuff[27];
	UOSInt packetSize;
	WriteInt64(&cmdBuff[0], cliTime);
	WriteInt64(&cmdBuff[8], svrTime);
	cmdBuff[16] = status;
	packetSize = this->protoHdlr->BuildPacket(packetBuff, 1, 0, cmdBuff, 17, 0);
	stm->Write(packetBuff, packetSize);
}

void SSWR::SMonitor::SMonitorSvrCore::TCPSendKAReply(IO::Stream *stm, Int64 cliTime, Int64 svrTime)
{
	UInt8 cmdBuff[16];
	UInt8 packetBuff[26];
	UOSInt packetSize;
	WriteInt64(&cmdBuff[0], cliTime);
	WriteInt64(&cmdBuff[8], svrTime);
	packetSize = this->protoHdlr->BuildPacket(packetBuff, 7, 0, cmdBuff, 16, 0);
	stm->Write(packetBuff, packetSize);
}

void SSWR::SMonitor::SMonitorSvrCore::TCPSendCapturePhoto(IO::Stream *stm)
{
	UInt8 packetBuff[10];
	UOSInt packetSize;
	packetSize = this->protoHdlr->BuildPacket(packetBuff, 9, 0, 0, 0, 0);
	stm->Write(packetBuff, packetSize);
}

void SSWR::SMonitor::SMonitorSvrCore::TCPSendPhotoEnd(IO::Stream *stm, Int64 photoTime)
{
	UInt8 cmdBuff[8];
	UInt8 packetBuff[18];
	UOSInt packetSize;
	WriteInt64(&cmdBuff[0], photoTime);
	packetSize = this->protoHdlr->BuildPacket(packetBuff, 15, 0, cmdBuff, 8, 0);
	stm->Write(packetBuff, packetSize);
}

void SSWR::SMonitor::SMonitorSvrCore::TCPSendSetOutput(IO::Stream *stm, UInt32 outputNum, Bool toHigh)
{
	UInt8 cmdBuff[2];
	UInt8 packetBuff[12];
	UOSInt packetSize;
	cmdBuff[0] = (UInt8)outputNum;
	cmdBuff[1] = toHigh?1:0;
	packetSize = this->protoHdlr->BuildPacket(packetBuff, 21, 0, cmdBuff, 2, 0);
	stm->Write(packetBuff, packetSize);
}

void SSWR::SMonitor::SMonitorSvrCore::UDPSendReadingRecv(const Net::SocketUtil::AddressInfo *addr, UInt16 port, Int64 recTime)
{
	UInt8 reply[14];
	UInt8 calcVal[2];
	reply[0] = 'S';
	reply[1] = 'm';
	WriteInt16(&reply[2], 1);
	WriteInt64(&reply[4], recTime);
	this->dataCRC->Calc(reply, 12, calcVal);
	reply[12] = calcVal[0] ^ 0x12;
	reply[13] = calcVal[1] ^ 0x34;
	this->dataUDP->SendTo(addr, port, reply, 14);
}

void SSWR::SMonitor::SMonitorSvrCore::UDPSendCapturePhoto(const Net::SocketUtil::AddressInfo *addr, UInt16 port)
{
	UInt8 reply[6];
	UInt8 calcVal[2];
	reply[0] = 'S';
	reply[1] = 'm';
	WriteInt16(&reply[2], 9);
	this->dataCRC->Calc(reply, 4, calcVal);
	reply[4] = calcVal[0] ^ 0x12;
	reply[5] = calcVal[1] ^ 0x34;
	this->dataUDP->SendTo(addr, port, reply, 6);
}

void SSWR::SMonitor::SMonitorSvrCore::UDPSendPhotoPacket(const Net::SocketUtil::AddressInfo *addr, UInt16 port, Int64 photoTime, UInt32 seq)
{
	UInt8 reply[18];
	UInt8 calcVal[2];
	reply[0] = 'S';
	reply[1] = 'm';
	WriteInt16(&reply[2], 13);
	WriteInt64(&reply[4], photoTime);
	WriteUInt32(&reply[12], seq);
	this->dataCRC->Calc(reply, 16, calcVal);
	reply[16] = calcVal[0] ^ 0x12;
	reply[17] = calcVal[1] ^ 0x34;
	this->dataUDP->SendTo(addr, port, reply, 18);
}

void SSWR::SMonitor::SMonitorSvrCore::UDPSendPhotoEnd(const Net::SocketUtil::AddressInfo *addr, UInt16 port, Int64 photoTime)
{
	UInt8 reply[14];
	UInt8 calcVal[2];
	reply[0] = 'S';
	reply[1] = 'm';
	WriteInt16(&reply[2], 15);
	WriteInt64(&reply[4], photoTime);
	this->dataCRC->Calc(reply, 12, calcVal);
	reply[12] = calcVal[0] ^ 0x12;
	reply[13] = calcVal[1] ^ 0x34;
	this->dataUDP->SendTo(addr, port, reply, 14);
}

void SSWR::SMonitor::SMonitorSvrCore::UDPSendSetOutput(const Net::SocketUtil::AddressInfo *addr, UInt16 port, UInt8 outputNum, Bool isHigh)
{
	UInt8 reply[8];
	UInt8 calcVal[2];
	reply[0] = 'S';
	reply[1] = 'm';
	WriteInt16(&reply[2], 21);
	reply[4] = outputNum;
	reply[5] = isHigh?1:0;
	this->dataCRC->Calc(reply, 6, calcVal);
	reply[6] = calcVal[0] ^ 0x12;
	reply[7] = calcVal[1] ^ 0x34;
	this->dataUDP->SendTo(addr, port, reply, 8);
}

void SSWR::SMonitor::SMonitorSvrCore::SaveDatas()
{
	Data::ArrayList<DeviceInfo*> devList;
	Data::ArrayList<DevRecord2*> recList;
	Data::ArrayList<DevRecord2*> recList2;
	DeviceInfo *dev;
	DevRecord2 *rec;
	Data::DateTime dt;
	IO::FileStream *fs;
	UOSInt i;
	UOSInt j;
	UOSInt k;
	Int32 currYear;
	Int32 currMonth;
	Int32 currDay;
	UTF8Char sbuff[512];
	UTF8Char *sptr;
	UInt8 fsBuff[32];

	this->devMut->LockRead();
	devList.AddAll(this->devMap->GetValues());
	this->devMut->UnlockRead();
	dt.ToUTCTime();
	fs = 0;

	i = devList.GetCount();
	while (i-- > 0)
	{
		dev = devList.GetItem(i);
		dev->mut->LockWrite();
		recList.AddAll(dev->recToStore);
		dev->recToStore->Clear();
		dev->mut->UnlockWrite();

		currYear = 0;
		currMonth = 0;
		currDay = 0;

		j = 0;
		k = recList.GetCount();
		while (j < k)
		{
			rec = recList.GetItem(j);
			dt.SetTicks(rec->recTime);
			if (dt.GetDay() != currDay || dt.GetMonth() != currMonth || dt.GetYear() != currYear)
			{
				SDEL_CLASS(fs);
				currYear = dt.GetYear();
				currMonth = dt.GetMonth();
				currDay = dt.GetDay();

				sptr = Text::StrConcat(sbuff, this->dataDir);
				sptr = dt.ToString(sptr, "yyyyMM");
				*sptr++ = IO::Path::PATH_SEPERATOR;
				sptr = Text::StrInt64(sptr, dev->cliId);
				IO::Path::CreateDirectory(sbuff);
				*sptr++ = IO::Path::PATH_SEPERATOR;
				sptr = dt.ToString(sptr, "yyyyMMdd");
				sptr = Text::StrConcat(sptr, (const UTF8Char*)".rec");

				NEW_CLASS(fs, IO::FileStream(sbuff, IO::FileStream::FileMode::Append, IO::FileStream::FileShare::DenyNone, IO::FileStream::BufferType::Normal));
				if (fs->IsError())
				{
					DEL_CLASS(fs);
					fs = 0;
				}
			}

			if (fs)
			{
				WriteUInt32(&fsBuff[0], rec->digitalVals);
				fsBuff[4] = 2;
				fsBuff[5] = (UInt8)rec->nreading;
				WriteInt16(&fsBuff[6], -1);
				WriteInt64(&fsBuff[8], rec->recTime);
				WriteInt32(&fsBuff[16], 0);
				fsBuff[20] = (UInt8)rec->ndigital;
				fsBuff[21] = 0;
				WriteInt16(&fsBuff[22], -2);
				WriteInt64(&fsBuff[24], rec->recvTime);
				fs->Write(fsBuff, 32);
				if (rec->nreading)
				{
					fs->Write((UInt8*)&rec->readings[0], 16 * rec->nreading);
				}
				MemFree(rec);
			}
			else
			{
				recList2.Add(rec);
			}
			j++;
		}
		recList.Clear();
		SDEL_CLASS(fs);

		if (recList2.GetCount() > 0)
		{
			dev->mut->LockWrite();
			dev->recToStore->AddAll(&recList2);
			dev->mut->UnlockWrite();
			recList2.Clear();
		}
	}
}

void SSWR::SMonitor::SMonitorSvrCore::SavePhoto(Int64 cliId, Int64 photoTime, Int32 photoFmt, UInt8 *photoBuff, UOSInt photoSize)
{
	UTF8Char sbuff[512];
	UTF8Char *sptr;
	Data::DateTime dt;
	IO::FileStream *fs;
	dt.ToUTCTime();
	dt.SetTicks(photoTime);

	sptr = Text::StrConcat(sbuff, this->dataDir);
	sptr = dt.ToString(sptr, "yyyyMM");
	*sptr++ = IO::Path::PATH_SEPERATOR;
	sptr = Text::StrInt64(sptr, cliId);
	*sptr++ = IO::Path::PATH_SEPERATOR;
	sptr = Text::StrConcat(sptr, (const UTF8Char*)"Photo");
	sptr = dt.ToString(sptr, "yyyyMMdd");
	IO::Path::CreateDirectory(sbuff);
	*sptr++ = IO::Path::PATH_SEPERATOR;
	sptr = Text::StrInt64(sptr, photoTime);
	sptr = Text::StrConcat(sptr, (const UTF8Char*)".jpg");
	NEW_CLASS(fs, IO::FileStream(sbuff, IO::FileStream::FileMode::Create, IO::FileStream::FileShare::DenyNone, IO::FileStream::BufferType::Normal));
	fs->Write(photoBuff, photoSize);
	DEL_CLASS(fs);

	/////////////////////////////////////////////
}

void SSWR::SMonitor::SMonitorSvrCore::LoadData()
{
	Text::StringBuilderUTF8 sb;
	Data::DateTime dt;
	Sync::MutexUsage mutUsage;
	DB::DBTool *db = this->UseDB(&mutUsage);
	DB::DBReader *r;
	WebUser *user;
	UTF8Char *sarr[2];
	UOSInt i;
	UOSInt j;
	r = db->ExecuteReader((const UTF8Char*)"select id, userName, pwd, userType from webuser order by id");
	if (r)
	{
		while (r->ReadNext())
		{
			user = MemAlloc(WebUser, 1);
			user->userId = r->GetInt32(0);
			sb.ClearStr();
			r->GetStr(1, &sb);
			user->userName = Text::StrCopyNew(sb.ToString());
			sb.ClearStr();
			r->GetStr(2, &sb);
			sb.Hex2Bytes(user->md5Pwd);
			user->userType = r->GetInt32(3);
			NEW_CLASS(user->mut, Sync::RWMutex());
			NEW_CLASS(user->devMap, Data::Int64Map<DeviceInfo*>());
			this->userMut->LockWrite();
			this->userMap->Put(user->userId, user);
			this->userNameMap->Put(user->userName, user);
			this->userMut->UnlockWrite();
		}
		db->CloseReader(r);
	}

	DeviceInfo *dev;
	r = db->ExecuteReader((const UTF8Char*)"select id, cpuName, platformName, lastKATime, flags, readingTime, nreading, dsensors, ndigital, reading1Status, reading1, reading2Status, reading2, reading3Status, reading3, reading4Status, reading4, reading5Status, reading5, reading6Status, reading6, reading7Status, reading7, reading8Status, reading8, devName, readingNames, digitalNames, reading9Status, reading9, reading10Status, reading10, reading11Status, reading11, reading12Status, reading12, reading13Status, reading13, reading14Status, reading14, reading15Status, reading15, reading16Status, reading16, reading17Status, reading17, reading18Status, reading18, reading19Status, reading19, reading20Status, reading20, reading21Status, reading21, reading22Status, reading22, reading23Status, reading23, reading24Status, reading24, nOutput, reading25Status, reading25, reading26Status, reading26, reading27Status, reading27, reading28Status, reading28, reading29Status, reading29, reading30Status, reading30, reading31Status, reading31, reading32Status, reading32, reading33Status, reading33, reading34Status, reading34, reading35Status, reading35, reading36Status, reading36, reading37Status, reading37, reading38Status, reading38, reading39Status, reading39, reading40Status, reading40, version from device order by id");
	if (r)
	{
		Data::ArrayList<DevRecord2*> recList;
		DevRecord2 *rec;

		while (r->ReadNext())
		{
			dev = MemAlloc(DeviceInfo, 1);
			NEW_CLASS(dev->mut, Sync::RWMutex());
			dev->cliId = r->GetInt64(0);
			sb.ClearStr();
			r->GetStr(1, &sb);
			dev->cpuName = Text::StrCopyNew(sb.ToString());
			sb.ClearStr();
			r->GetStr(2, &sb);
			dev->platformName = Text::StrCopyNew(sb.ToString());
			r->GetDate(3, &dt);
			dev->lastKATime = dt.ToTicks();
			dev->flags = r->GetInt32(4);
			if (r->GetDate(5, &dt))
			{
				dev->readingTime = 0;
			}
			else
			{
				dev->readingTime = dt.ToTicks();
			}
			dev->version = r->GetInt64(93);
			dev->nReading = (UInt32)r->GetInt32(6);
			dev->digitalVals = (UInt32)r->GetInt32(7);
			dev->ndigital = (UInt32)r->GetInt32(8);
			WriteInt64(dev->readings[0].status, r->GetInt64(9));
			dev->readings[0].reading = r->GetDbl(10);
			WriteInt64(dev->readings[1].status, r->GetInt64(11));
			dev->readings[1].reading = r->GetDbl(12);
			WriteInt64(dev->readings[2].status, r->GetInt64(13));
			dev->readings[2].reading = r->GetDbl(14);
			WriteInt64(dev->readings[3].status, r->GetInt64(15));
			dev->readings[3].reading = r->GetDbl(16);
			WriteInt64(dev->readings[4].status, r->GetInt64(17));
			dev->readings[4].reading = r->GetDbl(18);
			WriteInt64(dev->readings[5].status, r->GetInt64(19));
			dev->readings[5].reading = r->GetDbl(20);
			WriteInt64(dev->readings[6].status, r->GetInt64(21));
			dev->readings[6].reading = r->GetDbl(22);
			WriteInt64(dev->readings[7].status, r->GetInt64(23));
			dev->readings[7].reading = r->GetDbl(24);
			sb.ClearStr();
			if (r->GetStr(25, &sb))
			{
				dev->devName = Text::StrCopyNew(sb.ToString());
			}
			else
			{
				dev->devName = 0;
			}
			i = SMONITORCORE_DEVREADINGCNT;
			while (i-- > 0)
			{
				dev->readingNames[i] = 0;
			}
			i = SMONITORCORE_DIGITALCNT;
			while (i-- > 0)
			{
				dev->digitalNames[i] = 0;
			}
			sb.ClearStr();
			if (r->GetStr(26, &sb))
			{
				sarr[1] = sb.ToString();
				i = 0;
				while (true)
				{
					j = Text::StrSplit(sarr, 2, sarr[1], '|');
					dev->readingNames[i] = Text::StrCopyNew(sarr[0]);
					i++;
					if (j != 2)
						break;
				}
			}
			sb.ClearStr();
			if (r->GetStr(27, &sb))
			{
				sarr[1] = sb.ToString();
				i = 0;
				while (true)
				{
					j = Text::StrSplit(sarr, 2, sarr[1], '|');
					dev->digitalNames[i] = Text::StrCopyNew(sarr[0]);
					i++;
					if (j != 2)
						break;
				}
			}
			WriteInt64(dev->readings[8].status, r->GetInt64(28));
			dev->readings[8].reading = r->GetDbl(29);
			WriteInt64(dev->readings[9].status, r->GetInt64(30));
			dev->readings[9].reading = r->GetDbl(31);
			WriteInt64(dev->readings[10].status, r->GetInt64(32));
			dev->readings[10].reading = r->GetDbl(33);
			WriteInt64(dev->readings[11].status, r->GetInt64(34));
			dev->readings[11].reading = r->GetDbl(35);
			WriteInt64(dev->readings[12].status, r->GetInt64(36));
			dev->readings[12].reading = r->GetDbl(37);
			WriteInt64(dev->readings[13].status, r->GetInt64(38));
			dev->readings[13].reading = r->GetDbl(39);
			WriteInt64(dev->readings[14].status, r->GetInt64(40));
			dev->readings[14].reading = r->GetDbl(41);
			WriteInt64(dev->readings[15].status, r->GetInt64(42));
			dev->readings[15].reading = r->GetDbl(43);
			WriteInt64(dev->readings[16].status, r->GetInt64(44));
			dev->readings[16].reading = r->GetDbl(45);
			WriteInt64(dev->readings[17].status, r->GetInt64(46));
			dev->readings[17].reading = r->GetDbl(47);
			WriteInt64(dev->readings[18].status, r->GetInt64(48));
			dev->readings[18].reading = r->GetDbl(49);
			WriteInt64(dev->readings[19].status, r->GetInt64(50));
			dev->readings[19].reading = r->GetDbl(51);
			WriteInt64(dev->readings[20].status, r->GetInt64(52));
			dev->readings[20].reading = r->GetDbl(53);
			WriteInt64(dev->readings[21].status, r->GetInt64(54));
			dev->readings[21].reading = r->GetDbl(55);
			WriteInt64(dev->readings[22].status, r->GetInt64(56));
			dev->readings[22].reading = r->GetDbl(57);
			WriteInt64(dev->readings[23].status, r->GetInt64(58));
			dev->readings[23].reading = r->GetDbl(59);
			dev->nOutput = (UInt32)r->GetInt32(60);
			WriteInt64(dev->readings[24].status, r->GetInt64(61));
			dev->readings[24].reading = r->GetDbl(62);
			WriteInt64(dev->readings[25].status, r->GetInt64(63));
			dev->readings[25].reading = r->GetDbl(64);
			WriteInt64(dev->readings[26].status, r->GetInt64(65));
			dev->readings[26].reading = r->GetDbl(66);
			WriteInt64(dev->readings[27].status, r->GetInt64(67));
			dev->readings[27].reading = r->GetDbl(68);
			WriteInt64(dev->readings[28].status, r->GetInt64(69));
			dev->readings[28].reading = r->GetDbl(70);
			WriteInt64(dev->readings[29].status, r->GetInt64(71));
			dev->readings[29].reading = r->GetDbl(72);
			WriteInt64(dev->readings[30].status, r->GetInt64(73));
			dev->readings[30].reading = r->GetDbl(74);
			WriteInt64(dev->readings[31].status, r->GetInt64(75));
			dev->readings[31].reading = r->GetDbl(76);
			WriteInt64(dev->readings[32].status, r->GetInt64(77));
			dev->readings[32].reading = r->GetDbl(78);
			WriteInt64(dev->readings[33].status, r->GetInt64(79));
			dev->readings[33].reading = r->GetDbl(80);
			WriteInt64(dev->readings[34].status, r->GetInt64(81));
			dev->readings[34].reading = r->GetDbl(82);
			WriteInt64(dev->readings[35].status, r->GetInt64(83));
			dev->readings[35].reading = r->GetDbl(84);
			WriteInt64(dev->readings[36].status, r->GetInt64(85));
			dev->readings[36].reading = r->GetDbl(86);
			WriteInt64(dev->readings[37].status, r->GetInt64(87));
			dev->readings[37].reading = r->GetDbl(88);
			WriteInt64(dev->readings[38].status, r->GetInt64(89));
			dev->readings[38].reading = r->GetDbl(90);
			WriteInt64(dev->readings[39].status, r->GetInt64(91));
			dev->readings[39].reading = r->GetDbl(92);
			NEW_CLASS(dev->recToStore, Data::ArrayList<DevRecord2*>());
			NEW_CLASS(dev->todayRecs, Data::Int64Map<DevRecord2*>());
			NEW_CLASS(dev->yesterdayRecs, Data::Int64Map<DevRecord2*>());
			NEW_CLASS(dev->imgCaches, Data::Int32Map<IO::MemoryStream*>());
			dev->stm = 0;
			dev->photoBuff = 0;
			dev->photoOfst = 0;
			dev->photoSeq = 0;
			dev->photoSize = 0;
			dev->photoTime = 0;
			dev->valUpdated = true;

			this->devMut->LockWrite();
			this->devMap->Put(dev->cliId, dev);
			this->devMut->UnlockWrite();

			this->DeviceQueryRec(dev->cliId, this->currDate, this->currDate + 86400000, &recList);
			i = 0;
			j = recList.GetCount();
			while (i < j)
			{
				rec = recList.GetItem(i);
				rec = dev->todayRecs->Put(rec->recTime, rec);
				if (rec)
				{
					MemFree(rec);
				}
				i++;
			}
			recList.Clear();
			this->DeviceQueryRec(dev->cliId, this->currDate - 86400000, this->currDate, &recList);
			i = 0;
			j = recList.GetCount();
			while (i < j)
			{
				rec = recList.GetItem(i);
				rec = dev->yesterdayRecs->Put(rec->recTime, rec);
				if (rec)
				{
					MemFree(rec);
				}
				i++;
			}
			recList.Clear();
		}
		db->CloseReader(r);
	}

	r = db->ExecuteReader((const UTF8Char*)"select webuser_id, device_id from webuser_device order by webuser_id");
	if (r)
	{
		while (r->ReadNext())
		{
			dev = this->DeviceGet(r->GetInt64(1));
			if (dev)
			{
				this->userMut->LockRead();
				user = this->userMap->Get(r->GetInt32(0));
				this->userMut->UnlockRead();
				if (user)
				{
					user->mut->LockWrite();
					user->devMap->Put(dev->cliId, dev);
					user->mut->UnlockWrite();
				}
			}
		}
		db->CloseReader(r);
	}
}

DB::DBTool *SSWR::SMonitor::SMonitorSvrCore::UseDB(Sync::MutexUsage *mutUsage)
{
	mutUsage->ReplaceMutex(this->dbMut);
	return this->db;
}

void SSWR::SMonitor::SMonitorSvrCore::UserPwdCalc(const UTF8Char *userName, const UTF8Char *pwd, UInt8 *buff)
{
	Crypto::Hash::MD5 md5;
	Text::StringBuilderUTF8 sb;
	sb.Append(userName);
	sb.Append((const UTF8Char*)" pwd ");
	sb.Append(pwd);
	md5.Calc(sb.ToString(), sb.GetLength());
	md5.GetValue(buff);
}

SSWR::SMonitor::SMonitorSvrCore::SMonitorSvrCore(IO::Writer *writer, Media::DrawEngine *deng)
{
	NEW_CLASS(this->sockf, Net::OSSocketFactory(true));
	NEW_CLASS(this->log, IO::LogTool());
	NEW_CLASS(this->protoHdlr, IO::ProtoHdlr::ProtoSMonHandler(this));
	NEW_CLASS(this->parsers, Parser::FullParserList());
	this->deng = deng;
	this->dataDir = 0;
	this->cliSvr = 0;
	this->cliMgr = 0;
	this->dataUDP = 0;
	this->dataCRC = 0;
	this->db = 0;
	this->dbMut = 0;
	this->listener = 0;
	this->webHdlr = 0;
	Data::DateTime dt;
	dt.SetCurrTime();
	dt.ClearTime();
	this->currDate = dt.ToTicks();
	NEW_CLASS(this->dateMut, Sync::RWMutex());
	NEW_CLASS(this->checkEvt, Sync::Event(true, (const UTF8Char*)"SSWR.SMonitor.SMonitorSvrCore.checkEvt"));
	this->checkRunning = false;
	this->checkToStop = false;
	this->initErr = false;

	NEW_CLASS(this->devMut, Sync::RWMutex());
	NEW_CLASS(this->devMap, Data::Int64Map<DeviceInfo*>());
	NEW_CLASS(this->userMut, Sync::RWMutex());
	NEW_CLASS(this->userMap, Data::Int32Map<WebUser*>());
	NEW_CLASS(this->userNameMap, Data::StringUTF8Map<WebUser*>());
	NEW_CLASS(this->uaLog, IO::StringLogger());
	NEW_CLASS(this->refererLog, IO::StringLogger());

	IO::ConfigFile *cfg = IO::IniFile::ParseProgConfig(0);
	const UTF8Char *csptr1;
	const UTF8Char *csptr2;
	const UTF8Char *csptr3;
	UInt16 port;
	Text::StringBuilderUTF8 sb;
	{
		UTF8Char sbuff[512];
		IO::FileStream *fs;
		Text::UTF8Reader *reader;
		IO::Path::GetProcessFileName(sbuff);
		IO::Path::AppendPath(sbuff, (const UTF8Char*)"UserAgent.txt");
		NEW_CLASS(fs, IO::FileStream(sbuff, IO::FileStream::FileMode::ReadOnly, IO::FileStream::FileShare::DenyNone, IO::FileStream::BufferType::Normal));
		NEW_CLASS(reader, Text::UTF8Reader(fs));
		this->uaLog->ReadLogs(reader);
		DEL_CLASS(reader);
		DEL_CLASS(fs);

		IO::Path::GetProcessFileName(sbuff);
		IO::Path::AppendPath(sbuff, (const UTF8Char*)"Referer.txt");
		NEW_CLASS(fs, IO::FileStream(sbuff, IO::FileStream::FileMode::ReadOnly, IO::FileStream::FileShare::DenyNone, IO::FileStream::BufferType::Normal));
		NEW_CLASS(reader, Text::UTF8Reader(fs));
		this->refererLog->ReadLogs(reader);
		DEL_CLASS(reader);
		DEL_CLASS(fs);
	}
	if (cfg == 0)
	{
		writer->WriteLine((const UTF8Char*)"Config file not found");
	}
	else
	{
		csptr1 = cfg->GetValue((const UTF8Char*)"LogDir");
		if (csptr1)
		{
			sb.ClearStr();
			sb.Append(csptr1);
			if (!sb.EndsWith((Char)IO::Path::PATH_SEPERATOR))
			{
				sb.AppendChar(IO::Path::PATH_SEPERATOR, 1);
			}
			sb.Append((const UTF8Char*)"Log");
			this->log->AddFileLog(sb.ToString(), IO::ILogHandler::LOG_TYPE_PER_DAY, IO::ILogHandler::LOG_GROUP_TYPE_PER_MONTH, IO::ILogHandler::LOG_LEVEL_COMMAND, "yyyy-MM-dd HH:mm:ss.fff", false);
			sb.RemoveChars(3);
			sb.Append((const UTF8Char*)"Raw");
			this->log->AddFileLog(sb.ToString(), IO::ILogHandler::LOG_TYPE_PER_DAY, IO::ILogHandler::LOG_GROUP_TYPE_PER_MONTH, IO::ILogHandler::LOG_LEVEL_RAW, "yyyy-MM-dd HH:mm:ss.fff", false);
		}

		csptr1 = cfg->GetValue((const UTF8Char*)"DataDir");
		if (csptr1)
		{
			sb.ClearStr();
			sb.Append(csptr1);
			if (!sb.EndsWith((Char)IO::Path::PATH_SEPERATOR))
			{
				sb.AppendChar(IO::Path::PATH_SEPERATOR, 1);
			}
			this->dataDir = Text::StrCopyNew(sb.ToString());
		}

		csptr1 = cfg->GetValue((const UTF8Char*)"MySQLServer");
		csptr2 = cfg->GetValue((const UTF8Char*)"MySQLDB");
		if (csptr1 && csptr2)
		{
			this->db = DB::MySQLConn::CreateDBTool(this->sockf, csptr1, csptr2, cfg->GetValue((const UTF8Char*)"UID"), cfg->GetValue((const UTF8Char*)"PWD"), log, (const UTF8Char*)"DB: ");
			NEW_CLASS(this->dbMut, Sync::Mutex());
			if (this->db == 0)
			{
				writer->WriteLine((const UTF8Char*)"Error in connecting to mysql database");
			}
			else
			{
				LoadData();
			}
		}
		else
		{
			csptr1 = cfg->GetValue((const UTF8Char*)"DSN");
			csptr2 = cfg->GetValue((const UTF8Char*)"UID");
			csptr3 = cfg->GetValue((const UTF8Char*)"PWD");
			if (csptr1)
			{
				this->db = DB::ODBCConn::CreateDBTool(csptr1, csptr2, csptr3, cfg->GetValue((const UTF8Char*)"Schema"), log, (const UTF8Char*)"DB: ");
				NEW_CLASS(this->dbMut, Sync::Mutex());
				if (this->db == 0)
				{
					writer->WriteLine((const UTF8Char*)"Error in connecting to odbc database");
				}
				else
				{
					LoadData();
				}
			}
			else
			{
				writer->WriteLine((const UTF8Char*)"Config DSN/MySQLServer not found");
			}
		}

		csptr1 = cfg->GetValue((const UTF8Char*)"WebPort");
		csptr2 = cfg->GetValue((const UTF8Char*)"HTTPFiles");
		if (csptr1)
		{
			if (csptr2 && IO::Path::GetPathType(csptr2) == IO::Path::PathType::Directory)
			{
				if (Text::StrToUInt16(csptr1, &port) && port > 0)
				{
					Net::WebServer::HTTPDirectoryHandler *hdlr;
					SSWR::SMonitor::SMonitorWebHandler *shdlr;
					Net::WebServer::HTTPDirectoryHandler *fileshdlr;
					SSWR::Benchmark::BenchmarkWebHandler *benchhdlr;
					NEW_CLASS(hdlr, Net::WebServer::HTTPDirectoryHandler(csptr2, false, 0, false));
					NEW_CLASS(shdlr, SSWR::SMonitor::SMonitorWebHandler(this));
					NEW_CLASS(benchhdlr, SSWR::Benchmark::BenchmarkWebHandler());
					sb.ClearStr();
					IO::Path::GetProcessFileName(&sb);
					IO::Path::AppendPath(&sb, (const UTF8Char*)"files");
					NEW_CLASS(fileshdlr, Net::WebServer::HTTPDirectoryHandler(sb.ToString(), false, 0, false));
					shdlr->HandlePath((const UTF8Char*)"/files", fileshdlr, true);
					hdlr->HandlePath((const UTF8Char*)"/monitor", shdlr, true);
					hdlr->HandlePath((const UTF8Char*)"/benchmark", benchhdlr, true);
					hdlr->ExpandPackageFiles(this->parsers);
					this->webHdlr = hdlr;
					NEW_CLASS(this->listener, Net::WebServer::WebListener(this->sockf, 0, hdlr, port, 60, 4, (const UTF8Char*)"SSWRServer/1.0", false, true));
					if (this->listener->IsError())
					{
						DEL_CLASS(this->listener);
						this->listener = 0;
						this->initErr = true;
						writer->WriteLine((const UTF8Char*)"Error in listening web port");
					}
					else
					{
						this->listener->SetAccessLog(this->log, IO::ILogHandler::LOG_LEVEL_COMMAND);
						this->listener->SetRequestLog(this);
					}
				}
				else
				{
					writer->WriteLine((const UTF8Char*)"Config WebPort invalid");
				}
			}
			else
			{
				writer->WriteLine((const UTF8Char*)"Config HTTPFiles not valid");
			}
		}
		else
		{
			writer->WriteLine((const UTF8Char*)"Config WebPort not found");
		}

		csptr1 = cfg->GetValue((const UTF8Char*)"ClientPort");
		if (csptr1)
		{
			if (Text::StrToUInt16(csptr1, &port) && port > 0)
			{
				NEW_CLASS(this->cliMgr, Net::TCPClientMgr(300, OnClientEvent, OnClientData, this, 4, OnClientTimeout));
				NEW_CLASS(this->cliSvr, Net::TCPServer(this->sockf, port, this->log, OnServerConn, this, (const UTF8Char*)"CLI: "));
				if (this->cliSvr->IsV4Error())
				{
					DEL_CLASS(this->cliSvr);
					this->cliSvr = 0;
					this->initErr = true;
					writer->WriteLine((const UTF8Char*)"Error in listening client port");
				}
			}
			else
			{
				writer->WriteLine((const UTF8Char*)"Config ClientPort invalid");
			}
		}
		else
		{
			writer->WriteLine((const UTF8Char*)"Config ClientPort not found");
		}

		csptr1 = cfg->GetValue((const UTF8Char*)"DataUDPPort");
		if (csptr1)
		{
			if (Text::StrToUInt16(csptr1, &port) && port > 0)
			{
				Crypto::Hash::CRC16 *crc;
				NEW_CLASS(crc, Crypto::Hash::CRC16(Crypto::Hash::CRC16::GetPolynomialCCITT()));
				NEW_CLASS(this->dataCRC, Crypto::Hash::HashCalc(crc));
				NEW_CLASS(this->dataUDP, Net::UDPServer(this->sockf, 0, port, 0, OnDataUDPPacket, this, this->log, (const UTF8Char*)"DUDP: ", 4, false));
				if (this->dataUDP->IsError())
				{
					DEL_CLASS(this->dataUDP);
					this->dataUDP = 0;
					writer->WriteLine((const UTF8Char*)"Error in listening data UDP port");
				}
			}
			else
			{
				writer->WriteLine((const UTF8Char*)"Config DataUDPPort invalid");
			}
		}
		else
		{
			writer->WriteLine((const UTF8Char*)"Config DataUDPPort not found");
		}

		DEL_CLASS(cfg);

		if (!this->IsError())
		{
			Sync::Thread::Create(CheckThread, this);
			while (!this->checkRunning)
			{
				Sync::Thread::Sleep(10);
			}
		}
	}
}

SSWR::SMonitor::SMonitorSvrCore::~SMonitorSvrCore()
{
	SDEL_CLASS(this->cliSvr);
	SDEL_CLASS(this->cliMgr);
	SDEL_CLASS(this->dataUDP);
	SDEL_CLASS(this->listener);
	if (this->webHdlr)
	{
		this->webHdlr->Release();
		this->webHdlr = 0;
	}
	SDEL_CLASS(this->db);
	SDEL_CLASS(this->dbMut);
	this->checkToStop = true;
	this->checkEvt->Set();
	while (this->checkRunning)
	{
		Sync::Thread::Sleep(10);
	}
	this->SaveDatas();

	UOSInt i;
	UOSInt j;
	Data::ArrayList<WebUser*> *userList = this->userMap->GetValues();
	WebUser *user;
	i = userList->GetCount();
	while (i-- > 0)
	{
		user = userList->GetItem(i);
		DEL_CLASS(user->mut);
		DEL_CLASS(user->devMap);
		Text::StrDelNew(user->userName);
		MemFree(user);
	}
	DEL_CLASS(this->userMap);
	DEL_CLASS(this->userNameMap);
	DEL_CLASS(this->userMut);

	Data::ArrayList<DeviceInfo*> *devList = this->devMap->GetValues();
	DeviceInfo *dev;
	DevRecord2 *rec;
	i = devList->GetCount();
	while (i-- > 0)
	{
		dev = devList->GetItem(i);
		SDEL_TEXT(dev->devName);
		SDEL_TEXT(dev->cpuName);
		SDEL_TEXT(dev->platformName);
		j = SMONITORCORE_DEVREADINGCNT;
		while (j-- > 0)
		{
			SDEL_TEXT(dev->readingNames[j]);
		}
		j = SMONITORCORE_DIGITALCNT;
		while (j-- > 0)
		{
			SDEL_TEXT(dev->digitalNames[j]);
		}
		DEL_CLASS(dev->mut);

		j = dev->recToStore->GetCount();
		while (j-- > 0)
		{
			rec = dev->recToStore->GetItem(j);
			MemFree(rec);
		}
		DEL_CLASS(dev->recToStore);

		Data::ArrayList<DevRecord2*> *recList = dev->todayRecs->GetValues();
		j = recList->GetCount();
		while (j-- > 0)
		{
			rec = recList->GetItem(j);
			MemFree(rec);
		}
		DEL_CLASS(dev->todayRecs);

		recList = dev->yesterdayRecs->GetValues();
		j = recList->GetCount();
		while (j-- > 0)
		{
			rec = recList->GetItem(j);
			MemFree(rec);
		}
		DEL_CLASS(dev->yesterdayRecs);

		Data::ArrayList<IO::MemoryStream*> *cacheList = dev->imgCaches->GetValues();
		IO::MemoryStream *mstm;
		j = cacheList->GetCount();
		while (j-- > 0)
		{
			mstm = cacheList->GetItem(j);
			DEL_CLASS(mstm);
		}
		DEL_CLASS(dev->imgCaches);
		if (dev->photoBuff)
		{
			MemFree(dev->photoBuff);
		}
		MemFree(dev);
	}
	DEL_CLASS(this->devMap);
	DEL_CLASS(this->devMut);

	this->UserAgentStore();
	DEL_CLASS(this->uaLog);
	this->RefererStore();
	DEL_CLASS(this->refererLog);

	DEL_CLASS(this->protoHdlr);
	SDEL_CLASS(this->dataCRC);
	DEL_CLASS(this->parsers);
	DEL_CLASS(this->log);
	DEL_CLASS(this->sockf);
	DEL_CLASS(this->checkEvt);
	DEL_CLASS(this->dateMut);
	DEL_CLASS(this->deng);
	SDEL_TEXT(this->dataDir);
}

Bool SSWR::SMonitor::SMonitorSvrCore::IsError()
{
	return this->cliSvr == 0 || this->db == 0 || this->listener == 0 || this->dataDir == 0 || this->initErr;
}

Media::DrawEngine *SSWR::SMonitor::SMonitorSvrCore::GetDrawEngine()
{
	return this->deng;
}

SSWR::SMonitor::SMonitorSvrCore::DeviceInfo *SSWR::SMonitor::SMonitorSvrCore::DevGet(Int64 cliId, Bool toAdd)
{
	DeviceInfo *devInfo = this->DeviceGet(cliId);
	if (devInfo)
		return devInfo;
	if (toAdd)
	{
		return this->DevAdd(cliId, (const UTF8Char*)"UDP Unknown", (const UTF8Char*)"UDP Unknown");
	}
	return 0;
}

SSWR::SMonitor::SMonitorSvrCore::DeviceInfo *SSWR::SMonitor::SMonitorSvrCore::DevAdd(Int64 cliId, const UTF8Char *cpuName, const UTF8Char *platformName)
{
	DeviceInfo *dev;
	this->devMut->LockWrite();
	dev = this->devMap->Get(cliId);
	if (dev)
	{
		this->devMut->UnlockWrite();
		return 0;
	}

	Data::DateTime dt;
	dt.SetCurrTimeUTC();
	Sync::MutexUsage dbMutUsage;
	DB::DBTool *db = this->UseDB(&dbMutUsage);
	DB::SQLBuilder sql(this->db);
	sql.AppendCmd((const UTF8Char*)"insert into device (id, cpuName, platformName, lastKATime, flags) values (");
	sql.AppendInt64(cliId);
	sql.AppendCmd((const UTF8Char*)", ");
	sql.AppendStrUTF8(cpuName);
	sql.AppendCmd((const UTF8Char*)", ");
	sql.AppendStrUTF8(platformName);
	sql.AppendCmd((const UTF8Char*)", ");
	sql.AppendDate(&dt);
	sql.AppendCmd((const UTF8Char*)", ");
	sql.AppendInt32(0);
	sql.AppendCmd((const UTF8Char*)")");
	if (db->ExecuteNonQuery(sql.ToString()) > 0)
	{
		OSInt i;
		dev = MemAlloc(DeviceInfo, 1);
		NEW_CLASS(dev->mut, Sync::RWMutex());
		dev->cliId = cliId;
		dev->cpuName = Text::StrCopyNew(cpuName);
		dev->platformName = Text::StrCopyNew(platformName);
		dev->lastKATime = dt.ToTicks();
		dev->flags = 0;
		dev->version = 0;

		dev->readingTime = 0;
		dev->ndigital = 0;
		dev->digitalVals = 0;
		dev->nReading = 0;
		dev->nOutput = 0;
		dev->devName = 0;
		dev->stm = 0;
		i = SMONITORCORE_DEVREADINGCNT;
		while (i-- > 0)
		{
			dev->readingNames[i] = 0;
		}
		MemClear(dev->readings, sizeof(ReadingInfo) * SMONITORCORE_DEVREADINGCNT);
		i = SMONITORCORE_DIGITALCNT;
		while (i-- > 0)
		{
			dev->digitalNames[i] = 0;
		}
		NEW_CLASS(dev->recToStore, Data::ArrayList<DevRecord2*>());
		NEW_CLASS(dev->todayRecs, Data::Int64Map<DevRecord2*>());
		NEW_CLASS(dev->yesterdayRecs, Data::Int64Map<DevRecord2*>());
		NEW_CLASS(dev->imgCaches, Data::Int32Map<IO::MemoryStream*>());
		dev->photoBuff = 0;
		dev->photoOfst = 0;
		dev->photoSeq = 0;
		dev->photoSize = 0;
		dev->photoTime = 0;
		dev->valUpdated = true;

		this->devMap->Put(dev->cliId, dev);
	}
	dbMutUsage.EndUse();

	this->devMut->UnlockWrite();
	return dev;
}

Bool SSWR::SMonitor::SMonitorSvrCore::DeviceRecvReading(DeviceInfo *dev, Int64 cliTime, UOSInt nDigitals, UOSInt nReading, UOSInt nOutput, UInt32 digitalVals, ReadingInfo *readings, Int32 profileId, UInt32 cliIP, UInt16 port)
{
	UTF8Char sbuff[32];
	UTF8Char *sptr;
	UOSInt i;
	Bool succ = false;
	Data::DateTime dt;
	Int64 t;
	dt.SetCurrTimeUTC();
	t = dt.ToTicks();
	if (cliTime > dev->readingTime && cliTime < t + 300000)
	{
		Sync::MutexUsage dbMutUsage;
		DB::DBTool *db = this->UseDB(&dbMutUsage);
		DB::SQLBuilder sql(this->db);
		if (nReading > SMONITORCORE_DEVREADINGCNT)
		{
			nReading = SMONITORCORE_DEVREADINGCNT;
		}
		sql.AppendCmd((const UTF8Char*)"update device set readingTime = ");
		dt.SetTicks(cliTime);
		sql.AppendDate(&dt);
		sql.AppendCmd((const UTF8Char*)", nreading = ");
		sql.AppendInt32((Int32)nReading);
		sql.AppendCmd((const UTF8Char*)", ndigital = ");
		sql.AppendInt32((Int32)nDigitals);
		sql.AppendCmd((const UTF8Char*)", nOutput = ");
		sql.AppendInt32((Int32)nOutput);
		sql.AppendCmd((const UTF8Char*)", dsensors = ");
		sql.AppendInt32((Int32)digitalVals);
		i = 0;
		while (i < nReading)
		{
			if (ReadInt16(&readings[i].status[6]) != 0)
			{
				sptr = Text::StrUOSInt(Text::StrConcat(sbuff, (const UTF8Char*)"reading"), i + 1);
				sql.AppendCmd((const UTF8Char*)", ");
				sql.AppendCol(sbuff);
				sql.AppendCmd((const UTF8Char*)" = ");
				sql.AppendDbl(readings[i].reading);

				sptr = Text::StrConcat(sptr, (const UTF8Char*)"Status");
				sql.AppendCmd((const UTF8Char*)", ");
				sql.AppendCol(sbuff);
				sql.AppendCmd((const UTF8Char*)" = ");
				sql.AppendInt64(ReadInt64(readings[i].status));
			}

			i++;
		}
		sql.AppendCmd((const UTF8Char*)" where id = ");
		sql.AppendInt64(dev->cliId);
		if (db->ExecuteNonQuery(sql.ToString()) > 0)
		{
			dev->mut->LockWrite();
			dev->readingTime = cliTime;
			dev->ndigital = nDigitals;
			dev->digitalVals = digitalVals;
			dev->nOutput = nOutput;
			dev->nReading = nReading;
			i = 0;
			while (i < nReading)
			{
				if (ReadInt16(&readings[i].status[6]) != 0)
				{
					MemCopyNO(&dev->readings[i], &readings[i], sizeof(ReadingInfo));
				}
				i++;
			}
			dev->valUpdated = true;
			dev->mut->UnlockWrite();
		}
		dbMutUsage.EndUse();

		DevRecord2 *rec;
		rec = MemAlloc(DevRecord2, 1);
		rec->recTime = cliTime;
		rec->recvTime = t;
		rec->profileId = profileId;
		rec->ndigital = nDigitals;
		rec->nreading = nReading;
		rec->nOutput = nOutput;
		rec->digitalVals = digitalVals;
		MemCopyNO(rec->readings, readings, sizeof(ReadingInfo) * nReading);

		this->dateMut->LockRead();
		dev->mut->LockWrite();
		dev->recToStore->Add(rec);

		if (t >= this->currDate - 86400000LL && t < this->currDate)
		{
			DevRecord2 *rec2;
			rec2 = MemAlloc(DevRecord2, 1);
			MemCopyNO(rec2, rec, sizeof(DevRecord2));
			rec2 = dev->yesterdayRecs->Put(rec2->recTime, rec2);
			if (rec2)
			{
				MemFree(rec2);
			}
			dev->valUpdated = true;
		}
		else if (t >= this->currDate && t < this->currDate + 86400000LL)
		{
			DevRecord2 *rec2;
			rec2 = MemAlloc(DevRecord2, 1);
			MemCopyNO(rec2, rec, sizeof(DevRecord2));
			rec2 = dev->todayRecs->Put(rec2->recTime, rec2);
			if (rec2)
			{
				MemFree(rec2);
			}
			dev->valUpdated = true;
		}
		dev->mut->UnlockWrite();
		this->dateMut->UnlockRead();
	}
	return succ;
}

Bool SSWR::SMonitor::SMonitorSvrCore::DeviceKARecv(DeviceInfo *dev, Int64 kaTime)
{
	Data::DateTime dt;
	dt.SetTicks(kaTime);
	Bool succ = false;

	Sync::MutexUsage dbMutUsage;
	DB::DBTool *db = this->UseDB(&dbMutUsage);
	DB::SQLBuilder sql(this->db);
	sql.AppendCmd((const UTF8Char*)"update device set lastKATime = ");
	sql.AppendDate(&dt);
	sql.AppendCmd((const UTF8Char*)" where id = ");
	sql.AppendInt64(dev->cliId);
	if (db->ExecuteNonQuery(sql.ToString()) >= 0)
	{
		succ = true;
	}
	return succ;
}

Bool SSWR::SMonitor::SMonitorSvrCore::DeviceSetName(Int64 cliId, const UTF8Char *devName)
{
	SSWR::SMonitor::ISMonitorCore::DeviceInfo *dev;
	dev = this->DeviceGet(cliId);
	if (dev == 0)
		return false;
	if (devName == 0 || devName[0] == 0)
		return false;
	dev->mut->LockRead();
	if (dev->devName && Text::StrEquals(dev->devName, devName))
	{
		dev->mut->UnlockRead();
		return true;
	}
	dev->mut->UnlockRead();
	Bool succ = false;

	Sync::MutexUsage dbMutUsage;
	DB::DBTool *db = this->UseDB(&dbMutUsage);
	DB::SQLBuilder sql(this->db);
	sql.AppendCmd((const UTF8Char*)"update device set devName = ");
	sql.AppendStrUTF8(devName);
	sql.AppendCmd((const UTF8Char*)" where id = ");
	sql.AppendInt64(dev->cliId);
	if (db->ExecuteNonQuery(sql.ToString()) >= 0)
	{
		dev->mut->LockWrite();
		SDEL_TEXT(dev->devName);
		if (devName)
		{
			dev->devName = Text::StrCopyNew(devName);
		}
		dev->mut->UnlockWrite();
		succ = true;
	}
	return succ;
}

Bool SSWR::SMonitor::SMonitorSvrCore::DeviceSetPlatform(Int64 cliId, const UTF8Char *platformName)
{
	SSWR::SMonitor::ISMonitorCore::DeviceInfo *dev;
	dev = this->DeviceGet(cliId);
	if (dev == 0)
		return false;
	if (platformName == 0 || platformName[0] == 0)
		return false;
	dev->mut->LockRead();
	if (dev->platformName && Text::StrEquals(dev->platformName, platformName))
	{
		dev->mut->UnlockRead();
		return true;
	}
	dev->mut->UnlockRead();
	Bool succ = false;

	Sync::MutexUsage dbMutUsage;
	DB::DBTool *db = this->UseDB(&dbMutUsage);
	DB::SQLBuilder sql(this->db);
	sql.AppendCmd((const UTF8Char*)"update device set platformName = ");
	sql.AppendStrUTF8(platformName);
	sql.AppendCmd((const UTF8Char*)" where id = ");
	sql.AppendInt64(dev->cliId);
	if (db->ExecuteNonQuery(sql.ToString()) >= 0)
	{
		dev->mut->LockWrite();
		SDEL_TEXT(dev->platformName);
		if (platformName)
		{
			dev->platformName = Text::StrCopyNew(platformName);
		}
		dev->mut->UnlockWrite();
		succ = true;
	}
	return succ;
}

Bool SSWR::SMonitor::SMonitorSvrCore::DeviceSetCPUName(Int64 cliId, const UTF8Char *cpuName)
{
	SSWR::SMonitor::ISMonitorCore::DeviceInfo *dev;
	dev = this->DeviceGet(cliId);
	if (dev == 0)
		return false;
	if (cpuName == 0 || cpuName[0] == 0)
		return false;
	dev->mut->LockRead();
	if (dev->cpuName && Text::StrEquals(dev->cpuName, cpuName))
	{
		dev->mut->UnlockRead();
		return true;
	}
	dev->mut->UnlockRead();
	Bool succ = false;

	Sync::MutexUsage dbMutUsage;
	DB::DBTool *db = this->UseDB(&dbMutUsage);
	DB::SQLBuilder sql(this->db);
	sql.AppendCmd((const UTF8Char*)"update device set cpuName = ");
	sql.AppendStrUTF8(cpuName);
	sql.AppendCmd((const UTF8Char*)" where id = ");
	sql.AppendInt64(dev->cliId);
	if (db->ExecuteNonQuery(sql.ToString()) >= 0)
	{
		dev->mut->LockWrite();
		SDEL_TEXT(dev->cpuName);
		if (cpuName)
		{
			dev->cpuName = Text::StrCopyNew(cpuName);
		}
		dev->mut->UnlockWrite();
		succ = true;
	}
	return succ;
}

Bool SSWR::SMonitor::SMonitorSvrCore::DeviceSetReading(Int64 cliId, UInt32 index, UInt16 sensorId, UInt16 readingId, const UTF8Char *readingName)
{
	SSWR::SMonitor::ISMonitorCore::DeviceInfo *dev;
	dev = this->DeviceGet(cliId);
	if (dev == 0)
		return false;
	if (readingName == 0 || readingName[0] == 0)
		return false;
	dev->mut->LockRead();
	if (index < 0 || index >= (OSInt)dev->nReading)
	{
		dev->mut->UnlockRead();
		return false;
	}
	if (ReadUInt16(&dev->readings[index].status[0]) != sensorId || ReadUInt16(&dev->readings[index].status[4]) != readingId)
	{
		dev->mut->UnlockRead();
		return false;
	}
	if (dev->readingNames[index] && Text::StrEquals(dev->readingNames[index], readingName))
	{
		dev->mut->UnlockRead();
		return true;
	}
	UOSInt i = 0;
	Text::StringBuilderUTF8 sb;
	while (i < dev->nReading)
	{
		if (i > 0)
		{
			sb.AppendChar('|', 1);
		}
		if (i == index)
		{
			sb.Append(readingName);
		}
		else if (dev->readingNames[i])
		{
			sb.Append(dev->readingNames[i]);
		}
		
		i++;
	}
	dev->mut->UnlockRead();
	Bool succ = false;

	Sync::MutexUsage dbMutUsage;
	DB::DBTool *db = this->UseDB(&dbMutUsage);
	DB::SQLBuilder sql(this->db);
	sql.AppendCmd((const UTF8Char*)"update device set readingNames = ");
	sql.AppendStrUTF8(sb.ToString());
	sql.AppendCmd((const UTF8Char*)" where id = ");
	sql.AppendInt64(dev->cliId);
	if (db->ExecuteNonQuery(sql.ToString()) >= 0)
	{
		dev->mut->LockWrite();
		SDEL_TEXT(dev->readingNames[index]);
		dev->readingNames[index] = Text::StrCopyNew(readingName);
		dev->valUpdated = true;
		dev->mut->UnlockWrite();
		succ = true;
	}
	return succ;
}

Bool SSWR::SMonitor::SMonitorSvrCore::DeviceSetVersion(Int64 cliId, Int64 version)
{
	SSWR::SMonitor::ISMonitorCore::DeviceInfo *dev;
	dev = this->DeviceGet(cliId);
	if (dev == 0)
		return false;
	if (version <= dev->version)
		return true;
	Bool succ = false;

	Sync::MutexUsage dbMutUsage;
	DB::DBTool *db = this->UseDB(&dbMutUsage);
	DB::SQLBuilder sql(this->db);
	sql.AppendCmd((const UTF8Char*)"update device set version = ");
	sql.AppendInt64(version);
	sql.AppendCmd((const UTF8Char*)" where id = ");
	sql.AppendInt64(dev->cliId);
	if (db->ExecuteNonQuery(sql.ToString()) >= 0)
	{
		dev->mut->LockWrite();
		dev->version = version;
		dev->mut->UnlockWrite();
		succ = true;
	}
	return succ;
}

SSWR::SMonitor::ISMonitorCore::DeviceInfo *SSWR::SMonitor::SMonitorSvrCore::DeviceGet(Int64 cliId)
{
	SSWR::SMonitor::ISMonitorCore::DeviceInfo *dev;
	this->devMut->LockRead();
	dev = this->devMap->Get(cliId);
	this->devMut->UnlockRead();
	return dev;
}

Bool SSWR::SMonitor::SMonitorSvrCore::DeviceModify(Int64 cliId, const UTF8Char *devName, Int32 flags)
{
	DeviceInfo *dev = DeviceGet(cliId);
	if (dev == 0)
		return false;
	if (devName && devName[0] == 0)
	{
		devName = 0;
	}
	Bool succ = false;

	Sync::MutexUsage dbMutUsage;
	DB::DBTool *db = this->UseDB(&dbMutUsage);
	DB::SQLBuilder sql(this->db);
	sql.AppendCmd((const UTF8Char*)"update device set devName = ");
	sql.AppendStrUTF8(devName);
	sql.AppendCmd((const UTF8Char*)", flags = ");
	sql.AppendInt32(flags);
	sql.AppendCmd((const UTF8Char*)" where id = ");
	sql.AppendInt64(dev->cliId);
	if (db->ExecuteNonQuery(sql.ToString()) >= 0)
	{
		dev->mut->LockWrite();
		SDEL_TEXT(dev->devName);
		if (devName)
		{
			dev->devName = Text::StrCopyNew(devName);
		}
		dev->flags = flags;
		dev->mut->UnlockWrite();
		succ = true;
	}
	return succ;
}

Bool SSWR::SMonitor::SMonitorSvrCore::DeviceSetReadings(DeviceInfo *dev, const UTF8Char *readings)
{
	if (dev == 0)
		return false;
	if (readings && readings[0] == 0)
	{
		readings = 0;
	}
	Bool succ = false;

	Sync::MutexUsage dbMutUsage;
	DB::DBTool *db = this->UseDB(&dbMutUsage);
	DB::SQLBuilder sql(this->db);
	sql.AppendCmd((const UTF8Char*)"update device set readingNames = ");
	sql.AppendStrUTF8(readings);
	sql.AppendCmd((const UTF8Char*)" where id = ");
	sql.AppendInt64(dev->cliId);
	if (db->ExecuteNonQuery(sql.ToString()) >= 0)
	{
		UOSInt i;
		UOSInt j;
		dev->mut->LockWrite();
		i = SMONITORCORE_DEVREADINGCNT;
		while (i-- > 0)
		{
			SDEL_TEXT(dev->readingNames[i]);
		}
		if (readings)
		{
			Text::StringBuilderUTF8 sb;
			UTF8Char *sarr[2];
			sb.Append(readings);
			sarr[1] = sb.ToString();
			i = 0;
			while (true)
			{
				j = Text::StrSplit(sarr, 2, sarr[1], '|');
				if (sarr[0][0])
				{
					dev->readingNames[i] = Text::StrCopyNew(sarr[0]);
				}
				if (j != 2)
					break;
				i++;
			}
		}
		dev->valUpdated = true;
		dev->mut->UnlockWrite();
		succ = true;
	}
	return succ;
}

Bool SSWR::SMonitor::SMonitorSvrCore::DeviceSetDigitals(DeviceInfo *dev, const UTF8Char *digitals)
{
	if (dev == 0)
		return false;
	if (digitals && digitals[0] == 0)
	{
		digitals = 0;
	}
	Bool succ = false;

	Sync::MutexUsage dbMutUsage;
	DB::DBTool *db = this->UseDB(&dbMutUsage);
	DB::SQLBuilder sql(this->db);
	sql.AppendCmd((const UTF8Char*)"update device set digitalNames = ");
	sql.AppendStrUTF8(digitals);
	sql.AppendCmd((const UTF8Char*)" where id = ");
	sql.AppendInt64(dev->cliId);
	if (db->ExecuteNonQuery(sql.ToString()) >= 0)
	{
		UOSInt i;
		UOSInt j;
		dev->mut->LockWrite();
		i = SMONITORCORE_DIGITALCNT;
		while (i-- > 0)
		{
			SDEL_TEXT(dev->digitalNames[i]);
		}
		if (digitals)
		{
			Text::StringBuilderUTF8 sb;
			UTF8Char *sarr[2];
			sb.Append(digitals);
			sarr[1] = sb.ToString();
			i = 0;
			while (true)
			{
				j = Text::StrSplit(sarr, 2, sarr[1], '|');
				if (sarr[0][0])
				{
					dev->digitalNames[i] = Text::StrCopyNew(sarr[0]);
				}
				if (j != 2)
					break;
				i++;
			}
		}
		dev->mut->UnlockWrite();
		succ = true;
	}
	return succ;
}

UOSInt SSWR::SMonitor::SMonitorSvrCore::DeviceQueryRec(Int64 cliId, Int64 startTime, Int64 endTime, Data::ArrayList<DevRecord2*> *recList)
{
	UOSInt ret = 0;
	Int64 t;
	Int64 currTime;
	UTF8Char sbuff[512];
	UTF8Char *sptr;
	IO::FileStream *fs;
	UInt8 *fileBuff;
	UOSInt fileBuffSize;
	UOSInt i;
	Data::DateTime dt;
	DevRecord2 *rec;

	fileBuff = MemAlloc(UInt8, 4096);
	dt.ToUTCTime();
	dt.SetTicks(startTime);
	dt.ClearTime();
	currTime = dt.ToTicks();
	while (currTime < endTime)
	{
		sptr = Text::StrConcat(sbuff, this->dataDir);
		sptr = dt.ToString(sptr, "yyyyMM");
		*sptr++ = IO::Path::PATH_SEPERATOR;
		sptr = Text::StrInt64(sptr, cliId);
		*sptr++ = IO::Path::PATH_SEPERATOR;
		sptr = dt.ToString(sptr, "yyyyMMdd");
		sptr = Text::StrConcat(sptr, (const UTF8Char*)".rec");

		NEW_CLASS(fs, IO::FileStream(sbuff, IO::FileStream::FileMode::ReadOnly, IO::FileStream::FileShare::DenyNone, IO::FileStream::BufferType::Normal));
		if (!fs->IsError())
		{
			fileBuffSize = 0;
			while (true)
			{
				i = fs->Read(&fileBuff[fileBuffSize], 4096 - fileBuffSize);
				if (i <= 0)
				{
					break;
				}
				fileBuffSize += i;

				i = 0;
				while (i <= fileBuffSize - 32)
				{
					if (ReadInt16(&fileBuff[i + 6]) == -1)
					{
						if (fileBuff[i + 4] < 2)
						{
							i += 16;
						}
						else
						{
							if (i + (fileBuff[i + 4] + (UOSInt)fileBuff[i + 5]) * 16 <= fileBuffSize)
							{
								t = ReadInt64(&fileBuff[i + 8]);
								if (t >= startTime && t < endTime)
								{
									rec = MemAlloc(DevRecord2, 1);
									rec->recTime = t;
									rec->recvTime = ReadInt64(&fileBuff[i + 24]);
									rec->profileId = 0;
									rec->digitalVals = ReadUInt32(&fileBuff[i]);
									rec->ndigital = fileBuff[i + 20];
									rec->nreading = fileBuff[i + 5];
									rec->nOutput = 0;
									if (rec->nreading > 0)
									{
										MemCopyNO(rec->readings, &fileBuff[i + 16 * (UOSInt)fileBuff[i + 4]], rec->nreading * 16);
									}
									recList->Add(rec);
									ret++;
								}
								i += (fileBuff[i + 4] + (UOSInt)fileBuff[i + 5]) * 16;
							}
							else
							{
								break;
							}
						}
					}
					else
					{
						i+= 16;
					}
				}
				if (i == 0)
				{
					fileBuffSize = 0;
				}
				else if (i >= fileBuffSize)
				{
					fileBuffSize = 0;
				}
				else
				{
					MemCopyO(fileBuff, &fileBuff[i], fileBuffSize - i);
					fileBuffSize -= i;
				}
			}
		}
		DEL_CLASS(fs);
		currTime += 86400000;
		dt.AddDay(1);
	}
	MemFree(fileBuff);
	return ret;
}

Bool SSWR::SMonitor::SMonitorSvrCore::DeviceSetOutput(Int64 cliId, UInt32 outputNum, Bool toHigh)
{
	DeviceInfo *dev;
	Bool succ = false;
	this->devMut->LockRead();
	dev = this->devMap->Get(cliId);
	this->devMut->UnlockRead();
	if (dev)
	{
		dev->mut->LockRead();
		if (dev->stm)
		{
			this->TCPSendSetOutput(dev->stm, outputNum, toHigh);
			succ = true;
		}
		dev->mut->UnlockRead();
	}
	return succ;
}

Bool SSWR::SMonitor::SMonitorSvrCore::UserExist()
{
	return this->userMap->GetCount() > 0;
}

Bool SSWR::SMonitor::SMonitorSvrCore::UserAdd(const UTF8Char *userName, const UTF8Char *password, Int32 userType)
{
	WebUser *user;
	Bool succ = false;
	this->userMut->LockWrite();
	if (this->userNameMap->Get(userName))
	{
		this->userMut->UnlockWrite();
		return false;
	}
	UTF8Char sbuff[64];
	UInt8 pwdBuff[16];
	this->UserPwdCalc(userName, password, pwdBuff);
	Text::StrHexBytes(sbuff, pwdBuff, 16, 0);

	Sync::MutexUsage dbMutUsage;
	DB::DBTool *db = this->UseDB(&dbMutUsage);
	DB::SQLBuilder sql(this->db);
	sql.AppendCmd((const UTF8Char*)"insert into webuser (userName, pwd, userType) values (");
	sql.AppendStrUTF8(userName);
	sql.AppendCmd((const UTF8Char*)", ");
	sql.AppendStrUTF8(sbuff);
	sql.AppendCmd((const UTF8Char*)", ");
	sql.AppendInt32(userType);
	sql.AppendCmd((const UTF8Char*)")");
	if (db->ExecuteNonQuery(sql.ToString()) > 0)
	{
		user = MemAlloc(WebUser, 1);
		user->userId = db->GetLastIdentity32();
		user->userName = Text::StrCopyNew(userName);
		MemCopyNO(user->md5Pwd, pwdBuff, 16);
		user->userType = userType;
		NEW_CLASS(user->mut, Sync::RWMutex());
		NEW_CLASS(user->devMap, Data::Int64Map<DeviceInfo*>());

		this->userMap->Put(user->userId, user);
		this->userNameMap->Put(user->userName, user);
		succ = true;
	}
	dbMutUsage.EndUse();

	this->userMut->UnlockWrite();
	return succ;
}

Bool SSWR::SMonitor::SMonitorSvrCore::UserSetPassword(Int32 userId, const UTF8Char *password)
{
	WebUser *user;
	Bool succ = false;
	this->userMut->LockRead();
	user = this->userMap->Get(userId);
	this->userMut->UnlockRead();
	if (user == 0)
	{
		return false;
	}
	UTF8Char sbuff[64];
	UInt8 pwdBuff[16];
	user->mut->LockRead();
	this->UserPwdCalc(user->userName, password, pwdBuff);
	Text::StrHexBytes(sbuff, pwdBuff, 16, 0);
	user->mut->UnlockRead();

	Sync::MutexUsage dbMutUsage;
	DB::DBTool *db = this->UseDB(&dbMutUsage);
	DB::SQLBuilder sql(this->db);
	sql.AppendCmd((const UTF8Char*)"update webuser set pwd = ");
	sql.AppendStrUTF8(sbuff);
	sql.AppendCmd((const UTF8Char*)" where id = ");
	sql.AppendInt32(userId);
	if (db->ExecuteNonQuery(sql.ToString()) >= 0)
	{
		user->mut->LockWrite();
		MemCopyNO(user->md5Pwd, pwdBuff, 16);
		user->mut->UnlockWrite();
		succ = true;
	}
	return succ;
}

SSWR::SMonitor::ISMonitorCore::LoginInfo *SSWR::SMonitor::SMonitorSvrCore::UserLogin(const UTF8Char *userName, const UTF8Char *password)
{
	WebUser *user;
	SSWR::SMonitor::ISMonitorCore::LoginInfo *login = 0;
	this->userMut->LockRead();
	user = this->userNameMap->Get(userName);
	if (user)
	{
		Bool eq = true;
		OSInt i;
		UInt8 pwdBuff[16];
		this->UserPwdCalc(userName, password, pwdBuff);
		user->mut->LockRead();
		i = 16;
		while (i-- > 0)
		{
			if (user->md5Pwd[i] != pwdBuff[i])
			{
				eq = false;
				break;
			}
		}
		if (eq)
		{
			login = MemAlloc(SSWR::SMonitor::ISMonitorCore::LoginInfo, 1);
			login->userId = user->userId;
			login->loginId = -1;
			login->userType = user->userType;
		}
		user->mut->UnlockRead();
	}
	this->userMut->UnlockRead();
	return login;
}

void SSWR::SMonitor::SMonitorSvrCore::UserFreeLogin(LoginInfo *login)
{
	MemFree(login);
}

UOSInt SSWR::SMonitor::SMonitorSvrCore::UserGetDevices(Int32 userId, Int32 userType, Data::ArrayList<DeviceInfo*> *devList)
{
	UOSInt retCnt;
	Data::ArrayList<DeviceInfo *> *devs;
	DeviceInfo *dev;
	UOSInt i;
	UOSInt j;
	if (userType == 0)
	{
		retCnt = 0;
		this->devMut->LockRead();
		devs = this->devMap->GetValues();
		i = 0;
		j = devs->GetCount();
		while (i < j)
		{
			dev = devs->GetItem(i);
			if ((dev->flags & 3) == 1)
			{
				devList->Add(dev);
				retCnt++;
			}
			i++;
		}
		this->devMut->UnlockRead();
	}
	else if (userType == 1)
	{
		retCnt = 0;
		this->devMut->LockRead();
		devs = this->devMap->GetValues();
		i = 0;
		j = devs->GetCount();
		while (i < j)
		{
			dev = devs->GetItem(i);
			if ((dev->flags & 2) == 0)
			{
				devList->Add(dev);
				retCnt++;
			}
			i++;
		}
		this->devMut->UnlockRead();
	}
	else
	{
		WebUser *user;
		this->userMut->LockRead();
		user = this->userMap->Get(userId);
		this->userMut->UnlockRead();
		if (user)
		{
			retCnt = 0;
			user->mut->LockRead();
			devs = user->devMap->GetValues();
			i = 0;
			j = devs->GetCount();
			while (i < j)
			{
				dev = devs->GetItem(i);
				if ((dev->flags & 2) == 0)
				{
					devList->Add(dev);
					retCnt++;
				}
				i++;
			}
			user->mut->UnlockRead();
		}
		else
		{
			retCnt = 0;
		}
	}
	return retCnt;
}

Bool SSWR::SMonitor::SMonitorSvrCore::UserHasDevice(Int32 userId, Int32 userType, Int64 cliId)
{
	Bool ret;
	DeviceInfo *dev;
	if (userType == 0)
	{
		this->devMut->LockRead();
		dev = this->devMap->Get(cliId);
		ret = dev && (dev->flags & 1) != 0;
		this->devMut->UnlockRead();
	}
	else if (userType == 1)
	{
		this->devMut->LockRead();
		dev = this->devMap->Get(cliId);
		ret = dev != 0;
		this->devMut->UnlockRead();
	}
	else
	{
		WebUser *user;
		this->userMut->LockRead();
		user = this->userMap->Get(userId);
		this->userMut->UnlockRead();
		if (user)
		{
			user->mut->LockRead();
			dev = user->devMap->Get(cliId);
			ret = dev != 0;
			user->mut->UnlockRead();
		}
		else
		{
			ret = false;
		}
	}
	return ret;
}

UOSInt SSWR::SMonitor::SMonitorSvrCore::UserGetList(Data::ArrayList<WebUser*> *userList)
{
	UOSInt ret = userList->GetCount();
	this->userMut->LockRead();
	userList->AddAll(this->userMap->GetValues());
	this->userMut->UnlockRead();
	return userList->GetCount() - ret;
}

SSWR::SMonitor::ISMonitorCore::WebUser *SSWR::SMonitor::SMonitorSvrCore::UserGet(Int32 userId)
{
	WebUser *user;
	this->userMut->LockRead();
	user = this->userMap->Get(userId);
	this->userMut->UnlockRead();
	return user;
}

Bool SSWR::SMonitor::SMonitorSvrCore::UserAssign(Int32 userId, Data::ArrayList<Int64> *devIdList)
{
	Bool valid;

	WebUser *user;
	Bool succ = false;
	this->userMut->LockRead();
	user = this->userMap->Get(userId);
	this->userMut->UnlockRead();
	if (user == 0)
	{
		return false;
	}
	Int64 cliId;
	UOSInt i;
	UOSInt j;
	i = devIdList->GetCount();
	valid = true;
	this->devMut->LockRead();
	while (i-- > 0)
	{
		if (this->devMap->Get(devIdList->GetItem(i)) == 0)
		{
			valid = false;
			break;
		}
	}
	this->devMut->UnlockRead();
	if (!valid)
	{
		return false;
	}

	Sync::MutexUsage dbMutUsage;
	DB::DBTool *db = this->UseDB(&dbMutUsage);
	DB::SQLBuilder sql(this->db);
	sql.AppendCmd((const UTF8Char*)"delete from webuser_device where webuser_id = ");
	sql.AppendInt32(user->userId);
	if (db->ExecuteNonQuery(sql.ToString()) < 0)
	{
		return false;
	}
	succ = true;
	user->mut->LockWrite();
	user->devMap->Clear();
	i = 0;
	j = devIdList->GetCount();
	while (i < j)
	{
		sql.Clear();
		cliId = devIdList->GetItem(i);
		sql.AppendCmd((const UTF8Char*)"insert into webuser_device (webuser_id, device_id) values (");
		sql.AppendInt32(user->userId);
		sql.AppendCmd((const UTF8Char*)", ");
		sql.AppendInt64(cliId);
		sql.AppendCmd((const UTF8Char*)")");
		if (db->ExecuteNonQuery(sql.ToString()) < 0)
		{
			succ = false;
			break;
		}
		else
		{
			this->devMut->LockRead();
			user->devMap->Put(cliId, this->devMap->Get(cliId));
			this->devMut->UnlockRead();
		}

		i++;
	}
	user->mut->UnlockWrite();
	return succ;
}

Bool SSWR::SMonitor::SMonitorSvrCore::SendCapturePhoto(Int64 cliId)
{
	DeviceInfo *dev;
	Bool succ = false;
	this->devMut->LockRead();
	dev = this->devMap->Get(cliId);
	this->devMut->UnlockRead();
	if (dev)
	{
		dev->mut->LockRead();
		if (dev->stm)
		{
			this->TCPSendCapturePhoto(dev->stm);
			succ = true;
		}
		dev->mut->UnlockRead();
	}
	return succ;
}

void SSWR::SMonitor::SMonitorSvrCore::LogRequest(Net::WebServer::IWebRequest *req)
{
	Text::StringBuilderUTF8 sb;
	if (req->GetHeader(&sb, (const UTF8Char*)"User-Agent"))
	{
		this->UserAgentLog(sb.ToString());
	}

	sb.ClearStr();
	if (req->GetHeader(&sb, (const UTF8Char*)"Referer"))
	{
		this->RefererLog(sb.ToString());
	}
}

void SSWR::SMonitor::SMonitorSvrCore::UserAgentLog(const UTF8Char *userAgent)
{
	this->uaLog->LogStr(userAgent);
}

void SSWR::SMonitor::SMonitorSvrCore::UserAgentStore()
{
	if (this->uaLog->IsModified())
	{
		UTF8Char sbuff[512];
		IO::FileStream *fs;
		Text::UTF8Writer *writer;
		IO::Path::GetProcessFileName(sbuff);
		IO::Path::AppendPath(sbuff, (const UTF8Char*)"UserAgent.txt");
		NEW_CLASS(fs, IO::FileStream(sbuff, IO::FileStream::FileMode::Create, IO::FileStream::FileShare::DenyNone, IO::FileStream::BufferType::Normal));
		NEW_CLASS(writer, Text::UTF8Writer(fs));
		this->uaLog->WriteLogs(writer);
		DEL_CLASS(writer);
		DEL_CLASS(fs);

	}
}

void SSWR::SMonitor::SMonitorSvrCore::RefererLog(const UTF8Char *referer)
{
	if (Text::StrStartsWith(referer, (const UTF8Char*)"http://sswroom.no-ip.org"))
	{
		return;
	}
	else if (Text::StrStartsWith(referer, (const UTF8Char*)"http://192.168.0.15"))
	{
		return;
	}
	this->refererLog->LogStr(referer);
}

void SSWR::SMonitor::SMonitorSvrCore::RefererStore()
{
	if (this->refererLog->IsModified())
	{
		UTF8Char sbuff[512];
		IO::FileStream *fs;
		Text::UTF8Writer *writer;
		IO::Path::GetProcessFileName(sbuff);
		IO::Path::AppendPath(sbuff, (const UTF8Char*)"Referer.txt");
		NEW_CLASS(fs, IO::FileStream(sbuff, IO::FileStream::FileMode::Create, IO::FileStream::FileShare::DenyNone, IO::FileStream::BufferType::Normal));
		NEW_CLASS(writer, Text::UTF8Writer(fs));
		this->refererLog->WriteLogs(writer);
		DEL_CLASS(writer);
		DEL_CLASS(fs);

	}
}
