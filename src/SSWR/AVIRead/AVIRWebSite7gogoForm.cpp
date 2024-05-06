#include "Stdafx.h"
#include "Net/SSLEngineFactory.h"
#include "Net/UserAgentDB.h"
#include "SSWR/AVIRead/AVIRWebSite7gogoForm.h"

void __stdcall SSWR::AVIRead::AVIRWebSite7gogoForm::OnRequestPageClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRWebSite7gogoForm> me = userObj.GetNN<SSWR::AVIRead::AVIRWebSite7gogoForm>();
	Text::StringBuilderUTF8 sb;
	UTF8Char sbuff[64];
	UTF8Char *sptr;
	me->txtChannelId->GetText(sb);
	me->lvItems->ClearItems();
	if (sb.GetLength() > 0)
	{
		Data::DateTime dt;
		UOSInt i;
		UOSInt j;
		Net::WebSite::WebSite7gogoControl::ChannelInfo chInfo;
		MemClear(&chInfo, sizeof(chInfo));
		Data::ArrayListNN<Net::WebSite::WebSite7gogoControl::ItemData> itemList;
		NN<Net::WebSite::WebSite7gogoControl::ItemData> item;
		NN<Text::String> s = Text::String::New(sb.ToString(), sb.GetLength());
		me->ctrl->GetChannelItems(s, 0, itemList, chInfo);
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
			if (item->imgURL)
			{
				sb.ClearStr();
				sb.Append(item->imgURL);
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
		me->ctrl->FreeChannelInfo(chInfo);
	}
}

SSWR::AVIRead::AVIRWebSite7gogoForm::AVIRWebSite7gogoForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core) : UI::GUIForm(parent, 1024, 768, ui)
{
	this->SetText(CSTR("755"));
	this->SetFont(UTF8STRC("MingLiu"), 8.25, false);

	this->core = core;
	this->ssl = Net::SSLEngineFactory::Create(this->core->GetSocketFactory(), true);
//	const UTF8Char *userAgent = 0;//Net::UserAgentDB::FindUserAgent(Manage::OSInfo::OT_WINDOWS_NT64, Net::BrowserInfo::BT_FIREFOX);
	NEW_CLASS(this->ctrl, Net::WebSite::WebSite7gogoControl(core->GetSocketFactory(), this->ssl, core->GetEncFactory(), 0));
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

SSWR::AVIRead::AVIRWebSite7gogoForm::~AVIRWebSite7gogoForm()
{
	DEL_CLASS(this->ctrl);
	this->ssl.Delete();
}

void SSWR::AVIRead::AVIRWebSite7gogoForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
