#include "stdafx.h"
#include "IO/StmData/FileData.h"
#include "Media/FileAnalyse/MPEGFileAnalyse.h"
#include "SSWR/AVIRead/AVIRMPEGToolForm.h"
#include "Text/MyString.h"
#include "UI/FileDialog.h"
#include "UI/MessageDialog.h"

#define PER_PAGE 10000

void __stdcall SSWR::AVIRead::AVIRMPEGToolForm::OnFileDrop(void *userObj, const WChar **files, OSInt nFiles)
{
	SSWR::AVIRead::AVIRMPEGToolForm *me = (SSWR::AVIRead::AVIRMPEGToolForm *)userObj;
	OSInt i;
	i = 0;
	while (i < nFiles)
	{
		if (me->OpenFile(files[i]))
			break;
		i++;
	}
}

void __stdcall SSWR::AVIRead::AVIRMPEGToolForm::OnFileClicked(void *userObj)
{
	SSWR::AVIRead::AVIRMPEGToolForm *me = (SSWR::AVIRead::AVIRMPEGToolForm *)userObj;
	UI::FileDialog *dlg;
	NEW_CLASS(dlg, UI::FileDialog(L"SSWR", L"AVIRead", L"MPEGTool", false));
	dlg->AddFilter(L"*.mpg", L"MPEG System Stream");
	if (dlg->ShowDialog(me->GetHandle()))
	{
		me->OpenFile(dlg->GetFileName());
	}
}

void __stdcall SSWR::AVIRead::AVIRMPEGToolForm::OnTrimPaddingClicked(void *userObj)
{
	SSWR::AVIRead::AVIRMPEGToolForm *me = (SSWR::AVIRead::AVIRMPEGToolForm *)userObj;
	UI::FileDialog *dlg;
	Text::StringBuilder sb;
	NEW_CLASS(dlg, UI::FileDialog(L"SSWR", L"AVIRead", L"MPEGTrimPadding", true));
	dlg->AddFilter(L"*.mpg", L"MPEG System Stream");
	me->txtFile->GetText(&sb);
	dlg->SetFileName(sb.ToString());
	if (dlg->ShowDialog(me->GetHandle()))
	{
		if (me->file->TrimPadding(dlg->GetFileName()))
		{
		}
		else
		{
			UI::MessageDialog::ShowDialog(L"Error in saving the file", L"Error");
		}
	}
	DEL_CLASS(dlg);
}

void __stdcall SSWR::AVIRead::AVIRMPEGToolForm::OnTimerTick(void *userObj)
{
	SSWR::AVIRead::AVIRMPEGToolForm *me = (SSWR::AVIRead::AVIRMPEGToolForm *)userObj;
	WChar sbuff[32];
	WChar *sptr;
	if (me->file)
	{
		OSInt currCnt = me->file->GetFrameCount();
		OSInt i;
		OSInt j;
		if (currCnt != me->lastPackCount)
		{
			i = (currCnt - 1) / PER_PAGE;
			j = (me->lastPackCount - 1) / PER_PAGE;
			if (currCnt == 0)
				i = -1;
			if (me->lastPackCount == 0)
				j = -1;
			if (me->lastPackCount % PER_PAGE)
			{
				if (j == i)
				{
					sptr = Text::StrInt32(sbuff, (Int32)j * PER_PAGE);
					sptr = Text::StrConcat(sptr, L" - ");
					sptr = Text::StrInt32(sptr, (Int32)(currCnt - 1));
					me->lbPackList->RemoveItem(j);
					me->lbPackList->AddItem(sbuff, 0);
					j++;
				}
				else
				{
					sptr = Text::StrInt32(sbuff, (Int32)j * PER_PAGE);
					sptr = Text::StrConcat(sptr, L" - ");
					sptr = Text::StrInt32(sptr, (Int32)(j * PER_PAGE + PER_PAGE - 1));
					me->lbPackList->RemoveItem(j);
					me->lbPackList->AddItem(sbuff, 0);

				}
			}
			while (++j < i)
			{
				sptr = Text::StrInt32(sbuff, (Int32)j * PER_PAGE);
				sptr = Text::StrConcat(sptr, L" - ");
				sptr = Text::StrInt32(sptr, (Int32)(j * PER_PAGE + PER_PAGE - 1));
				me->lbPackList->AddItem(sbuff, 0);
			}
			if (j == i)
			{
				sptr = Text::StrInt32(sbuff, (Int32)j * PER_PAGE);
				sptr = Text::StrConcat(sptr, L" - ");
				sptr = Text::StrInt32(sptr, (Int32)(currCnt - 1));
				me->lbPackList->AddItem(sbuff, 0);
				j++;
			}

			me->lastPackCount = currCnt;
		}
	}
}

void __stdcall SSWR::AVIRead::AVIRMPEGToolForm::OnPackListChanged(void *userObj)
{
	SSWR::AVIRead::AVIRMPEGToolForm *me = (SSWR::AVIRead::AVIRMPEGToolForm *)userObj;
	Text::StringBuilder sb;
	OSInt i;
	OSInt j;
	me->lbPackItems->ClearItems();
	i = me->lbPackList->GetSelectedIndex();
	if (i < 0)
		return;
	i = i * PER_PAGE;
	j = i + PER_PAGE;
	if (j > me->file->GetFrameCount())
	{
		j = me->file->GetFrameCount();
	}
	while (i < j)
	{
		sb.ClearStr();
		me->file->GetFrameName(i, &sb);
		me->lbPackItems->AddItem(sb.ToString(), (void*)i);
		i++;
	}
}

