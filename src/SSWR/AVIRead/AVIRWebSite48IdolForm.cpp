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
	me->txtPageNo->GetText(&sb);
	sb.ToInt32(&pageNo);
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
			dt.ToString(sbuff, "yyyy-MM-dd HH:mm:ss");
			me->lvItems->SetSubItem(i, 1, sbuff);
			sb.ClearStr();
			Net::WebSite::WebSite48IdolControl::Title2DisplayName(item->title, &sb);
			me->lvItems->SetSubItem(i, 2, sb.ToString());
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
	me->txtVideoId->GetText(&sb);
	sb.ToInt32(&videoId);
	if (videoId != 0)
	{
		sb.ClearStr();
		if (me->ctrl->GetDownloadLink(videoId, 0, &sb))
		{
			me->txtDownloadLink->SetText(sb.ToString());
		}
		else
		{
			me->txtDownloadLink->SetText((const UTF8Char*)"");
		}
	}
}

void __stdcall SSWR::AVIRead::AVIRWebSite48IdolForm::OnVideoNameClicked(void *userObj)
{
	SSWR::AVIRead::AVIRWebSite48IdolForm *me = (SSWR::AVIRead::AVIRWebSite48IdolForm*)userObj;
	Text::StringBuilderUTF8 sb;
	Int32 videoId = 0;
	me->txtNameVideoId->GetText(&sb);
	sb.ToInt32(&videoId);
	if (videoId != 0)
	{
		sb.ClearStr();
		if (me->ctrl->GetVideoName(videoId, &sb))
		{
			me->txtVideoName->SetText(sb.ToString());
		}
		else
		{
			me->txtVideoName->SetText((const UTF8Char*)"");
		}
	}
}

SSWR::AVIRead::AVIRWebSite48IdolForm::AVIRWebSite48IdolForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core) : UI::GUIForm(parent, 1024, 768, ui)
{
	this->SetText((const UTF8Char*)"48idol");
	this->SetFont(UTF8STRC("MingLiu"), 8.25, false);

	this->core = core;
	this->ssl = Net::SSLEngineFactory::Create(this->core->GetSocketFactory(), true);
	const UTF8Char *userAgent = Net::UserAgentDB::FindUserAgent(Manage::OSInfo::OT_WINDOWS_NT64, Net::BrowserInfo::BT_FIREFOX);
	Text::String *ua = Text::String::NewNotNull(userAgent);
	NEW_CLASS(this->ctrl, Net::WebSite::WebSite48IdolControl(core->GetSocketFactory(), this->ssl, core->GetEncFactory(), ua));
	ua->Release();
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	NEW_CLASS(this->tcMain, UI::GUITabControl(ui, this));
	this->tcMain->SetDockType(UI::GUIControl::DOCK_FILL);

	this->tpItems = this->tcMain->AddTabPage((const UTF8Char*)"Items");
	NEW_CLASS(this->pnlRequest, UI::GUIPanel(ui, this->tpItems));
	this->pnlRequest->SetRect(0, 0, 100, 31, false);
	this->pnlRequest->SetDockType(UI::GUIControl::DOCK_TOP);
	NEW_CLASS(this->lblPageNo, UI::GUILabel(ui, this->pnlRequest, (const UTF8Char*)"PageNo"));
	this->lblPageNo->SetRect(4, 4, 100, 23, false);
	NEW_CLASS(this->txtPageNo, UI::GUITextBox(ui, this->pnlRequest, CSTR("")));
	this->txtPageNo->SetRect(104, 4, 60, 23, false);
	NEW_CLASS(this->btnRequestPage, UI::GUIButton(ui, this->pnlRequest, (const UTF8Char*)"Request"));
	this->btnRequestPage->SetRect(164, 4, 75, 23, false);
	this->btnRequestPage->HandleButtonClick(OnRequestPageClicked, this);
	NEW_CLASS(this->lvItems, UI::GUIListView(ui, this->tpItems, UI::GUIListView::LVSTYLE_TABLE, 3));
	this->lvItems->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lvItems->SetFullRowSelect(true);
	this->lvItems->SetShowGrid(true);
	this->lvItems->AddColumn((const UTF8Char*)"Id", 60);
	this->lvItems->AddColumn((const UTF8Char*)"Date", 120);
	this->lvItems->AddColumn((const UTF8Char*)"Title", 600);

	this->tpDownloadLink = this->tcMain->AddTabPage((const UTF8Char*)"Download Link");
	NEW_CLASS(this->lblVideoId, UI::GUILabel(ui, this->tpDownloadLink, (const UTF8Char*)"VideoId"));
	this->lblVideoId->SetRect(4, 4, 100, 23, false);
	NEW_CLASS(this->txtVideoId, UI::GUITextBox(ui, this->tpDownloadLink, CSTR("")));
	this->txtVideoId->SetRect(104, 4, 100, 23, false);
	NEW_CLASS(this->btnDownloadLink, UI::GUIButton(ui, this->tpDownloadLink, (const UTF8Char*)"Get Link"));
	this->btnDownloadLink->SetRect(204, 4, 75, 23, false);
	this->btnDownloadLink->HandleButtonClick(OnDownloadLinkClicked, this);
	NEW_CLASS(this->lblDownloadLink, UI::GUILabel(ui, this->tpDownloadLink, (const UTF8Char*)"Link"));
	this->lblDownloadLink->SetRect(4, 28, 100, 23, false);
	NEW_CLASS(this->txtDownloadLink, UI::GUITextBox(ui, this->tpDownloadLink, CSTR("")));
	this->txtDownloadLink->SetRect(104, 28, 500, 23, false);
	this->txtDownloadLink->SetReadOnly(true);

	this->tpVideoName = this->tcMain->AddTabPage((const UTF8Char*)"Video Name");
	NEW_CLASS(this->lblNameVideoId, UI::GUILabel(ui, this->tpVideoName, (const UTF8Char*)"VideoId"));
	this->lblNameVideoId->SetRect(4, 4, 100, 23, false);
	NEW_CLASS(this->txtNameVideoId, UI::GUITextBox(ui, this->tpVideoName, CSTR("")));
	this->txtNameVideoId->SetRect(104, 4, 100, 23, false);
	NEW_CLASS(this->btnVideoName, UI::GUIButton(ui, this->tpVideoName, (const UTF8Char*)"Get Name"));
	this->btnVideoName->SetRect(204, 4, 75, 23, false);
	this->btnVideoName->HandleButtonClick(OnVideoNameClicked, this);
	NEW_CLASS(this->lblVideoName, UI::GUILabel(ui, this->tpVideoName, (const UTF8Char*)"Video Name"));
	this->lblVideoName->SetRect(4, 28, 100, 23, false);
	NEW_CLASS(this->txtVideoName, UI::GUITextBox(ui, this->tpVideoName, CSTR("")));
	this->txtVideoName->SetRect(104, 28, 500, 23, false);
	this->txtVideoName->SetReadOnly(true);
}

SSWR::AVIRead::AVIRWebSite48IdolForm::~AVIRWebSite48IdolForm()
{
	DEL_CLASS(this->ctrl);
	SDEL_CLASS(this->ssl);
}

void SSWR::AVIRead::AVIRWebSite48IdolForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
