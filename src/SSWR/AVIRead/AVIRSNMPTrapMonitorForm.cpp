#include "Stdafx.h"
#include "Data/ByteTool.h"
#include "Net/ASN1OIDDB.h"
#include "Net/ASN1Util.h"
#include "Net/SNMPInfo.h"
#include "SSWR/AVIRead/AVIRSNMPTrapMonitorForm.h"
#include "Sync/MutexUsage.h"

void __stdcall SSWR::AVIRead::AVIRSNMPTrapMonitorForm::OnResultSelChg(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRSNMPTrapMonitorForm> me = userObj.GetNN<SSWR::AVIRead::AVIRSNMPTrapMonitorForm>();
	SNMPPacket *packet = (SNMPPacket*)me->lbResults->GetSelectedItem().p;
	me->lvResults->ClearItems();
	if (packet == 0)
	{
		me->txtCommunity->SetText(CSTR(""));
		me->txtEnterpriseOID->SetText(CSTR(""));
		me->txtEnterpriseName->SetText(CSTR(""));
		me->txtRemoteIP->SetText(CSTR(""));
		me->txtRemotePort->SetText(CSTR(""));
		me->txtAgentAddr->SetText(CSTR(""));
		me->txtGenericTrap->SetText(CSTR(""));
		me->txtSpecificTrap->SetText(CSTR(""));
		me->txtRecvTime->SetText(CSTR(""));
		me->txtTrapTime->SetText(CSTR(""));
		return;
	}
	UTF8Char sbuff[64];
	UnsafeArray<UTF8Char> sptr;
	UOSInt i;
	UOSInt j;
	Data::DateTime dt;
	Text::StringBuilderUTF8 sb;
	me->txtCommunity->SetText({packet->trap.community, Text::StrCharCnt(packet->trap.community)});
	Net::ASN1Util::OIDToString(Data::ByteArrayR(packet->trap.entOID, packet->trap.entOIDLen), sb);
	me->txtEnterpriseOID->SetText(sb.ToCString());
	sb.ClearStr();
	Net::ASN1OIDDB::OIDToNameString(Data::ByteArrayR(packet->trap.entOID, packet->trap.entOIDLen), sb);
	me->txtEnterpriseName->SetText(sb.ToCString());
	sptr = Net::SocketUtil::GetAddrName(sbuff, packet->addr).Or(sbuff);
	me->txtRemoteIP->SetText(CSTRP(sbuff, sptr));
	sptr = Text::StrUInt16(sbuff, packet->port);
	me->txtRemotePort->SetText(CSTRP(sbuff, sptr));
	sptr = Net::SocketUtil::GetIPv4Name(sbuff, packet->trap.agentIPv4);
	me->txtAgentAddr->SetText(CSTRP(sbuff, sptr));
	sptr = Text::StrUInt32(sbuff, packet->trap.genericTrap);
	me->txtGenericTrap->SetText(CSTRP(sbuff, sptr));
	sptr = Text::StrUInt32(sbuff, packet->trap.specificTrap);
	me->txtSpecificTrap->SetText(CSTRP(sbuff, sptr));
	dt.SetTicks(packet->t);
	dt.ToLocalTime();
	sptr = dt.ToString(sbuff, "yyyy-MM-dd HH:mm:ss.fff");
	me->txtRecvTime->SetText(CSTRP(sbuff, sptr));
	sptr = Text::StrUInt32(sbuff, packet->trap.timeStamp);
	me->txtTrapTime->SetText(CSTRP(sbuff, sptr));

	NN<Net::SNMPUtil::BindingItem> item;
	me->lvResults->ClearItems();
	i = 0;
	j = packet->itemList.GetCount();
	while (i < j)
	{
		item = packet->itemList.GetItemNoCheck(i);
		sb.ClearStr();
		Net::ASN1Util::OIDToString(Data::ByteArrayR(item->oid, item->oidLen), sb);
		me->lvResults->AddItem(sb.ToCString(), 0);
		sb.ClearStr();
		Net::ASN1OIDDB::OIDToNameString(Data::ByteArrayR(item->oid, item->oidLen), sb);
		me->lvResults->SetSubItem(i, 1, sb.ToCString());
		me->lvResults->SetSubItem(i, 2, Net::SNMPUtil::TypeGetName(item->valType));
		if (item->valBuff)
		{
			sb.ClearStr();
			Net::SNMPInfo::ValueToString(item->valType, Data::ByteArrayR(item->valBuff, item->valLen), sb);
			me->lvResults->SetSubItem(i, 3, sb.ToCString());
		}
		i++;
	}
}

