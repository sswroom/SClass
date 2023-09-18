#include "Stdafx.h"
#include "Net/SSLEngineFactory.h"
#include "Net/UserAgentDB.h"
#include "SSWR/AVIRead/AVIRSNSManagerForm.h"

void __stdcall SSWR::AVIRead::AVIRSNSManagerForm::OnChannelAddClicked(void *userObj)
{
	SSWR::AVIRead::AVIRSNSManagerForm *me = (SSWR::AVIRead::AVIRSNSManagerForm*)userObj;
	Net::SNS::SNSControl::SNSType typ = (Net::SNS::SNSControl::SNSType)(OSInt)me->cboChannel->GetSelectedItem();
	Text::StringBuilderUTF8 sb;
	me->txtChannelId->GetText(sb);
	Net::SNS::SNSControl *ctrl = me->mgr->AddChannel(typ, sb.ToCString());
	if (ctrl)
	{
		me->lbChannels->AddItem(ctrl->GetName(), ctrl);
	}
}

void __stdcall SSWR::AVIRead::AVIRSNSManagerForm::OnChannelsSelChg(void *userObj)
{
	SSWR::AVIRead::AVIRSNSManagerForm *me = (SSWR::AVIRead::AVIRSNSManagerForm*)userObj;
	Net::SNS::SNSControl *ctrl = (Net::SNS::SNSControl*)me->lbChannels->GetSelectedItem();
	me->lvCurrItems->ClearItems();
	if (ctrl)
	{
		Net::SNS::SNSControl::SNSItem *item;
		Data::ArrayList<Net::SNS::SNSControl::SNSItem *> itemList;
		UOSInt i;
		UOSInt j;
		Data::DateTime dt;
		NotNullPtr<Text::String> s;
		UTF8Char sbuff[32];
		UTF8Char *sptr;
		Sync::MutexUsage mutUsage;
		me->mgr->Use(mutUsage);
		ctrl->GetCurrItems(itemList);
		i = itemList.GetCount();
		while (i-- > 0)
		{
			item = itemList.GetItem(i);
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

SSWR::AVIRead::AVIRSNSManagerForm::AVIRSNSManagerForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core) : UI::GUIForm(parent, 1024, 768, ui)
{
	this->SetText(CSTR("SNS Manager"));
	this->SetFont(UTF8STRC("MingLiu"), 8.25, false);

	this->core = core;
	this->ssl = Net::SSLEngineFactory::Create(this->core->GetSocketFactory(), true);
	Text::CString userAgent = Net::UserAgentDB::FindUserAgent(Manage::OSInfo::OT_WINDOWS_NT64, Net::BrowserInfo::BT_FIREFOX);
	NEW_CLASS(this->mgr, Net::SNS::SNSManager(core->GetSocketFactory(), this->ssl, core->GetEncFactory(), userAgent, CSTR_NULL, this->core->GetLog()));
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	NEW_CLASS(this->pnlChannel, UI::GUIPanel(ui, this));
	this->pnlChannel->SetRect(0, 0, 100, 31, false);
	this->pnlChannel->SetDockType(UI::GUIControl::DOCK_TOP);
	NEW_CLASS(this->cboChannel, UI::GUIComboBox(ui, this->pnlChannel, false));
	this->cboChannel->SetRect(4, 4, 100, 23, false);
	NEW_CLASS(this->txtChannelId, UI::GUITextBox(ui, this->pnlChannel, CSTR("")));
	this->txtChannelId->SetRect(104, 4, 300, 23, false);
	NEW_CLASS(this->btnChannelAdd, UI::GUIButton(ui, this->pnlChannel, CSTR("Add")));
	this->btnChannelAdd->SetRect(404, 4, 75, 23, false);
	this->btnChannelAdd->HandleButtonClick(OnChannelAddClicked, this);
	NEW_CLASS(this->lbChannels, UI::GUIListBox(ui, this, false));
	this->lbChannels->SetRect(0, 0, 150, 23, false);
	this->lbChannels->SetDockType(UI::GUIControl::DOCK_LEFT);
	this->lbChannels->HandleSelectionChange(OnChannelsSelChg, this);
	NEW_CLASS(this->tcChannels, UI::GUITabControl(ui, this));
	this->tcChannels->SetDockType(UI::GUIControl::DOCK_FILL);

	this->tpCurrItems = this->tcChannels->AddTabPage(CSTR("Curr Items"));
	NEW_CLASS(this->lvCurrItems, UI::GUIListView(ui, this->tpCurrItems, UI::GUIListView::LVSTYLE_TABLE, 4));
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
	Net::SNS::SNSControl *ctrl;
	UOSInt j = this->mgr->GetCount();
	i = 0;
	while (i < j)
	{
		ctrl = this->mgr->GetItem(i);
		this->lbChannels->AddItem(ctrl->GetName(), ctrl);
		i++;
	}
}

SSWR::AVIRead::AVIRSNSManagerForm::~AVIRSNSManagerForm()
{
	DEL_CLASS(this->mgr);
	SDEL_CLASS(this->ssl);
}

void SSWR::AVIRead::AVIRSNSManagerForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
