#include "Stdafx.h"
#include "Data/ByteTool.h"
#include "Net/ARPHandler.h"
#include "Net/ARPInfo.h"
#include "Net/ConnectionInfo.h"
#include "Net/MACInfo.h"
#include "SSWR/AVIRead/AVIRARPScanForm.h"
#include "Sync/MutexUsage.h"
#include "Sync/Thread.h"
#include "UI/MessageDialog.h"

//#include <stdio.h>

void __stdcall SSWR::AVIRead::AVIRARPScanForm::OnARPHandler(const UInt8 *hwAddr, UInt32 ipAddr, void *userObj)
{
	SSWR::AVIRead::AVIRARPScanForm *me = (SSWR::AVIRead::AVIRARPScanForm *)userObj;
	SSWR::AVIRead::AVIRARPScanForm::IPMapInfo *ipInfo;
	Sync::MutexUsage mutUsage(me->arpMut);
	ipInfo = me->arpMap->Get(ipAddr);
	if (ipInfo == 0)
	{
		ipInfo = MemAlloc(SSWR::AVIRead::AVIRARPScanForm::IPMapInfo, 1);
		MemCopyNO(ipInfo->hwAddr, hwAddr, 6);
		ipInfo->ipAddr = ipAddr;
		me->arpMap->Put(ipInfo->ipAddr, ipInfo);
		me->arpUpdated = true;
	}
	mutUsage.EndUse();

/*	Text::StringBuilderUTF8 sb;
	UTF8Char sbuff[64];
	sb.ClearStr();
	sb.Append((const UTF8Char*)"ARP Reply: HW Addr = ");
	sb.AppendHexBuff(hwAddr, 6, ':', Text::LBT_NONE);
	sb.Append((const UTF8Char*)", IP = ");
	Net::SocketUtil::GetIPv4Name(sbuff, ipAddr);
	sb.Append(sbuff);
	printf("%s\r\n", sb.ToString());*/
}

void __stdcall SSWR::AVIRead::AVIRARPScanForm::OnTimerTick(void *userObj)
{
	SSWR::AVIRead::AVIRARPScanForm *me = (SSWR::AVIRead::AVIRARPScanForm *)userObj;
	if (me->arpUpdated)
	{
		me->arpUpdated = false;
		me->UpdateARPList();
	}
}

void __stdcall SSWR::AVIRead::AVIRARPScanForm::OnScanClicked(void *userObj)
{
	SSWR::AVIRead::AVIRARPScanForm *me = (SSWR::AVIRead::AVIRARPScanForm *)userObj;
	SSWR::AVIRead::AVIRARPScanForm::AdapterInfo *adapter;
	SSWR::AVIRead::AVIRARPScanForm::IPMapInfo *ipInfo;
	adapter = (SSWR::AVIRead::AVIRARPScanForm::AdapterInfo*)me->cboAdapter->GetSelectedItem();
	if (adapter)
	{
		UInt8 buff[4];
		WriteNInt32(buff, adapter->ipAddr);
		if (buff[0] == 192 && buff[1] == 168)
		{
		}
		else
		{
			UI::MessageDialog::ShowDialog((const UTF8Char*)"To many ip address", (const UTF8Char*)"ARP Scan", me);
			return;
		}
		Net::ARPHandler *arp;
		NEW_CLASS(arp, Net::ARPHandler(me->core->GetSocketFactory(), adapter->ifName, adapter->hwAddr, adapter->ipAddr, OnARPHandler, me, 1));
		if (arp->IsError())
		{
			UI::MessageDialog::ShowDialog((const UTF8Char*)"Error in listening to ARP data", (const UTF8Char*)"ARP Scan", me);
		}
		else
		{
			buff[3] = 1;
			Sync::MutexUsage mutUsage(me->arpMut);
			while (buff[3] < 255)
			{
				ipInfo = me->arpMap->Get(ReadNInt32(buff));
				if (ipInfo == 0)
				{
					arp->MakeRequest(ReadNInt32(buff));
				}
				buff[3]++;
			}
			mutUsage.EndUse();
			Sync::Thread::Sleep(3000);
		}
		DEL_CLASS(arp);
	}
}

void SSWR::AVIRead::AVIRARPScanForm::UpdateARPList()
{
	OSInt i;
	OSInt j;
	OSInt k;
	UTF8Char sbuff[64];

	const Net::MACInfo::MACEntry *macEntry;
	SSWR::AVIRead::AVIRARPScanForm::IPMapInfo *ipInfo;
	Sync::MutexUsage mutUsage(this->arpMut);
	Data::ArrayList<SSWR::AVIRead::AVIRARPScanForm::IPMapInfo *> *arpList = this->arpMap->GetValues();
	this->lvARP->ClearItems();
	i = 0;
	j = arpList->GetCount();
	while (i < j)
	{
		ipInfo = arpList->GetItem(i);
		Net::SocketUtil::GetIPv4Name(sbuff, ipInfo->ipAddr);
		k = this->lvARP->AddItem(sbuff, ipInfo);
		Text::StrHexBytes(sbuff, ipInfo->hwAddr, 6, ':');
		this->lvARP->SetSubItem(k, 1, sbuff);
		macEntry = Net::MACInfo::GetMACInfoBuff(ipInfo->hwAddr);
		if (macEntry)
		{
			this->lvARP->SetSubItem(k, 2, (const UTF8Char*)macEntry->name);
		}
		i++;
	}
	mutUsage.EndUse();
}

