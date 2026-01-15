#include "Stdafx.h"
#include "DB/CSVFile.h"
#include "SSWR/AVIRead/AVIRInvestmentAccountForm.h"

#define TITLE CSTR("Account")

void __stdcall SSWR::AVIRead::AVIRInvestmentAccountForm::OnAddClicked(AnyType userObj)
{
	NN<AVIRInvestmentAccountForm> me = userObj.GetNN<AVIRInvestmentAccountForm>();
	Text::StringBuilderUTF8 sb;
	me->txtName->GetText(sb);
	if (sb.leng > 0)
	{
		me->inputName = Text::String::New(sb.ToCString());
		me->SetDialogResult(UI::GUIForm::DR_OK);
	}
}

void __stdcall SSWR::AVIRead::AVIRInvestmentAccountForm::OnCancelClicked(AnyType userObj)
{
	NN<AVIRInvestmentAccountForm> me = userObj.GetNN<AVIRInvestmentAccountForm>();
	me->SetDialogResult(UI::GUIForm::DR_CANCEL);
}

SSWR::AVIRead::AVIRInvestmentAccountForm::AVIRInvestmentAccountForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core) : UI::GUIForm(parent, 640, 120, ui)
{
	this->SetFont(nullptr, 8.25, false);
	this->SetText(TITLE);
	this->SetNoResize(true);

	this->core = core;
	this->inputName = nullptr;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	this->lblName = ui->NewLabel(*this, CSTR("Name"));
	this->lblName->SetRect(4, 4, 100, 23, false);
	this->txtName = ui->NewTextBox(*this, CSTR(""));
	this->txtName->SetRect(104, 4, 200, 23, false);
	this->btnCancel = ui->NewButton(*this, CSTR("Cancel"));
	this->btnCancel->SetRect(104, 28, 75, 23, false);
	this->btnCancel->HandleButtonClick(OnCancelClicked, this);
	this->btnAdd = ui->NewButton(*this, CSTR("Add"));
	this->btnAdd->SetRect(184, 28, 75, 23, false);
	this->btnAdd->HandleButtonClick(OnAddClicked, this);
	this->SetDefaultButton(this->btnAdd);
	this->SetDefaultButton(this->btnCancel);
}

SSWR::AVIRead::AVIRInvestmentAccountForm::~AVIRInvestmentAccountForm()
{
	OPTSTR_DEL(this->inputName);
}

void SSWR::AVIRead::AVIRInvestmentAccountForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
