#include "Stdafx.h"
#include "SSWR/AVIRead/AVIRInvestmentCInterestForm.h"

#define TITLE CSTR("Investment Interest")

void __stdcall SSWR::AVIRead::AVIRInvestmentCInterestForm::OnOKClicked(AnyType userObj)
{
	NN<AVIRInvestmentCInterestForm> me = userObj.GetNN<AVIRInvestmentCInterestForm>();
	Data::Date dt;
	UInt32 curr;
	Double amount;
	Text::StringBuilderUTF8 sb;
	me->txtDate->GetText(sb);
	dt = Data::Date(sb.ToCString());
	if (dt.IsNull())
	{
		me->ui->ShowMsgOK(CSTR("Please enter valid date"), TITLE, me);
		return;
	}
	sb.ClearStr();
	me->txtCurrency->GetText(sb);
	if (sb.leng != 3)
	{
		me->ui->ShowMsgOK(CSTR("Please input valid currency"), TITLE, me);
		return;
	}
	curr = CURRENCY(sb.v.Ptr());
	sb.ClearStr();
	me->txtAmount->GetText(sb);
	if (!sb.ToDouble(amount) || amount <= 0)
	{
		me->ui->ShowMsgOK(CSTR("Please input valid amount"), TITLE, me);
		return;
	}
	if (me->mgr->AddTransactionCInterest(Data::Timestamp::FromDate(dt, Data::DateTimeUtil::GetLocalTzQhr()), curr, amount))
	{
		me->SetDialogResult(UI::GUIForm::DR_OK);
	}
	else
	{
		me->ui->ShowMsgOK(CSTR("Error in adding transaction"), TITLE, me);
	}
}

void __stdcall SSWR::AVIRead::AVIRInvestmentCInterestForm::OnCancelClicked(AnyType userObj)
{
	NN<AVIRInvestmentCInterestForm> me = userObj.GetNN<AVIRInvestmentCInterestForm>();
	me->SetDialogResult(UI::GUIForm::DR_CANCEL);
}

SSWR::AVIRead::AVIRInvestmentCInterestForm::AVIRInvestmentCInterestForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core, NN<Data::Invest::InvestmentManager> mgr) : UI::GUIForm(parent, 1024, 300, ui)
{
	UTF8Char sbuff[64];
	UnsafeArray<UTF8Char> sptr;
	this->SetFont(0, 0, 8.25, false);
	this->SetText(TITLE);
	this->SetNoResize(true);

	this->core = core;
	this->mgr = mgr;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	this->lblDate = ui->NewLabel(*this, CSTR("Date"));
	this->lblDate->SetRect(4, 4, 100, 23, false);
	sptr = Data::Date::Today().ToString(sbuff);
	this->txtDate = ui->NewTextBox(*this, CSTRP(sbuff, sptr));
	this->txtDate->SetRect(104, 4, 150, 23, false);
	this->lblCurrency = ui->NewLabel(*this, CSTR("Currency"));
	this->lblCurrency->SetRect(4, 28, 100, 23, false);
	UInt32 c = mgr->GetRefCurrency();
	this->txtCurrency = ui->NewTextBox(*this, CURRENCYSTR(c));
	this->txtCurrency->SetRect(104, 28, 150, 23, false);
	this->lblAmount = ui->NewLabel(*this, CSTR("Amount"));
	this->lblAmount->SetRect(4, 52, 100, 23, false);
	this->txtAmount = ui->NewTextBox(*this, CSTR("0"));
	this->txtAmount->SetRect(104, 52, 100, 23, false);
	this->btnCancel = ui->NewButton(*this, CSTR("Cancel"));
	this->btnCancel->SetRect(104, 76, 75, 23, false);
	this->btnCancel->HandleButtonClick(OnCancelClicked, this);
	this->btnOK = ui->NewButton(*this, CSTR("OK"));
	this->btnOK->SetRect(184, 76, 75, 23, false);
	this->btnOK->HandleButtonClick(OnOKClicked, this);
	this->SetDefaultButton(this->btnOK);
	this->SetDefaultButton(this->btnCancel);
}

SSWR::AVIRead::AVIRInvestmentCInterestForm::~AVIRInvestmentCInterestForm()
{
}

void SSWR::AVIRead::AVIRInvestmentCInterestForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
