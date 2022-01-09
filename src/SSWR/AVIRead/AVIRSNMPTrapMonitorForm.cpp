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
		me->txtCommunity->SetText((const UTF8Char*)"");
		me->txtEnterpriseOID->SetText((const UTF8Char*)"");
		me->txtEnterpriseName->SetText((const UTF8Char*)"");
		me->txtRemoteIP->SetText((const UTF8Char*)"");
		me->txtRemotePort->SetText((const UTF8Char*)"");
		me->txtAgentAddr->SetText((const UTF8Char*)"");
		me->txtGenericTrap->SetText((const UTF8Char*)"");
		me->txtSpecificTrap->SetText((const UTF8Char*)"");
		me->txtRecvTime->SetText((const UTF8Char*)"");
		me->txtTrapTime->SetText((const UTF8Char*)"");
		return;
	}
	UTF8Char sbuff[64];
	UOSInt i;
	UOSInt j;
	Data::DateTime dt;
	Text::StringBuilderUTF8 sb;
	me->txtCommunity->SetText(packet->trap.community);
	Net::ASN1Util::OIDToString(packet->trap.entOID, packet->trap.entOIDLen, &sb);
	me->txtEnterpriseOID->SetText(sb.ToString());
	sb.ClearStr();
	Net::ASN1OIDDB::OIDToNameString(packet->trap.entOID, packet->trap.entOIDLen, &sb);
	me->txtEnterpriseName->SetText(sb.ToString());
	Net::SocketUtil::GetAddrName(sbuff, &packet->addr);
	me->txtRemoteIP->SetText(sbuff);
	Text::StrUInt16(sbuff, packet->port);
	me->txtRemotePort->SetText(sbuff);
	Net::SocketUtil::GetIPv4Name(sbuff, packet->trap.agentIPv4);
	me->txtAgentAddr->SetText(sbuff);
	Text::StrUInt32(sbuff, packet->trap.genericTrap);
	me->txtGenericTrap->SetText(sbuff);
	Text::StrUInt32(sbuff, packet->trap.specificTrap);
	me->txtSpecificTrap->SetText(sbuff);
	dt.SetTicks(packet->t);
	dt.ToLocalTime();
	dt.ToString(sbuff, "yyyy-MM-dd HH:mm:ss.fff");
	me->txtRecvTime->SetText(sbuff);
	Text::StrUInt32(sbuff, packet->trap.timeStamp);
	me->txtTrapTime->SetText(sbuff);

	Net::SNMPUtil::BindingItem *item;
	me->lvResults->ClearItems();
	i = 0;
	j = packet->itemList->GetCount();
	while (i < j)
	{
		item = packet->itemList->GetItem(i);
		sb.ClearStr();
		Net::ASN1Util::OIDToString(item->oid, item->oidLen, &sb);
		me->lvResults->AddItem(sb.ToString(), 0);
		sb.ClearStr();
		Net::ASN1OIDDB::OIDToNameString(item->oid, item->oidLen, &sb);
		me->lvResults->SetSubItem(i, 1, sb.ToString());
		me->lvResults->SetSubItem(i, 2, Net::SNMPUtil::TypeGetName(item->valType));
		if (item->valBuff)
		{
			sb.ClearStr();
			Net::SNMPInfo::ValueToString(item->valType, item->valBuff, item->valLen, &sb);
			me->lvResults->SetSubItem(i, 3, sb.ToString());
		}
		i++;
	}
}

void __stdcall SSWR::AVIRead::AVIRSNMPTrapMonitorForm::OnTimerTick(void *userObj)
{
	SSWR::AVIRead::AVIRSNMPTrapMonitorForm *me = (SSWR::AVIRead::AVIRSNMPTrapMonitorForm*)userObj;
	UOSInt i = me->lbResults->GetCount();
	UOSInt j = me->packetList->GetCount();
	SNMPPacket *packet;
	if (i < j)
	{
		Data::DateTime dt;
		UTF8Char sbuff[32];
		Sync::MutexUsage mutUsage(me->packetMut);
		while (i < j)
		{
			packet = me->packetList->GetItem(i);
			dt.SetTicks(packet->t);
			dt.ToLocalTime();
			dt.ToString(sbuff, "yyyy-MM-dd HH:mm:ss.fff");
			me->lbResults->AddItem(sbuff, packet);
			i++;
		}
		mutUsage.EndUse();
	}
}

