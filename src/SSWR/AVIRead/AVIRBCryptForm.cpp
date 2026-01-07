#include "Stdafx.h"
#include "Crypto/Hash/Bcrypt.h"
#include "SSWR/AVIRead/AVIRBCryptForm.h"

void __stdcall SSWR::AVIRead::AVIRBCryptForm::OnGenHashClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRBCryptForm> me = userObj.GetNN<SSWR::AVIRead::AVIRBCryptForm>();
	UInt32 cost;
	Text::StringBuilderUTF8 sbCost;
	Text::StringBuilderUTF8 sbPassword;
	me->txtCost->GetText(sbCost);
	me->txtGenPassword->GetText(sbPassword);
	if (!sbCost.ToUInt32(cost))
	{
		me->ui->ShowMsgOK(CSTR("Please enter valid cost (4-31)"), CSTR("BCrypt"), me);
		me->txtCost->Focus();
		return;
	}
	if (cost < 4 || cost > 31)
	{
		me->ui->ShowMsgOK(CSTR("Cost must be within 4-31"), CSTR("BCrypt"), me);
		me->txtCost->Focus();
		return;
	}
	sbCost.ClearStr();
	Crypto::Hash::Bcrypt bcrypt;
	bcrypt.GenHash(sbCost, cost, sbPassword.ToCString());
	me->txtGenHash->SetText(sbCost.ToCString());
}

void __stdcall SSWR::AVIRead::AVIRBCryptForm::OnCheckClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRBCryptForm> me = userObj.GetNN<SSWR::AVIRead::AVIRBCryptForm>();
	Text::StringBuilderUTF8 sbHash;
	Text::StringBuilderUTF8 sbPassword;
	me->txtCheckHash->GetText(sbHash);
	me->txtCheckPassword->GetText(sbPassword);
	Crypto::Hash::Bcrypt bcrypt;
	if (bcrypt.Matches(sbHash.ToCString(), sbPassword.ToCString()))
	{
		me->txtCheckResult->SetText(CSTR("Valid"));
	}
	else
	{
		me->txtCheckResult->SetText(CSTR("Not Valid"));
	}
}

SSWR::AVIRead::AVIRBCryptForm::AVIRBCryptForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core) : UI::GUIForm(parent, 800, 320, ui)
{
	this->SetFont(nullptr, 8.25, false);
	this->SetText(CSTR("BCrypt"));
	this->SetNoResize(true);
	this->core = core;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	this->grpGenerate = ui->NewGroupBox(*this, CSTR("Generate"));
	this->grpGenerate->SetRect(0, 0, 100, 120, false);
	this->grpGenerate->SetDockType(UI::GUIControl::DOCK_TOP);
	this->lblCost = ui->NewLabel(this->grpGenerate, CSTR("Cost"));
	this->lblCost->SetRect(4, 4, 100, 23, false);
	this->txtCost = ui->NewTextBox(this->grpGenerate, CSTR("10"));
	this->txtCost->SetRect(104, 4, 100, 23, false);
	this->lblGenPassword = ui->NewLabel(this->grpGenerate, CSTR("Password"));
	this->lblGenPassword->SetRect(4, 28, 100, 23, false);
	this->txtGenPassword = ui->NewTextBox(this->grpGenerate, CSTR(""));
	this->txtGenPassword->SetRect(104, 28, 200, 23, false);
	this->btnGenerate = ui->NewButton(this->grpGenerate, CSTR("Generate"));
	this->btnGenerate->SetRect(104, 52, 75, 23, false);
	this->btnGenerate->HandleButtonClick(OnGenHashClicked, this);
	this->lblGenHash = ui->NewLabel(this->grpGenerate, CSTR("Hash"));
	this->lblGenHash->SetRect(4, 76, 100, 23, false);
	this->txtGenHash = ui->NewTextBox(this->grpGenerate, CSTR(""));
	this->txtGenHash->SetRect(104, 76, 500, 23, false);
	this->txtGenHash->SetReadOnly(true);

	this->grpCheck = ui->NewGroupBox(*this, CSTR("Check"));
	this->grpCheck->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lblCheckHash = ui->NewLabel(this->grpCheck, CSTR("Hash"));
	this->lblCheckHash->SetRect(4, 4, 100, 23, false);
	this->txtCheckHash = ui->NewTextBox(this->grpCheck, CSTR(""));
	this->txtCheckHash->SetRect(104, 4, 500, 23, false);
	this->lblCheckPassword = ui->NewLabel(this->grpCheck, CSTR("Password"));
	this->lblCheckPassword->SetRect(4, 28, 100, 23, false);
	this->txtCheckPassword = ui->NewTextBox(this->grpCheck, CSTR(""));
	this->txtCheckPassword->SetRect(104, 28, 200, 23, false);
	this->btnCheck = ui->NewButton(this->grpCheck, CSTR("Check"));
	this->btnCheck->SetRect(104, 52, 75, 23, false);
	this->btnCheck->HandleButtonClick(OnCheckClicked, this);
	this->lblCheckResult = ui->NewLabel(this->grpCheck, CSTR("Result"));
	this->lblCheckResult->SetRect(4, 76, 100, 23, false);
	this->txtCheckResult = ui->NewTextBox(this->grpCheck, CSTR(""));
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
