#include "Stdafx.h"
#include "Data/ByteTool.h"
#include "Net/ASN1OIDDB.h"
#include "Net/ASN1Util.h"
#include "Net/SNMPInfo.h"
#include "SSWR/AVIRead/AVIRSNMPClientForm.h"
#include "UI/MessageDialog.h"

void __stdcall SSWR::AVIRead::AVIRSNMPClientForm::OnRequestClicked(void *userObj)
{
	SSWR::AVIRead::AVIRSNMPClientForm *me = (SSWR::AVIRead::AVIRSNMPClientForm*)userObj;
	Text::StringBuilderUTF8 sbComm;
	Text::StringBuilderUTF8 sbOID;
	Net::SocketUtil::AddressInfo addr;
	me->txtAgent->GetText(&sbComm);
	if (!me->core->GetSocketFactory()->DNSResolveIP(sbComm.ToString(), sbComm.GetLength(), &addr))
	{
		UI::MessageDialog::ShowDialog((const UTF8Char*)"Error in resolving Agent Address", (const UTF8Char*)"SNMP Client", me);
		return;
	}
	sbComm.ClearStr();
	me->txtCommunity->GetText(&sbComm);
	me->txtOID->GetText(&sbOID);
	if (sbComm.GetLength() <= 0)
	{
		UI::MessageDialog::ShowDialog((const UTF8Char*)"Please enter community", (const UTF8Char*)"SNMP Client", me);
		return;
	}
	if (sbOID.GetLength() <= 0)
	{
		UI::MessageDialog::ShowDialog((const UTF8Char*)"Please enter OID", (const UTF8Char*)"SNMP Client", me);
		return;
	}
	UOSInt i = me->cboCommandType->GetSelectedIndex();
	Data::ArrayList<Net::SNMPUtil::BindingItem*> itemList;
	Net::SNMPUtil::ErrorStatus err;
	Text::String *community = Text::String::New(sbComm.ToString(), sbComm.GetLength());
	if (i == 0)
	{
		err = me->cli->V1GetRequest(&addr, community, sbOID.ToString(), sbOID.GetLength(), &itemList);
	}
	else if (i == 1)
	{
		err = me->cli->V1GetNextRequest(&addr, community, sbOID.ToString(), sbOID.GetLength(), &itemList);
	}
	else
	{
		err = me->cli->V1Walk(&addr, community, sbOID.ToString(), sbOID.GetLength(), &itemList);
	}
	community->Release();
	UOSInt j;
	Text::StringBuilderUTF8 sb;
	Net::SNMPUtil::BindingItem *item;
	me->lvResults->ClearItems();
	if (err != Net::SNMPUtil::ES_NOERROR)
	{
		sb.ClearStr();
		sb.AppendC(UTF8STRC("Error in requesting to the server, error code = "));
		sb.Append(Net::SNMPUtil::ErrorStatusToString(err));
		UI::MessageDialog::ShowDialog(sb.ToString(), (const UTF8Char*)"SNMP Client", me);
	}
	else
	{
		i = 0;
		j = itemList.GetCount();
		while (i < j)
		{
			item = itemList.GetItem(i);
			sb.ClearStr();
			Net::ASN1Util::OIDToString(item->oid, item->oidLen, &sb);
			me->lvResults->AddItem(sb.ToString(), 0);
			sb.ClearStr();
			Net::ASN1OIDDB::OIDToNameString(item->oid, item->oidLen, &sb);
			me->lvResults->SetSubItem(i, 1, sb.ToString());
			me->lvResults->SetSubItem(i, 2, Net::SNMPUtil::TypeGetName(item->valType).v);
			if (item->valBuff)
			{
				sb.ClearStr();
				Net::SNMPInfo::ValueToString(item->valType, item->valBuff, item->valLen, &sb);
				me->lvResults->SetSubItem(i, 3, sb.ToString());
			}
			i++;
		}
	}
	i = itemList.GetCount();
	while (i-- > 0)
	{
		Net::SNMPUtil::FreeBindingItem(itemList.GetItem(i));
	}
}

void __stdcall SSWR::AVIRead::AVIRSNMPClientForm::OnTimerTick(void *userObj)
{
//	SSWR::AVIRead::AVIRSNMPClientForm *me = (SSWR::AVIRead::AVIRSNMPClientForm*)userObj;
}

