#include "Stdafx.h"
#include "Text/MyString.h"
#include "UI/GUIFileDialog.h"
#include "UtilUI/TextGotoDialog.h"
#include "UtilUI/TextSearchForm.h"
#include "UtilUI/TextViewerForm.h"

#define MNU_FILE_OPEN 101
#define MNU_EDIT_GOTO 102
#define MNU_EDIT_SEARCH 103

void __stdcall UtilUI::TextViewerForm::OnFileDrop(AnyType userObj, Data::DataArray<NN<Text::String>> files)
{
	NN<UtilUI::TextViewerForm> me = userObj.GetNN<UtilUI::TextViewerForm>();
	me->LoadFile(files[0]);
}

void __stdcall UtilUI::TextViewerForm::OnTextPosUpd(AnyType userObj, UInt32 textPosX, UOSInt textPosY)
{
	NN<UtilUI::TextViewerForm> me = userObj.GetNN<UtilUI::TextViewerForm>();
	UTF8Char sbuff[32];
	UnsafeArray<UTF8Char> sptr;
	sptr = Text::StrUInt32(Text::StrConcatC(Text::StrUOSInt(sbuff, (textPosY + 1)), UTF8STRC(" : ")), textPosX + 1);
	me->txtStatus->SetText(CSTRP(sbuff, sptr));
}

void __stdcall UtilUI::TextViewerForm::OnSearchClosed(AnyType userObj, NN<UI::GUIForm> frm)
{
	NN<UtilUI::TextViewerForm> me = userObj.GetNN<UtilUI::TextViewerForm>();
	me->srchFrm = 0;
}

UtilUI::TextViewerForm::TextViewerForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<Media::MonitorMgr> monMgr, NN<Media::DrawEngine> deng, UInt32 codePage) : UI::GUIForm(parent, 1024, 768, ui)
{
	NN<UI::GUIMenu> mnu;
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
	NEW_CLASSNN(this->txtView, UI::GUITextFileView(ui, *this, deng, 0));
	this->txtView->SetCodePage(codePage);
	this->txtView->SetDockType(UI::GUIControl::DOCK_FILL);
	this->HandleDropFiles(OnFileDrop, this);
	this->txtView->HandleTextPosUpdate(OnTextPosUpd, this);
	NEW_CLASSNN(this->mnuMain, UI::GUIMainMenu());
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
	NN<UI::GUIForm> srchFrm;
	if (this->srchFrm.SetTo(srchFrm))
	{
		srchFrm->Close();
	}
}

void UtilUI::TextViewerForm::EventMenuClicked(UInt16 cmdId)
{
	UTF8Char sbuff[530];
	UnsafeArray<UTF8Char> sptr;
	NN<Text::String> fileName;
	switch (cmdId)
	{
	case MNU_FILE_OPEN:
		{
			NN<UI::GUIFileDialog> dlg = this->ui->NewFileDialog(L"SSWR", L"TextViewer", L"Open", false);
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
		this->OpenSearch(nullptr);
		break;
	}
}

void UtilUI::TextViewerForm::OnMonitorChanged()
{
	this->SetDPI(this->monMgr->GetMonitorHDPI(this->GetHMonitor()), this->monMgr->GetMonitorDDPI(this->GetHMonitor()));
}

Bool UtilUI::TextViewerForm::SearchText(Text::CStringNN txt)
{
	this->txtView->SearchText(txt);
	return true;
}

Bool UtilUI::TextViewerForm::LoadFile(NN<Text::String> filePath)
{
	UTF8Char sbuff[530];
	UnsafeArray<UTF8Char> sptr;
	if (this->txtView->LoadFile(filePath))
	{
		sptr = filePath->ConcatTo(Text::StrConcatC(sbuff, UTF8STRC("Text Viewer - ")));
		this->SetText(CSTRP(sbuff, sptr));
		return true;
	}
	return false;
}

Bool UtilUI::TextViewerForm::LoadStreamData(NN<IO::StreamData> data)
{
	UTF8Char sbuff[530];
	UnsafeArray<UTF8Char> sptr;
	if (this->txtView->LoadStreamData(data))
	{
		sptr = data->GetShortName().OrEmpty().ConcatTo(Text::StrConcatC(sbuff, UTF8STRC("Text Viewer - ")));
		this->SetText(CSTRP(sbuff, sptr));
		return true;
	}
	return false;
}

Bool UtilUI::TextViewerForm::OpenSearch(Text::CString txt)
{
	NN<UtilUI::TextSearchForm> srchFrm;
	if (Optional<UtilUI::TextSearchForm>::ConvertFrom(this->srchFrm).SetTo(srchFrm))
	{
		srchFrm->Focus();
		if (txt.leng > 0)
			srchFrm->SetSearchText(txt.OrEmpty());
	}
	else
	{
		NEW_CLASSNN(srchFrm, UtilUI::TextSearchForm(0, this->ui, this->monMgr, *this));
		this->srchFrm = srchFrm;
		srchFrm->HandleFormClosed(OnSearchClosed, this);
		if (txt.leng > 0)
			srchFrm->SetSearchText(txt.OrEmpty());
		srchFrm->Show();
	}
	return true;
}
