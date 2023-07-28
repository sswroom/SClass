#include "Stdafx.h"
#include "IO/Path.h"
#include "IO/StmData/FileData.h"
#include "Manage/HiResClock.h"
#include "Media/IVideoSource.h"
#include "Media/MediaFile.h"
#include "Media/Decoder/AudioDecoderFinder.h"
#include "Media/Decoder/VideoDecoderFinder.h"
#include "SSWR/AVIRead/AVIRVideoCheckerForm.h"
#include "Sync/MutexUsage.h"
#include "Sync/SimpleThread.h"
#include "Sync/ThreadUtil.h"
#include "Text/MyString.h"
#include "Text/MyStringFloat.h"

void __stdcall SSWR::AVIRead::AVIRVideoCheckerForm::OnFileHandler(void *userObj, NotNullPtr<Text::String> *files, UOSInt nFiles)
{
	SSWR::AVIRead::AVIRVideoCheckerForm *me = (SSWR::AVIRead::AVIRVideoCheckerForm*)userObj;
	UOSInt i = 0;
	UOSInt j;
	FileQueue *file;
	while (i < nFiles)
	{
		j = files[i]->LastIndexOf(IO::Path::PATH_SEPERATOR);
		Text::CString name = files[i]->ToCString().Substring(j + 1);
		j = me->lvFiles->AddItem(name, 0);

		file = MemAlloc(FileQueue, 1);
		file->fileName = files[i]->Clone();
		file->index = j;
		Sync::MutexUsage mutUsage(me->fileMut);
		me->fileList.Add(file);
		mutUsage.EndUse();
		i++;
	}
}

void __stdcall SSWR::AVIRead::AVIRVideoCheckerForm::OnCancelClicked(void *userObj)
{
	SSWR::AVIRead::AVIRVideoCheckerForm *me = (SSWR::AVIRead::AVIRVideoCheckerForm*)userObj;
	me->CancelQueues();
}

void __stdcall SSWR::AVIRead::AVIRVideoCheckerForm::OnAllowTimeSkipChange(void *userObj, Bool newVal)
{
	SSWR::AVIRead::AVIRVideoCheckerForm *me = (SSWR::AVIRead::AVIRVideoCheckerForm*)userObj;
	me->checker.SetAllowTimeSkip(newVal);
}

void __stdcall SSWR::AVIRead::AVIRVideoCheckerForm::OnTimerTick(void *userObj)
{
	SSWR::AVIRead::AVIRVideoCheckerForm *me = (SSWR::AVIRead::AVIRVideoCheckerForm*)userObj;
	while (me->updateList.GetCount() > 0)
	{
		UpdateQueue *update;
		Sync::MutexUsage mutUsage(me->updateMut);
		update = me->updateList.RemoveAt(me->updateList.GetCount() - 1);
		mutUsage.EndUse();
		if (update->status == 0)
		{
			Text::StringBuilderUTF8 sb;
			sb.AppendC(UTF8STRC("Decode Ok, time used = "));
			Text::SBAppendF64(&sb, update->t);
			me->lvFiles->SetSubItem(update->index, 1, sb.ToCString());
		}
		else if (update->status == 1)
		{
			me->lvFiles->SetSubItem(update->index, 1, CSTR("Not valid"));
		}
		else if (update->status == 2)
		{
			me->lvFiles->SetSubItem(update->index, 1, CSTR("Not video file"));
		}
		else
		{
			me->lvFiles->SetSubItem(update->index, 1, CSTR("Unknown"));
		}
		MemFree(update);
	}
}

