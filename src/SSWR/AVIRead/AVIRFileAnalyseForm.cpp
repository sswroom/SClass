#include "Stdafx.h"
#include "IO/StmData/FileData.h"
#include "IO/FileAnalyse/IFileAnalyse.h"
#include "SSWR/AVIRead/AVIRFileAnalyseForm.h"
#include "Text/MyString.h"
#include "Text/StringBuilderUTF8.h"
#include "UI/FileDialog.h"
#include "UI/MessageDialog.h"

#define PER_PAGE 10000

void __stdcall SSWR::AVIRead::AVIRFileAnalyseForm::OnFileDrop(void *userObj, const UTF8Char **files, UOSInt nFiles)
{
	SSWR::AVIRead::AVIRFileAnalyseForm *me = (SSWR::AVIRead::AVIRFileAnalyseForm *)userObj;
	UOSInt i;
	i = 0;
	while (i < nFiles)
	{
		if (me->OpenFile(files[i]))
			break;
		i++;
	}
}

void __stdcall SSWR::AVIRead::AVIRFileAnalyseForm::OnFileClicked(void *userObj)
{
	SSWR::AVIRead::AVIRFileAnalyseForm *me = (SSWR::AVIRead::AVIRFileAnalyseForm *)userObj;
	UI::FileDialog *dlg;
	NEW_CLASS(dlg, UI::FileDialog(L"SSWR", L"AVIRead", L"MPEGTool", false));
	IO::FileAnalyse::IFileAnalyse::AddFilters(dlg);
	if (dlg->ShowDialog(me->GetHandle()))
	{
		me->OpenFile(dlg->GetFileName());
	}
}

void __stdcall SSWR::AVIRead::AVIRFileAnalyseForm::OnTrimPaddingClicked(void *userObj)
{
	SSWR::AVIRead::AVIRFileAnalyseForm *me = (SSWR::AVIRead::AVIRFileAnalyseForm *)userObj;
	UI::FileDialog *dlg;
	Text::StringBuilderUTF8 sb;
	NEW_CLASS(dlg, UI::FileDialog(L"SSWR", L"AVIRead", L"MPEGTrimPadding", true));
	dlg->AddFilter((const UTF8Char*)"*.mpg", (const UTF8Char*)"MPEG System Stream");
	me->txtFile->GetText(&sb);
	dlg->SetFileName(sb.ToString());
	if (dlg->ShowDialog(me->GetHandle()))
	{
		if (me->file->TrimPadding(dlg->GetFileName()))
		{
		}
		else
		{
			UI::MessageDialog::ShowDialog((const UTF8Char*)"Error in saving the file", (const UTF8Char*)"Error", me);
		}
	}
	DEL_CLASS(dlg);
}

void __stdcall SSWR::AVIRead::AVIRFileAnalyseForm::OnTimerTick(void *userObj)
{
	SSWR::AVIRead::AVIRFileAnalyseForm *me = (SSWR::AVIRead::AVIRFileAnalyseForm *)userObj;
	UTF8Char sbuff[32];
	UTF8Char *sptr;
	if (me->file)
	{
		UOSInt currCnt = me->file->GetFrameCount();
		OSInt i;
		OSInt j;
		if (currCnt != me->lastPackCount)
		{
			i = (OSInt)(currCnt - 1) / PER_PAGE;
			j = (OSInt)(me->lastPackCount - 1) / PER_PAGE;
			if (currCnt == 0)
				i = -1;
			if (me->lastPackCount == 0)
				j = -1;
			if (me->lastPackCount % PER_PAGE)
			{
				if (j == i)
				{
					sptr = Text::StrInt32(sbuff, (Int32)j * PER_PAGE);
					sptr = Text::StrConcatC(sptr, UTF8STRC(" - "));
					sptr = Text::StrInt32(sptr, (Int32)(currCnt - 1));
					me->lbPackList->RemoveItem((UOSInt)j);
					me->lbPackList->AddItem(sbuff, 0);
					j++;
				}
				else
				{
					sptr = Text::StrInt32(sbuff, (Int32)j * PER_PAGE);
					sptr = Text::StrConcatC(sptr, UTF8STRC(" - "));
					sptr = Text::StrInt32(sptr, (Int32)(j * PER_PAGE + PER_PAGE - 1));
					me->lbPackList->RemoveItem((UOSInt)j);
					me->lbPackList->AddItem(sbuff, 0);

				}
			}
			while (++j < i)
			{
				sptr = Text::StrInt32(sbuff, (Int32)j * PER_PAGE);
				sptr = Text::StrConcatC(sptr, UTF8STRC(" - "));
				sptr = Text::StrInt32(sptr, (Int32)(j * PER_PAGE + PER_PAGE - 1));
				me->lbPackList->AddItem(sbuff, 0);
			}
			if (j == i)
			{
				sptr = Text::StrInt32(sbuff, (Int32)j * PER_PAGE);
				sptr = Text::StrConcatC(sptr, UTF8STRC(" - "));
				sptr = Text::StrInt32(sptr, (Int32)(currCnt - 1));
				me->lbPackList->AddItem(sbuff, 0);
				j++;
			}

			me->lastPackCount = currCnt;
		}
	}
}

