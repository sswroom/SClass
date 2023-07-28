#include "Stdafx.h"
#include "IO/FileStream.h"
#include "Net/UserAgentDB.h"
#include "Net/WebSite/WebSite48IdolControl.h"
#include "SSWR/DownloadMonitor/DownMonMainForm.h"
#include "Sync/MutexUsage.h"
#include "Text/UTF8Reader.h"
#include "Text/UTF8Util.h"
#include "Text/UTF8Writer.h"
#include "UI/Clipboard.h"
#include "UI/MessageDialog.h"
#include <stdio.h>

void __stdcall SSWR::DownloadMonitor::DownMonMainForm::OnTimerTick(void *userObj)
{
	SSWR::DownloadMonitor::DownMonMainForm *me = (SSWR::DownloadMonitor::DownMonMainForm*)userObj;
	SSWR::DownloadMonitor::DownMonCore::CheckStatus status = me->core->GetCurrStatus();
	if (status != me->currStatus)
	{
		me->currStatus = status;
		switch (status)
		{
		case SSWR::DownloadMonitor::DownMonCore::CS_IDLE:
			me->txtStatus->SetText(CSTR("Idle"));
			break;
		case SSWR::DownloadMonitor::DownMonCore::CS_DOWNLOADING:
			me->txtStatus->SetText(CSTR("Downloading"));
			break;
		case SSWR::DownloadMonitor::DownMonCore::CS_CHECKING:
			me->txtStatus->SetText(CSTR("Checking"));
			break;
		case SSWR::DownloadMonitor::DownMonCore::CS_EXTRACTING:
			me->txtStatus->SetText(CSTR("Extracting"));
			break;
		case SSWR::DownloadMonitor::DownMonCore::CS_VALIDATING:
			me->txtStatus->SetText(CSTR("Validating"));
			break;
		case SSWR::DownloadMonitor::DownMonCore::CS_MOVING:
			me->txtStatus->SetText(CSTR("Moving"));
			break;
		case SSWR::DownloadMonitor::DownMonCore::CS_MUXING:
			me->txtStatus->SetText(CSTR("Muxing"));
			break;
		default:
			break;
		}
	}

	Bool changed = false;
	UOSInt i;
	UOSInt j;
	Int32 id;
	Sync::MutexUsage mutUsage(me->endedMut);
	while (me->endedList.GetCount() > 0)
	{
		id = me->endedList.RemoveAt(0);
		i = 0;
		j = me->lvFiles->GetCount();
		while (i < j)
		{
			if (id == (OSInt)me->lvFiles->GetItem(i))
			{
				me->lvFiles->RemoveItem(i);
				changed = true;
				break;
			}
			i++;
		}
	}
	mutUsage.EndUse();
	if (changed)
	{
		me->SaveList();
		if (me->chkAutoStart->IsChecked())
		{
			id = (Int32)(OSInt)me->lvFiles->GetItem(0);
			if (id)
			{
				me->core->FileStart(id & 0xffffff, id >> 24, me->GetHandle());
			}
		}
	}

	if (me->alarmSet)
	{
		Data::DateTime dt;
		dt.SetCurrTimeUTC();
		if (dt.ToTicks() >= me->alarmTime)
		{
			me->alarmSet = false;
			me->lblAlarm->SetText(CSTR(""));
			id = (Int32)(OSInt)me->lvFiles->GetItem(0);
			if (id)
			{
				me->core->FileStart(id & 0xffffff, id >> 24, me->GetHandle());
			}
		}
	}
}

