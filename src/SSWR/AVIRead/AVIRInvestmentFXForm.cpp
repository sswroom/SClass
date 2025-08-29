#include "Stdafx.h"
#include "SSWR/AVIRead/AVIRInvestmentFXForm.h"

#define TITLE CSTR("Investment FX")

void __stdcall SSWR::AVIRead::AVIRInvestmentFXForm::OnOKClicked(AnyType userObj)
{
	NN<AVIRInvestmentFXForm> me = userObj.GetNN<AVIRInvestmentFXForm>();
	UInt32 localC = me->mgr->GetLocalCurrency();
	UInt32 refC = me->mgr->GetRefCurrency();
	UInt32 foreignC = 0;
	UInt32 curr1;
	UInt32 curr2;
	Bool hasLocal = false;
	Text::StringBuilderUTF8 sb;
	me->txtCurrency1->GetText(sb);
	if (sb.leng != 3)
	{
		me->ui->ShowMsgOK(CSTR("Currency1 is not valid"), TITLE, me);
		return;
	}
	curr1 = CURRENCY(sb.v.Ptr());
	if (curr1 == localC)
	{
		hasLocal = true;
	}
	else if (curr1 == refC)
	{
	}
	else
	{
		foreignC = curr1;
	}

	sb.ClearStr();
	me->txtCurrency2->GetText(sb);
	if (sb.leng != 3)
	{
		me->ui->ShowMsgOK(CSTR("Currency2 is not valid"), TITLE, me);
		return;
	}
	curr2 = CURRENCY(sb.v.Ptr());
	if (curr2 == localC)
	{
		hasLocal = true;
	}
	else if (curr2 == refC)
	{
	}
	else if (foreignC != 0)
	{
		me->ui->ShowMsgOK(CSTR("Currency1 and Currency2 cannot be both foreign currency"), TITLE, me);
		return;
	}
	else
	{
		foreignC = curr2;
	}
	Double refRate = 0;
	if (foreignC != 0 && hasLocal)
	{
		sb.ClearStr();
		me->txtRefRate->GetText(sb);
		if (!sb.ToDouble(refRate))
		{
			me->ui->ShowMsgOK(CSTR("Ref Rate is not valid"), TITLE, me);
			return;
		}
	}
	Double val1;
	Double val2;
	sb.ClearStr();
	me->txtValue1->GetText(sb);
	if (!sb.ToDouble(val1))
	{
		me->ui->ShowMsgOK(CSTR("Currency value1 is not valid"), TITLE, me);
		return;
	}
	sb.ClearStr();
	me->txtValue2->GetText(sb);
	if (!sb.ToDouble(val2))
	{
		me->ui->ShowMsgOK(CSTR("Currency value2 is not valid"), TITLE, me);
		return;
	}
	sb.ClearStr();
	me->txtDate->GetText(sb);
	Data::Date dt = Data::Date(sb.ToCString());
	if (dt.IsNull())
	{
		me->ui->ShowMsgOK(CSTR("Date is not valid"), TITLE, me);
		return;
	}
	if (me->mgr->AddTransactionFX(Data::Timestamp::FromDate(dt, Data::DateTimeUtil::GetLocalTzQhr()), curr1, val1, curr2, val2, refRate))
	{
		me->SetDialogResult(UI::GUIForm::DR_OK);
	}
	else
	{
		me->ui->ShowMsgOK(CSTR("Error in adding transaction"), TITLE, me);
	}
}

void __stdcall SSWR::AVIRead::AVIRInvestmentFXForm::OnCancelClicked(AnyType userObj)
{
	NN<AVIRInvestmentFXForm> me = userObj.GetNN<AVIRInvestmentFXForm>();
	me->SetDialogResult(UI::GUIForm::DR_CANCEL);
}

