#include "Stdafx.h"
#include "Data/ByteTool.h"
#include "Net/ARPInfo.h"
#include "Net/ConnectionInfo.h"
#include "Net/MACInfo.h"
#include "SSWR/AVIRead/AVIRIPScanForm.h"
#include "Sync/MutexUsage.h"
#include "Sync/Thread.h"
#include "Text/MyStringFloat.h"
#include "Text/StringBuilderUTF8.h"
#include "UI/MessageDialog.h"

#define THREADLEV 5

typedef struct
{
	SSWR::AVIRead::AVIRIPScanForm *me;
	UInt32 ipStart;
	UInt32 ipEnd;
	Bool ended;
	Sync::Event *evt;
} PingStatus;

void SSWR::AVIRead::AVIRIPScanForm::ICMPChecksum(UInt8 *buff, OSInt buffSize)
{
	UInt8 *oriBuff = buff;
    UInt32 sum = 0xffff;
    while (buffSize > 1)
	{
        sum += ReadUInt16(buff);
        buff += 2;
        buffSize -= 2;
    }

    if(buffSize == 1)
        sum += buff[0];

    sum = (sum & 0xffff) + (sum >> 16);
    sum = (sum & 0xffff) + (sum >> 16);

	WriteInt16(&oriBuff[2], ~sum);
}

UInt32 __stdcall SSWR::AVIRead::AVIRIPScanForm::Ping1Thread(void *userObj)
{
	PingStatus *status = (PingStatus*)userObj;
	UInt8 buff1[4];
	UInt8 buff2[4];
	ScanResult *result;
	Net::SocketUtil::AddressInfo addr;
	Int32 respTime;
	Int32 ttl;
	WriteNInt32(buff1, status->ipStart);
	WriteNInt32(buff2, status->ipEnd);

	while (buff1[3] <= buff2[3])
	{
		if (buff1[3] != 0 && buff1[3] != 255)
		{
			Net::SocketUtil::SetAddrInfoV4(&addr, ReadNInt32(buff1));
			if (status->me->sockf->IcmpSendEcho2(&addr, &respTime, &ttl))
			{
				result = MemAlloc(ScanResult, 1);
				result->ip = ReadNInt32(buff1);
				result->respTime = respTime / 1000000.0;
				result->mac[0] = 0;
				result->mac[1] = 0;
				result->mac[2] = 0;
				result->mac[3] = 0;
				result->mac[4] = 0;
				result->mac[5] = 0;
				Sync::MutexUsage mutUsage(status->me->resultMut);
				status->me->results->Put(Net::SocketUtil::IPv4ToSortable(result->ip), result);
				mutUsage.EndUse();
			}
		}
		if (buff1[3] == buff2[3])
		{
			break;
		}
		buff1[3]++;
	}
	status->ended = true;
	status->evt->Set();
	return 0;
}

UInt32 __stdcall SSWR::AVIRead::AVIRIPScanForm::Ping2Thread(void *userObj)
{
	SSWR::AVIRead::AVIRIPScanForm *me = (SSWR::AVIRead::AVIRIPScanForm*)userObj;
	UInt8 *readBuff;
	Net::SocketUtil::AddressInfo addr;
	UInt16 port;
	Net::SocketFactory::ErrorType et;
	OSInt readSize;
	ScanResult *result;
	readBuff = MemAlloc(UInt8, 4096);
	UInt8 *ipData;
	OSInt ipDataSize;
	while (!me->threadToStop)
	{
		readSize = me->sockf->UDPReceive(me->soc, readBuff, 4096, &addr, &port, &et);
		if (readSize >= 36)
		{
			if ((readBuff[0] & 0xf0) == 0x40 && readBuff[9] == 1)
			{
				if ((readBuff[0] & 0xf) <= 5)
				{
					ipData = &readBuff[20];
					ipDataSize = readSize - 20;
				}
				else
				{
					ipData = &readBuff[(readBuff[0] & 0xf) << 2];
					ipDataSize = readSize - ((readBuff[0] & 0xf) << 2);
				}

				if (ipData[0] == 0 && ipDataSize >= 8)
				{
					Sync::MutexUsage mutUsage(me->resultMut);
					result = me->results->Get(ReadMInt32(&readBuff[12]));
					if (result == 0)
					{
						result = MemAlloc(ScanResult, 1);
						result->ip = ReadNInt32(&readBuff[12]);
						result->respTime = me->clk->GetTimeDiff();;
						result->mac[0] = 0;
						result->mac[1] = 0;
						result->mac[2] = 0;
						result->mac[3] = 0;
						result->mac[4] = 0;
						result->mac[5] = 0;
						me->results->Put(ReadMInt32(&readBuff[12]), result);
					}
					mutUsage.EndUse();
				}
			}
		}
	}
	MemFree(readBuff);
	me->threadRunning = false;
	return 0;
}

