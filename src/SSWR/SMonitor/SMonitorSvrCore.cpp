#include "Stdafx.h"
#include "Crypto/Hash/CRC16.h"
#include "Crypto/Hash/MD5.h"
#include "Data/ByteTool.h"
#include "DB/DBReader.h"
#include "DB/ODBCConn.h"
#include "IO/FileStream.h"
#include "IO/IniFile.h"
#include "IO/LogWriter.h"
#include "IO/MemoryStream.h"
#include "IO/Path.h"
#include "IO/StreamWriter.h"
#include "IO/ProtoHdlr/ProtoSMonHandler.h"
#include "Net/MySQLTCPClient.h"
#include "Net/OSSocketFactory.h"
#include "Net/SSLEngineFactory.h"
#include "Net/Email/SMTPClient.h"
#include "Net/WebServer/HTTPDirectoryHandler.h"
#include "Net/WebServer/NodeModuleHandler.h"
#include "Parser/FullParserList.h"
#include "SSWR/Benchmark/BenchmarkWebHandler.h"
#include "SSWR/SMonitor/SMonitorSvrCore.h"
#include "SSWR/SMonitor/SMonitorWebHandler.h"
#include "SSWR/VAMS/VAMSBTWebHandler.h"
#include "Sync/RWMutexUsage.h"
#include "Text/StringBuilderUTF8.h"
#include "Text/UTF8Reader.h"
#include "Text/UTF8Writer.h"

void __stdcall SSWR::SMonitor::SMonitorSvrCore::OnClientEvent(NN<Net::TCPClient> cli, AnyType userObj, AnyType cliData, Net::TCPClientMgr::TCPEventType evtType)
{
	NN<SSWR::SMonitor::SMonitorSvrCore> me = userObj.GetNN<SSWR::SMonitor::SMonitorSvrCore>();
	UTF8Char sbuff[32];
	UnsafeArray<UTF8Char> sptr;
	NN<DeviceInfo> dev;
	NN<ClientStatus> status = cliData.GetNN<ClientStatus>();
	switch (evtType)
	{
	case Net::TCPClientMgr::TCP_EVENT_DISCONNECT:
		{
			sptr = cli->GetRemoteName(sbuff).Or(sbuff);
			Text::StringBuilderUTF8 sb;
			sb.AppendC(UTF8STRC("CLI: Client disconnected: "));
			sb.AppendP(sbuff, sptr);
			me->log.LogMessage(sb.ToCString(), IO::LogHandler::LogLevel::Action);

			if (status->dev.SetTo(dev))
			{
				Sync::RWMutexUsage mutUsage(dev->mut, true);
				if (dev->stm == cli.Ptr())
				{
					dev->stm = 0;
				}
			}
			MemFree(status->dataBuff);
			me->protoHdlr.DeleteStreamData(cli, status->stmData);
			MemFreeNN(status);
			cli.Delete();
		}
		break;
	case Net::TCPClientMgr::TCP_EVENT_CONNECT:
	case Net::TCPClientMgr::TCP_EVENT_HASDATA:
	case Net::TCPClientMgr::TCP_EVENT_SHUTDOWN:
	default:
		break;
	}
}