void __stdcall SSWR::AVIRead::AVIRSNMPTrapMonitorForm::OnTimerTick(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRSNMPTrapMonitorForm> me = userObj.GetNN<SSWR::AVIRead::AVIRSNMPTrapMonitorForm>();
	UOSInt i = me->lbResults->GetCount();
	UOSInt j = me->packetList.GetCount();
	NN<SNMPPacket> packet;
	if (i < j)
	{
		Data::DateTime dt;
		UTF8Char sbuff[32];
		UnsafeArray<UTF8Char> sptr;
		Sync::MutexUsage mutUsage(me->packetMut);
		while (i < j)
		{
			packet = me->packetList.GetItemNoCheck(i);
			dt.SetTicks(packet->t);
			dt.ToLocalTime();
			sptr = dt.ToString(sbuff, "yyyy-MM-dd HH:mm:ss.fff");
			me->lbResults->AddItem(CSTRP(sbuff, sptr), packet);
			i++;
		}
	}
}

Bool __stdcall SSWR::AVIRead::AVIRSNMPTrapMonitorForm::OnSNMPTrapPacket(AnyType userObj, NN<const Net::SocketUtil::AddressInfo> addr, UInt16 port, NN<const Net::SNMPUtil::TrapInfo> trap, NN<Data::ArrayListNN<Net::SNMPUtil::BindingItem>> itemList)
{
	NN<SSWR::AVIRead::AVIRSNMPTrapMonitorForm> me = userObj.GetNN<SSWR::AVIRead::AVIRSNMPTrapMonitorForm>();
	NN<SNMPPacket> packet;
	Data::DateTime dt;
	dt.SetCurrTimeUTC();
	NEW_CLASSNN(packet, SNMPPacket());
	packet->t = dt.ToTicks();
	packet->addr = addr.Ptr()[0];
	packet->port = port;
	MemCopyNO(&packet->trap, trap.Ptr(), sizeof(Net::SNMPUtil::TrapInfo));
	packet->itemList.AddAll(itemList);
	Sync::MutexUsage mutUsage(me->packetMut);
	me->packetList.Add(packet);
	return true;
}

