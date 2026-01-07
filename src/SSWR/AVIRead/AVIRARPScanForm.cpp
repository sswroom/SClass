#include "Stdafx.h"
#include "Core/ByteTool_C.h"
#include "Net/ARPHandler.h"
#include "Net/ARPInfo.h"
#include "Net/ConnectionInfo.h"
#include "Net/MACInfo.h"
#include "SSWR/AVIRead/AVIRARPScanForm.h"
#include "Sync/MutexUsage.h"
#include "Sync/SimpleThread.h"

//#include <stdio.h>

void __stdcall SSWR::AVIRead::AVIRARPScanForm::OnARPHandler(UnsafeArray<const UInt8> hwAddr, UInt32 ipAddr, AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRARPScanForm> me = userObj.GetNN<SSWR::AVIRead::AVIRARPScanForm>();
	NN<SSWR::AVIRead::AVIRARPScanForm::IPMapInfo> ipInfo;
	Sync::MutexUsage mutUsage(me->arpMut);
	if (!me->arpMap.Get(ipAddr).SetTo(ipInfo))
	{
		ipInfo = MemAllocNN(SSWR::AVIRead::AVIRARPScanForm::IPMapInfo);
		MemCopyNO(ipInfo->hwAddr, hwAddr.Ptr(), 6);
		ipInfo->ipAddr = ipAddr;
		me->arpMap.Put(ipInfo->ipAddr, ipInfo);
		me->arpUpdated = true;
	}
	mutUsage.EndUse();

/*	Text::StringBuilderUTF8 sb;
	UTF8Char sbuff[64];
	sb.ClearStr();
	sb.AppendC(UTF8STRC("ARP Reply: HW Addr = "));
	sb.AppendHexBuff(hwAddr, 6, ':', Text::LineBreakType::None);
	sb.AppendC(UTF8STRC(", IP = "));
	Net::SocketUtil::GetIPv4Name(sbuff, ipAddr);
	sb.Append(sbuff);
	printf("%s\r\n", sb.ToString());*/
}

void __stdcall SSWR::AVIRead::AVIRARPScanForm::OnTimerTick(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRARPScanForm> me = userObj.GetNN<SSWR::AVIRead::AVIRARPScanForm>();
	if (me->arpUpdated)
	{
		me->arpUpdated = false;
		me->UpdateARPList();
	}
}

void __stdcall SSWR::AVIRead::AVIRARPScanForm::OnScanClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRARPScanForm> me = userObj.GetNN<SSWR::AVIRead::AVIRARPScanForm>();
	NN<SSWR::AVIRead::AVIRARPScanForm::AdapterInfo> adapter;
	NN<SSWR::AVIRead::AVIRARPScanForm::IPMapInfo> ipInfo;
	if (me->cboAdapter->GetSelectedItem().GetOpt<SSWR::AVIRead::AVIRARPScanForm::AdapterInfo>().SetTo(adapter))
	{
		UInt8 buff[4];
		WriteNUInt32(buff, adapter->ipAddr);
		if (buff[0] == 192 && buff[1] == 168)
		{
		}
		else
		{
			me->ui->ShowMsgOK(CSTR("To many ip address"), CSTR("ARP Scan"), me);
			return;
		}
		Net::ARPHandler *arp;
		NEW_CLASS(arp, Net::ARPHandler(me->core->GetSocketFactory(), adapter->ifName, adapter->hwAddr, adapter->ipAddr, OnARPHandler, me, 1));
		if (arp->IsError())
		{
			me->ui->ShowMsgOK(CSTR("Error in listening to ARP data"), CSTR("ARP Scan"), me);
		}
		else
		{
			buff[3] = 1;
			Sync::MutexUsage mutUsage(me->arpMut);
			while (buff[3] < 255)
			{
				if (!me->arpMap.Get(ReadNUInt32(buff)).SetTo(ipInfo))
				{
					arp->MakeRequest(ReadNUInt32(buff));
				}
				buff[3]++;
			}
			mutUsage.EndUse();
			Sync::SimpleThread::Sleep(3000);
		}
		DEL_CLASS(arp);
	}
}

void SSWR::AVIRead::AVIRARPScanForm::UpdateARPList()
{
	UOSInt i;
	UOSInt j;
	UOSInt k;
	UTF8Char sbuff[64];
	UnsafeArray<UTF8Char> sptr;

	NN<const Net::MACInfo::MACEntry> macEntry;
	NN<SSWR::AVIRead::AVIRARPScanForm::IPMapInfo> ipInfo;
	Sync::MutexUsage mutUsage(this->arpMut);
	this->lvARP->ClearItems();
	i = 0;
	j = this->arpMap.GetCount();
	while (i < j)
	{
		ipInfo = this->arpMap.GetItemNoCheck(i);
		sptr = Net::SocketUtil::GetIPv4Name(sbuff, ipInfo->ipAddr);
		k = this->lvARP->AddItem(CSTRP(sbuff, sptr), ipInfo);
		sptr = Text::StrHexBytes(sbuff, ipInfo->hwAddr, 6, ':');
		this->lvARP->SetSubItem(k, 1, CSTRP(sbuff, sptr));
		macEntry = Net::MACInfo::GetMACInfoBuff(ipInfo->hwAddr);
		this->lvARP->SetSubItem(k, 2, {macEntry->name, macEntry->nameLen});
		i++;
	}
	mutUsage.EndUse();
}

