#include "Stdafx.h"
#include "Exporter/MD4Exporter.h"
#include "Exporter/MD5Exporter.h"
#include "Exporter/SFVExporter.h"
#include "Exporter/SHA1Exporter.h"
#include "IO/FileStream.h"
#include "IO/Path.h"
#include "SSWR/AVIRead/AVIRFileHashForm.h"
#include "Sync/MutexUsage.h"
#include "Sync/SimpleThread.h"
#include "Sync/ThreadUtil.h"

void __stdcall SSWR::AVIRead::AVIRFileHashForm::OnFileDrop(void *userObj, Text::String **files, UOSInt nFiles)
{
	SSWR::AVIRead::AVIRFileHashForm *me = (SSWR::AVIRead::AVIRFileHashForm *)userObj;
	UOSInt i;
	i = 0;
	while (i < nFiles)
	{
		me->AddFile(files[i]->ToCString());
		i++;
	}
	me->fileListChg = true;
}

void __stdcall SSWR::AVIRead::AVIRFileHashForm::OnTimerTick(void *userObj)
{
	SSWR::AVIRead::AVIRFileHashForm *me = (SSWR::AVIRead::AVIRFileHashForm *)userObj;
	UTF8Char sbuff[32];
	UTF8Char *sptr;
	me->UpdateUI();
	Data::DateTime currTime;
	currTime.SetCurrTimeUTC();
	Sync::MutexUsage mutUsage(&me->readMut);
	if (me->progNameChg)
	{
		me->txtFileName->SetText(me->progName->ToCString());
		me->progNameChg = false;
	}
	if (me->progLastCount != me->progCount)
	{
		me->prgFile->ProgressStart(me->progName->ToCString(), me->progCount);
		me->progLastCount = me->progCount;
	}

	UInt64 currRead = me->readSize;
	UInt64 currTotal = me->totalRead;
	me->readSize = 0;
	me->prgFile->ProgressUpdate(me->progCurr, me->progCount);
	mutUsage.EndUse();

	Int64 timeDiff = currTime.DiffMS(&me->lastTimerTime);
	Double spd;
	if (timeDiff > 0)
	{
		sptr = Text::StrUInt64(sbuff, currRead * 1000 / (UInt64)timeDiff);
		spd = (Double)currRead * 1000.0 / (Double)timeDiff;
	}
	else
	{
		sptr = Text::StrUInt64(sbuff, currRead);
		spd = (Double)currRead;
	}
	me->txtSpeed->SetText(CSTRP(sbuff, sptr));
	me->rlcSpeed->AddSample(&spd);
	sptr = Text::StrUInt64(sbuff, currTotal);
	me->txtTotalSize->SetText(CSTRP(sbuff, sptr));
	me->lastTimerTime.SetValue(&currTime);
}

void __stdcall SSWR::AVIRead::AVIRFileHashForm::OnCheckTypeChg(void *userObj)
{
	SSWR::AVIRead::AVIRFileHashForm *me = (SSWR::AVIRead::AVIRFileHashForm*)userObj;
	me->currHashType = (Crypto::Hash::HashType)(OSInt)me->cboCheckType->GetSelectedItem();
}

