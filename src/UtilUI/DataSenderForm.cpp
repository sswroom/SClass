#include "stdafx.h"
#include "Text/MyString.h"
#include "IO/Path.h"
#include "UI/MSWindowLabel.h"
#include "UI/MessageDialog.h"
#include "UtilUI/DataSenderForm.h"

void __stdcall UtilUI::DataSenderForm::FileHandler(void *userObj, const WChar **files, OSInt nFiles)
{
	UtilUI::DataSenderForm *me = (UtilUI::DataSenderForm*)userObj;
	OSInt i = 0;
	while (i < nFiles)
	{
		me->AddFile(files[i]);
		i++;
	}
}

void __stdcall UtilUI::DataSenderForm::OnStartClicked(void *userObj)
{
	WChar sbuff[12];
	UtilUI::DataSenderForm *me = (UtilUI::DataSenderForm*)userObj;
	if (me->svr)
	{
		me->svr->Close();
		me->reader->SetServer(0);
		DEL_CLASS(me->svr);
		me->svr = 0;
		me->txtPort->SetReadOnly(false);
		return;
	}
	me->txtPort->GetText(sbuff);
	Int32 port = Text::StrToInt32(sbuff);
	NEW_CLASS(me->svr, Rodsum::LSGateResendServer(me->sockf, me->log, port));
	if (me->svr->IsError())
	{
		DEL_CLASS(me->svr);
		me->svr = 0;
		UI::MessageDialog::ShowDialog(L"Error in listening to the port", L"Error");
		return;
	}
	else
	{
		me->reader->SetServer(me->svr);
		me->txtPort->SetReadOnly(true);
	}
}

void __stdcall UtilUI::DataSenderForm::OnLogSelectedChange(void *userObj)
{
	UtilUI::DataSenderForm *me = (UtilUI::DataSenderForm*)userObj;
	OSInt i = me->lbLog->GetSelectedIndex();
	if (i >= 0)
	{
		const WChar *txt = me->lbLog->GetSelectedItemTextNew();
		me->txtLog->SetText(txt);
		Text::StrDelNew(txt);
	}
}

void __stdcall UtilUI::DataSenderForm::OnFileDelayClicked(void *userObj)
{
	WChar sbuff[12];
	UtilUI::DataSenderForm *me = (UtilUI::DataSenderForm*)userObj;
	me->txtFileDelay->GetText(sbuff);
	me->reader->SetFileDelay(Text::StrToInt32(sbuff));
}

void __stdcall UtilUI::DataSenderForm::OnPacketDelayClicked(void *userObj)
{
	WChar sbuff[12];
	UtilUI::DataSenderForm *me = (UtilUI::DataSenderForm*)userObj;
	me->txtPacketDelay->GetText(sbuff);
	me->reader->SetPacketDelay(Text::StrToInt32(sbuff));
}

void UtilUI::DataSenderForm::AddFile(const WChar *fileName)
{
	WChar sbuff[512];
	WChar *sptr;
	IO::Path::PathType pt = IO::Path::GetPathType(fileName);
	if (pt == IO::Path::PathType::Directory)
	{
		Text::StrConcat(Text::StrConcat(sbuff, L"(Dir)"), fileName);
		this->lbFiles->AddItem(sbuff, 0);
		this->files->Add(Text::StrCopyNew(fileName));
		sptr = Text::StrConcat(sbuff, fileName);
		if (sptr[-1] != '\\')
		{
			*sptr++ = '\\';
		}
		Text::StrConcat(sptr , L"*.*");
		void *sess = IO::Path::FindFile(sbuff);
		if (sess)
		{
			IO::Path::PathType pt;
			while (IO::Path::FindNextFile(sptr, sess, 0, &pt, 0))
			{
				if (pt == IO::Path::PathType::File)
				{
					this->reader->AddFile(sbuff);
				}
			}
			IO::Path::FindFileClose(sess);
		}
	}
	else if (pt == IO::Path::PathType::File)
	{
		Text::StrConcat(Text::StrConcat(sbuff, L"(File)"), fileName);
		this->lbFiles->AddItem(sbuff, 0);
		this->files->Add(Text::StrCopyNew(fileName));
		this->reader->AddFile(fileName);
	}
}