void __stdcall SSWR::AVIRead::AVIRFileAnalyseForm::OnPackListChanged(void *userObj)
{
	SSWR::AVIRead::AVIRFileAnalyseForm *me = (SSWR::AVIRead::AVIRFileAnalyseForm *)userObj;
	Text::StringBuilderUTF8 sb;
	UOSInt i;
	UOSInt j;
	me->lbPackItems->ClearItems();
	i = (UOSInt)me->lbPackList->GetSelectedIndex();
	if (i == (UOSInt)-1)
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

void __stdcall SSWR::AVIRead::AVIRFileAnalyseForm::OnPackItemChanged(void *userObj)
{
	SSWR::AVIRead::AVIRFileAnalyseForm *me = (SSWR::AVIRead::AVIRFileAnalyseForm *)userObj;
	Text::StringBuilderUTF8 sb;
	UOSInt i = (UOSInt)me->lbPackItems->GetSelectedIndex();
	if (i == (UOSInt)-1)
	{
		me->txtPack->SetText((const UTF8Char*)"");
		return;
	}
	i = (UOSInt)me->lbPackItems->GetItem(i);
	sb.ClearStr();
	me->file->GetFrameDetail(i, &sb);
	me->txtPack->SetText(sb.ToString());
}

Bool SSWR::AVIRead::AVIRFileAnalyseForm::OpenFile(const UTF8Char *fileName)
{
	IO::StmData::FileData *fd;
	IO::FileAnalyse::IFileAnalyse *file;
	NEW_CLASS(fd, IO::StmData::FileData(fileName, false));
	file = IO::FileAnalyse::IFileAnalyse::AnalyseFile(fd);
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

SSWR::AVIRead::AVIRFileAnalyseForm::AVIRFileAnalyseForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core) : UI::GUIForm(parent, 800, 600, ui)
{
	this->SetText((const UTF8Char*)"File Analyse");
	this->SetFont(0, 0, 8.25, false);

	this->core = core;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
	this->file = 0;
	this->lastPackCount = 0;

	NEW_CLASS(this->pnlCtrl, UI::GUIPanel(ui, this));
	this->pnlCtrl->SetRect(0, 0, 100, 40, false);
	this->pnlCtrl->SetDockType(UI::GUIControl::DOCK_TOP);
	NEW_CLASS(this->tcMain, UI::GUITabControl(ui, this));
	this->tcMain->SetDockType(UI::GUIControl::DOCK_FILL);
	NEW_CLASS(this->lblFile, UI::GUILabel(ui, this->pnlCtrl, (const UTF8Char*)"MPEG File"));
	this->lblFile->SetRect(8, 8, 100, 23, false);
	NEW_CLASS(this->txtFile, UI::GUITextBox(ui, this->pnlCtrl, (const UTF8Char*)""));
	this->txtFile->SetRect(108, 8, 500, 23, false);
	this->txtFile->SetReadOnly(true);
	NEW_CLASS(this->btnFile, UI::GUIButton(ui, this->pnlCtrl, (const UTF8Char*)"B&rowse"));
	this->btnFile->SetRect(608, 8, 75, 23, false);
	this->btnFile->HandleButtonClick(OnFileClicked, this);
	this->tpContent = this->tcMain->AddTabPage((const UTF8Char*)"Content");
	this->tpStream = this->tcMain->AddTabPage((const UTF8Char*)"Stream");
	this->tpTool = this->tcMain->AddTabPage((const UTF8Char*)"Tool");

	NEW_CLASS(this->lbPackList, UI::GUIListBox(ui, this->tpContent, false));
	this->lbPackList->SetRect(0, 0, 200, 100, false);
	this->lbPackList->SetDockType(UI::GUIControl::DOCK_LEFT);
	this->lbPackList->HandleSelectionChange(OnPackListChanged, this);
	NEW_CLASS(this->hspPack, UI::GUIHSplitter(ui, this->tpContent, 3, false));
	NEW_CLASS(this->lbPackItems, UI::GUIListBox(ui, this->tpContent, false));
	this->lbPackItems->SetRect(0, 0, 250, 100, false);
	this->lbPackItems->SetDockType(UI::GUIControl::DOCK_LEFT);
	this->lbPackItems->HandleSelectionChange(OnPackItemChanged, this);
	NEW_CLASS(this->hspPack2, UI::GUIHSplitter(ui, this->tpContent, 3, false));
	NEW_CLASS(this->txtPack, UI::GUITextBox(ui, this->tpContent, (const UTF8Char*)"", true));
	this->txtPack->SetDockType(UI::GUIControl::DOCK_FILL);
	this->txtPack->SetReadOnly(true);

	NEW_CLASS(this->btnTrimPadding, UI::GUIButton(ui, this->tpTool, (const UTF8Char*)"Trim Padding"));
	this->btnTrimPadding->SetRect(8, 8, 75, 23, false);
	this->btnTrimPadding->HandleButtonClick(OnTrimPaddingClicked, this);

	this->HandleDropFiles(OnFileDrop, this);
	this->AddTimer(1000, OnTimerTick, this);
}

SSWR::AVIRead::AVIRFileAnalyseForm::~AVIRFileAnalyseForm()
{
	SDEL_CLASS(this->file);
}

void SSWR::AVIRead::AVIRFileAnalyseForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