Bool __stdcall SSWR::AVIRead::AVIRSNMPTrapMonitorForm::OnSNMPTrapPacket(void *userObj, const Net::SocketUtil::AddressInfo *addr, UInt16 port, const Net::SNMPUtil::TrapInfo *trap, Data::ArrayList<Net::SNMPUtil::BindingItem*> *itemList)
{
	SSWR::AVIRead::AVIRSNMPTrapMonitorForm *me = (SSWR::AVIRead::AVIRSNMPTrapMonitorForm*)userObj;
	SNMPPacket *packet;
	Data::DateTime dt;
	dt.SetCurrTimeUTC();
	packet = MemAlloc(SNMPPacket, 1);
	packet->t = dt.ToTicks();
	packet->addr = *addr;
	packet->port = port;
	MemCopyNO(&packet->trap, trap, sizeof(Net::SNMPUtil::TrapInfo));
	NEW_CLASS(packet->itemList, Data::ArrayList<Net::SNMPUtil::BindingItem*>());
	packet->itemList->AddAll(itemList);
	Sync::MutexUsage mutUsage(me->packetMut);
	me->packetList->Add(packet);
	mutUsage.EndUse();
	return true;
}

SSWR::AVIRead::AVIRSNMPTrapMonitorForm::AVIRSNMPTrapMonitorForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core) : UI::GUIForm(parent, 1024, 768, ui)
{
	this->SetFont(0, 0, 8.25, false);
	this->SetText((const UTF8Char*)"SNMP Trap Monitor");

	this->core = core;
	NEW_CLASS(this->packetMut, Sync::Mutex());
	NEW_CLASS(this->packetList, Data::ArrayList<SNMPPacket*>());
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
	NEW_CLASS(this->lblCommunity, UI::GUILabel(ui, this->pnlTrap, (const UTF8Char*)"Community"));
	this->lblCommunity->SetRect(4, 4, 100, 23, false);
	NEW_CLASS(this->txtCommunity, UI::GUITextBox(ui, this->pnlTrap, (const UTF8Char*)""));
	this->txtCommunity->SetRect(104, 4, 300, 23, false);
	this->txtCommunity->SetReadOnly(true);
	NEW_CLASS(this->lblEnterpriseOID, UI::GUILabel(ui, this->pnlTrap, (const UTF8Char*)"Enterprise OID"));
	this->lblEnterpriseOID->SetRect(4, 28, 100, 23, false);
	NEW_CLASS(this->txtEnterpriseOID, UI::GUITextBox(ui, this->pnlTrap, (const UTF8Char*)""));
	this->txtEnterpriseOID->SetRect(104, 28, 300, 23, false);
	this->txtEnterpriseOID->SetReadOnly(true);
	NEW_CLASS(this->lblEnterpriseName, UI::GUILabel(ui, this->pnlTrap, (const UTF8Char*)"Enterprise Name"));
	this->lblEnterpriseName->SetRect(4, 52, 100, 23, false);
	NEW_CLASS(this->txtEnterpriseName, UI::GUITextBox(ui, this->pnlTrap, (const UTF8Char*)""));
	this->txtEnterpriseName->SetRect(104, 52, 300, 23, false);
	this->txtEnterpriseName->SetReadOnly(true);
	NEW_CLASS(this->lblRemoteIP, UI::GUILabel(ui, this->pnlTrap, (const UTF8Char*)"Remote IP"));
	this->lblRemoteIP->SetRect(4, 76, 100, 23, false);
	NEW_CLASS(this->txtRemoteIP, UI::GUITextBox(ui, this->pnlTrap, (const UTF8Char*)""));
	this->txtRemoteIP->SetRect(104, 76, 150, 23, false);
	this->txtRemoteIP->SetReadOnly(true);
	NEW_CLASS(this->lblRemotePort, UI::GUILabel(ui, this->pnlTrap, (const UTF8Char*)"Remote Port"));
	this->lblRemotePort->SetRect(4, 100, 100, 23, false);
	NEW_CLASS(this->txtRemotePort, UI::GUITextBox(ui, this->pnlTrap, (const UTF8Char*)""));
	this->txtRemotePort->SetRect(104, 100, 100, 23, false);
	this->txtRemotePort->SetReadOnly(true);
	NEW_CLASS(this->lblAgentAddr, UI::GUILabel(ui, this->pnlTrap, (const UTF8Char*)"Agent Addr"));
	this->lblAgentAddr->SetRect(4, 124, 100, 23, false);
	NEW_CLASS(this->txtAgentAddr, UI::GUITextBox(ui, this->pnlTrap, (const UTF8Char*)""));
	this->txtAgentAddr->SetRect(104, 124, 150, 23, false);
	this->txtAgentAddr->SetReadOnly(true);
	NEW_CLASS(this->lblGenericTrap, UI::GUILabel(ui, this->pnlTrap, (const UTF8Char*)"Generic Trap"));
	this->lblGenericTrap->SetRect(4, 148, 100, 23, false);
	NEW_CLASS(this->txtGenericTrap, UI::GUITextBox(ui, this->pnlTrap, (const UTF8Char*)""));
	this->txtGenericTrap->SetRect(104, 148, 100, 23, false);
	this->txtGenericTrap->SetReadOnly(true);
	NEW_CLASS(this->lblSpecificTrap, UI::GUILabel(ui, this->pnlTrap, (const UTF8Char*)"Specific Trap"));
	this->lblSpecificTrap->SetRect(4, 172, 100, 23, false);
	NEW_CLASS(this->txtSpecificTrap, UI::GUITextBox(ui, this->pnlTrap, (const UTF8Char*)""));
	this->txtSpecificTrap->SetRect(104, 172, 100, 23, false);
	this->txtSpecificTrap->SetReadOnly(true);
	NEW_CLASS(this->lblRecvTime, UI::GUILabel(ui, this->pnlTrap, (const UTF8Char*)"Recv Time"));
	this->lblRecvTime->SetRect(4, 196, 100, 23, false);
	NEW_CLASS(this->txtRecvTime, UI::GUITextBox(ui, this->pnlTrap, (const UTF8Char*)""));
	this->txtRecvTime->SetRect(104, 196, 200, 23, false);
	this->txtRecvTime->SetReadOnly(true);
	NEW_CLASS(this->lblTrapTime, UI::GUILabel(ui, this->pnlTrap, (const UTF8Char*)"Trap Time"));
	this->lblTrapTime->SetRect(4, 220, 100, 23, false);
	NEW_CLASS(this->txtTrapTime, UI::GUITextBox(ui, this->pnlTrap, (const UTF8Char*)""));
	this->txtTrapTime->SetRect(104, 220, 200, 23, false);
	this->txtTrapTime->SetReadOnly(true);
	NEW_CLASS(this->lvResults, UI::GUIListView(ui, this->pnlResults, UI::GUIListView::LVSTYLE_TABLE, 4));
	this->lvResults->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lvResults->SetFullRowSelect(true);
	this->lvResults->SetShowGrid(true);
	this->lvResults->AddColumn((const UTF8Char*)"OID", 150);
	this->lvResults->AddColumn((const UTF8Char*)"Name", 150);
	this->lvResults->AddColumn((const UTF8Char*)"ValueType", 100);
	this->lvResults->AddColumn((const UTF8Char*)"Value", 200);

	this->AddTimer(1000, OnTimerTick, this);

	NEW_CLASS(this->mon, Net::SNMPTrapMonitor(this->core->GetSocketFactory(), OnSNMPTrapPacket, this));
	if (this->mon->IsError())
	{
		UI::MessageDialog::ShowDialog((const UTF8Char*)"Error in starting SNMP Trap Monitor", (const UTF8Char*)"Error", this);
	}
}

SSWR::AVIRead::AVIRSNMPTrapMonitorForm::~AVIRSNMPTrapMonitorForm()
{
	DEL_CLASS(this->mon);
	SNMPPacket *packet;
	UOSInt i = this->packetList->GetCount();
	UOSInt j;
	while (i-- > 0)
	{
		packet = this->packetList->GetItem(i);
		j = packet->itemList->GetCount();
		while (j-- > 0)
		{
			Net::SNMPUtil::FreeBindingItem(packet->itemList->GetItem(j));
		}
		DEL_CLASS(packet->itemList);
		MemFree(packet);
	}
	DEL_CLASS(this->packetMut);
	DEL_CLASS(this->packetList);
}

void SSWR::AVIRead::AVIRSNMPTrapMonitorForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}

Bool SSWR::AVIRead::AVIRSNMPTrapMonitorForm::IsError()
{
	return this->mon->IsError();
}