void __stdcall SSWR::SMonitor::SMonitorSvrCore::OnClientData(NN<Net::TCPClient> cli, AnyType userObj, AnyType cliData, const Data::ByteArrayR &buff)
{
	NN<SSWR::SMonitor::SMonitorSvrCore> me = userObj.GetNN<SSWR::SMonitor::SMonitorSvrCore>();
	NN<ClientStatus> status = cliData.GetNN<ClientStatus>();
	MemCopyNO(&status->dataBuff[status->dataSize], buff.Arr().Ptr(), buff.GetSize());
	status->dataSize += buff.GetSize();

	UOSInt retSize = me->protoHdlr.ParseProtocol(cli, status, status->stmData, Data::ByteArrayR(status->dataBuff, status->dataSize));
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

void __stdcall SSWR::SMonitor::SMonitorSvrCore::OnClientTimeout(NN<Net::TCPClient> cli, AnyType userObj, AnyType cliData)
{
	NN<SSWR::SMonitor::SMonitorSvrCore> me = userObj.GetNN<SSWR::SMonitor::SMonitorSvrCore>();
	UTF8Char sbuff[32];
	UnsafeArray<UTF8Char> sptr;
	sptr = cli->GetRemoteName(sbuff).Or(sbuff);
	Text::StringBuilderUTF8 sb;
	sb.AppendC(UTF8STRC("CLI: Client process timeout: "));
	sb.AppendP(sbuff, sptr);
	me->log.LogMessage(sb.ToCString(), IO::LogHandler::LogLevel::Action);
}

void __stdcall SSWR::SMonitor::SMonitorSvrCore::OnServerConn(NN<Socket> s, AnyType userObj)
{
	NN<SSWR::SMonitor::SMonitorSvrCore> me = userObj.GetNN<SSWR::SMonitor::SMonitorSvrCore>();
	NN<Net::TCPClient> cli;
	ClientStatus *status;
	NEW_CLASSNN(cli, Net::TCPClient(me->sockf, s));
	status = MemAlloc(ClientStatus, 1);
	status->cliId = 0;
	status->dataBuff = MemAlloc(UInt8, TCP_BUFF_SIZE + 2048);
	status->dataSize = 0;
	status->me = me;
	status->dev = 0;
	status->stmData = me->protoHdlr.CreateStreamData(cli);
	me->cliMgr->AddClient(cli, status);
}

void __stdcall SSWR::SMonitor::SMonitorSvrCore::CheckThread(NN<Sync::Thread> thread)
{
	NN<SSWR::SMonitor::SMonitorSvrCore> me = thread->GetUserObj().GetNN<SSWR::SMonitor::SMonitorSvrCore>();
	NN<DeviceInfo> dev;
	Int64 t;
	UOSInt i;
	UOSInt j;
	Data::DateTime currTime;
	Data::DateTime lastStoreTime;
	Data::ArrayListNN<DeviceInfo> devList;
	Data::ArrayListNN<DevRecord2> recList;
	lastStoreTime.SetCurrTimeUTC();
	while (!thread->IsStopping())
	{
		currTime.SetCurrTimeUTC();
		if (currTime.DiffMS(lastStoreTime) >= 300000)
		{
			lastStoreTime.SetValue(currTime);
			me->SaveDatas();
			
			if (me->uaLog.IsModified())
			{
				me->UserAgentStore();
			}

			if (me->refererLog.IsModified())
			{
				me->RefererStore();
			}
		}
		t = currTime.ToTicks();
		if (t >= me->currDate + 86400000)
		{
			Sync::RWMutexUsage mutUsage(me->devMut, false);
			devList.AddAll(me->devMap);
			mutUsage.EndUse();

			mutUsage.ReplaceMutex(me->dateMut, true);
			me->currDate += 86400000;
			i = devList.GetCount();
			while (i-- > 0)
			{
				dev = devList.GetItemNoCheck(i);
				Sync::RWMutexUsage mutUsage(dev->mut, true);
				recList.AddAll(dev->yesterdayRecs);
				dev->yesterdayRecs.Clear();
				dev->yesterdayRecs.PutAll(dev->todayRecs);
				dev->todayRecs.Clear();
				j = recList.GetCount();
				if (j > 0)
				{
					dev->valUpdated = true;
				}
				mutUsage.EndUse();

				while (j-- > 0)
				{
					MemFreeNN(recList.GetItemNoCheck(j));
				}
				recList.Clear();
			}
			mutUsage.EndUse();
			devList.Clear();
		}
		thread->Wait(1000);
	}
}

void __stdcall SSWR::SMonitor::SMonitorSvrCore::OnDataUDPPacket(NN<const Net::SocketUtil::AddressInfo> addr, UInt16 port, Data::ByteArrayR data, AnyType userData)
{
	NN<SSWR::SMonitor::SMonitorSvrCore> me = userData.GetNN<SSWR::SMonitor::SMonitorSvrCore>();
	NN<SSWR::SMonitor::ISMonitorCore::DeviceInfo> devInfo;
	if (data.GetSize() >= 6 && data[0] == 'S' && data[1] == 'm')
	{
		UInt8 calcVal[2];
		me->dataCRC->Calc(data.Arr(), data.GetSize() - 2, calcVal);
		if (calcVal[0] == (data[data.GetSize() - 2] ^ 0x12) && calcVal[1] == (data[data.GetSize() - 1] ^ 0x34))
		{
			switch (ReadUInt16(&data[2]))
			{
			case 0: //Readings
				if (data.GetSize() >= 42)
				{
					Int32 profileId = ReadInt32(&data[4]);
					Int64 clientId = ReadInt64(&data[8]);
					Int64 recTime = ReadInt64(&data[16]);
					UInt32 digitalVals = ReadUInt32(&data[24]);
//					Int32 reportInterval = ReadInt32(&data[28]);
//					Int32 kaInterval = ReadInt32(&data[32]);
					UInt32 nReading = data[36];
					UInt32 nDigital = data[37];
					UInt32 nOutput = data[38];
					if (data.GetSize() >= 42 + 16 * nReading)
					{
						me->UDPSendReadingRecv(addr, port, recTime);
						if (me->DevGetOrAdd(clientId).SetTo(devInfo))
						{
							devInfo->udpAddr = addr.Ptr()[0];
							devInfo->udpPort = port;

							me->DeviceRecvReading(devInfo, recTime, nDigital, nReading, nOutput, digitalVals, (ReadingInfo*)&data[40], profileId, *(UInt32*)addr->addr, port);
						}
					}
				}
				break;
			case 10:
				if (data.GetSize() >= 34)
				{
					Int64 clientId = ReadInt64(&data[4]);
					NN<DeviceInfo> dev;
					if (me->DevGetOrAdd(clientId).SetTo(dev))
					{
						Sync::RWMutexUsage mutUsage(dev->mut, true);
						if (dev->photoBuff)
						{
							MemFree(dev->photoBuff);
						}
						if (dev->photoBuffRecv)
						{
							MemFree(dev->photoBuffRecv);
						}
						dev->photoTime = ReadInt64(&data[12]);
						dev->photoSize = ReadUInt32(&data[20]);
						dev->photoFmt = ReadInt32(&data[24]);
						dev->photoPacketSize = ReadUInt32(&data[28]);
						dev->photoBuff = MemAlloc(UInt8, dev->photoSize);
						dev->photoBuffRecv = MemAlloc(UInt8, (dev->photoSize / dev->photoPacketSize) + 1);
						MemClear(dev->photoBuffRecv, (dev->photoSize / dev->photoPacketSize) + 1);
						dev->photoOfst = 0;
						dev->photoSeq = 0;
						mutUsage.EndUse();
						me->log.LogMessage(CSTR("Received photo info"), IO::LogHandler::LogLevel::Raw);
					}
				}
				break;
			case 12:
				if (data.GetSize() >= 26)
				{
					Int64 clientId = ReadInt64(&data[4]);
					NN<DeviceInfo> dev;
					if (me->DevGet(clientId).SetTo(dev))
					{
						Int64 photoTime = ReadInt64(&data[12]);
						UInt32 seq = ReadUInt32(&data[20]);
						Sync::RWMutexUsage mutUsage(dev->mut, true);
						if (dev->photoBuff && dev->photoTime == photoTime)
						{
							UOSInt currOfst = seq * dev->photoPacketSize;
							UOSInt currSize = dev->photoPacketSize;
							if (currOfst + currSize > dev->photoSize)
							{
								currSize = dev->photoSize - currOfst;
							}
							if (currOfst < dev->photoSize && currSize == data.GetSize() - 26)
							{
								MemCopyNO(&dev->photoBuff[currOfst], &data[24], data.GetSize() - 26);
								dev->photoBuffRecv[seq] = 1;
								me->log.LogMessage(CSTR("Received photo packet, success"), IO::LogHandler::LogLevel::Raw);
							}
							else
							{
								me->log.LogMessage(CSTR("Received photo packet, size error"), IO::LogHandler::LogLevel::Raw);
							}
						}
						else
						{
							me->log.LogMessage(CSTR("Received photo packet, photo not receiving"), IO::LogHandler::LogLevel::Raw);
						}
					}
					else
					{
						me->log.LogMessage(CSTR("Received photo packet, device not found"), IO::LogHandler::LogLevel::Raw);
					}
				}
				break;
			case 14:
				if (data.GetSize() >= 26)
				{
					Int64 clientId = ReadInt64(&data[4]);
					NN<DeviceInfo> dev;
					if (me->DevGet(clientId).SetTo(dev))
					{
						Bool succ = false;
						Int64 photoTime = ReadInt64(&data[12]);
						Sync::RWMutexUsage mutUsage(dev->mut, true);
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
						mutUsage.EndUse();
						if (succ)
						{
							me->UDPSendPhotoEnd(addr, port, photoTime);
						}
					}
				}
				break;
			case 16:
				if (data.GetSize() > 14)
				{
					Int64 cliId = ReadInt64(&data[4]);
					NN<Text::String> name = Text::String::New(&data[12], data.GetSize() - 14);
					me->DeviceSetName(cliId, name);
					name->Release();
				}
				break;
			case 18:
				if (data.GetSize() > 14)
				{
					Int64 cliId = ReadInt64(&data[4]);
					NN<Text::String> name = Text::String::New(&data[12], data.GetSize() - 14);
					me->DeviceSetPlatform(cliId, name);
					name->Release();
				}
				break;
			case 20:
				if (data.GetSize() > 14)
				{
					Int64 cliId = ReadInt64(&data[4]);
					NN<Text::String> name = Text::String::New(&data[12], data.GetSize() - 14);
					me->DeviceSetCPUName(cliId, name);
					name->Release();
				}
				break;
			case 22:
				if (data.GetSize() > 22)
				{
					Int64 cliId = ReadInt64(&data[4]);
					UInt32 index = ReadUInt32(&data[12]);
					UInt16 sensorId = ReadUInt16(&data[16]);
					UInt16 readingId = ReadUInt16(&data[18]);
					UnsafeArray<const UTF8Char> name = Text::StrCopyNewC(&data[20], data.GetSize() - 22).Ptr();
					me->DeviceSetReading(cliId, index, sensorId, readingId, name);
					Text::StrDelNew(name);
				}
				break;
			case 24:
				if (data.GetSize() >= 22)
				{
					Int64 cliId = ReadInt64(&data[4]);
					Int64 version = ReadInt64(&data[12]);
					me->DeviceSetVersion(cliId, version);
				}
				break;
			default:
				{
					Text::StringBuilderUTF8 sb;
					sb.AppendC(UTF8STRC("Received unknown packet func "));
					sb.AppendU16(ReadUInt16(&data[2]));
					sb.AppendC(UTF8STRC(", size = "));
					sb.AppendUOSInt(data.GetSize());
					me->log.LogMessage(sb.ToCString(), IO::LogHandler::LogLevel::Error);
				}
				break;
			}
		}
	}
}

void __stdcall SSWR::SMonitor::SMonitorSvrCore::OnNotifyUDPPacket(NN<const Net::SocketUtil::AddressInfo> addr, UInt16 port, Data::ByteArrayR data, AnyType userData)
{
	NN<SSWR::SMonitor::SMonitorSvrCore> me = userData.GetNN<SSWR::SMonitor::SMonitorSvrCore>();
	if (data.GetSize() < 4)
		return;
	if (data[0] == 'S' && data[1] == 'm' && data[2] == 'P' && data[3] == 'M')
	{
		if (data.GetSize() >= 25)
		{
			Text::CStringNN pwd = me->notifyPwd->ToCString();
			UInt32 crcVal;
			{
				Sync::MutexUsage mutUsage(me->notifyCRCMut);
				me->notifyCRC.Clear();
				me->notifyCRC.Calc(pwd.v, pwd.leng);
				me->notifyCRC.Calc(data.Arr(), data.GetSize() - 4);
				crcVal = me->notifyCRC.GetValueU32();
			}
			if (crcVal == ReadMUInt32(&data[data.GetSize() - 4]))
			{
				Data::Timestamp ts = Data::Timestamp(Data::TimeInstant(ReadInt64(&data[4]), ReadUInt32(&data[12])), 0);
				Data::Timestamp currTime = Data::Timestamp::UtcNow();
				Int64 t = currTime.DiffSec(ts);
				if (t >= -180 && t <= 180)
				{
					me->NewNotify(addr, port, ts, data[16], ReadUInt32(&data[17]), Text::CStringNN(&data[21], data.GetSize() - 25));
				}
				else
				{
					me->log.LogMessage(CSTR("Not: Time invalid"), IO::LogHandler::LogLevel::Action);
				}
			}
			else
			{
				me->log.LogMessage(CSTR("Not: CRC invalid"), IO::LogHandler::LogLevel::Action);
			}
		}
	}
}

void SSWR::SMonitor::SMonitorSvrCore::DataParsed(NN<IO::Stream> stm, AnyType stmObj, Int32 cmdType, Int32 seqId, UnsafeArray<const UInt8> cmd, UOSInt cmdSize)
{
	NN<DeviceInfo> dev;
	NN<DeviceInfo> sdev;
	NN<ClientStatus> status = stmObj.GetNN<ClientStatus>();
	switch (cmdType)
	{
	case 0:
		if (cmdSize >= 16)
		{
			Data::DateTime dt;
			dt.SetCurrTimeUTC();
			Int64 cliId = ReadInt64(&cmd[0]);
			Int64 cliTime = ReadInt64(&cmd[8]);
			Sync::RWMutexUsage mutUsage(this->devMut, false);
			if (this->devMap.Get(cliId).SetTo(dev))
			{
				mutUsage.EndUse();
				if (status->dev != dev)
				{
					if (status->dev.SetTo(sdev))
					{
						Sync::RWMutexUsage mutUsage(sdev->mut, true);
						if (sdev->stm == stm.Ptr())
						{
							sdev->stm = 0;
						}
					}

					status->dev = dev;
					Sync::RWMutexUsage mutUsage(dev->mut, true);
					if (dev->stm)
					{
						dev->stm->Close();
					}
					dev->stm = stm.Ptr();
				}
				status->cliId = cliId;
				dev->lastKATime = dt.ToTicks();
				this->TCPSendLoginReply(stm, cliTime, dev->lastKATime, 0);
			}
			else
			{
				mutUsage.EndUse();
				this->TCPSendLoginReply(stm, cliTime, dt.ToTicks(), 1);
			}
		}
		break;
	case 2:
		if (cmdSize >= 10 && cmdSize >= (UOSInt)(10 + cmd[8] + cmd[9]))
		{
			UTF8Char sbuff[256];
			UTF8Char sbuff2[256];
			UnsafeArray<UTF8Char> sbuffEnd;
			UnsafeArray<UTF8Char> sbuff2End;
			Int64 cliId = ReadInt64(&cmd[0]);
			sbuffEnd = Text::StrConcatC(sbuff, (const UTF8Char*)&cmd[10], cmd[8]);
			sbuff2End = Text::StrConcatC(sbuff2, (const UTF8Char*)&cmd[10 + cmd[8]], cmd[9]);

			Sync::RWMutexUsage mutUsage(this->devMut, false);
			if (!this->devMap.Get(cliId).SetTo(dev))
			{
				mutUsage.EndUse();
				if (this->DevAdd(cliId, CSTRP(sbuff, sbuffEnd), CSTRP(sbuff2, sbuff2End)).SetTo(dev))
				{
					status->dev = dev;
					dev->stm = stm.Ptr();
					status->cliId = cliId;
				}
			}
		}
		break;
	case 4:
		if (status->dev.SetTo(dev) && cmdSize >= 16)
		{
			if (cmdSize >= (UOSInt)(16 + cmd[12] * 16))
			{
				this->DeviceRecvReading(dev, ReadInt64(&cmd[0]), cmd[13], cmd[12], cmd[14], ReadUInt32(&cmd[8]), (ReadingInfo *)&cmd[16], 0, 0, 0);
			}
		}
		break;
	case 6:
		if (cmdSize >= 8)
		{
			Data::DateTime dt;
			dt.SetCurrTimeUTC();
			Int64 cliTime = ReadInt64(&cmd[0]);
			if (status->dev.SetTo(dev))
			{
				dev->lastKATime = dt.ToTicks();
				this->TCPSendKAReply(stm, cliTime, dev->lastKATime);
				this->DeviceKARecv(dev, dev->lastKATime);
			}
		}
		break;
	case 10:
		if (cmdSize >= 16)
		{
			if (status->dev.SetTo(dev))
			{
				Sync::RWMutexUsage mutUsage(dev->mut, true);
				if (dev->photoBuff)
				{
					MemFree(dev->photoBuff);
				}
				dev->photoTime = ReadInt64(&cmd[0]);
				dev->photoSize = ReadUInt32(&cmd[8]);
				dev->photoFmt = ReadInt32(&cmd[12]);
				dev->photoBuff = MemAlloc(UInt8, dev->photoSize);
				dev->photoOfst = 0;
				dev->photoSeq = 0;
			}
		}
		break;
	case 12:
		if (cmdSize >= 12)
		{
			if (status->dev.SetTo(dev))
			{
				Sync::RWMutexUsage mutUsage(dev->mut, true);
				if (dev->photoBuff && dev->photoTime == ReadInt64(&cmd[0]) && dev->photoSeq == ReadInt32(&cmd[8]))
				{
					if ((dev->photoOfst + cmdSize - 12) <= dev->photoSize)
					{
						MemCopyNO(&dev->photoBuff[dev->photoOfst], &cmd[12], cmdSize - 12);
						dev->photoOfst += cmdSize - 12;
						dev->photoSeq++;
					}
				}
			}
		}
		break;
	case 14:
		if (cmdSize >= 12)
		{
			if (status->dev.SetTo(dev))
			{
				Bool succ = false;
				Sync::RWMutexUsage mutUsage(dev->mut, true);
				if (dev->photoBuff && dev->photoTime == ReadInt64(&cmd[0]) && dev->photoOfst == dev->photoSize)
				{
					this->SavePhoto(status->cliId, dev->photoTime, dev->photoFmt, dev->photoBuff, dev->photoSize);
					MemFree(dev->photoBuff);
					dev->photoBuff = 0;
					succ = true;
				}
				mutUsage.EndUse();
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

void SSWR::SMonitor::SMonitorSvrCore::DataSkipped(NN<IO::Stream> stm, AnyType stmObj, UnsafeArray<const UInt8> buff, UOSInt buffSize)
{
}

void SSWR::SMonitor::SMonitorSvrCore::NewNotify(NN<const Net::SocketUtil::AddressInfo> addr, UInt16 port, Data::Timestamp ts, UInt8 type, UInt32 procId, Text::CStringNN progName)
{
	UTF8Char sbuff[128];
	UnsafeArray<UTF8Char> sptr;
	Net::Email::EmailMessage msg;
	Text::StringBuilderUTF8 sb;
	sptr = Net::SocketUtil::GetAddrName(sbuff, addr).Or(sbuff);
	sb.AppendC(UTF8STRC("Server IP: "));
	sb.AppendP(sbuff, sptr);
	sb.AppendC(UTF8STRC("\r\nSource Port: "));
	sb.AppendU16(port);
	sb.AppendC(UTF8STRC("\r\nAction Time: "));
	sb.AppendTSNoZone(ts.ToLocalTime());
	sb.AppendC(UTF8STRC("\r\nAction Type: "));
	sb.Append((type == 1)?CSTR("Update"):CSTR("Start"));
	sb.AppendC(UTF8STRC("\r\nProcess Id: "));
	sb.AppendU32(procId);
	sb.AppendC(UTF8STRC("\r\nProcess Name: "));
	sb.Append(progName);

	msg.SetSubject(CSTR("ProgMonitor Notification"));
	msg.SetContent(sb.ToCString(), CSTR("text/plain"));
	msg.AddTo(CSTR("Simon Wong"), CSTR("sswroom@yahoo.com"));

	Text::CStringNN smtpFrom = CSTR("alert@caronline.hk");
	IO::LogWriter logWriter(this->log, IO::LogHandler::LogLevel::Raw);
	Net::Email::SMTPClient cli(this->sockf, this->ssl, CSTR("webmail.caronline.hk"), 465, Net::Email::SMTPConn::ConnType::SSL, &logWriter, 60);
	cli.SetPlainAuth(CSTR("alert@caronline.hk"), CSTR("caronlineskypower"));

	msg.SetFrom(CSTR_NULL, smtpFrom);
	sb.ClearStr();
	Net::Email::EmailMessage::GenerateMessageID(sb, smtpFrom);
	msg.SetMessageId(sb.ToCString());
	cli.Send(msg);
}

void SSWR::SMonitor::SMonitorSvrCore::TCPSendLoginReply(NN<IO::Stream> stm, Int64 cliTime, Int64 svrTime, UInt8 status)
{
	UInt8 cmdBuff[17];
	UInt8 packetBuff[27];
	UOSInt packetSize;
	WriteInt64(&cmdBuff[0], cliTime);
	WriteInt64(&cmdBuff[8], svrTime);
	cmdBuff[16] = status;
	packetSize = this->protoHdlr.BuildPacket(packetBuff, 1, 0, cmdBuff, 17, 0);
	stm->Write(Data::ByteArrayR(packetBuff, packetSize));
}

void SSWR::SMonitor::SMonitorSvrCore::TCPSendKAReply(NN<IO::Stream> stm, Int64 cliTime, Int64 svrTime)
{
	UInt8 cmdBuff[16];
	UInt8 packetBuff[26];
	UOSInt packetSize;
	WriteInt64(&cmdBuff[0], cliTime);
	WriteInt64(&cmdBuff[8], svrTime);
	packetSize = this->protoHdlr.BuildPacket(packetBuff, 7, 0, cmdBuff, 16, 0);
	stm->Write(Data::ByteArrayR(packetBuff, packetSize));
}

void SSWR::SMonitor::SMonitorSvrCore::TCPSendCapturePhoto(NN<IO::Stream> stm)
{
	UInt8 packetBuff[10];
	UOSInt packetSize;
	packetSize = this->protoHdlr.BuildPacket(packetBuff, 9, 0, packetBuff, 0, 0);
	stm->Write(Data::ByteArrayR(packetBuff, packetSize));
}

void SSWR::SMonitor::SMonitorSvrCore::TCPSendPhotoEnd(NN<IO::Stream> stm, Int64 photoTime)
{
	UInt8 cmdBuff[8];
	UInt8 packetBuff[18];
	UOSInt packetSize;
	WriteInt64(&cmdBuff[0], photoTime);
	packetSize = this->protoHdlr.BuildPacket(packetBuff, 15, 0, cmdBuff, 8, 0);
	stm->Write(Data::ByteArrayR(packetBuff, packetSize));
}

void SSWR::SMonitor::SMonitorSvrCore::TCPSendSetOutput(NN<IO::Stream> stm, UInt32 outputNum, Bool toHigh)
{
	UInt8 cmdBuff[2];
	UInt8 packetBuff[12];
	UOSInt packetSize;
	cmdBuff[0] = (UInt8)outputNum;
	cmdBuff[1] = toHigh?1:0;
	packetSize = this->protoHdlr.BuildPacket(packetBuff, 21, 0, cmdBuff, 2, 0);
	stm->Write(Data::ByteArrayR(packetBuff, packetSize));
}

void SSWR::SMonitor::SMonitorSvrCore::UDPSendReadingRecv(NN<const Net::SocketUtil::AddressInfo> addr, UInt16 port, Int64 recTime)
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

void SSWR::SMonitor::SMonitorSvrCore::UDPSendCapturePhoto(NN<const Net::SocketUtil::AddressInfo> addr, UInt16 port)
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

void SSWR::SMonitor::SMonitorSvrCore::UDPSendPhotoPacket(NN<const Net::SocketUtil::AddressInfo> addr, UInt16 port, Int64 photoTime, UInt32 seq)
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

void SSWR::SMonitor::SMonitorSvrCore::UDPSendPhotoEnd(NN<const Net::SocketUtil::AddressInfo> addr, UInt16 port, Int64 photoTime)
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

void SSWR::SMonitor::SMonitorSvrCore::UDPSendSetOutput(NN<const Net::SocketUtil::AddressInfo> addr, UInt16 port, UInt8 outputNum, Bool isHigh)
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
	Data::ArrayListNN<DeviceInfo> devList;
	Data::ArrayListNN<DevRecord2> recList;
	Data::ArrayListNN<DevRecord2> recList2;
	NN<DeviceInfo> dev;
	NN<DevRecord2> rec;
	Data::DateTime dt;
	IO::FileStream *fs;
	UOSInt i;
	UOSInt j;
	UOSInt k;
	Int32 currYear;
	Int32 currMonth;
	Int32 currDay;
	UTF8Char sbuff[512];
	UnsafeArray<UTF8Char> sptr;
	UInt8 fsBuff[32];

	Sync::RWMutexUsage mutUsage(this->devMut, false);
	devList.AddAll(this->devMap);
	mutUsage.EndUse();
	dt.ToUTCTime();
	fs = 0;

	i = devList.GetCount();
	while (i-- > 0)
	{
		dev = devList.GetItemNoCheck(i);
		Sync::RWMutexUsage mutUsage(dev->mut, true);
		recList.AddAll(dev->recToStore);
		dev->recToStore.Clear();
		mutUsage.EndUse();

		currYear = 0;
		currMonth = 0;
		currDay = 0;

		j = 0;
		k = recList.GetCount();
		while (j < k)
		{
			rec = recList.GetItemNoCheck(j);
			dt.SetTicks(rec->recTime);
			if (dt.GetDay() != currDay || dt.GetMonth() != currMonth || dt.GetYear() != currYear)
			{
				SDEL_CLASS(fs);
				currYear = dt.GetYear();
				currMonth = dt.GetMonth();
				currDay = dt.GetDay();

				sptr = this->dataDir->ConcatTo(sbuff);
				sptr = dt.ToString(sptr, "yyyyMM");
				*sptr++ = IO::Path::PATH_SEPERATOR;
				sptr = Text::StrInt64(sptr, dev->cliId);
				IO::Path::CreateDirectory(CSTRP(sbuff, sptr));
				*sptr++ = IO::Path::PATH_SEPERATOR;
				sptr = dt.ToString(sptr, "yyyyMMdd");
				sptr = Text::StrConcatC(sptr, UTF8STRC(".rec"));

				NEW_CLASS(fs, IO::FileStream(CSTRP(sbuff, sptr), IO::FileMode::Append, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal));
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
				fs->Write(Data::ByteArrayR(fsBuff, 32));
				if (rec->nreading)
				{
					fs->Write(Data::ByteArrayR((UInt8*)&rec->readings[0], 16 * rec->nreading));
				}
				MemFreeNN(rec);
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
			Sync::RWMutexUsage mutUsage(dev->mut, true);
			dev->recToStore.AddAll(recList2);
			mutUsage.EndUse();
			recList2.Clear();
		}
	}
}

void SSWR::SMonitor::SMonitorSvrCore::SavePhoto(Int64 cliId, Int64 photoTime, Int32 photoFmt, UInt8 *photoBuff, UOSInt photoSize)
{
	UTF8Char sbuff[512];
	UnsafeArray<UTF8Char> sptr;
	Data::DateTime dt;
	dt.ToUTCTime();
	dt.SetTicks(photoTime);

	sptr = this->dataDir->ConcatTo(sbuff);
	sptr = dt.ToString(sptr, "yyyyMM");
	*sptr++ = IO::Path::PATH_SEPERATOR;
	sptr = Text::StrInt64(sptr, cliId);
	*sptr++ = IO::Path::PATH_SEPERATOR;
	sptr = Text::StrConcatC(sptr, UTF8STRC("Photo"));
	sptr = dt.ToString(sptr, "yyyyMMdd");
	IO::Path::CreateDirectory(CSTRP(sbuff, sptr));
	*sptr++ = IO::Path::PATH_SEPERATOR;
	sptr = Text::StrInt64(sptr, photoTime);
	sptr = Text::StrConcatC(sptr, UTF8STRC(".jpg"));
	IO::FileStream fs(CSTRP(sbuff, sptr), IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
	fs.Write(Data::ByteArrayR(photoBuff, photoSize));
	
	/////////////////////////////////////////////
}

void SSWR::SMonitor::SMonitorSvrCore::LoadData()
{
	Text::StringBuilderUTF8 sb;
	Data::DateTime dt;
	Sync::MutexUsage mutUsage;
	NN<DB::DBTool> db;
	if (!this->UseDB(mutUsage).SetTo(db))
		return;
	NN<DB::DBReader> r;
	NN<WebUser> user;
	UnsafeArray<UTF8Char> sarr[2];
	UOSInt i;
	UOSInt j;
	if (db->ExecuteReader(CSTR("select id, userName, pwd, userType from webuser order by id")).SetTo(r))
	{
		while (r->ReadNext())
		{
			NEW_CLASSNN(user, WebUser());
			user->userId = r->GetInt32(0);
			sb.ClearStr();
			r->GetStr(1, sb);
			user->userName = Text::StrCopyNew(sb.ToString()).Ptr();
			sb.ClearStr();
			r->GetStr(2, sb);
			sb.Hex2Bytes(user->md5Pwd);
			user->userType = r->GetInt32(3);
			Sync::RWMutexUsage mutUsage(this->userMut, true);
			this->userMap.Put(user->userId, user);
			this->userNameMap.Put(user->userName, user.Ptr());
			mutUsage.EndUse();
		}
		db->CloseReader(r);
	}

	NN<DeviceInfo> dev;
	if (db->ExecuteReader(CSTR("select id, cpuName, platformName, lastKATime, flags, readingTime, nreading, dsensors, ndigital, reading1Status, reading1, reading2Status, reading2, reading3Status, reading3, reading4Status, reading4, reading5Status, reading5, reading6Status, reading6, reading7Status, reading7, reading8Status, reading8, devName, readingNames, digitalNames, reading9Status, reading9, reading10Status, reading10, reading11Status, reading11, reading12Status, reading12, reading13Status, reading13, reading14Status, reading14, reading15Status, reading15, reading16Status, reading16, reading17Status, reading17, reading18Status, reading18, reading19Status, reading19, reading20Status, reading20, reading21Status, reading21, reading22Status, reading22, reading23Status, reading23, reading24Status, reading24, nOutput, reading25Status, reading25, reading26Status, reading26, reading27Status, reading27, reading28Status, reading28, reading29Status, reading29, reading30Status, reading30, reading31Status, reading31, reading32Status, reading32, reading33Status, reading33, reading34Status, reading34, reading35Status, reading35, reading36Status, reading36, reading37Status, reading37, reading38Status, reading38, reading39Status, reading39, reading40Status, reading40, version from device order by id")).SetTo(r))
	{
		Data::ArrayListNN<DevRecord2> recList;
		NN<DevRecord2> rec;

		while (r->ReadNext())
		{
			NEW_CLASSNN(dev, DeviceInfo());
			dev->cliId = r->GetInt64(0);
			dev->cpuName = r->GetNewStrNN(1);
			dev->platformName = r->GetNewStrNN(2);
			dev->lastKATime = r->GetTimestamp(3).ToTicks();
			dev->flags = r->GetInt32(4);
			dev->readingTime = r->GetTimestamp(5).ToTicks();
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
			dev->devName = r->GetNewStr(25);
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
			if (r->GetStr(26, sb))
			{
				sarr[1] = sb.v;
				i = 0;
				while (true)
				{
					j = Text::StrSplit(sarr, 2, sarr[1], '|');
					dev->readingNames[i] = Text::StrCopyNew(sarr[0]).Ptr();
					i++;
					if (j != 2)
						break;
				}
			}
			sb.ClearStr();
			if (r->GetStr(27, sb))
			{
				sarr[1] = sb.v;
				i = 0;
				while (true)
				{
					j = Text::StrSplit(sarr, 2, sarr[1], '|');
					dev->digitalNames[i] = Text::StrCopyNew(sarr[0]).Ptr();
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
			dev->stm = 0;
			dev->photoBuff = 0;
			dev->photoOfst = 0;
			dev->photoSeq = 0;
			dev->photoSize = 0;
			dev->photoTime = 0;
			dev->valUpdated = true;

			Sync::RWMutexUsage devMutUsage(this->devMut, true);
			this->devMap.Put(dev->cliId, dev);
			devMutUsage.EndUse();

			this->DeviceQueryRec(dev->cliId, this->currDate, this->currDate + 86400000, recList);
			i = 0;
			j = recList.GetCount();
			while (i < j)
			{
				rec = recList.GetItemNoCheck(i);
				if (dev->todayRecs.Put(rec->recTime, rec).SetTo(rec))
				{
					MemFreeNN(rec);
				}
				i++;
			}
			recList.Clear();
			this->DeviceQueryRec(dev->cliId, this->currDate - 86400000, this->currDate, recList);
			i = 0;
			j = recList.GetCount();
			while (i < j)
			{
				rec = recList.GetItemNoCheck(i);
				if (dev->yesterdayRecs.Put(rec->recTime, rec).SetTo(rec))
				{
					MemFreeNN(rec);
				}
				i++;
			}
			recList.Clear();
		}
		db->CloseReader(r);
	}

	if (db->ExecuteReader(CSTR("select webuser_id, device_id from webuser_device order by webuser_id")).SetTo(r))
	{
		while (r->ReadNext())
		{
			if (this->DeviceGet(r->GetInt64(1)).SetTo(dev))
			{
				Sync::RWMutexUsage mutUsage(this->userMut, false);
				if (this->userMap.Get(r->GetInt32(0)).SetTo(user))
				{
					mutUsage.EndUse();
					Sync::RWMutexUsage mutUsage(user->mut, true);
					user->devMap.Put(dev->cliId, dev);
				}
			}
		}
		db->CloseReader(r);
	}
}

Optional<DB::DBTool> SSWR::SMonitor::SMonitorSvrCore::UseDB(NN<Sync::MutexUsage> mutUsage)
{
	NN<Sync::Mutex> mut;
	if (mut.Set(this->dbMut))
	{
		mutUsage->ReplaceMutex(mut);
		return this->db;
	}
	return 0;
}

void SSWR::SMonitor::SMonitorSvrCore::UserPwdCalc(UnsafeArray<const UTF8Char> userName, UnsafeArray<const UTF8Char> pwd, UInt8 *buff)
{
	Crypto::Hash::MD5 md5;
	Text::StringBuilderUTF8 sb;
	sb.AppendSlow(userName);
	sb.AppendC(UTF8STRC(" pwd "));
	sb.AppendSlow(pwd);
	md5.Calc(sb.ToString(), sb.GetLength());
	md5.GetValue(buff);
}

SSWR::SMonitor::SMonitorSvrCore::SMonitorSvrCore(NN<IO::Writer> writer, NN<Media::DrawEngine> deng) : protoHdlr(*this), thread(CheckThread, this, CSTR("SMonitorSvrCore"))
{
	NEW_CLASSNN(this->sockf, Net::OSSocketFactory(true));
	this->ssl = Net::SSLEngineFactory::Create(sockf, true);
	NEW_CLASSNN(this->parsers, Parser::FullParserList());
	this->deng = deng;
	this->dataDir = 0;
	this->cliSvr = 0;
	this->cliMgr = 0;
	this->notifyUDP = 0;
	this->notifyPwd = 0;
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
	this->initErr = false;

	NN<IO::ConfigFile> cfg;
	NN<Text::String> s;
	NN<Text::String> s2;
	UInt16 port;
	Text::StringBuilderUTF8 sb;
	{
		UTF8Char sbuff[512];
		UnsafeArray<UTF8Char> sptr;
		sptr = IO::Path::GetProcessFileName(sbuff).Or(sbuff);
		sptr = IO::Path::AppendPath(sbuff, sptr, CSTR("UserAgent.txt"));
		{
			IO::FileStream fs(CSTRP(sbuff, sptr), IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
			Text::UTF8Reader reader(fs);
			this->uaLog.ReadLogs(&reader);
		}

		sptr = IO::Path::GetProcessFileName(sbuff).Or(sbuff);
		sptr = IO::Path::AppendPath(sbuff, sptr, CSTR("Referer.txt"));
		{
			IO::FileStream fs(CSTRP(sbuff, sptr), IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
			Text::UTF8Reader reader(fs);
			this->refererLog.ReadLogs(&reader);
		}
	}
	if (!IO::IniFile::ParseProgConfig(0).SetTo(cfg))
	{
		writer->WriteLine(CSTR("Config file not found"));
	}
	else
	{
		if (cfg->GetValue(CSTR("LogDir")).SetTo(s))
		{
			sb.ClearStr();
			sb.Append(s);
			if (!sb.EndsWith(IO::Path::PATH_SEPERATOR))
			{
				sb.AppendChar(IO::Path::PATH_SEPERATOR, 1);
			}
			sb.AppendC(UTF8STRC("Log"));
			this->log.AddFileLog(sb.ToCString(), IO::LogHandler::LogType::PerDay, IO::LogHandler::LogGroup::PerMonth, IO::LogHandler::LogLevel::Command, "yyyy-MM-dd HH:mm:ss.fff", false);
			sb.RemoveChars(3);
			sb.AppendC(UTF8STRC("Raw"));
			this->log.AddFileLog(sb.ToCString(), IO::LogHandler::LogType::PerDay, IO::LogHandler::LogGroup::PerMonth, IO::LogHandler::LogLevel::Raw, "yyyy-MM-dd HH:mm:ss.fff", false);
		}

		if (cfg->GetValue(CSTR("NotifyPwd")).SetTo(s))
		{
			this->notifyPwd = s->Clone().Ptr();
		}
		else
		{
			writer->WriteLine(CSTR("NotifyPwd not found"));
			this->initErr = true;
		}
		if (cfg->GetValue(CSTR("DataDir")).SetTo(s))
		{
			if (!s->EndsWith(IO::Path::PATH_SEPERATOR))
			{
				sb.ClearStr();
				sb.Append(s);
				sb.AppendChar(IO::Path::PATH_SEPERATOR, 1);
				this->dataDir = Text::String::New(sb.ToString(), sb.GetLength()).Ptr();
			}
			else
			{
				this->dataDir = s->Clone().Ptr();
			}
		}

		if (cfg->GetValue(CSTR("MySQLServer")).SetTo(s) && cfg->GetValue(CSTR("MySQLDB")).SetTo(s2))
		{
			this->db = Net::MySQLTCPClient::CreateDBTool(this->sockf, s, s2, Text::String::OrEmpty(cfg->GetValue(CSTR("UID"))), Text::String::OrEmpty(cfg->GetValue(CSTR("PWD"))), this->log, CSTR("DB: "));
			NEW_CLASS(this->dbMut, Sync::Mutex());
			if (this->db.IsNull())
			{
				writer->WriteLine(CSTR("Error in connecting to mysql database"));
			}
			else
			{
				LoadData();
			}
		}
		else
		{
			if (cfg->GetValue(CSTR("DSN")).SetTo(s))
			{
				this->db = DB::ODBCConn::CreateDBTool(s, cfg->GetValue(CSTR("UID")), cfg->GetValue(CSTR("PWD")), cfg->GetValue(CSTR("Schema")), this->log, CSTR("DB: "));
				NEW_CLASS(this->dbMut, Sync::Mutex());
				if (this->db.IsNull())
				{
					writer->WriteLine(CSTR("Error in connecting to odbc database"));
				}
				else
				{
					LoadData();
				}
			}
			else
			{
				writer->WriteLine(CSTR("Config DSN/MySQLServer not found"));
			}
		}

		if (cfg->GetValue(CSTR("WebPort")).SetTo(s))
		{
			if (cfg->GetValue(CSTR("HTTPFiles")).SetTo(s2) && IO::Path::GetPathType(s2->ToCString()) == IO::Path::PathType::Directory)
			{
				if (s->ToUInt16(port) && port > 0)
				{
					NN<Net::WebServer::HTTPDirectoryHandler> hdlr;
					NN<SSWR::SMonitor::SMonitorWebHandler> shdlr;
					NN<Net::WebServer::HTTPDirectoryHandler> fileshdlr;
					NN<SSWR::Benchmark::BenchmarkWebHandler> benchhdlr;
					NN<SSWR::VAMS::VAMSBTWebHandler> vamsHdlr;
					NN<Net::WebServer::HTTPDirectoryHandler> jshdlr;
					SSWR::VAMS::VAMSBTList *btList;
					NEW_CLASSNN(hdlr, Net::WebServer::HTTPDirectoryHandler(Text::String::OrEmpty(s2), false, 0, false));
					NEW_CLASSNN(shdlr, SSWR::SMonitor::SMonitorWebHandler(this));
					NEW_CLASSNN(benchhdlr, SSWR::Benchmark::BenchmarkWebHandler());

					sb.ClearStr();
					IO::Path::GetProcessFileName(sb);
					IO::Path::AppendPath(sb, UTF8STRC("files"));
					NEW_CLASSNN(fileshdlr, Net::WebServer::HTTPDirectoryHandler(sb.ToCString(), false, 0, false));
					shdlr->HandlePath(CSTR("/files"), fileshdlr, true);
					hdlr->HandlePath(CSTR("/monitor"), shdlr, true);
					hdlr->HandlePath(CSTR("/benchmark"), benchhdlr, true);
					if (cfg->GetValue(CSTR("VAMSLogPath")).SetTo(s))
					{
						NEW_CLASS(btList, SSWR::VAMS::VAMSBTList());
						NEW_CLASSNN(vamsHdlr, SSWR::VAMS::VAMSBTWebHandler(s, btList));
						hdlr->HandlePath(CSTR("/vams"), vamsHdlr, true);
					}
					sb.ClearStr();
					IO::Path::GetProcessFileName(sb);
					IO::Path::AppendPath(sb, UTF8STRC("node_modules"));
					NEW_CLASSNN(jshdlr, Net::WebServer::NodeModuleHandler(sb.ToCString(), 0));
					hdlr->HandlePath(CSTR("/js"), jshdlr, true);

					hdlr->ExpandPackageFiles(this->parsers, CSTR("*.spk"));
					this->webHdlr = hdlr.Ptr();
					NEW_CLASS(this->listener, Net::WebServer::WebListener(this->sockf, 0, hdlr, port, 60, 1, 4, CSTR("SSWRServer/1.0"), false, Net::WebServer::KeepAlive::Default, false));
					if (this->listener->IsError())
					{
						DEL_CLASS(this->listener);
						this->listener = 0;
						this->initErr = true;
						writer->WriteLine(CSTR("Error in listening web port"));
					}
					else
					{
						this->listener->SetAccessLog(&this->log, IO::LogHandler::LogLevel::Command);
						this->listener->SetRequestLog(this);
						if (this->notifyPwd)
						{
							NEW_CLASS(this->notifyUDP, Net::UDPServer(this->sockf, 0, port, CSTR_NULL, OnNotifyUDPPacket, this, this->log, CSTR("Not: "), 2, false));
							if (this->notifyUDP->IsError())
							{
								writer->WriteLine(CSTR("Error in listening web(notify) port"));
								DEL_CLASS(this->notifyUDP);
								this->notifyUDP = 0;
							}
						}
					}
				}
				else
				{
					writer->WriteLine(CSTR("Config WebPort invalid"));
				}
			}
			else
			{
				writer->WriteLine(CSTR("Config HTTPFiles not valid"));
			}
		}
		else
		{
			writer->WriteLine(CSTR("Config WebPort not found"));
		}

		if (cfg->GetValue(CSTR("ClientPort")).SetTo(s))
		{
			if (s->ToUInt16(port) && port > 0)
			{
				NEW_CLASS(this->cliMgr, Net::TCPClientMgr(300, OnClientEvent, OnClientData, this, 4, OnClientTimeout));
				NEW_CLASS(this->cliSvr, Net::TCPServer(this->sockf, 0, port, this->log, OnServerConn, this, CSTR("CLI: "), false));
				if (this->cliSvr->IsV4Error())
				{
					DEL_CLASS(this->cliSvr);
					this->cliSvr = 0;
					this->initErr = true;
					writer->WriteLine(CSTR("Error in listening client port"));
				}
			}
			else
			{
				writer->WriteLine(CSTR("Config ClientPort invalid"));
			}
		}
		else
		{
			writer->WriteLine(CSTR("Config ClientPort not found"));
		}

		if (cfg->GetValue(CSTR("DataUDPPort")).SetTo(s))
		{
			if (s->ToUInt16(port) && port > 0)
			{
				NN<Crypto::Hash::CRC16> crc;
				NEW_CLASSNN(crc, Crypto::Hash::CRC16(Crypto::Hash::CRC16::GetPolynomialCCITT()));
				NEW_CLASS(this->dataCRC, Crypto::Hash::HashCalc(crc));
				NEW_CLASS(this->dataUDP, Net::UDPServer(this->sockf, 0, port, CSTR_NULL, OnDataUDPPacket, this, this->log, CSTR("DUDP: "), 4, false));
				if (this->dataUDP->IsError())
				{
					DEL_CLASS(this->dataUDP);
					this->dataUDP = 0;
					writer->WriteLine(CSTR("Error in listening data UDP port"));
				}
			}
			else
			{
				writer->WriteLine(CSTR("Config DataUDPPort invalid"));
			}
		}
		else
		{
			writer->WriteLine(CSTR("Config DataUDPPort not found"));
		}

		cfg.Delete();

		if (!this->IsError() && this->cliSvr->Start() && this->listener->Start())
		{
			this->thread.Start();
		}
	}
}

SSWR::SMonitor::SMonitorSvrCore::~SMonitorSvrCore()
{
	SDEL_CLASS(this->cliSvr);
	SDEL_CLASS(this->cliMgr);
	SDEL_CLASS(this->dataUDP);
	SDEL_CLASS(this->notifyUDP);
	SDEL_CLASS(this->listener);
	SDEL_CLASS(this->webHdlr);
	this->db.Delete();
	SDEL_CLASS(this->dbMut);
	this->thread.Stop();
	this->SaveDatas();

	UOSInt i;
	UOSInt j;
	NN<WebUser> user;
	i = this->userMap.GetCount();
	while (i-- > 0)
	{
		user = this->userMap.GetItemNoCheck(i);
		Text::StrDelNew(user->userName);
		user.Delete();
	}

	NN<DeviceInfo> dev;
	NN<DevRecord2> rec;
	i = this->devMap.GetCount();
	while (i-- > 0)
	{
		dev = this->devMap.GetItemNoCheck(i);
		OPTSTR_DEL(dev->devName);
		dev->cpuName->Release();
		dev->platformName->Release();
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

		j = dev->recToStore.GetCount();
		while (j-- > 0)
		{
			rec = dev->recToStore.GetItemNoCheck(j);
			MemFreeNN(rec);
		}

		j = dev->todayRecs.GetCount();
		while (j-- > 0)
		{
			rec = dev->todayRecs.GetItemNoCheck(j);
			MemFreeNN(rec);
		}

		j = dev->yesterdayRecs.GetCount();
		while (j-- > 0)
		{
			rec = dev->yesterdayRecs.GetItemNoCheck(j);
			MemFreeNN(rec);
		}

		NN<IO::MemoryStream> mstm;
		j = dev->imgCaches.GetCount();
		while (j-- > 0)
		{
			mstm = dev->imgCaches.GetItemNoCheck(j);
			mstm.Delete();
		}
		if (dev->photoBuff)
		{
			MemFree(dev->photoBuff);
		}
		dev.Delete();
	}

	this->UserAgentStore();
	this->RefererStore();

	SDEL_CLASS(this->dataCRC);
	this->parsers.Delete();
	this->ssl.Delete();
	this->sockf.Delete();
	this->deng.Delete();
	SDEL_STRING(this->notifyPwd);
	SDEL_STRING(this->dataDir);
}

Bool SSWR::SMonitor::SMonitorSvrCore::IsError()
{
	return this->cliSvr == 0 || this->db.IsNull() || this->listener == 0 || this->dataDir == 0 || this->notifyUDP == 0 || this->initErr;
}

NN<Media::DrawEngine> SSWR::SMonitor::SMonitorSvrCore::GetDrawEngine()
{
	return this->deng;
}

Optional<SSWR::SMonitor::SMonitorSvrCore::DeviceInfo> SSWR::SMonitor::SMonitorSvrCore::DevGet(Int64 cliId)
{
	NN<DeviceInfo> devInfo;
	if (this->DeviceGet(cliId).SetTo(devInfo))
		return devInfo;
	return 0;
}

Optional<SSWR::SMonitor::SMonitorSvrCore::DeviceInfo> SSWR::SMonitor::SMonitorSvrCore::DevGetOrAdd(Int64 cliId)
{
	NN<DeviceInfo> devInfo;
	if (this->DeviceGet(cliId).SetTo(devInfo))
		return devInfo;
	return this->DevAdd(cliId, CSTR("UDP Unknown"), CSTR("UDP Unknown"));
}

Optional<SSWR::SMonitor::SMonitorSvrCore::DeviceInfo> SSWR::SMonitor::SMonitorSvrCore::DevAdd(Int64 cliId, Text::CStringNN cpuName, Text::CStringNN platformName)
{
	NN<DeviceInfo> dev;
	Sync::RWMutexUsage devMutUsage(this->devMut, true);
	if (this->devMap.Get(cliId).SetTo(dev))
	{
		return 0;
	}

	Data::Timestamp ts = Data::Timestamp::UtcNow();
	Sync::MutexUsage dbMutUsage;
	NN<DB::DBTool> db;
	if (!this->UseDB(dbMutUsage).SetTo(db))
	{
		return 0;
	}
	DB::SQLBuilder sql(db);
	sql.AppendCmdC(CSTR("insert into device (id, cpuName, platformName, lastKATime, flags) values ("));
	sql.AppendInt64(cliId);
	sql.AppendCmdC(CSTR(", "));
	sql.AppendStrC(cpuName);
	sql.AppendCmdC(CSTR(", "));
	sql.AppendStrC(platformName);
	sql.AppendCmdC(CSTR(", "));
	sql.AppendTS(ts);
	sql.AppendCmdC(CSTR(", "));
	sql.AppendInt32(0);
	sql.AppendCmdC(CSTR(")"));
	if (db->ExecuteNonQuery(sql.ToCString()) > 0)
	{
		UOSInt i;
		NEW_CLASSNN(dev, DeviceInfo());
		dev->cliId = cliId;
		dev->cpuName = Text::String::New(cpuName);
		dev->platformName = Text::String::New(platformName);
		dev->lastKATime = ts.ToTicks();
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
		dev->photoBuff = 0;
		dev->photoOfst = 0;
		dev->photoSeq = 0;
		dev->photoSize = 0;
		dev->photoTime = 0;
		dev->valUpdated = true;

		this->devMap.Put(dev->cliId, dev);
		return dev;
	}
	return 0;
}

Bool SSWR::SMonitor::SMonitorSvrCore::DeviceRecvReading(NN<DeviceInfo> dev, Int64 cliTime, UOSInt nDigitals, UOSInt nReading, UOSInt nOutput, UInt32 digitalVals, ReadingInfo *readings, Int32 profileId, UInt32 cliIP, UInt16 port)
{
	UTF8Char sbuff[32];
	UnsafeArray<UTF8Char> sptr;
	UOSInt i;
	Bool succ = false;
	Data::Timestamp ts = Data::Timestamp::UtcNow();
	Int64 t;
	t = ts.ToTicks();
	if (cliTime > dev->readingTime && cliTime < t + 300000)
	{
		Sync::MutexUsage dbMutUsage;
		NN<DB::DBTool> db;
		if (this->UseDB(dbMutUsage).SetTo(db))
		{
			DB::SQLBuilder sql(db);
			if (nReading > SMONITORCORE_DEVREADINGCNT)
			{
				nReading = SMONITORCORE_DEVREADINGCNT;
			}
			sql.AppendCmdC(CSTR("update device set readingTime = "));
			sql.AppendTS(Data::Timestamp(cliTime, 0));
			sql.AppendCmdC(CSTR(", nreading = "));
			sql.AppendInt32((Int32)nReading);
			sql.AppendCmdC(CSTR(", ndigital = "));
			sql.AppendInt32((Int32)nDigitals);
			sql.AppendCmdC(CSTR(", nOutput = "));
			sql.AppendInt32((Int32)nOutput);
			sql.AppendCmdC(CSTR(", dsensors = "));
			sql.AppendInt32((Int32)digitalVals);
			i = 0;
			while (i < nReading)
			{
				if (ReadInt16(&readings[i].status[6]) != 0)
				{
					sptr = Text::StrUOSInt(Text::StrConcatC(sbuff, UTF8STRC("reading")), i + 1);
					sql.AppendCmdC(CSTR(", "));
					sql.AppendCol(sbuff);
					sql.AppendCmdC(CSTR(" = "));
					sql.AppendDbl(readings[i].reading);

					sptr = Text::StrConcatC(sptr, UTF8STRC("Status"));
					sql.AppendCmdC(CSTR(", "));
					sql.AppendCol(sbuff);
					sql.AppendCmdC(CSTR(" = "));
					sql.AppendInt64(ReadInt64(readings[i].status));
				}

				i++;
			}
			sql.AppendCmdC(CSTR(" where id = "));
			sql.AppendInt64(dev->cliId);
			if (db->ExecuteNonQuery(sql.ToCString()) > 0)
			{
				Sync::RWMutexUsage mutUsage(dev->mut, true);
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
			}
			dbMutUsage.EndUse();
		}

		NN<DevRecord2> rec;
		rec = MemAllocNN(DevRecord2);
		rec->recTime = cliTime;
		rec->recvTime = t;
		rec->profileId = profileId;
		rec->ndigital = nDigitals;
		rec->nreading = nReading;
		rec->nOutput = nOutput;
		rec->digitalVals = digitalVals;
		MemCopyNO(rec->readings, readings, sizeof(ReadingInfo) * nReading);

		Sync::RWMutexUsage dateMutUsage(this->dateMut, false);
		Sync::RWMutexUsage devMutUsage(dev->mut, true);
		dev->recToStore.Add(rec);

		if (t >= this->currDate - 86400000LL && t < this->currDate)
		{
			NN<DevRecord2> rec2;
			rec2 = MemAllocNN(DevRecord2);
			rec2.CopyFrom(rec);
			if (dev->yesterdayRecs.Put(rec2->recTime, rec2).SetTo(rec2))
			{
				MemFreeNN(rec2);
			}
			dev->valUpdated = true;
		}
		else if (t >= this->currDate && t < this->currDate + 86400000LL)
		{
			NN<DevRecord2> rec2;
			rec2 = MemAllocNN(DevRecord2);
			rec2.CopyFrom(rec);
			if (dev->todayRecs.Put(rec2->recTime, rec2).SetTo(rec2))
			{
				MemFreeNN(rec2);
			}
			dev->valUpdated = true;
		}
		devMutUsage.EndUse();
	}
	return succ;
}

Bool SSWR::SMonitor::SMonitorSvrCore::DeviceKARecv(NN<DeviceInfo> dev, Int64 kaTime)
{
	Data::Timestamp ts = Data::Timestamp(kaTime, 0);
	Bool succ = false;

	Sync::MutexUsage dbMutUsage;
	NN<DB::DBTool> db;
	if (this->UseDB(dbMutUsage).SetTo(db))
	{
		DB::SQLBuilder sql(db);
		sql.AppendCmdC(CSTR("update device set lastKATime = "));
		sql.AppendTS(ts);
		sql.AppendCmdC(CSTR(" where id = "));
		sql.AppendInt64(dev->cliId);
		if (db->ExecuteNonQuery(sql.ToCString()) >= 0)
		{
			succ = true;
		}
	}
	return succ;
}

Bool SSWR::SMonitor::SMonitorSvrCore::DeviceSetName(Int64 cliId, NN<Text::String> devName)
{
	NN<SSWR::SMonitor::ISMonitorCore::DeviceInfo> dev;
	if (!this->DeviceGet(cliId).SetTo(dev))
		return false;
	if (devName->leng == 0)
		return false;
	Sync::RWMutexUsage devMutUsage(dev->mut, false);
	NN<Text::String> s;
	if (dev->devName.SetTo(s) && s->Equals(devName))
	{
		return true;
	}
	devMutUsage.EndUse();
	Bool succ = false;

	Sync::MutexUsage dbMutUsage;
	NN<DB::DBTool> db;
	if (this->UseDB(dbMutUsage).SetTo(db))
	{
		DB::SQLBuilder sql(db);
		sql.AppendCmdC(CSTR("update device set devName = "));
		sql.AppendStr(devName);
		sql.AppendCmdC(CSTR(" where id = "));
		sql.AppendInt64(dev->cliId);
		if (db->ExecuteNonQuery(sql.ToCString()) >= 0)
		{
			Sync::RWMutexUsage mutUsage(dev->mut, true);
			OPTSTR_DEL(dev->devName);
			dev->devName = devName->Clone().Ptr();
			succ = true;
		}
	}
	return succ;
}

Bool SSWR::SMonitor::SMonitorSvrCore::DeviceSetPlatform(Int64 cliId, NN<Text::String> platformName)
{
	NN<SSWR::SMonitor::ISMonitorCore::DeviceInfo> dev;
	if (!this->DeviceGet(cliId).SetTo(dev))
		return false;
	if (platformName->leng == 0)
		return false;
	Sync::RWMutexUsage devMutUsage(dev->mut, false);
	if (dev->platformName->Equals(platformName))
	{
		return true;
	}
	devMutUsage.EndUse();
	Bool succ = false;

	Sync::MutexUsage dbMutUsage;
	NN<DB::DBTool> db;
	if (this->UseDB(dbMutUsage).SetTo(db))
	{
		DB::SQLBuilder sql(db);
		sql.AppendCmdC(CSTR("update device set platformName = "));
		sql.AppendStr(platformName);
		sql.AppendCmdC(CSTR(" where id = "));
		sql.AppendInt64(dev->cliId);
		if (db->ExecuteNonQuery(sql.ToCString()) >= 0)
		{
			Sync::RWMutexUsage mutUsage(dev->mut, true);
			dev->platformName->Release();
			dev->platformName = platformName->Clone();
			succ = true;
		}
	}
	return succ;
}

Bool SSWR::SMonitor::SMonitorSvrCore::DeviceSetCPUName(Int64 cliId, NN<Text::String> cpuName)
{
	NN<SSWR::SMonitor::ISMonitorCore::DeviceInfo> dev;
	if (!this->DeviceGet(cliId).SetTo(dev))
		return false;
	if (cpuName->leng == 0)
		return false;
	Sync::RWMutexUsage devMutUsage(dev->mut, false);
	if (dev->cpuName->Equals(cpuName))
	{
		return true;
	}
	devMutUsage.EndUse();
	Bool succ = false;

	Sync::MutexUsage dbMutUsage;
	NN<DB::DBTool> db;
	if (this->UseDB(dbMutUsage).SetTo(db))
	{
		DB::SQLBuilder sql(db);
		sql.AppendCmdC(CSTR("update device set cpuName = "));
		sql.AppendStr(cpuName);
		sql.AppendCmdC(CSTR(" where id = "));
		sql.AppendInt64(dev->cliId);
		if (db->ExecuteNonQuery(sql.ToCString()) >= 0)
		{
			Sync::RWMutexUsage mutUsage(dev->mut, true);
			dev->cpuName->Release();
			dev->cpuName = cpuName->Clone();
			succ = true;
		}
	}
	return succ;
}

Bool SSWR::SMonitor::SMonitorSvrCore::DeviceSetReading(Int64 cliId, UInt32 index, UInt16 sensorId, UInt16 readingId, UnsafeArray<const UTF8Char> readingName)
{
	NN<SSWR::SMonitor::ISMonitorCore::DeviceInfo> dev;
	if (!this->DeviceGet(cliId).SetTo(dev))
		return false;
	UnsafeArray<const UTF8Char> nns;
	if (readingName[0] == 0)
		return false;
	Sync::RWMutexUsage devMutUsage(dev->mut, false);
	if (index >= dev->nReading)
	{
		return false;
	}
	if (ReadUInt16(&dev->readings[index].status[0]) != sensorId || ReadUInt16(&dev->readings[index].status[4]) != readingId)
	{
		return false;
	}
	if (dev->readingNames[index].SetTo(nns) && Text::StrEquals(nns, readingName))
	{
		return true;
	}
	UOSInt i = 0;
	Text::StringBuilderUTF8 sb;
	while (i < dev->nReading)
	{
		if (i > 0)
		{
			sb.AppendUTF8Char('|');
		}
		if (i == index)
		{
			sb.AppendSlow(readingName);
		}
		else if (dev->readingNames[i].SetTo(nns))
		{
			sb.AppendSlow(nns);
		}
		
		i++;
	}
	devMutUsage.EndUse();
	Bool succ = false;

	Sync::MutexUsage dbMutUsage;
	NN<DB::DBTool> db;
	if (this->UseDB(dbMutUsage).SetTo(db))
	{
		DB::SQLBuilder sql(db);
		sql.AppendCmdC(CSTR("update device set readingNames = "));
		sql.AppendStrUTF8(sb.ToString());
		sql.AppendCmdC(CSTR(" where id = "));
		sql.AppendInt64(dev->cliId);
		if (db->ExecuteNonQuery(sql.ToCString()) >= 0)
		{
			Sync::RWMutexUsage mutUsage(dev->mut, true);
			SDEL_TEXT(dev->readingNames[index]);
			dev->readingNames[index] = Text::StrCopyNew(readingName).Ptr();
			dev->valUpdated = true;
			succ = true;
		}
	}
	return succ;
}

Bool SSWR::SMonitor::SMonitorSvrCore::DeviceSetVersion(Int64 cliId, Int64 version)
{
	NN<SSWR::SMonitor::ISMonitorCore::DeviceInfo> dev;
	if (!this->DeviceGet(cliId).SetTo(dev))
		return false;
	if (version <= dev->version)
		return true;
	Bool succ = false;

	Sync::MutexUsage dbMutUsage;
	NN<DB::DBTool> db;
	if (this->UseDB(dbMutUsage).SetTo(db))
	{
		DB::SQLBuilder sql(db);
		sql.AppendCmdC(CSTR("update device set version = "));
		sql.AppendInt64(version);
		sql.AppendCmdC(CSTR(" where id = "));
		sql.AppendInt64(dev->cliId);
		if (db->ExecuteNonQuery(sql.ToCString()) >= 0)
		{
			Sync::RWMutexUsage mutUsage(dev->mut, true);
			dev->version = version;
			succ = true;
		}
	}
	return succ;
}

Optional<SSWR::SMonitor::ISMonitorCore::DeviceInfo> SSWR::SMonitor::SMonitorSvrCore::DeviceGet(Int64 cliId)
{
	Sync::RWMutexUsage mutUsage(this->devMut, false);
	return this->devMap.Get(cliId);
}

Bool SSWR::SMonitor::SMonitorSvrCore::DeviceModify(Int64 cliId, Text::CString devName, Int32 flags)
{
	NN<DeviceInfo> dev;
	if (!DeviceGet(cliId).SetTo(dev))
		return false;
	Text::CStringNN nndevName;
	if (devName.SetTo(nndevName) && nndevName.leng == 0)
	{
		devName.v = 0;
	}
	Bool succ = false;

	Sync::MutexUsage dbMutUsage;
	NN<DB::DBTool> db;
	if (this->UseDB(dbMutUsage).SetTo(db))
	{
		DB::SQLBuilder sql(db);
		sql.AppendCmdC(CSTR("update device set devName = "));
		sql.AppendStrC(devName);
		sql.AppendCmdC(CSTR(", flags = "));
		sql.AppendInt32(flags);
		sql.AppendCmdC(CSTR(" where id = "));
		sql.AppendInt64(dev->cliId);
		if (db->ExecuteNonQuery(sql.ToCString()) >= 0)
		{
			Sync::RWMutexUsage mutUsage(dev->mut, true);
			OPTSTR_DEL(dev->devName);
			if (devName.SetTo(nndevName))
			{
				dev->devName = Text::String::New(nndevName).Ptr();
			}
			dev->flags = flags;
			succ = true;
		}
	}
	return succ;
}

Bool SSWR::SMonitor::SMonitorSvrCore::DeviceSetReadings(NN<DeviceInfo> dev, UnsafeArrayOpt<const UTF8Char> readings)
{
	UnsafeArray<const UTF8Char> nnreadings;
	if (readings.SetTo(nnreadings) && nnreadings[0] == 0)
	{
		readings = 0;
	}
	Bool succ = false;

	Sync::MutexUsage dbMutUsage;
	NN<DB::DBTool> db;
	if (this->UseDB(dbMutUsage).SetTo(db))
	{
		DB::SQLBuilder sql(db);
		sql.AppendCmdC(CSTR("update device set readingNames = "));
		sql.AppendStrUTF8(readings);
		sql.AppendCmdC(CSTR(" where id = "));
		sql.AppendInt64(dev->cliId);
		if (db->ExecuteNonQuery(sql.ToCString()) >= 0)
		{
			UOSInt i;
			UOSInt j;
			Sync::RWMutexUsage mutUsage(dev->mut, true);
			i = SMONITORCORE_DEVREADINGCNT;
			while (i-- > 0)
			{
				SDEL_TEXT(dev->readingNames[i]);
			}
			if (readings.SetTo(nnreadings))
			{
				Text::StringBuilderUTF8 sb;
				UnsafeArray<UTF8Char> sarr[2];
				sb.AppendSlow(nnreadings);
				sarr[1] = sb.v;
				i = 0;
				while (true)
				{
					j = Text::StrSplit(sarr, 2, sarr[1], '|');
					if (sarr[0][0])
					{
						dev->readingNames[i] = Text::StrCopyNew(sarr[0]).Ptr();
					}
					if (j != 2)
						break;
					i++;
				}
			}
			dev->valUpdated = true;
			succ = true;
		}
	}
	return succ;
}

Bool SSWR::SMonitor::SMonitorSvrCore::DeviceSetDigitals(NN<DeviceInfo> dev, UnsafeArrayOpt<const UTF8Char> digitals)
{
	UnsafeArray<const UTF8Char> nndigitals;
	if (digitals.SetTo(nndigitals) && nndigitals[0] == 0)
	{
		digitals = 0;
	}
	Bool succ = false;

	Sync::MutexUsage dbMutUsage;
	NN<DB::DBTool> db;
	if (this->UseDB(dbMutUsage).SetTo(db))
	{
		DB::SQLBuilder sql(db);
		sql.AppendCmdC(CSTR("update device set digitalNames = "));
		sql.AppendStrUTF8(digitals);
		sql.AppendCmdC(CSTR(" where id = "));
		sql.AppendInt64(dev->cliId);
		if (db->ExecuteNonQuery(sql.ToCString()) >= 0)
		{
			UOSInt i;
			UOSInt j;
			Sync::RWMutexUsage mutUsage(dev->mut, true);
			i = SMONITORCORE_DIGITALCNT;
			while (i-- > 0)
			{
				SDEL_TEXT(dev->digitalNames[i]);
			}
			if (digitals.SetTo(nndigitals))
			{
				Text::StringBuilderUTF8 sb;
				UnsafeArray<UTF8Char> sarr[2];
				sb.AppendSlow(nndigitals);
				sarr[1] = sb.v;
				i = 0;
				while (true)
				{
					j = Text::StrSplit(sarr, 2, sarr[1], '|');
					if (sarr[0][0])
					{
						dev->digitalNames[i] = Text::StrCopyNew(sarr[0]).Ptr();
					}
					if (j != 2)
						break;
					i++;
				}
			}
			succ = true;
		}
	}
	return succ;
}

UOSInt SSWR::SMonitor::SMonitorSvrCore::DeviceQueryRec(Int64 cliId, Int64 startTime, Int64 endTime, NN<Data::ArrayListNN<DevRecord2>> recList)
{
	UOSInt ret = 0;
	Int64 t;
	Int64 currTime;
	UTF8Char sbuff[512];
	UnsafeArray<UTF8Char> sptr;
	UInt8 *fileBuff;
	UOSInt fileBuffSize;
	UOSInt i;
	Data::DateTime dt;
	NN<DevRecord2> rec;

	fileBuff = MemAlloc(UInt8, 4096);
	dt.ToUTCTime();
	dt.SetTicks(startTime);
	dt.ClearTime();
	currTime = dt.ToTicks();
	while (currTime < endTime)
	{
		sptr = this->dataDir->ConcatTo(sbuff);
		sptr = dt.ToString(sptr, "yyyyMM");
		*sptr++ = IO::Path::PATH_SEPERATOR;
		sptr = Text::StrInt64(sptr, cliId);
		*sptr++ = IO::Path::PATH_SEPERATOR;
		sptr = dt.ToString(sptr, "yyyyMMdd");
		sptr = Text::StrConcatC(sptr, UTF8STRC(".rec"));

		IO::FileStream fs(CSTRP(sbuff, sptr), IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
		if (!fs.IsError())
		{
			fileBuffSize = 0;
			while (true)
			{
				i = fs.Read(Data::ByteArray(&fileBuff[fileBuffSize], 4096 - fileBuffSize));
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
									rec = MemAllocNN(DevRecord2);
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
		currTime += 86400000;
		dt.AddDay(1);
	}
	MemFree(fileBuff);
	return ret;
}

Bool SSWR::SMonitor::SMonitorSvrCore::DeviceSetOutput(Int64 cliId, UInt32 outputNum, Bool toHigh)
{
	NN<DeviceInfo> dev;
	Bool succ = false;
	Sync::RWMutexUsage mutUsage(this->devMut, false);
	if (this->devMap.Get(cliId).SetTo(dev))
	{
		mutUsage.EndUse();
		Sync::RWMutexUsage mutUsage(dev->mut, false);
		NN<IO::Stream> cli;
		if (cli.Set(dev->stm))
		{
			this->TCPSendSetOutput(cli, outputNum, toHigh);
			succ = true;
		}
	}
	return succ;
}

Bool SSWR::SMonitor::SMonitorSvrCore::UserExist()
{
	return this->userMap.GetCount() > 0;
}

Bool SSWR::SMonitor::SMonitorSvrCore::UserAdd(UnsafeArray<const UTF8Char> userName, UnsafeArray<const UTF8Char> password, Int32 userType)
{
	NN<WebUser> user;
	Bool succ = false;
	Sync::RWMutexUsage userMutUsage(this->userMut, true);
	if (this->userNameMap.Get(userName))
	{
		return false;
	}
	UTF8Char sbuff[64];
	UInt8 pwdBuff[16];
	this->UserPwdCalc(userName, password, pwdBuff);
	Text::StrHexBytes(sbuff, pwdBuff, 16, 0);

	Sync::MutexUsage dbMutUsage;
	NN<DB::DBTool> db;
	if (this->UseDB(dbMutUsage).SetTo(db))
	{
		DB::SQLBuilder sql(db);
		sql.AppendCmdC(CSTR("insert into webuser (userName, pwd, userType) values ("));
		sql.AppendStrUTF8(userName);
		sql.AppendCmdC(CSTR(", "));
		sql.AppendStrUTF8(sbuff);
		sql.AppendCmdC(CSTR(", "));
		sql.AppendInt32(userType);
		sql.AppendCmdC(CSTR(")"));
		if (db->ExecuteNonQuery(sql.ToCString()) > 0)
		{
			NEW_CLASSNN(user, WebUser());
			user->userId = db->GetLastIdentity32();
			user->userName = Text::StrCopyNew(userName).Ptr();
			MemCopyNO(user->md5Pwd, pwdBuff, 16);
			user->userType = userType;

			this->userMap.Put(user->userId, user);
			this->userNameMap.Put(user->userName, user.Ptr());
			succ = true;
		}
	}
	return succ;
}

Bool SSWR::SMonitor::SMonitorSvrCore::UserSetPassword(Int32 userId, UnsafeArray<const UTF8Char> password)
{
	NN<WebUser> user;
	Bool succ = false;
	Sync::RWMutexUsage userMutUsage(this->userMut, false);
	if (!this->userMap.Get(userId).SetTo(user))
	{
		return false;
	}
	userMutUsage.EndUse();
	UTF8Char sbuff[64];
	UInt8 pwdBuff[16];
	userMutUsage.ReplaceMutex(user->mut, false);
	this->UserPwdCalc(user->userName, password, pwdBuff);
	Text::StrHexBytes(sbuff, pwdBuff, 16, 0);
	userMutUsage.EndUse();

	Sync::MutexUsage dbMutUsage;
	NN<DB::DBTool> db;
	if (this->UseDB(dbMutUsage).SetTo(db))
	{
		DB::SQLBuilder sql(db);
		sql.AppendCmdC(CSTR("update webuser set pwd = "));
		sql.AppendStrUTF8(sbuff);
		sql.AppendCmdC(CSTR(" where id = "));
		sql.AppendInt32(userId);
		if (db->ExecuteNonQuery(sql.ToCString()) >= 0)
		{
			userMutUsage.ReplaceMutex(user->mut, true);
			MemCopyNO(user->md5Pwd, pwdBuff, 16);
			userMutUsage.EndUse();
			succ = true;
		}
	}
	return succ;
}

Optional<SSWR::SMonitor::ISMonitorCore::LoginInfo> SSWR::SMonitor::SMonitorSvrCore::UserLogin(UnsafeArray<const UTF8Char> userName, UnsafeArray<const UTF8Char> password)
{
	WebUser *user;
	NN<SSWR::SMonitor::ISMonitorCore::LoginInfo> login;
	Sync::RWMutexUsage mutUsage(this->userMut, false);
	user = this->userNameMap.Get(userName);
	if (user)
	{
		Bool eq = true;
		OSInt i;
		UInt8 pwdBuff[16];
		this->UserPwdCalc(userName, password, pwdBuff);
		Sync::RWMutexUsage userMutUsage(user->mut, false);
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
			login = MemAllocNN(SSWR::SMonitor::ISMonitorCore::LoginInfo);
			login->userId = user->userId;
			login->loginId = -1;
			login->userType = user->userType;
			return login;
		}
	}
	return login;
}

void SSWR::SMonitor::SMonitorSvrCore::UserFreeLogin(NN<LoginInfo> login)
{
	MemFreeNN(login);
}

UOSInt SSWR::SMonitor::SMonitorSvrCore::UserGetDevices(Int32 userId, Int32 userType, NN<Data::ArrayListNN<DeviceInfo>> devList)
{
	UOSInt retCnt;
	NN<DeviceInfo> dev;
	UOSInt i;
	UOSInt j;
	if (userType == 0)
	{
		retCnt = 0;
		Sync::RWMutexUsage mutUsage(this->devMut, false);
		i = 0;
		j = this->devMap.GetCount();
		while (i < j)
		{
			dev = this->devMap.GetItemNoCheck(i);
			if ((dev->flags & 3) == 1)
			{
				devList->Add(dev);
				retCnt++;
			}
			i++;
		}
	}
	else if (userType == 1)
	{
		retCnt = 0;
		Sync::RWMutexUsage mutUsage(this->devMut, false);
		i = 0;
		j = this->devMap.GetCount();
		while (i < j)
		{
			dev = this->devMap.GetItemNoCheck(i);
			if ((dev->flags & 2) == 0)
			{
				devList->Add(dev);
				retCnt++;
			}
			i++;
		}
	}
	else
	{
		NN<WebUser> user;
		Sync::RWMutexUsage mutUsage(this->userMut, false);
		if (this->userMap.Get(userId).SetTo(user))
		{
			mutUsage.EndUse();
			retCnt = 0;
			mutUsage.ReplaceMutex(user->mut, false);
			i = 0;
			j = user->devMap.GetCount();
			while (i < j)
			{
				dev = user->devMap.GetItemNoCheck(i);
				if ((dev->flags & 2) == 0)
				{
					devList->Add(dev);
					retCnt++;
				}
				i++;
			}
			mutUsage.EndUse();
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
	NN<DeviceInfo> dev;
	if (userType == 0)
	{
		Sync::RWMutexUsage mutUsage(this->devMut, false);
		ret = this->devMap.Get(cliId).SetTo(dev) && (dev->flags & 1) != 0;
	}
	else if (userType == 1)
	{
		Sync::RWMutexUsage mutUsage(this->devMut, false);
		ret = this->devMap.Get(cliId).NotNull();
	}
	else
	{
		NN<WebUser> user;
		Sync::RWMutexUsage mutUsage(this->userMut, false);
		if (this->userMap.Get(userId).SetTo(user))
		{
			mutUsage.ReplaceMutex(user->mut, false);
			ret = user->devMap.Get(cliId).NotNull();
			mutUsage.EndUse();
		}
		else
		{
			ret = false;
		}
	}
	return ret;
}

UOSInt SSWR::SMonitor::SMonitorSvrCore::UserGetList(NN<Data::ArrayListNN<WebUser>> userList)
{
	UOSInt ret = userList->GetCount();
	Sync::RWMutexUsage mutUsage(this->userMut, false);
	userList->AddAll(this->userMap);
	return userList->GetCount() - ret;
}

Optional<SSWR::SMonitor::ISMonitorCore::WebUser> SSWR::SMonitor::SMonitorSvrCore::UserGet(Int32 userId)
{
	Sync::RWMutexUsage mutUsage(this->userMut, false);
	return this->userMap.Get(userId);
}

Bool SSWR::SMonitor::SMonitorSvrCore::UserAssign(Int32 userId, NN<Data::ArrayList<Int64>> devIdList)
{
	Bool valid;

	NN<WebUser> user;
	Bool succ = false;
	Sync::RWMutexUsage mutUsage(this->userMut, false);
	if (!this->userMap.Get(userId).SetTo(user))
	{
		return false;
	}
	mutUsage.EndUse();
	Int64 cliId;
	UOSInt i;
	UOSInt j;
	i = devIdList->GetCount();
	valid = true;
	mutUsage.ReplaceMutex(this->devMut, false);
	while (i-- > 0)
	{
		if (this->devMap.Get(devIdList->GetItem(i)).IsNull())
		{
			valid = false;
			break;
		}
	}
	mutUsage.EndUse();
	if (!valid)
	{
		return false;
	}

	Sync::MutexUsage dbMutUsage;
	NN<DB::DBTool> db;
	if (this->UseDB(dbMutUsage).SetTo(db))
	{
		DB::SQLBuilder sql(db);
		sql.AppendCmdC(CSTR("delete from webuser_device where webuser_id = "));
		sql.AppendInt32(user->userId);
		if (db->ExecuteNonQuery(sql.ToCString()) < 0)
		{
			return false;
		}
		succ = true;
		mutUsage.ReplaceMutex(user->mut, true);
		user->devMap.Clear();
		i = 0;
		j = devIdList->GetCount();
		while (i < j)
		{
			sql.Clear();
			cliId = devIdList->GetItem(i);
			sql.AppendCmdC(CSTR("insert into webuser_device (webuser_id, device_id) values ("));
			sql.AppendInt32(user->userId);
			sql.AppendCmdC(CSTR(", "));
			sql.AppendInt64(cliId);
			sql.AppendCmdC(CSTR(")"));
			if (db->ExecuteNonQuery(sql.ToCString()) < 0)
			{
				succ = false;
				break;
			}
			else
			{
				Sync::RWMutexUsage devMutUsage(this->devMut, false);
				NN<DeviceInfo> dev;
				if (this->devMap.Get(cliId).SetTo(dev))
				{
					user->devMap.Put(cliId, dev);
				}
			}

			i++;
		}
	}
	return succ;
}

Bool SSWR::SMonitor::SMonitorSvrCore::SendCapturePhoto(Int64 cliId)
{
	NN<DeviceInfo> dev;
	Bool succ = false;
	Sync::RWMutexUsage mutUsage(this->devMut, false);
	if (this->devMap.Get(cliId).SetTo(dev))
	{
		mutUsage.ReplaceMutex(dev->mut, false);
		NN<IO::Stream> cli;
		if (cli.Set(dev->stm))
		{
			this->TCPSendCapturePhoto(cli);
			succ = true;
		}
		mutUsage.EndUse();
	}
	return succ;
}

void SSWR::SMonitor::SMonitorSvrCore::LogRequest(NN<Net::WebServer::IWebRequest> req)
{
	NN<Text::String> s;
	if (req->GetSHeader(CSTR("User-Agent")).SetTo(s))
	{
		this->UserAgentLog(s->v, s->leng);
	}

	if (req->GetSHeader(CSTR("Referer")).SetTo(s))
	{
		this->RefererLog(s->v, s->leng);
	}
}

void SSWR::SMonitor::SMonitorSvrCore::UserAgentLog(UnsafeArray<const UTF8Char> userAgent, UOSInt len)
{
	this->uaLog.LogStr(userAgent, len);
}

void SSWR::SMonitor::SMonitorSvrCore::UserAgentStore()
{
	if (this->uaLog.IsModified())
	{
		UTF8Char sbuff[512];
		UnsafeArray<UTF8Char> sptr;
		sptr = IO::Path::GetProcessFileName(sbuff).Or(sbuff);
		sptr = IO::Path::AppendPath(sbuff, sptr, CSTR("UserAgent.txt"));
		IO::FileStream fs(CSTRP(sbuff, sptr), IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
		Text::UTF8Writer writer(fs);
		this->uaLog.WriteLogs(&writer);
	}
}

void SSWR::SMonitor::SMonitorSvrCore::RefererLog(UnsafeArray<const UTF8Char> referer, UOSInt len)
{
	if (Text::StrStartsWithC(referer, len, UTF8STRC("http://sswroom.no-ip.org")))
	{
		return;
	}
	else if (Text::StrStartsWithC(referer, len, UTF8STRC("http://192.168.0.15")))
	{
		return;
	}
	this->refererLog.LogStr(referer, len);
}

void SSWR::SMonitor::SMonitorSvrCore::RefererStore()
{
	if (this->refererLog.IsModified())
	{
		UTF8Char sbuff[512];
		UnsafeArray<UTF8Char> sptr;
		sptr = IO::Path::GetProcessFileName(sbuff).Or(sbuff);
		sptr = IO::Path::AppendPath(sbuff, sptr, CSTR("Referer.txt"));
		IO::FileStream fs(CSTRP(sbuff, sptr), IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
		Text::UTF8Writer writer(fs);
		this->refererLog.WriteLogs(&writer);
	}
}
