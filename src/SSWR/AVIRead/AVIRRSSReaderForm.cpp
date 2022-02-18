#include "Stdafx.h"
#include "IO/FileStream.h"
#include "IO/Path.h"
#include "Net/SSLEngineFactory.h"
#include "Net/UserAgentDB.h"
#include "SSWR/AVIRead/AVIRRSSItemForm.h"
#include "SSWR/AVIRead/AVIRRSSReaderForm.h"
#include "Text/HTMLUtil.h"
#include "Text/UTF8Reader.h"
#include "Text/UTF8Writer.h"

void __stdcall SSWR::AVIRead::AVIRRSSReaderForm::OnRequestClicked(void *userObj)
{
	SSWR::AVIRead::AVIRRSSReaderForm *me = (SSWR::AVIRead::AVIRRSSReaderForm*)userObj;
	UTF8Char sbuff[128];
	Text::StringBuilderUTF8 sb;
	me->txtURL->GetText(&sb);
	me->lvInfo->ClearItems();
	me->lvItems->ClearItems();
	SDEL_CLASS(me->rss);
	if (sb.GetLength() > 8)
	{
		UOSInt i;
		UOSInt j;
		OSInt si;
		Net::RSS *rss;
		Net::RSSItem *item;
		Text::CString userAgent = Net::UserAgentDB::FindUserAgent(Manage::OSInfo::OT_WINDOWS_NT64, Net::BrowserInfo::BT_FIREFOX);
		Text::String *ua = Text::String::New(userAgent);
		NEW_CLASS(rss, Net::RSS(sb.ToCString(), ua, me->core->GetSocketFactory(), me->ssl));
		ua->Release();
		if (!rss->IsError())
		{
			Text::String *s;
			Data::DateTime *dt;
			i = me->lvInfo->AddItem(CSTR("Title"), 0);
			if ((s = rss->GetTitle()) != 0)
			{
				me->lvInfo->SetSubItem(i, 1, s);
			}
			i = me->lvInfo->AddItem(CSTR("Link"), 0);
			if ((s = rss->GetLink()) != 0)
			{
				me->lvInfo->SetSubItem(i, 1, s);
			}
			i = me->lvInfo->AddItem(CSTR("Description"), 0);
			if ((s = rss->GetDescription()) != 0)
			{
				me->lvInfo->SetSubItem(i, 1, s);
			}
			i = me->lvInfo->AddItem(CSTR("Language"), 0);
			if ((s = rss->GetLanguage()) != 0)
			{
				me->lvInfo->SetSubItem(i, 1, s);
			}
			i = me->lvInfo->AddItem(CSTR("Copyright"), 0);
			if ((s = rss->GetCopyright()) != 0)
			{
				me->lvInfo->SetSubItem(i, 1, s);
			}
			i = me->lvInfo->AddItem(CSTR("ManagingEditor"), 0);
			if ((s = rss->GetManagingEditor()) != 0)
			{
				me->lvInfo->SetSubItem(i, 1, s);
			}
			i = me->lvInfo->AddItem(CSTR("WebMaster"), 0);
			if ((s = rss->GetWebMaster()) != 0)
			{
				me->lvInfo->SetSubItem(i, 1, s);
			}
			i = me->lvInfo->AddItem(CSTR("PubDate"), 0);
			if ((dt = rss->GetPubDate()) != 0)
			{
				dt->ToString(sbuff, "yyyy-MM-dd HH:mm:ss zzzz");
				me->lvInfo->SetSubItem(i, 1, sbuff);
			}
			i = me->lvInfo->AddItem(CSTR("LastBuildDate"), 0);
			if ((dt = rss->GetLastBuildDate()) != 0)
			{
				dt->ToString(sbuff, "yyyy-MM-dd HH:mm:ss zzzz");
				me->lvInfo->SetSubItem(i, 1, sbuff);
			}
			i = me->lvInfo->AddItem(CSTR("Generator"), 0);
			if ((s = rss->GetGenerator()) != 0)
			{
				me->lvInfo->SetSubItem(i, 1, s);
			}
			i = me->lvInfo->AddItem(CSTR("Docs"), 0);
			if ((s = rss->GetDocs()) != 0)
			{
				me->lvInfo->SetSubItem(i, 1, s);
			}
		}
		if (rss->GetCount() > 0)
		{
			si = me->rssList->SortedIndexOfPtr(sb.ToString(), sb.GetLength());
			if (si < 0)
			{
				j = me->rssList->SortedInsert(Text::String::New(sb.ToString(), sb.GetLength()));
				me->cboRecent->InsertItem(j, sb.ToCString(), 0);
				me->RSSListStore();
			}
			me->rss = rss;
			i = 0;
			j = rss->GetCount();
			while (i < j)
			{
				item = rss->GetItem(i);
				me->lvItems->AddItem(item->title, item);
				if (item->pubDate)
				{
					item->pubDate->ToString(sbuff, "yyyy-MM-dd HH:mm:ss");
					me->lvItems->SetSubItem(i, 1, sbuff);
				}
				if (item->description)
				{
					sb.ClearStr();
					Text::HTMLUtil::HTMLGetText(me->core->GetEncFactory(), item->description->v, item->description->leng, true, &sb, 0);
					me->lvItems->SetSubItem(i, 2, sb.ToString());
				}
				i++;
			}
		}
		else
		{
			DEL_CLASS(rss);
		}
	}
}

