#include "Stdafx.h"
#include "SSWR/AVIRead/AVIRInvestmentAInterestForm.h"

#define TITLE CSTR("Investment Asset Interest")

void __stdcall SSWR::AVIRead::AVIRInvestmentAInterestForm::OnOKClicked(AnyType userObj)
{
	NN<AVIRInvestmentAInterestForm> me = userObj.GetNN<AVIRInvestmentAInterestForm>();
	NN<Data::Invest::Asset> ass;
	Double currencyValue;
	Data::Date startDate;
	Data::Date endDate;
	NN<Data::Invest::TradeEntry> ent;
	if (me->ent.SetTo(ent))
	{
		Text::StringBuilderUTF8 sb;
		me->txtCurrencyValue->GetText(sb);
		if (!sb.ToDouble(currencyValue))
		{
			me->ui->ShowMsgOK(CSTR("Please input valid value"), TITLE, me);
			return;
		}
		startDate = ent->fromDetail.tranBeginDate.ToDate();
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
		if (me->mgr->UpdateTransactionAInterest(ent, Data::Timestamp::FromDate(endDate, Data::DateTimeUtil::GetLocalTzQhr()), currencyValue))
		{
			me->SetDialogResult(UI::GUIForm::DR_OK);
		}
		else
		{
			me->ui->ShowMsgOK(CSTR("Error in updating transaction"), TITLE, me);
		}
	}
	else
	{
		if (!me->cboAsset->GetSelectedItem().GetOpt<Data::Invest::Asset>().SetTo(ass))
		{
			me->ui->ShowMsgOK(CSTR("Please select an asset"), TITLE, me);
			return;
		}
		Text::StringBuilderUTF8 sb;
		me->txtCurrencyValue->GetText(sb);
		if (!sb.ToDouble(currencyValue))
		{
			me->ui->ShowMsgOK(CSTR("Please input valid value"), TITLE, me);
			return;
		}

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
		OnStartDateChg(me);
		OnCurrencyValueChg(me);
	}
}

void __stdcall SSWR::AVIRead::AVIRInvestmentAInterestForm::OnStartDateChg(AnyType userObj)
{
	NN<AVIRInvestmentAInterestForm> me = userObj.GetNN<AVIRInvestmentAInterestForm>();
	NN<Data::Invest::Asset> ass;
	if (me->cboAsset->GetSelectedItem().GetOpt<Data::Invest::Asset>().SetTo(ass))
	{
		Text::StringBuilderUTF8 sb;
		me->txtStartDate->GetText(sb);
		Data::Date dt = Data::Date(sb.ToCString());
		if (dt.IsNull())
		{
			me->txtAssetAmount->SetText(CSTR(""));
		}
		else
		{
			sb.ClearStr();
			sb.AppendDouble(me->mgr->AssetGetAmount(ass, Data::Timestamp::FromDate(dt, Data::DateTimeUtil::GetLocalTzQhr())));
			me->txtAssetAmount->SetText(sb.ToCString());
		}
	}
	else
	{
		me->txtAssetAmount->SetText(CSTR(""));
	}
}

