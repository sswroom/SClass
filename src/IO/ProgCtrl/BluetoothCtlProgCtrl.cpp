#include "Stdafx.h"
#include "Data/ByteTool.h"
#include "IO/ProgCtrl/BluetoothCtlProgCtrl.h"
#include "Sync/SimpleThread.h"
#include "Sync/ThreadUtil.h"
#include "Text/StringBuilderC.h"
#include "Text/StringBuilderUTF8.h"

#include <stdio.h>

UInt32 __stdcall IO::ProgCtrl::BluetoothCtlProgCtrl::ReadThread(void *obj)
{
	IO::ProgCtrl::BluetoothCtlProgCtrl *me = (IO::ProgCtrl::BluetoothCtlProgCtrl*)obj;
	UInt8 buff[512];
	UOSInt readSize;
	Text::PString sarr[2];
	UOSInt i;
	IO::BTScanLog::ScanRecord3 *dev;

	me->threadRunning = true;
	{
		Text::StringBuilderUTF8 sb;
		Text::StringBuilderUTF8 sbBuff;
		while (!me->threadToStop)
		{
			readSize = me->prog->Read(BYTEARR(buff));
/*			sb.ClearStr();
			sb.AppendHexBuff(buff, readSize, ' ', Text::LineBreakType::CRLF);
			printf("Read Size: %d\r\n", (Int32)readSize);
			printf("%s\r\n", sb.ToString());*/

			sbBuff.AppendC(buff, readSize);
			sbBuff.RemoveANSIEscapes();
			sarr[1] = sbBuff;
			while ((i = Text::StrSplitLineP(sarr, 2, sarr[1])) == 2)
			{
				Sync::MutexUsage mutUsage(&me->lastCmdMut);
				if (me->lastCmd && me->lastCmd->Equals(sarr[0].v, sarr[0].leng))
				{
					me->cmdReady = true;
				}
				else
				{
					mutUsage.EndUse();
					if (sarr[0].v[0] == 0 || sarr[0].v[0] == ' ' || sarr[0].v[0] == '\t')
					{

					}
					else if (Text::StrEqualsC(sarr[0].v, sarr[0].leng, UTF8STRC("Agent registered")))
					{
						me->agentOn = true;
					}
					else if (Text::StrEqualsC(sarr[0].v, sarr[0].leng, UTF8STRC("Agent is already registered")))
					{
						me->agentOn = true;
					}
					else if (Text::StrEqualsC(sarr[0].v, sarr[0].leng, UTF8STRC("Agent unregistered")))
					{
						me->agentOn = false;
					}
					else if (Text::StrEqualsC(sarr[0].v, sarr[0].leng, UTF8STRC("No agent is registered")))
					{
						me->agentOn = false;
					}
					else if (Text::StrEqualsC(sarr[0].v, sarr[0].leng, UTF8STRC("Discovery started")))
					{
						me->scanOn = true;
					}
					else if (Text::StrEqualsC(sarr[0].v, sarr[0].leng, UTF8STRC("Discovery stopped")))
					{
						me->scanOn = false;
					}
					else if (Text::StrStartsWithC(sarr[0].v, sarr[0].leng, UTF8STRC("[CHG] Controller ")))
					{
						//[CHG] Controller 04:EA:56:8E:0C:46 Discovering: yes
						if (Text::StrStartsWithC(&sarr[0].v[35], sarr[0].leng - 35, UTF8STRC("Discovering: ")))
						{
							
						}
						//[CHG] Controller 04:EA:56:8E:0C:46 Class: 0x0000010c
						else if (Text::StrStartsWithC(&sarr[0].v[35], sarr[0].leng - 35, UTF8STRC("Class: ")))
						{
							
						}
						//[CHG] Controller 04:EA:56:8E:0C:46 UUIDs: 00001112-0000-1000-8000-00805f9b34fb
						else if (Text::StrStartsWithC(&sarr[0].v[35], sarr[0].leng - 35, UTF8STRC("UUIDs: ")))
						{
							
						}
						//[CHG] Controller 04:EA:56:8E:0C:46 Alias: BlueZ 5.50
						else if (Text::StrStartsWithC(&sarr[0].v[35], sarr[0].leng - 35, UTF8STRC("Alias: ")))
						{
							
						}
						//[CHG] Controller 04:EA:56:8E:0C:46 Name: debiandell
						else if (Text::StrStartsWithC(&sarr[0].v[35], sarr[0].leng - 35, UTF8STRC("Name: ")))
						{
							
						}
						else if (Text::StrStartsWithC(&sarr[0].v[35], sarr[0].leng - 35, UTF8STRC("Pairable: ")))
						{
							
						}
						else
						{
							printf("Unknown Line: %s\r\n", sarr[0].v);
						}
					}
					else if (Text::StrStartsWithC(sarr[0].v, sarr[0].leng, UTF8STRC("[NEW] Device ")))
					{
						//[NEW] Device 51:87:A3:4B:EA:4E Sabbat E16
						sarr[0].v[30] = 0;
						dev = me->DeviceGetByStr(&sarr[0].v[13], 30 - 13);
						if (dev)
						{
							SDEL_STRING(dev->name);
							dev->name = Text::String::New(&sarr[0].v[31], sarr[0].leng - 31).Ptr();
							dev->inRange = true;
							dev->lastSeenTime = Data::DateTimeUtil::GetCurrTimeMillis();
							if (me->recHdlr) me->recHdlr(dev, UT_NEW_DEVICE, me->recHdlrObj);
						}
						else
						{
							printf("Error in getting device \"%s\"\r\n", &sarr[0].v[13]);
						}
					}
					else if (Text::StrStartsWithC(sarr[0].v, sarr[0].leng, UTF8STRC("[NEW] Primary Service")))
					{

					}
					else if (Text::StrStartsWithC(sarr[0].v, sarr[0].leng, UTF8STRC("[NEW] Characteristic")))
					{
						
					}
					else if (Text::StrStartsWithC(sarr[0].v, sarr[0].leng, UTF8STRC("[NEW] Descriptor")))
					{
						
					}
					else if (Text::StrStartsWithC(sarr[0].v, sarr[0].leng, UTF8STRC("[CHG] Device ")))
					{
						//[CHG] Device ED:8E:0E:77:6E:15 ManufacturerData Key: 0x3512
						sarr[0].v[30] = 0;
						dev = me->DeviceGetByStr(&sarr[0].v[13], 30 - 13);
						if (dev)
						{
							dev->inRange = true;
							dev->lastSeenTime = Data::DateTimeUtil::GetCurrTimeMillis();
							//[CHG] Device ED:8E:0E:77:6E:15 Connected: yes
							//[CHG] Device ED:8E:0E:77:6E:15 Connected: no
							if (Text::StrStartsWithC(&sarr[0].v[31], sarr[0].leng - 31, UTF8STRC("Connected: ")))
							{
								dev->connected = Text::StrEqualsC(&sarr[0].v[42], sarr[0].leng - 42, UTF8STRC("yes"));
								if (me->recHdlr) me->recHdlr(dev, UT_CONNECT, me->recHdlrObj);
							}
							//[CHG] Device 19:08:19:32:09:3A Name: Ble T70939
							else if (Text::StrStartsWithC(&sarr[0].v[31], sarr[0].leng - 31, UTF8STRC("Name: ")))
							{
								SDEL_STRING(dev->name);
								dev->name = Text::String::New(&sarr[0].v[37], sarr[0].leng - 37).Ptr();
								if (me->recHdlr) me->recHdlr(dev, UT_NAME, me->recHdlrObj);
							}
							//[CHG] Device 19:08:19:32:09:3A Alias: Ble T70939
							else if (Text::StrStartsWithC(&sarr[0].v[31], sarr[0].leng - 31, UTF8STRC("Alias: ")))
							{
								SDEL_STRING(dev->name);
								dev->name = Text::String::New(&sarr[0].v[38], sarr[0].leng - 38).Ptr();
								if (me->recHdlr) me->recHdlr(dev, UT_NAME, me->recHdlrObj);
							}
							//[CHG] Device ED:8E:0E:77:6E:15 RSSI: -64
							else if (Text::StrStartsWithC(&sarr[0].v[31], sarr[0].leng - 31, UTF8STRC("RSSI: ")))
							{
								dev->rssi = (Int8)Text::StrToInt32(&sarr[0].v[37]);
								if (me->recHdlr) me->recHdlr(dev, UT_RSSI, me->recHdlrObj);
							}
							//[CHG] Device 90:DD:5D:C2:E6:DA TxPower: 12
							else if (Text::StrStartsWithC(&sarr[0].v[31], sarr[0].leng - 31, UTF8STRC("TxPower: ")))
							{
								dev->txPower = (Int8)Text::StrToInt32(&sarr[0].v[40]);
								if (me->recHdlr) me->recHdlr(dev, UT_TXPOWER, me->recHdlrObj);
							}
							//[CHG] Device ED:8E:0E:77:6E:15 ManufacturerData Key: 0x3512
							else if (Text::StrStartsWithC(&sarr[0].v[31], sarr[0].leng - 31, UTF8STRC("ManufacturerData Key: ")))
							{
								UInt16 key;
								if (Text::StrToUInt16(&sarr[0].v[53], &key))
								{
									dev->company = key;
									if (me->recHdlr) me->recHdlr(dev, UT_COMPANY, me->recHdlrObj);
								}
							}
							//[CHG] Device E8:50:BD:A8:07:D4 ManufacturerData Value:
							else if (Text::StrStartsWithC(&sarr[0].v[31], sarr[0].leng - 31, UTF8STRC("ManufacturerData Value:")))
							{

							}
							//[CHG] Device E8:50:BD:A8:07:D4 UUIDs: 0000180a-0000-1000-8000-00805f9b34fb
							else if (Text::StrStartsWithC(&sarr[0].v[31], sarr[0].leng - 31, UTF8STRC("UUIDs: ")))
							{

							}
							//[CHG] Device ED:8E:0E:77:6E:15 ServicesResolved: yes
							else if (Text::StrStartsWithC(&sarr[0].v[31], sarr[0].leng - 31, UTF8STRC("ServicesResolved: ")))
							{

							}
							//[CHG] Device 00:1C:88:30:A0:B9 LegacyPairing: yes
							else if (Text::StrStartsWithC(&sarr[0].v[31], sarr[0].leng - 31, UTF8STRC("LegacyPairing: ")))
							{

							}
							//[CHG] Device D8:16:35:2D:60:F3 Appearance: 0x00c0
							else if (Text::StrStartsWithC(&sarr[0].v[31], sarr[0].leng - 31, UTF8STRC("Appearance: ")))
							{

							}
							//[CHG] Device A4:30:7A:91:5E:EF Icon: audio-card
							else if (Text::StrStartsWithC(&sarr[0].v[31], sarr[0].leng - 31, UTF8STRC("Icon: ")))
							{

							}
							//[CHG] Device FF:02:2F:D2:23:49 ServiceData Key: 000002a0-0000-1000-8000-00805f9b34fb
							else if (Text::StrStartsWithC(&sarr[0].v[31], sarr[0].leng - 31, UTF8STRC("ServiceData Key: ")))
							{

							}
							//[CHG] Device C9:CE:83:47:9A:0E ServiceData Value:
							else if (Text::StrStartsWithC(&sarr[0].v[31], sarr[0].leng - 31, UTF8STRC("ServiceData Value:")))
							{

							}
							//[CHG] Device 04:23:09:C3:75:46 Class: 0x00040424
							else if (Text::StrStartsWithC(&sarr[0].v[31], sarr[0].leng - 31, UTF8STRC("Class: ")))
							{

							}
							//[CHG] Device 00:7C:2D:08:6B:8D Modalias: bluetooth:v04E8p8080d0000
							else if (Text::StrStartsWithC(&sarr[0].v[31], sarr[0].leng - 31, UTF8STRC("Modalias: ")))
							{

							}
							//[CHG] Device 10:00:18:28:57:54 RSSI is nil
							else if (Text::StrStartsWithC(&sarr[0].v[31], sarr[0].leng - 31, UTF8STRC("RSSI is nil")))
							{

							}
							//[CHG] Device 68:BF:A1:3A:57:F1 TxPower is nil
							else if (Text::StrStartsWithC(&sarr[0].v[31], sarr[0].leng - 31, UTF8STRC("TxPower is nil")))
							{

							}
							else
							{
								sarr[0].v[30] = ' ';
								printf("Unknown Line: %s\r\n", sarr[0].v);
							}
						}
						else
						{
							printf("Error in getting device \"%s\"\r\n", &sarr[0].v[13]);
						}
					}
					else if (Text::StrStartsWithC(sarr[0].v, sarr[0].leng, UTF8STRC("[DEL] Device ")))
					{
						//[DEL] Device ED:8E:0E:77:6D:15 RAPOO BT4.0 MS
						sarr[0].v[30] = 0;
						dev = me->DeviceGetByStr(&sarr[0].v[13], 30 - 13);
						if (dev)
						{
							dev->inRange = false;
							if (me->recHdlr) me->recHdlr(dev, UT_OTHER, me->recHdlrObj);
						}
						else
						{
							printf("Error in getting device \"%s\"\r\n", &sarr[0].v[13]);
						}
					}
					else if (sarr[0].v[0] == '[' && Text::StrIndexOfC(sarr[0].v, sarr[0].leng, UTF8STRC("]# ")) != INVALID_INDEX)
					{
						if (!me->cmdReady)
						{
							//printf("cmdReady = true\r\n");
						}
						me->cmdReady = true;
					}
					else
					{
						printf("Unknown Line: %s\r\n", sarr[0].v);
					}
				}
			}

			if (sarr[0].v[0] == '[' && Text::StrIndexOfC(sarr[0].v, sarr[0].leng, UTF8STRC("]# ")) != INVALID_INDEX)
			{
				if (!me->cmdReady)
				{
					//printf("cmdReady = true\r\n");
				}
				me->cmdReady = true;
			}
			sbBuff.SetSubstr((UOSInt)(sarr[0].v - sb.ToString()));
		}
	}
	me->threadRunning = false;
	return 0;
}
void IO::ProgCtrl::BluetoothCtlProgCtrl::SendCmd(const UTF8Char *cmd, UOSInt cmdLen)
{
	if (!this->threadRunning)
	{
		return;
	}
	//printf("Sending cmd %s\r\n", cmd);
	UInt8 sbuff[256];

	this->WaitForCmdReady();
	//printf("cmdReady = false\r\n");
	this->cmdReady = false;
	{
		Sync::MutexUsage mutUsage(&this->lastCmdMut);
		SDEL_STRING(this->lastCmd);
		this->lastCmd = Text::String::New(cmd, cmdLen).Ptr();
	}
	if (cmdLen < 255)
	{
		UTF8Char *sptr = Text::StrConcatC(sbuff, cmd, cmdLen);
		sptr[0] = '\r';
//		sptr[1] = '\n';
		this->prog->Write(sbuff, cmdLen + 1);
	}
	else
	{
		Text::StringBuilderUTF8 sb;
		sb.AppendC(cmd, cmdLen);
		sb.AppendUTF8Char('\r');
//		sb.AppendUTF8Char('\n');
		this->prog->Write((UInt8*)sb.ToString(), sb.GetLength());
	}
}