void __stdcall SSWR::DownloadMonitor::DownMonMainForm::OnPasteTableClicked(void *userObj)
{
	SSWR::DownloadMonitor::DownMonMainForm *me = (SSWR::DownloadMonitor::DownMonMainForm*)userObj;
	Text::StringBuilderUTF8 sb;
	UI::Clipboard::GetString(me->GetHandle(), &sb);
	UTF8Char sbuff[32];
	UTF8Char *sptr;
	Text::PString sarr[2];
	Text::PString sarr2[2];
	Bool changed = false;
	UOSInt i;
	UOSInt j;
	sarr[1] = sb;
	while (true)
	{
		i = Text::StrSplitLineP(sarr, 2, sarr[1]);
		if (Text::StrStartsWithC(sarr[0].v, sarr[0].leng, UTF8STRC("https://")) && Text::StrSplitP(sarr2, 2, sarr[0], '\t') == 2)
		{
			Int32 id = 0;
			Int32 webType = 0;
			id = ParseURL(sarr[0].ToCString(), &webType);

			if (id != 0)
			{
				NotNullPtr<Text::String> s = Text::String::New(sarr2[1].v, sarr2[1].leng);
				if (me->core->FileAdd(id, webType, s))
				{
					Sync::MutexUsage mutUsage;
					SSWR::DownloadMonitor::DownMonCore::FileInfo *file = me->core->FileGet(id, webType, mutUsage);
					if (file != 0)
					{
						sptr = Text::StrInt32(sbuff, file->id);
						j = me->lvFiles->AddItem(CSTRP(sbuff, sptr), (void*)(OSInt)((file->webType << 24) | file->id));
						me->lvFiles->SetSubItem(j, 1, file->fileName);
						changed = true;
					}
				}
				s->Release();
			}
		}
		if (i != 2)
			break;
	}

	if (changed)
	{
		me->SaveList();
	}
}

void __stdcall SSWR::DownloadMonitor::DownMonMainForm::OnPasteHTMLClicked(void *userObj)
{
	SSWR::DownloadMonitor::DownMonMainForm *me = (SSWR::DownloadMonitor::DownMonMainForm*)userObj;
	Data::ArrayList<UInt32> formats;
	UOSInt i;
	UOSInt j;
	UInt32 fmtId = (UInt32)-1;
	UTF8Char sbuff[512];
	UTF8Char *sptr;
	const UTF8Char *url;
	UOSInt urlLen;
	Text::PString sarr[2];
	Data::ArrayList<const UTF8Char *> urlStrList;
	Data::ArrayList<UOSInt> urlLenList;
	Data::ArrayList<const UTF8Char *> descList;
	UI::Clipboard clipboard(me->GetHandle());
	clipboard.GetDataFormats(&formats);
	i = formats.GetCount();
	while (i-- > 0)
	{
		UI::Clipboard::GetFormatName(formats.GetItem(i), sbuff, 256);
		if (Text::StrEquals(sbuff, (const UTF8Char*)"text/html"))
		{
			fmtId = formats.GetItem(i);
			break;
		}
	}

	if (fmtId != (UInt32)-1)
	{
		const UTF8Char *desc;
		Text::StringBuilderUTF8 sb;
		if (clipboard.GetDataText(fmtId, &sb))
		{
			sarr[1] = sb;
			while (true)
			{
				i = Text::StrSplitLineP(sarr, 2, sarr[1]);
				if (i == 2 && Text::StrIndexOfC(sarr[0].v, sarr[0].leng, UTF8STRC("<div class=\"post-thumb\">")) != INVALID_INDEX)
				{
					desc = 0;
					i = Text::StrSplitLineP(sarr, 2, sarr[1]);
					j = Text::StrIndexOfC(sarr[0].v, sarr[0].leng, UTF8STRC("<img class=\"lazyload\" "));
					if (i == 2 && j != INVALID_INDEX)
					{
						UTF8Char *linePtr = &sarr[0].v[j + 22];
						j = Text::StrIndexOf(linePtr, (const UTF8Char*)"alt=\"");
						if (j != INVALID_INDEX)
						{
							linePtr = &linePtr[j + 5];
							j = Text::StrIndexOfChar(linePtr, '\"');
							if (j != INVALID_INDEX)
							{
								linePtr[j] = 0;
								desc = linePtr;
							}
						}
					}
					if (desc)
					{
						i = Text::StrSplitLineP(sarr, 2, sarr[1]);
						j = Text::StrIndexOfC(sarr[0].v, sarr[0].leng, UTF8STRC("<a href=\""));
						if (i == 2 && j != INVALID_INDEX)
						{
							UTF8Char *linePtr = &sarr[0].v[j + 9];
							j = Text::StrIndexOfChar(linePtr, '\"');
							if (j != INVALID_INDEX)
							{
								linePtr[j] = 0;
								urlStrList.Add(linePtr);
								urlLenList.Add(j);
								descList.Add(desc);
							}
						}
					}
				}
				if (i != 2)
				{
					break;
				}
			}

			if (urlStrList.GetCount() > 0)
			{
				Bool changed = false;
				Int32 id;
				Int32 webType;
				i = urlStrList.GetCount();
				while (i-- > 0)
				{
					url = urlStrList.GetItem(i);
					urlLen = urlLenList.GetItem(i);
					desc = descList.GetItem(i);

					id = 0;
					webType = 0;
					id = ParseURL(Text::CString(url, urlLen), &webType);

					if (id != 0)
					{
						NotNullPtr<Text::String> s = Text::String::NewNotNullSlow(desc);
						if (me->core->FileAdd(id, webType, s))
						{
							Sync::MutexUsage mutUsage;
							SSWR::DownloadMonitor::DownMonCore::FileInfo *file = me->core->FileGet(id, 3, mutUsage);
							if (file != 0)
							{
								sptr = Text::StrInt32(sbuff, file->id);
								j = me->lvFiles->AddItem(CSTRP(sbuff, sptr), (void*)(OSInt)((file->webType << 24) | file->id));
								me->lvFiles->SetSubItem(j, 1, file->fileName);
								changed = true;
							}
						}
						s->Release();
					}
				}

				if (changed)
				{
					me->SaveList();
				}
			}
		}
	}
}