SSWR::AVIRead::AVIRSNMPTrapMonitorForm::AVIRSNMPTrapMonitorForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core) : UI::GUIForm(parent, 1024, 768, ui)
{
	this->SetFont(0, 0, 8.25, false);
	this->SetText(CSTR("SNMP Trap Monitor"));

	this->core = core;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	this->lbResults = ui->NewListBox(*this, false);
	this->lbResults->SetRect(0, 0, 140, 23, false);
	this->lbResults->SetDockType(UI::GUIControl::DOCK_LEFT);
	this->lbResults->HandleSelectionChange(OnResultSelChg, this);
	this->hspResults = ui->NewHSplitter(*this, 3, false);
	this->pnlResults = ui->NewPanel(*this);
	this->pnlResults->SetDockType(UI::GUIControl::DOCK_FILL);
	this->pnlTrap = ui->NewPanel(this->pnlResults);
	this->pnlTrap->SetRect(0, 0, 100, 248, false);
	this->pnlTrap->SetDockType(UI::GUIControl::DOCK_TOP);
	this->lblCommunity = ui->NewLabel(this->pnlTrap, CSTR("Community"));
	this->lblCommunity->SetRect(4, 4, 100, 23, false);
	this->txtCommunity = ui->NewTextBox(this->pnlTrap, CSTR(""));
	this->txtCommunity->SetRect(104, 4, 300, 23, false);
	this->txtCommunity->SetReadOnly(true);
	this->lblEnterpriseOID = ui->NewLabel(this->pnlTrap, CSTR("Enterprise OID"));
	this->lblEnterpriseOID->SetRect(4, 28, 100, 23, false);
	this->txtEnterpriseOID = ui->NewTextBox(this->pnlTrap, CSTR(""));
	this->txtEnterpriseOID->SetRect(104, 28, 300, 23, false);
	this->txtEnterpriseOID->SetReadOnly(true);
	this->lblEnterpriseName = ui->NewLabel(this->pnlTrap, CSTR("Enterprise Name"));
	this->lblEnterpriseName->SetRect(4, 52, 100, 23, false);
	this->txtEnterpriseName = ui->NewTextBox(this->pnlTrap, CSTR(""));
	this->txtEnterpriseName->SetRect(104, 52, 300, 23, false);
	this->txtEnterpriseName->SetReadOnly(true);
	this->lblRemoteIP = ui->NewLabel(this->pnlTrap, CSTR("Remote IP"));
	this->lblRemoteIP->SetRect(4, 76, 100, 23, false);
	this->txtRemoteIP = ui->NewTextBox(this->pnlTrap, CSTR(""));
	this->txtRemoteIP->SetRect(104, 76, 150, 23, false);
	this->txtRemoteIP->SetReadOnly(true);
	this->lblRemotePort = ui->NewLabel(this->pnlTrap, CSTR("Remote Port"));
	this->lblRemotePort->SetRect(4, 100, 100, 23, false);
	this->txtRemotePort = ui->NewTextBox(this->pnlTrap, CSTR(""));
	this->txtRemotePort->SetRect(104, 100, 100, 23, false);
	this->txtRemotePort->SetReadOnly(true);
	this->lblAgentAddr = ui->NewLabel(this->pnlTrap, CSTR("Agent Addr"));
	this->lblAgentAddr->SetRect(4, 124, 100, 23, false);
	this->txtAgentAddr = ui->NewTextBox(this->pnlTrap, CSTR(""));
	this->txtAgentAddr->SetRect(104, 124, 150, 23, false);
	this->txtAgentAddr->SetReadOnly(true);
	this->lblGenericTrap = ui->NewLabel(this->pnlTrap, CSTR("Generic Trap"));
	this->lblGenericTrap->SetRect(4, 148, 100, 23, false);
	this->txtGenericTrap = ui->NewTextBox(this->pnlTrap, CSTR(""));
	this->txtGenericTrap->SetRect(104, 148, 100, 23, false);
	this->txtGenericTrap->SetReadOnly(true);
	this->lblSpecificTrap = ui->NewLabel(this->pnlTrap, CSTR("Specific Trap"));
	this->lblSpecificTrap->SetRect(4, 172, 100, 23, false);
	this->txtSpecificTrap = ui->NewTextBox(this->pnlTrap, CSTR(""));
	this->txtSpecificTrap->SetRect(104, 172, 100, 23, false);
	this->txtSpecificTrap->SetReadOnly(true);
	this->lblRecvTime = ui->NewLabel(this->pnlTrap, CSTR("Recv Time"));
	this->lblRecvTime->SetRect(4, 196, 100, 23, false);
	this->txtRecvTime = ui->NewTextBox(this->pnlTrap, CSTR(""));
	this->txtRecvTime->SetRect(104, 196, 200, 23, false);
	this->txtRecvTime->SetReadOnly(true);
	this->lblTrapTime = ui->NewLabel(this->pnlTrap, CSTR("Trap Time"));
	this->lblTrapTime->SetRect(4, 220, 100, 23, false);
	this->txtTrapTime = ui->NewTextBox(this->pnlTrap, CSTR(""));
	this->txtTrapTime->SetRect(104, 220, 200, 23, false);
	this->txtTrapTime->SetReadOnly(true);
	this->lvResults = ui->NewListView(this->pnlResults, UI::ListViewStyle::Table, 4);
	this->lvResults->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lvResults->SetFullRowSelect(true);
	this->lvResults->SetShowGrid(true);
	this->lvResults->AddColumn(CSTR("OID"), 150);
	this->lvResults->AddColumn(CSTR("Name"), 150);
	this->lvResults->AddColumn(CSTR("ValueType"), 100);
	this->lvResults->AddColumn(CSTR("Value"), 200);

	this->AddTimer(1000, OnTimerTick, this);

	NEW_CLASSNN(this->mon, Net::SNMPTrapMonitor(this->core->GetSocketFactory(), OnSNMPTrapPacket, this, this->core->GetLog()));
	if (this->mon->IsError())
	{
		this->ui->ShowMsgOK(CSTR("Error in starting SNMP Trap Monitor"), CSTR("Error"), this);
	}
}

SSWR::AVIRead::AVIRSNMPTrapMonitorForm::~AVIRSNMPTrapMonitorForm()
{
	this->mon.Delete();
	NN<SNMPPacket> packet;
	UOSInt i = this->packetList.GetCount();
	while (i-- > 0)
	{
		packet = this->packetList.GetItemNoCheck(i);
		packet->itemList.FreeAll(Net::SNMPUtil::FreeBindingItem);
		packet.Delete();
	}
}

void SSWR::AVIRead::AVIRSNMPTrapMonitorForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}

Bool SSWR::AVIRead::AVIRSNMPTrapMonitorForm::IsError()
{
	return this->mon->IsError();
}