void __stdcall SSWR::AVIRead::AVIRInvestmentAInterestForm::OnCurrencyValueChg(AnyType userObj)
{
	NN<AVIRInvestmentAInterestForm> me = userObj.GetNN<AVIRInvestmentAInterestForm>();
	Text::StringBuilderUTF8 sb;
	Double amount;
	Double value;
	me->txtAssetAmount->GetText(sb);
	if (!sb.ToDouble(amount))
	{
		me->txtAssetDiv->SetText(CSTR(""));
		return;
	}
	sb.ClearStr();
	me->txtCurrencyValue->GetText(sb);
	if (!sb.ToDouble(value))
	{
		me->txtAssetDiv->SetText(CSTR(""));
		return;
	}
	sb.ClearStr();
	sb.AppendDouble(value / amount);
	me->txtAssetDiv->SetText(sb.ToCString());
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
	this->ent = 0;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	this->lblStartDate = ui->NewLabel(*this, CSTR("Start Date"));
	this->lblStartDate->SetRect(4, 4, 100, 23, false);
	sptr = Data::Date::Today().ToString(sbuff);
	this->txtStartDate = ui->NewTextBox(*this, CSTRP(sbuff, sptr));
	this->txtStartDate->SetRect(104, 4, 150, 23, false);
	this->txtStartDate->HandleTextChanged(OnStartDateChg, this);
	this->lblEndDate = ui->NewLabel(*this, CSTR("End Date"));
	this->lblEndDate->SetRect(4, 28, 100, 23, false);
	this->txtEndDate = ui->NewTextBox(*this, CSTR(""));
	this->txtEndDate->SetRect(104, 28, 150, 23, false);
	this->lblAsset = ui->NewLabel(*this, CSTR("Asset"));
	this->lblAsset->SetRect(4, 52, 100, 23, false);
	this->cboAsset = ui->NewComboBox(*this, false);
	this->cboAsset->SetRect(104, 52, 50, 23, false);
	this->cboAsset->HandleSelectionChange(OnAssetSelChg, this);
	this->lblAssetAmount = ui->NewLabel(*this, CSTR("Amount"));
	this->lblAssetAmount->SetRect(4, 76, 100, 23, false);
	this->txtAssetAmount = ui->NewTextBox(*this, CSTR(""));
	this->txtAssetAmount->SetRect(104, 76, 100, 23, false);
	this->txtAssetAmount->SetReadOnly(true);
	this->lblCurrencyValue = ui->NewLabel(*this, CSTR("Value"));
	this->lblCurrencyValue->SetRect(4, 100, 100, 23, false);
	this->txtCurrencyValue = ui->NewTextBox(*this, CSTR("0.0"));
	this->txtCurrencyValue->SetRect(104, 100, 100, 23, false);
	this->txtCurrencyValue->HandleTextChanged(OnCurrencyValueChg, this);
	this->lblAssetDiv = ui->NewLabel(*this, CSTR("Div"));
	this->lblAssetDiv->SetRect(4, 124, 100, 23, false);
	this->txtAssetDiv = ui->NewTextBox(*this, CSTR(""));
	this->txtAssetDiv->SetRect(104, 124, 100, 23, false);
	this->txtAssetDiv->SetReadOnly(true);
	this->btnCancel = ui->NewButton(*this, CSTR("Cancel"));
	this->btnCancel->SetRect(104, 148, 75, 23, false);
	this->btnCancel->HandleButtonClick(OnCancelClicked, this);
	this->btnOK = ui->NewButton(*this, CSTR("OK"));
	this->btnOK->SetRect(184, 148, 75, 23, false);
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

void SSWR::AVIRead::AVIRInvestmentAInterestForm::SetEntry(NN<Data::Invest::TradeEntry> ent)
{
	if (ent->type == Data::Invest::TradeType::AssetInterest)
	{
		UTF8Char sbuff[64];
		UnsafeArray<UTF8Char> sptr;
		this->txtStartDate->SetReadOnly(true);
		sptr = ent->fromDetail.tranBeginDate.ToString(sbuff, "yyyy-MM-dd");
		this->txtStartDate->SetText(CSTRP(sbuff, sptr));
		if (ent->toDetail.tranEndDate.IsNull())
		{
			this->txtEndDate->SetText(CSTR(""));
		}
		else
		{
			sptr = ent->toDetail.tranEndDate.ToString(sbuff, "yyyy-MM-dd");
			this->txtEndDate->SetText(CSTRP(sbuff, sptr));
		}
		this->cboAsset->SetEnabled(false);
		NN<Data::Invest::Asset> ass;
		UOSInt i = 0;
		UOSInt j = this->cboAsset->GetCount();
		while (i < j)
		{
			ass = this->cboAsset->GetItem(i).GetNN<Data::Invest::Asset>();
			if (ass->index == ent->fromIndex)
			{
				this->cboAsset->SetSelectedIndex(i);
				OnAssetSelChg(this);
				break;
			}
			i++;
		}
		sptr = Text::StrDouble(sbuff, ent->toDetail.amount);
		this->txtCurrencyValue->SetText(CSTRP(sbuff, sptr));
		this->ent = ent;
	}
}
