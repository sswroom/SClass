#include "Stdafx.h"
#include "IO/EXEFile.h"
#include "IO/Path.h"
#include "SSWR/AVIRead/AVIRStringMsgForm.h"
#include "Text/MyString.h"
#include "UI/GUIButton.h"
#include "UI/GUILabel.h"

void __stdcall SSWR::AVIRead::AVIRStringMsgForm::OnOKClicked(void *userObj)
{
	SSWR::AVIRead::AVIRStringMsgForm *me = (SSWR::AVIRead::AVIRStringMsgForm*)userObj;
	me->Close();
}

SSWR::AVIRead::AVIRStringMsgForm::AVIRStringMsgForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core, const UTF8Char *title, Text::CString msg) : UI::GUIForm(parent, 480, 144, ui)
{
	this->SetText(title);
	this->SetFont(0, 0, 8.25, false);
	
	this->core = core;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	NEW_CLASS(this->pnlButton, UI::GUIPanel(ui, this));
	this->pnlButton->SetRect(0, 0, 100, 32, false);
	this->pnlButton->SetDockType(UI::GUIControl::DOCK_BOTTOM);
	NEW_CLASS(this->btnOK, UI::GUIButton(ui, this->pnlButton, (const UTF8Char*)"OK"));
	this->btnOK->SetRect(24, 4, 75, 23, false);
	this->btnOK->HandleButtonClick(OnOKClicked, this);
	NEW_CLASS(this->txtMessage, UI::GUITextBox(ui, this, msg, true));
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