IO::BTScanLog::ScanRecord3 *IO::ProgCtrl::BluetoothCtlProgCtrl::DeviceGetByStr(const UTF8Char *s, UOSInt len)
{
	UTF8Char sbuff[18];
	UTF8Char *sarr[7];
	UInt8 macBuff[16];
	if (len != 17)
	{
		return 0;
	}
	macBuff[0] = 0;
	macBuff[1] = 0;
	Text::StrConcatC(sbuff, s, len);
	if (Text::StrSplit(sarr, 7, sbuff, ':') != 6)
	{
		return 0;
	}
	macBuff[2] = Text::StrHex2UInt8C(sarr[0]);
	macBuff[3] = Text::StrHex2UInt8C(sarr[1]);
	macBuff[4] = Text::StrHex2UInt8C(sarr[2]);
	macBuff[5] = Text::StrHex2UInt8C(sarr[3]);
	macBuff[6] = Text::StrHex2UInt8C(sarr[4]);
	macBuff[7] = Text::StrHex2UInt8C(sarr[5]);
	UInt64 macInt = ReadMUInt64(macBuff);
	IO::BTScanLog::ScanRecord3 *dev;
	Sync::MutexUsage mutUsage(&this->devMut);
	dev = this->devMap.Get(macInt);
	if (dev)
		return dev;
	dev = MemAlloc(IO::BTScanLog::ScanRecord3, 1);
	MemClear(dev, sizeof(IO::BTScanLog::ScanRecord3));
	dev->mac[0] = macBuff[2];
	dev->mac[1] = macBuff[3];
	dev->mac[2] = macBuff[4];
	dev->mac[3] = macBuff[5];
	dev->mac[4] = macBuff[6];
	dev->mac[5] = macBuff[7];
	dev->macInt = macInt;
	dev->addrType = IO::BTScanLog::AT_UNKNOWN;
	dev->radioType = IO::BTScanLog::RT_UNKNOWN;
	dev->company = 0;
	dev->measurePower = 0;
	dev->advType = IO::BTScanLog::ADVT_UNKNOWN;
	this->devMap.Put(macInt, dev);
	return dev;
}