UInt32 __stdcall SSWR::AVIRead::AVIRVideoCheckerForm::ProcessThread(void *userObj)
{
	SSWR::AVIRead::AVIRVideoCheckerForm *me = (SSWR::AVIRead::AVIRVideoCheckerForm*)userObj;
	FileQueue *file;
	UpdateQueue *update;
	Media::MediaFile *mediaFile;
	me->threadRunning = true;
	{
		Manage::HiResClock clk;
		while (!me->threadToStop)
		{
			Sync::MutexUsage mutUsage(me->fileMut);
			file = me->fileList.RemoveAt(0);
			mutUsage.EndUse();
			if (file)
			{
				{
					IO::StmData::FileData fd(file->fileName, false);
					mediaFile = (Media::MediaFile*)me->core->GetParserList()->ParseFileType(fd, IO::ParserType::MediaFile);
				}

				update = MemAlloc(UpdateQueue, 1);
				update->index = file->index;
				update->t = 0;
				update->status = -1;
				if (mediaFile)
				{
					clk.Start();
					if (me->checker.IsValid(mediaFile))
					{
						update->status = 0;
					}
					else
					{
						update->status = 1;
					}
					update->t = clk.GetTimeDiff();
					DEL_CLASS(mediaFile);
				}
				else
				{
					update->status = 2;
				}

				file->fileName->Release();
				MemFree(file);

				Sync::MutexUsage mutUsage(me->updateMut);
				me->updateList.Add(update);
				mutUsage.EndUse();
			}
			else
			{
				me->threadEvt.Wait(1000);
			}
		}
	}
	me->threadRunning = false;
	return 0;
}

void SSWR::AVIRead::AVIRVideoCheckerForm::CancelQueues()
{
	UOSInt i;
	FileQueue *file;
	Sync::MutexUsage mutUsage(this->fileMut);
	i = this->fileList.GetCount();
	while (i-- > 0)
	{
		file = this->fileList.RemoveAt(i);
		this->lvFiles->SetSubItem(file->index, 1, CSTR("Cancelled"));

		file->fileName->Release();
		MemFree(file);
	}
}

SSWR::AVIRead::AVIRVideoCheckerForm::AVIRVideoCheckerForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, SSWR::AVIRead::AVIRCore *core) : UI::GUIForm(parent, 640, 480, ui), checker(false)
{
	this->SetFont(0, 0, 8.25, false);
	this->SetText(CSTR("Video Checker"));
	
	this->core = core;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	NEW_CLASS(this->pnlCtrl, UI::GUIPanel(ui, this));
	this->pnlCtrl->SetRect(0, 0, 100, 31, false);
	this->pnlCtrl->SetDockType(UI::GUIControl::DOCK_TOP);
	NEW_CLASS(this->btnCancel, UI::GUIButton(ui, this->pnlCtrl, CSTR("Cancel")));
	this->btnCancel->SetRect(4, 4, 75, 23, false);
	this->btnCancel->HandleButtonClick(OnCancelClicked, this);
	NEW_CLASS(this->chkAllowTimeSkip, UI::GUICheckBox(ui, this->pnlCtrl, CSTR("Allow Time Skip"), false));
	this->chkAllowTimeSkip->SetRect(84, 4, 120, 23, false);
	this->chkAllowTimeSkip->HandleCheckedChange(OnAllowTimeSkipChange, this);
	NEW_CLASS(this->lvFiles, UI::GUIListView(ui, this, UI::GUIListView::LVSTYLE_TABLE, 2));
	this->lvFiles->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lvFiles->AddColumn(CSTR("File Name"), 400);
	this->lvFiles->AddColumn(CSTR("Status"), 300);
	this->lvFiles->SetFullRowSelect(true);
	this->lvFiles->SetShowGrid(true);

	this->HandleDropFiles(OnFileHandler, this);

	this->threadRunning = false;
	this->threadToStop = false;

	Sync::ThreadUtil::Create(ProcessThread, this);
	this->AddTimer(1000, OnTimerTick, this);
}

SSWR::AVIRead::AVIRVideoCheckerForm::~AVIRVideoCheckerForm()
{
	this->threadToStop = true;
	this->threadEvt.Set();
	this->CancelQueues();
	while (this->threadRunning)
	{
		Sync::SimpleThread::Sleep(1);
	}
	UOSInt i = this->updateList.GetCount();
	UpdateQueue *update;
	while (i-- > 0)
	{
		update = this->updateList.RemoveAt(i);
		MemFree(update);
	}
}

void SSWR::AVIRead::AVIRVideoCheckerForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