SSWR::AVIRead::AVIRSNMPClientForm::AVIRSNMPClientForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core) : UI::GUIForm(parent, 1024, 768, ui)
{
	this->SetFont(0, 0, 8.25, false);
	this->SetText((const UTF8Char*)"SNMP Client");

	this->core = core;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	NEW_CLASS(this->pnlRequest, UI::GUIPanel(ui, this));
	this->pnlRequest->SetRect(0, 0, 100, 127, false);
	this->pnlRequest->SetDockType(UI::GUIControl::DOCK_TOP);
	NEW_CLASS(this->lblAgent, UI::GUILabel(ui, this->pnlRequest, (const UTF8Char*)"Agent"));
	this->lblAgent->SetRect(4, 4, 100, 23, false);
	NEW_CLASS(this->txtAgent, UI::GUITextBox(ui, this->pnlRequest, (const UTF8Char*)"127.0.0.1"));
	this->txtAgent->SetRect(104, 4, 150, 23, false);
	NEW_CLASS(this->lblCommunity, UI::GUILabel(ui, this->pnlRequest, (const UTF8Char*)"Community"));
	this->lblCommunity->SetRect(4, 28, 100, 23, false);
	NEW_CLASS(this->txtCommunity, UI::GUITextBox(ui, this->pnlRequest, (const UTF8Char*)"public"));
	this->txtCommunity->SetRect(104, 28, 120, 23, false);
	NEW_CLASS(this->lblOID, UI::GUILabel(ui, this->pnlRequest, (const UTF8Char*)"OID"));
	this->lblOID->SetRect(4, 52, 100, 23, false);
	NEW_CLASS(this->txtOID, UI::GUITextBox(ui, this->pnlRequest, (const UTF8Char*)"1.3.6.1.2.1"));
	this->txtOID->SetRect(104, 52, 200, 23, false);
	NEW_CLASS(this->lblCommandType, UI::GUILabel(ui, this->pnlRequest, (const UTF8Char*)"Command Type"));
	this->lblCommandType->SetRect(4, 76, 100, 23, false);
	NEW_CLASS(this->cboCommandType, UI::GUIComboBox(ui, this->pnlRequest, false));
	this->cboCommandType->SetRect(104, 76, 120, 23, false);
	this->cboCommandType->AddItem((const UTF8Char*)"GetRequest", (void*)1);
	this->cboCommandType->AddItem((const UTF8Char*)"GetNextRequest", (void*)2);
	this->cboCommandType->AddItem((const UTF8Char*)"Walk", (void*)3);
	this->cboCommandType->SetSelectedIndex(2);
	NEW_CLASS(this->btnRequest, UI::GUIButton(ui, this->pnlRequest, (const UTF8Char*)"Request"));
	this->btnRequest->SetRect(104, 100, 75, 23, false);
	this->btnRequest->HandleButtonClick(OnRequestClicked, this);

	NEW_CLASS(this->lvResults, UI::GUIListView(ui, this, UI::GUIListView::LVSTYLE_TABLE, 4));
	this->lvResults->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lvResults->SetFullRowSelect(true);
	this->lvResults->SetShowGrid(true);
	this->lvResults->AddColumn((const UTF8Char*)"OID", 150);
	this->lvResults->AddColumn((const UTF8Char*)"Name", 150);
	this->lvResults->AddColumn((const UTF8Char*)"ValueType", 100);
	this->lvResults->AddColumn((const UTF8Char*)"Value", 200);

	this->AddTimer(1000, OnTimerTick, this);

	NEW_CLASS(this->cli, Net::SNMPClient(this->core->GetSocketFactory()));
	if (this->cli->IsError())
	{
		UI::MessageDialog::ShowDialog((const UTF8Char*)"Error in starting SNMP Client", (const UTF8Char*)"Error", this);
	}
}

SSWR::AVIRead::AVIRSNMPClientForm::~AVIRSNMPClientForm()
{
	DEL_CLASS(this->cli);
}

void SSWR::AVIRead::AVIRSNMPClientForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}

Bool SSWR::AVIRead::AVIRSNMPClientForm::IsError()
{
	return this->cli->IsError();
}
