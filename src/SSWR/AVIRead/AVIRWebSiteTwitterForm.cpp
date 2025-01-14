#include "Stdafx.h"
#include "Net/SSLEngineFactory.h"
#include "Net/UserAgentDB.h"
#include "SSWR/AVIRead/AVIRWebSiteTwitterForm.h"

void __stdcall SSWR::AVIRead::AVIRWebSiteTwitterForm::OnRequestPageClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRWebSiteTwitterForm> me = userObj.GetNN<SSWR::AVIRead::AVIRWebSiteTwitterForm>();
	Text::StringBuilderUTF8 sb;
	UTF8Char sbuff[64];
	UnsafeArray<UTF8Char> sptr;
	me->txtChannelId->GetText(sb);
	me->lvItems->ClearItems();
	if (sb.GetLength() > 0)
	{
		Data::DateTime dt;
		UOSInt i;
		UOSInt j;
		Data::ArrayListNN<Net::WebSite::WebSiteTwitterControl::ItemData> itemList;
		NN<Net::WebSite::WebSiteTwitterControl::ItemData> item;
		NN<Text::String> s = Text::String::New(sb.ToString(), sb.GetLength());
		me->ctrl->GetChannelItems(s, 0, itemList, 0);
		s->Release();
		i = 0;
		j = itemList.GetCount();
		while (i < j)
		{
			item = itemList.GetItemNoCheck(i);
			sptr = Text::StrInt64(sbuff, item->id);
			me->lvItems->AddItem(CSTRP(sbuff, sptr), 0);
			dt.SetTicks(item->recTime);
			dt.ToLocalTime();
			sptr = dt.ToString(sbuff, "yyyy-MM-dd HH:mm:ss");
			me->lvItems->SetSubItem(i, 1, CSTRP(sbuff, sptr));
			if (item->imgURL.SetTo(s))
			{
				sb.ClearStr();
				sb.Append(s);
				sb.Replace(' ', '\n');
				me->lvItems->SetSubItem(i, 2, sb.ToCString());
			}
			sb.ClearStr();
			sb.Append(item->message);
			sb.Replace('\r', ' ');
			sb.Replace('\n', ' ');
			me->lvItems->SetSubItem(i, 3, sb.ToCString());
			i++;
		}
		me->ctrl->FreeItems(itemList);
	}
}

SSWR::AVIRead::AVIRWebSiteTwitterForm::AVIRWebSiteTwitterForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core) : UI::GUIForm(parent, 1024, 768, ui)
{
	this->SetText(CSTR("Twitter"));
	this->SetFont(UTF8STRC("MingLiu"), 8.25, false);

	this->core = core;
	this->ssl = Net::SSLEngineFactory::Create(this->core->GetTCPClientFactory(), true);
	//const UnsafeArray<UTF8Char> userAgent = 0;//Net::UserAgentDB::FindUserAgent(Manage::OSInfo::OT_WINDOWS_NT64, Net::BrowserInfo::BT_FIREFOX);
	NEW_CLASSNN(this->ctrl, Net::WebSite::WebSiteTwitterControl(core->GetTCPClientFactory(), this->ssl, core->GetEncFactory(), 0));
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	this->pnlRequest = ui->NewPanel(*this);
	this->pnlRequest->SetRect(0, 0, 100, 31, false);
	this->pnlRequest->SetDockType(UI::GUIControl::DOCK_TOP);
	this->lblChannelId = ui->NewLabel(this->pnlRequest, CSTR("ChannelId"));
	this->lblChannelId->SetRect(4, 4, 100, 23, false);
	this->txtChannelId = ui->NewTextBox(this->pnlRequest, CSTR(""));
	this->txtChannelId->SetRect(104, 4, 150, 23, false);
	this->btnRequestPage = ui->NewButton(this->pnlRequest, CSTR("Request"));
	this->btnRequestPage->SetRect(254, 4, 75, 23, false);
	this->btnRequestPage->HandleButtonClick(OnRequestPageClicked, this);
	this->lvItems = ui->NewListView(*this, UI::ListViewStyle::Table, 4);
	this->lvItems->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lvItems->SetFullRowSelect(true);
	this->lvItems->SetShowGrid(true);
	this->lvItems->AddColumn(CSTR("Id"), 100);
	this->lvItems->AddColumn(CSTR("Date"), 120);
	this->lvItems->AddColumn(CSTR("ImageURL"), 200);
	this->lvItems->AddColumn(CSTR("Message"), 400);
}

SSWR::AVIRead::AVIRWebSiteTwitterForm::~AVIRWebSiteTwitterForm()
{
	this->ctrl.Delete();
	this->ssl.Delete();
}

void SSWR::AVIRead::AVIRWebSiteTwitterForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
