#include "Stdafx.h"
#include "Data/Invest/InvestmentManager.h"
#include "DB/CSVFile.h"
#include "SSWR/AVIRead/AVIRInvestmentAssetForm.h"

#define TITLE CSTR("Asset")

void __stdcall SSWR::AVIRead::AVIRInvestmentAssetForm::OnAddClicked(AnyType userObj)
{
	NN<AVIRInvestmentAssetForm> me = userObj.GetNN<AVIRInvestmentAssetForm>();
	Text::StringBuilderUTF8 sb;
	me->txtCurrency->GetText(sb);
	if (sb.leng != 3 || !sb.IsBlockLetters())
	{
		me->ui->ShowMsgOK(CSTR("Currency must be 3 capital characters"), TITLE, me);
		return;
	}
	me->currency = CURRENCY(sb.v.Ptr());
	sb.ClearStr();
	me->txtShortName->GetText(sb);
	OPTSTR_DEL(me->shortName);
	if (sb.leng <= 0)
	{
		me->ui->ShowMsgOK(CSTR("Please input short name"), TITLE, me);
		return;
	}
	me->shortName = Text::String::New(sb.ToCString());
	sb.ClearStr();
	me->txtFullName->GetText(sb);
	OPTSTR_DEL(me->fullName);
	if (sb.leng <= 0)
	{
		me->ui->ShowMsgOK(CSTR("Please input full name"), TITLE, me);
		return;
	}
	me->fullName = Text::String::New(sb.ToCString());
	me->SetDialogResult(UI::GUIForm::DR_OK);
}

void __stdcall SSWR::AVIRead::AVIRInvestmentAssetForm::OnCancelClicked(AnyType userObj)
{
	NN<AVIRInvestmentAssetForm> me = userObj.GetNN<AVIRInvestmentAssetForm>();
	me->SetDialogResult(UI::GUIForm::DR_CANCEL);
}

SSWR::AVIRead::AVIRInvestmentAssetForm::AVIRInvestmentAssetForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core) : UI::GUIForm(parent, 640, 160, ui)
{
	this->SetFont(nullptr, 8.25, false);
	this->SetText(TITLE);
	this->SetNoResize(true);

	this->core = core;
	this->shortName = 0;
	this->fullName = 0;
	this->currency = 0;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	this->lblShortName = ui->NewLabel(*this, CSTR("Short Name"));
	this->lblShortName->SetRect(4, 4, 100, 23, false);
	this->txtShortName = ui->NewTextBox(*this, CSTR(""));
	this->txtShortName->SetRect(104, 4, 150, 23, false);
	this->lblFullName = ui->NewLabel(*this, CSTR("Full Name"));
	this->lblFullName->SetRect(4, 28, 100, 23, false);
	this->txtFullName = ui->NewTextBox(*this, CSTR(""));
	this->txtFullName->SetRect(104, 28, 200, 23, false);
	this->lblCurrency = ui->NewLabel(*this, CSTR("Currency"));
	this->lblCurrency->SetRect(4, 52, 100, 23, false);
	this->txtCurrency = ui->NewTextBox(*this, CSTR("USD"));
	this->txtCurrency->SetRect(104, 52, 50, 23, false);
	this->btnCancel = ui->NewButton(*this, CSTR("Cancel"));
	this->btnCancel->SetRect(104, 76, 75, 23, false);
	this->btnCancel->HandleButtonClick(OnCancelClicked, this);
	this->btnAdd = ui->NewButton(*this, CSTR("Add"));
	this->btnAdd->SetRect(184, 76, 75, 23, false);
	this->btnAdd->HandleButtonClick(OnAddClicked, this);
	this->SetDefaultButton(this->btnAdd);
	this->SetDefaultButton(this->btnCancel);
}

SSWR::AVIRead::AVIRInvestmentAssetForm::~AVIRInvestmentAssetForm()
{
	OPTSTR_DEL(this->shortName);
	OPTSTR_DEL(this->fullName);
}

void SSWR::AVIRead::AVIRInvestmentAssetForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
