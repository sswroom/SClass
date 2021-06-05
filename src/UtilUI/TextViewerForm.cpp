#include "Stdafx.h"
#include "Text/MyString.h"
#include "UI/FileDialog.h"
#include "UtilUI/TextGotoDialog.h"
#include "UtilUI/TextSearchForm.h"
#include "UtilUI/TextViewerForm.h"

#define MNU_FILE_OPEN 101
#define MNU_EDIT_GOTO 102
#define MNU_EDIT_SEARCH 103

void __stdcall UtilUI::TextViewerForm::OnFileDrop(void *userObj, const UTF8Char **files, UOSInt nFiles)
{
	UtilUI::TextViewerForm *me = (UtilUI::TextViewerForm*)userObj;
	UTF8Char sbuff[530];
	if (me->txtView->LoadFile(files[0]))
	{
		Text::StrConcat(Text::StrConcat(sbuff, (const UTF8Char*)"Text Viewer - "), files[0]);
		me->SetText(sbuff);
	}
}

void __stdcall UtilUI::TextViewerForm::OnTextPosUpd(void *userObj, UInt32 textPosX, UOSInt textPosY)
{
	UtilUI::TextViewerForm *me = (UtilUI::TextViewerForm*)userObj;
	UTF8Char sbuff[32];
	Text::StrUInt32(Text::StrConcat(Text::StrUOSInt(sbuff, (textPosY + 1)), (const UTF8Char*)" : "), textPosX + 1);
	me->txtStatus->SetText(sbuff);
}

void __stdcall UtilUI::TextViewerForm::OnSearchClosed(void *userObj, UI::GUIForm *frm)
{
	UtilUI::TextViewerForm *me = (UtilUI::TextViewerForm*)userObj;
	me->srchFrm = 0;
}

UtilUI::TextViewerForm::TextViewerForm(UI::GUIClientControl *parent, UI::GUICore *ui, Media::MonitorMgr *monMgr, UInt32 codePage) : UI::GUIForm(parent, 1024, 768, ui)
{
	UI::GUIMenu *mnu;
	this->SetFont(0, 8.25, false);
	this->SetText((const UTF8Char*)"Text Viewer");

	this->monMgr = monMgr;
	this->srchFrm = 0;
	NEW_CLASS(this->pnlStatus, UI::GUIPanel(ui, this));
	this->pnlStatus->SetRect(0, 0, 100, 24, false);
	this->pnlStatus->SetDockType(UI::GUIControl::DOCK_BOTTOM);
	NEW_CLASS(this->txtStatus, UI::GUITextBox(ui, this->pnlStatus, (const UTF8Char*)""));
	this->txtStatus->SetRect(0, 0, 200, 23, false);
	this->txtStatus->SetReadOnly(true);
	this->txtStatus->SetDockType(UI::GUIControl::DOCK_LEFT);
	NEW_CLASS(this->txtView, UI::GUITextFileView(ui, this));
	this->txtView->SetCodePage(codePage);
	this->txtView->SetDockType(UI::GUIControl::DOCK_FILL);
	this->HandleDropFiles(OnFileDrop, this);
	this->txtView->HandleTextPosUpdate(OnTextPosUpd, this);
	NEW_CLASS(this->mnuMain, UI::GUIMainMenu());
	mnu = this->mnuMain->AddSubMenu((const UTF8Char*)"&File");
	mnu->AddItem((const UTF8Char*)"&Open...", MNU_FILE_OPEN, UI::GUIMenu::KM_CONTROL, UI::GUIControl::GK_O);
	mnu = this->mnuMain->AddSubMenu((const UTF8Char*)"&Edit");
	mnu->AddItem((const UTF8Char*)"&GoTo...", MNU_EDIT_GOTO, UI::GUIMenu::KM_CONTROL, UI::GUIControl::GK_G);
	mnu->AddItem((const UTF8Char*)"&Search", MNU_EDIT_SEARCH, UI::GUIMenu::KM_CONTROL, UI::GUIControl::GK_F);
	this->SetMenu(this->mnuMain);
	this->SetDPI(this->monMgr->GetMonitorHDPI(this->GetHMonitor()), this->monMgr->GetMonitorDDPI(this->GetHMonitor()));
}

UtilUI::TextViewerForm::~TextViewerForm()
{
	if (this->srchFrm)
	{
		this->srchFrm->Close();
	}
}

void UtilUI::TextViewerForm::EventMenuClicked(UInt16 cmdId)
{
	UTF8Char sbuff[530];
	UI::FileDialog *dlg;
	UtilUI::TextGotoDialog *gotoDlg;
	const UTF8Char *fileName;
	switch (cmdId)
	{
	case MNU_FILE_OPEN:
		NEW_CLASS(dlg, UI::FileDialog(L"sswr", L"TextViewer", L"Open", false));
		dlg->AddFilter((const UTF8Char*)"*.txt", (const UTF8Char*)"Text File");
		dlg->AddFilter((const UTF8Char*)"*.log", (const UTF8Char*)"Log File");
		dlg->AddFilter((const UTF8Char*)"*.cfg", (const UTF8Char*)"Config File");
		dlg->AddFilter((const UTF8Char*)"*.ini", (const UTF8Char*)"Ini File");
		fileName = this->txtView->GetFileName();
		if (fileName)
		{
			dlg->SetFileName(fileName);
		}
		if (dlg->ShowDialog(this->GetHandle()))
		{
			if (this->txtView->LoadFile(dlg->GetFileName()))
			{
				Text::StrConcat(Text::StrConcat(sbuff, (const UTF8Char*)"Text Viewer - "), dlg->GetFileName());
				this->SetText(sbuff);
			}
		}
		DEL_CLASS(dlg);
		break;
	case MNU_EDIT_GOTO:
		NEW_CLASS(gotoDlg, UtilUI::TextGotoDialog(0, this->ui, this->monMgr));
		gotoDlg->SetLineNumber(this->txtView->GetTextPosY() + 1);
		if (gotoDlg->ShowDialog(this) == UI::GUIForm::DR_OK)
		{
			this->txtView->GoToText(gotoDlg->GetLineNumber() - 1, 0);
			this->txtView->Focus();
		}
		DEL_CLASS(gotoDlg);
		break;
	case MNU_EDIT_SEARCH:
		if (this->srchFrm)
		{
			this->srchFrm->Focus();
		}
		else
		{
			NEW_CLASS(this->srchFrm, UtilUI::TextSearchForm(0, this->ui, this->monMgr, this));
			this->srchFrm->HandleFormClosed(OnSearchClosed, this);
			this->srchFrm->Show();
		}
		break;
	}
}

void UtilUI::TextViewerForm::OnMonitorChanged()
{
	this->SetDPI(this->monMgr->GetMonitorHDPI(this->GetHMonitor()), this->monMgr->GetMonitorDDPI(this->GetHMonitor()));
}

Bool UtilUI::TextViewerForm::SearchText(const UTF8Char *txt)
{
	this->txtView->SearchText(txt);
	return true;
}
