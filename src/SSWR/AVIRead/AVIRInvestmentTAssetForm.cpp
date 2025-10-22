#include "Stdafx.h"
#include "SSWR/AVIRead/AVIRInvestmentTAssetForm.h"

#define TITLE CSTR("Investment Transaction Asset")

void __stdcall SSWR::AVIRead::AVIRInvestmentTAssetForm::OnOKClicked(AnyType userObj)
{
	NN<AVIRInvestmentTAssetForm> me = userObj.GetNN<AVIRInvestmentTAssetForm>();
	NN<Data::Invest::TradeEntry> ent;
	if (me->ent.SetTo(ent))
	{
		Double assetAmount;
		Double currencyValue;
		Text::StringBuilderUTF8 sb;
		me->txtAssetAmount->GetText(sb);
		if (!sb.ToDouble(assetAmount))
		{
			me->ui->ShowMsgOK(CSTR("Please input valid asset amount"), TITLE, me);
			return;
		}
		sb.ClearStr();
		me->txtCurrencyValue->GetText(sb);
		if (!sb.ToDouble(currencyValue))
		{
			me->ui->ShowMsgOK(CSTR("Please input valid value"), TITLE, me);
			return;
		}
		Data::Date startDate = ent->fromDetail.tranBeginDate.ToDate();
		Data::Date priceDate;
		Data::Date endDate;
		sb.ClearStr();
		me->txtEndDate->GetText(sb);
		endDate = Data::Date(sb.ToCString());
		if (endDate.IsNull() && sb.leng > 0)
		{
			me->ui->ShowMsgOK(CSTR("Please enter valid end date"), TITLE, me);
			return;
		}
		sb.ClearStr();
		me->txtPriceDate->GetText(sb);
		priceDate = Data::Date(sb.ToCString());
		if (priceDate.IsNull() && sb.leng > 0)
		{
			me->ui->ShowMsgOK(CSTR("Please enter valid price date"), TITLE, me);
			return;
		}
		if (!endDate.IsNull())
		{
			if (priceDate.IsNull() || priceDate > endDate || startDate > endDate || priceDate < startDate)
			{
				me->ui->ShowMsgOK(CSTR("Dates are not in valid sequence"), TITLE, me);
				return;
			}
		}
		else if (!priceDate.IsNull())
		{
			if (priceDate < startDate)
			{
				me->ui->ShowMsgOK(CSTR("Price Date is before start date"), TITLE, me);
				return;
			}
		}
		if (me->mgr->UpdateTransactionAsset(ent,
			Data::Timestamp::FromDate(endDate, Data::DateTimeUtil::GetLocalTzQhr()),
			Data::Timestamp::FromDate(priceDate, Data::DateTimeUtil::GetLocalTzQhr()),
			assetAmount, currencyValue))
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
		NN<Data::Invest::Asset> ass;
		if (!me->cboAsset->GetSelectedItem().GetOpt<Data::Invest::Asset>().SetTo(ass))
		{
			me->ui->ShowMsgOK(CSTR("Please select an asset"), TITLE, me);
			return;
		}
		Double assetAmount;
		Double currencyValue;
		Text::StringBuilderUTF8 sb;
		me->txtAssetAmount->GetText(sb);
		if (!sb.ToDouble(assetAmount))
		{
			me->ui->ShowMsgOK(CSTR("Please input valid asset amount"), TITLE, me);
			return;
		}
		sb.ClearStr();
		me->txtCurrencyValue->GetText(sb);
		if (!sb.ToDouble(currencyValue))
		{
			me->ui->ShowMsgOK(CSTR("Please input valid value"), TITLE, me);
			return;
		}

		Data::Date startDate;
		Data::Date priceDate;
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
		sb.ClearStr();
		me->txtPriceDate->GetText(sb);
		priceDate = Data::Date(sb.ToCString());
		if (priceDate.IsNull() && sb.leng > 0)
		{
			me->ui->ShowMsgOK(CSTR("Please enter valid price date"), TITLE, me);
			return;
		}
		if (!endDate.IsNull())
		{
			if (priceDate.IsNull() || priceDate > endDate || startDate > endDate || priceDate < startDate)
			{
				me->ui->ShowMsgOK(CSTR("Dates are not in valid sequence"), TITLE, me);
				return;
			}
		}
		else if (!priceDate.IsNull())
		{
			if (priceDate < startDate)
			{
				me->ui->ShowMsgOK(CSTR("Price Date is before start date"), TITLE, me);
				return;
			}
		}
		if (me->mgr->AddTransactionAsset(Data::Timestamp::FromDate(startDate, Data::DateTimeUtil::GetLocalTzQhr()),
			Data::Timestamp::FromDate(endDate, Data::DateTimeUtil::GetLocalTzQhr()),
			Data::Timestamp::FromDate(priceDate, Data::DateTimeUtil::GetLocalTzQhr()),
			ass->index, assetAmount, currencyValue))
		{
			me->SetDialogResult(UI::GUIForm::DR_OK);
		}
		else
		{
			me->ui->ShowMsgOK(CSTR("Error in adding transaction"), TITLE, me);
		}
	}
}

void __stdcall SSWR::AVIRead::AVIRInvestmentTAssetForm::OnCancelClicked(AnyType userObj)
{
	NN<AVIRInvestmentTAssetForm> me = userObj.GetNN<AVIRInvestmentTAssetForm>();
	me->SetDialogResult(UI::GUIForm::DR_CANCEL);
}

