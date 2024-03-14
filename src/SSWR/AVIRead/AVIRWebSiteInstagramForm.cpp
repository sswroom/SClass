#include "Stdafx.h"
#include "Net/SSLEngineFactory.h"
#include "Net/UserAgentDB.h"
#include "SSWR/AVIRead/AVIRWebSiteInstagramForm.h"

void __stdcall SSWR::AVIRead::AVIRWebSiteInstagramForm::OnRequestUserClicked(AnyType userObj)
{
	NotNullPtr<SSWR::AVIRead::AVIRWebSiteInstagramForm> me = userObj.GetNN<SSWR::AVIRead::AVIRWebSiteInstagramForm>();
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
		Net::WebSite::WebSiteInstagramControl::ChannelInfo chInfo;
		MemClear(&chInfo, sizeof(chInfo));
		Data::ArrayList<Net::WebSite::WebSiteInstagramControl::ItemData*> itemList;
		Net::WebSite::WebSiteInstagramControl::ItemData *item;
		NotNullPtr<Text::String> s = Text::String::New(sb.ToString(), sb.GetLength());
		me->ctrl->GetChannelItems(s, 0, &itemList, &chInfo);
		s->Release();
		i = 0;
		j = itemList.GetCount();
		while (i < j)
		{
			item = itemList.GetItem(i);
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
			me->lvItems->SetSubItem(i, 3, item->moreImages?CSTR("true"):CSTR("false"));
			sb.ClearStr();
			sb.Append(item->message);
			sb.Replace('\r', ' ');
			sb.Replace('\n', ' ');
			me->lvItems->SetSubItem(i, 4, sb.ToCString());
			i++;
		}
		me->ctrl->FreeItems(&itemList);
		me->ctrl->FreeChannelInfo(&chInfo);
	}
}

void __stdcall SSWR::AVIRead::AVIRWebSiteInstagramForm::OnPageClicked(AnyType userObj)
{
	NotNullPtr<SSWR::AVIRead::AVIRWebSiteInstagramForm> me = userObj.GetNN<SSWR::AVIRead::AVIRWebSiteInstagramForm>();
	Text::StringBuilderUTF8 sb;
	me->txtShortCode->GetText(sb);
	me->lbImageURL->ClearItems();
	if (sb.GetLength() > 0)
	{
		Data::ArrayListStringNN imageList;
		Data::ArrayListStringNN videoList;
		NotNullPtr<Text::String> s = Text::String::New(sb.ToString(), sb.GetLength());
		me->ctrl->GetPageImages(s, &imageList, &videoList);
		s->Release();
		Data::ArrayIterator<NotNullPtr<Text::String>> it = imageList.Iterator();
		while (it.HasNext())
		{
			NotNullPtr<Text::String> s = it.Next();
			me->lbImageURL->AddItem(s, 0);
			s->Release();
		}

		it = videoList.Iterator();
		while (it.HasNext())
		{
			NotNullPtr<Text::String> s = it.Next();
			me->lbImageURL->AddItem(s, 0);
			s->Release();
		}
	}
}

SSWR::AVIRead::AVIRWebSiteInstagramForm::AVIRWebSiteInstagramForm(Optional<UI::GUIClientControl> parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core) : UI::GUIForm(parent, 1024, 768, ui)
{
	this->SetText(CSTR("Instagram"));
	this->SetFont(UTF8STRC("MingLiu"), 8.25, false);

	this->core = core;
	this->ssl = Net::SSLEngineFactory::Create(this->core->GetSocketFactory(), true);
	Text::CString userAgent = Net::UserAgentDB::FindUserAgent(Manage::OSInfo::OT_WINDOWS_NT64, Net::BrowserInfo::BT_FIREFOX);
	NotNullPtr<Text::String> ua = Text::String::New(userAgent);
	NEW_CLASS(this->ctrl, Net::WebSite::WebSiteInstagramControl(core->GetSocketFactory(), this->ssl, core->GetEncFactory(), ua.Ptr()));
	ua->Release();
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	this->tcMain = ui->NewTabControl(*this);
	this->tcMain->SetDockType(UI::GUIControl::DOCK_FILL);

	this->tpUser = this->tcMain->AddTabPage(CSTR("User"));
	this->pnlRequest = ui->NewPanel(this->tpUser);
	this->pnlRequest->SetRect(0, 0, 100, 31, false);
	this->pnlRequest->SetDockType(UI::GUIControl::DOCK_TOP);
	this->lblChannelId = ui->NewLabel(this->pnlRequest, CSTR("ChannelId"));
	this->lblChannelId->SetRect(4, 4, 100, 23, false);
	this->txtChannelId = ui->NewTextBox(this->pnlRequest, CSTR(""));
	this->txtChannelId->SetRect(104, 4, 150, 23, false);
	this->btnRequestUser = ui->NewButton(this->pnlRequest, CSTR("Request"));
	this->btnRequestUser->SetRect(254, 4, 75, 23, false);
	this->btnRequestUser->HandleButtonClick(OnRequestUserClicked, this);
	this->lvItems = ui->NewListView(this->tpUser, UI::ListViewStyle::Table, 5);
	this->lvItems->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lvItems->SetFullRowSelect(true);
	this->lvItems->SetShowGrid(true);
	this->lvItems->AddColumn(CSTR("Id"), 100);
	this->lvItems->AddColumn(CSTR("Date"), 120);
	this->lvItems->AddColumn(CSTR("ImageURL"), 200);
	this->lvItems->AddColumn(CSTR("MoreImages"), 60);
	this->lvItems->AddColumn(CSTR("Message"), 400);

	this->tpPage = this->tcMain->AddTabPage(CSTR("Page"));
	this->pnlPage = ui->NewPanel(this->tpPage);
	this->pnlPage->SetRect(0, 0, 100, 31, false);
	this->pnlPage->SetDockType(UI::GUIControl::DOCK_TOP);
	this->lblShortCode = ui->NewLabel(this->pnlPage, CSTR("ShortCode"));
	this->lblShortCode->SetRect(4, 4, 100, 23, false);
	this->txtShortCode = ui->NewTextBox(this->pnlPage, CSTR(""));
	this->txtShortCode->SetRect(104, 4, 150, 23, false);
	this->btnPage = ui->NewButton(this->pnlPage, CSTR("Request"));
	this->btnPage->SetRect(254, 4, 75, 23, false);
	this->btnPage->HandleButtonClick(OnPageClicked, this);
	this->lbImageURL = ui->NewListBox(this->tpPage, false);
	this->lbImageURL->SetDockType(UI::GUIControl::DOCK_FILL);
}

SSWR::AVIRead::AVIRWebSiteInstagramForm::~AVIRWebSiteInstagramForm()
{
	DEL_CLASS(this->ctrl);
	this->ssl.Delete();
}

void SSWR::AVIRead::AVIRWebSiteInstagramForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
