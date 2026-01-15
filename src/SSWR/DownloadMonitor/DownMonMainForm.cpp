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
#include <stdio.h>

void __stdcall SSWR::DownloadMonitor::DownMonMainForm::OnTimerTick(AnyType userObj)
{
	NN<SSWR::DownloadMonitor::DownMonMainForm> me = userObj.GetNN<SSWR::DownloadMonitor::DownMonMainForm>();
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
	UIntOS i;
	UIntOS j;
	Int32 id;
	Sync::MutexUsage mutUsage(me->endedMut);
	while (me->endedList.GetCount() > 0)
	{
		id = me->endedList.RemoveAt(0);
		i = 0;
		j = me->lvFiles->GetCount();
		while (i < j)
		{
			if (id == me->lvFiles->GetItem(i).GetIntOS())
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
			id = (Int32)me->lvFiles->GetItem(0).GetIntOS();
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
			id = (Int32)me->lvFiles->GetItem(0).GetIntOS();
			if (id)
			{
				me->core->FileStart(id & 0xffffff, id >> 24, me->GetHandle());
			}
		}
	}
}

void __stdcall SSWR::DownloadMonitor::DownMonMainForm::OnPasteTableClicked(AnyType userObj)
{
	NN<SSWR::DownloadMonitor::DownMonMainForm> me = userObj.GetNN<SSWR::DownloadMonitor::DownMonMainForm>();
	Text::StringBuilderUTF8 sb;
	UI::Clipboard::GetString(me->GetHandle(), sb);
	UTF8Char sbuff[32];
	UnsafeArray<UTF8Char> sptr;
	Text::PString sarr[2];
	Text::PString sarr2[2];
	Bool changed = false;
	UIntOS i;
	UIntOS j;
	sarr[1] = sb;
	while (true)
	{
		i = Text::StrSplitLineP(sarr, 2, sarr[1]);
		if (Text::StrStartsWithC(sarr[0].v, sarr[0].leng, UTF8STRC("https://")) && Text::StrSplitP(sarr2, 2, sarr[0], '\t') == 2)
		{
			Int32 id = 0;
			Int32 webType = 0;
			id = ParseURL(sarr[0].ToCString(), webType);

			if (id != 0)
			{
				NN<Text::String> s = Text::String::New(sarr2[1].v, sarr2[1].leng);
				if (me->core->FileAdd(id, webType, s))
				{
					Sync::MutexUsage mutUsage;
					NN<SSWR::DownloadMonitor::DownMonCore::FileInfo> file;
					if (me->core->FileGet(id, webType, mutUsage).SetTo(file))
					{
						sptr = Text::StrInt32(sbuff, file->id);
						j = me->lvFiles->AddItem(CSTRP(sbuff, sptr), (void*)(IntOS)((file->webType << 24) | file->id));
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

void __stdcall SSWR::DownloadMonitor::DownMonMainForm::OnPasteHTMLClicked(AnyType userObj)
{
	NN<SSWR::DownloadMonitor::DownMonMainForm> me = userObj.GetNN<SSWR::DownloadMonitor::DownMonMainForm>();
	Data::ArrayListNative<UInt32> formats;
	UIntOS i;
	UIntOS j;
	UInt32 fmtId = (UInt32)-1;
	UTF8Char sbuff[512];
	UnsafeArray<UTF8Char> sptr;
	UnsafeArray<const UTF8Char> url;
	UIntOS urlLen;
	Text::PString sarr[2];
	Data::ArrayListObj<const UTF8Char *> urlStrList;
	Data::ArrayListNative<UIntOS> urlLenList;
	Data::ArrayListObj<const UTF8Char *> descList;
	UI::Clipboard clipboard(me->GetHandle());
	clipboard.GetDataFormats(formats);
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
		if (clipboard.GetDataText(fmtId, sb))
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
					id = ParseURL(Text::CStringNN(url, urlLen), webType);

					if (id != 0)
					{
						NN<Text::String> s = Text::String::NewNotNullSlow(desc);
						if (me->core->FileAdd(id, webType, s))
						{
							Sync::MutexUsage mutUsage;
							NN<SSWR::DownloadMonitor::DownMonCore::FileInfo> file;
							if (me->core->FileGet(id, 3, mutUsage).SetTo(file))
							{
								sptr = Text::StrInt32(sbuff, file->id);
								j = me->lvFiles->AddItem(CSTRP(sbuff, sptr), (void*)(IntOS)((file->webType << 24) | file->id));
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

void __stdcall SSWR::DownloadMonitor::DownMonMainForm::OnCopyTableClicked(AnyType userObj)
{
	NN<SSWR::DownloadMonitor::DownMonMainForm> me = userObj.GetNN<SSWR::DownloadMonitor::DownMonMainForm>();
	Text::StringBuilderUTF8 sb;
	UIntOS i;
	UIntOS j;
	i = 0;
	j = me->lvFiles->GetCount();
	while (i < j)
	{
		Int32 id = (Int32)me->lvFiles->GetItem(i).GetIntOS();
		Sync::MutexUsage mutUsage;
		NN<SSWR::DownloadMonitor::DownMonCore::FileInfo> file;
		if (me->core->FileGet(id & 0xffffff, id >> 24, mutUsage).SetTo(file))
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

void __stdcall SSWR::DownloadMonitor::DownMonMainForm::OnFilesDblClick(AnyType userObj, UIntOS itemIndex)
{
	NN<SSWR::DownloadMonitor::DownMonMainForm> me = userObj.GetNN<SSWR::DownloadMonitor::DownMonMainForm>();
	Int32 id = (Int32)me->lvFiles->GetItem(itemIndex).GetIntOS();
	if (id)
	{
		me->core->FileStart(id & 0xffffff, id >> 24, me->GetHandle());
	}
}

void __stdcall SSWR::DownloadMonitor::DownMonMainForm::OnFileEndClicked(AnyType userObj)
{
	NN<SSWR::DownloadMonitor::DownMonMainForm> me = userObj.GetNN<SSWR::DownloadMonitor::DownMonMainForm>();
	Int32 id = (Int32)me->lvFiles->GetSelectedItem().GetIntOS();
	if (id > 0)
	{
		if (me->ui->ShowMsgYesNo(CSTR("Are you sure to remove selected file?"), CSTR("Question"), me))
		{
			me->core->FileEnd(id & 0xffffff, id >> 24);
			Sync::MutexUsage mutUsage(me->endedMut);
			me->endedList.Add(id);
			mutUsage.EndUse();
		}
	}
}

void __stdcall SSWR::DownloadMonitor::DownMonMainForm::OnWebUpdateClicked(AnyType userObj)
{
	NN<SSWR::DownloadMonitor::DownMonMainForm> me = userObj.GetNN<SSWR::DownloadMonitor::DownMonMainForm>();
	Int32 webType = 4;
	Int32 maxId = me->core->FileGetMaxId(webType);
	IntOS currPage = 1;
	UTF8Char sbuff[32];
	UnsafeArray<UTF8Char> sptr;
	UIntOS i;
	Data::ArrayListNN<Net::WebSite::WebSite48IdolControl::ItemData> pageList;
	Data::ArrayListNN<Net::WebSite::WebSite48IdolControl::ItemData> totalList;
	NN<Net::WebSite::WebSite48IdolControl::ItemData> item;
	Net::WebSite::WebSite48IdolControl *ctrl;
	NN<Text::EncodingFactory> encFact;
	Text::CStringNN userAgent = Net::UserAgentDB::FindUserAgent(Manage::OSInfo::OT_WINDOWS_NT64, Net::BrowserInfo::BT_FIREFOX);
	NN<Text::String> ua = Text::String::New(userAgent);
	NEW_CLASSNN(encFact, Text::EncodingFactory());
	NEW_CLASS(ctrl, Net::WebSite::WebSite48IdolControl(me->core->GetTCPClientFactory(), me->core->GetSSLEngine(), encFact, ua.Ptr()));
	ua->Release();
	while (true)
	{
		ctrl->GetTVPageItems(currPage, pageList);
		i = pageList.GetCount();
		printf("Page %d get, %d items found\r\n", (Int32)currPage, (Int32)i);
		if (i <= 0)
		{
			break;
		}
		while (pageList.GetCount() > 0)
		{
			item = pageList.GetItemNoCheck(0);
			if (item->id <= maxId)
			{
				break;
			}
			totalList.Add(item);
			pageList.RemoveAt(0);
		}
		if (maxId == 0 || pageList.GetCount() > 0)
		{
			ctrl->FreeItems(pageList);
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
			item = totalList.GetItemNoCheck(i);
			if (me->core->FileAdd(item->id, webType, item->title))
			{
				Sync::MutexUsage mutUsage;
				NN<SSWR::DownloadMonitor::DownMonCore::FileInfo> file;
				UIntOS j;
				if (me->core->FileGet(item->id, webType, mutUsage).SetTo(file))
				{
					sptr = Text::StrInt32(UARR(sbuff), file->id);
					j = me->lvFiles->AddItem(CSTRP(sbuff, sptr), (void*)(IntOS)((file->webType << 24) | file->id));
					me->lvFiles->SetSubItem(j, 1, file->fileName);
					changed = true;
				}
			}
		}
		ctrl->FreeItems(totalList);
		if (changed)
		{
			me->SaveList();
		}
	}

	DEL_CLASS(ctrl);
	encFact.Delete();
}

void __stdcall SSWR::DownloadMonitor::DownMonMainForm::On30MinutesClicked(AnyType userObj)
{
	NN<SSWR::DownloadMonitor::DownMonMainForm> me = userObj.GetNN<SSWR::DownloadMonitor::DownMonMainForm>();
	UTF8Char sbuff[64];
	UnsafeArray<UTF8Char> sptr;
	Data::DateTime dt;
	dt.SetCurrTime();
	dt.AddMinute(30);
	me->alarmTime = dt.ToTicks();
	me->alarmSet = true;
	sptr = dt.ToString(sbuff, "HH:mm:ss.fff");
	me->lblAlarm->SetText(CSTRP(sbuff, sptr));
}

void __stdcall SSWR::DownloadMonitor::DownMonMainForm::OnFileEnd(AnyType userObj, Int32 fileId, Int32 webType)
{
	NN<SSWR::DownloadMonitor::DownMonMainForm> me = userObj.GetNN<SSWR::DownloadMonitor::DownMonMainForm>();
	Sync::MutexUsage mutUsage(me->endedMut);
	me->endedList.Add((webType << 24) | fileId);
	mutUsage.EndUse();
}

Int32 SSWR::DownloadMonitor::DownMonMainForm::ParseURL(Text::CStringNN url, OutParam<Int32> webType)
{
	Int32 id;
	if (url.StartsWith(UTF8STRC("https://48idol.com/video/")))
	{
		id = Text::StrToInt32(&url.v[25]);
		webType.Set(1);
	}
	else if (url.StartsWith(UTF8STRC("https://48idol.net/video/")))
	{
		id = Text::StrToInt32(&url.v[25]);
		webType.Set(2);
	}
	else if (url.StartsWith(UTF8STRC("https://48idol.tv/archive/video/")))
	{
		id = Text::StrToInt32(&url.v[32]);
		webType.Set(3);
	}
	else if (url.StartsWith(UTF8STRC("https://48idol.tv/video/")))
	{
		id = Text::StrToInt32(&url.v[24]);
		webType.Set(4);
	}
	else
	{
		id = 0;
		webType.Set(0);
	}
	return id;
}

void SSWR::DownloadMonitor::DownMonMainForm::LoadList()
{
	Text::StringBuilderUTF8 sb;

	Net::WebSite::WebSite48IdolControl *ctrl = 0;
	Text::EncodingFactory *encFact = 0;
	Text::CStringNN userAgent = Net::UserAgentDB::FindUserAgent(Manage::OSInfo::OT_WINDOWS_NT64, Net::BrowserInfo::BT_FIREFOX);
	NN<Text::String> ua = Text::String::New(userAgent);
	Text::StringBuilderUTF8 sb2;
	Bool updated = false;

	UTF8Char sbuff[32];
	UnsafeArray<UTF8Char> sptr;
	Text::PString sarr[2];
	UIntOS i;
	NN<Text::String> listFile;
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
				id = ParseURL(sarr[0].ToCString(), webType);

				if (id != 0)
				{
					if (!Text::UTF8Util::ValidStr(sarr[1].v))
					{
						printf("Invalid char found, id = %d\r\n", id);
						if (ctrl == 0)
						{
							NEW_CLASS(encFact, Text::EncodingFactory());
							NEW_CLASS(ctrl, Net::WebSite::WebSite48IdolControl(this->core->GetTCPClientFactory(), this->core->GetSSLEngine(), encFact, ua.Ptr()));
						}
						sb2.ClearStr();
						if (ctrl->GetVideoName(id, sb2))
						{
							printf("Name of id %d updated\r\n", id);
							sarr[1] = sb2;
							updated = true;
						}
					}
					NN<Text::String> s = Text::String::New(sarr[1].v, sarr[1].leng);
					if (this->core->FileAdd(id, webType, s))
					{
						Sync::MutexUsage mutUsage;
						NN<SSWR::DownloadMonitor::DownMonCore::FileInfo> file;
						if (this->core->FileGet(id, webType, mutUsage).SetTo(file))
						{
							sptr = Text::StrInt32(UARR(sbuff), file->id);
							i = this->lvFiles->AddItem(CSTRP(sbuff, sptr), (void*)(IntOS)((file->webType << 24) | file->id));
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
	UIntOS i;
	UIntOS j;

	NN<Text::String> listFile;
	if (!listFile.Set(this->core->GetListFile()))
		return;
	IO::FileStream fs(listFile, IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
	Text::UTF8Writer writer(fs);
	writer.WriteSignature();
	i = 0;
	j = this->lvFiles->GetCount();
	while (i < j)
	{
		Int32 id = (Int32)this->lvFiles->GetItem(i).GetIntOS();
		Sync::MutexUsage mutUsage;
		NN<SSWR::DownloadMonitor::DownMonCore::FileInfo> file;
		if (this->core->FileGet(id & 0xffffff, id >> 24, mutUsage).SetTo(file))
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
			writer.WriteLine(sb.ToCString());
		}
		i++;
	}
}

SSWR::DownloadMonitor::DownMonMainForm::DownMonMainForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, SSWR::DownloadMonitor::DownMonCore *core) : UI::GUIForm(parent, 1024, 768, ui)
{
	this->core = core;
	this->SetText(CSTR("Download Monitor"));
	this->SetFont(nullptr, 8.25, false);
	this->alarmSet = false;
	this->alarmTime = 0;
	this->core->SetFileEndHandler(OnFileEnd, this);

	this->pnlButtons = ui->NewPanel(*this);
	this->pnlButtons->SetRect(0, 0, 100, 55, false);
	this->pnlButtons->SetDockType(UI::GUIControl::DOCK_TOP);
	this->lblStatus = ui->NewLabel(this->pnlButtons, CSTR("Status"));
	this->lblStatus->SetRect(4, 4, 100, 23, false);
	this->txtStatus = ui->NewTextBox(this->pnlButtons, CSTR("Idle"));
	this->txtStatus->SetRect(104, 4, 100, 23, false);
	this->txtStatus->SetReadOnly(true);
	this->chkAutoStart = ui->NewCheckBox(this->pnlButtons, CSTR("AutoStart"), false);
	this->chkAutoStart->SetRect(204, 4, 100, 23, false);
	this->btnPasteTable = ui->NewButton(this->pnlButtons, CSTR("Paste Table"));
	this->btnPasteTable->SetRect(304, 4, 75, 23, false);
	this->btnPasteTable->HandleButtonClick(OnPasteTableClicked, this);
	this->btnPasteHTML = ui->NewButton(this->pnlButtons, CSTR("Paste HTML"));
	this->btnPasteHTML->SetRect(384, 4, 75, 23, false);
	this->btnPasteHTML->HandleButtonClick(OnPasteHTMLClicked, this);
	this->btnCopyTable = ui->NewButton(this->pnlButtons, CSTR("Copy Table"));
	this->btnCopyTable->SetRect(464, 4, 75, 23, false);
	this->btnCopyTable->HandleButtonClick(OnCopyTableClicked, this);
	this->btnFileEnd = ui->NewButton(this->pnlButtons, CSTR("File End"));
	this->btnFileEnd->SetRect(544, 4, 75, 23, false);
	this->btnFileEnd->HandleButtonClick(OnFileEndClicked, this);
	this->btnWebUpdate = ui->NewButton(this->pnlButtons, CSTR("Web Update"));
	this->btnWebUpdate->SetRect(624, 4, 75, 23, false);
	this->btnWebUpdate->HandleButtonClick(OnWebUpdateClicked, this);
	this->lblAlarm = ui->NewLabel(this->pnlButtons, CSTR(""));
	this->lblAlarm->SetRect(4, 28, 100, 23, false);
	this->btn30Minutes = ui->NewButton(this->pnlButtons, CSTR("30 Minutes"));
	this->btn30Minutes->SetRect(104, 28, 75, 23, false);
	this->btn30Minutes->HandleButtonClick(On30MinutesClicked, this);

	this->lvFiles = ui->NewListView(*this, UI::ListViewStyle::Table, 3);
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