void __stdcall SSWR::AVIRead::AVIRMPEGToolForm::OnPackItemChanged(void *userObj)
{
	SSWR::AVIRead::AVIRMPEGToolForm *me = (SSWR::AVIRead::AVIRMPEGToolForm *)userObj;
	Text::StringBuilder sb;
	OSInt i = me->lbPackItems->GetSelectedIndex();
	if (i < 0)
	{
		me->txtPack->SetText(L"");
		return;
	}
	i = (OSInt)me->lbPackItems->GetItem(i);
	sb.ClearStr();
	me->file->GetFrameDetail(i, &sb);
	me->txtPack->SetText(sb.ToString());
}

Bool SSWR::AVIRead::AVIRMPEGToolForm::OpenFile(const WChar *fileName)
{
	IO::StmData::FileData *fd;
	Media::FileAnalyse::IFileAnalyse *file;
	NEW_CLASS(fd, IO::StmData::FileData(fileName, false));
	file = Media::FileAnalyse::IFileAnalyse::AnalyseFile(fd);
	DEL_CLASS(fd);
	if (file)
	{
		SDEL_CLASS(this->file);
		this->file = file;
		this->txtFile->SetText(fileName);
		this->lastPackCount = 0;
		this->lbPackList->ClearItems();
		this->lbPackItems->ClearItems();
		return true;
	}
	DEL_CLASS(file);
	return false;
}

SSWR::AVIRead::AVIRMPEGToolForm::AVIRMPEGToolForm(void *hInst, UI::MSWindowClientControl *parent, UI::MSWindowUI *ui, SSWR::AVIRead::AVIRCore *core) : UI::MSWindowForm(hInst, parent, 800, 600, ui)
{
	this->SetText(L"MPEG Tool");
	this->SetFont(0, 8.25, false);

	this->core = core;
	this->file = 0;
	this->lastPackCount = 0;

	NEW_CLASS(this->pnlCtrl, UI::MSWindowPanel(hInst, this));
	this->pnlCtrl->SetRect(0, 0, 100, 40, false);
	this->pnlCtrl->SetDockType(UI::MSWindowControl::DOCK_TOP);
	NEW_CLASS(this->tcMain, UI::MSWindowTabControl(hInst, this));
	this->tcMain->SetDockType(UI::MSWindowControl::DOCK_FILL);
	NEW_CLASS(this->lblFile, UI::MSWindowLabel(hInst, this->pnlCtrl, L"MPEG File"));
	this->lblFile->SetRect(8, 8, 100, 23, false);
	NEW_CLASS(this->txtFile, UI::MSWindowTextBox(hInst, this->pnlCtrl, L""));
	this->txtFile->SetRect(108, 8, 500, 23, false);
	this->txtFile->SetReadOnly(true);
	NEW_CLASS(this->btnFile, UI::MSWindowButton(hInst, this->pnlCtrl, L"B&rowse"));
	this->btnFile->SetRect(608, 8, 75, 23, false);
	this->btnFile->HandleButtonClick(OnFileClicked, this);
	this->tpContent = this->tcMain->AddTabPage(L"Content");
	this->tpStream = this->tcMain->AddTabPage(L"Stream");
	this->tpTool = this->tcMain->AddTabPage(L"Tool");

	NEW_CLASS(this->lbPackList, UI::MSWindowListBox(hInst, this->tpContent));
	this->lbPackList->SetRect(0, 0, 200, 100, false);
	this->lbPackList->SetDockType(UI::MSWindowControl::DOCK_LEFT);
	this->lbPackList->HandleSelectionChange(OnPackListChanged, this);
	NEW_CLASS(this->hspPack, UI::MSWindowHSplitter(hInst, this->tpContent, 3, false));
	NEW_CLASS(this->lbPackItems, UI::MSWindowListBox(hInst, this->tpContent));
	this->lbPackItems->SetRect(0, 0, 250, 100, false);
	this->lbPackItems->SetDockType(UI::MSWindowControl::DOCK_LEFT);
	this->lbPackItems->HandleSelectionChange(OnPackItemChanged, this);
	NEW_CLASS(this->hspPack2, UI::MSWindowHSplitter(hInst, this->tpContent, 3, false));
	NEW_CLASS(this->txtPack, UI::MSWindowTextBox(hInst, this->tpContent, L"", true));
	this->txtPack->SetDockType(UI::MSWindowControl::DOCK_FILL);
	this->txtPack->SetReadOnly(true);

	NEW_CLASS(this->btnTrimPadding, UI::MSWindowButton(hInst, this->tpTool, L"Trim Padding"));
	this->btnTrimPadding->SetRect(8, 8, 75, 23, false);
	this->btnTrimPadding->HandleButtonClick(OnTrimPaddingClicked, this);

	this->HandleDropFiles(OnFileDrop, this);
	this->AddTimer(1000, OnTimerTick, this);
}

SSWR::AVIRead::AVIRMPEGToolForm::~AVIRMPEGToolForm()
{
	SDEL_CLASS(this->file);
}