UtilUI::DataSenderForm::DataSenderForm(void *hInst, UI::MSWindowClientControl *parent, UI::MSWindowUI *ui, Net::SocketFactory *sockf) : UI::MSWindowForm(hInst, parent, 640, 480, ui)
{
	this->sockf = sockf;
	this->svr = 0;
	this->closing = false;
	this->HandleDropFiles(FileHandler, this);
	this->SetNoResize(true);
	this->SetFont(0, 9, false);
	this->SetText(L"DataSender");
	NEW_CLASS(this->log, IO::LogTool());
	NEW_CLASS(files, Data::ArrayListStr());
	NEW_CLASS(this->reader, Rodsum::LSGateRAWReader());
	this->reader->SetProgressHdlr(this);

	UI::MSWindowLabel *lbl;

	NEW_CLASS(this->tcMain, UI::MSWindowTabControl(hInst, this));
	this->tpCtrl = this->tcMain->AddTabPage(L"Control");
	this->tpLog = this->tcMain->AddTabPage(L"Log");

	NEW_CLASS(this->pnlCtrl, UI::MSWindowPanel(hInst, this->tpCtrl));
	this->pnlCtrl->SetRect(0, 0, 640, 200, false);
	this->pnlCtrl->SetDockType(UI::MSWindowControl::DOCK_TOP);
	NEW_CLASS(this->pgbFile, UI::MSWindowProgressBar(hInst, this->tpCtrl, 100));
	this->pgbFile->SetRect(0, 0, 100, 19, false);
	this->pgbFile->SetDockType(UI::MSWindowControl::DOCK_BOTTOM);
	NEW_CLASS(this->lbFiles, UI::MSWindowListBox(hInst, this->tpCtrl));
	this->lbFiles->SetDockType(UI::MSWindowControl::DOCK_FILL);
	NEW_CLASS(lbl, UI::MSWindowLabel(hInst, this->pnlCtrl, L"Port:"));
	lbl->SetRect(0, 0, 100, 19, false);
	NEW_CLASS(this->txtPort, UI::MSWindowTextBox(hInst, this->pnlCtrl, L"18001"));
	this->txtPort->SetRect(100, 0, 80, 19, false);
	NEW_CLASS(this->btnStart, UI::MSWindowButton(hInst, this->pnlCtrl, L"&Listen"));
	this->btnStart->SetRect(100, 20, 100, 24, false);
	this->btnStart->HandleButtonClick(OnStartClicked, this);
	NEW_CLASS(lbl, UI::MSWindowLabel(hInst, this->pnlCtrl, L"File Delay"));
	lbl->SetRect(0, 44, 100, 19, false);
	NEW_CLASS(this->txtFileDelay, UI::MSWindowTextBox(hInst, this->pnlCtrl, L"0"));
	this->txtFileDelay->SetRect(100, 44, 100, 19, false);
	NEW_CLASS(this->btnFileDelay, UI::MSWindowButton(hInst, this->pnlCtrl, L"&Set"));
	this->btnFileDelay->SetRect(200, 44, 80, 19, false);
	this->btnFileDelay->HandleButtonClick(OnFileDelayClicked, this);
	NEW_CLASS(lbl, UI::MSWindowLabel(hInst, this->pnlCtrl, L"Packet Delay"));
	lbl->SetRect(0, 64, 100, 19, false);
	NEW_CLASS(this->txtPacketDelay, UI::MSWindowTextBox(hInst, this->pnlCtrl, L"0"));
	this->txtPacketDelay->SetRect(100, 64, 100, 19, false);
	NEW_CLASS(this->btnPacketDelay, UI::MSWindowButton(hInst, this->pnlCtrl, L"S&et"));
	this->btnPacketDelay->SetRect(200, 64, 80, 19, false);
	this->btnPacketDelay->HandleButtonClick(OnPacketDelayClicked, this);

	NEW_CLASS(this->txtLog, UI::MSWindowTextBox(hInst, this->tpLog, L""));
	this->txtLog->SetRect(0, 0, 100, 19, false);
	this->txtLog->SetDockType(UI::MSWindowControl::DOCK_BOTTOM);
	NEW_CLASS(this->lbLog, UI::MSWindowListBox(hInst, this->tpLog));
	this->lbLog->SetDockType(UI::MSWindowControl::DOCK_FILL);
	this->lbLog->HandleSelectionChange(OnLogSelectedChange, this);

	NEW_CLASS(this->logger, UI::ListBoxLogger(this, lbLog, 500, true));
	this->log->AddLogHandler(logger, IO::ILogHandler::LOG_LEVEL_RAW);
}

UtilUI::DataSenderForm::~DataSenderForm()
{
	this->closing = true;
	OSInt i = this->files->GetCount();
	while (i-- > 0)
	{
		Text::StrDelNew(this->files->RemoveAt(i));
	}
	DEL_CLASS(this->files);
	if (this->svr)
	{
		this->svr->Close();
		this->reader->SetServer(0);
		DEL_CLASS(this->svr);
		this->svr = 0;
	}
	DEL_CLASS(this->reader);
	DEL_CLASS(this->log);
	DEL_CLASS(this->logger);
}

void UtilUI::DataSenderForm::ProgressUpdate(const WChar *fileName, Int64 currPos, Int64 fileSize)
{
	this->pgbFile->ProgressUpdate(currPos, fileSize);
}

void UtilUI::DataSenderForm::FileStarted(const WChar *fileName, Int64 fileSize)
{
	this->pgbFile->ProgressStart(fileName, fileSize);
}

void UtilUI::DataSenderForm::FileFinished(const WChar *fileName)
{
	if (!closing)
	{
		this->pgbFile->ProgressEnd();
	}
}
