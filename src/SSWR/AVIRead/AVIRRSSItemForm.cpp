#include "Stdafx.h"
#include "Net/WebBrowser.h"
#include "SSWR/AVIRead/AVIRRSSItemForm.h"
#include "Text/HTMLUtil.h"

void __stdcall SSWR::AVIRead::AVIRRSSItemForm::OnImageSelChg(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRRSSItemForm> me = userObj.GetNN<SSWR::AVIRead::AVIRRSSItemForm>();
	UTF8Char sbuff[512];
	UnsafeArray<UTF8Char> sptr;
	UOSInt i = me->cboImage->GetSelectedIndex();
	if (i == INVALID_INDEX)
		return;
	sptr = me->cboImage->GetItemText(sbuff, i).Or(sbuff);
	Net::WebBrowser *browser = me->core->GetWebBrowser();
	NN<IO::StreamData> fd;
	if (fd.Set(browser->GetData(CSTRP(sbuff, sptr), false, sbuff)))
	{
		NN<Media::ImageList> imgList;
		if (Optional<Media::ImageList>::ConvertFrom(me->core->GetParserList()->ParseFile(fd, 0, IO::ParserType::ImageList)).SetTo(imgList))
		{
			fd.Delete();
			imgList->ToStaticImage(0);
			me->currImg.Delete();
			me->currImg = imgList;
			me->pbImage->SetImage(Optional<Media::StaticImage>::ConvertFrom(imgList->GetImage(0, 0)));
		}
		else
		{
			fd.Delete();
		}
	}
}

SSWR::AVIRead::AVIRRSSItemForm::AVIRRSSItemForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core, NN<Net::RSSItem> rssItem) : UI::GUIForm(parent, 1024, 768, ui)
{
	UOSInt i;
	UTF8Char sbuff[128];
	UnsafeArray<UTF8Char> sptr;
	this->SetText(CSTR("RSS Item"));
	this->SetFont(UTF8STRC("MingLiu"), 8.25, false);

	this->core = core;
	this->rssItem = rssItem;
	this->currImg = 0;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	this->tcMain = ui->NewTabControl(*this);
	this->tcMain->SetDockType(UI::GUIControl::DOCK_FILL);

	this->tpInfo = this->tcMain->AddTabPage(CSTR("Info"));
	this->lvInfo = ui->NewListView(this->tpInfo, UI::ListViewStyle::Table, 2);
	this->lvInfo->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lvInfo->SetFullRowSelect(true);
	this->lvInfo->SetShowGrid(true);
	this->lvInfo->AddColumn(CSTR("Name"), 150);
	this->lvInfo->AddColumn(CSTR("Value"), 400);

	this->tpImage = this->tcMain->AddTabPage(CSTR("Image"));
	this->cboImage = ui->NewComboBox(this->tpImage, false);
	this->cboImage->SetRect(0, 0, 100, 23, false);
	this->cboImage->SetDockType(UI::GUIControl::DOCK_TOP);
	this->cboImage->HandleSelectionChange(OnImageSelChg, this);
	this->pbImage = ui->NewPictureBoxSimple(this->tpImage, this->core->GetDrawEngine(), false);
	this->pbImage->SetDockType(UI::GUIControl::DOCK_FILL);

	this->tpText = this->tcMain->AddTabPage(CSTR("Items"));
	this->txtText = ui->NewTextBox(this->tpText, CSTR(""), true);
	this->txtText->SetDockType(UI::GUIControl::DOCK_FILL);
	this->txtText->SetReadOnly(true);
	NN<Text::String> s;

	i = this->lvInfo->AddItem(CSTR("Title"), 0);
	if (s.Set(rssItem->title))
	{
		this->lvInfo->SetSubItem(i, 1, s);
	}
	i = this->lvInfo->AddItem(CSTR("Link"), 0);
	if (s.Set(rssItem->link))
	{
		this->lvInfo->SetSubItem(i, 1, s);
	}
	i = this->lvInfo->AddItem(CSTR("Author"), 0);
	if (s.Set(rssItem->author))
	{
		this->lvInfo->SetSubItem(i, 1, s);
	}
	i = this->lvInfo->AddItem(CSTR("Category"), 0);
	if (s.Set(rssItem->category))
	{
		this->lvInfo->SetSubItem(i, 1, s);
	}
	i = this->lvInfo->AddItem(CSTR("Comments"), 0);
	if (s.Set(rssItem->comments))
	{
		this->lvInfo->SetSubItem(i, 1, s);
	}
	i = this->lvInfo->AddItem(CSTR("Enclosure"), 0);
	if (s.Set(rssItem->enclosure))
	{
		this->lvInfo->SetSubItem(i, 1, s);
	}
	i = this->lvInfo->AddItem(CSTR("PubDate"), 0);
	if (!rssItem->pubDate.IsNull())
	{
		sptr = rssItem->pubDate.ToString(sbuff, "yyyy-MM-dd HH:mm:ss zzzz");
		this->lvInfo->SetSubItem(i, 1, CSTRP(sbuff, sptr));
	}
	i = this->lvInfo->AddItem(CSTR("Source"), 0);
	if (s.Set(rssItem->source))
	{
		this->lvInfo->SetSubItem(i, 1, s);
	}
	i = this->lvInfo->AddItem(CSTR("GUID"), 0);
	if (s.Set(rssItem->guid))
	{
		this->lvInfo->SetSubItem(i, 1, s);
	}
	if (s.Set(rssItem->objectId))
	{
		i = this->lvInfo->AddItem(CSTR("ObjectId"), 0);
		this->lvInfo->SetSubItem(i, 1, s);
	}

	if (rssItem->description)
	{
		Data::ArrayListStringNN imgList;
		NN<Text::String> url;
		if (rssItem->descHTML)
		{
			Text::StringBuilderUTF8 sb;
			Text::HTMLUtil::HTMLGetText(this->core->GetEncFactory(), rssItem->description->v, rssItem->description->leng, false, sb, &imgList);
			this->txtText->SetText(sb.ToCString());
		}
		else
		{
			if (rssItem->imgURL)
			{
				imgList.Add(rssItem->imgURL->Clone());
			}
			this->txtText->SetText(rssItem->description->ToCString());
		}

		Data::ArrayIterator<NN<Text::String>> it = imgList.Iterator();
		while (it.HasNext())
		{
			url = it.Next();
			this->cboImage->AddItem(url, 0);
			url->Release();
		}
		if (imgList.GetCount() > 0)
		{
			this->cboImage->SetSelectedIndex(0);
		}
	}
}

SSWR::AVIRead::AVIRRSSItemForm::~AVIRRSSItemForm()
{
	this->ClearChildren();
	this->currImg.Delete();
}

void SSWR::AVIRead::AVIRRSSItemForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
