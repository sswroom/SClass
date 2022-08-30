#include "Stdafx.h"
#include "IO/Path.h"
#include "SSWR/AVIRead/AVIRPackageForm.h"
#include "Sync/MutexUsage.h"
#include "Sync/Thread.h"
#include "Text/MyString.h"
#include "Text/MyStringFloat.h"
#include "Text/URLString.h"
#include "UI/FolderDialog.h"
#include "UI/MessageDialog.h"
#include "Win32/Clipboard.h"

typedef enum
{
	MNU_PASTE = 100,
	MNU_COPYTO,
	MNU_COPYALLTO,
	MNU_SAVEAS
} MenuItem;

UInt32 __stdcall SSWR::AVIRead::AVIRPackageForm::ProcessThread(void *userObj)
{
	SSWR::AVIRead::AVIRPackageForm *me = (SSWR::AVIRead::AVIRPackageForm*)userObj;
	UTF8Char sbuff[512];
	UTF8Char *sptr;
	Text::String *fname = 0;
	Text::CString fileName = CSTR_NULL;
	ActionType atype = AT_COPY;
	UOSInt i;
	UOSInt j;
	Bool found;
	Bool lastFound = false;
	me->threadRunning = true;
	while (!me->threadToStop)
	{
		found = false;
		Sync::MutexUsage mutUsage(&me->fileMut);
		i = 0;
		j = me->fileNames.GetCount();
		while (!found && i < j)
		{
			fname = me->fileNames.GetItem(i);
			atype = me->fileAction.GetItem(i);
			switch (atype)
			{
			case AT_COPY:
				found = true;
				break;
			case AT_MOVE:
				found = true;
				break;
			case AT_RETRYCOPY:
				found = true;
				break;
			case AT_RETRYMOVE:
				found = true;
				break;
			case AT_DELETE:
			case AT_DELETEFAIL:
			case AT_COPYFAIL:
			case AT_MOVEFAIL:
			case AT_SUCCEED:
			default:
				break;
			}
			i++;
		}
		mutUsage.EndUse();
		if (found)
		{
		
			if (fname->StartsWith(UTF8STRC("file:///")))
			{
				sptr = Text::URLString::GetURLFilePath(sbuff, fname->v, fname->leng);
				fileName = CSTRP(sbuff, sptr);
			}
			else
			{
				fileName = fname->ToCString();
			}
			lastFound = true;
			if (atype == AT_COPY)
			{
				found = me->packFile->CopyFrom(fileName, me, &me->statusBNT);
				mutUsage.BeginUse();
				i = me->fileNames.GetCount();
				while (i-- > 0)
				{
					if (me->fileNames.GetItem(i) == fname)
					{
						if (found)
						{
							me->fileAction.SetItem(i, AT_SUCCEED);
						}
						else
						{
							me->fileAction.SetItem(i, AT_COPYFAIL);
						}
						me->statusChg = true;
						break;
					}
				}
				mutUsage.EndUse();
			}
			else if (atype == AT_MOVE)
			{
				found = me->packFile->MoveFrom(fileName, me, &me->statusBNT);
				mutUsage.BeginUse();
				i = me->fileNames.GetCount();
				while (i-- > 0)
				{
					if (me->fileNames.GetItem(i) == fname)
					{
						if (found)
						{
							me->fileAction.SetItem(i, AT_SUCCEED);
						}
						else
						{
							me->fileAction.SetItem(i, AT_MOVEFAIL);
						}
						me->statusChg = true;
						break;
					}
				}
				mutUsage.EndUse();
			}
			else if (atype == AT_RETRYCOPY)
			{
				found = me->packFile->RetryCopyFrom(fileName, me, &me->statusBNT);
				mutUsage.BeginUse();
				i = me->fileNames.GetCount();
				while (i-- > 0)
				{
					if (me->fileNames.GetItem(i) == fname)
					{
						if (found)
						{
							me->fileAction.SetItem(i, AT_SUCCEED);
						}
						else
						{
							me->fileAction.SetItem(i, AT_COPYFAIL);
						}
						me->statusChg = true;
						break;
					}
				}
				mutUsage.EndUse();
			}
			else if (atype == AT_RETRYMOVE)
			{
				found = me->packFile->RetryCopyFrom(fileName, me, &me->statusBNT);
				mutUsage.BeginUse();
				i = me->fileNames.GetCount();
				while (i-- > 0)
				{
					if (me->fileNames.GetItem(i) == fname)
					{
						if (found)
						{
							me->fileAction.SetItem(i, AT_SUCCEED);
						}
						else
						{
							me->fileAction.SetItem(i, AT_MOVEFAIL);
						}
						me->statusChg = true;
						break;
					}
				}
				mutUsage.EndUse();
			}
		}
		else
		{
			if (lastFound)
			{
				me->ProgressEnd();
			}
			lastFound = false;
			me->threadEvt.Wait(1000);
		}
	}
	me->threadRunning = false;
	return 0;
}