UInt32 __stdcall SSWR::AVIRead::AVIRFileHashForm::HashThread(void *userObj)
{
	SSWR::AVIRead::AVIRFileHashForm *me = (SSWR::AVIRead::AVIRFileHashForm*)userObj;
	Bool found;
	FileStatus *status = 0;
	UTF8Char sbuff[512];
	UTF8Char *sptr;
	UOSInt i;
	UOSInt j;
	IO::FileExporter *exporter;
	Crypto::Hash::HashType chkType;
	me->threadStatus = 1;
	while (!me->threadToStop)
	{
		Sync::MutexUsage mutUsage(&me->fileMut);
		found = false;
		i = 0;
		j = me->fileList.GetCount();
		while (i < j)
		{
			status = me->fileList.GetItem(i);
			if (status->status == 0)
			{
				found = true;
				status->status = 1;
				me->fileListChg = true;
				break;
			}
			i++;
		}
		mutUsage.EndUse();
		if (found)
		{
			chkType = me->currHashType;
			if (chkType == Crypto::Hash::HashType::MD5)
			{
				sptr = Text::StrConcatC(status->fileName->ConcatTo(sbuff), UTF8STRC(".md5"));
			}
			else if (chkType == Crypto::Hash::HashType::CRC32)
			{
				sptr = Text::StrConcatC(status->fileName->ConcatTo(sbuff), UTF8STRC(".sfv"));
			}
			else if (chkType == Crypto::Hash::HashType::SHA1)
			{
				sptr = Text::StrConcatC(status->fileName->ConcatTo(sbuff), UTF8STRC(".sha1"));
			}
			else if (chkType == Crypto::Hash::HashType::MD4)
			{
				sptr = Text::StrConcatC(status->fileName->ConcatTo(sbuff), UTF8STRC(".md4"));
			}
			else
			{
				*sbuff = 0;
				sptr = sbuff;
			}
			if (IO::Path::GetPathType(CSTRP(sbuff, sptr)) == IO::Path::PathType::Unknown)
			{
				IO::FileCheck *fchk = IO::FileCheck::CreateCheck(status->fileName->ToCString(), chkType, me, false);
				if (fchk)
				{
					if (chkType == Crypto::Hash::HashType::CRC32)
					{
						NEW_CLASS(exporter, Exporter::SFVExporter());
					}
					else if (chkType == Crypto::Hash::HashType::MD4)
					{
						NEW_CLASS(exporter, Exporter::MD4Exporter());
					}
					else if (chkType == Crypto::Hash::HashType::MD5)
					{
						NEW_CLASS(exporter, Exporter::MD5Exporter());
					}
					else if (chkType == Crypto::Hash::HashType::SHA1)
					{
						NEW_CLASS(exporter, Exporter::SHA1Exporter());
					}
					else
					{
						exporter = 0;
					}
					if (exporter)
					{
						exporter->SetCodePage(me->core->GetCurrCodePage());
						{
							IO::FileStream fs(CSTRP(sbuff, sptr), IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
							exporter->ExportFile(&fs, CSTRP(sbuff, sptr), fchk, 0);
						}
						DEL_CLASS(exporter);
					}
					status->status = 2;
					status->fchk = fchk;
				}
				else
				{
					status->status = 3;
				}
			}
			else
			{
				status->status = 3;
			}
			me->fileListChg = true;
		}
		else
		{
			me->fileEvt.Wait(1000);
		}
	}
	me->threadStatus = 2;
	return 0;
}

void SSWR::AVIRead::AVIRFileHashForm::AddFile(Text::CString fileName)
{
	FileStatus *status;
	Sync::MutexUsage mutUsage(&this->fileMut);
	status = MemAlloc(FileStatus, 1);
	status->fchk = 0;
	status->status = 0;
	status->fileName = Text::String::New(fileName);
	this->fileList.Add(status);
	mutUsage.EndUse();
	this->fileEvt.Set();
}

void SSWR::AVIRead::AVIRFileHashForm::UpdateUI()
{
	UInt8 hashBuff[64];
	UTF8Char sbuff[512];
	UTF8Char *sptr;
	if (this->fileListChg)
	{
		UOSInt i;
		UOSInt j;
		UOSInt k;
		UOSInt l;
		UOSInt m;
		FileStatus *status;
		this->fileListChg = false;
		this->lvFiles->ClearItems();
		this->lvTasks->ClearItems();
		Sync::MutexUsage mutUsage(&this->fileMut);
		i = 0;
		j = this->fileList.GetCount();
		while (i < j)
		{
			status = this->fileList.GetItem(i);
			k = this->lvTasks->AddItem(status->fileName, status);
			if (status->status == 0)
			{
				this->lvTasks->SetSubItem(k, 1, CSTR("Queued"));
			}
			else if (status->status == 1)
			{
				this->lvTasks->SetSubItem(k, 1, CSTR("Processing"));
			}
			else if (status->status == 2)
			{
				this->lvTasks->SetSubItem(k, 1, CSTR("Finished"));
				if (status->fchk)
				{
					k = 0;
					l = status->fchk->GetCount();
					while (k < l)
					{
						m = this->lvFiles->AddItem(status->fchk->GetEntryName(k), 0);
						status->fchk->GetEntryHash(k, hashBuff);
						sptr = Text::StrHexBytes(sbuff, hashBuff, status->fchk->GetHashSize(), 0);
						this->lvFiles->SetSubItem(m, 1, CSTRP(sbuff, sptr));
						k++;
					}
				}
			}
			else if (status->status == 3)
			{
				this->lvTasks->SetSubItem(k, 1, CSTR("Error"));
			}
			else
			{
				this->lvTasks->SetSubItem(k, 1, CSTR("Unknown"));
			}
			i++;
		}
	}
}

SSWR::AVIRead::AVIRFileHashForm::AVIRFileHashForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core) : UI::GUIForm(parent, 1024, 768, ui)
{
	this->SetFont(0, 0, 8.25, false);
	this->SetText(CSTR("File Hash"));

	this->core = core;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
	this->progName = 0;
	this->progNameChg = false;
	this->progLastCount = 0;
	this->progCurr = 0;
	this->progCount = 0;
	this->threadStatus = 0;
	this->threadToStop = false;
	this->currHashType = Crypto::Hash::HashType::MD5;
	this->readSize = 0;
	this->totalRead = 0;
	this->fileListChg = false;

	NEW_CLASS(this->pnlCheckType, UI::GUIPanel(ui, this));
	this->pnlCheckType->SetRect(0, 0, 100, 31, false);
	this->pnlCheckType->SetDockType(UI::GUIControl::DOCK_TOP);
	NEW_CLASS(this->lblCheckType, UI::GUILabel(ui, this->pnlCheckType, CSTR("Hash Type")));
	this->lblCheckType->SetRect(4, 4, 100, 23, false);
	NEW_CLASS(this->cboCheckType, UI::GUIComboBox(ui, this->pnlCheckType, false));
	this->cboCheckType->SetRect(104, 4, 100, 23, false);
	Crypto::Hash::HashType currType = Crypto::Hash::HashType::First;
	Crypto::Hash::HashType lastType = Crypto::Hash::HashType::Last;
	while (currType <= lastType)
	{
		UOSInt i = this->cboCheckType->AddItem(Crypto::Hash::HashTypeGetName(currType), (void*)(OSInt)currType);
		if (currType == this->currHashType)
		{
			this->cboCheckType->SetSelectedIndex(i);
		}
		currType = (Crypto::Hash::HashType)((OSInt)currType + 1);
	}
	this->cboCheckType->HandleSelectionChange(OnCheckTypeChg, this);
	NEW_CLASS(this->tcMain, UI::GUITabControl(ui, this));
	this->tcMain->SetDockType(UI::GUIControl::DOCK_FILL);
	this->tpStatus = this->tcMain->AddTabPage(CSTR("Status"));
	this->tpTasks = this->tcMain->AddTabPage(CSTR("Tasks"));
	this->tpFiles = this->tcMain->AddTabPage(CSTR("Files"));

	NEW_CLASS(this->pnlStatus, UI::GUIPanel(ui, this->tpStatus));
	this->pnlStatus->SetRect(0, 0, 100, 72, false);
	this->pnlStatus->SetDockType(UI::GUIControl::DOCK_TOP);
	NEW_CLASS(this->pnlStatusFile, UI::GUIPanel(ui, this->pnlStatus));
	this->pnlStatusFile->SetRect(0, 0, 100, 24, false);
	this->pnlStatusFile->SetDockType(UI::GUIControl::DOCK_TOP);
	NEW_CLASS(this->lblFile, UI::GUILabel(ui, this->pnlStatusFile, CSTR("File")));
	this->lblFile->SetRect(0, 0, 56, 24, false);
	this->lblFile->SetDockType(UI::GUIControl::DOCK_LEFT);
	NEW_CLASS(this->txtFileName, UI::GUITextBox(ui, this->pnlStatusFile, CSTR("")));
	this->txtFileName->SetReadOnly(true);
	this->txtFileName->SetDockType(UI::GUIControl::DOCK_FILL);
	NEW_CLASS(this->lblSpeed, UI::GUILabel(ui, this->pnlStatus, CSTR("Speed")));
	this->lblSpeed->SetRect(0, 24, 48, 23, false);
	NEW_CLASS(this->txtSpeed, UI::GUITextBox(ui, this->pnlStatus, CSTR("")));
	this->txtSpeed->SetReadOnly(true);
	this->txtSpeed->SetRect(56, 24, 100, 23, false);
	NEW_CLASS(this->lblTotalSize, UI::GUILabel(ui, this->pnlStatus, CSTR("Total Size")));
	this->lblTotalSize->SetRect(184, 24, 80, 23, false);
	NEW_CLASS(this->txtTotalSize, UI::GUITextBox(ui, this->pnlStatus, CSTR("")));
	this->txtTotalSize->SetRect(272, 24, 100, 23, false);
	this->txtTotalSize->SetReadOnly(true);
	NEW_CLASS(this->prgFile, UI::GUIProgressBar(ui, this->pnlStatus, 0));
	this->prgFile->SetRect(0, 0, 100, 24, false);
	this->prgFile->SetDockType(UI::GUIControl::DOCK_BOTTOM);
 	NEW_CLASS(this->rlcSpeed, UI::GUIRealtimeLineChart(ui, this->tpStatus, this->core->GetDrawEngine(), 1, 350, 500));
	this->rlcSpeed->SetDockType(UI::GUIControl::DOCK_FILL);

	NEW_CLASS(this->pnlTasks, UI::GUIPanel(ui, this->tpTasks));
	this->pnlTasks->SetRect(0, 0, 100, 32, false);
	this->pnlTasks->SetDockType(UI::GUIControl::DOCK_BOTTOM);
	NEW_CLASS(this->btnTasksClear, UI::GUIButton(ui, this->pnlTasks, CSTR("Clear Completed")));
	this->btnTasksClear->SetRect(4, 4, 120, 23, false);
	NEW_CLASS(this->lvTasks, UI::GUIListView(ui, this->tpTasks, UI::GUIListView::LVSTYLE_TABLE, 2));
	this->lvTasks->AddColumn(CSTR("File Name"), 500);
	this->lvTasks->AddColumn(CSTR("Status"), 100);
	this->lvTasks->SetShowGrid(true);
	this->lvTasks->SetFullRowSelect(true);
	this->lvTasks->SetDockType(UI::GUIControl::DOCK_FILL);

	NEW_CLASS(this->lvFiles, UI::GUIListView(ui, this->tpFiles, UI::GUIListView::LVSTYLE_TABLE, 2));
	this->lvFiles->AddColumn(CSTR("File Name"), 500);
	this->lvFiles->AddColumn(CSTR("Hash"), 200);
	this->lvFiles->SetShowGrid(true);
	this->lvFiles->SetFullRowSelect(true);
	this->lvFiles->SetDockType(UI::GUIControl::DOCK_FILL);

	this->HandleDropFiles(OnFileDrop, this);
	this->AddTimer(500, OnTimerTick, this);
	Sync::ThreadUtil::Create(HashThread, this);
}

