#include "Stdafx.h"
#include "Net/SSLEngineFactory.h"
#include "Net/UserAgentDB.h"
#include "SSWR/AVIRead/AVIRSNSManagerForm.h"

void __stdcall SSWR::AVIRead::AVIRSNSManagerForm::OnChannelAddClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRSNSManagerForm> me = userObj.GetNN<SSWR::AVIRead::AVIRSNSManagerForm>();
	Net::SNS::SNSControl::SNSType typ = (Net::SNS::SNSControl::SNSType)me->cboChannel->GetSelectedItem().GetOSInt();
	Text::StringBuilderUTF8 sb;
	me->txtChannelId->GetText(sb);
	NN<Net::SNS::SNSControl> ctrl;
	if (me->mgr->AddChannel(typ, sb.ToCString()).SetTo(ctrl))
	{
		me->lbChannels->AddItem(ctrl->GetName(), ctrl);
	}
}

void __stdcall SSWR::AVIRead::AVIRSNSManagerForm::OnChannelsSelChg(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRSNSManagerForm> me = userObj.GetNN<SSWR::AVIRead::AVIRSNSManagerForm>();
	Net::SNS::SNSControl *ctrl = (Net::SNS::SNSControl*)me->lbChannels->GetSelectedItem().p;
	me->lvCurrItems->ClearItems();
	if (ctrl)
	{
		NN<Net::SNS::SNSControl::SNSItem> item;
		Data::ArrayListNN<Net::SNS::SNSControl::SNSItem> itemList;
		UOSInt i;
		UOSInt j;
		Data::DateTime dt;
		NN<Text::String> s;
		UTF8Char sbuff[32];
		UnsafeArray<UTF8Char> sptr;
		Sync::MutexUsage mutUsage;
		me->mgr->Use(mutUsage);
		ctrl->GetCurrItems(itemList);
		i = itemList.GetCount();
		while (i-- > 0)
		{
			item = itemList.GetItemNoCheck(i);
			j = me->lvCurrItems->AddItem(item->id, 0);
			dt.SetTicks(item->msgTime);
			dt.ToLocalTime();
			sptr = dt.ToString(sbuff, "yyyy-MM-dd HH:mm:ss");
			me->lvCurrItems->SetSubItem(j, 1, CSTRP(sbuff, sptr));
			if (s.Set(item->title))
			{
				me->lvCurrItems->SetSubItem(j, 2, s);
			}
			me->lvCurrItems->SetSubItem(j, 3, item->message);
		}
	}
}

SSWR::AVIRead::AVIRSNSManagerForm::AVIRSNSManagerForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core) : UI::GUIForm(parent, 1024, 768, ui)
{
	this->SetText(CSTR("SNS Manager"));
	this->SetFont(UTF8STRC("MingLiu"), 8.25, false);

	this->core = core;
	this->ssl = Net::SSLEngineFactory::Create(this->core->GetSocketFactory(), true);
	Text::CString userAgent = Net::UserAgentDB::FindUserAgent(Manage::OSInfo::OT_WINDOWS_NT64, Net::BrowserInfo::BT_FIREFOX);
	NEW_CLASS(this->mgr, Net::SNS::SNSManager(core->GetSocketFactory(), this->ssl, core->GetEncFactory(), userAgent, CSTR_NULL, this->core->GetLog()));
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	this->pnlChannel = ui->NewPanel(*this);
	this->pnlChannel->SetRect(0, 0, 100, 31, false);
	this->pnlChannel->SetDockType(UI::GUIControl::DOCK_TOP);
	this->cboChannel = ui->NewComboBox(this->pnlChannel, false);
	this->cboChannel->SetRect(4, 4, 100, 23, false);
	this->txtChannelId = ui->NewTextBox(this->pnlChannel, CSTR(""));
	this->txtChannelId->SetRect(104, 4, 300, 23, false);
	this->btnChannelAdd = ui->NewButton(this->pnlChannel, CSTR("Add"));
	this->btnChannelAdd->SetRect(404, 4, 75, 23, false);
	this->btnChannelAdd->HandleButtonClick(OnChannelAddClicked, this);
	this->lbChannels = ui->NewListBox(*this, false);
	this->lbChannels->SetRect(0, 0, 150, 23, false);
	this->lbChannels->SetDockType(UI::GUIControl::DOCK_LEFT);
	this->lbChannels->HandleSelectionChange(OnChannelsSelChg, this);
	this->tcChannels = ui->NewTabControl(*this);
	this->tcChannels->SetDockType(UI::GUIControl::DOCK_FILL);

	this->tpCurrItems = this->tcChannels->AddTabPage(CSTR("Curr Items"));
	this->lvCurrItems = ui->NewListView(this->tpCurrItems, UI::ListViewStyle::Table, 4);
	this->lvCurrItems->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lvCurrItems->SetFullRowSelect(true);
	this->lvCurrItems->SetShowGrid(true);
	this->lvCurrItems->AddColumn(CSTR("Id"), 120);
	this->lvCurrItems->AddColumn(CSTR("Date"), 120);
	this->lvCurrItems->AddColumn(CSTR("Title"), 200);
	this->lvCurrItems->AddColumn(CSTR("Message"), 400);

	UOSInt i = Net::SNS::SNSControl::ST_FIRST;
	Text::CStringNN cstr;
	while (i <= Net::SNS::SNSControl::ST_LAST)
	{
		cstr = Net::SNS::SNSControl::SNSTypeGetName((Net::SNS::SNSControl::SNSType)i);
		this->cboChannel->AddItem(cstr, (void*)(OSInt)i);
		i++;
	}
	this->cboChannel->SetSelectedIndex(0);

	Sync::MutexUsage mutUsage;
	this->mgr->Use(mutUsage);
	NN<Net::SNS::SNSControl> ctrl;
	UOSInt j = this->mgr->GetCount();
	i = 0;
	while (i < j)
	{
		ctrl = this->mgr->GetItemNoCheck(i);
		this->lbChannels->AddItem(ctrl->GetName(), ctrl);
		i++;
	}
}

SSWR::AVIRead::AVIRSNSManagerForm::~AVIRSNSManagerForm()
{
	DEL_CLASS(this->mgr);
	this->ssl.Delete();
}

void SSWR::AVIRead::AVIRSNSManagerForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