void __stdcall SSWR::AVIRead::AVIRRSSReaderForm::OnRecentSelChg(void *userObj)
{
	SSWR::AVIRead::AVIRRSSReaderForm *me = (SSWR::AVIRead::AVIRRSSReaderForm*)userObj;
	UOSInt i = me->cboRecent->GetSelectedIndex();
	if (i != INVALID_INDEX)
	{
		me->txtURL->SetText(me->rssList->GetItem(i)->ToCString());
	}
}

void __stdcall SSWR::AVIRead::AVIRRSSReaderForm::OnItemsDblClick(void *userObj, UOSInt index)
{
	SSWR::AVIRead::AVIRRSSReaderForm *me = (SSWR::AVIRead::AVIRRSSReaderForm*)userObj;
	Net::RSSItem *item = (Net::RSSItem*)me->lvItems->GetItem(index);
	if (item)
	{
		SSWR::AVIRead::AVIRRSSItemForm *frm;
		NEW_CLASS(frm, SSWR::AVIRead::AVIRRSSItemForm(0, me->ui, me->core, item));
		frm->ShowDialog(me);
		DEL_CLASS(frm);
	}
}

void SSWR::AVIRead::AVIRRSSReaderForm::RSSListLoad()
{
	UTF8Char sbuff[512];
	UTF8Char *sptr;
	sptr = IO::Path::GetProcessFileName(sbuff);
	sptr = IO::Path::AppendPathC(sbuff, sptr, UTF8STRC("RSSList.txt"));
	IO::FileStream *fs;
	Text::UTF8Reader *reader;
	UOSInt i;
	NEW_CLASS(fs, IO::FileStream(CSTRP(sbuff, sptr), IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal));
	if (!fs->IsError())
	{
		Text::StringBuilderUTF8 sb;
		NEW_CLASS(reader, Text::UTF8Reader(fs));
		while (reader->ReadLine(&sb, 4096))
		{
			i = this->rssList->SortedInsert(Text::String::New(sb.ToString(), sb.GetLength()));
			this->cboRecent->InsertItem(i, sb.ToCString(), 0);
			sb.ClearStr();
		}
		DEL_CLASS(reader);
	}
	DEL_CLASS(fs);
}

void SSWR::AVIRead::AVIRRSSReaderForm::RSSListStore()
{
	UTF8Char sbuff[512];
	UTF8Char *sptr;
	sptr = IO::Path::GetProcessFileName(sbuff);
	sptr = IO::Path::AppendPathC(sbuff, sptr, UTF8STRC("RSSList.txt"));
	IO::FileStream *fs;
	Text::UTF8Writer *writer;
	UOSInt i;
	UOSInt j;
	NEW_CLASS(fs, IO::FileStream(CSTRP(sbuff, sptr), IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal));
	if (!fs->IsError())
	{
		NEW_CLASS(writer, Text::UTF8Writer(fs));
		i = 0;
		j = this->rssList->GetCount();
		while (i < j)
		{
			Text::String *s = rssList->GetItem(i);
			writer->WriteLineC(s->v, s->leng);
			i++;
		}
		DEL_CLASS(writer);
	}
	DEL_CLASS(fs);
}