SSWR::AVIRead::AVIRFileHashForm::~AVIRFileHashForm()
{
	UOSInt i;
	FileStatus *status;
	this->threadToStop = true;
	this->fileEvt.Set();
	while (this->threadStatus != 2)
	{
		Sync::SimpleThread::Sleep(10);
	}

	i = this->fileList.GetCount();
	while (i-- > 0)
	{
		status = this->fileList.GetItem(i);
		status->fileName->Release();
		SDEL_CLASS(status->fchk);
		MemFree(status);
	}
	SDEL_STRING(this->progName);
}

void SSWR::AVIRead::AVIRFileHashForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}

void SSWR::AVIRead::AVIRFileHashForm::ProgressStart(Text::CString name, UInt64 count)
{
	Sync::MutexUsage mutUsage(&this->readMut);
	if (this->progName)
	{
		this->progName->Release();
	}
	this->progName = Text::String::New(name);
	this->progNameChg = true;
	this->readSize += this->progCount - this->progCurr;
	this->totalRead += this->progCount - this->progCurr;
	this->progCount = count;
	this->progCurr = 0;
}

void SSWR::AVIRead::AVIRFileHashForm::ProgressUpdate(UInt64 currCount, UInt64 newCount)
{
	Sync::MutexUsage mutUsage(&this->readMut);
	this->readSize += currCount - this->progCurr;
	this->totalRead += currCount - this->progCurr;
	this->progCurr = currCount;
}

void SSWR::AVIRead::AVIRFileHashForm::ProgressEnd()
{
}