SSWR::AVIRead::AVIRARPScanForm::AVIRARPScanForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core) : UI::GUIForm(parent, 1024, 768, ui)
{
	this->SetFont(nullptr, 8.25, false);
	this->SetText(CSTR("ARP Scan"));

	this->core = core;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	this->pnlCtrl = ui->NewPanel(*this);
	this->pnlCtrl->SetRect(0, 0, 100, 31, false);
	this->pnlCtrl->SetDockType(UI::GUIControl::DOCK_TOP);
	this->lblAdapter = ui->NewLabel(this->pnlCtrl, CSTR("Adapter"));
	this->lblAdapter->SetRect(4, 4, 100, 23, false);
	this->cboAdapter = ui->NewComboBox(this->pnlCtrl, false);
	this->cboAdapter->SetRect(104, 4, 150, 23, false);
	this->btnScan = ui->NewButton(this->pnlCtrl, CSTR("Scan"));
	this->btnScan->SetRect(254, 4, 75, 23, false);
	this->btnScan->HandleButtonClick(OnScanClicked, this);
	this->lvARP = ui->NewListView(*this, UI::ListViewStyle::Table, 3);
	this->lvARP->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lvARP->SetFullRowSelect(true);
	this->lvARP->SetShowGrid(true);
	this->lvARP->AddColumn(CSTR("IP"), 100);
	this->lvARP->AddColumn(CSTR("HW Addr"), 150);
	this->lvARP->AddColumn(CSTR("Vendor"), 300);

	this->arpUpdated = false;

	UOSInt i;
	UOSInt j;
	NN<SSWR::AVIRead::AVIRARPScanForm::IPMapInfo> ipInfo;
	Data::ArrayListNN<Net::ARPInfo> arpList;
	Net::ARPInfo::ARPType arpType;
	NN<Net::ARPInfo> arp;
	Net::ARPInfo::GetARPInfoList(arpList);
	i = 0;
	j = arpList.GetCount();
	while (i < j)
	{
		arp = arpList.GetItemNoCheck(i);
		arpType = arp->GetARPType();
		if (arpType == Net::ARPInfo::ARPT_STATIC || arpType == Net::ARPInfo::ARPT_DYNAMIC)
		{
			ipInfo = MemAllocNN(SSWR::AVIRead::AVIRARPScanForm::IPMapInfo);
			ipInfo->ipAddr = arp->GetIPAddress();
			arp->GetPhysicalAddr(ipInfo->hwAddr);
			if (this->arpMap.Put(ipInfo->ipAddr, ipInfo).SetTo(ipInfo))
			{
				MemFreeNN(ipInfo);
			}
		}
		arp.Delete();
		i++;
	}
	this->UpdateARPList();

	Data::ArrayListNN<Net::ConnectionInfo> connInfoList;
	NN<Net::ConnectionInfo> connInfo;
	NN<SSWR::AVIRead::AVIRARPScanForm::AdapterInfo> adapter;
	Net::IPType ipType;
	UInt8 hwAddr[32];
	UTF8Char sbuff[128];
	UnsafeArray<UTF8Char> sptr;
	UOSInt k;
	UInt32 ip;
	this->core->GetSocketFactory()->GetConnInfoList(connInfoList);
	i = 0;
	j = connInfoList.GetCount();
	while (i < j)
	{
		connInfo = connInfoList.GetItemNoCheck(i);
		k = 0;
		while (true)
		{
			ip = connInfo->GetIPAddress(k);
			if (ip == 0)
				break;
			ipType = Net::SocketUtil::GetIPv4Type(ip);
			if (ipType == Net::IPType::Private)
			{
				if (connInfo->GetPhysicalAddress(hwAddr, 32) == 6)
				{
					adapter = MemAllocNN(SSWR::AVIRead::AVIRARPScanForm::AdapterInfo);
					sbuff[0] = 0;
					connInfo->GetName(sbuff);
					adapter->ifName = Text::StrCopyNew(sbuff);
					adapter->ipAddr = ip;
					MemCopyNO(adapter->hwAddr, hwAddr, 6);
					this->adapters.Add(adapter);
					sptr = Net::SocketUtil::GetIPv4Name(sbuff, ip);
					this->cboAdapter->AddItem(CSTRP(sbuff, sptr), adapter);
				}
			}
			k++;
		}
		connInfo.Delete();
		i++;
	}
	if (this->adapters.GetCount() > 0)
	{
		this->cboAdapter->SetSelectedIndex(0);
	}

	this->AddTimer(1000, OnTimerTick, this);
}

SSWR::AVIRead::AVIRARPScanForm::~AVIRARPScanForm()
{
	UOSInt i;
	NN<SSWR::AVIRead::AVIRARPScanForm::AdapterInfo> adapter;
	NN<SSWR::AVIRead::AVIRARPScanForm::IPMapInfo> ipInfo;
	i = this->adapters.GetCount();
	while (i-- > 0)
	{
		adapter = this->adapters.GetItemNoCheck(i);
		Text::StrDelNew(adapter->ifName);
		MemFreeNN(adapter);
	}

	i = this->arpMap.GetCount();
	while (i-- > 0)
	{
		ipInfo = this->arpMap.GetItemNoCheck(i);
		MemFreeNN(ipInfo);
	}
}

void SSWR::AVIRead::AVIRARPScanForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
