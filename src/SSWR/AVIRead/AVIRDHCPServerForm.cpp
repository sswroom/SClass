#include "Stdafx.h"
#include "Data/ByteTool.h"
#include "IO/Path.h"
#include "Net/ConnectionInfo.h"
#include "Net/MACInfo.h"
#include "SSWR/AVIRead/AVIRDHCPServerForm.h"
#include "Text/MyString.h"
#include "Text/StringBuilderUTF8.h"
#include "UI/MessageDialog.h"

void __stdcall SSWR::AVIRead::AVIRDHCPServerForm::OnStartClicked(void *userObj)
{
	SSWR::AVIRead::AVIRDHCPServerForm *me = (SSWR::AVIRead::AVIRDHCPServerForm*)userObj;
	if (me->svr)
	{
		DEL_CLASS(me->svr);
		me->svr = 0;
		me->cboIP->SetEnabled(true);
		me->txtSubnet->SetReadOnly(false);
		me->txtFirstIP->SetReadOnly(false);
		me->txtDevCount->SetReadOnly(false);
		me->txtGateway->SetReadOnly(false);
		me->txtDNS1->SetReadOnly(false);
		me->txtDNS2->SetReadOnly(false);
	}
	else
	{
		UInt32 ifIp = (UInt32)(OSInt)me->cboIP->GetSelectedItem();
		UInt32 subnet = 0;
		UInt32 firstIP = 0;
		UInt32 devCount = 0;
		UInt32 gateway = 0;
		Data::ArrayList<UInt32> dnsList;
		Text::StringBuilderUTF8 sb;
		if (ifIp == 0)
		{
			UI::MessageDialog::ShowDialog((const UTF8Char*)"Please select an interface", (const UTF8Char*)"Error", me);
			return;
		}

		sb.ClearStr();
		me->txtSubnet->GetText(&sb);
		subnet = Net::SocketUtil::GetIPAddr(sb.ToString());
		if (!Net::SocketUtil::IPv4SubnetValid(subnet))
		{
			UI::MessageDialog::ShowDialog((const UTF8Char*)"Subnet is not valid", (const UTF8Char*)"Error", me);
			return;
		}

		sb.ClearStr();
		me->txtFirstIP->GetText(&sb);
		if (!sb.ToUInt32(&firstIP))
		{
			UI::MessageDialog::ShowDialog((const UTF8Char*)"First IP is not valid", (const UTF8Char*)"Error", me);
			return;
		}
		else if (firstIP == 0)
		{
			UI::MessageDialog::ShowDialog((const UTF8Char*)"First IP is not valid", (const UTF8Char*)"Error", me);
			return;
		}

		sb.ClearStr();
		me->txtDevCount->GetText(&sb);
		if (!sb.ToUInt32(&devCount))
		{
			UI::MessageDialog::ShowDialog((const UTF8Char*)"Device Count is not valid", (const UTF8Char*)"Error", me);
			return;
		}
		else if (devCount == 0)
		{
			UI::MessageDialog::ShowDialog((const UTF8Char*)"Device Count is not valid", (const UTF8Char*)"Error", me);
			return;
		}

		sb.ClearStr();
		me->txtGateway->GetText(&sb);
		if (sb.GetLength() == 0)
		{
			gateway = 0;
		}
		else if ((gateway = Net::SocketUtil::GetIPAddr(sb.ToString())) == 0)
		{
			UI::MessageDialog::ShowDialog((const UTF8Char*)"Gateway is not valid", (const UTF8Char*)"Error", me);
			return;
		}

		UInt32 tmpIP;
		sb.ClearStr();
		me->txtDNS1->GetText(&sb);
		if (sb.GetLength() == 0)
		{
		}
		else if ((tmpIP = Net::SocketUtil::GetIPAddr(sb.ToString())) == 0)
		{
			UI::MessageDialog::ShowDialog((const UTF8Char*)"DNS1 is not valid", (const UTF8Char*)"Error", me);
			return;
		}
		else
		{
			dnsList.Add(tmpIP);
		}
		
		sb.ClearStr();
		me->txtDNS2->GetText(&sb);
		if (sb.GetLength() == 0)
		{
		}
		else if ((tmpIP = Net::SocketUtil::GetIPAddr(sb.ToString())) == 0)
		{
			UI::MessageDialog::ShowDialog((const UTF8Char*)"DNS2 is not valid", (const UTF8Char*)"Error", me);
			return;
		}
		else
		{
			dnsList.Add(tmpIP);
		}

		NEW_CLASS(me->svr, Net::DHCPServer(me->sockf, ifIp, subnet, firstIP, devCount, gateway, &dnsList));
		if (me->svr->IsError())
		{
			DEL_CLASS(me->svr);
			me->svr = 0;
			UI::MessageDialog::ShowDialog((const UTF8Char*)"Error in starting server", (const UTF8Char*)"Error", me);
		}
		else
		{
			me->cboIP->SetEnabled(false);
			me->txtSubnet->SetReadOnly(true);
			me->txtFirstIP->SetReadOnly(true);
			me->txtDevCount->SetReadOnly(true);
			me->txtGateway->SetReadOnly(true);
			me->txtDNS1->SetReadOnly(true);
			me->txtDNS2->SetReadOnly(true);
		}
	}
}