void __stdcall SSWR::AVIRead::AVIRPackageForm::OnTimerTick(void *userObj)
{
	SSWR::AVIRead::AVIRPackageForm *me = (SSWR::AVIRead::AVIRPackageForm*)userObj;
	UTF8Char sbuff[64];
	UTF8Char *sptr;
	if (me->statusChg)
	{
		UOSInt i;
		UOSInt j;
		UOSInt k;
		OSInt scrVPos = me->lvStatus->GetScrollVPos();
		Text::String *fname;
		me->statusChg = false;
		me->lvStatus->ClearItems();
		Sync::MutexUsage mutUsage(&me->fileMut);
		i = 0;
		j = me->fileNames.GetCount();
		while (i < j)
		{
			fname = me->fileNames.GetItem(i);
			k = fname->LastIndexOf(IO::Path::PATH_SEPERATOR);
			k = me->lvStatus->AddItem(fname->ToCString().Substring(k + 1), (void*)fname->v);
			switch (me->fileAction.GetItem(i))
			{
			case AT_COPY:
				me->lvStatus->SetSubItem(k, 1, CSTR("Copy"));
				break;
			case AT_MOVE:
				me->lvStatus->SetSubItem(k, 1, CSTR("Move"));
				break;
			case AT_DELETE:
				me->lvStatus->SetSubItem(k, 1, CSTR("Delete"));
				break;
			case AT_COPYFAIL:
				me->lvStatus->SetSubItem(k, 1, CSTR("Copy Failed"));
				break;
			case AT_MOVEFAIL:
				me->lvStatus->SetSubItem(k, 1, CSTR("Move Failed"));
				break;
			case AT_DELETEFAIL:
				me->lvStatus->SetSubItem(k, 1, CSTR("Delete Failed"));
				break;
			case AT_SUCCEED:
				me->lvStatus->SetSubItem(k, 1, CSTR("Succeed"));
				break;
			case AT_RETRYCOPY:
				me->lvStatus->SetSubItem(k, 1, CSTR("Retry Copy"));
				break;
			case AT_RETRYMOVE:
				me->lvStatus->SetSubItem(k, 1, CSTR("Retry Move"));
				break;
			}
			i++;
		}
		mutUsage.EndUse();
		me->lvStatus->ScrollTo(0, scrVPos);
	}

	UInt64 readPos;
	UInt64 readCurr;
	Int64 timeDiff;
	Double spd;
	Sync::MutexUsage readMutUsage(&me->readMut);
	readPos = me->readLast;
	readCurr = me->readCurr;
	me->readCurr = 0;
	timeDiff = me->readReadTime.DiffMS(&me->readLastTimer);
	me->readLastTimer.SetValue(&me->readReadTime);
	readMutUsage.EndUse();
	if (timeDiff > 0)
	{
		spd = (Double)readCurr * 1000 / (Double)timeDiff;
		
	}
	else
	{
		spd = 0;
	}
	me->rlcStatus->AddSample(&spd);

	Sync::MutexUsage progMutUsage(&me->progMut);
	if (me->progStarted)
	{
		me->progStarted = false;
		me->prgStatus->ProgressStart(me->progName->ToCString(), me->progStartCnt);
	}
	UInt64 progUpdateCurr = me->progUpdateCurr;
	UInt64 progUpdateNew = me->progUpdateNew;
	Bool progUpdated = me->progUpdated;
	Bool progEnd = me->progEnd;
	me->progUpdated = false;
	me->progEnd = false;
	progMutUsage.EndUse();
	if (progUpdated)
	{
		me->prgStatus->ProgressUpdate(progUpdateCurr, progUpdateNew);
	}
	if (progEnd)
	{
		me->prgStatus->ProgressEnd();
	}

	Bool hasFile;
	UInt64 fileSize = 0;
	if (me->statusFileChg)
	{
		Sync::MutexUsage mutUsage(&me->statusFileMut);
		hasFile = (me->statusFile != 0);
		if (hasFile)
		{
			me->txtStatusFile->SetText(me->statusFile->ToCString());
			sptr = Text::StrUInt64(sbuff, me->statusFileSize);
			me->txtStatusFileSize->SetText(CSTRP(sbuff, sptr));
			fileSize = me->statusFileSize;
		}
		else
		{
			me->txtStatusFile->SetText(CSTR(""));
			me->txtStatusFileSize->SetText(CSTR(""));
		}
		mutUsage.EndUse();
	}
	else
	{
		Sync::MutexUsage mutUsage(&me->statusFileMut);
		hasFile = (me->statusFile != 0);
		fileSize = me->statusFileSize;
		mutUsage.EndUse();
	}
	if (readPos != me->statusDispSize)
	{
		sptr = Text::StrUInt64(sbuff, readPos);
		me->txtStatusCurrSize->SetText(CSTRP(sbuff, sptr));
		me->statusDispSize = readPos;
	}
	if (spd != me->statusDispSpd)
	{
		sptr = Text::StrDoubleFmt(sbuff, spd, "0.0");
		me->txtStatusCurrSpeed->SetText(CSTRP(sbuff, sptr));
		me->statusDispSpd = spd;
	}
	if (hasFile && fileSize != 0 && spd != 0)
	{
		Double t = (Double)(fileSize - readPos) / spd;
		sptr = Text::StrDoubleFmt(sbuff, t, "0.0");
		me->txtStatusTimeLeft->SetText(CSTRP(sbuff, sptr));
	}
	else
	{
		me->txtStatusTimeLeft->SetText(CSTR("?"));
	}
	if (me->statusDispBNT != me->statusBNT)
	{
		me->statusDispBNT = me->statusBNT;
		if (me->statusDispBNT == IO::ActiveStreamReader::BNT_UNKNOWN)
		{
			me->pnlStatusBNT->SetBGColor(0xffc0c0c0);
		}
		else if (me->statusDispBNT == IO::ActiveStreamReader::BNT_READ)
		{
			me->pnlStatusBNT->SetBGColor(0xff60ff60);
		}
		else if (me->statusDispBNT == IO::ActiveStreamReader::BNT_WRITE)
		{
			me->pnlStatusBNT->SetBGColor(0xffff4040);
		}
		me->pnlStatus->Redraw();
	}
}

