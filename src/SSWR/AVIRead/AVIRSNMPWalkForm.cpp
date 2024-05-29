#include "Stdafx.h"
#include "Data/ByteTool.h"
#include "Net/ASN1OIDDB.h"
#include "Net/ASN1Util.h"
#include "Net/SNMPClient.h"
#include "Net/SNMPInfo.h"
#include "SSWR/AVIRead/AVIRSNMPWalkForm.h"

SSWR::AVIRead::AVIRSNMPWalkForm::AVIRSNMPWalkForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core, NN<const Net::SocketUtil::AddressInfo> addr, NN<Text::String> community) : UI::GUIForm(parent, 1024, 768, ui)
{
	UTF8Char sbuff[128];
	UTF8Char *sptr;
	this->SetFont(0, 0, 8.25, false);
	this->SetText(CSTR("SNMP Walk"));

	this->core = core;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	this->pnlRequest = ui->NewPanel(*this);
	this->pnlRequest->SetRect(0, 0, 100, 31, false);
	this->pnlRequest->SetDockType(UI::GUIControl::DOCK_TOP);
	this->lblAgent = ui->NewLabel(this->pnlRequest, CSTR("Agent"));
	this->lblAgent->SetRect(4, 4, 100, 23, false);
	sptr=  Net::SocketUtil::GetAddrName(sbuff, addr);
	this->txtAgent = ui->NewTextBox(this->pnlRequest, CSTRP(sbuff, sptr));
	this->txtAgent->SetRect(104, 4, 150, 23, false);
	this->txtAgent->SetReadOnly(true);
	this->lvResults = ui->NewListView(*this, UI::ListViewStyle::Table, 4);
	this->lvResults->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lvResults->SetFullRowSelect(true);
	this->lvResults->SetShowGrid(true);
	this->lvResults->AddColumn(CSTR("OID"), 150);
	this->lvResults->AddColumn(CSTR("Name"), 150);
	this->lvResults->AddColumn(CSTR("ValueType"), 100);
	this->lvResults->AddColumn(CSTR("Value"), 200);

	Net::SNMPClient *cli;
	NEW_CLASS(cli, Net::SNMPClient(this->core->GetSocketFactory(), this->core->GetLog()));
	if (!cli->IsError())
	{
		Data::ArrayListNN<Net::SNMPUtil::BindingItem> itemList;
		Net::SNMPUtil::ErrorStatus err;
		UOSInt i;
		UOSInt j;
		Text::StringBuilderUTF8 sb;
		NN<Net::SNMPUtil::BindingItem> item;
		err = cli->V1Walk(addr, community, CSTR("1.3.6.1.2.1"), itemList);
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
				item = itemList.GetItemNoCheck(i);
				sb.ClearStr();
				Net::ASN1Util::OIDToString(Data::ByteArrayR(item->oid, item->oidLen), sb);
				this->lvResults->AddItem(sb.ToCString(), 0);
				sb.ClearStr();
				Net::ASN1OIDDB::OIDToNameString(Data::ByteArrayR(item->oid, item->oidLen), sb);
				this->lvResults->SetSubItem(i, 1, sb.ToCString());
				this->lvResults->SetSubItem(i, 2, Net::SNMPUtil::TypeGetName(item->valType));
				if (item->valBuff)
				{
					sb.ClearStr();
					Net::SNMPInfo::ValueToString(item->valType, Data::ByteArrayR(item->valBuff, item->valLen), sb);
					this->lvResults->SetSubItem(i, 3, sb.ToCString());
				}
				i++;
			}
		}
		itemList.FreeAll(Net::SNMPUtil::FreeBindingItem);
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
