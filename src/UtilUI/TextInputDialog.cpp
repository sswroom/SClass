#include "Stdafx.h"
#include "Text/MyString.h"
#include "UtilUI/TextInputDialog.h"

void __stdcall UtilUI::TextInputDialog::OnOKClicked(void *userObj)
{
	UtilUI::TextInputDialog *me = (UtilUI::TextInputDialog*)userObj;
	Text::StringBuilderUTF8 sb;
	if (me->txtInput->GetText(sb) && sb.GetLength() > 0)
	{
		me->retInput = Text::String::New(sb.ToCString()).Ptr();
		me->SetDialogResult(UI::GUIForm::DR_OK);
	}
}

void __stdcall UtilUI::TextInputDialog::OnCancelClicked(void *userObj)
{
	UtilUI::TextInputDialog *me = (UtilUI::TextInputDialog*)userObj;
	me->SetDialogResult(UI::GUIForm::DR_CANCEL);
}

UtilUI::TextInputDialog::TextInputDialog(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, Media::MonitorMgr *monMgr, Text::CStringNN title, Text::CStringNN message) : UI::GUIForm(parent, 320, 120, ui)
{
	this->SetFont(0, 0, 8.25, false);
	this->SetText(title);
	this->SetNoResize(true);
	this->monMgr = monMgr;
	this->retInput = 0;

	NEW_CLASS(this->lblMessage, UI::GUILabel(ui, *this, message));
	this->lblMessage->SetRect(4, 4, 300, 23, false);
	NEW_CLASS(this->txtInput, UI::GUITextBox(ui, *this, CSTR("")));
	this->txtInput->SetRect(4, 28, 300, 23, false);
	NEW_CLASS(this->btnOk, UI::GUIButton(ui, *this, CSTR("&Ok")));
	this->btnOk->SetRect(8, 56, 100, 23, false);
	this->btnOk->HandleButtonClick(OnOKClicked, this);
	NEW_CLASS(this->btnCancel, UI::GUIButton(ui, *this, CSTR("&Cancel")));
	this->btnCancel->SetRect(128, 56, 100, 23, false);
	this->btnCancel->HandleButtonClick(OnCancelClicked, this);
	this->SetDefaultButton(this->btnOk);
	this->SetCancelButton(this->btnCancel);
	this->SetDPI(this->monMgr->GetMonitorHDPI(this->GetHMonitor()), this->monMgr->GetMonitorDDPI(this->GetHMonitor()));
}

UtilUI::TextInputDialog::~TextInputDialog()
{
	SDEL_STRING(this->retInput);
}

void UtilUI::TextInputDialog::OnShow()
{
	this->txtInput->Focus();
}

void UtilUI::TextInputDialog::OnMonitorChanged()
{
	this->SetDPI(this->monMgr->GetMonitorHDPI(this->GetHMonitor()), this->monMgr->GetMonitorDDPI(this->GetHMonitor()));
}

void UtilUI::TextInputDialog::SetInputString(Text::CStringNN s)
{
	this->txtInput->SetText(s);
}

Bool UtilUI::TextInputDialog::GetInputString(NotNullPtr<Text::StringBuilderUTF8> sb)
{
	if (this->retInput == 0)
		return false;
	sb->Append(this->retInput);
	return true;	
}
