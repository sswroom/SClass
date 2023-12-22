#include "Stdafx.h"
#include "Text/MyString.h"
#include "UI/GUIFileDialog.h"
#include "UtilUI/TextGotoDialog.h"
#include "UtilUI/TextSearchForm.h"
#include "UtilUI/TextViewerForm.h"

#define MNU_FILE_OPEN 101
#define MNU_EDIT_GOTO 102
#define MNU_EDIT_SEARCH 103

void __stdcall UtilUI::TextViewerForm::OnFileDrop(void *userObj, NotNullPtr<Text::String> *files, UOSInt nFiles)
{
	UtilUI::TextViewerForm *me = (UtilUI::TextViewerForm*)userObj;
	me->LoadFile(files[0]);
}

void __stdcall UtilUI::TextViewerForm::OnTextPosUpd(void *userObj, UInt32 textPosX, UOSInt textPosY)
{
	UtilUI::TextViewerForm *me = (UtilUI::TextViewerForm*)userObj;
	UTF8Char sbuff[32];
	UTF8Char *sptr;
	sptr = Text::StrUInt32(Text::StrConcatC(Text::StrUOSInt(sbuff, (textPosY + 1)), UTF8STRC(" : ")), textPosX + 1);
	me->txtStatus->SetText(CSTRP(sbuff, sptr));
}

void __stdcall UtilUI::TextViewerForm::OnSearchClosed(void *userObj, UI::GUIForm *frm)
{
	UtilUI::TextViewerForm *me = (UtilUI::TextViewerForm*)userObj;
	me->srchFrm = 0;
}

UtilUI::TextViewerForm::TextViewerForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, Media::MonitorMgr *monMgr, NotNullPtr<Media::DrawEngine> deng, UInt32 codePage) : UI::GUIForm(parent, 1024, 768, ui)
{
	UI::GUIMenu *mnu;
	this->SetFont(0, 0, 8.25, false);
	this->SetText(CSTR("Text Viewer"));

	this->monMgr = monMgr;
	this->srchFrm = 0;
	this->pnlStatus = ui->NewPanel(*this);
	this->pnlStatus->SetRect(0, 0, 100, 24, false);
	this->pnlStatus->SetDockType(UI::GUIControl::DOCK_BOTTOM);
	this->txtStatus = ui->NewTextBox(this->pnlStatus, CSTR(""));
	this->txtStatus->SetRect(0, 0, 200, 23, false);
	this->txtStatus->SetReadOnly(true);
	this->txtStatus->SetDockType(UI::GUIControl::DOCK_LEFT);
	NEW_CLASS(this->txtView, UI::GUITextFileView(ui, *this, deng));
	this->txtView->SetCodePage(codePage);
	this->txtView->SetDockType(UI::GUIControl::DOCK_FILL);
	this->HandleDropFiles(OnFileDrop, this);
	this->txtView->HandleTextPosUpdate(OnTextPosUpd, this);
	NEW_CLASS(this->mnuMain, UI::GUIMainMenu());
	mnu = this->mnuMain->AddSubMenu(CSTR("&File"));
	mnu->AddItem(CSTR("&Open..."), MNU_FILE_OPEN, UI::GUIMenu::KM_CONTROL, UI::GUIControl::GK_O);
	mnu = this->mnuMain->AddSubMenu(CSTR("&Edit"));
	mnu->AddItem(CSTR("&GoTo..."), MNU_EDIT_GOTO, UI::GUIMenu::KM_CONTROL, UI::GUIControl::GK_G);
	mnu->AddItem(CSTR("&Search"), MNU_EDIT_SEARCH, UI::GUIMenu::KM_CONTROL, UI::GUIControl::GK_F);
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
	UTF8Char *sptr;
	NotNullPtr<Text::String> fileName;
	switch (cmdId)
	{
	case MNU_FILE_OPEN:
		{
			NotNullPtr<UI::GUIFileDialog> dlg = this->ui->NewFileDialog(L"SSWR", L"TextViewer", L"Open", false);
			dlg->AddFilter(CSTR("*.txt"), CSTR("Text File"));
			dlg->AddFilter(CSTR("*.log"), CSTR("Log File"));
			dlg->AddFilter(CSTR("*.cfg"), CSTR("Config File"));
			dlg->AddFilter(CSTR("*.ini"), CSTR("Ini File"));
			fileName = this->txtView->GetFileName();
			if (fileName->leng > 0)
			{
				dlg->SetFileName(fileName->ToCString());
			}
			if (dlg->ShowDialog(this->GetHandle()))
			{
				if (this->txtView->LoadFile(dlg->GetFileName()))
				{
					sptr = dlg->GetFileName()->ConcatTo(Text::StrConcatC(sbuff, UTF8STRC("Text Viewer - ")));
					this->SetText(CSTRP(sbuff, sptr));
				}
			}
			dlg.Delete();
		}
		break;
	case MNU_EDIT_GOTO:
		{
			UtilUI::TextGotoDialog gotoDlg(0, this->ui, this->monMgr);
			gotoDlg.SetLineNumber(this->txtView->GetTextPosY() + 1);
			if (gotoDlg.ShowDialog(this) == UI::GUIForm::DR_OK)
			{
				this->txtView->GoToText(gotoDlg.GetLineNumber() - 1, 0);
				this->txtView->Focus();
			}
		}
		break;
	case MNU_EDIT_SEARCH:
		this->OpenSearch(CSTR_NULL);
		break;
	}
}

void UtilUI::TextViewerForm::OnMonitorChanged()
{
	this->SetDPI(this->monMgr->GetMonitorHDPI(this->GetHMonitor()), this->monMgr->GetMonitorDDPI(this->GetHMonitor()));
}

Bool UtilUI::TextViewerForm::SearchText(Text::CString txt)
{
	this->txtView->SearchText(txt);
	return true;
}

Bool UtilUI::TextViewerForm::LoadFile(NotNullPtr<Text::String> filePath)
{
	UTF8Char sbuff[530];
	UTF8Char *sptr;
	if (this->txtView->LoadFile(filePath))
	{
		sptr = filePath->ConcatTo(Text::StrConcatC(sbuff, UTF8STRC("Text Viewer - ")));
		this->SetText(CSTRP(sbuff, sptr));
		return true;
	}
	return false;
}

Bool UtilUI::TextViewerForm::LoadStreamData(NotNullPtr<IO::StreamData> data)
{
	UTF8Char sbuff[530];
	UTF8Char *sptr;
	if (this->txtView->LoadStreamData(data))
	{
		sptr = data->GetShortName().ConcatTo(Text::StrConcatC(sbuff, UTF8STRC("Text Viewer - ")));
		this->SetText(CSTRP(sbuff, sptr));
		return true;
	}
	return false;
}

Bool UtilUI::TextViewerForm::OpenSearch(Text::CString txt)
{
	if (this->srchFrm)
	{
		this->srchFrm->Focus();
		if (txt.leng > 0)
			((UtilUI::TextSearchForm*)this->srchFrm)->SetSearchText(txt.OrEmpty());
	}
	else
	{
		NEW_CLASS(this->srchFrm, UtilUI::TextSearchForm(0, this->ui, this->monMgr, this));
		this->srchFrm->HandleFormClosed(OnSearchClosed, this);
		if (txt.leng > 0)
			((UtilUI::TextSearchForm*)this->srchFrm)->SetSearchText(txt.OrEmpty());
		this->srchFrm->Show();
	}
	return true;
}
