#include "Stdafx.h"
#include "Data/ByteTool.h"
#include "IO/Path.h"
#include "Net/ConnectionInfo.h"
#include "Net/MACInfo.h"
#include "SSWR/AVIRead/AVIRDHCPServerForm.h"
#include "Text/MyString.h"
#include "Text/StringBuilderUTF8.h"

void __stdcall SSWR::AVIRead::AVIRDHCPServerForm::OnStartClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRDHCPServerForm> me = userObj.GetNN<SSWR::AVIRead::AVIRDHCPServerForm>();
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
		UInt32 ifIp = (UInt32)me->cboIP->GetSelectedItem().GetUOSInt();
		UInt32 subnet = 0;
		UInt32 firstIP = 0;
		UInt32 devCount = 0;
		UInt32 gateway = 0;
		Data::ArrayList<UInt32> dnsList;
		Text::StringBuilderUTF8 sb;
		if (ifIp == 0)
		{
			me->ui->ShowMsgOK(CSTR("Please select an interface"), CSTR("Error"), me);
			return;
		}

		sb.ClearStr();
		me->txtSubnet->GetText(sb);
		subnet = Net::SocketUtil::GetIPAddr(sb.ToCString());
		if (!Net::SocketUtil::IPv4SubnetValid(subnet))
		{
			me->ui->ShowMsgOK(CSTR("Subnet is not valid"), CSTR("Error"), me);
			return;
		}

		sb.ClearStr();
		me->txtFirstIP->GetText(sb);
		if (!sb.ToUInt32(firstIP))
		{
			me->ui->ShowMsgOK(CSTR("First IP is not valid"), CSTR("Error"), me);
			return;
		}
		else if (firstIP == 0)
		{
			me->ui->ShowMsgOK(CSTR("First IP is not valid"), CSTR("Error"), me);
			return;
		}

		sb.ClearStr();
		me->txtDevCount->GetText(sb);
		if (!sb.ToUInt32(devCount))
		{
			me->ui->ShowMsgOK(CSTR("Device Count is not valid"), CSTR("Error"), me);
			return;
		}
		else if (devCount == 0)
		{
			me->ui->ShowMsgOK(CSTR("Device Count is not valid"), CSTR("Error"), me);
			return;
		}

		sb.ClearStr();
		me->txtGateway->GetText(sb);
		if (sb.GetLength() == 0)
		{
			gateway = 0;
		}
		else if ((gateway = Net::SocketUtil::GetIPAddr(sb.ToCString())) == 0)
		{
			me->ui->ShowMsgOK(CSTR("Gateway is not valid"), CSTR("Error"), me);
			return;
		}

		UInt32 tmpIP;
		sb.ClearStr();
		me->txtDNS1->GetText(sb);
		if (sb.GetLength() == 0)
		{
		}
		else if ((tmpIP = Net::SocketUtil::GetIPAddr(sb.ToCString())) == 0)
		{
			me->ui->ShowMsgOK(CSTR("DNS1 is not valid"), CSTR("Error"), me);
			return;
		}
		else
		{
			dnsList.Add(tmpIP);
		}
		
		sb.ClearStr();
		me->txtDNS2->GetText(sb);
		if (sb.GetLength() == 0)
		{
		}
		else if ((tmpIP = Net::SocketUtil::GetIPAddr(sb.ToCString())) == 0)
		{
			me->ui->ShowMsgOK(CSTR("DNS2 is not valid"), CSTR("Error"), me);
			return;
		}
		else
		{
			dnsList.Add(tmpIP);
		}

		NEW_CLASS(me->svr, Net::DHCPServer(me->sockf, ifIp, subnet, firstIP, devCount, gateway, dnsList, me->core->GetLog()));
		if (me->svr->IsError())
		{
			DEL_CLASS(me->svr);
			me->svr = 0;
			me->ui->ShowMsgOK(CSTR("Error in starting server"), CSTR("Error"), me);
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

void __stdcall SSWR::AVIRead::AVIRDHCPServerForm::OnTimerTick(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRDHCPServerForm> me = userObj.GetNN<SSWR::AVIRead::AVIRDHCPServerForm>();
	if (me->svr)
	{
		UTF8Char sbuff[64];
		UnsafeArray<UTF8Char> sptr;
		UInt8 mac[8];
		UOSInt i;
		UOSInt j;
		NN<Net::DHCPServer::DeviceStatus> dhcp;
		NN<const Net::MACInfo::MACEntry> macInfo;
		NN<Text::String> s;
		Sync::MutexUsage mutUsage;
		me->svr->UseStatus(mutUsage);
		NN<const Data::ReadingListNN<Net::DHCPServer::DeviceStatus>> dhcpList = me->svr->StatusGetList();
		if (dhcpList->GetCount() != me->lvDevices->GetCount())
		{
			Optional<Net::DHCPServer::DeviceStatus> currSel = me->lvDevices->GetSelectedItem().GetOpt<Net::DHCPServer::DeviceStatus>();
			me->lvDevices->ClearItems();
			i = 0;
			j = dhcpList->GetCount();
			while (i < j)
			{
				dhcp = dhcpList->GetItemNoCheck(i);
				WriteMUInt64(mac, dhcp->hwAddr);
				sptr = Text::StrHexBytes(sbuff, &mac[2], 6, ':');
				me->lvDevices->AddItem(CSTRP(sbuff, sptr), dhcp);
				macInfo = Net::MACInfo::GetMACInfo(dhcp->hwAddr);
				me->lvDevices->SetSubItem(i, 1, {macInfo->name, macInfo->nameLen});
				if (dhcp.Ptr() == currSel.OrNull())
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
			dhcp = dhcpList->GetItemNoCheck(i);
			if (dhcp->updated)
			{
				Data::DateTime dt;
				dhcp->updated = false;
				sptr = Net::SocketUtil::GetIPv4Name(sbuff, dhcp->assignedIP);
				me->lvDevices->SetSubItem(i, 2, CSTRP(sbuff, sptr));
				dt.SetTicks(dhcp->assignTime);
				dt.ToLocalTime();
				sptr = dt.ToString(sbuff, "yyyy-MM-dd HH:mm:ss.fff");
				me->lvDevices->SetSubItem(i, 3, CSTRP(sbuff, sptr));
				dt.AddSecond((OSInt)me->svr->GetIPLeaseTime());
				sptr = dt.ToString(sbuff, "yyyy-MM-dd HH:mm:ss.fff");
				me->lvDevices->SetSubItem(i, 4, CSTRP(sbuff, sptr));
				if (s.Set(dhcp->hostName))
					me->lvDevices->SetSubItem(i, 5, s);
				if (s.Set(dhcp->vendorClass))
					me->lvDevices->SetSubItem(i, 6, s);
			}
			i++;
		}
	}
}

SSWR::AVIRead::AVIRDHCPServerForm::AVIRDHCPServerForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core) : UI::GUIForm(parent, 1024, 768, ui)
{
	this->core = core;
	this->sockf = this->core->GetSocketFactory();
	this->svr = 0;
	this->SetText(CSTR("DHCP Server"));
	this->SetFont(0, 0, 8.25, false);
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	this->pnlControl = ui->NewPanel(*this);
	this->pnlControl->SetRect(0, 0, 100, 199, false);
	this->pnlControl->SetDockType(UI::GUIControl::DOCK_TOP);
	this->lblIP = ui->NewLabel(this->pnlControl, CSTR("Interface"));
	this->lblIP->SetRect(4, 4, 100, 23, false);
	this->cboIP = ui->NewComboBox(this->pnlControl, false);
	this->cboIP->SetRect(104, 4, 150, 23, false);
	this->lblSubnet = ui->NewLabel(this->pnlControl, CSTR("Subnet Mask"));
	this->lblSubnet->SetRect(4, 28, 100, 23, false);
	this->txtSubnet = ui->NewTextBox(this->pnlControl, CSTR("255.255.255.0"));
	this->txtSubnet->SetRect(104, 28, 100, 23, false);
	this->lblFirstIP = ui->NewLabel(this->pnlControl, CSTR("First IP"));
	this->lblFirstIP->SetRect(4, 52, 100, 23, false);
	this->txtFirstIP = ui->NewTextBox(this->pnlControl, CSTR("51"));
	this->txtFirstIP->SetRect(104, 52, 100, 23, false);
	this->lblDevCount = ui->NewLabel(this->pnlControl, CSTR("Dev Count"));
	this->lblDevCount->SetRect(4, 76, 100, 23, false);
	this->txtDevCount = ui->NewTextBox(this->pnlControl, CSTR("100"));
	this->txtDevCount->SetRect(104, 76, 100, 23, false);
	this->lblGateway = ui->NewLabel(this->pnlControl, CSTR("Gateway"));
	this->lblGateway->SetRect(4, 100, 100, 23, false);
	this->txtGateway = ui->NewTextBox(this->pnlControl, CSTR("192.168.0.1"));
	this->txtGateway->SetRect(104, 100, 100, 23, false);
	this->lblDNS1 = ui->NewLabel(this->pnlControl, CSTR("DNS1"));
	this->lblDNS1->SetRect(4, 124, 100, 23, false);
	this->txtDNS1 = ui->NewTextBox(this->pnlControl, CSTR("192.168.0.1"));
	this->txtDNS1->SetRect(104, 124, 100, 23, false);
	this->lblDNS2 = ui->NewLabel(this->pnlControl, CSTR("DNS2"));
	this->lblDNS2->SetRect(4, 148, 100, 23, false);
	this->txtDNS2 = ui->NewTextBox(this->pnlControl, CSTR(""));
	this->txtDNS2->SetRect(104, 148, 100, 23, false);
	this->btnStart = ui->NewButton(this->pnlControl, CSTR("Start"));
	this->btnStart->SetRect(104, 172, 75, 23, false);
	this->btnStart->HandleButtonClick(OnStartClicked, this);
	this->lvDevices = ui->NewListView(*this, UI::ListViewStyle::Table, 7);
	this->lvDevices->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lvDevices->SetShowGrid(true);
	this->lvDevices->SetFullRowSelect(true);
	this->lvDevices->AddColumn(CSTR("HW Address"), 105);
	this->lvDevices->AddColumn(CSTR("Vendor"), 200);
	this->lvDevices->AddColumn(CSTR("IP Addr"), 100);
	this->lvDevices->AddColumn(CSTR("Obtain Time"), 140);
	this->lvDevices->AddColumn(CSTR("Expire Time"), 140);
	this->lvDevices->AddColumn(CSTR("Host Name"), 100);
	this->lvDevices->AddColumn(CSTR("Vendor Class"), 100);

	Data::ArrayListNN<Net::ConnectionInfo> connInfoList;
	NN<Net::ConnectionInfo> connInfo;
	UTF8Char sbuff[32];
	UnsafeArray<UTF8Char> sptr;
	UOSInt i;
	UOSInt j;
	UOSInt k;
	UInt32 ip;
	this->sockf->GetConnInfoList(connInfoList);
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
			Net::IPType ipType = Net::SocketUtil::GetIPv4Type(ip);
			if (ipType == Net::IPType::Private)
			{
				sptr = Net::SocketUtil::GetIPv4Name(sbuff, ip);
				this->cboIP->AddItem(CSTRP(sbuff, sptr), (void*)(OSInt)ip);
			}
			k++;
		}
		connInfo.Delete();
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