void __stdcall SSWR::DownloadMonitor::DownMonMainForm::OnCopyTableClicked(void *userObj)
{
	SSWR::DownloadMonitor::DownMonMainForm *me = (SSWR::DownloadMonitor::DownMonMainForm*)userObj;
	Text::StringBuilderUTF8 sb;
	UOSInt i;
	UOSInt j;
	i = 0;
	j = me->lvFiles->GetCount();
	while (i < j)
	{
		Int32 id = (Int32)(OSInt)me->lvFiles->GetItem(i);
		Sync::MutexUsage mutUsage;
		SSWR::DownloadMonitor::DownMonCore::FileInfo *file = me->core->FileGet(id & 0xffffff, id >> 24, mutUsage);
		if (file != 0)
		{
			if (file->webType == 2)
			{
				sb.AppendC(UTF8STRC("https://48idol.net/video/"));
			}
			else if (file->webType == 1)
			{
				sb.AppendC(UTF8STRC("https://48idol.com/video/"));
			}
			else if (file->webType == 3)
			{
				sb.AppendC(UTF8STRC("https://48idol.tv/archive/video/"));
			}
			else
			{
				sb.AppendC(UTF8STRC("https://48idol.tv/video/"));
			}
			sb.AppendI32(file->id);
			sb.AppendUTF8Char('\t');
			sb.Append(file->dbName);
			sb.AppendC(UTF8STRC("\r\n"));
		}
		i++;
	}

	UI::Clipboard::SetString(me->GetHandle(), sb.ToCString());
}

void __stdcall SSWR::DownloadMonitor::DownMonMainForm::OnFilesDblClick(void *userObj, UOSInt itemIndex)
{
	SSWR::DownloadMonitor::DownMonMainForm *me = (SSWR::DownloadMonitor::DownMonMainForm*)userObj;
	Int32 id = (Int32)(OSInt)me->lvFiles->GetItem(itemIndex);
	if (id)
	{
		me->core->FileStart(id & 0xffffff, id >> 24, me->GetHandle());
	}
}

void __stdcall SSWR::DownloadMonitor::DownMonMainForm::OnFileEndClicked(void *userObj)
{
	SSWR::DownloadMonitor::DownMonMainForm *me = (SSWR::DownloadMonitor::DownMonMainForm*)userObj;
	Int32 id = (Int32)(OSInt)me->lvFiles->GetSelectedItem();
	if (id > 0)
	{
		if (UI::MessageDialog::ShowYesNoDialog(CSTR("Are you sure to remove selected file?"), CSTR("Question"), me))
		{
			me->core->FileEnd(id & 0xffffff, id >> 24);
			Sync::MutexUsage mutUsage(me->endedMut);
			me->endedList.Add(id);
			mutUsage.EndUse();
		}
	}
}