void __stdcall SSWR::AVIRead::AVIRInvestmentFXForm::OnCurrencyChanged(AnyType userObj)
{
	NN<AVIRInvestmentFXForm> me = userObj.GetNN<AVIRInvestmentFXForm>();
	UInt32 localC = me->mgr->GetLocalCurrency();
	UInt32 refC = me->mgr->GetRefCurrency();
	UInt32 foreignC = 0;
	Bool hasLocal = false;
//	Bool hasRef = false;
	Text::StringBuilderUTF8 sb;
	me->txtCurrency1->GetText(sb);
	if (sb.leng != 3)
	{
		me->txtRefRateName->SetText(CSTR(""));
		return;
	}
	UInt32 c = CURRENCY(sb.v.Ptr());
	if (c == localC)
	{
		hasLocal = true;
	}
	else if (c == refC)
	{
//		hasRef = true;
	}
	else
	{
		foreignC = c;
	}

	sb.ClearStr();
	me->txtCurrency2->GetText(sb);
	if (sb.leng != 3)
	{
		me->txtRefRateName->SetText(CSTR(""));
		return;
	}
	c = CURRENCY(sb.v.Ptr());
	if (c == localC)
	{
		hasLocal = true;
	}
	else if (c == refC)
	{
//		hasRef = true;
	}
	else if (foreignC != 0)
	{
		me->txtRefRateName->SetText(CSTR(""));
		return;
	}
	else
	{
		foreignC = c;
	}

	if (foreignC != 0 && hasLocal)
	{
		sb.ClearStr();
		sb.Append(CURRENCYSTR(refC));
		sb.AppendUTF8Char('/');
		sb.Append(CURRENCYSTR(foreignC));
		me->txtRefRateName->SetText(sb.ToCString());
	}
	else
	{
		me->txtRefRateName->SetText(CSTR(""));
	}
}

SSWR::AVIRead::AVIRInvestmentFXForm::AVIRInvestmentFXForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core, NN<Data::Invest::InvestmentManager> mgr) : UI::GUIForm(parent, 1024, 300, ui)
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
	this->lblCurrency1 = ui->NewLabel(*this, CSTR("Currency1"));
	this->lblCurrency1->SetRect(4, 28, 100, 23, false);
	UInt32 c = this->mgr->GetLocalCurrency();
	this->txtCurrency1 = ui->NewTextBox(*this, CURRENCYSTR(c));
	this->txtCurrency1->SetRect(104, 28, 50, 23, false);
	this->txtCurrency1->HandleTextChanged(OnCurrencyChanged, this);
	this->txtValue1 = ui->NewTextBox(*this, CSTR("0.0"));
	this->txtValue1->SetRect(154, 28, 100, 23, false);
	this->lblCurrency2 = ui->NewLabel(*this, CSTR("Currency2"));
	this->lblCurrency2->SetRect(4, 52, 100, 23, false);
	c = this->mgr->GetRefCurrency();
	this->txtCurrency2 = ui->NewTextBox(*this, CURRENCYSTR(c));
	this->txtCurrency2->SetRect(104, 52, 50, 23, false);
	this->txtCurrency2->HandleTextChanged(OnCurrencyChanged, this);
	this->txtValue2 = ui->NewTextBox(*this, CSTR("0.0"));
	this->txtValue2->SetRect(154, 52, 100, 23, false);
	this->lblRefRate = ui->NewLabel(*this, CSTR("Ref Rate"));
	this->lblRefRate->SetRect(4, 76, 100, 23, false);
	this->txtRefRateName = ui->NewTextBox(*this, CSTR(""));
	this->txtRefRateName->SetRect(104, 76, 100, 23, false);
	this->txtRefRateName->SetReadOnly(true);
	this->txtRefRate = ui->NewTextBox(*this, CSTR("0.0"));
	this->txtRefRate->SetRect(204, 76, 100, 23, false);
	this->btnCancel = ui->NewButton(*this, CSTR("Cancel"));
	this->btnCancel->SetRect(104, 100, 75, 23, false);
	this->btnCancel->HandleButtonClick(OnCancelClicked, this);
	this->btnOK = ui->NewButton(*this, CSTR("OK"));
	this->btnOK->SetRect(184, 100, 75, 23, false);
	this->btnOK->HandleButtonClick(OnOKClicked, this);
	
	this->SetDefaultButton(this->btnOK);
	this->SetDefaultButton(this->btnCancel);
}

SSWR::AVIRead::AVIRInvestmentFXForm::~AVIRInvestmentFXForm()
{
}

void SSWR::AVIRead::AVIRInvestmentFXForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
