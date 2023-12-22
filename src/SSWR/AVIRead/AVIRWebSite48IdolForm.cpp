#include "Stdafx.h"
#include "Net/SSLEngineFactory.h"
#include "Net/UserAgentDB.h"
#include "SSWR/AVIRead/AVIRWebSite48IdolForm.h"

void __stdcall SSWR::AVIRead::AVIRWebSite48IdolForm::OnRequestPageClicked(void *userObj)
{
	SSWR::AVIRead::AVIRWebSite48IdolForm *me = (SSWR::AVIRead::AVIRWebSite48IdolForm*)userObj;
	Text::StringBuilderUTF8 sb;
	UTF8Char sbuff[64];
	UTF8Char *sptr;
	Int32 pageNo = 0;
	me->txtPageNo->GetText(sb);
	sb.ToInt32(pageNo);
	me->lvItems->ClearItems();
	if (pageNo > 0)
	{
		Data::DateTime dt;
		UOSInt i;
		UOSInt j;
		Data::ArrayList<Net::WebSite::WebSite48IdolControl::ItemData*> itemList;
		Net::WebSite::WebSite48IdolControl::ItemData *item;
		me->ctrl->GetTVPageItems(pageNo, &itemList);
		i = 0;
		j = itemList.GetCount();
		while (i < j)
		{
			item = itemList.GetItem(i);
			sptr = Text::StrInt32(sbuff, item->id);
			me->lvItems->AddItem(CSTRP(sbuff, sptr), 0);
			dt.SetTicks(item->recTime);
			sptr = dt.ToString(sbuff, "yyyy-MM-dd HH:mm:ss");
			me->lvItems->SetSubItem(i, 1, CSTRP(sbuff, sptr));
			sb.ClearStr();
			Net::WebSite::WebSite48IdolControl::Title2DisplayName(item->title, sb);
			me->lvItems->SetSubItem(i, 2, sb.ToCString());
			i++;
		}
		me->ctrl->FreeItems(&itemList);
	}
}

void __stdcall SSWR::AVIRead::AVIRWebSite48IdolForm::OnDownloadLinkClicked(void *userObj)
{
	SSWR::AVIRead::AVIRWebSite48IdolForm *me = (SSWR::AVIRead::AVIRWebSite48IdolForm*)userObj;
	Text::StringBuilderUTF8 sb;
	Int32 videoId = 0;
	me->txtVideoId->GetText(sb);
	sb.ToInt32(videoId);
	if (videoId != 0)
	{
		sb.ClearStr();
		if (me->ctrl->GetDownloadLink(videoId, 0, sb))
		{
			me->txtDownloadLink->SetText(sb.ToCString());
		}
		else
		{
			me->txtDownloadLink->SetText(CSTR(""));
		}
	}
}

void __stdcall SSWR::AVIRead::AVIRWebSite48IdolForm::OnVideoNameClicked(void *userObj)
{
	SSWR::AVIRead::AVIRWebSite48IdolForm *me = (SSWR::AVIRead::AVIRWebSite48IdolForm*)userObj;
	Text::StringBuilderUTF8 sb;
	Int32 videoId = 0;
	me->txtNameVideoId->GetText(sb);
	sb.ToInt32(videoId);
	if (videoId != 0)
	{
		sb.ClearStr();
		if (me->ctrl->GetVideoName(videoId, sb))
		{
			me->txtVideoName->SetText(sb.ToCString());
		}
		else
		{
			me->txtVideoName->SetText(CSTR(""));
		}
	}
}

