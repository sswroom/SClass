#include "Stdafx.h"
#include "Crypto/Hash/Bcrypt.h"
#include "SSWR/AVIRead/AVIRBCryptForm.h"
#include "UI/MessageDialog.h"

void __stdcall SSWR::AVIRead::AVIRBCryptForm::OnGenHashClicked(void *userObj)
{
	SSWR::AVIRead::AVIRBCryptForm *me = (SSWR::AVIRead::AVIRBCryptForm*)userObj;
	UInt32 cost;
	Text::StringBuilderUTF8 sbCost;
	Text::StringBuilderUTF8 sbPassword;
	me->txtCost->GetText(&sbCost);
	me->txtGenPassword->GetText(&sbPassword);
	if (!sbCost.ToUInt32(&cost))
	{
		UI::MessageDialog::ShowDialog((const UTF8Char*)"Please enter valid cost (4-31)", (const UTF8Char*)"BCrypt", me);
		me->txtCost->Focus();
		return;
	}
	sbCost.ClearStr();
	Crypto::Hash::Bcrypt bcrypt;
	bcrypt.GenHash(&sbCost, cost, sbPassword.ToString());
	me->txtGenHash->SetText(sbCost.ToString());
}

void __stdcall SSWR::AVIRead::AVIRBCryptForm::OnCheckClicked(void *userObj)
{
	SSWR::AVIRead::AVIRBCryptForm *me = (SSWR::AVIRead::AVIRBCryptForm*)userObj;
	Text::StringBuilderUTF8 sbHash;
	Text::StringBuilderUTF8 sbPassword;
	me->txtCheckHash->GetText(&sbHash);
	me->txtCheckPassword->GetText(&sbPassword);
	Crypto::Hash::Bcrypt bcrypt;
	if (bcrypt.Matches(sbHash.ToString(), sbPassword.ToString()))
	{
		me->txtCheckResult->SetText((const UTF8Char*)"Valid");
	}
	else
	{
		me->txtCheckResult->SetText((const UTF8Char*)"Not Valid");
	}
}

SSWR::AVIRead::AVIRBCryptForm::AVIRBCryptForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core) : UI::GUIForm(parent, 800, 320, ui)
{
	this->SetFont(0, 8.25, false);
	this->SetText((const UTF8Char*)"BCrypt");
	this->SetNoResize(true);
	this->core = core;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	NEW_CLASS(this->grpGenerate, UI::GUIGroupBox(ui, this, (const UTF8Char*)"Generate"));
	this->grpGenerate->SetRect(0, 0, 100, 120, false);
	this->grpGenerate->SetDockType(UI::GUIControl::DOCK_TOP);
	NEW_CLASS(this->lblCost, UI::GUILabel(ui, this->grpGenerate, (const UTF8Char*)"Cost"));
	this->lblCost->SetRect(4, 4, 100, 23, false);
	NEW_CLASS(this->txtCost, UI::GUITextBox(ui, this->grpGenerate, (const UTF8Char*)"10"));
	this->txtCost->SetRect(104, 4, 100, 23, false);
	NEW_CLASS(this->lblGenPassword, UI::GUILabel(ui, this->grpGenerate, (const UTF8Char*)"Password"));
	this->lblGenPassword->SetRect(4, 28, 100, 23, false);
	NEW_CLASS(this->txtGenPassword, UI::GUITextBox(ui, this->grpGenerate, (const UTF8Char*)""));
	this->txtGenPassword->SetRect(104, 28, 200, 23, false);
	NEW_CLASS(this->btnGenerate, UI::GUIButton(ui, this->grpGenerate, (const UTF8Char*)"Generate"));
	this->btnGenerate->SetRect(104, 52, 75, 23, false);
	this->btnGenerate->HandleButtonClick(OnGenHashClicked, this);
	NEW_CLASS(this->lblGenHash, UI::GUILabel(ui, this->grpGenerate, (const UTF8Char*)"Hash"));
	this->lblGenHash->SetRect(4, 76, 100, 23, false);
	NEW_CLASS(this->txtGenHash, UI::GUITextBox(ui, this->grpGenerate, (const UTF8Char*)""));
	this->txtGenHash->SetRect(104, 76, 500, 23, false);
	this->txtGenHash->SetReadOnly(true);

	NEW_CLASS(this->grpCheck, UI::GUIGroupBox(ui, this, (const UTF8Char*)"Check"));
	this->grpCheck->SetDockType(UI::GUIControl::DOCK_FILL);
	NEW_CLASS(this->lblCheckHash, UI::GUILabel(ui, this->grpCheck, (const UTF8Char*)"Hash"));
	this->lblCheckHash->SetRect(4, 4, 100, 23, false);
	NEW_CLASS(this->txtCheckHash, UI::GUITextBox(ui, this->grpCheck, (const UTF8Char*)""));
	this->txtCheckHash->SetRect(104, 4, 500, 23, false);
	NEW_CLASS(this->lblCheckPassword, UI::GUILabel(ui, this->grpCheck, (const UTF8Char*)"Password"));
	this->lblCheckPassword->SetRect(4, 28, 100, 23, false);
	NEW_CLASS(this->txtCheckPassword, UI::GUITextBox(ui, this->grpCheck, (const UTF8Char*)""));
	this->txtCheckPassword->SetRect(104, 28, 200, 23, false);
	NEW_CLASS(this->btnCheck, UI::GUIButton(ui, this->grpCheck, (const UTF8Char*)"Check"));
	this->btnCheck->SetRect(104, 52, 75, 23, false);
	this->btnCheck->HandleButtonClick(OnCheckClicked, this);
	NEW_CLASS(this->lblCheckResult, UI::GUILabel(ui, this->grpCheck, (const UTF8Char*)"Result"));
	this->lblCheckResult->SetRect(4, 76, 100, 23, false);
	NEW_CLASS(this->txtCheckResult, UI::GUITextBox(ui, this->grpCheck, (const UTF8Char*)""));
	this->txtCheckResult->SetRect(104, 76, 200, 23, false);
	this->txtCheckResult->SetReadOnly(true);
}

SSWR::AVIRead::AVIRBCryptForm::~AVIRBCryptForm()
{
}

void SSWR::AVIRead::AVIRBCryptForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