void __stdcall SSWR::DownloadMonitor::DownMonMainForm::OnWebUpdateClicked(void *userObj)
{
	SSWR::DownloadMonitor::DownMonMainForm *me = (SSWR::DownloadMonitor::DownMonMainForm*)userObj;
	Int32 webType = 4;
	Int32 maxId = me->core->FileGetMaxId(webType);
	OSInt currPage = 1;
	UTF8Char sbuff[32];
	UTF8Char *sptr;
	UOSInt i;
	Data::ArrayList<Net::WebSite::WebSite48IdolControl::ItemData*> pageList;
	Data::ArrayList<Net::WebSite::WebSite48IdolControl::ItemData*> totalList;
	Net::WebSite::WebSite48IdolControl::ItemData *item;
	Net::WebSite::WebSite48IdolControl *ctrl;
	Text::EncodingFactory *encFact;
	Text::CString userAgent = Net::UserAgentDB::FindUserAgent(Manage::OSInfo::OT_WINDOWS_NT64, Net::BrowserInfo::BT_FIREFOX);
	NotNullPtr<Text::String> ua = Text::String::New(userAgent);
	NEW_CLASS(encFact, Text::EncodingFactory());
	NEW_CLASS(ctrl, Net::WebSite::WebSite48IdolControl(me->core->GetSocketFactory(), me->core->GetSSLEngine(), encFact, ua.Ptr()));
	ua->Release();
	while (true)
	{
		ctrl->GetTVPageItems(currPage, &pageList);
		i = pageList.GetCount();
		printf("Page %d get, %d items found\r\n", (Int32)currPage, (Int32)i);
		if (i <= 0)
		{
			break;
		}
		while (pageList.GetCount() > 0)
		{
			item = pageList.GetItem(0);
			if (item->id <= maxId)
			{
				break;
			}
			totalList.Add(pageList.RemoveAt(0));
		}
		if (maxId == 0 || pageList.GetCount() > 0)
		{
			ctrl->FreeItems(&pageList);
			break;
		}
		currPage++;
	}
	i = totalList.GetCount();
	if (i > 0)
	{
		Bool changed = false;
		while (i-- > 0)
		{
			item = totalList.GetItem(i);
			if (me->core->FileAdd(item->id, webType, item->title))
			{
				Sync::MutexUsage mutUsage;
				SSWR::DownloadMonitor::DownMonCore::FileInfo *file = me->core->FileGet(item->id, webType, mutUsage);
				UOSInt j;
				if (file != 0)
				{
					sptr = Text::StrInt32(sbuff, file->id);
					j = me->lvFiles->AddItem(CSTRP(sbuff, sptr), (void*)(OSInt)((file->webType << 24) | file->id));
					me->lvFiles->SetSubItem(j, 1, file->fileName);
					changed = true;
				}
			}
		}
		ctrl->FreeItems(&totalList);
		if (changed)
		{
			me->SaveList();
		}
	}

	DEL_CLASS(ctrl);
	DEL_CLASS(encFact);
}

void __stdcall SSWR::DownloadMonitor::DownMonMainForm::On30MinutesClicked(void *userObj)
{
	SSWR::DownloadMonitor::DownMonMainForm *me = (SSWR::DownloadMonitor::DownMonMainForm*)userObj;
	UTF8Char sbuff[64];
	UTF8Char *sptr;
	Data::DateTime dt;
	dt.SetCurrTime();
	dt.AddMinute(30);
	me->alarmTime = dt.ToTicks();
	me->alarmSet = true;
	sptr = dt.ToString(sbuff, "HH:mm:ss.fff");
	me->lblAlarm->SetText(CSTRP(sbuff, sptr));
}

void __stdcall SSWR::DownloadMonitor::DownMonMainForm::OnFileEnd(void *userObj, Int32 fileId, Int32 webType)
{
	SSWR::DownloadMonitor::DownMonMainForm *me = (SSWR::DownloadMonitor::DownMonMainForm*)userObj;
	Sync::MutexUsage mutUsage(me->endedMut);
	me->endedList.Add((webType << 24) | fileId);
	mutUsage.EndUse();
}

Int32 SSWR::DownloadMonitor::DownMonMainForm::ParseURL(Text::CString url, Int32 *webType)
{
	Int32 id;
	if (url.StartsWith(UTF8STRC("https://48idol.com/video/")))
	{
		id = Text::StrToInt32(&url.v[25]);
		*webType = 1;
	}
	else if (url.StartsWith(UTF8STRC("https://48idol.net/video/")))
	{
		id = Text::StrToInt32(&url.v[25]);
		*webType = 2;
	}
	else if (url.StartsWith(UTF8STRC("https://48idol.tv/archive/video/")))
	{
		id = Text::StrToInt32(&url.v[32]);
		*webType = 3;
	}
	else if (url.StartsWith(UTF8STRC("https://48idol.tv/video/")))
	{
		id = Text::StrToInt32(&url.v[24]);
		*webType = 4;
	}
	else
	{
		id = 0;
		*webType = 0;
	}
	return id;
}

