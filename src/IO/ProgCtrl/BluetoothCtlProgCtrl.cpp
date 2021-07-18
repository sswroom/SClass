#include "Stdafx.h"
#include "Data/ByteTool.h"
#include "IO/ProgCtrl/BluetoothCtlProgCtrl.h"
#include "Sync/Thread.h"
#include "Text/StringBuilderC.h"
#include "Text/StringBuilderUTF8.h"

#include <stdio.h>

/*
[NEW] Descriptor
*/

UInt32 __stdcall IO::ProgCtrl::BluetoothCtlProgCtrl::ReadThread(void *obj)
{
	IO::ProgCtrl::BluetoothCtlProgCtrl *me = (IO::ProgCtrl::BluetoothCtlProgCtrl*)obj;
	Text::StringBuilderUTF8 *sb;
	UInt8 buff[512];
	Text::StringBuilderUTF8 *sbBuff;
	UOSInt readSize;
	UTF8Char *sarr[2];
	UOSInt i;
	DeviceInfo *dev;

	me->threadRunning = true;
	NEW_CLASS(sb, Text::StringBuilderUTF8());
	NEW_CLASS(sbBuff, Text::StringBuilderUTF8());
	while (!me->threadToStop)
	{
		readSize = me->prog->Read(buff, 512);
//		sb->ClearStr();
//		sb->AppendHexBuff(buff, readSize, ' ', Text::LBT_CRLF);
//		printf("Read Size: %d\r\n", (Int32)readSize);
//		printf("%s\r\n", sb->ToString());

		sbBuff->AppendC(buff, readSize);
		sbBuff->RemoveANSIEscapes();
		sarr[1] = sbBuff->ToString();
		while ((i = Text::StrSplitLine(sarr, 2, sarr[1])) == 2)
		{
			Sync::MutexUsage mutUsage(me->lastCmdMut);
			if (me->lastCmd && Text::StrEquals(me->lastCmd, sarr[0]))
			{

			}
			else
			{
				mutUsage.EndUse();
				 if (sarr[0][0] == 0 || sarr[0][0] == ' ' || sarr[0][0] == '\t')
				{

				}
				else if (Text::StrEquals(sarr[0], (const UTF8Char*)"Agent registered"))
				{
					me->agentOn = true;
				}
				else if (Text::StrEquals(sarr[0], (const UTF8Char*)"Agent is already registered"))
				{
					me->agentOn = true;
				}
				else if (Text::StrEquals(sarr[0], (const UTF8Char*)"Agent unregistered"))
				{
					me->agentOn = false;
				}
				else if (Text::StrEquals(sarr[0], (const UTF8Char*)"No agent is registered"))
				{
					me->agentOn = false;
				}
				else if (Text::StrEquals(sarr[0], (const UTF8Char*)"Discovery started"))
				{
					me->scanOn = true;
				}
				else if (Text::StrStartsWith(sarr[0], (const UTF8Char*)"[CHG] Controller "))
				{
					//[CHG] Controller 04:EA:56:8E:0C:46 Discovering: yes
					if (Text::StrStartsWith(&sarr[0][35], (const UTF8Char*)"Discovering: "))
					{
						
					}
					//[CHG] Controller 04:EA:56:8E:0C:46 Class: 0x0000010c
					else if (Text::StrStartsWith(&sarr[0][35], (const UTF8Char*)"Class: "))
					{
						
					}
					//[CHG] Controller 04:EA:56:8E:0C:46 UUIDs: 00001112-0000-1000-8000-00805f9b34fb
					else if (Text::StrStartsWith(&sarr[0][35], (const UTF8Char*)"UUIDs: "))
					{
						
					}
					//[CHG] Controller 04:EA:56:8E:0C:46 Alias: BlueZ 5.50
					else if (Text::StrStartsWith(&sarr[0][35], (const UTF8Char*)"Alias: "))
					{
						
					}
					//[CHG] Controller 04:EA:56:8E:0C:46 Name: debiandell
					else if (Text::StrStartsWith(&sarr[0][35], (const UTF8Char*)"Name: "))
					{
						
					}
					else
					{
						printf("Unknown Line: %s\r\n", sarr[0]);
					}
				}
				else if (Text::StrStartsWith(sarr[0], (const UTF8Char*)"[NEW] Device "))
				{
					//[NEW] Device 51:87:A3:4B:EA:4E Sabbat E16
					sarr[0][30] = 0;
					dev = me->DeviceGetByStr(&sarr[0][13]);
					if (dev)
					{
						SDEL_TEXT(dev->name);
						dev->name = Text::StrCopyNew(&sarr[0][31]);
						if (me->devHdlr) me->devHdlr(dev, me->devHdlrObj);
					}
					else
					{
						printf("Error in getting device \"%s\"\r\n", &sarr[0][13]);
					}
				}
				else if (Text::StrStartsWith(sarr[0], (const UTF8Char*)"[NEW] Primary Service"))
				{

				}
				else if (Text::StrStartsWith(sarr[0], (const UTF8Char*)"[NEW] Characteristic"))
				{
					
				}
				else if (Text::StrStartsWith(sarr[0], (const UTF8Char*)"[NEW] Descriptor"))
				{
					
				}
				else if (Text::StrStartsWith(sarr[0], (const UTF8Char*)"[CHG] Device "))
				{
					//[CHG] Device ED:8E:0E:77:6E:15 ManufacturerData Key: 0x3512
					sarr[0][30] = 0;
					dev = me->DeviceGetByStr(&sarr[0][13]);
					if (dev)
					{
						//[CHG] Device ED:8E:0E:77:6E:15 Connected: yes
						//[CHG] Device ED:8E:0E:77:6E:15 Connected: no
						if (Text::StrStartsWith(&sarr[0][31], (const UTF8Char*)"Connected: "))
						{
							dev->connected = Text::StrEquals(&sarr[0][42], (const UTF8Char*)"yes");
							if (me->devHdlr) me->devHdlr(dev, me->devHdlrObj);
						}
						//[CHG] Device ED:8E:0E:77:6E:15 RSSI: -64
						else if (Text::StrStartsWith(&sarr[0][31], (const UTF8Char*)"RSSI: "))
						{
							dev->rssi = Text::StrToInt32(&sarr[0][37]);
							if (me->devHdlr) me->devHdlr(dev, me->devHdlrObj);
						}
						//[CHG] Device ED:8E:0E:77:6E:15 ManufacturerData Key: 0x3512
						else if (Text::StrStartsWith(&sarr[0][31], (const UTF8Char*)"ManufacturerData Key: "))
						{

						}
						//[CHG] Device ED:8E:0E:77:6E:15 ServicesResolved: yes
						else if (Text::StrStartsWith(&sarr[0][31], (const UTF8Char*)"ServicesResolved: "))
						{

						}
						else
						{
							sarr[0][30] = ' ';
							printf("Unknown Line: %s\r\n", sarr[0]);
						}
					}
					else
					{
						printf("Error in getting device \"%s\"\r\n", &sarr[0][13]);
					}
				}
				else if (sarr[0][0] == '[' && Text::StrIndexOf(sarr[0], (const UTF8Char*)"]# ") >= 0)
				{
					if (!me->cmdReady)
					{
						printf("cmdReady = true\r\n");
					}
					me->cmdReady = true;
				}
				else
				{
					printf("Unknown Line: %s\r\n", sarr[0]);
				}
			}
		}

		if (sarr[0][0] == '[' && Text::StrIndexOf(sarr[0], (const UTF8Char*)"]# ") >= 0)
		{
			if (!me->cmdReady)
			{
				printf("cmdReady = true\r\n");
			}
			me->cmdReady = true;
		}
		sbBuff->SetSubstr((UOSInt)(sarr[0] - sb->ToString()));
	}
	DEL_CLASS(sbBuff);
	DEL_CLASS(sb);
	me->threadRunning = false;
	return 0;
}
void IO::ProgCtrl::BluetoothCtlProgCtrl::SendCmd(const Char *cmd)
{
	if (!this->threadRunning)
	{
		return;
	}
	printf("Sending cmd %s\r\n", cmd);
	UInt8 sbuff[256];
	UOSInt cmdLen = Text::StrCharCnt(cmd);

	this->WaitForCmdReady();
	printf("cmdReady = false\r\n");
	this->cmdReady = false;
	{
		Sync::MutexUsage mutUsage(this->lastCmdMut);
		SDEL_TEXT(this->lastCmd);
		this->lastCmd = Text::StrCopyNew((const UTF8Char*)cmd);
	}
	if (cmdLen < 255)
	{
		Char *sptr = Text::StrConcat((Char*)sbuff, cmd);
		sptr[0] = '\r';
//		sptr[1] = '\n';
		this->prog->Write(sbuff, cmdLen + 1);
	}
	else
	{
		Text::StringBuilderC sb;
		sb.Append(cmd);
		sb.AppendChar('\r', 1);
//		sb.AppendChar('\n', 1);
		this->prog->Write((UInt8*)sb.ToString(), sb.GetLength());
	}
}

