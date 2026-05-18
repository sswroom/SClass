#include "Stdafx.h"
#include "SSWR/AVIRead/AVIRKeyTestForm.h"

UI::EventState __stdcall SSWR::AVIRead::AVIRKeyTestForm::OnKeyInputKeyDown(AnyType userObj, UInt32 osKey)
{
	NN<SSWR::AVIRead::AVIRKeyTestForm> me = userObj.GetNN<SSWR::AVIRead::AVIRKeyTestForm>();
	UTF8Char sbuff[32];
	UnsafeArray<UTF8Char> sptr;
	UI::GUIControl::GUIKey key = UI::GUIControl::OSKey2GUIKey(osKey);
	me->txtGUIKey->SetText(UI::GUIControl::GUIKeyGetName(key));
	sptr = Text::StrUInt32(sbuff, osKey);
	me->txtOSKey->SetText(CSTRP(sbuff, sptr));
	sptr = Text::StrHexVal32V(sbuff, osKey);
	me->txtOSKeyHex->SetText(CSTRP(sbuff, sptr));
	return UI::EventState::StopEvent;
}

SSWR::AVIRead::AVIRKeyTestForm::AVIRKeyTestForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core) : UI::GUIForm(parent, 640, 240, ui)
{
	this->SetFont(nullptr, 8.25, false);
	this->SetText(CSTR("Key Test"));

	this->core = core;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
	this->SetNoResize(true);

	this->lblKeyInput = ui->NewLabel(*this, CSTR("Key Input"));
	this->lblKeyInput->SetRect(4, 4, 100, 23, false);
	this->txtKeyInput = ui->NewTextBox(*this, CSTR(""));
	this->txtKeyInput->SetRect(104, 4, 200, 23, false);
	this->txtKeyInput->HandleKeyDown(OnKeyInputKeyDown, this);
	this->lblGUIKey = ui->NewLabel(*this, CSTR("GUI Key"));
	this->lblGUIKey->SetRect(4, 28, 100, 23, false);
	this->txtGUIKey = ui->NewTextBox(*this, CSTR(""));
	this->txtGUIKey->SetRect(104, 28, 200, 23, false);
	this->txtGUIKey->SetReadOnly(true);
	this->lblOSKey = ui->NewLabel(*this, CSTR("OS Key"));
	this->lblOSKey->SetRect(4, 52, 100, 23, false);
	this->txtOSKey = ui->NewTextBox(*this, CSTR(""));
	this->txtOSKey->SetRect(104, 52, 200, 23, false);
	this->txtOSKey->SetReadOnly(true);
	this->lblOSKeyHex = ui->NewLabel(*this, CSTR("OS Key (Hex)"));
	this->lblOSKeyHex->SetRect(4, 76, 100, 23, false);
	this->txtOSKeyHex = ui->NewTextBox(*this, CSTR(""));
	this->txtOSKeyHex->SetRect(104, 76, 200, 23, false);
	this->txtOSKeyHex->SetReadOnly(true);
}

SSWR::AVIRead::AVIRKeyTestForm::~AVIRKeyTestForm()
{
}

void SSWR::AVIRead::AVIRKeyTestForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
