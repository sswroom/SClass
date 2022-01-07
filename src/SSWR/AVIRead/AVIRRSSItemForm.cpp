#include "Stdafx.h"
#include "Net/WebBrowser.h"
#include "SSWR/AVIRead/AVIRRSSItemForm.h"
#include "Text/HTMLUtil.h"

void __stdcall SSWR::AVIRead::AVIRRSSItemForm::OnImageSelChg(void *userObj)
{
	SSWR::AVIRead::AVIRRSSItemForm *me = (SSWR::AVIRead::AVIRRSSItemForm *)userObj;
	UTF8Char sbuff[512];
	UOSInt i = me->cboImage->GetSelectedIndex();
	if (i == INVALID_INDEX)
		return;
	me->cboImage->GetItemText(sbuff, i);
	Net::WebBrowser *browser = me->core->GetWebBrowser();
	IO::IStreamData *fd = browser->GetData(sbuff, false, sbuff);
	if (fd)
	{
		Media::ImageList *imgList = (Media::ImageList*)me->core->GetParserList()->ParseFile(fd, 0, 0, IO::ParserType::ImageList);
		DEL_CLASS(fd);
		if (imgList)
		{
			imgList->ToStaticImage(0);
			SDEL_CLASS(me->currImg);
			me->currImg = imgList;
			me->pbImage->SetImage((Media::StaticImage*)imgList->GetImage(0, 0));
		}
	}
}

SSWR::AVIRead::AVIRRSSItemForm::AVIRRSSItemForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core, Net::RSSItem *rssItem) : UI::GUIForm(parent, 1024, 768, ui)
{
	UOSInt i;
	UOSInt j;
	UTF8Char sbuff[128];
	this->SetText((const UTF8Char*)"RSS Item");
	this->SetFont((const UTF8Char*)"MingLiu", 8.25, false);

	this->core = core;
	this->rssItem = rssItem;
	this->currImg = 0;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	NEW_CLASS(this->tcMain, UI::GUITabControl(ui, this));
	this->tcMain->SetDockType(UI::GUIControl::DOCK_FILL);

	this->tpInfo = this->tcMain->AddTabPage((const UTF8Char*)"Info");
	NEW_CLASS(this->lvInfo, UI::GUIListView(ui, this->tpInfo, UI::GUIListView::LVSTYLE_TABLE, 2));
	this->lvInfo->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lvInfo->SetFullRowSelect(true);
	this->lvInfo->SetShowGrid(true);
	this->lvInfo->AddColumn((const UTF8Char*)"Name", 150);
	this->lvInfo->AddColumn((const UTF8Char*)"Value", 400);

#if defined(_WIN32) || defined(WIN64)
	this->tpImage = this->tcMain->AddTabPage((const UTF8Char*)"Image");
	NEW_CLASS(this->cboImage, UI::GUIComboBox(ui, this->tpImage, false));
	this->cboImage->SetRect(0, 0, 100, 23, false);
	this->cboImage->SetDockType(UI::GUIControl::DOCK_TOP);
	this->cboImage->HandleSelectionChange(OnImageSelChg, this);
	NEW_CLASS(this->pbImage, UI::GUIPictureBoxSimple(ui, this->tpImage, this->core->GetDrawEngine(), false));
	this->pbImage->SetDockType(UI::GUIControl::DOCK_FILL);
#else
	this->tpImage = 0;
	this->cboImage = 0;
	this->pbImage = 0;
#endif

	this->tpText = this->tcMain->AddTabPage((const UTF8Char*)"Items");
	NEW_CLASS(this->txtText, UI::GUITextBox(ui, this->tpText, (const UTF8Char*)"", true));
	this->txtText->SetDockType(UI::GUIControl::DOCK_FILL);
	this->txtText->SetReadOnly(true);

	i = this->lvInfo->AddItem((const UTF8Char*)"Title", 0);
	if (rssItem->title)
	{
		this->lvInfo->SetSubItem(i, 1, rssItem->title);
	}
	i = this->lvInfo->AddItem((const UTF8Char*)"Link", 0);
	if (rssItem->link)
	{
		this->lvInfo->SetSubItem(i, 1, rssItem->link);
	}
	i = this->lvInfo->AddItem((const UTF8Char*)"Author", 0);
	if (rssItem->author)
	{
		this->lvInfo->SetSubItem(i, 1, rssItem->author);
	}
	i = this->lvInfo->AddItem((const UTF8Char*)"Category", 0);
	if (rssItem->category)
	{
		this->lvInfo->SetSubItem(i, 1, rssItem->category);
	}
	i = this->lvInfo->AddItem((const UTF8Char*)"Comments", 0);
	if (rssItem->comments)
	{
		this->lvInfo->SetSubItem(i, 1, rssItem->comments);
	}
	i = this->lvInfo->AddItem((const UTF8Char*)"Enclosure", 0);
	if (rssItem->enclosure)
	{
		this->lvInfo->SetSubItem(i, 1, rssItem->enclosure);
	}
	i = this->lvInfo->AddItem((const UTF8Char*)"PubDate", 0);
	if (rssItem->pubDate)
	{
		rssItem->pubDate->ToString(sbuff, "yyyy-MM-dd HH:mm:ss zzzz");
		this->lvInfo->SetSubItem(i, 1, sbuff);
	}
	i = this->lvInfo->AddItem((const UTF8Char*)"Source", 0);
	if (rssItem->source)
	{
		this->lvInfo->SetSubItem(i, 1, rssItem->source);
	}
	i = this->lvInfo->AddItem((const UTF8Char*)"GUID", 0);
	if (rssItem->guid)
	{
		this->lvInfo->SetSubItem(i, 1, rssItem->guid);
	}
	if (rssItem->objectId)
	{
		i = this->lvInfo->AddItem((const UTF8Char*)"ObjectId", 0);
		this->lvInfo->SetSubItem(i, 1, rssItem->objectId);
	}

	if (rssItem->description)
	{
		Data::ArrayList<Text::String*> imgList;
		Text::String *url;
		if (rssItem->descHTML)
		{
			Text::StringBuilderUTF8 sb;
			Text::HTMLUtil::HTMLGetText(this->core->GetEncFactory(), rssItem->description->v, rssItem->description->leng, false, &sb, &imgList);
			this->txtText->SetText(sb.ToString());
		}
		else
		{
			if (rssItem->imgURL)
			{
				imgList.Add(rssItem->imgURL->Clone());
			}
			this->txtText->SetText(rssItem->description->v);
		}

		i = 0;
		j = imgList.GetCount();
		while (i < j)
		{
			url = imgList.GetItem(i);
			if (this->cboImage)
			{
				this->cboImage->AddItem(url, 0);
			}

			url->Release();
			i++;
		}
		if (j > 0 && this->cboImage)
		{
			this->cboImage->SetSelectedIndex(0);
		}
	}
}

SSWR::AVIRead::AVIRRSSItemForm::~AVIRRSSItemForm()
{
	this->ClearChildren();
	SDEL_CLASS(this->currImg);
}

void SSWR::AVIRead::AVIRRSSItemForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