void __stdcall SSWR::AVIRead::AVIRIPScanForm::OnStartClicked(void *userObj)
{
	SSWR::AVIRead::AVIRIPScanForm *me = (SSWR::AVIRead::AVIRIPScanForm*)userObj;
	UInt8 buff[8];
	UInt8 packetBuff[64];
	UTF8Char sbuff[32];
	ScanResult *result;
	OSInt i;
	OSInt j;
	UInt32 ip = (UInt32)(OSInt)me->cboIP->GetSelectedItem();
	if (ip != 0)
	{
		me->ClearResults();
		UInt32 *s;
		WriteNInt32(buff, ip);
		if (buff[0] == 192 && buff[1] == 168)
		{
		}
		else
		{
			UI::MessageDialog::ShowDialog((const UTF8Char*)"To many ip address", (const UTF8Char*)"Error", me);
			return;
		}

		if (0)
		{
		}
/*		if ((s = me->sockf->CreateRAWSocket()) != 0)
		{
			me->sockf->DestroySocket(s);
		}*/
		else if ((s = me->sockf->CreateICMPIPv4Socket(ip)) != 0)
		{
			me->threadRunning = true;
			me->threadToStop = false;
			me->soc = s;
			NEW_CLASS(me->clk, Manage::HiResClock());
			Sync::Thread::Create(Ping2Thread, me);

			Net::SocketUtil::AddressInfo addr;
			packetBuff[0] = 8; //type = Echo request
			packetBuff[1] = 0; //code = 0
			WriteNInt16(&packetBuff[2], 0); //checksum = 0;
			WriteNInt16(&packetBuff[4], 0); //id = 0;
			WriteNInt16(&packetBuff[6], 0); //seq = 0;
			MemClear(&packetBuff[8], 56);
			ICMPChecksum(packetBuff, 64);

			me->clk->Start();
			buff[3] = 1;
			while (buff[3] < 255)
			{
				Net::SocketUtil::SetAddrInfoV4(&addr, ReadNInt32(buff));
				me->sockf->SendTo(s, packetBuff, 64, &addr, 0);
				buff[3]++;
			}

			Sync::Thread::Sleep(3000);
			me->threadToStop = true;
			me->sockf->DestroySocket(s);
			while (me->threadRunning)
			{
				Sync::Thread::Sleep(1);
			}
			me->soc = 0;
			DEL_CLASS(me->clk);
			me->clk = 0;

			me->AppendMACs(ip);
		}
		else
		{
			PingStatus *status = MemAlloc(PingStatus, (1 << THREADLEV));
			NEW_CLASS(status[0].evt, Sync::Event(true, (const UTF8Char*)"SSWR.AVIRead.AVIRIPScanForm.OnStartClicked.evt"));
			i = 0;
			while (i < (1 << THREADLEV))
			{
				status[i].me = me;
				status[i].evt = status[0].evt;
				status[i].ended = false;
				buff[3] = (UInt8)(i << (8 - THREADLEV));
				status[i].ipStart = ReadNInt32(buff);
				buff[3] = buff[3] + (1 << (8 - THREADLEV)) - 1;
				status[i].ipEnd = ReadNInt32(buff);
				Sync::Thread::Create(Ping1Thread, &status[i]);
				i++;
			}

			Bool running;
			while (true)
			{
				running = false;
				i = 1 << THREADLEV;
				while (i-- > 0)
				{
					if (!status[i].ended)
					{
						running = true;
						break;
					}
				}
				if (running)
				{
					status[0].evt->Wait(1000);
				}
				else
				{
					break;
				}
			}
			DEL_CLASS(status[0].evt);
			MemFree(status);

			me->AppendMACs(ip);
		}

		me->lvIP->ClearItems();
		Data::ArrayList<ScanResult*> *resultList = me->results->GetValues();
		i = 0;
		j = resultList->GetCount();
		while (i < j)
		{
			result = resultList->GetItem(i);
			Net::SocketUtil::GetIPv4Name(sbuff, result->ip);
			me->lvIP->AddItem(sbuff, result);
			Text::StrHexBytes(sbuff, result->mac, 6, ':');
			me->lvIP->SetSubItem(i, 1, sbuff);
			me->lvIP->SetSubItem(i, 2, (const UTF8Char*)Net::MACInfo::GetMACInfoBuff(result->mac)->name);
			Text::StrDouble(sbuff, result->respTime);
			me->lvIP->SetSubItem(i, 3, sbuff);
			i++;
		}
	}
}

void __stdcall SSWR::AVIRead::AVIRIPScanForm::OnTimerTick(void *userObj)
{
//	SSWR::AVIRead::AVIRIPScanForm *me = (SSWR::AVIRead::AVIRIPScanForm*)userObj;
}

