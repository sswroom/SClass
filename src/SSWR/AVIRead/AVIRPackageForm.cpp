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
	const UTF8Char *fname = 0;
	ActionType atype = AT_COPY;
	UOSInt i;
	UOSInt j;
	Bool found;
	Bool lastFound = false;
	me->threadRunning = true;
	while (!me->threadToStop)
	{
		found = false;
		Sync::MutexUsage mutUsage(me->fileMut);
		i = 0;
		j = me->fileNames->GetCount();
		while (!found && i < j)
		{
			fname = me->fileNames->GetItem(i);
			atype = me->fileAction->GetItem(i);
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
			if (Text::StrStartsWith(fname, (const UTF8Char*)"file:///"))
			{
				Text::URLString::GetURLFilePath(sbuff, fname);
				fname = sbuff;
			}
			lastFound = true;
			if (atype == AT_COPY)
			{
				found = me->packFile->CopyFrom(fname, me, &me->statusBNT);
				mutUsage.BeginUse();
				i = me->fileNames->GetCount();
				while (i-- > 0)
				{
					if (me->fileNames->GetItem(i) == fname)
					{
						if (found)
						{
							me->fileAction->SetItem(i, AT_SUCCEED);
						}
						else
						{
							me->fileAction->SetItem(i, AT_COPYFAIL);
						}
						me->statusChg = true;
						break;
					}
				}
				mutUsage.EndUse();
			}
			else if (atype == AT_MOVE)
			{
				found = me->packFile->MoveFrom(fname, me, &me->statusBNT);
				mutUsage.BeginUse();
				i = me->fileNames->GetCount();
				while (i-- > 0)
				{
					if (me->fileNames->GetItem(i) == fname)
					{
						if (found)
						{
							me->fileAction->SetItem(i, AT_SUCCEED);
						}
						else
						{
							me->fileAction->SetItem(i, AT_MOVEFAIL);
						}
						me->statusChg = true;
						break;
					}
				}
				mutUsage.EndUse();
			}
			else if (atype == AT_RETRYCOPY)
			{
				found = me->packFile->RetryCopyFrom(fname, me, &me->statusBNT);
				mutUsage.BeginUse();
				i = me->fileNames->GetCount();
				while (i-- > 0)
				{
					if (me->fileNames->GetItem(i) == fname)
					{
						if (found)
						{
							me->fileAction->SetItem(i, AT_SUCCEED);
						}
						else
						{
							me->fileAction->SetItem(i, AT_COPYFAIL);
						}
						me->statusChg = true;
						break;
					}
				}
				mutUsage.EndUse();
			}
			else if (atype == AT_RETRYMOVE)
			{
				found = me->packFile->RetryCopyFrom(fname, me, &me->statusBNT);
				mutUsage.BeginUse();
				i = me->fileNames->GetCount();
				while (i-- > 0)
				{
					if (me->fileNames->GetItem(i) == fname)
					{
						if (found)
						{
							me->fileAction->SetItem(i, AT_SUCCEED);
						}
						else
						{
							me->fileAction->SetItem(i, AT_MOVEFAIL);
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
			me->threadEvt->Wait(1000);
		}
	}
	me->threadRunning = false;
	return 0;
}

void __stdcall SSWR::AVIRead::AVIRPackageForm::OnTimerTick(void *userObj)
{
	SSWR::AVIRead::AVIRPackageForm *me = (SSWR::AVIRead::AVIRPackageForm*)userObj;
	UTF8Char sbuff[64];
	if (me->statusChg)
	{
		UOSInt i;
		UOSInt j;
		UOSInt k;
		OSInt scrVPos = me->lvStatus->GetScrollVPos();
		const UTF8Char *fname;
		me->statusChg = false;
		me->lvStatus->ClearItems();
		Sync::MutexUsage mutUsage(me->fileMut);
		i = 0;
		j = me->fileNames->GetCount();
		while (i < j)
		{
			fname = me->fileNames->GetItem(i);
			k = Text::StrLastIndexOf(fname, IO::Path::PATH_SEPERATOR);
			k = me->lvStatus->AddItem(&fname[k + 1], (void*)fname);
			switch (me->fileAction->GetItem(i))
			{
			case AT_COPY:
				me->lvStatus->SetSubItem(k, 1, (const UTF8Char*)"Copy");
				break;
			case AT_MOVE:
				me->lvStatus->SetSubItem(k, 1, (const UTF8Char*)"Move");
				break;
			case AT_DELETE:
				me->lvStatus->SetSubItem(k, 1, (const UTF8Char*)"Delete");
				break;
			case AT_COPYFAIL:
				me->lvStatus->SetSubItem(k, 1, (const UTF8Char*)"Copy Failed");
				break;
			case AT_MOVEFAIL:
				me->lvStatus->SetSubItem(k, 1, (const UTF8Char*)"Move Failed");
				break;
			case AT_DELETEFAIL:
				me->lvStatus->SetSubItem(k, 1, (const UTF8Char*)"Delete Failed");
				break;
			case AT_SUCCEED:
				me->lvStatus->SetSubItem(k, 1, (const UTF8Char*)"Succeed");
				break;
			case AT_RETRYCOPY:
				me->lvStatus->SetSubItem(k, 1, (const UTF8Char*)"Retry Copy");
				break;
			case AT_RETRYMOVE:
				me->lvStatus->SetSubItem(k, 1, (const UTF8Char*)"Retry Move");
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
	Sync::MutexUsage readMutUsage(me->readMut);
	readPos = me->readLast;
	readCurr = me->readCurr;
	me->readCurr = 0;
	timeDiff = me->readReadTime->DiffMS(me->readLastTimer);
	me->readLastTimer->SetValue(me->readReadTime);
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

	Sync::MutexUsage progMutUsage(me->progMut);
	if (me->progStarted)
	{
		me->progStarted = false;
		me->prgStatus->ProgressStart(me->progName, me->progStartCnt);
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
		Sync::MutexUsage mutUsage(me->statusFileMut);
		hasFile = (me->statusFile != 0);
		if (hasFile)
		{
			me->txtStatusFile->SetText(me->statusFile);
			Text::StrUInt64(sbuff, me->statusFileSize);
			me->txtStatusFileSize->SetText(sbuff);
			fileSize = me->statusFileSize;
		}
		else
		{
			me->txtStatusFile->SetText((const UTF8Char*)"");
			me->txtStatusFileSize->SetText((const UTF8Char*)"");
		}
		mutUsage.EndUse();
	}
	else
	{
		Sync::MutexUsage mutUsage(me->statusFileMut);
		hasFile = (me->statusFile != 0);
		fileSize = me->statusFileSize;
		mutUsage.EndUse();
	}
	if (readPos != me->statusDispSize)
	{
		Text::StrUInt64(sbuff, readPos);
		me->txtStatusCurrSize->SetText(sbuff);
		me->statusDispSize = readPos;
	}
	if (spd != me->statusDispSpd)
	{
		Text::StrDoubleFmt(sbuff, spd, "0.0");
		me->txtStatusCurrSpeed->SetText(sbuff);
		me->statusDispSpd = spd;
	}
	if (hasFile && fileSize != 0 && spd != 0)
	{
		Double t = (Double)(fileSize - readPos) / spd;
		Text::StrDoubleFmt(sbuff, t, "0.0");
		me->txtStatusTimeLeft->SetText(sbuff);
	}
	else
	{
		me->txtStatusTimeLeft->SetText((const UTF8Char*)"?");
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
	Sync::MutexUsage mutUsage(me->fileMut);
	if (me->fileAction->GetItem(index) == AT_COPYFAIL)
	{
		me->fileAction->SetItem(index, AT_RETRYCOPY);
		me->statusChg = true;
	}
	else if (me->fileAction->GetItem(index) == AT_MOVEFAIL)
	{
		me->fileAction->SetItem(index, AT_RETRYMOVE);
		me->statusChg = true;
	}
	mutUsage.EndUse();
}

void SSWR::AVIRead::AVIRPackageForm::DisplayPackFile(IO::PackageFile *packFile)
{
	UTF8Char sbuff[512];
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
		packFile->GetItemName(sbuff, i);
		pot = packFile->GetItemType(i);
		k = this->lvFiles->AddItem(sbuff, (void*)i);
		w = this->lvFiles->GetStringWidth(sbuff);
		if (w > maxWidth)
			maxWidth = w;

		dt.SetTicks(packFile->GetItemModTimeTick(i));
		dt.ToString(sbuff, "yyyy-MM-dd HH:mm:ss");
		this->lvFiles->SetSubItem(k, 3, sbuff);
		if (pot == IO::PackageFile::POT_STREAMDATA)
		{
			this->lvFiles->SetSubItem(k, 1, (const UTF8Char*)"File");
			Text::StrUInt64(sbuff, packFile->GetItemSize(i));
			this->lvFiles->SetSubItem(k, 2, sbuff);
			if (packFile->IsCompressed(i))
			{
				this->lvFiles->SetSubItem(k, 4, Data::Compress::Decompressor::GetCompMethName(packFile->GetItemComp(i)));
			}
			else
			{
				this->lvFiles->SetSubItem(k, 4, (const UTF8Char*)"Uncompressed");
			}
		}
		else if (pot == IO::PackageFile::POT_PACKAGEFILE)
		{
			this->lvFiles->SetSubItem(k, 1, (const UTF8Char*)"Folder");
		}
		else if (pot == IO::PackageFile::POT_PARSEDOBJECT)
		{
			this->lvFiles->SetSubItem(k, 1, (const UTF8Char*)"Object");
		}
		else
		{
			this->lvFiles->SetSubItem(k, 1, (const UTF8Char*)"Unknown");
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
	packFile->GetSourceNameObj()->ConcatTo(Text::StrConcatC(sbuff, UTF8STRC("Package Form - ")));
	this->SetText(sbuff);

	this->core = core;
	this->packFile = packFile;
	this->statusChg = false;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
	NEW_CLASS(this->threadEvt, Sync::Event(true, (const UTF8Char*)"SSWR.AVIRead.AVIRPackageForm.threadEvt"));
	NEW_CLASS(this->fileMut, Sync::Mutex());
	NEW_CLASS(this->fileNames, Data::ArrayList<const UTF8Char *>());
	NEW_CLASS(this->fileAction, Data::ArrayList<ActionType>());
	this->threadRunning = false;
	this->threadToStop = false;
	NEW_CLASS(this->readMut, Sync::Mutex());
	NEW_CLASS(this->readLastTimer, Data::DateTime());
	NEW_CLASS(this->readReadTime, Data::DateTime());
	this->readLastTimer->SetCurrTimeUTC();
	this->readReadTime->SetValue(this->readLastTimer);
	this->readTotal = 0;
	this->readCurr = 0;
	this->readLast = 0;
	this->readCurrFile = 0;
	this->readFileCnt = 0;
	NEW_CLASS(this->statusFileMut, Sync::Mutex());
	this->statusFileChg = false;
	this->statusFile = 0;
	this->statusFileSize = 0;
	this->statusDispSize = (UOSInt)-1;
	this->statusDispSpd = -1;
	this->statusBNT = IO::ActiveStreamReader::BNT_UNKNOWN;
	this->statusDispBNT = IO::ActiveStreamReader::BNT_UNKNOWN;
	NEW_CLASS(this->progMut, Sync::Mutex());
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


	this->tpFiles = this->tcMain->AddTabPage((const UTF8Char*)"Files");
	NEW_CLASS(this->lvFiles, UI::GUIListView(ui, this->tpFiles, UI::GUIListView::LVSTYLE_TABLE, 5));
	this->lvFiles->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lvFiles->AddColumn((const UTF8Char*)"Item Name", 150);
	this->lvFiles->AddColumn((const UTF8Char*)"Type", 80);
	this->lvFiles->AddColumn((const UTF8Char*)"Size", 100);
	this->lvFiles->AddColumn((const UTF8Char*)"Modify Time", 150);
	this->lvFiles->AddColumn((const UTF8Char*)"Compression", 100);

	this->lvFiles->HandleDblClk(LVDblClick, this);
	DisplayPackFile(this->packFile);

	UI::GUIMainMenu *mnuMain;
	UI::GUIMenu *mnu;
//	UI::GUIMenu *mnu2;
	UOSInt ind;
	NEW_CLASS(mnuMain, UI::GUIMainMenu());
	mnu = mnuMain->AddSubMenu((const UTF8Char*)"&File");
	mnu->AddItem((const UTF8Char*)"Save &As...", MNU_SAVEAS, UI::GUIMenu::KM_CONTROL, UI::GUIControl::GK_S);
	mnu = mnuMain->AddSubMenu((const UTF8Char*)"&Edit");
	ind = mnu->AddItem((const UTF8Char*)"&Paste", MNU_PASTE, UI::GUIMenu::KM_CONTROL, UI::GUIControl::GK_V);
	mnu->AddItem((const UTF8Char*)"Copy To...", MNU_COPYTO, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu->AddItem((const UTF8Char*)"Copy All To...", MNU_COPYALLTO, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);

	NEW_CLASS(this->mnuPopup, UI::GUIPopupMenu());
	this->mnuPopup->AddItem((const UTF8Char*)"Copy To...", MNU_COPYTO, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);

	if (!packFile->AllowWrite())
	{
		mnu->SetItemEnabled((UInt16)ind, false);
	}
	else
	{
		this->tpStatus = this->tcMain->AddTabPage((const UTF8Char*)"Status");
		NEW_CLASS(this->pnlStatus, UI::GUIPanel(ui, this->tpStatus));
		this->pnlStatus->SetRect(0, 0, 100, 48, false);
		this->pnlStatus->SetDockType(UI::GUIControl::DOCK_TOP);
		NEW_CLASS(this->lblStatusFile, UI::GUILabel(ui, this->pnlStatus, (const UTF8Char*)"Copy From"));
		this->lblStatusFile->SetRect(0, 0, 100, 23, false);
		NEW_CLASS(this->txtStatusFile, UI::GUITextBox(ui, this->pnlStatus, (const UTF8Char*)""));
		this->txtStatusFile->SetRect(100, 0, 800, 23, false);
		this->txtStatusFile->SetReadOnly(true);
		NEW_CLASS(this->pnlStatusBNT, UI::GUIPanel(ui, this->pnlStatus));
		this->pnlStatusBNT->SetBGColor(0xffc0c0c0);
		this->pnlStatusBNT->SetRect(900, 0, 23, 23, false);
		NEW_CLASS(this->lblStatusFileSize, UI::GUILabel(ui, this->pnlStatus, (const UTF8Char*)"File Size"));
		this->lblStatusFileSize->SetRect(0, 24, 100, 23, false);
		NEW_CLASS(this->txtStatusFileSize, UI::GUITextBox(ui, this->pnlStatus, (const UTF8Char*)""));
		this->txtStatusFileSize->SetRect(100, 24, 100, 23, false);
		this->txtStatusFileSize->SetReadOnly(true);
		NEW_CLASS(this->lblStatusCurrSize, UI::GUILabel(ui, this->pnlStatus, (const UTF8Char*)"Curr Size"));
		this->lblStatusCurrSize->SetRect(220, 24, 100, 23, false);
		NEW_CLASS(this->txtStatusCurrSize, UI::GUITextBox(ui, this->pnlStatus, (const UTF8Char*)""));
		this->txtStatusCurrSize->SetRect(320, 24, 100, 23, false);
		this->txtStatusCurrSize->SetReadOnly(true);
		NEW_CLASS(this->lblStatusCurrSpeed, UI::GUILabel(ui, this->pnlStatus, (const UTF8Char*)"Curr Speed"));
		this->lblStatusCurrSpeed->SetRect(440, 24, 100, 23, false);
		NEW_CLASS(this->txtStatusCurrSpeed, UI::GUITextBox(ui, this->pnlStatus, (const UTF8Char*)""));
		this->txtStatusCurrSpeed->SetRect(540, 24, 100, 23, false);
		this->txtStatusCurrSpeed->SetReadOnly(true);
		NEW_CLASS(this->lblStatusTimeLeft, UI::GUILabel(ui, this->pnlStatus, (const UTF8Char*)"Time Left"));
		this->lblStatusTimeLeft->SetRect(660, 24, 100, 23, false);
		NEW_CLASS(this->txtStatusTimeLeft, UI::GUITextBox(ui, this->pnlStatus, (const UTF8Char*)""));
		this->txtStatusTimeLeft->SetRect(760, 24, 100, 23, false);
		this->txtStatusTimeLeft->SetReadOnly(true);
		NEW_CLASS(this->rlcStatus, UI::GUIRealtimeLineChart(ui, this->tpStatus, this->core->GetDrawEngine(), 1, 720, 1000));
		this->rlcStatus->SetRect(0, 0, 100, 360, false);
		this->rlcStatus->SetDockType(UI::GUIControl::DOCK_BOTTOM);
		NEW_CLASS(this->vspStatus, UI::GUIVSplitter(ui, this->tpStatus, 3, true));
		NEW_CLASS(this->lvStatus, UI::GUIListView(ui, this->tpStatus, UI::GUIListView::LVSTYLE_TABLE, 2));
		this->lvStatus->SetDockType(UI::GUIControl::DOCK_FILL);
		this->lvStatus->SetFullRowSelect(true);
		this->lvStatus->AddColumn((const UTF8Char*)"Source File", 200);
		this->lvStatus->AddColumn((const UTF8Char*)"Status", 200);
		this->lvStatus->HandleDblClk(OnStatusDblClick, this);

		this->AddTimer(500, OnTimerTick, this);
		Sync::Thread::Create(ProcessThread, this);
	}

	this->tpInfo = this->tcMain->AddTabPage((const UTF8Char*)"Info");
	NEW_CLASS(this->txtInfo, UI::GUITextBox(ui, this->tpInfo, (const UTF8Char*)"", true));
	this->txtInfo->SetReadOnly(true);
	this->txtInfo->SetDockType(UI::GUIControl::DOCK_FILL);
	Text::StringBuilderUTF8 sb;
	this->packFile->GetInfoText(&sb);
	this->txtInfo->SetText(sb.ToString());

	this->SetMenu(mnuMain);
}

SSWR::AVIRead::AVIRPackageForm::~AVIRPackageForm()
{
	UOSInt i;
	this->threadToStop = true;
	this->threadEvt->Set();
	while (this->threadRunning)
	{
		Sync::Thread::Sleep(10);
	}
	DEL_CLASS(this->packFile);
	i = this->fileNames->GetCount();
	while (i-- > 0)
	{
		Text::StrDelNew(this->fileNames->GetItem(i));
	}
	DEL_CLASS(this->fileMut);
	DEL_CLASS(this->fileAction);
	DEL_CLASS(this->fileNames);
	DEL_CLASS(this->readMut);
	DEL_CLASS(this->readLastTimer);
	DEL_CLASS(this->readReadTime);
	DEL_CLASS(this->threadEvt);
	DEL_CLASS(this->statusFileMut);
	SDEL_TEXT(this->statusFile);
	DEL_CLASS(this->mnuPopup);
	DEL_CLASS(this->progMut);
	SDEL_TEXT(this->progName);
}

void SSWR::AVIRead::AVIRPackageForm::EventMenuClicked(UInt16 cmdId)
{
	switch (cmdId)
	{
	case MNU_PASTE:
		{
			Win32::Clipboard clipboard(this->hwnd);
			Data::ArrayList<const UTF8Char *> fileNames;
			Win32::Clipboard::FilePasteType fpt;
			UOSInt i;
			UOSInt j;
			fpt = clipboard.GetDataFiles(&fileNames);
			if (fpt == Win32::Clipboard::FPT_MOVE)
			{
				Sync::MutexUsage mutUsage(this->fileMut);
				i = 0;
				j = fileNames.GetCount();
				while (i < j)
				{
					this->fileNames->Add(Text::StrCopyNew(fileNames.GetItem(i)));
					this->fileAction->Add(AT_MOVE);
					i++;
				}
				this->statusChg = true;
				mutUsage.EndUse();
				clipboard.FreeDataFiles(&fileNames);
			}
			else if (fpt == Win32::Clipboard::FPT_COPY)
			{
				Sync::MutexUsage mutUsage(this->fileMut);
				i = 0;
				j = fileNames.GetCount();
				while (i < j)
				{
					this->fileNames->Add(Text::StrCopyNew(fileNames.GetItem(i)));
					this->fileAction->Add(AT_COPY);
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
				UI::FolderDialog *dlg;
				NEW_CLASS(dlg, UI::FolderDialog(L"SSWR", L"AVIRead", L"PackageCopyTo"));
				if (dlg->ShowDialog(this->GetHandle()) == UI::GUIForm::DR_OK)
				{
					UOSInt j;
					j = 0;
					while (j < i)
					{
						if (!packFile->CopyTo(selIndices.GetItem(j), dlg->GetFolder(), false))
						{
							UI::MessageDialog::ShowDialog((const UTF8Char*)"Error in copying", (const UTF8Char*)"Copy To", this);
							break;
						}
						j++;
					}
				}
				DEL_CLASS(dlg);
			}
		}
		break;
	case MNU_COPYALLTO:
		if (packFile->GetCount() > 0)
		{
			UI::FolderDialog *dlg;
			NEW_CLASS(dlg, UI::FolderDialog(L"SSWR", L"AVIRead", L"PackageCopyAllTo"));
			if (dlg->ShowDialog(this->GetHandle()) == UI::GUIForm::DR_OK)
			{
				UOSInt i = 0;
				UOSInt j = packFile->GetCount();
				while (i < j)
				{
					if (!packFile->CopyTo(i, dlg->GetFolder(), false))
					{
						UI::MessageDialog::ShowDialog((const UTF8Char*)"Error in copying", (const UTF8Char*)"Copy To", this);
						break;
					}
					i++;
				}
			}
			DEL_CLASS(dlg);
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

void SSWR::AVIRead::AVIRPackageForm::ProgressStart(const UTF8Char *name, UInt64 count)
{
	{
		Sync::MutexUsage mutUsage(this->readMut);
		this->readLast = 0;
		this->readCurrFile = name;
		this->readFileCnt = count;
		mutUsage.EndUse();
	}

	{
		Sync::MutexUsage mutUsage(this->statusFileMut);
		SDEL_TEXT(this->statusFile);
		this->statusFile = Text::StrCopyNew(name);
		this->statusFileSize = count;
		this->statusFileChg = true;
		mutUsage.EndUse();
	}

	{
		Sync::MutexUsage mutUsage(this->progMut);
		this->progStarted = true;
		SDEL_TEXT(this->progName);
		this->progName = Text::StrCopyNew(name);
		this->progStartCnt = count;
		this->progEnd = false;
		mutUsage.EndUse();
	}
}

void SSWR::AVIRead::AVIRPackageForm::ProgressUpdate(UInt64 currCount, UInt64 newCount)
{
	{
		Sync::MutexUsage mutUsage(this->readMut);
		UInt64 readThis = currCount - this->readLast;
		if ((Int64)readThis < 0)
			readThis = 0;
		this->readCurr += readThis;
		this->readTotal += readThis;
		this->readLast = currCount;
		this->readReadTime->SetCurrTimeUTC();
		mutUsage.EndUse();
	}

	{
		Sync::MutexUsage mutUsage(this->progMut);
		this->progUpdated = true;
		this->progUpdateCurr = currCount;
		this->progUpdateNew = newCount;
		this->progEnd = false;
		mutUsage.EndUse();
	}
}

void SSWR::AVIRead::AVIRPackageForm::ProgressEnd()
{
	this->readLast = 0;
	this->readFileCnt = 0;
	Sync::MutexUsage mutUsage(this->progMut);
	this->progEnd = true;
	mutUsage.EndUse();

	{
		Sync::MutexUsage mutUsage(this->statusFileMut);
		SDEL_TEXT(this->statusFile);
		this->statusFileChg = true;
		mutUsage.EndUse();
	}
}
