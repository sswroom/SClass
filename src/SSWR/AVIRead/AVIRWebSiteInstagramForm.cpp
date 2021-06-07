#include "Stdafx.h"
#include "Net/UserAgentDB.h"
#include "SSWR/AVIRead/AVIRWebSiteInstagramForm.h"

void __stdcall SSWR::AVIRead::AVIRWebSiteInstagramForm::OnRequestUserClicked(void *userObj)
{
	SSWR::AVIRead::AVIRWebSiteInstagramForm *me = (SSWR::AVIRead::AVIRWebSiteInstagramForm*)userObj;
	Text::StringBuilderUTF8 sb;
	UTF8Char sbuff[64];
	me->txtChannelId->GetText(&sb);
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
		me->ctrl->GetChannelItems(sb.ToString(), 0, &itemList, &chInfo);
		i = 0;
		j = itemList.GetCount();
		while (i < j)
		{
			item = itemList.GetItem(i);
			Text::StrInt64(sbuff, item->id);
			me->lvItems->AddItem(sbuff, 0);
			dt.SetTicks(item->recTime);
			dt.ToLocalTime();
			dt.ToString(sbuff, "yyyy-MM-dd HH:mm:ss");
			me->lvItems->SetSubItem(i, 1, sbuff);
			if (item->imgURL)
			{
				sb.ClearStr();
				sb.Append(item->imgURL);
				sb.Replace(' ', '\n');
				me->lvItems->SetSubItem(i, 2, sb.ToString());
			}
			me->lvItems->SetSubItem(i, 3, item->moreImages?(const UTF8Char*)"true":(const UTF8Char*)"false");
			sb.ClearStr();
			sb.Append(item->message);
			sb.Replace('\r', ' ');
			sb.Replace('\n', ' ');
			me->lvItems->SetSubItem(i, 4, sb.ToString());
			i++;
		}
		me->ctrl->FreeItems(&itemList);
		me->ctrl->FreeChannelInfo(&chInfo);
	}
}

void __stdcall SSWR::AVIRead::AVIRWebSiteInstagramForm::OnPageClicked(void *userObj)
{
	SSWR::AVIRead::AVIRWebSiteInstagramForm *me = (SSWR::AVIRead::AVIRWebSiteInstagramForm*)userObj;
	Text::StringBuilderUTF8 sb;
	me->txtShortCode->GetText(&sb);
	me->lbImageURL->ClearItems();
	if (sb.GetLength() > 0)
	{
		Data::ArrayList<const UTF8Char*> imageList;
		Data::ArrayList<const UTF8Char*> videoList;
		UOSInt i;
		UOSInt j;
		me->ctrl->GetPageImages(sb.ToString(), &imageList, &videoList);
		i = 0;
		j = imageList.GetCount();
		while (i < j)
		{
			me->lbImageURL->AddItem(imageList.GetItem(i), 0);
			Text::StrDelNew(imageList.GetItem(i));

			i++;
		}

		i = 0;
		j = videoList.GetCount();
		while (i < j)
		{
			me->lbImageURL->AddItem(videoList.GetItem(i), 0);
			Text::StrDelNew(videoList.GetItem(i));

			i++;
		}
	}
}

SSWR::AVIRead::AVIRWebSiteInstagramForm::AVIRWebSiteInstagramForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core) : UI::GUIForm(parent, 1024, 768, ui)
{
	this->SetText((const UTF8Char*)"Instagram");
	this->SetFont((const UTF8Char*)"MingLiu", 8.25, false);

	this->core = core;
	const UTF8Char *userAgent = Net::UserAgentDB::FindUserAgent(Manage::OSInfo::OT_WINDOWS_NT64, Net::BrowserInfo::BT_FIREFOX);
	NEW_CLASS(this->ctrl, Net::WebSite::WebSiteInstagramControl(core->GetSocketFactory(), core->GetEncFactory(), userAgent));
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	NEW_CLASS(this->tcMain, UI::GUITabControl(ui, this));
	this->tcMain->SetDockType(UI::GUIControl::DOCK_FILL);

	this->tpUser = this->tcMain->AddTabPage((const UTF8Char *)"User");
	NEW_CLASS(this->pnlRequest, UI::GUIPanel(ui, this->tpUser));
	this->pnlRequest->SetRect(0, 0, 100, 31, false);
	this->pnlRequest->SetDockType(UI::GUIControl::DOCK_TOP);
	NEW_CLASS(this->lblChannelId, UI::GUILabel(ui, this->pnlRequest, (const UTF8Char*)"ChannelId"));
	this->lblChannelId->SetRect(4, 4, 100, 23, false);
	NEW_CLASS(this->txtChannelId, UI::GUITextBox(ui, this->pnlRequest, (const UTF8Char*)""));
	this->txtChannelId->SetRect(104, 4, 150, 23, false);
	NEW_CLASS(this->btnRequestUser, UI::GUIButton(ui, this->pnlRequest, (const UTF8Char*)"Request"));
	this->btnRequestUser->SetRect(254, 4, 75, 23, false);
	this->btnRequestUser->HandleButtonClick(OnRequestUserClicked, this);
	NEW_CLASS(this->lvItems, UI::GUIListView(ui, this->tpUser, UI::GUIListView::LVSTYLE_TABLE, 5));
	this->lvItems->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lvItems->SetFullRowSelect(true);
	this->lvItems->SetShowGrid(true);
	this->lvItems->AddColumn((const UTF8Char*)"Id", 100);
	this->lvItems->AddColumn((const UTF8Char*)"Date", 120);
	this->lvItems->AddColumn((const UTF8Char*)"ImageURL", 200);
	this->lvItems->AddColumn((const UTF8Char*)"MoreImages", 60);
	this->lvItems->AddColumn((const UTF8Char*)"Message", 400);

	this->tpPage = this->tcMain->AddTabPage((const UTF8Char*)"Page");
	NEW_CLASS(this->pnlPage, UI::GUIPanel(ui, this->tpPage));
	this->pnlPage->SetRect(0, 0, 100, 31, false);
	this->pnlPage->SetDockType(UI::GUIControl::DOCK_TOP);
	NEW_CLASS(this->lblShortCode, UI::GUILabel(ui, this->pnlPage, (const UTF8Char*)"ShortCode"));
	this->lblShortCode->SetRect(4, 4, 100, 23, false);
	NEW_CLASS(this->txtShortCode, UI::GUITextBox(ui, this->pnlPage, (const UTF8Char*)""));
	this->txtShortCode->SetRect(104, 4, 150, 23, false);
	NEW_CLASS(this->btnPage, UI::GUIButton(ui, this->pnlPage, (const UTF8Char*)"Request"));
	this->btnPage->SetRect(254, 4, 75, 23, false);
	this->btnPage->HandleButtonClick(OnPageClicked, this);
	NEW_CLASS(this->lbImageURL, UI::GUIListBox(ui, this->tpPage, false));
	this->lbImageURL->SetDockType(UI::GUIControl::DOCK_FILL);
}

SSWR::AVIRead::AVIRWebSiteInstagramForm::~AVIRWebSiteInstagramForm()
{
	DEL_CLASS(this->ctrl);
}

void SSWR::AVIRead::AVIRWebSiteInstagramForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