SSWR::AVIRead::AVIRRSSReaderForm::AVIRRSSReaderForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core) : UI::GUIForm(parent, 1024, 768, ui)
{
	this->SetText(CSTR("RSS Reader"));
	this->SetFont(UTF8STRC("MingLiu"), 8.25, false);

	this->core = core;
	this->ssl = Net::SSLEngineFactory::Create(this->core->GetSocketFactory(), true);
	NEW_CLASS(this->rssList, Data::ArrayListString());
	this->rss = 0;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	NEW_CLASS(this->pnlURL, UI::GUIPanel(ui, this));
	this->pnlURL->SetRect(0, 0, 100, 55, false);
	this->pnlURL->SetDockType(UI::GUIControl::DOCK_TOP);
	NEW_CLASS(this->lblRecent, UI::GUILabel(ui, this->pnlURL, (const UTF8Char*)"Recent"));
	this->lblRecent->SetRect(4, 4, 100, 23, false);
	NEW_CLASS(this->cboRecent, UI::GUIComboBox(ui, this->pnlURL, false));
	this->cboRecent->SetRect(104, 4, 600, 23, false);
	this->cboRecent->HandleSelectionChange(OnRecentSelChg, this);
	NEW_CLASS(this->lblURL, UI::GUILabel(ui, this->pnlURL, (const UTF8Char*)"URL"));
	this->lblURL->SetRect(4, 28, 100, 23, false);
	NEW_CLASS(this->txtURL, UI::GUITextBox(ui, this->pnlURL, CSTR("")));
	this->txtURL->SetRect(104, 28, 600, 23, false);
	NEW_CLASS(this->btnRequest, UI::GUIButton(ui, this->pnlURL, CSTR("Request")));
	this->btnRequest->SetRect(704, 28, 75, 23, false);
	this->btnRequest->HandleButtonClick(OnRequestClicked, this);
	NEW_CLASS(this->tcRSS, UI::GUITabControl(ui, this));
	this->tcRSS->SetDockType(UI::GUIControl::DOCK_FILL);

	this->tpInfo = this->tcRSS->AddTabPage(CSTR("Info"));
	NEW_CLASS(this->lvInfo, UI::GUIListView(ui, this->tpInfo, UI::GUIListView::LVSTYLE_TABLE, 2));
	this->lvInfo->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lvInfo->SetFullRowSelect(true);
	this->lvInfo->SetShowGrid(true);
	this->lvInfo->AddColumn((const UTF8Char*)"Name", 150);
	this->lvInfo->AddColumn((const UTF8Char*)"Value", 400);

	this->tpItems = this->tcRSS->AddTabPage(CSTR("Items"));
	NEW_CLASS(this->lvItems, UI::GUIListView(ui, this->tpItems, UI::GUIListView::LVSTYLE_TABLE, 3));
	this->lvItems->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lvItems->SetFullRowSelect(true);
	this->lvItems->SetShowGrid(true);
	this->lvItems->AddColumn((const UTF8Char*)"Title", 150);
	this->lvItems->AddColumn((const UTF8Char*)"Date", 120);
	this->lvItems->AddColumn((const UTF8Char*)"Description", 400);
	this->lvItems->HandleDblClk(OnItemsDblClick, this);

	this->RSSListLoad();
}

SSWR::AVIRead::AVIRRSSReaderForm::~AVIRRSSReaderForm()
{
	UOSInt i;
	i = this->rssList->GetCount();
	while (i-- > 0)
	{
		this->rssList->GetItem(i)->Release();
	}
	DEL_CLASS(this->rssList);
	SDEL_CLASS(this->rss);
	SDEL_CLASS(this->ssl);
}

void SSWR::AVIRead::AVIRRSSReaderForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