void __stdcall SSWR::AVIRead::AVIRPackageForm::LVDblClick(void *userObj, UOSInt index)
{
	SSWR::AVIRead::AVIRPackageForm *me = (SSWR::AVIRead::AVIRPackageForm*)userObj;
	IO::PackageFile::PackObjectType pot = me->packFile->GetItemType(index);
	if (pot == IO::PackageFile::POT_PACKAGEFILE)
	{
		IO::PackageFile *pkg = me->packFile->GetItemPack(index);
		if (pkg)
		{
			me->core->OpenObject(pkg);
		}
	}
	else if (pot == IO::PackageFile::POT_PARSEDOBJECT)
	{
		IO::ParsedObject *pobj = me->packFile->GetItemPObj(index);
		if (pobj)
		{
			me->core->OpenObject(pobj);
		}
	}
	else if (pot == IO::PackageFile::POT_STREAMDATA)
	{
		IO::IStreamData *data = me->packFile->GetItemStmData(index);
		if (data)
		{
			me->core->LoadData(data, me->packFile);
			DEL_CLASS(data);
		}
	}
}

void __stdcall SSWR::AVIRead::AVIRPackageForm::OnStatusDblClick(void *userObj, UOSInt index)
{
	SSWR::AVIRead::AVIRPackageForm *me = (SSWR::AVIRead::AVIRPackageForm*)userObj;
	Sync::MutexUsage mutUsage(&me->fileMut);
	if (me->fileAction.GetItem(index) == AT_COPYFAIL)
	{
		me->fileAction.SetItem(index, AT_RETRYCOPY);
		me->statusChg = true;
	}
	else if (me->fileAction.GetItem(index) == AT_MOVEFAIL)
	{
		me->fileAction.SetItem(index, AT_RETRYMOVE);
		me->statusChg = true;
	}
}