IO::ProgCtrl::BluetoothCtlProgCtrl::DeviceInfo *IO::ProgCtrl::BluetoothCtlProgCtrl::DeviceGetByStr(const UTF8Char *s)
{
	UTF8Char sbuff[18];
	UTF8Char *sarr[7];
	UInt8 macBuff[16];
	if (Text::StrCharCnt(s) != 17)
	{
		return 0;
	}
	macBuff[0] = 0;
	macBuff[1] = 0;
	Text::StrConcat(sbuff, s);
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
	DeviceInfo *dev;
	Sync::MutexUsage mutUsage(this->devMut);
	dev = this->devMap->Get(macInt);
	if (dev)
		return dev;
	dev = MemAlloc(DeviceInfo, 1);
	MemClear(dev, sizeof(DeviceInfo));
	dev->mac[0] = macBuff[2];
	dev->mac[1] = macBuff[3];
	dev->mac[2] = macBuff[4];
	dev->mac[3] = macBuff[5];
	dev->mac[4] = macBuff[6];
	dev->mac[5] = macBuff[7];
	dev->macInt = macInt;
	this->devMap->Put(macInt, dev);
	return dev;
}

void IO::ProgCtrl::BluetoothCtlProgCtrl::DeviceFree(DeviceInfo *dev)
{
	SDEL_TEXT(dev->name);
	MemFree(dev);
}