void SSWR::DownloadMonitor::DownMonMainForm::LoadList()
{
	Text::StringBuilderUTF8 sb;

	Net::WebSite::WebSite48IdolControl *ctrl = 0;
	Text::EncodingFactory *encFact = 0;
	Text::CString userAgent = Net::UserAgentDB::FindUserAgent(Manage::OSInfo::OT_WINDOWS_NT64, Net::BrowserInfo::BT_FIREFOX);
	NotNullPtr<Text::String> ua = Text::String::New(userAgent);
	Text::StringBuilderUTF8 sb2;
	Bool updated = false;

	UTF8Char sbuff[32];
	UTF8Char *sptr;
	Text::PString sarr[2];
	UOSInt i;
	NotNullPtr<Text::String> listFile;
	if (listFile.Set(this->core->GetListFile()))
	{
		IO::FileStream fs(listFile, IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
		Text::UTF8Reader reader(fs);
		while (reader.ReadLine(sb, 4096))
		{
			if (sb.StartsWith(UTF8STRC("https://")) && Text::StrSplitP(sarr, 2, sb, '\t') == 2)
			{
				Int32 id = 0;
				Int32 webType = 0;
				id = ParseURL(sarr[0].ToCString(), &webType);

				if (id != 0)
				{
					if (!Text::UTF8Util::ValidStr(sarr[1].v))
					{
						printf("Invalid char found, id = %d\r\n", id);
						if (ctrl == 0)
						{
							NEW_CLASS(encFact, Text::EncodingFactory());
							NEW_CLASS(ctrl, Net::WebSite::WebSite48IdolControl(this->core->GetSocketFactory(), this->core->GetSSLEngine(), encFact, ua.Ptr()));
						}
						sb2.ClearStr();
						if (ctrl->GetVideoName(id, &sb2))
						{
							printf("Name of id %d updated\r\n", id);
							sarr[1] = sb2;
							updated = true;
						}
					}
					NotNullPtr<Text::String> s = Text::String::New(sarr[1].v, sarr[1].leng);
					if (this->core->FileAdd(id, webType, s))
					{
						Sync::MutexUsage mutUsage;
						SSWR::DownloadMonitor::DownMonCore::FileInfo *file = this->core->FileGet(id, webType, mutUsage);
						if (file != 0)
						{
							sptr = Text::StrInt32(sbuff, file->id);
							i = this->lvFiles->AddItem(CSTRP(sbuff, sptr), (void*)(OSInt)((file->webType << 24) | file->id));
							this->lvFiles->SetSubItem(i, 1, file->fileName);
						}
					}
					s->Release();
				}
			}

			sb.ClearStr();
		}
	}
	ua->Release();
	SDEL_CLASS(ctrl);
	SDEL_CLASS(encFact);
	
	if (updated)
	{
		this->SaveList();
	}
}

void SSWR::DownloadMonitor::DownMonMainForm::SaveList()
{
	Text::StringBuilderUTF8 sb;
	UOSInt i;
	UOSInt j;

	NotNullPtr<Text::String> listFile;
	if (!listFile.Set(this->core->GetListFile()))
		return;
	IO::FileStream fs(listFile, IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
	Text::UTF8Writer writer(fs);
	writer.WriteSignature();
	i = 0;
	j = this->lvFiles->GetCount();
	while (i < j)
	{
		Int32 id = (Int32)(OSInt)this->lvFiles->GetItem(i);
		Sync::MutexUsage mutUsage;
		SSWR::DownloadMonitor::DownMonCore::FileInfo *file = this->core->FileGet(id & 0xffffff, id >> 24, mutUsage);
		if (file != 0)
		{
			sb.ClearStr();
			if (file->webType == 2)
			{
				sb.AppendC(UTF8STRC("https://48idol.net/video/"));
			}
			else if (file->webType == 1)
			{
				sb.AppendC(UTF8STRC("https://48idol.com/video/"));
			}
			else if (file->webType == 3)
			{
				sb.AppendC(UTF8STRC("https://48idol.tv/archive/video/"));
			}
			else
			{
				sb.AppendC(UTF8STRC("https://48idol.tv/video/"));
			}
			sb.AppendI32(file->id);
			sb.AppendUTF8Char('\t');
			sb.Append(file->dbName);
			writer.WriteLineC(sb.ToString(), sb.GetLength());
		}
		i++;
	}
}

SSWR::DownloadMonitor::DownMonMainForm::DownMonMainForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, SSWR::DownloadMonitor::DownMonCore *core) : UI::GUIForm(parent, 1024, 768, ui)
{
	this->core = core;
	this->SetText(CSTR("Download Monitor"));
	this->SetFont(0, 0, 8.25, false);
	this->alarmSet = false;
	this->alarmTime = 0;
	this->core->SetFileEndHandler(OnFileEnd, this);

	NEW_CLASS(this->pnlButtons, UI::GUIPanel(ui, this));
	this->pnlButtons->SetRect(0, 0, 100, 55, false);
	this->pnlButtons->SetDockType(UI::GUIControl::DOCK_TOP);
	NEW_CLASS(this->lblStatus, UI::GUILabel(ui, this->pnlButtons, CSTR("Status")));
	this->lblStatus->SetRect(4, 4, 100, 23, false);
	NEW_CLASS(this->txtStatus, UI::GUITextBox(ui, this->pnlButtons, CSTR("Idle")));
	this->txtStatus->SetRect(104, 4, 100, 23, false);
	this->txtStatus->SetReadOnly(true);
	NEW_CLASS(this->chkAutoStart, UI::GUICheckBox(ui, this->pnlButtons, CSTR("AutoStart"), false));
	this->chkAutoStart->SetRect(204, 4, 100, 23, false);
	NEW_CLASS(this->btnPasteTable, UI::GUIButton(ui, this->pnlButtons, CSTR("Paste Table")));
	this->btnPasteTable->SetRect(304, 4, 75, 23, false);
	this->btnPasteTable->HandleButtonClick(OnPasteTableClicked, this);
	NEW_CLASS(this->btnPasteHTML, UI::GUIButton(ui, this->pnlButtons, CSTR("Paste HTML")));
	this->btnPasteHTML->SetRect(384, 4, 75, 23, false);
	this->btnPasteHTML->HandleButtonClick(OnPasteHTMLClicked, this);
	NEW_CLASS(this->btnCopyTable, UI::GUIButton(ui, this->pnlButtons, CSTR("Copy Table")));
	this->btnCopyTable->SetRect(464, 4, 75, 23, false);
	this->btnCopyTable->HandleButtonClick(OnCopyTableClicked, this);
	NEW_CLASS(this->btnFileEnd, UI::GUIButton(ui, this->pnlButtons, CSTR("File End")));
	this->btnFileEnd->SetRect(544, 4, 75, 23, false);
	this->btnFileEnd->HandleButtonClick(OnFileEndClicked, this);
	NEW_CLASS(this->btnWebUpdate, UI::GUIButton(ui, this->pnlButtons, CSTR("Web Update")));
	this->btnWebUpdate->SetRect(624, 4, 75, 23, false);
	this->btnWebUpdate->HandleButtonClick(OnWebUpdateClicked, this);
	NEW_CLASS(this->lblAlarm, UI::GUILabel(ui, this->pnlButtons, CSTR("")));
	this->lblAlarm->SetRect(4, 28, 100, 23, false);
	NEW_CLASS(this->btn30Minutes, UI::GUIButton(ui, this->pnlButtons, CSTR("30 Minutes")));
	this->btn30Minutes->SetRect(104, 28, 75, 23, false);
	this->btn30Minutes->HandleButtonClick(On30MinutesClicked, this);

	NEW_CLASS(this->lvFiles, UI::GUIListView(ui, this, UI::GUIListView::LVSTYLE_TABLE, 3));
	this->lvFiles->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lvFiles->AddColumn(CSTR("Id"), 60);
	this->lvFiles->AddColumn(CSTR("File Name"), 240);
	this->lvFiles->SetFullRowSelect(true);
	this->lvFiles->SetShowGrid(true);
	this->lvFiles->HandleDblClk(OnFilesDblClick, this);
//	this->lvFiles->HandleRightClick(OnFilesRClick, this);

	this->AddTimer(1000, OnTimerTick, this);
	this->LoadList();
}

SSWR::DownloadMonitor::DownMonMainForm::~DownMonMainForm()
{
}