void SSWR::AVIRead::AVIRPackageForm::DisplayPackFile(IO::PackageFile *packFile)
{
	UTF8Char sbuff[512];
	UTF8Char *sptr;
	Data::DateTime dt;
	UOSInt maxWidth = 0;
	UOSInt w;
	this->lvFiles->ClearItems();
	UOSInt i;
	UOSInt j;
	UOSInt k;
	IO::PackageFile::PackObjectType pot;
	dt.ToLocalTime();
	i = 0;
	j = packFile->GetCount();
	while (i < j)
	{
		sptr = packFile->GetItemName(sbuff, i);
		pot = packFile->GetItemType(i);
		k = this->lvFiles->AddItem(CSTRP(sbuff, sptr), (void*)i);
		w = this->lvFiles->GetStringWidth(sbuff);
		if (w > maxWidth)
			maxWidth = w;

		dt.SetTicks(packFile->GetItemModTimeTick(i));
		sptr = dt.ToString(sbuff, "yyyy-MM-dd HH:mm:ss.fff");
		this->lvFiles->SetSubItem(k, 4, CSTRP(sbuff, sptr));
		if (pot == IO::PackageFile::POT_STREAMDATA)
		{
			this->lvFiles->SetSubItem(k, 1, CSTR("File"));
			sptr = Text::StrUInt64(sbuff, packFile->GetItemStoreSize(i));
			this->lvFiles->SetSubItem(k, 2, CSTRP(sbuff, sptr));
			sptr = Text::StrUInt64(sbuff, packFile->GetItemSize(i));
			this->lvFiles->SetSubItem(k, 3, CSTRP(sbuff, sptr));
			if (packFile->IsCompressed(i))
			{
				this->lvFiles->SetSubItem(k, 5, Data::Compress::Decompressor::GetCompMethName(packFile->GetItemComp(i)));
			}
			else
			{
				this->lvFiles->SetSubItem(k, 5, CSTR("Uncompressed"));
			}
		}
		else if (pot == IO::PackageFile::POT_PACKAGEFILE)
		{
			this->lvFiles->SetSubItem(k, 1, CSTR("Folder"));
		}
		else if (pot == IO::PackageFile::POT_PARSEDOBJECT)
		{
			this->lvFiles->SetSubItem(k, 1, CSTR("Object"));
		}
		else
		{
			this->lvFiles->SetSubItem(k, 1, CSTR("Unknown"));
		}

		i++;
	}
	if (maxWidth > 0)
	{
		this->lvFiles->SetColumnWidth(0, (Int32)(maxWidth + 6));
	}
}