void IO::ProgCtrl::BluetoothCtlProgCtrl::DeviceFree(IO::BTScanLog::ScanRecord3 *dev)
{
	SDEL_STRING(dev->name);
	MemFree(dev);
}

IO::ProgCtrl::BluetoothCtlProgCtrl::BluetoothCtlProgCtrl()
{
	this->lastCmd = 0;
	this->recHdlr = 0;
	this->recHdlrObj = 0;
	this->threadRunning = false;
	this->threadToStop = false;
	this->agentOn = false;
	this->scanOn = false;
	this->cmdReady = false;
	NEW_CLASS(this->prog, Manage::ProcessExecution(CSTR("bluetoothctl")));
	if (this->prog->IsRunning())
	{
		Sync::ThreadUtil::Create(ReadThread, this);
	}
	else
	{
		this->threadToStop = true;
	}
}

IO::ProgCtrl::BluetoothCtlProgCtrl::~BluetoothCtlProgCtrl()
{
	this->Close();
	DEL_CLASS(this->prog);
	LIST_CALL_FUNC(&this->devMap, DeviceFree);
	SDEL_STRING(this->lastCmd);
}

void IO::ProgCtrl::BluetoothCtlProgCtrl::HandleRecordUpdate(RecordHandler hdlr, void *userObj)
{
	this->recHdlrObj = userObj;
	this->recHdlr = hdlr;
}