SSWR::AVIRead::AVIRWebSite48IdolForm::AVIRWebSite48IdolForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core) : UI::GUIForm(parent, 1024, 768, ui)
{
	this->SetText(CSTR("48idol"));
	this->SetFont(UTF8STRC("MingLiu"), 8.25, false);

	this->core = core;
	this->ssl = Net::SSLEngineFactory::Create(this->core->GetSocketFactory(), true);
	Text::CString userAgent = Net::UserAgentDB::FindUserAgent(Manage::OSInfo::OT_WINDOWS_NT64, Net::BrowserInfo::BT_FIREFOX);
	NotNullPtr<Text::String> ua = Text::String::New(userAgent);
	NEW_CLASS(this->ctrl, Net::WebSite::WebSite48IdolControl(core->GetSocketFactory(), this->ssl, core->GetEncFactory(), ua.Ptr()));
	ua->Release();
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	this->tcMain = ui->NewTabControl(*this);
	this->tcMain->SetDockType(UI::GUIControl::DOCK_FILL);

	this->tpItems = this->tcMain->AddTabPage(CSTR("Items"));
	this->pnlRequest = ui->NewPanel(this->tpItems);
	this->pnlRequest->SetRect(0, 0, 100, 31, false);
	this->pnlRequest->SetDockType(UI::GUIControl::DOCK_TOP);
	this->lblPageNo = ui->NewLabel(this->pnlRequest, CSTR("PageNo"));
	this->lblPageNo->SetRect(4, 4, 100, 23, false);
	this->txtPageNo = ui->NewTextBox(this->pnlRequest, CSTR(""));
	this->txtPageNo->SetRect(104, 4, 60, 23, false);
	this->btnRequestPage = ui->NewButton(this->pnlRequest, CSTR("Request"));
	this->btnRequestPage->SetRect(164, 4, 75, 23, false);
	this->btnRequestPage->HandleButtonClick(OnRequestPageClicked, this);
	this->lvItems = ui->NewListView(this->tpItems, UI::ListViewStyle::Table, 3);
	this->lvItems->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lvItems->SetFullRowSelect(true);
	this->lvItems->SetShowGrid(true);
	this->lvItems->AddColumn(CSTR("Id"), 60);
	this->lvItems->AddColumn(CSTR("Date"), 120);
	this->lvItems->AddColumn(CSTR("Title"), 600);

	this->tpDownloadLink = this->tcMain->AddTabPage(CSTR("Download Link"));
	this->lblVideoId = ui->NewLabel(this->tpDownloadLink, CSTR("VideoId"));
	this->lblVideoId->SetRect(4, 4, 100, 23, false);
	this->txtVideoId = ui->NewTextBox(this->tpDownloadLink, CSTR(""));
	this->txtVideoId->SetRect(104, 4, 100, 23, false);
	this->btnDownloadLink = ui->NewButton(this->tpDownloadLink, CSTR("Get Link"));
	this->btnDownloadLink->SetRect(204, 4, 75, 23, false);
	this->btnDownloadLink->HandleButtonClick(OnDownloadLinkClicked, this);
	this->lblDownloadLink = ui->NewLabel(this->tpDownloadLink, CSTR("Link"));
	this->lblDownloadLink->SetRect(4, 28, 100, 23, false);
	this->txtDownloadLink = ui->NewTextBox(this->tpDownloadLink, CSTR(""));
	this->txtDownloadLink->SetRect(104, 28, 500, 23, false);
	this->txtDownloadLink->SetReadOnly(true);

	this->tpVideoName = this->tcMain->AddTabPage(CSTR("Video Name"));
	this->lblNameVideoId = ui->NewLabel(this->tpVideoName, CSTR("VideoId"));
	this->lblNameVideoId->SetRect(4, 4, 100, 23, false);
	this->txtNameVideoId = ui->NewTextBox(this->tpVideoName, CSTR(""));
	this->txtNameVideoId->SetRect(104, 4, 100, 23, false);
	this->btnVideoName = ui->NewButton(this->tpVideoName, CSTR("Get Name"));
	this->btnVideoName->SetRect(204, 4, 75, 23, false);
	this->btnVideoName->HandleButtonClick(OnVideoNameClicked, this);
	this->lblVideoName = ui->NewLabel(this->tpVideoName, CSTR("Video Name"));
	this->lblVideoName->SetRect(4, 28, 100, 23, false);
	this->txtVideoName = ui->NewTextBox(this->tpVideoName, CSTR(""));
	this->txtVideoName->SetRect(104, 28, 500, 23, false);
	this->txtVideoName->SetReadOnly(true);
}

SSWR::AVIRead::AVIRWebSite48IdolForm::~AVIRWebSite48IdolForm()
{
	DEL_CLASS(this->ctrl);
	this->ssl.Delete();
}

void SSWR::AVIRead::AVIRWebSite48IdolForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
