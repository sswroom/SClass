#include "Stdafx.h"
#include "Data/ByteTool.h"
#include "Net/ASN1OIDDB.h"
#include "Net/ASN1Util.h"
#include "Net/SNMPInfo.h"
#include "SSWR/AVIRead/AVIRSNMPTrapMonitorForm.h"
#include "Sync/MutexUsage.h"
#include "UI/MessageDialog.h"

void __stdcall SSWR::AVIRead::AVIRSNMPTrapMonitorForm::OnResultSelChg(void *userObj)
{
	SSWR::AVIRead::AVIRSNMPTrapMonitorForm *me = (SSWR::AVIRead::AVIRSNMPTrapMonitorForm*)userObj;
	SNMPPacket *packet = (SNMPPacket*)me->lbResults->GetSelectedItem();
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
	UTF8Char *sptr;
	UOSInt i;
	UOSInt j;
	Data::DateTime dt;
	Text::StringBuilderUTF8 sb;
	me->txtCommunity->SetText({packet->trap.community, Text::StrCharCnt(packet->trap.community)});
	Net::ASN1Util::OIDToString(packet->trap.entOID, packet->trap.entOIDLen, sb);
	me->txtEnterpriseOID->SetText(sb.ToCString());
	sb.ClearStr();
	Net::ASN1OIDDB::OIDToNameString(packet->trap.entOID, packet->trap.entOIDLen, sb);
	me->txtEnterpriseName->SetText(sb.ToCString());
	sptr = Net::SocketUtil::GetAddrName(sbuff, packet->addr);
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

	Net::SNMPUtil::BindingItem *item;
	me->lvResults->ClearItems();
	i = 0;
	j = packet->itemList->GetCount();
	while (i < j)
	{
		item = packet->itemList->GetItem(i);
		sb.ClearStr();
		Net::ASN1Util::OIDToString(item->oid, item->oidLen, sb);
		me->lvResults->AddItem(sb.ToCString(), 0);
		sb.ClearStr();
		Net::ASN1OIDDB::OIDToNameString(item->oid, item->oidLen, sb);
		me->lvResults->SetSubItem(i, 1, sb.ToCString());
		me->lvResults->SetSubItem(i, 2, Net::SNMPUtil::TypeGetName(item->valType));
		if (item->valBuff)
		{
			sb.ClearStr();
			Net::SNMPInfo::ValueToString(item->valType, item->valBuff, item->valLen, sb);
			me->lvResults->SetSubItem(i, 3, sb.ToCString());
		}
		i++;
	}
}

void __stdcall SSWR::AVIRead::AVIRSNMPTrapMonitorForm::OnTimerTick(void *userObj)
{
	SSWR::AVIRead::AVIRSNMPTrapMonitorForm *me = (SSWR::AVIRead::AVIRSNMPTrapMonitorForm*)userObj;
	UOSInt i = me->lbResults->GetCount();
	UOSInt j = me->packetList.GetCount();
	SNMPPacket *packet;
	if (i < j)
	{
		Data::DateTime dt;
		UTF8Char sbuff[32];
		UTF8Char *sptr;
		Sync::MutexUsage mutUsage(me->packetMut);
		while (i < j)
		{
			packet = me->packetList.GetItem(i);
			dt.SetTicks(packet->t);
			dt.ToLocalTime();
			sptr = dt.ToString(sbuff, "yyyy-MM-dd HH:mm:ss.fff");
			me->lbResults->AddItem(CSTRP(sbuff, sptr), packet);
			i++;
		}
	}
}