Bool IO::ProgCtrl::BluetoothCtlProgCtrl::IsScanOn()
{
	return this->scanOn;
}

void IO::ProgCtrl::BluetoothCtlProgCtrl::ScanOn()
{
	this->SendCmd(UTF8STRC("scan on"));
}

void IO::ProgCtrl::BluetoothCtlProgCtrl::ScanOff()
{
	this->SendCmd(UTF8STRC("scan off"));
}

void IO::ProgCtrl::BluetoothCtlProgCtrl::Close()
{
	this->threadToStop = true;
	if (this->prog->IsRunning())
	{
		if (this->agentOn)
		{
			this->SendCmd(UTF8STRC("agent off"));
		}
		this->SendCmd(UTF8STRC("exit"));
		Data::DateTime dt;
		dt.SetCurrTimeUTC();
		Int64 startTime = dt.ToTicks();
		while (this->prog->IsRunning())
		{
			dt.SetCurrTimeUTC();
			if (dt.ToTicks() - startTime >= 1000)
			{
				break;
			}
			Sync::SimpleThread::Sleep(1);
		}
		this->prog->Close();
	}
	while (this->threadRunning)
	{
		Sync::SimpleThread::Sleep(1);
	}
}

Bool IO::ProgCtrl::BluetoothCtlProgCtrl::SetScanMode(ScanMode scanMode)
{
	return false;
}

Bool IO::ProgCtrl::BluetoothCtlProgCtrl::WaitForCmdReady()
{
	if ((this->threadRunning || !this->threadToStop) && !this->cmdReady)
	{
		Int64 stTime;
		Data::DateTime dt;
		dt.SetCurrTimeUTC();
		stTime = dt.ToTicks();
		while (!this->cmdReady)
		{
			dt.SetCurrTimeUTC();
			if (dt.ToTicks() - stTime >= 2000)
			{
				break;
			}
			Sync::SimpleThread::Sleep(1);
		}
	}
	return this->cmdReady;
}

NotNullPtr<Data::FastMap<UInt64, IO::BTScanLog::ScanRecord3*>> IO::ProgCtrl::BluetoothCtlProgCtrl::GetPublicMap(Sync::MutexUsage *mutUsage)
{
	mutUsage->ReplaceMutex(&this->devMut);
	return this->devMap;
}

NotNullPtr<Data::FastMap<UInt64, IO::BTScanLog::ScanRecord3*>> IO::ProgCtrl::BluetoothCtlProgCtrl::GetRandomMap(Sync::MutexUsage *mutUsage)
{
	mutUsage->ReplaceMutex(&this->devMut);
	return this->devMap;
}