void __stdcall SSWR::AVIRead::AVIRInvestmentTAssetForm::OnPriceDateChg(AnyType userObj)
{
	NN<AVIRInvestmentTAssetForm> me = userObj.GetNN<AVIRInvestmentTAssetForm>();
	NN<Data::Invest::Asset> ass;
	if (me->cboAsset->GetSelectedItem().GetOpt<Data::Invest::Asset>().SetTo(ass))
	{
		Text::StringBuilderUTF8 sb;
		me->txtPriceDate->GetText(sb);
		Data::Date dt = Data::Date(sb.ToCString());
		if (dt.IsNull())
		{
			me->txtPrice->SetText(CSTR(""));
		}
		else
		{
			sb.ClearStr();
			sb.AppendDouble(me->mgr->AssetGetPrice(ass, Data::Timestamp::FromDate(dt, Data::DateTimeUtil::GetLocalTzQhr())));
			me->txtPrice->SetText(sb.ToCString());
		}
	}
	else
	{
		me->txtPrice->SetText(CSTR(""));
	}
}

void __stdcall SSWR::AVIRead::AVIRInvestmentTAssetForm::OnAssetSelChg(AnyType userObj)
{
	NN<AVIRInvestmentTAssetForm> me = userObj.GetNN<AVIRInvestmentTAssetForm>();
	NN<Data::Invest::Asset> ass;
	if (me->cboAsset->GetSelectedItem().GetOpt<Data::Invest::Asset>().SetTo(ass))
	{
		Text::StringBuilderUTF8 sb;
		sb.Append(CSTR("Value ("));
		sb.Append(CURRENCYSTR(ass->currency));
		sb.AppendUTF8Char(')');
		me->lblCurrencyValue->SetText(sb.ToCString());
		OnPriceDateChg(me);
	}
}

SSWR::AVIRead::AVIRInvestmentTAssetForm::AVIRInvestmentTAssetForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core, NN<Data::Invest::InvestmentManager> mgr) : UI::GUIForm(parent, 1024, 300, ui)
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
	this->lblPriceDate = ui->NewLabel(*this, CSTR("Price Date"));
	this->lblPriceDate->SetRect(4, 28, 100, 23, false);
	this->txtPriceDate = ui->NewTextBox(*this, CSTR(""));
	this->txtPriceDate->SetRect(104, 28, 150, 23, false);
	this->txtPriceDate->HandleTextChanged(OnPriceDateChg, this);
	this->txtPrice = ui->NewTextBox(*this, CSTR(""));
	this->txtPrice->SetRect(254, 28, 50, 23, false);
	this->txtPrice->SetReadOnly(true);
	this->lblEndDate = ui->NewLabel(*this, CSTR("End Date"));
	this->lblEndDate->SetRect(4, 52, 100, 23, false);
	this->txtEndDate = ui->NewTextBox(*this, CSTR(""));
	this->txtEndDate->SetRect(104, 52, 150, 23, false);
	this->lblAsset = ui->NewLabel(*this, CSTR("Asset"));
	this->lblAsset->SetRect(4, 76, 100, 23, false);
	this->cboAsset = ui->NewComboBox(*this, false);
	this->cboAsset->SetRect(104, 76, 50, 23, false);
	this->cboAsset->HandleSelectionChange(OnAssetSelChg, this);
	this->lblAssetAmount = ui->NewLabel(*this, CSTR("Asset Amount"));
	this->lblAssetAmount->SetRect(4, 100, 100, 23, false);
	this->txtAssetAmount = ui->NewTextBox(*this, CSTR("0.0"));
	this->txtAssetAmount->SetRect(104, 100, 100, 23, false);
	this->lblCurrencyValue = ui->NewLabel(*this, CSTR("Value"));
	this->lblCurrencyValue->SetRect(4, 124, 100, 23, false);
	this->txtCurrencyValue = ui->NewTextBox(*this, CSTR("0.0"));
	this->txtCurrencyValue->SetRect(104, 124, 100, 23, false);
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

SSWR::AVIRead::AVIRInvestmentTAssetForm::~AVIRInvestmentTAssetForm()
{
}

void SSWR::AVIRead::AVIRInvestmentTAssetForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}

void SSWR::AVIRead::AVIRInvestmentTAssetForm::SetEntry(NN<Data::Invest::TradeEntry> ent)
{
	if (ent->type == Data::Invest::TradeType::CashToAsset)
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
		if (ent->toDetail.priceDate.IsNull())
		{
			this->txtPriceDate->SetText(CSTR(""));
		}
		else
		{
			sptr = ent->toDetail.priceDate.ToString(sbuff, "yyyy-MM-dd");
			this->txtPriceDate->SetText(CSTRP(sbuff, sptr));
		}
		this->cboAsset->SetEnabled(false);
		NN<Data::Invest::Asset> ass;
		UOSInt i = 0;
		UOSInt j = this->cboAsset->GetCount();
		while (i < j)
		{
			ass = this->cboAsset->GetItem(i).GetNN<Data::Invest::Asset>();
			if (ass->index == ent->toIndex)
			{
				this->cboAsset->SetSelectedIndex(i);
				OnAssetSelChg(this);
				break;
			}
			i++;
		}
		sptr = Text::StrDouble(sbuff, ent->fromDetail.amount);
		this->txtCurrencyValue->SetText(CSTRP(sbuff, sptr));
		sptr = Text::StrDouble(sbuff, ent->toDetail.amount);
		this->txtAssetAmount->SetText(CSTRP(sbuff, sptr));
		this->btnOK->SetText(CSTR("Update"));
		this->ent = ent;
	}
}