IO::ProgCtrl::BluetoothCtlProgCtrl::BluetoothCtlProgCtrl()
{
	NEW_CLASS(this->devMap, Data::UInt64Map<DeviceInfo*>());
	NEW_CLASS(this->devMut, Sync::Mutex());
	NEW_CLASS(this->lastCmdMut, Sync::Mutex());
	this->lastCmd = 0;
	this->devHdlr = 0;
	this->devHdlrObj = 0;
	this->threadRunning = false;
	this->threadToStop = false;
	this->agentOn = false;
	this->scanOn = false;
	this->cmdReady = false;
	NEW_CLASS(this->prog, Manage::ProcessExecution((const UTF8Char*)"bluetoothctl"));
	if (this->prog->IsRunning())
	{
		Sync::Thread::Create(ReadThread, this);
	}
	else
	{
		this->threadToStop = true;
	}
}

IO::ProgCtrl::BluetoothCtlProgCtrl::~BluetoothCtlProgCtrl()
{
	this->Exit();
	DEL_CLASS(this->prog);
	Data::ArrayList<DeviceInfo*> *devList = this->devMap->GetValues();
	LIST_CALL_FUNC(devList, DeviceFree);
	DEL_CLASS(this->devMap);
	DEL_CLASS(this->devMut);
	SDEL_TEXT(this->lastCmd);
	DEL_CLASS(this->lastCmd);
}

void IO::ProgCtrl::BluetoothCtlProgCtrl::HandleDeviceUpdate(DeviceHandler hdlr, void *userObj)
{
	this->devHdlrObj = userObj;
	this->devHdlr = hdlr;
}

void IO::ProgCtrl::BluetoothCtlProgCtrl::ScanOn()
{
	this->SendCmd("scan on");
}

void IO::ProgCtrl::BluetoothCtlProgCtrl::ScanOff()
{
	this->SendCmd("scan off");
}

void IO::ProgCtrl::BluetoothCtlProgCtrl::Exit()
{
	this->threadToStop = true;
	if (this->prog->IsRunning())
	{
		if (this->agentOn)
		{
			this->SendCmd("agent off");
		}
		this->SendCmd("exit");
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
			Sync::Thread::Sleep(1);
		}
		this->prog->Close();
	}
	while (this->threadRunning)
	{
		Sync::Thread::Sleep(1);
	}
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
			Sync::Thread::Sleep(1);
		}
	}
	return this->cmdReady;
}

Data::UInt64Map<IO::ProgCtrl::BluetoothCtlProgCtrl::DeviceInfo*> *IO::ProgCtrl::BluetoothCtlProgCtrl::GetDeviceMap(Sync::MutexUsage *mutUsage)
{
	mutUsage->ReplaceMutex(this->devMut);
	return this->devMap;
}
