#include "Stdafx.h"
#include "Text/MyString.h"
#include "UtilUI/TextInputDialog.h"

void __stdcall UtilUI::TextInputDialog::OnOKClicked(AnyType userObj)
{
	NN<UtilUI::TextInputDialog> me = userObj.GetNN<UtilUI::TextInputDialog>();
	Text::StringBuilderUTF8 sb;
	if (me->txtInput->GetText(sb) && sb.GetLength() > 0)
	{
		me->retInput = Text::String::New(sb.ToCString());
		me->SetDialogResult(UI::GUIForm::DR_OK);
	}
}

void __stdcall UtilUI::TextInputDialog::OnCancelClicked(AnyType userObj)
{
	NN<UtilUI::TextInputDialog> me = userObj.GetNN<UtilUI::TextInputDialog>();
	me->SetDialogResult(UI::GUIForm::DR_CANCEL);
}

UtilUI::TextInputDialog::TextInputDialog(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<Media::MonitorMgr> monMgr, Text::CStringNN title, Text::CStringNN message) : UI::GUIForm(parent, 320, 120, ui)
{
	this->SetFont(nullptr, 8.25, false);
	this->SetText(title);
	this->SetNoResize(true);
	this->monMgr = monMgr;
	this->retInput = nullptr;

	this->lblMessage = ui->NewLabel(*this, message);
	this->lblMessage->SetRect(4, 4, 300, 23, false);
	this->txtInput = ui->NewTextBox(*this, CSTR(""));
	this->txtInput->SetRect(4, 28, 300, 23, false);
	this->btnOk = ui->NewButton(*this, CSTR("&Ok"));
	this->btnOk->SetRect(8, 56, 100, 23, false);
	this->btnOk->HandleButtonClick(OnOKClicked, this);
	this->btnCancel = ui->NewButton(*this, CSTR("&Cancel"));
	this->btnCancel->SetRect(128, 56, 100, 23, false);
	this->btnCancel->HandleButtonClick(OnCancelClicked, this);
	this->SetDefaultButton(this->btnOk);
	this->SetCancelButton(this->btnCancel);
	this->SetDPI(this->monMgr->GetMonitorHDPI(this->GetHMonitor()), this->monMgr->GetMonitorDDPI(this->GetHMonitor()));
}

UtilUI::TextInputDialog::~TextInputDialog()
{
	OPTSTR_DEL(this->retInput);
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

Bool UtilUI::TextInputDialog::GetInputString(NN<Text::StringBuilderUTF8> sb)
{
	NN<Text::String> s;
	if (!this->retInput.SetTo(s))
		return false;
	sb->Append(s);
	return true;	
}
