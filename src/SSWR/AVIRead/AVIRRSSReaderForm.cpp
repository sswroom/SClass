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

void __stdcall SSWR::AVIRead::AVIRRSSReaderForm::OnRequestClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRRSSReaderForm> me = userObj.GetNN<SSWR::AVIRead::AVIRRSSReaderForm>();
	UTF8Char sbuff[128];
	UnsafeArray<UTF8Char> sptr;
	Text::StringBuilderUTF8 sb;
	me->txtURL->GetText(sb);
	me->lvInfo->ClearItems();
	me->lvItems->ClearItems();
	me->rss.Delete();
	if (sb.GetLength() > 8)
	{
		UOSInt i;
		UOSInt j;
		OSInt si;
		Net::RSS *rss;
		NN<Text::String> s;
		NN<Net::RSSItem> item;
		Text::CStringNN userAgent = Net::UserAgentDB::FindUserAgent(Manage::OSInfo::OT_WINDOWS_NT64, Net::BrowserInfo::BT_FIREFOX);
		NN<Text::String> ua = Text::String::New(userAgent);
		NEW_CLASS(rss, Net::RSS(sb.ToCString(), ua.Ptr(), me->core->GetTCPClientFactory(), me->ssl, 30000, me->core->GetLog()));
		ua->Release();
		if (!rss->IsError())
		{
			Data::Timestamp dt;
			i = me->lvInfo->AddItem(CSTR("Title"), 0);
			if (rss->GetTitle().SetTo(s))
			{
				me->lvInfo->SetSubItem(i, 1, s->ToCString());
			}
			i = me->lvInfo->AddItem(CSTR("Link"), 0);
			if (rss->GetLink().SetTo(s))
			{
				me->lvInfo->SetSubItem(i, 1, s->ToCString());
			}
			i = me->lvInfo->AddItem(CSTR("Description"), 0);
			if (rss->GetDescription().SetTo(s))
			{
				me->lvInfo->SetSubItem(i, 1, s->ToCString());
			}
			i = me->lvInfo->AddItem(CSTR("Language"), 0);
			if (rss->GetLanguage().SetTo(s))
			{
				me->lvInfo->SetSubItem(i, 1, s->ToCString());
			}
			i = me->lvInfo->AddItem(CSTR("Copyright"), 0);
			if (rss->GetCopyright().SetTo(s))
			{
				me->lvInfo->SetSubItem(i, 1, s->ToCString());
			}
			i = me->lvInfo->AddItem(CSTR("ManagingEditor"), 0);
			if (rss->GetManagingEditor().SetTo(s))
			{
				me->lvInfo->SetSubItem(i, 1, s->ToCString());
			}
			i = me->lvInfo->AddItem(CSTR("WebMaster"), 0);
			if (rss->GetWebMaster().SetTo(s))
			{
				me->lvInfo->SetSubItem(i, 1, s->ToCString());
			}
			i = me->lvInfo->AddItem(CSTR("PubDate"), 0);
			if (!(dt = rss->GetPubDate()).IsNull())
			{
				sptr = dt.ToString(sbuff, "yyyy-MM-dd HH:mm:ss zzzz");
				me->lvInfo->SetSubItem(i, 1, CSTRP(sbuff, sptr));
			}
			i = me->lvInfo->AddItem(CSTR("LastBuildDate"), 0);
			if (!(dt = rss->GetLastBuildDate()).IsNull())
			{
				sptr = dt.ToString(sbuff, "yyyy-MM-dd HH:mm:ss zzzz");
				me->lvInfo->SetSubItem(i, 1, CSTRP(sbuff, sptr));
			}
			i = me->lvInfo->AddItem(CSTR("Generator"), 0);
			if (rss->GetGenerator().SetTo(s))
			{
				me->lvInfo->SetSubItem(i, 1, s->ToCString());
			}
			i = me->lvInfo->AddItem(CSTR("Docs"), 0);
			if (rss->GetDocs().SetTo(s))
			{
				me->lvInfo->SetSubItem(i, 1, s->ToCString());
			}
		}
		if (rss->GetCount() > 0)
		{
			si = me->rssList.SortedIndexOfC(sb.ToCString());
			if (si < 0)
			{
				j = me->rssList.SortedInsert(Text::String::New(sb.ToString(), sb.GetLength()));
				me->cboRecent->InsertItem(j, sb.ToCString(), 0);
				me->RSSListStore();
			}
			me->rss = rss;
			i = 0;
			j = rss->GetCount();
			while (i < j)
			{
				item = rss->GetItemNoCheck(i);
				me->lvItems->AddItem(Text::String::OrEmpty(item->title), item);
				if (!item->pubDate.IsNull())
				{
					sptr = item->pubDate.ToString(sbuff, "yyyy-MM-dd HH:mm:ss");
					me->lvItems->SetSubItem(i, 1, CSTRP(sbuff, sptr));
				}
				if (item->description.SetTo(s))
				{
					sb.ClearStr();
					Text::HTMLUtil::HTMLGetText(me->core->GetEncFactory(), s->v, s->leng, true, sb, 0);
					me->lvItems->SetSubItem(i, 2, sb.ToCString());
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

void __stdcall SSWR::AVIRead::AVIRRSSReaderForm::OnRecentSelChg(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRRSSReaderForm> me = userObj.GetNN<SSWR::AVIRead::AVIRRSSReaderForm>();
	NN<Text::String> s;
	UOSInt i = me->cboRecent->GetSelectedIndex();
	if (i != INVALID_INDEX && me->rssList.GetItem(i).SetTo(s))
	{
		me->txtURL->SetText(s->ToCString());
	}
}

void __stdcall SSWR::AVIRead::AVIRRSSReaderForm::OnItemsDblClick(AnyType userObj, UOSInt index)
{
	NN<SSWR::AVIRead::AVIRRSSReaderForm> me = userObj.GetNN<SSWR::AVIRead::AVIRRSSReaderForm>();
	NN<Net::RSSItem> item;
	if (me->lvItems->GetItem(index).GetOpt<Net::RSSItem>().SetTo(item))
	{
		SSWR::AVIRead::AVIRRSSItemForm frm(0, me->ui, me->core, item);
		frm.ShowDialog(me);
	}
}

void SSWR::AVIRead::AVIRRSSReaderForm::RSSListLoad()
{
	UTF8Char sbuff[512];
	UnsafeArray<UTF8Char> sptr;
	sptr = IO::Path::GetProcessFileName(sbuff).Or(sbuff);
	sptr = IO::Path::AppendPath(sbuff, sptr, CSTR("RSSList.txt"));
	UOSInt i;
	IO::FileStream fs(CSTRP(sbuff, sptr), IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
	if (!fs.IsError())
	{
		Text::StringBuilderUTF8 sb;
		Text::UTF8Reader reader(fs);
		while (reader.ReadLine(sb, 4096))
		{
			i = this->rssList.SortedInsert(Text::String::New(sb.ToString(), sb.GetLength()));
			this->cboRecent->InsertItem(i, sb.ToCString(), 0);
			sb.ClearStr();
		}
	}
}

void SSWR::AVIRead::AVIRRSSReaderForm::RSSListStore()
{
	UTF8Char sbuff[512];
	UnsafeArray<UTF8Char> sptr;
	sptr = IO::Path::GetProcessFileName(sbuff).Or(sbuff);
	sptr = IO::Path::AppendPath(sbuff, sptr, CSTR("RSSList.txt"));
	IO::FileStream fs(CSTRP(sbuff, sptr), IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
	if (!fs.IsError())
	{
		Text::UTF8Writer writer(fs);
		Data::ArrayIterator<NN<Text::String>> it = this->rssList.Iterator();
		while (it.HasNext())
		{
			writer.WriteLine(it.Next()->ToCString());
		}
	}
}

SSWR::AVIRead::AVIRRSSReaderForm::AVIRRSSReaderForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core) : UI::GUIForm(parent, 1024, 768, ui)
{
	this->SetText(CSTR("RSS Reader"));
	this->SetFont(CSTR("MingLiu"), 8.25, false);

	this->core = core;
	this->ssl = Net::SSLEngineFactory::Create(this->core->GetTCPClientFactory(), true);
	this->rss = 0;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	this->pnlURL = ui->NewPanel(*this);
	this->pnlURL->SetRect(0, 0, 100, 55, false);
	this->pnlURL->SetDockType(UI::GUIControl::DOCK_TOP);
	this->lblRecent = ui->NewLabel(this->pnlURL, CSTR("Recent"));
	this->lblRecent->SetRect(4, 4, 100, 23, false);
	this->cboRecent = ui->NewComboBox(this->pnlURL, false);
	this->cboRecent->SetRect(104, 4, 600, 23, false);
	this->cboRecent->HandleSelectionChange(OnRecentSelChg, this);
	this->lblURL = ui->NewLabel(this->pnlURL, CSTR("URL"));
	this->lblURL->SetRect(4, 28, 100, 23, false);
	this->txtURL = ui->NewTextBox(this->pnlURL, CSTR(""));
	this->txtURL->SetRect(104, 28, 600, 23, false);
	this->btnRequest = ui->NewButton(this->pnlURL, CSTR("Request"));
	this->btnRequest->SetRect(704, 28, 75, 23, false);
	this->btnRequest->HandleButtonClick(OnRequestClicked, this);
	this->tcRSS = ui->NewTabControl(*this);
	this->tcRSS->SetDockType(UI::GUIControl::DOCK_FILL);

	this->tpInfo = this->tcRSS->AddTabPage(CSTR("Info"));
	this->lvInfo = ui->NewListView(this->tpInfo, UI::ListViewStyle::Table, 2);
	this->lvInfo->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lvInfo->SetFullRowSelect(true);
	this->lvInfo->SetShowGrid(true);
	this->lvInfo->AddColumn(CSTR("Name"), 150);
	this->lvInfo->AddColumn(CSTR("Value"), 400);

	this->tpItems = this->tcRSS->AddTabPage(CSTR("Items"));
	this->lvItems = ui->NewListView(this->tpItems, UI::ListViewStyle::Table, 3);
	this->lvItems->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lvItems->SetFullRowSelect(true);
	this->lvItems->SetShowGrid(true);
	this->lvItems->AddColumn(CSTR("Title"), 150);
	this->lvItems->AddColumn(CSTR("Date"), 120);
	this->lvItems->AddColumn(CSTR("Description"), 400);
	this->lvItems->HandleDblClk(OnItemsDblClick, this);

	this->RSSListLoad();
}

SSWR::AVIRead::AVIRRSSReaderForm::~AVIRRSSReaderForm()
{
	UOSInt i;
	i = this->rssList.GetCount();
	while (i-- > 0)
	{
		OPTSTR_DEL(this->rssList.GetItem(i));
	}
	this->rss.Delete();
	this->ssl.Delete();
}

void SSWR::AVIRead::AVIRRSSReaderForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
