#include "Stdafx.h"
#include "SSWR/AVIRead/AVIRInvestmentAInterestForm.h"

#define TITLE CSTR("Investment Asset Interest")

void __stdcall SSWR::AVIRead::AVIRInvestmentAInterestForm::OnOKClicked(AnyType userObj)
{
	NN<AVIRInvestmentAInterestForm> me = userObj.GetNN<AVIRInvestmentAInterestForm>();
	NN<Data::Invest::Asset> ass;
	if (!me->cboAsset->GetSelectedItem().GetOpt<Data::Invest::Asset>().SetTo(ass))
	{
		me->ui->ShowMsgOK(CSTR("Please select an asset"), TITLE, me);
		return;
	}
	Double currencyValue;
	Text::StringBuilderUTF8 sb;
	me->txtCurrencyValue->GetText(sb);
	if (!sb.ToDouble(currencyValue))
	{
		me->ui->ShowMsgOK(CSTR("Please input valid value"), TITLE, me);
		return;
	}

	Data::Date startDate;
	Data::Date endDate;
	sb.ClearStr();
	me->txtStartDate->GetText(sb);
	startDate = Data::Date(sb.ToCString());
	if (startDate.IsNull())
	{
		me->ui->ShowMsgOK(CSTR("Please enter valid start date"), TITLE, me);
		return;
	}
	sb.ClearStr();
	me->txtEndDate->GetText(sb);
	endDate = Data::Date(sb.ToCString());
	if (endDate.IsNull() && sb.leng > 0)
	{
		me->ui->ShowMsgOK(CSTR("Please enter valid end date"), TITLE, me);
		return;
	}
	if (!endDate.IsNull())
	{
		if (startDate > endDate)
		{
			me->ui->ShowMsgOK(CSTR("End Date cannot before start date"), TITLE, me);
			return;
		}
	}
	if (me->mgr->AddTransactionAInterest(Data::Timestamp::FromDate(startDate, Data::DateTimeUtil::GetLocalTzQhr()),
		Data::Timestamp::FromDate(endDate, Data::DateTimeUtil::GetLocalTzQhr()),
		ass->index, currencyValue))
	{
		me->SetDialogResult(UI::GUIForm::DR_OK);
	}
	else
	{
		me->ui->ShowMsgOK(CSTR("Error in adding transaction"), TITLE, me);
	}
}

void __stdcall SSWR::AVIRead::AVIRInvestmentAInterestForm::OnCancelClicked(AnyType userObj)
{
	NN<AVIRInvestmentAInterestForm> me = userObj.GetNN<AVIRInvestmentAInterestForm>();
	me->SetDialogResult(UI::GUIForm::DR_CANCEL);
}

void __stdcall SSWR::AVIRead::AVIRInvestmentAInterestForm::OnAssetSelChg(AnyType userObj)
{
	NN<AVIRInvestmentAInterestForm> me = userObj.GetNN<AVIRInvestmentAInterestForm>();
	NN<Data::Invest::Asset> ass;
	if (me->cboAsset->GetSelectedItem().GetOpt<Data::Invest::Asset>().SetTo(ass))
	{
		Text::StringBuilderUTF8 sb;
		sb.Append(CSTR("Value ("));
		sb.Append(CURRENCYSTR(ass->currency));
		sb.AppendUTF8Char(')');
		me->lblCurrencyValue->SetText(sb.ToCString());
	}
}

SSWR::AVIRead::AVIRInvestmentAInterestForm::AVIRInvestmentAInterestForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core, NN<Data::Invest::InvestmentManager> mgr) : UI::GUIForm(parent, 1024, 300, ui)
{
	UTF8Char sbuff[64];
	UnsafeArray<UTF8Char> sptr;
	this->SetFont(0, 0, 8.25, false);
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
	this->txtEndDate = ui->NewTextBox(*this, CSTR(""));
	this->txtEndDate->SetRect(104, 28, 150, 23, false);
	this->lblAsset = ui->NewLabel(*this, CSTR("Asset"));
	this->lblAsset->SetRect(4, 52, 100, 23, false);
	this->cboAsset = ui->NewComboBox(*this, false);
	this->cboAsset->SetRect(104, 52, 50, 23, false);
	this->cboAsset->HandleSelectionChange(OnAssetSelChg, this);
	this->lblCurrencyValue = ui->NewLabel(*this, CSTR("Value"));
	this->lblCurrencyValue->SetRect(4, 76, 100, 23, false);
	this->txtCurrencyValue = ui->NewTextBox(*this, CSTR("0.0"));
	this->txtCurrencyValue->SetRect(104, 76, 100, 23, false);
	this->btnCancel = ui->NewButton(*this, CSTR("Cancel"));
	this->btnCancel->SetRect(104, 100, 75, 23, false);
	this->btnCancel->HandleButtonClick(OnCancelClicked, this);
	this->btnOK = ui->NewButton(*this, CSTR("OK"));
	this->btnOK->SetRect(184, 100, 75, 23, false);
	this->btnOK->HandleButtonClick(OnOKClicked, this);
	NN<Data::Invest::Asset> ass;
	UOSInt i = 0;
	UOSInt j = mgr->GetAssetCount();
	while (i < j)
	{
		if (mgr->GetAsset(i).SetTo(ass))
		{
			this->cboAsset->AddItem(ass->shortName, ass);
		}
		i++;
	}
	if (j > 0)
	{
		this->cboAsset->SetSelectedIndex(0);
		OnAssetSelChg(this);
	}
	
	this->SetDefaultButton(this->btnOK);
	this->SetDefaultButton(this->btnCancel);
}

SSWR::AVIRead::AVIRInvestmentAInterestForm::~AVIRInvestmentAInterestForm()
{
}

void SSWR::AVIRead::AVIRInvestmentAInterestForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