Bool __stdcall SSWR::AVIRead::AVIRSNMPTrapMonitorForm::OnSNMPTrapPacket(void *userObj, NotNullPtr<const Net::SocketUtil::AddressInfo> addr, UInt16 port, NotNullPtr<const Net::SNMPUtil::TrapInfo> trap, NotNullPtr<Data::ArrayList<Net::SNMPUtil::BindingItem*>> itemList)
{
	SSWR::AVIRead::AVIRSNMPTrapMonitorForm *me = (SSWR::AVIRead::AVIRSNMPTrapMonitorForm*)userObj;
	SNMPPacket *packet;
	Data::DateTime dt;
	dt.SetCurrTimeUTC();
	packet = MemAlloc(SNMPPacket, 1);
	packet->t = dt.ToTicks();
	packet->addr = addr.Ptr()[0];
	packet->port = port;
	MemCopyNO(&packet->trap, trap.Ptr(), sizeof(Net::SNMPUtil::TrapInfo));
	NEW_CLASS(packet->itemList, Data::ArrayList<Net::SNMPUtil::BindingItem*>());
	packet->itemList->AddAll(itemList);
	Sync::MutexUsage mutUsage(me->packetMut);
	me->packetList.Add(packet);
	return true;
}

SSWR::AVIRead::AVIRSNMPTrapMonitorForm::AVIRSNMPTrapMonitorForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core) : UI::GUIForm(parent, 1024, 768, ui)
{
	this->SetFont(0, 0, 8.25, false);
	this->SetText(CSTR("SNMP Trap Monitor"));

	this->core = core;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	NEW_CLASS(this->lbResults, UI::GUIListBox(ui, this, false));
	this->lbResults->SetRect(0, 0, 140, 23, false);
	this->lbResults->SetDockType(UI::GUIControl::DOCK_LEFT);
	this->lbResults->HandleSelectionChange(OnResultSelChg, this);
	NEW_CLASS(this->hspResults, UI::GUIHSplitter(ui, this, 3, false));
	NEW_CLASS(this->pnlResults, UI::GUIPanel(ui, this));
	this->pnlResults->SetDockType(UI::GUIControl::DOCK_FILL);
	NEW_CLASS(this->pnlTrap, UI::GUIPanel(ui, this->pnlResults));
	this->pnlTrap->SetRect(0, 0, 100, 248, false);
	this->pnlTrap->SetDockType(UI::GUIControl::DOCK_TOP);
	NEW_CLASS(this->lblCommunity, UI::GUILabel(ui, this->pnlTrap, CSTR("Community")));
	this->lblCommunity->SetRect(4, 4, 100, 23, false);
	NEW_CLASS(this->txtCommunity, UI::GUITextBox(ui, this->pnlTrap, CSTR("")));
	this->txtCommunity->SetRect(104, 4, 300, 23, false);
	this->txtCommunity->SetReadOnly(true);
	NEW_CLASS(this->lblEnterpriseOID, UI::GUILabel(ui, this->pnlTrap, CSTR("Enterprise OID")));
	this->lblEnterpriseOID->SetRect(4, 28, 100, 23, false);
	NEW_CLASS(this->txtEnterpriseOID, UI::GUITextBox(ui, this->pnlTrap, CSTR("")));
	this->txtEnterpriseOID->SetRect(104, 28, 300, 23, false);
	this->txtEnterpriseOID->SetReadOnly(true);
	NEW_CLASS(this->lblEnterpriseName, UI::GUILabel(ui, this->pnlTrap, CSTR("Enterprise Name")));
	this->lblEnterpriseName->SetRect(4, 52, 100, 23, false);
	NEW_CLASS(this->txtEnterpriseName, UI::GUITextBox(ui, this->pnlTrap, CSTR("")));
	this->txtEnterpriseName->SetRect(104, 52, 300, 23, false);
	this->txtEnterpriseName->SetReadOnly(true);
	NEW_CLASS(this->lblRemoteIP, UI::GUILabel(ui, this->pnlTrap, CSTR("Remote IP")));
	this->lblRemoteIP->SetRect(4, 76, 100, 23, false);
	NEW_CLASS(this->txtRemoteIP, UI::GUITextBox(ui, this->pnlTrap, CSTR("")));
	this->txtRemoteIP->SetRect(104, 76, 150, 23, false);
	this->txtRemoteIP->SetReadOnly(true);
	NEW_CLASS(this->lblRemotePort, UI::GUILabel(ui, this->pnlTrap, CSTR("Remote Port")));
	this->lblRemotePort->SetRect(4, 100, 100, 23, false);
	NEW_CLASS(this->txtRemotePort, UI::GUITextBox(ui, this->pnlTrap, CSTR("")));
	this->txtRemotePort->SetRect(104, 100, 100, 23, false);
	this->txtRemotePort->SetReadOnly(true);
	NEW_CLASS(this->lblAgentAddr, UI::GUILabel(ui, this->pnlTrap, CSTR("Agent Addr")));
	this->lblAgentAddr->SetRect(4, 124, 100, 23, false);
	NEW_CLASS(this->txtAgentAddr, UI::GUITextBox(ui, this->pnlTrap, CSTR("")));
	this->txtAgentAddr->SetRect(104, 124, 150, 23, false);
	this->txtAgentAddr->SetReadOnly(true);
	NEW_CLASS(this->lblGenericTrap, UI::GUILabel(ui, this->pnlTrap, CSTR("Generic Trap")));
	this->lblGenericTrap->SetRect(4, 148, 100, 23, false);
	NEW_CLASS(this->txtGenericTrap, UI::GUITextBox(ui, this->pnlTrap, CSTR("")));
	this->txtGenericTrap->SetRect(104, 148, 100, 23, false);
	this->txtGenericTrap->SetReadOnly(true);
	NEW_CLASS(this->lblSpecificTrap, UI::GUILabel(ui, this->pnlTrap, CSTR("Specific Trap")));
	this->lblSpecificTrap->SetRect(4, 172, 100, 23, false);
	NEW_CLASS(this->txtSpecificTrap, UI::GUITextBox(ui, this->pnlTrap, CSTR("")));
	this->txtSpecificTrap->SetRect(104, 172, 100, 23, false);
	this->txtSpecificTrap->SetReadOnly(true);
	NEW_CLASS(this->lblRecvTime, UI::GUILabel(ui, this->pnlTrap, CSTR("Recv Time")));
	this->lblRecvTime->SetRect(4, 196, 100, 23, false);
	NEW_CLASS(this->txtRecvTime, UI::GUITextBox(ui, this->pnlTrap, CSTR("")));
	this->txtRecvTime->SetRect(104, 196, 200, 23, false);
	this->txtRecvTime->SetReadOnly(true);
	NEW_CLASS(this->lblTrapTime, UI::GUILabel(ui, this->pnlTrap, CSTR("Trap Time")));
	this->lblTrapTime->SetRect(4, 220, 100, 23, false);
	NEW_CLASS(this->txtTrapTime, UI::GUITextBox(ui, this->pnlTrap, CSTR("")));
	this->txtTrapTime->SetRect(104, 220, 200, 23, false);
	this->txtTrapTime->SetReadOnly(true);
	NEW_CLASS(this->lvResults, UI::GUIListView(ui, this->pnlResults, UI::GUIListView::LVSTYLE_TABLE, 4));
	this->lvResults->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lvResults->SetFullRowSelect(true);
	this->lvResults->SetShowGrid(true);
	this->lvResults->AddColumn(CSTR("OID"), 150);
	this->lvResults->AddColumn(CSTR("Name"), 150);
	this->lvResults->AddColumn(CSTR("ValueType"), 100);
	this->lvResults->AddColumn(CSTR("Value"), 200);

	this->AddTimer(1000, OnTimerTick, this);

	NEW_CLASS(this->mon, Net::SNMPTrapMonitor(this->core->GetSocketFactory(), OnSNMPTrapPacket, this, this->core->GetLog()));
	if (this->mon->IsError())
	{
		UI::MessageDialog::ShowDialog(CSTR("Error in starting SNMP Trap Monitor"), CSTR("Error"), this);
	}
}

SSWR::AVIRead::AVIRSNMPTrapMonitorForm::~AVIRSNMPTrapMonitorForm()
{
	DEL_CLASS(this->mon);
	SNMPPacket *packet;
	UOSInt i = this->packetList.GetCount();
	UOSInt j;
	while (i-- > 0)
	{
		packet = this->packetList.GetItem(i);
		j = packet->itemList->GetCount();
		while (j-- > 0)
		{
			Net::SNMPUtil::FreeBindingItem(packet->itemList->GetItem(j));
		}
		DEL_CLASS(packet->itemList);
		MemFree(packet);
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