void __stdcall SSWR::AVIRead::AVIRDHCPServerForm::OnTimerTick(void *userObj)
{
	SSWR::AVIRead::AVIRDHCPServerForm *me = (SSWR::AVIRead::AVIRDHCPServerForm*)userObj;
	if (me->svr)
	{
		UTF8Char sbuff[64];
		UInt8 mac[8];
		OSInt i;
		OSInt j;
		Net::DHCPServer::DeviceStatus *dhcp;
		const Net::MACInfo::MACEntry *macInfo;
		Sync::MutexUsage mutUsage;
		me->svr->UseStatus(&mutUsage);
		Data::ArrayList<Net::DHCPServer::DeviceStatus*> *dhcpList = me->svr->StatusGetList();
		if (dhcpList->GetCount() != me->lvDevices->GetCount())
		{
			Net::DHCPServer::DeviceStatus *currSel = (Net::DHCPServer::DeviceStatus*)me->lvDevices->GetSelectedItem();
			me->lvDevices->ClearItems();
			i = 0;
			j = dhcpList->GetCount();
			while (i < j)
			{
				dhcp = dhcpList->GetItem(i);
				WriteMInt64(mac, dhcp->hwAddr);
				Text::StrHexBytes(sbuff, &mac[2], 6, ':');
				me->lvDevices->AddItem(sbuff, dhcp);
				macInfo = Net::MACInfo::GetMACInfo(dhcp->hwAddr);
				me->lvDevices->SetSubItem(i, 1, (const UTF8Char*)macInfo->name);
				if (dhcp == currSel)
				{
					me->lvDevices->SetSelectedIndex(i);
				}
				dhcp->updated = true;
				i++;
			}
		}

		i = 0;
		j = dhcpList->GetCount();
		while (i < j)
		{
			dhcp = dhcpList->GetItem(i);
			if (dhcp->updated)
			{
				Data::DateTime dt;
				dhcp->updated = false;
				Net::SocketUtil::GetIPv4Name(sbuff, dhcp->assignedIP);
				me->lvDevices->SetSubItem(i, 2, sbuff);
				dt.SetTicks(dhcp->assignTime);
				dt.ToLocalTime();
				dt.ToString(sbuff, "yyyy-MM-dd HH:mm:ss.fff");
				me->lvDevices->SetSubItem(i, 3, sbuff);
				dt.AddSecond(me->svr->GetIPLeaseTime());
				dt.ToString(sbuff, "yyyy-MM-dd HH:mm:ss.fff");
				me->lvDevices->SetSubItem(i, 4, sbuff);
				if (dhcp->hostName)
					me->lvDevices->SetSubItem(i, 5, dhcp->hostName);
				if (dhcp->vendorClass)
					me->lvDevices->SetSubItem(i, 6, dhcp->vendorClass);
			}
			i++;
		}
	}
}