SSWR::AVIRead::AVIRARPScanForm::AVIRARPScanForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core) : UI::GUIForm(parent, 1024, 768, ui)
{
	this->SetFont(0, 8.25, false);
	this->SetText((const UTF8Char*)"ARP Scan");

	this->core = core;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	NEW_CLASS(this->pnlCtrl, UI::GUIPanel(ui, this));
	this->pnlCtrl->SetRect(0, 0, 100, 31, false);
	this->pnlCtrl->SetDockType(UI::GUIControl::DOCK_TOP);
	NEW_CLASS(this->lblAdapter, UI::GUILabel(ui, this->pnlCtrl, (const UTF8Char*)"Adapter"));
	this->lblAdapter->SetRect(4, 4, 100, 23, false);
	NEW_CLASS(this->cboAdapter, UI::GUIComboBox(ui, this->pnlCtrl, false));
	this->cboAdapter->SetRect(104, 4, 150, 23, false);
	NEW_CLASS(this->btnScan, UI::GUIButton(ui, this->pnlCtrl, (const UTF8Char*)"Scan"));
	this->btnScan->SetRect(254, 4, 75, 23, false);
	this->btnScan->HandleButtonClick(OnScanClicked, this);
	NEW_CLASS(this->lvARP, UI::GUIListView(ui, this, UI::GUIListView::LVSTYLE_TABLE, 3));
	this->lvARP->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lvARP->SetFullRowSelect(true);
	this->lvARP->SetShowGrid(true);
	this->lvARP->AddColumn((const UTF8Char*)"IP", 100);
	this->lvARP->AddColumn((const UTF8Char*)"HW Addr", 150);
	this->lvARP->AddColumn((const UTF8Char*)"Vendor", 300);

	NEW_CLASS(this->arpMut, Sync::Mutex());
	NEW_CLASS(this->arpMap, Data::Integer32Map<SSWR::AVIRead::AVIRARPScanForm::IPMapInfo*>());
	this->arpUpdated = false;
	NEW_CLASS(this->adapters, Data::ArrayList<SSWR::AVIRead::AVIRARPScanForm::AdapterInfo*>());

	OSInt i;
	OSInt j;
	SSWR::AVIRead::AVIRARPScanForm::IPMapInfo *ipInfo;
	Data::ArrayList<Net::ARPInfo *> arpList;
	Net::ARPInfo::ARPType arpType;
	Net::ARPInfo *arp;
	Net::ARPInfo::GetARPInfoList(&arpList);
	i = 0;
	j = arpList.GetCount();
	while (i < j)
	{
		arp = arpList.GetItem(i);
		arpType = arp->GetARPType();
		if (arpType == Net::ARPInfo::ARPT_STATIC || arpType == Net::ARPInfo::ARPT_DYNAMIC)
		{
			ipInfo = MemAlloc(SSWR::AVIRead::AVIRARPScanForm::IPMapInfo, 1);
			ipInfo->ipAddr = arp->GetIPAddress();
			arp->GetPhysicalAddr(ipInfo->hwAddr);
			ipInfo = this->arpMap->Put(ipInfo->ipAddr, ipInfo);
			if (ipInfo)
			{
				MemFree(ipInfo);
			}
		}

		DEL_CLASS(arp);
		i++;
	}
	this->UpdateARPList();

	Data::ArrayList<Net::ConnectionInfo*> connInfoList;
	Net::ConnectionInfo *connInfo;
	SSWR::AVIRead::AVIRARPScanForm::AdapterInfo *adapter;
	Net::SocketUtil::IPType ipType;
	UInt8 hwAddr[32];
	UTF8Char sbuff[128];
	OSInt k;
	UInt32 ip;
	this->core->GetSocketFactory()->GetConnInfoList(&connInfoList);
	i = 0;
	j = connInfoList.GetCount();
	while (i < j)
	{
		connInfo = connInfoList.GetItem(i);
		k = 0;
		while (true)
		{
			ip = connInfo->GetIPAddress(k);
			if (ip == 0)
				break;
			ipType = Net::SocketUtil::GetIPv4Type(ip);
			if (ipType == Net::SocketUtil::IT_PRIVATE)
			{
				if (connInfo->GetPhysicalAddress(hwAddr, 32) == 6)
				{
					adapter = MemAlloc(SSWR::AVIRead::AVIRARPScanForm::AdapterInfo, 1);
					sbuff[0] = 0;
					connInfo->GetName(sbuff);
					adapter->ifName = Text::StrCopyNew(sbuff);
					adapter->ipAddr = ip;
					MemCopyNO(adapter->hwAddr, hwAddr, 6);
					this->adapters->Add(adapter);
					Net::SocketUtil::GetIPv4Name(sbuff, ip);
					this->cboAdapter->AddItem(sbuff, adapter);
				}
			}
			k++;
		}
		DEL_CLASS(connInfo);
		i++;
	}
	if (this->adapters->GetCount() > 0)
	{
		this->cboAdapter->SetSelectedIndex(0);
	}

	this->AddTimer(1000, OnTimerTick, this);
}

SSWR::AVIRead::AVIRARPScanForm::~AVIRARPScanForm()
{
	OSInt i;
	SSWR::AVIRead::AVIRARPScanForm::AdapterInfo *adapter;
	SSWR::AVIRead::AVIRARPScanForm::IPMapInfo *ipInfo;
	Data::ArrayList<SSWR::AVIRead::AVIRARPScanForm::IPMapInfo*> *ipList;
	i = this->adapters->GetCount();
	while (i-- > 0)
	{
		adapter = this->adapters->GetItem(i);
		Text::StrDelNew(adapter->ifName);
		MemFree(adapter);
	}
	DEL_CLASS(this->adapters);

	ipList = this->arpMap->GetValues();
	i = ipList->GetCount();
	while (i-- > 0)
	{
		ipInfo = ipList->GetItem(i);
		MemFree(ipInfo);
	}
	DEL_CLASS(this->arpMap);
	DEL_CLASS(this->arpMut);
}

void SSWR::AVIRead::AVIRARPScanForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