SSWR::AVIRead::AVIRPackageForm::AVIRPackageForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core, IO::PackageFile *packFile) : UI::GUIForm(parent, 960, 768, ui)
{
	this->SetFont(0, 0, 8.25, false);
	UTF8Char sbuff[512];
	UTF8Char *sptr;
	sptr = packFile->GetSourceNameObj()->ConcatTo(Text::StrConcatC(sbuff, UTF8STRC("Package Form - ")));
	this->SetText(CSTRP(sbuff, sptr));

	this->core = core;
	this->packFile = packFile;
	this->statusChg = false;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
	this->threadRunning = false;
	this->threadToStop = false;
	this->readLastTimer.SetCurrTimeUTC();
	this->readReadTime.SetValue(&this->readLastTimer);
	this->readTotal = 0;
	this->readCurr = 0;
	this->readLast = 0;
	this->readCurrFile = CSTR_NULL;
	this->readFileCnt = 0;
	this->statusFileChg = false;
	this->statusFile = 0;
	this->statusFileSize = 0;
	this->statusDispSize = (UOSInt)-1;
	this->statusDispSpd = -1;
	this->statusBNT = IO::ActiveStreamReader::BNT_UNKNOWN;
	this->statusDispBNT = IO::ActiveStreamReader::BNT_UNKNOWN;
	this->progName = 0;
	this->progStarted = false;
	this->progUpdated = false;
	this->progEnd = false;
	
/*	NEW_CLASS(this->pnlCtrl, UI::GUIPanel(ui, this));
	this->pnlCtrl->SetRect(0, 0, 100, 28, false);
	this->pnlCtrl->SetDockType(UI::GUIControl::DOCK_TOP);*/
	NEW_CLASS(this->prgStatus, UI::GUIProgressBar(ui, this, 1));
	this->prgStatus->SetRect(0, 0, 100, 23, false);
	this->prgStatus->SetDockType(UI::GUIControl::DOCK_BOTTOM);
	NEW_CLASS(this->tcMain, UI::GUITabControl(ui, this));
	this->tcMain->SetDockType(UI::GUIControl::DOCK_FILL);


	this->tpFiles = this->tcMain->AddTabPage(CSTR("Files"));
	NEW_CLASS(this->lvFiles, UI::GUIListView(ui, this->tpFiles, UI::GUIListView::LVSTYLE_TABLE, 6));
	this->lvFiles->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lvFiles->AddColumn(CSTR("Item Name"), 150);
	this->lvFiles->AddColumn(CSTR("Type"), 80);
	this->lvFiles->AddColumn(CSTR("Store Size"), 100);
	this->lvFiles->AddColumn(CSTR("File Size"), 100);
	this->lvFiles->AddColumn(CSTR("Modify Time"), 150);
	this->lvFiles->AddColumn(CSTR("Compression"), 100);

	this->lvFiles->HandleDblClk(LVDblClick, this);
	DisplayPackFile(this->packFile);

	UI::GUIMainMenu *mnuMain;
	UI::GUIMenu *mnu;
//	UI::GUIMenu *mnu2;
	UOSInt ind;
	NEW_CLASS(mnuMain, UI::GUIMainMenu());
	mnu = mnuMain->AddSubMenu(CSTR("&File"));
	mnu->AddItem(CSTR("Save &As..."), MNU_SAVEAS, UI::GUIMenu::KM_CONTROL, UI::GUIControl::GK_S);
	mnu = mnuMain->AddSubMenu(CSTR("&Edit"));
	ind = mnu->AddItem(CSTR("&Paste"), MNU_PASTE, UI::GUIMenu::KM_CONTROL, UI::GUIControl::GK_V);
	mnu->AddItem(CSTR("Copy To..."), MNU_COPYTO, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu->AddItem(CSTR("Copy All To..."), MNU_COPYALLTO, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);

	NEW_CLASS(this->mnuPopup, UI::GUIPopupMenu());
	this->mnuPopup->AddItem(CSTR("Copy To..."), MNU_COPYTO, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);

	if (!packFile->AllowWrite())
	{
		mnu->SetItemEnabled((UInt16)ind, false);
	}
	else
	{
		this->tpStatus = this->tcMain->AddTabPage(CSTR("Status"));
		NEW_CLASS(this->pnlStatus, UI::GUIPanel(ui, this->tpStatus));
		this->pnlStatus->SetRect(0, 0, 100, 48, false);
		this->pnlStatus->SetDockType(UI::GUIControl::DOCK_TOP);
		NEW_CLASS(this->lblStatusFile, UI::GUILabel(ui, this->pnlStatus, CSTR("Copy From")));
		this->lblStatusFile->SetRect(0, 0, 100, 23, false);
		NEW_CLASS(this->txtStatusFile, UI::GUITextBox(ui, this->pnlStatus, CSTR("")));
		this->txtStatusFile->SetRect(100, 0, 800, 23, false);
		this->txtStatusFile->SetReadOnly(true);
		NEW_CLASS(this->pnlStatusBNT, UI::GUIPanel(ui, this->pnlStatus));
		this->pnlStatusBNT->SetBGColor(0xffc0c0c0);
		this->pnlStatusBNT->SetRect(900, 0, 23, 23, false);
		NEW_CLASS(this->lblStatusFileSize, UI::GUILabel(ui, this->pnlStatus, CSTR("File Size")));
		this->lblStatusFileSize->SetRect(0, 24, 100, 23, false);
		NEW_CLASS(this->txtStatusFileSize, UI::GUITextBox(ui, this->pnlStatus, CSTR("")));
		this->txtStatusFileSize->SetRect(100, 24, 100, 23, false);
		this->txtStatusFileSize->SetReadOnly(true);
		NEW_CLASS(this->lblStatusCurrSize, UI::GUILabel(ui, this->pnlStatus, CSTR("Curr Size")));
		this->lblStatusCurrSize->SetRect(220, 24, 100, 23, false);
		NEW_CLASS(this->txtStatusCurrSize, UI::GUITextBox(ui, this->pnlStatus, CSTR("")));
		this->txtStatusCurrSize->SetRect(320, 24, 100, 23, false);
		this->txtStatusCurrSize->SetReadOnly(true);
		NEW_CLASS(this->lblStatusCurrSpeed, UI::GUILabel(ui, this->pnlStatus, CSTR("Curr Speed")));
		this->lblStatusCurrSpeed->SetRect(440, 24, 100, 23, false);
		NEW_CLASS(this->txtStatusCurrSpeed, UI::GUITextBox(ui, this->pnlStatus, CSTR("")));
		this->txtStatusCurrSpeed->SetRect(540, 24, 100, 23, false);
		this->txtStatusCurrSpeed->SetReadOnly(true);
		NEW_CLASS(this->lblStatusTimeLeft, UI::GUILabel(ui, this->pnlStatus, CSTR("Time Left")));
		this->lblStatusTimeLeft->SetRect(660, 24, 100, 23, false);
		NEW_CLASS(this->txtStatusTimeLeft, UI::GUITextBox(ui, this->pnlStatus, CSTR("")));
		this->txtStatusTimeLeft->SetRect(760, 24, 100, 23, false);
		this->txtStatusTimeLeft->SetReadOnly(true);
		NEW_CLASS(this->rlcStatus, UI::GUIRealtimeLineChart(ui, this->tpStatus, this->core->GetDrawEngine(), 1, 720, 1000));
		this->rlcStatus->SetRect(0, 0, 100, 360, false);
		this->rlcStatus->SetDockType(UI::GUIControl::DOCK_BOTTOM);
		NEW_CLASS(this->vspStatus, UI::GUIVSplitter(ui, this->tpStatus, 3, true));
		NEW_CLASS(this->lvStatus, UI::GUIListView(ui, this->tpStatus, UI::GUIListView::LVSTYLE_TABLE, 2));
		this->lvStatus->SetDockType(UI::GUIControl::DOCK_FILL);
		this->lvStatus->SetFullRowSelect(true);
		this->lvStatus->AddColumn(CSTR("Source File"), 200);
		this->lvStatus->AddColumn(CSTR("Status"), 200);
		this->lvStatus->HandleDblClk(OnStatusDblClick, this);

		this->AddTimer(500, OnTimerTick, this);
		Sync::Thread::Create(ProcessThread, this);
	}

	this->tpInfo = this->tcMain->AddTabPage(CSTR("Info"));
	NEW_CLASS(this->txtInfo, UI::GUITextBox(ui, this->tpInfo, CSTR(""), true));
	this->txtInfo->SetReadOnly(true);
	this->txtInfo->SetDockType(UI::GUIControl::DOCK_FILL);
	Text::StringBuilderUTF8 sb;
	this->packFile->GetInfoText(&sb);
	this->txtInfo->SetText(sb.ToCString());

	this->SetMenu(mnuMain);
}

SSWR::AVIRead::AVIRPackageForm::~AVIRPackageForm()
{
	this->threadToStop = true;
	this->threadEvt.Set();
	while (this->threadRunning)
	{
		Sync::Thread::Sleep(10);
	}
	DEL_CLASS(this->packFile);
	LIST_FREE_STRING(&this->fileNames);
	SDEL_STRING(this->statusFile);
	DEL_CLASS(this->mnuPopup);
	SDEL_STRING(this->progName);
}

void SSWR::AVIRead::AVIRPackageForm::EventMenuClicked(UInt16 cmdId)
{
	switch (cmdId)
	{
	case MNU_PASTE:
		{
			Win32::Clipboard clipboard(this->hwnd);
			Data::ArrayList<Text::String *> fileNames;
			Win32::Clipboard::FilePasteType fpt;
			UOSInt i;
			UOSInt j;
			fpt = clipboard.GetDataFiles(&fileNames);
			if (fpt == Win32::Clipboard::FPT_MOVE)
			{
				Sync::MutexUsage mutUsage(&this->fileMut);
				i = 0;
				j = fileNames.GetCount();
				while (i < j)
				{
					this->fileNames.Add(fileNames.GetItem(i)->Clone());
					this->fileAction.Add(AT_MOVE);
					i++;
				}
				this->statusChg = true;
				mutUsage.EndUse();
				clipboard.FreeDataFiles(&fileNames);
			}
			else if (fpt == Win32::Clipboard::FPT_COPY)
			{
				Sync::MutexUsage mutUsage(&this->fileMut);
				i = 0;
				j = fileNames.GetCount();
				while (i < j)
				{
					this->fileNames.Add(fileNames.GetItem(i)->Clone());
					this->fileAction.Add(AT_COPY);
					i++;
				}
				this->statusChg = true;
				mutUsage.EndUse();
				clipboard.FreeDataFiles(&fileNames);
			}
		}
		break;
	case MNU_COPYTO:
		{
			Data::ArrayList<UOSInt> selIndices;
			this->lvFiles->GetSelectedIndices(&selIndices);
			UOSInt i = selIndices.GetCount();
			if (i > 0)
			{
				UI::FolderDialog dlg(L"SSWR", L"AVIRead", L"PackageCopyTo");
				if (dlg.ShowDialog(this->GetHandle()) == UI::GUIForm::DR_OK)
				{
					UOSInt j;
					j = 0;
					while (j < i)
					{
						if (!packFile->CopyTo(selIndices.GetItem(j), dlg.GetFolder()->ToCString(), false))
						{
							UI::MessageDialog::ShowDialog(CSTR("Error in copying"), CSTR("Copy To"), this);
							break;
						}
						j++;
					}
				}
			}
		}
		break;
	case MNU_COPYALLTO:
		if (packFile->GetCount() > 0)
		{
			UI::FolderDialog dlg(L"SSWR", L"AVIRead", L"PackageCopyAllTo");
			if (dlg.ShowDialog(this->GetHandle()) == UI::GUIForm::DR_OK)
			{
				UOSInt i = 0;
				UOSInt j = packFile->GetCount();
				while (i < j)
				{
					if (!packFile->CopyTo(i, dlg.GetFolder()->ToCString(), false))
					{
						UI::MessageDialog::ShowDialog(CSTR("Error in copying"), CSTR("Copy To"), this);
						break;
					}
					i++;
				}
			}
		}
		break;
	case MNU_SAVEAS:
		{
			this->core->SaveData(this, this->packFile, L"PackageSave");
		}
		break;
	}
}

void SSWR::AVIRead::AVIRPackageForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}

