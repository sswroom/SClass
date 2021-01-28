#include "Stdafx.h"
#include "Data/ByteTool.h"
#include "Net/SNMPClient.h"
#include "Net/SNMPInfo.h"
#include "Net/SNMPOIDDB.h"
#include "SSWR/AVIRead/AVIRSNMPWalkForm.h"
#include "UI/MessageDialog.h"

SSWR::AVIRead::AVIRSNMPWalkForm::AVIRSNMPWalkForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core, const Net::SocketUtil::AddressInfo *addr, const UTF8Char *community) : UI::GUIForm(parent, 1024, 768, ui)
{
	UTF8Char sbuff[128];
	this->SetFont(0, 8.25, false);
	this->SetText((const UTF8Char*)"SNMP Walk");

	this->core = core;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	NEW_CLASS(this->pnlRequest, UI::GUIPanel(ui, this));
	this->pnlRequest->SetRect(0, 0, 100, 31, false);
	this->pnlRequest->SetDockType(UI::GUIControl::DOCK_TOP);
	NEW_CLASS(this->lblAgent, UI::GUILabel(ui, this->pnlRequest, (const UTF8Char*)"Agent"));
	this->lblAgent->SetRect(4, 4, 100, 23, false);
	Net::SocketUtil::GetAddrName(sbuff, addr);
	NEW_CLASS(this->txtAgent, UI::GUITextBox(ui, this->pnlRequest, sbuff));
	this->txtAgent->SetRect(104, 4, 150, 23, false);
	this->txtAgent->SetReadOnly(true);
	NEW_CLASS(this->lvResults, UI::GUIListView(ui, this, UI::GUIListView::LVSTYLE_TABLE, 4));
	this->lvResults->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lvResults->SetFullRowSelect(true);
	this->lvResults->SetShowGrid(true);
	this->lvResults->AddColumn((const UTF8Char*)"OID", 150);
	this->lvResults->AddColumn((const UTF8Char*)"Name", 150);
	this->lvResults->AddColumn((const UTF8Char*)"ValueType", 100);
	this->lvResults->AddColumn((const UTF8Char*)"Value", 200);

	Net::SNMPClient *cli;
	NEW_CLASS(cli, Net::SNMPClient(this->core->GetSocketFactory()));
	if (!cli->IsError())
	{
		Data::ArrayList<Net::SNMPUtil::BindingItem*> itemList;
		Net::SNMPUtil::ErrorStatus err;
		OSInt i;
		OSInt j;
		Text::StringBuilderUTF8 sb;
		Net::SNMPUtil::BindingItem *item;
		err = cli->V1Walk(addr, community, (const UTF8Char*)"1.3.6.1.2.1", &itemList);
		this->lvResults->ClearItems();
		if (err != Net::SNMPUtil::ES_NOERROR)
		{
		}
		else
		{
			i = 0;
			j = itemList.GetCount();
			while (i < j)
			{
				item = itemList.GetItem(i);
				sb.ClearStr();
				Net::SNMPUtil::OIDToString(item->oid, item->oidLen, &sb);
				this->lvResults->AddItem(sb.ToString(), 0);
				sb.ClearStr();
				Net::SNMPOIDDB::OIDToNameString(item->oid, item->oidLen, &sb);
				this->lvResults->SetSubItem(i, 1, sb.ToString());
				this->lvResults->SetSubItem(i, 2, Net::SNMPUtil::TypeGetName(item->valType));
				if (item->valBuff)
				{
					sb.ClearStr();
					Net::SNMPInfo::ValueToString(item->valType, item->valBuff, item->valLen, &sb);
					this->lvResults->SetSubItem(i, 3, sb.ToString());
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
	DEL_CLASS(cli);
}

SSWR::AVIRead::AVIRSNMPWalkForm::~AVIRSNMPWalkForm()
{
}

void SSWR::AVIRead::AVIRSNMPWalkForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