void SSWR::AVIRead::AVIRIPScanForm::AppendMACs(UInt32 ip)
{
	Data::ArrayList<Net::ARPInfo *> arpList;
	Net::ARPInfo *arp;
	UInt8 mac[6];
	UInt32 ipAddr;
	OSInt i;
	OSInt j;
	ScanResult *result;
	Net::ARPInfo::GetARPInfoList(&arpList);
	i = 0;
	j = arpList.GetCount();
	while (i < j)
	{
		arp = arpList.GetItem(i);
		ipAddr = arp->GetIPAddress();
		if (arp->GetARPType() != Net::ARPInfo::ARPT_INVALID && arp->GetPhysicalAddr(mac) == 6)
		{
			result = this->results->Get(Net::SocketUtil::IPv4ToSortable(ipAddr));
			if (result)
			{
				MemCopyNO(result->mac, mac, 6);
			}
		}
		DEL_CLASS(arp);
		i++;
	}

	Data::ArrayList<Net::ConnectionInfo *> connList;
	Net::ConnectionInfo *conn;
	this->sockf->GetConnInfoList(&connList);
	i = connList.GetCount();
	while (i-- > 0)
	{
		conn = connList.GetItem(i);
		j = 0;
		while (true)
		{
			ipAddr = conn->GetIPAddress(j);
			if (ipAddr == 0)
			{
				break;
			}
			if (ipAddr == ip)
			{
				if (conn->GetPhysicalAddress(mac, 6) == 6)
				{
					result = this->results->Get(Net::SocketUtil::IPv4ToSortable(ipAddr));
					if (result)
					{
						MemCopyNO(result->mac, mac, 6);
					}
				}
				break;
			}
			j++;
		}
		DEL_CLASS(conn);
	}
}

void SSWR::AVIRead::AVIRIPScanForm::ClearResults()
{
	Data::ArrayList<ScanResult*> *resultList = this->results->GetValues();
	ScanResult *result;
	OSInt i = resultList->GetCount();
	while (i-- > 0)
	{
		result = resultList->GetItem(i);
		MemFree(result);
	}
	this->results->Clear();
}

SSWR::AVIRead::AVIRIPScanForm::AVIRIPScanForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core) : UI::GUIForm(parent, 1024, 768, ui)
{
	this->SetFont(0, 8.25, false);
	this->SetText((const UTF8Char*)"IP Scan");

	this->core = core;
	this->sockf = core->GetSocketFactory();
	NEW_CLASS(this->resultMut, Sync::Mutex());
	NEW_CLASS(this->results, Data::Integer32Map<ScanResult*>());
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	NEW_CLASS(this->pnlControl, UI::GUIPanel(ui, this));
	this->pnlControl->SetRect(0, 0, 100, 31, false);
	this->pnlControl->SetDockType(UI::GUIControl::DOCK_TOP);
	NEW_CLASS(this->lblIP, UI::GUILabel(ui, this->pnlControl, (const UTF8Char*)"IP"));
	this->lblIP->SetRect(4, 4, 100, 23, false);
	NEW_CLASS(this->cboIP, UI::GUIComboBox(ui, this->pnlControl, false));
	this->cboIP->SetRect(104, 4, 150, 23, false);
	NEW_CLASS(this->btnStart, UI::GUIButton(ui, this->pnlControl, (const UTF8Char*)"Start"));
	this->btnStart->SetRect(254, 4, 75, 23, false);
	this->btnStart->HandleButtonClick(OnStartClicked, this);
	NEW_CLASS(this->lvIP, UI::GUIListView(ui, this, UI::GUIListView::LVSTYLE_TABLE, 4));
	this->lvIP->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lvIP->SetFullRowSelect(true);
	this->lvIP->SetShowGrid(true);
	this->lvIP->AddColumn((const UTF8Char*)"IP", 80);
	this->lvIP->AddColumn((const UTF8Char*)"MAC", 120);
	this->lvIP->AddColumn((const UTF8Char*)"Vendor", 250);
	this->lvIP->AddColumn((const UTF8Char*)"Resp", 80);
	
	Data::ArrayList<Net::ConnectionInfo*> connInfoList;
	Net::ConnectionInfo *connInfo;
	UTF8Char sbuff[32];
	OSInt i;
	OSInt j;
	OSInt k;
	UInt32 ip;
	this->sockf->GetConnInfoList(&connInfoList);
	i = 0;
	j = connInfoList.GetCount();
	while (i < j)
	{
		connInfo = connInfoList.GetItem(i);
		if (connInfo->GetConnectionStatus() == Net::ConnectionInfo::CS_UP)
		{
			k = 0;
			while (true)
			{
				ip = connInfo->GetIPAddress(k);
				if (ip == 0)
					break;
				Net::SocketUtil::IPType ipType = Net::SocketUtil::GetIPv4Type(ip);
				if (ipType == Net::SocketUtil::IT_PRIVATE)
				{
					Net::SocketUtil::GetIPv4Name(sbuff, ip);
					this->cboIP->AddItem(sbuff, (void*)(OSInt)ip);
				}
				k++;
			}
		}
		DEL_CLASS(connInfo);
		i++;
	}
	if (this->cboIP->GetCount() > 0)
	{
		this->cboIP->SetSelectedIndex(0);
	}
	this->AddTimer(1000, OnTimerTick, this);
}

SSWR::AVIRead::AVIRIPScanForm::~AVIRIPScanForm()
{
	this->ClearResults();
	DEL_CLASS(this->results);
	DEL_CLASS(this->resultMut);
}

void SSWR::AVIRead::AVIRIPScanForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