void SSWR::AVIRead::AVIRPackageForm::ProgressStart(Text::CString name, UInt64 count)
{
	{
		Sync::MutexUsage mutUsage(&this->readMut);
		this->readLast = 0;
		this->readCurrFile = name;
		this->readFileCnt = count;
	}

	{
		Sync::MutexUsage mutUsage(&this->statusFileMut);
		SDEL_STRING(this->statusFile);
		this->statusFile = Text::String::New(name);
		this->statusFileSize = count;
		this->statusFileChg = true;
	}

	{
		Sync::MutexUsage mutUsage(&this->progMut);
		this->progStarted = true;
		SDEL_STRING(this->progName);
		this->progName = Text::String::New(name);
		this->progStartCnt = count;
		this->progEnd = false;
	}
}

void SSWR::AVIRead::AVIRPackageForm::ProgressUpdate(UInt64 currCount, UInt64 newCount)
{
	{
		Sync::MutexUsage mutUsage(&this->readMut);
		UInt64 readThis = currCount - this->readLast;
		if ((Int64)readThis < 0)
			readThis = 0;
		this->readCurr += readThis;
		this->readTotal += readThis;
		this->readLast = currCount;
		this->readReadTime.SetCurrTimeUTC();
	}

	{
		Sync::MutexUsage mutUsage(&this->progMut);
		this->progUpdated = true;
		this->progUpdateCurr = currCount;
		this->progUpdateNew = newCount;
		this->progEnd = false;
	}
}

void SSWR::AVIRead::AVIRPackageForm::ProgressEnd()
{
	this->readLast = 0;
	this->readFileCnt = 0;
	Sync::MutexUsage mutUsage(&this->progMut);
	this->progEnd = true;
	mutUsage.EndUse();

	{
		Sync::MutexUsage mutUsage(&this->statusFileMut);
		SDEL_STRING(this->statusFile);
		this->statusFileChg = true;
	}
}
