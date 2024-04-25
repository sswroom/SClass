#include "Stdafx.h"
#include "IO/EXEFile.h"
#include "IO/Path.h"
#include "SSWR/AVIRead/AVIRStringMsgForm.h"
#include "Text/MyString.h"
#include "UI/GUIButton.h"
#include "UI/GUILabel.h"

void __stdcall SSWR::AVIRead::AVIRStringMsgForm::OnOKClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRStringMsgForm> me = userObj.GetNN<SSWR::AVIRead::AVIRStringMsgForm>();
	me->Close();
}

SSWR::AVIRead::AVIRStringMsgForm::AVIRStringMsgForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core, Text::CStringNN title, Text::CStringNN msg) : UI::GUIForm(parent, 480, 144, ui)
{
	this->SetText(title);
	this->SetFont(0, 0, 8.25, false);
	
	this->core = core;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	this->pnlButton = ui->NewPanel(*this);
	this->pnlButton->SetRect(0, 0, 100, 32, false);
	this->pnlButton->SetDockType(UI::GUIControl::DOCK_BOTTOM);
	this->btnOK = ui->NewButton(this->pnlButton, CSTR("OK"));
	this->btnOK->SetRect(24, 4, 75, 23, false);
	this->btnOK->HandleButtonClick(OnOKClicked, this);
	this->txtMessage = ui->NewTextBox(*this, msg, true);
	this->txtMessage->SetDockType(UI::GUIControl::DOCK_FILL);
	this->txtMessage->SetReadOnly(true);
	this->txtMessage->Focus();
}

SSWR::AVIRead::AVIRStringMsgForm::~AVIRStringMsgForm()
{
}

void SSWR::AVIRead::AVIRStringMsgForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