SSWR::AVIRead::AVIRDHCPServerForm::AVIRDHCPServerForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core) : UI::GUIForm(parent, 1024, 768, ui)
{
	this->core = core;
	this->sockf = this->core->GetSocketFactory();
	this->svr = 0;
	this->SetText((const UTF8Char*)"DHCP Server");
	this->SetFont(0, 8.25, false);
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	NEW_CLASS(this->pnlControl, UI::GUIPanel(ui, this));
	this->pnlControl->SetRect(0, 0, 100, 199, false);
	this->pnlControl->SetDockType(UI::GUIControl::DOCK_TOP);
	NEW_CLASS(this->lblIP, UI::GUILabel(ui, this->pnlControl, (const UTF8Char*)"Interface"));
	this->lblIP->SetRect(4, 4, 100, 23, false);
	NEW_CLASS(this->cboIP, UI::GUIComboBox(ui, this->pnlControl, false));
	this->cboIP->SetRect(104, 4, 150, 23, false);
	NEW_CLASS(this->lblSubnet, UI::GUILabel(ui, this->pnlControl, (const UTF8Char *)"Subnet Mask"));
	this->lblSubnet->SetRect(4, 28, 100, 23, false);
	NEW_CLASS(this->txtSubnet, UI::GUITextBox(ui, this->pnlControl, (const UTF8Char*)"255.255.255.0"));
	this->txtSubnet->SetRect(104, 28, 100, 23, false);
	NEW_CLASS(this->lblFirstIP, UI::GUILabel(ui, this->pnlControl, (const UTF8Char *)"First IP"));
	this->lblFirstIP->SetRect(4, 52, 100, 23, false);
	NEW_CLASS(this->txtFirstIP, UI::GUITextBox(ui, this->pnlControl, (const UTF8Char*)"51"));
	this->txtFirstIP->SetRect(104, 52, 100, 23, false);
	NEW_CLASS(this->lblDevCount, UI::GUILabel(ui, this->pnlControl, (const UTF8Char *)"Dev Count"));
	this->lblDevCount->SetRect(4, 76, 100, 23, false);
	NEW_CLASS(this->txtDevCount, UI::GUITextBox(ui, this->pnlControl, (const UTF8Char*)"100"));
	this->txtDevCount->SetRect(104, 76, 100, 23, false);
	NEW_CLASS(this->lblGateway, UI::GUILabel(ui, this->pnlControl, (const UTF8Char *)"Gateway"));
	this->lblGateway->SetRect(4, 100, 100, 23, false);
	NEW_CLASS(this->txtGateway, UI::GUITextBox(ui, this->pnlControl, (const UTF8Char*)"192.168.0.1"));
	this->txtGateway->SetRect(104, 100, 100, 23, false);
	NEW_CLASS(this->lblDNS1, UI::GUILabel(ui, this->pnlControl, (const UTF8Char *)"DNS1"));
	this->lblDNS1->SetRect(4, 124, 100, 23, false);
	NEW_CLASS(this->txtDNS1, UI::GUITextBox(ui, this->pnlControl, (const UTF8Char*)"192.168.0.1"));
	this->txtDNS1->SetRect(104, 124, 100, 23, false);
	NEW_CLASS(this->lblDNS2, UI::GUILabel(ui, this->pnlControl, (const UTF8Char *)"DNS2"));
	this->lblDNS2->SetRect(4, 148, 100, 23, false);
	NEW_CLASS(this->txtDNS2, UI::GUITextBox(ui, this->pnlControl, (const UTF8Char*)""));
	this->txtDNS2->SetRect(104, 148, 100, 23, false);
	NEW_CLASS(this->btnStart, UI::GUIButton(ui, this->pnlControl, (const UTF8Char*)"Start"));
	this->btnStart->SetRect(104, 172, 75, 23, false);
	this->btnStart->HandleButtonClick(OnStartClicked, this);
	NEW_CLASS(this->lvDevices, UI::GUIListView(ui, this, UI::GUIListView::LVSTYLE_TABLE, 7));
	this->lvDevices->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lvDevices->SetShowGrid(true);
	this->lvDevices->SetFullRowSelect(true);
	this->lvDevices->AddColumn((const UTF8Char*)"HW Address", 105);
	this->lvDevices->AddColumn((const UTF8Char*)"Vendor", 200);
	this->lvDevices->AddColumn((const UTF8Char*)"IP Addr", 100);
	this->lvDevices->AddColumn((const UTF8Char*)"Obtain Time", 140);
	this->lvDevices->AddColumn((const UTF8Char*)"Expire Time", 140);
	this->lvDevices->AddColumn((const UTF8Char*)"Host Name", 100);
	this->lvDevices->AddColumn((const UTF8Char*)"Vendor Class", 100);

	Data::ArrayList<Net::ConnectionInfo*> connInfoList;
	Net::ConnectionInfo *connInfo;
	UTF8Char sbuff[32];
	UOSInt i;
	UOSInt j;
	UOSInt k;
	UInt32 ip;
	this->sockf->GetConnInfoList(&connInfoList);
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
			Net::SocketUtil::IPType ipType = Net::SocketUtil::GetIPv4Type(ip);
			if (ipType == Net::SocketUtil::IT_PRIVATE)
			{
				Net::SocketUtil::GetIPv4Name(sbuff, ip);
				this->cboIP->AddItem(sbuff, (void*)(OSInt)ip);
			}
			k++;
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

SSWR::AVIRead::AVIRDHCPServerForm::~AVIRDHCPServerForm()
{
	SDEL_CLASS(this->svr);
}

void SSWR::AVIRead::AVIRDHCPServerForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
