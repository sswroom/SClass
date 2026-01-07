#include "Stdafx.h"
#include "SSWR/AVIRead/AVIRInvestmentDepositForm.h"

#define TITLE CSTR("Investment Deposit")

void __stdcall SSWR::AVIRead::AVIRInvestmentDepositForm::OnOKClicked(AnyType userObj)
{
	NN<AVIRInvestmentDepositForm> me = userObj.GetNN<AVIRInvestmentDepositForm>();
	UInt32 curr;
	Text::StringBuilderUTF8 sb;
	me->txtCurrency->GetText(sb);
	if (sb.leng != 3)
	{
		me->ui->ShowMsgOK(CSTR("Currency is not valid"), TITLE, me);
		return;
	}
	curr = CURRENCY(sb.v.Ptr());

	Data::Date startDate;
	Data::Date endDate;
	Double startAmount;
	Double endAmount;
	sb.ClearStr();
	me->txtStartDate->GetText(sb);
	startDate = Data::Date(sb.ToCString());
	sb.ClearStr();
	me->txtEndDate->GetText(sb);
	endDate = Data::Date(sb.ToCString());
	if (startDate.IsNull())
	{
		me->ui->ShowMsgOK(CSTR("Please enter valid start date"), TITLE, me);
		return;
	}
	if (endDate.IsNull())
	{
		me->ui->ShowMsgOK(CSTR("Please enter valid end date"), TITLE, me);
		return;
	}
	if (endDate <= startDate)
	{
		me->ui->ShowMsgOK(CSTR("End date must be after start date"), TITLE, me);
		return;
	}
	sb.ClearStr();
	me->txtStartAmount->GetText(sb);
	if (!sb.ToDouble(startAmount) || startAmount <= 0.0)
	{
		me->ui->ShowMsgOK(CSTR("Start amount is not valid"), TITLE, me);
		return;
	}
	sb.ClearStr();
	me->txtEndAmount->GetText(sb);
	if (!sb.ToDouble(endAmount) || endAmount <= 0.0)
	{
		me->ui->ShowMsgOK(CSTR("End amount is not valid"), TITLE, me);
		return;
	}
	else if (endAmount <= startAmount)
	{
		me->ui->ShowMsgOK(CSTR("End amount must be larger than start amount"), TITLE, me);
		return;
	}
	if (me->mgr->AddTransactionDeposit(Data::Timestamp::FromDate(startDate, Data::DateTimeUtil::GetLocalTzQhr()), Data::Timestamp::FromDate(endDate, Data::DateTimeUtil::GetLocalTzQhr()), curr, startAmount, endAmount))
	{
		me->SetDialogResult(UI::GUIForm::DR_OK);
	}
	else
	{
		me->ui->ShowMsgOK(CSTR("Error in adding transaction"), TITLE, me);
	}
}

void __stdcall SSWR::AVIRead::AVIRInvestmentDepositForm::OnCancelClicked(AnyType userObj)
{
	NN<AVIRInvestmentDepositForm> me = userObj.GetNN<AVIRInvestmentDepositForm>();
	me->SetDialogResult(UI::GUIForm::DR_CANCEL);
}

SSWR::AVIRead::AVIRInvestmentDepositForm::AVIRInvestmentDepositForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core, NN<Data::Invest::InvestmentManager> mgr) : UI::GUIForm(parent, 1024, 300, ui)
{
	UTF8Char sbuff[64];
	UnsafeArray<UTF8Char> sptr;
	this->SetFont(nullptr, 8.25, false);
	this->SetText(TITLE);
	this->SetNoResize(true);

	this->core = core;
	this->mgr = mgr;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	this->lblStartDate = ui->NewLabel(*this, CSTR("Start Date"));
	this->lblStartDate->SetRect(4, 4, 100, 23, false);
	sptr = Data::Date::Today().ToString(sbuff);
	this->txtStartDate = ui->NewTextBox(*this, CSTRP(sbuff, sptr));
	this->txtStartDate->SetRect(104, 4, 150, 23, false);
	this->lblEndDate = ui->NewLabel(*this, CSTR("End Date"));
	this->lblEndDate->SetRect(4, 28, 100, 23, false);
	this->txtEndDate = ui->NewTextBox(*this, CSTRP(sbuff, sptr));
	this->txtEndDate->SetRect(104, 28, 150, 23, false);
	this->lblCurrency = ui->NewLabel(*this, CSTR("Currency"));
	this->lblCurrency->SetRect(4, 52, 100, 23, false);
	UInt32 c = this->mgr->GetRefCurrency();
	this->txtCurrency = ui->NewTextBox(*this, CURRENCYSTR(c));
	this->txtCurrency->SetRect(104, 52, 50, 23, false);
	this->lblStartAmount = ui->NewLabel(*this, CSTR("Start Amount"));
	this->lblStartAmount->SetRect(4, 76, 100, 23, false);
	this->txtStartAmount = ui->NewTextBox(*this, CSTR("0.0"));
	this->txtStartAmount->SetRect(104, 76, 100, 23, false);
	this->lblEndAmount = ui->NewLabel(*this, CSTR("End Amount"));
	this->lblEndAmount->SetRect(4, 100, 100, 23, false);
	this->txtEndAmount = ui->NewTextBox(*this, CSTR("0.0"));
	this->txtEndAmount->SetRect(104, 100, 100, 23, false);
	this->btnCancel = ui->NewButton(*this, CSTR("Cancel"));
	this->btnCancel->SetRect(104, 124, 75, 23, false);
	this->btnCancel->HandleButtonClick(OnCancelClicked, this);
	this->btnOK = ui->NewButton(*this, CSTR("OK"));
	this->btnOK->SetRect(184, 124, 75, 23, false);
	this->btnOK->HandleButtonClick(OnOKClicked, this);
	
	this->SetDefaultButton(this->btnOK);
	this->SetDefaultButton(this->btnCancel);
}

SSWR::AVIRead::AVIRInvestmentDepositForm::~AVIRInvestmentDepositForm()
{
}

void SSWR::AVIRead::AVIRInvestmentDepositForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
