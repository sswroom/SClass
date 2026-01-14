#include "Stdafx.h"
#include "Data/ChartPlotter.h"
#include "Data/Currency.h"
#include "IO/Path.h"
#include "IO/Registry.h"
#include "SSWR/AVIRead/AVIRInvestmentAccountForm.h"
#include "SSWR/AVIRead/AVIRInvestmentAInterestForm.h"
#include "SSWR/AVIRead/AVIRInvestmentAssetForm.h"
#include "SSWR/AVIRead/AVIRInvestmentCInterestForm.h"
#include "SSWR/AVIRead/AVIRInvestmentDepositForm.h"
#include "SSWR/AVIRead/AVIRInvestmentForm.h"
#include "SSWR/AVIRead/AVIRInvestmentFXForm.h"
#include "SSWR/AVIRead/AVIRInvestmentImportForm.h"
#include "SSWR/AVIRead/AVIRInvestmentTAssetForm.h"
#include "Text/MyStringW.h"
#include "Text/StringBuilderUTF8.h"

#define TITLE CSTR("Investment")

void __stdcall SSWR::AVIRead::AVIRInvestmentForm::OnDirClicked(AnyType userObj)
{
	NN<AVIRInvestmentForm> me = userObj.GetNN<AVIRInvestmentForm>();
	if (me->mgr.NotNull())
	{
		me->mgr.Delete();
		me->txtDir->SetReadOnly(false);
		me->lbCurrency->ClearItems();
		me->lbAssets->ClearItems();
		return;
	}
	Text::StringBuilderUTF8 sb;
	me->txtDir->GetText(sb);
	if (IO::Path::GetPathType(sb.ToCString()) != IO::Path::PathType::Directory)
	{
		me->ui->ShowMsgOK(CSTR("Input is not a directory"), TITLE, me);
		return;
	}

	NN<Data::Invest::InvestmentManager> mgr;
	NEW_CLASSNN(mgr, Data::Invest::InvestmentManager(sb.ToCString()));
	if (mgr->IsError())
	{
		mgr.Delete();
		me->ui->ShowMsgOK(CSTR("Error in initializing directory"), TITLE, me);
		return;
	}

	NN<IO::Registry> reg;
	if (IO::Registry::OpenSoftware(IO::Registry::REG_USER_THIS, L"SSWR", L"AVIRead").SetTo(reg))
	{
		UnsafeArray<const WChar> s = Text::StrToWCharNew(sb.v);
		reg->SetValue(L"Investment", s);
		Text::StrDelNew(s);
		IO::Registry::CloseRegistry(reg);
	}
	UInt32 c = mgr->GetLocalCurrency();
	me->txtLocalCurrency->SetText(CURRENCYSTR(c));
	c = mgr->GetRefCurrency();
	me->txtRefCurrency->SetText(CURRENCYSTR(c));
	me->mgr = mgr;
	me->txtDir->SetReadOnly(true);
	me->UpdateCurrencyList(mgr);
	NN<Data::Invest::Asset> ass;
	UOSInt i = 0;
	UOSInt j = mgr->GetAssetCount();
	while (i < j)
	{
		if (mgr->GetAsset(i).SetTo(ass))
		{
			me->lbAssets->AddItem(ass->shortName, ass);
		}
		i++;
	}
	me->DisplayTransactions(mgr);
	me->UpdateMonthly(mgr);
	me->UpdateYearly(mgr);
}

void __stdcall SSWR::AVIRead::AVIRInvestmentForm::OnCurrencyImportClicked(AnyType userObj)
{
	NN<AVIRInvestmentForm> me = userObj.GetNN<AVIRInvestmentForm>();
	NN<Data::Invest::Currency> curr;
	NN<Data::Invest::InvestmentManager> mgr;
	if (me->mgr.SetTo(mgr) && me->lbCurrency->GetSelectedItem().GetOpt<Data::Invest::Currency>().SetTo(curr) && curr->c != CURRENCY("USD"))
	{
		NN<DB::ReadingDB> db;
		NN<AVIRInvestmentImportForm> frm;
		NEW_CLASSNN(frm, AVIRInvestmentImportForm(nullptr, me->ui, me->core, curr->invert));
		if (frm->ShowDialog(me) == UI::GUIForm::DR_OK && frm->GetDB().SetTo(db))
		{
			if (mgr->CurrencyImport(curr, db, frm->GetTimeCol(), frm->GetValueCol(), frm->GetDateFormat(), frm->IsInvert()))
			{
				me->DisplayCurrency(curr);
			}
		}
		frm.Delete();
	}
}

void __stdcall SSWR::AVIRead::AVIRInvestmentForm::OnCurrencySelChg(AnyType userObj)
{
	NN<AVIRInvestmentForm> me = userObj.GetNN<AVIRInvestmentForm>();
	NN<Data::Invest::Currency> curr;
	if (me->lbCurrency->GetSelectedItem().GetOpt<Data::Invest::Currency>().SetTo(curr))
	{
		me->DisplayCurrency(curr);
	}
}

void __stdcall SSWR::AVIRead::AVIRInvestmentForm::OnCurrencySizeChg(AnyType userObj)
{
	NN<AVIRInvestmentForm> me = userObj.GetNN<AVIRInvestmentForm>();
	NN<Data::Invest::Currency> curr;
	if (me->lbCurrency->GetSelectedItem().GetOpt<Data::Invest::Currency>().SetTo(curr))
	{
		me->DisplayCurrencyImg(curr);
	}
}

void __stdcall SSWR::AVIRead::AVIRInvestmentForm::OnCurrencyHistUpdateClicked(AnyType userObj)
{
	NN<AVIRInvestmentForm> me = userObj.GetNN<AVIRInvestmentForm>();
	NN<Data::Invest::Currency> curr;
	NN<Data::Invest::InvestmentManager> mgr;
	if (me->mgr.SetTo(mgr) && me->lbCurrency->GetSelectedItem().GetOpt<Data::Invest::Currency>().SetTo(curr))
	{
		Data::Timestamp ts;
		Double val;
		Text::StringBuilderUTF8 sb;
		me->txtCurrencyHistDate->GetText(sb);
		ts = Data::Timestamp::FromStr(sb.ToCString(), Data::DateTimeUtil::GetLocalTzQhr());
		if (ts.IsNull())
		{
			me->ui->ShowMsgOK(CSTR("Please enter valid date"), TITLE, me);
			return;
		}
		sb.ClearStr();
		me->txtCurrencyHistValue->GetText(sb);
		if (!sb.ToDouble(val))
		{
			me->ui->ShowMsgOK(CSTR("Please enter valid value"), TITLE, me);
			return;
		}
		if (curr->invert)
		{
			val = 1 / val;
		}
		if (mgr->UpdateCurrency(curr, ts, val))
		{
			me->DisplayCurrency(curr);
		}
	}
}

void __stdcall SSWR::AVIRead::AVIRInvestmentForm::OnCurrencyHistSelChg(AnyType userObj)
{
	NN<AVIRInvestmentForm> me = userObj.GetNN<AVIRInvestmentForm>();
	NN<Text::String> s;
	UOSInt i = me->lvCurrencyHist->GetSelectedIndex();
	if (me->lvCurrencyHist->GetItemTextNew(i).SetTo(s))
	{
		me->txtCurrencyHistDate->SetText(s->ToCString());
		s->Release();
		Text::StringBuilderUTF8 sb;
		me->lvCurrencyHist->GetSubItem(i, 1, sb);
		me->txtCurrencyHistValue->SetText(sb.ToCString());
	}
}

void __stdcall SSWR::AVIRead::AVIRInvestmentForm::OnAssetsAddClicked(AnyType userObj)
{
	NN<AVIRInvestmentForm> me = userObj.GetNN<AVIRInvestmentForm>();
	NN<Data::Invest::InvestmentManager> mgr;
	if (me->mgr.SetTo(mgr))
	{
		AVIRInvestmentAssetForm frm(nullptr, me->ui, me->core);
		NN<Text::String> shortName;
		NN<Text::String> fullName;
		UInt32 currency;
		if (frm.ShowDialog(me) == UI::GUIForm::DR_OK)
		{
			shortName = frm.GetShortName();
			fullName = frm.GetFullName();
			currency = frm.GetCurrency();
			NN<Data::Invest::Asset> ass;
			if (mgr->AddAsset(shortName, fullName, currency).SetTo(ass))
			{
				UOSInt i = me->lbAssets->AddItem(ass->shortName, ass);
				me->lbAssets->SetSelectedIndex(i);
				me->DisplayAsset(ass);
				if (me->lbCurrency->GetCount() != mgr->GetCurrencyCount())
				{
					me->UpdateCurrencyList(mgr);
				}
			}
		}
	}

}

void __stdcall SSWR::AVIRead::AVIRInvestmentForm::OnAssetsSelChg(AnyType userObj)
{
	NN<AVIRInvestmentForm> me = userObj.GetNN<AVIRInvestmentForm>();
	NN<Data::Invest::Asset> ass;
	if (me->lbAssets->GetSelectedItem().GetOpt<Data::Invest::Asset>().SetTo(ass))
	{
		me->DisplayAsset(ass);
	}
}

void __stdcall SSWR::AVIRead::AVIRInvestmentForm::OnAssetsSizeChg(AnyType userObj)
{
	NN<AVIRInvestmentForm> me = userObj.GetNN<AVIRInvestmentForm>();
	NN<Data::Invest::Asset> ass;
	if (me->lbAssets->GetSelectedItem().GetOpt<Data::Invest::Asset>().SetTo(ass))
	{
		me->DisplayAssetImg(ass);
	}
}

void __stdcall SSWR::AVIRead::AVIRInvestmentForm::OnAssetsImportClicked(AnyType userObj)
{
	NN<AVIRInvestmentForm> me = userObj.GetNN<AVIRInvestmentForm>();
	NN<Data::Invest::Asset> ass;
	NN<Data::Invest::InvestmentManager> mgr;
	if (me->mgr.SetTo(mgr) && me->lbAssets->GetSelectedItem().GetOpt<Data::Invest::Asset>().SetTo(ass))
	{
		NN<DB::ReadingDB> db;
		NN<AVIRInvestmentImportForm> frm;
		NEW_CLASSNN(frm, AVIRInvestmentImportForm(nullptr, me->ui, me->core, false));
		if (frm->ShowDialog(me) == UI::GUIForm::DR_OK && frm->GetDB().SetTo(db))
		{
			if (mgr->AssetImport(ass, db, frm->GetTimeCol(), frm->GetValueCol(), frm->GetDateFormat()))
			{
				me->DisplayAsset(ass);
			}
		}
		frm.Delete();
	}
}

void __stdcall SSWR::AVIRead::AVIRInvestmentForm::OnAssetsImportDivClicked(AnyType userObj)
{
	NN<AVIRInvestmentForm> me = userObj.GetNN<AVIRInvestmentForm>();
	NN<Data::Invest::Asset> ass;
	NN<Data::Invest::InvestmentManager> mgr;
	if (me->mgr.SetTo(mgr) && me->lbAssets->GetSelectedItem().GetOpt<Data::Invest::Asset>().SetTo(ass))
	{
		NN<DB::ReadingDB> db;
		NN<AVIRInvestmentImportForm> frm;
		NEW_CLASSNN(frm, AVIRInvestmentImportForm(nullptr, me->ui, me->core, false));
		if (frm->ShowDialog(me) == UI::GUIForm::DR_OK && frm->GetDB().SetTo(db))
		{
			if (mgr->AssetImportDiv(ass, db, frm->GetTimeCol(), frm->GetValueCol(), frm->GetDateFormat()))
			{
				me->DisplayAsset(ass);
			}
		}
		frm.Delete();
	}
}

void __stdcall SSWR::AVIRead::AVIRInvestmentForm::OnAssetsHistUpdateClicked(AnyType userObj)
{
	NN<AVIRInvestmentForm> me = userObj.GetNN<AVIRInvestmentForm>();
	NN<Data::Invest::Asset> ass;
	NN<Data::Invest::InvestmentManager> mgr;
	if (me->mgr.SetTo(mgr) && me->lbAssets->GetSelectedItem().GetOpt<Data::Invest::Asset>().SetTo(ass))
	{
		Data::Timestamp ts;
		Double val;
		Double divVal;
		Text::StringBuilderUTF8 sb;
		me->txtAssetsHistDate->GetText(sb);
		ts = Data::Timestamp::FromStr(sb.ToCString(), Data::DateTimeUtil::GetLocalTzQhr());
		if (ts.IsNull())
		{
			me->ui->ShowMsgOK(CSTR("Please enter valid date"), TITLE, me);
			return;
		}
		sb.ClearStr();
		me->txtAssetsHistValue->GetText(sb);
		if (!sb.ToDouble(val))
		{
			me->ui->ShowMsgOK(CSTR("Please enter valid value"), TITLE, me);
			return;
		}
		sb.ClearStr();
		me->txtAssetsHistDiv->GetText(sb);
		if (!sb.ToDouble(divVal))
		{
			me->ui->ShowMsgOK(CSTR("Please enter valid div"), TITLE, me);
			return;
		}
		if (mgr->UpdateAsset(ass, ts, val, divVal))
		{
			me->DisplayAsset(ass);
		}
	}
}

void __stdcall SSWR::AVIRead::AVIRInvestmentForm::OnAssetsHistSelChg(AnyType userObj)
{
	NN<AVIRInvestmentForm> me = userObj.GetNN<AVIRInvestmentForm>();
	NN<Text::String> s;
	UOSInt i = me->lvAssetsHist->GetSelectedIndex();
	if (me->lvAssetsHist->GetItemTextNew(i).SetTo(s))
	{
		me->txtAssetsHistDate->SetText(s->ToCString());
		s->Release();
		Text::StringBuilderUTF8 sb;
		me->lvAssetsHist->GetSubItem(i, 1, sb);
		me->txtAssetsHistValue->SetText(sb.ToCString());
		sb.ClearStr();
		me->lvAssetsHist->GetSubItem(i, 2, sb);
		me->txtAssetsHistDiv->SetText(sb.ToCString());
	}
}

void __stdcall SSWR::AVIRead::AVIRInvestmentForm::OnTransactionFXClicked(AnyType userObj)
{
	NN<AVIRInvestmentForm> me = userObj.GetNN<AVIRInvestmentForm>();
	NN<Data::Invest::InvestmentManager> mgr;
	if (me->mgr.SetTo(mgr))
	{
		AVIRInvestmentFXForm frm(nullptr, me->ui, me->core, mgr);
		if (frm.ShowDialog(me) == UI::GUIForm::DR_OK)
		{
			me->DisplayTransactions(mgr);
		}
	}
}

void __stdcall SSWR::AVIRead::AVIRInvestmentForm::OnTransactionDepositClicked(AnyType userObj)
{
	NN<AVIRInvestmentForm> me = userObj.GetNN<AVIRInvestmentForm>();
	NN<Data::Invest::InvestmentManager> mgr;
	if (me->mgr.SetTo(mgr))
	{
		AVIRInvestmentDepositForm frm(nullptr, me->ui, me->core, mgr);
		if (frm.ShowDialog(me) == UI::GUIForm::DR_OK)
		{
			me->DisplayTransactions(mgr);
		}
	}
}

void __stdcall SSWR::AVIRead::AVIRInvestmentForm::OnTransactionAssetClicked(AnyType userObj)
{
	NN<AVIRInvestmentForm> me = userObj.GetNN<AVIRInvestmentForm>();
	NN<Data::Invest::InvestmentManager> mgr;
	if (me->mgr.SetTo(mgr))
	{
		AVIRInvestmentTAssetForm frm(nullptr, me->ui, me->core, mgr);
		if (frm.ShowDialog(me) == UI::GUIForm::DR_OK)
		{
			me->DisplayTransactions(mgr);
		}
	}
}

void __stdcall SSWR::AVIRead::AVIRInvestmentForm::OnTransactionAInterestClicked(AnyType userObj)
{
	NN<AVIRInvestmentForm> me = userObj.GetNN<AVIRInvestmentForm>();
	NN<Data::Invest::InvestmentManager> mgr;
	if (me->mgr.SetTo(mgr))
	{
		AVIRInvestmentAInterestForm frm(nullptr, me->ui, me->core, mgr);
		if (frm.ShowDialog(me) == UI::GUIForm::DR_OK)
		{
			me->DisplayTransactions(mgr);
		}
	}
}

void __stdcall SSWR::AVIRead::AVIRInvestmentForm::OnTransactionCInterestClicked(AnyType userObj)
{
	NN<AVIRInvestmentForm> me = userObj.GetNN<AVIRInvestmentForm>();
	NN<Data::Invest::InvestmentManager> mgr;
	if (me->mgr.SetTo(mgr))
	{
		AVIRInvestmentCInterestForm frm(nullptr, me->ui, me->core, mgr);
		if (frm.ShowDialog(me) == UI::GUIForm::DR_OK)
		{
			me->DisplayTransactions(mgr);
		}
	}
}

void __stdcall SSWR::AVIRead::AVIRInvestmentForm::OnTransactionDblClk(AnyType userObj, UOSInt index)
{
	NN<AVIRInvestmentForm> me = userObj.GetNN<AVIRInvestmentForm>();
	NN<Data::Invest::TradeEntry> ent;
	NN<Data::Invest::InvestmentManager> mgr;
	if (me->mgr.SetTo(mgr) && me->lvTransaction->GetItem(index).GetOpt<Data::Invest::TradeEntry>().SetTo(ent))
	{
		if (ent->type == Data::Invest::TradeType::AssetInterest)
		{
			AVIRInvestmentAInterestForm frm(nullptr, me->ui, me->core, mgr);
			frm.SetEntry(ent);
			if (frm.ShowDialog(me) == UI::GUIForm::DR_OK)
			{
				me->DisplayTransactions(mgr);
			}
		}
		else if (ent->type == Data::Invest::TradeType::CashToAsset)
		{
			AVIRInvestmentTAssetForm frm(nullptr, me->ui, me->core, mgr);
			frm.SetEntry(ent);
			if (frm.ShowDialog(me) == UI::GUIForm::DR_OK)
			{
				me->DisplayTransactions(mgr);
			}
		}
	}

}

void __stdcall SSWR::AVIRead::AVIRInvestmentForm::OnMonthlySelChg(AnyType userObj)
{
	NN<AVIRInvestmentForm> me = userObj.GetNN<AVIRInvestmentForm>();
	NN<Data::Invest::InvestmentManager> mgr;
	if (me->mgr.SetTo(mgr))
	{
		me->DisplayMonthly(mgr, (Int32)me->cboMonthlyYear->GetSelectedItem().GetOSInt(), (UInt8)me->cboMonthlyMonth->GetSelectedItem().GetUOSInt());
	}
}

void __stdcall SSWR::AVIRead::AVIRInvestmentForm::OnMonthlySizeChg(AnyType userObj)
{
	NN<AVIRInvestmentForm> me = userObj.GetNN<AVIRInvestmentForm>();
	me->DisplayMonthlyImg();	
}

void __stdcall SSWR::AVIRead::AVIRInvestmentForm::OnYearlySelChg(AnyType userObj)
{
	NN<AVIRInvestmentForm> me = userObj.GetNN<AVIRInvestmentForm>();
	NN<Data::Invest::InvestmentManager> mgr;
	if (me->mgr.SetTo(mgr))
	{
		me->DisplayYearly(mgr, (Int32)me->cboYearlyYear->GetSelectedItem().GetOSInt());
	}
}

void __stdcall SSWR::AVIRead::AVIRInvestmentForm::OnYearlySizeChg(AnyType userObj)
{
	NN<AVIRInvestmentForm> me = userObj.GetNN<AVIRInvestmentForm>();
	me->DisplayYearlyImg();	
}

void __stdcall SSWR::AVIRead::AVIRInvestmentForm::OnYearlyGridSelChg(AnyType userObj)
{
	NN<AVIRInvestmentForm> me = userObj.GetNN<AVIRInvestmentForm>();
	me->DisplayYearlyImg();
}

void SSWR::AVIRead::AVIRInvestmentForm::UpdateCurrencyList(NN<Data::Invest::InvestmentManager> mgr)
{
	NN<Data::Invest::Currency> curr;
	UOSInt i = 0;
	UOSInt j = mgr->GetCurrencyCount();
	while (i < j)
	{
		if (mgr->GetCurrencyInfo(i).SetTo(curr))
		{
			this->lbCurrency->AddItem(CURRENCYSTR(curr->c), curr);
		}
		i++;
	}
	if (this->lbCurrency->GetCount() > 0)
	{
		this->lbCurrency->SetSelectedIndex(0);
		OnCurrencySelChg(this);
	}
}

void SSWR::AVIRead::AVIRInvestmentForm::UpdateMonthly(NN<Data::Invest::InvestmentManager> mgr)
{
	UTF8Char sbuff[64];
	UnsafeArray<UTF8Char> sptr;
	Data::Timestamp ts = mgr->GetFirstTradeTime();
	Data::Timestamp now = Data::Timestamp::Now();
	Int32 startYear = ts.GetTimeValue().year;
	Data::DateTimeUtil::TimeValue tv = now.GetTimeValue();
	UOSInt i;
	UOSInt j;
	UOSInt k;
	Int32 endYear = tv.year;
	this->cboMonthlyYear->ClearItems();
	while (startYear <= endYear)
	{
		sptr = Text::StrInt32(sbuff, startYear);
		k = this->cboMonthlyYear->AddItem(CSTRP(sbuff, sptr), (void*)(OSInt)startYear);
		if (startYear == endYear)
		{
			this->cboMonthlyYear->SetSelectedIndex(k);
		}
		startYear++;
	}

	this->cboMonthlyMonth->ClearItems();
	i = 1;
	j = 12;
	while (i <= j)
	{
		sptr = Text::StrUOSInt(sbuff, i);
		k = this->cboMonthlyMonth->AddItem(CSTRP(sbuff, sptr), (void*)i);
		if (i == tv.month)
		{
			this->cboMonthlyMonth->SetSelectedIndex(k);
		}
		i++;
	}
	this->DisplayMonthly(mgr, tv.year, tv.month);
}

void SSWR::AVIRead::AVIRInvestmentForm::UpdateYearly(NN<Data::Invest::InvestmentManager> mgr)
{
	UTF8Char sbuff[64];
	UnsafeArray<UTF8Char> sptr;
	Data::Timestamp ts = mgr->GetFirstTradeTime();
	Data::Timestamp now = Data::Timestamp::Now();
	Int32 startYear = ts.GetTimeValue().year;
	Data::DateTimeUtil::TimeValue tv = now.GetTimeValue();
	UOSInt k;
	Int32 endYear = tv.year;
	this->cboYearlyYear->ClearItems();
	while (startYear <= endYear)
	{
		sptr = Text::StrInt32(sbuff, startYear);
		k = this->cboYearlyYear->AddItem(CSTRP(sbuff, sptr), (void*)(OSInt)startYear);
		if (startYear == endYear)
		{
			this->cboYearlyYear->SetSelectedIndex(k);
		}
		startYear++;
	}
	this->DisplayYearly(mgr, tv.year);
}

void SSWR::AVIRead::AVIRInvestmentForm::DisplayCurrency(NN<Data::Invest::Currency> curr)
{
	UOSInt dp = Data::Currency::GetDecimal(curr->c);
	UTF8Char sbuff[64];
	UnsafeArray<UTF8Char> sptr;
	if (curr->invert)
	{
		sptr = Text::StrDoubleGDP(sbuff, 1 / curr->current, 3, 0, dp);
	}
	else
	{
		sptr = Text::StrDoubleGDP(sbuff, curr->current, 3, 0, dp);
	}
	this->txtCurrencyCurr->SetText(CSTRP(sbuff, sptr));
	this->lvCurrencyTrade->ClearItems();
	this->lvCurrencyTotal->ClearItems();
	Double val = 0;
	Double totalVal = 0;
	NN<Data::Invest::TradeEntry> ent;
	NN<Data::Invest::TradeDetail> t;
	Optional<Data::Invest::TradeDetail> optt;
	NN<Data::Invest::Asset> ass;
	UOSInt i;
	UOSInt j;
	UOSInt k;
	UOSInt l;
	i = 0;
	j = curr->trades.GetCount();
	while (i < j)
	{
		val += curr->trades.GetItemNoCheck(i)->amount;
		i++;
	}
	sptr = Text::StrDoubleGDP(sbuff, val, 3, 0, dp);
	this->txtCurrencyTotal->SetText(CSTRP(sbuff, sptr));
	k = this->lvCurrencyTotal->AddItem(CURRENCYSTR(curr->c), 0);
	this->lvCurrencyTotal->SetSubItem(k, 1, CSTRP(sbuff, sptr));
	totalVal += val;
	NN<Data::Invest::InvestmentManager> mgr;
	if (this->mgr.SetTo(mgr))
	{
		UOSInt refDP = Data::Currency::GetDecimal(mgr->GetRefCurrency());
		if (curr->c == mgr->GetRefCurrency())
		{
			this->txtCurrencyValue->SetText(CSTRP(sbuff, sptr));
		}
		else
		{
			sptr = Text::StrDoubleGDP(sbuff, val / curr->current, 3, 0, refDP);
			this->txtCurrencyValue->SetText(CSTRP(sbuff, sptr));

			Double totalAmount = 0;
			Double refAmount = 0;
			i = 0;
			j = mgr->GetTransactionCount();
			while (i < j)
			{
				if (mgr->GetTransactionEntry(i).SetTo(ent) && ent->type == Data::Invest::TradeType::ForeignExchange)
				{
					if (ent->fromIndex == curr->c)
					{
						optt = ent->fromDetail;
					}
					else if (ent->toIndex == curr->c)
					{
						optt = ent->toDetail;
					}
					else
					{
						optt = nullptr;
					}
					if (optt.SetTo(t))
					{
						if (curr->c == mgr->GetLocalCurrency())
						{
							sptr = t->tranBeginDate.ToString(sbuff, "yyyy-MM-dd");
							k = this->lvCurrencyTrade->AddItem(CSTRP(sbuff, sptr), 0);
							sptr = Text::StrDoubleGDP(sbuff, t->amount, 3, 0, dp);
							this->lvCurrencyTrade->SetSubItem(k, 1, CSTRP(sbuff, sptr));
							if (t->amount >= 0)
							{
								Double a = refAmount * t->amount / totalAmount;
								refAmount += a;
								totalAmount += t->amount;
								if (curr->invert)
								{
									sptr = Text::StrDouble(sbuff, a / t->amount);
								}
								else
								{
									sptr = Text::StrDouble(sbuff, t->amount / a);
								}
								this->lvCurrencyTrade->SetSubItem(k, 2, CSTRP(sbuff, sptr));
								sptr = Text::StrDoubleGDP(sbuff, a, 3, 0, refDP);
								this->lvCurrencyTrade->SetSubItem(k, 3, CSTRP(sbuff, sptr));
							}
							else
							{
								refAmount += t->amount / t->cost;
								totalAmount += t->amount;
								if (curr->invert)
								{
									sptr = Text::StrDouble(sbuff, 1 / t->cost);
								}
								else
								{
									sptr = Text::StrDouble(sbuff, t->cost);
								}
								this->lvCurrencyTrade->SetSubItem(k, 2, CSTRP(sbuff, sptr));
								sptr = Text::StrDoubleGDP(sbuff, t->amount / t->cost, 3, 0, refDP);
								this->lvCurrencyTrade->SetSubItem(k, 3, CSTRP(sbuff, sptr));
							}
						}
						else
						{
							sptr = t->tranBeginDate.ToString(sbuff, "yyyy-MM-dd");
							k = this->lvCurrencyTrade->AddItem(CSTRP(sbuff, sptr), 0);
							sptr = Text::StrDoubleGDP(sbuff, t->amount, 3, 0, dp);
							this->lvCurrencyTrade->SetSubItem(k, 1, CSTRP(sbuff, sptr));
							if (t->amount < 0)
							{
								Double a = refAmount * t->amount / totalAmount;
								refAmount += a;
								totalAmount += t->amount;
								if (curr->invert)
								{
									sptr = Text::StrDouble(sbuff, a / t->amount);
								}
								else
								{
									sptr = Text::StrDouble(sbuff, t->amount / a);
								}
								this->lvCurrencyTrade->SetSubItem(k, 2, CSTRP(sbuff, sptr));
								sptr = Text::StrDoubleGDP(sbuff, a, 3, 0, refDP);
								this->lvCurrencyTrade->SetSubItem(k, 3, CSTRP(sbuff, sptr));
							}
							else
							{
								refAmount += t->amount / t->cost;
								totalAmount += t->amount;
								if (curr->invert)
								{
									sptr = Text::StrDouble(sbuff, 1 / t->cost);
								}
								else
								{
									sptr = Text::StrDouble(sbuff, t->cost);
								}
								this->lvCurrencyTrade->SetSubItem(k, 2, CSTRP(sbuff, sptr));
								sptr = Text::StrDoubleGDP(sbuff, t->amount / t->cost, 3, 0, refDP);
								this->lvCurrencyTrade->SetSubItem(k, 3, CSTRP(sbuff, sptr));
							}
						}
					}
				}
				i++;
			}
			if (curr->c == mgr->GetLocalCurrency())
			{
				k = this->lvCurrencyTrade->AddItem(CSTR("Total"), 0);
				sptr = Text::StrDoubleGDP(sbuff, totalAmount, 3, 0, dp);
				this->lvCurrencyTrade->SetSubItem(k, 1, CSTRP(sbuff, sptr));
				if (curr->invert)
				{
					sptr = Text::StrDouble(sbuff, refAmount / totalAmount);
				}
				else
				{
					sptr = Text::StrDouble(sbuff, totalAmount / refAmount);
				}
				this->lvCurrencyTrade->SetSubItem(k, 2, CSTRP(sbuff, sptr));
				sptr = Text::StrDoubleGDP(sbuff, refAmount, 3, 0, refDP);
				this->lvCurrencyTrade->SetSubItem(k, 3, CSTRP(sbuff, sptr));
			}
			else
			{
				k = this->lvCurrencyTrade->AddItem(CSTR("Total"), 0);
				sptr = Text::StrDoubleGDP(sbuff, totalAmount, 3, 0, dp);
				this->lvCurrencyTrade->SetSubItem(k, 1, CSTRP(sbuff, sptr));
				if (curr->invert)
				{
					sptr = Text::StrDouble(sbuff, refAmount / totalAmount);
				}
				else
				{
					sptr = Text::StrDouble(sbuff, totalAmount / refAmount);
				}
				this->lvCurrencyTrade->SetSubItem(k, 2, CSTRP(sbuff, sptr));
				sptr = Text::StrDoubleGDP(sbuff, refAmount, 3, 0, refDP);
				this->lvCurrencyTrade->SetSubItem(k, 3, CSTRP(sbuff, sptr));
			}
		}
		i = 0;
		j = mgr->GetAssetCount();
		while (i < j)
		{
			if (mgr->GetAsset(i).SetTo(ass) && ass->currency == curr->c)
			{
				Double thisTotal = 0;
				k = 0;
				l = ass->trades.GetCount();
				while (k < l)
				{
					thisTotal += ass->trades.GetItemNoCheck(k)->amount;
					k++;
				}
				k = this->lvCurrencyTotal->AddItem(ass->shortName, 0);
				if (ass->valList.GetCount() > 0)
				{
					sptr = Text::StrDoubleGDP(sbuff, thisTotal * ass->valList.GetItem(ass->valList.GetCount() - 1), 3, 0, dp);
					this->lvCurrencyTotal->SetSubItem(k, 1, CSTRP(sbuff, sptr));
					totalVal += thisTotal * ass->valList.GetItem(ass->valList.GetCount() - 1);
				}
			}
			i++;
		}
	}
	sptr = Text::StrDoubleGDP(sbuff, totalVal, 3, 0, dp);
	this->txtCurrencyTotalValue->SetText(CSTRP(sbuff, sptr));
	this->lvCurrencyHist->ClearItems();
	i = 0;
	j = curr->tsList.GetCount();
	if (j > 20)
	{
		i = j - 20;
	}
	while (i < j)
	{
		sptr = curr->tsList.GetItem(i).ToString(sbuff, "yyyy-MM-dd");
		k = this->lvCurrencyHist->AddItem(CSTRP(sbuff, sptr), 0);
		if (curr->invert)
		{
			sptr = Text::StrDouble(sbuff, 1 / curr->valList.GetItem(i));
		}
		else
		{
			sptr = Text::StrDouble(sbuff, curr->valList.GetItem(i));
		}
		this->lvCurrencyHist->SetSubItem(k, 1, CSTRP(sbuff, sptr));
		i++;
	}
	this->DisplayCurrencyImg(curr);
}

void SSWR::AVIRead::AVIRInvestmentForm::DisplayCurrencyImg(NN<Data::Invest::Currency> curr)
{
	Math::Size2D<UOSInt> sz = this->pbCurrency->GetSizeP();
	NN<Media::DrawImage> dimg;
	NN<Media::StaticImage> simg;
	if (this->deng->CreateImage32(sz, Media::AT_IGNORE_ALPHA).SetTo(dimg))
	{
		dimg->SetHDPI(this->pbCurrency->GetHDPI());
		dimg->SetVDPI(this->pbCurrency->GetHDPI());
		if (curr->tsList.GetCount() < 2)
		{
			NN<Media::DrawBrush> b = dimg->NewBrushARGB(0xffffffff);
			dimg->DrawRect(Math::Coord2DDbl(0, 0), sz.ToDouble(), nullptr, b);
			dimg->DelBrush(b);
		}
		else
		{
			Data::ChartPlotter chart(0);
			NN<Data::ChartPlotter::DoubleData> yData = Data::ChartPlotter::NewData(curr->valList);
			NN<Data::ChartPlotter::TimeData> xData = Data::ChartPlotter::NewData(curr->tsList);
			if (curr->invert)
			{
				yData->Invert();
			}
			UOSInt range = this->cboCurrencyRange->GetSelectedIndex();
			if (range == 1)
			{
				xData->KeepAfter<Double>(Data::Timestamp::Now().AddYear(-1), yData);
			}
			else if (range == 2)
			{
				xData->KeepAfter<Double>(Data::Timestamp::Now().ClearMonthAndDay(), yData);
			}
			else if (range == 3)
			{
				xData->KeepAfter<Double>(Data::Timestamp::Now().AddMonth(-6), yData);
			}
			else if (range == 4)
			{
				xData->KeepAfter<Double>(Data::Timestamp::Now().AddMonth(-3), yData);
			}
			else if (range == 5)
			{
				xData->KeepAfter<Double>(Data::Timestamp::Now().AddMonth(-1), yData);
			}
			chart.AddLineChart(CSTR(""), yData, xData, 0xffff0000);
			chart.Plot(dimg, 0, 0, (Double)sz.GetWidth(), (Double)sz.GetHeight());
		}
		if (dimg->ToStaticImage().SetTo(simg))
		{
			this->currencyImg.Delete();
			this->currencyImg = simg;
			this->pbCurrency->SetImage(simg);
		}
		this->deng->DeleteImage(dimg);
	}
}

struct AssetSummary
{
	Double amountLeft;
	Double currCost;
	Data::Timestamp endDate;
	NN<Data::Invest::TradeDetail> trade;
};

void SSWR::AVIRead::AVIRInvestmentForm::DisplayAsset(NN<Data::Invest::Asset> ass)
{
	UTF8Char sbuff[64];
	UnsafeArray<UTF8Char> sptr;
	UOSInt i;
	UOSInt j;
	UOSInt k;
	UOSInt l;
	NN<Data::Invest::TradeDetail> t;
	Data::Timestamp ts;
	this->txtAssetsShortName->SetText(ass->shortName->ToCString());
	this->txtAssetsFullName->SetText(ass->fullName->ToCString());
	this->txtAssetsCurrency->SetText(CURRENCYSTR(ass->currency));
	sptr = Text::StrDouble(sbuff, ass->current);
	this->txtAssetsCurrent->SetText(CSTRP(sbuff, sptr));
	Double total = 0;
	i = 0;
	j = ass->trades.GetCount();
	while (i < j)
	{
		t = ass->trades.GetItemNoCheck(i);
		total += t->amount;
		i++;
	}
	sptr = Text::StrDouble(sbuff, total);
	this->txtAssetsAmount->SetText(CSTRP(sbuff, sptr));
	sptr = Text::StrDouble(sbuff, ass->current * total);
	this->txtAssetsValue->SetText(CSTRP(sbuff, sptr));
	this->lvAssetsHist->ClearItems();
	i = 0;
	j = ass->tsList.GetCount();
	if (j > 20)
	{
		i = j - 20;
	}
	while (i < j)
	{
		ts = ass->tsList.GetItem(i);
		if (ts.IsDate())
		{
			sptr = ts.ToString(sbuff, "yyyy-MM-dd");
		}
		else
		{
			sptr = ts.ToString(sbuff, "yyyy-MM-dd HH:mm");
		}
		k = this->lvAssetsHist->AddItem(CSTRP(sbuff, sptr), 0);
		sptr = Text::StrDouble(sbuff, ass->valList.GetItem(i));
		this->lvAssetsHist->SetSubItem(k, 1, CSTRP(sbuff, sptr));
		sptr = Text::StrDouble(sbuff, ass->divList.GetItem(i));
		this->lvAssetsHist->SetSubItem(k, 2, CSTRP(sbuff, sptr));
		i++;
	}
	
	UOSInt buyCnt = 0;
	i = 0;
	j = ass->trades.GetCount();
	if (j > 0)
	{
		Data::Timestamp startTime;
		Data::Timestamp endTime;
		Data::Timestamp ts;
		t = ass->trades.GetItemNoCheck(0);
		startTime = t->tranBeginDate;
		endTime = Data::Timestamp::Now();
		while (i < j)
		{
			t = ass->trades.GetItemNoCheck(i);
			if (t->amount > 0)
			{
				buyCnt++;
			}
			if (t->tranBeginDate < startTime)
			{
				startTime = t->tranBeginDate;
			}
			if (endTime < t->tranBeginDate)
			{
				endTime = t->tranBeginDate;
			}
			i++;
		}
		this->lvAssetsTrade->ChangeColumnCnt(buyCnt + 3);
		this->lvAssetsTrade->ClearAll();
		this->lvAssetsTrade->AddColumn(CSTR(""), 80);
		this->lvAssetsTrade->AddColumn(CSTR(""), 70);
		this->lvAssetsTrade->AddColumn(CSTR(""), 70);

		UnsafeArray<AssetSummary> summ = MemAllocArr(AssetSummary, buyCnt);
		Double totalAmount = 0;
		Double totalCost = 0;
		k = 0;
		i = 0;
		j = ass->trades.GetCount();
		while (i < j)
		{
			t = ass->trades.GetItemNoCheck(i);
			totalAmount += t->amount;
			if (t->amount > 0)
			{
				sptr = t->tranBeginDate.ToString(sbuff, "yyyy-MM-dd");
				this->lvAssetsTrade->AddColumn(CSTRP(sbuff, sptr), 70);
				summ[k].trade = t;
				summ[k].endDate = nullptr;
				summ[k].amountLeft = t->amount;
				summ[k].currCost = t->cost;
				totalCost += t->amount * t->cost;
				endTime = Data::Timestamp::Now();
				k++;
			}
			else
			{
				Double a = -t->amount;
				UOSInt i2 = 0;
				while (i2 < k)
				{
					if (summ[i2].amountLeft > 0)
					{
						if (summ[i2].amountLeft <= a)
						{
							a -= summ[i2].amountLeft;
							summ[i2].endDate = t->tranBeginDate;
							totalCost -= summ[i2].amountLeft * t->cost;
							summ[i2].amountLeft = 0;
							if (a < 0.00005)
							{
								if (i2 == k - 1)
								{
									endTime = t->tranBeginDate;
								}
								break;
							}
						}
						else
						{
							summ[i2].amountLeft -= a;
							totalCost -= a * t->cost;
							if (summ[i2].amountLeft < 0.00005)
							{
								summ[i2].endDate = t->tranBeginDate;
								summ[i2].amountLeft = 0;
								if (i2 == k - 1)
								{
									endTime = t->tranBeginDate;
								}
							}
							break;
						}
					}
					i2++;
				}
			}
			i++;
		}
		this->lvAssetsTrade->AddItem(CSTR(""), 0);
		this->lvAssetsTrade->SetSubItem(0, 2, CSTR("Buy Date"));
		this->lvAssetsTrade->AddItem(CSTR("Est Interest"), 0);
		this->lvAssetsTrade->SetSubItem(1, 2, CSTR("Sell Date"));
		this->lvAssetsTrade->AddItem(CSTR("Avg Price"), 0);
		this->lvAssetsTrade->SetSubItem(2, 2, CSTR("Buy Price"));
		if (totalAmount > 0)
		{
			sptr = Text::StrDoubleGDP(sbuff, totalCost / totalAmount, 0, 0, 4);
			this->lvAssetsTrade->SetSubItem(2, 1, CSTRP(sbuff, sptr));
		}
		this->lvAssetsTrade->AddItem(CSTR("Curr Units"), 0);
		this->lvAssetsTrade->SetSubItem(3, 2, CSTR("Units"));
		sptr = Text::StrDouble(sbuff, totalAmount);
		this->lvAssetsTrade->SetSubItem(3, 1, CSTRP(sbuff, sptr));
		this->lvAssetsTrade->AddItem(CSTR("Current Price"), 0);
		this->lvAssetsTrade->SetSubItem(4, 2, CSTR("Home Price"));
		this->lvAssetsTrade->AddItem(CSTR(""), 0);
		this->lvAssetsTrade->SetSubItem(5, 2, CSTR("Profit Rate"));
		i = 0;
		while (i < buyCnt)
		{
			sptr = summ[i].trade->tranBeginDate.ToString(sbuff, "yyyy-MM-dd");
			this->lvAssetsTrade->SetSubItem(0, 3 + i, CSTRP(sbuff, sptr));
			if (summ[i].endDate.NotNull())
			{
				sptr = summ[i].endDate.ToString(sbuff, "yyyy-MM-dd");
				this->lvAssetsTrade->SetSubItem(1, 3 + i, CSTRP(sbuff, sptr));
			}
			sptr = Text::StrDoubleGDP(sbuff, summ[i].trade->cost, 0, 0, 4);
			this->lvAssetsTrade->SetSubItem(2, 3 + i, CSTRP(sbuff, sptr));
			sptr = Text::StrDouble(sbuff, summ[i].amountLeft);
			this->lvAssetsTrade->SetSubItem(3, 3 + i, CSTRP(sbuff, sptr));
			i++;
		}
		Bool hasInterest = false;
		i = 0;
		j = ass->divList.GetCount();
		while (i < j)
		{
			if (ass->divList.GetItem(i) > 0)
			{
				hasInterest = true;
			}
			i++;
		}
		Double lastDiv = 0;
		if (hasInterest)
		{
			this->lvAssetsTrade->AddItem(CSTR("Interest Date"), 0);
			this->lvAssetsTrade->SetSubItem(6, 1, CSTR("Price"));
			this->lvAssetsTrade->SetSubItem(6, 2, CSTR("Interest"));
			i = 0;
			j = ass->tsList.GetCount();
			while (i < j)
			{
				if (ass->divList.GetItem(i) > 0)
				{
					ts = ass->tsList.GetItem(i);
					if (ts >= startTime && ts < endTime)
					{
						lastDiv = ass->divList.GetItem(i);
						sptr = ts.ToString(sbuff, "yyyy-MM-dd");
						k = this->lvAssetsTrade->AddItem(CSTRP(sbuff, sptr), 0);
						sptr = Text::StrDouble(sbuff, ass->valList.GetItem(i));
						this->lvAssetsTrade->SetSubItem(k, 1, CSTRP(sbuff, sptr));
						sptr = Text::StrDouble(sbuff, ass->divList.GetItem(i));
						this->lvAssetsTrade->SetSubItem(k, 2, CSTRP(sbuff, sptr));
						l = 0;
						while (l < buyCnt)
						{
							if (summ[l].trade->tranBeginDate < ts && (summ[l].endDate.IsNull() || summ[l].endDate > ts))
							{
								summ[l].currCost -= ass->divList.GetItem(i);
								sptr = Text::StrConcatC(Text::StrDoubleDP(sbuff, (ass->valList.GetItem(i) - summ[l].currCost) * 100.0 / summ[l].trade->cost * 360 / ts.Diff(summ[l].trade->tranBeginDate).GetTotalDays(), 4, 4), UTF8STRC("%"));
								this->lvAssetsTrade->SetSubItem(k, 3 + l, CSTRP(sbuff, sptr));
							}
							l++;
						}
					}
				}
				i++;
			}
		}
		else
		{
		}
		Double currPrice = ass->valList.GetItem(ass->valList.GetCount() - 1);
		ts = ass->tsList.GetItem(ass->tsList.GetCount() - 1);
		if (lastDiv > 0)
		{
			sptr = Text::StrDouble(sbuff, totalAmount * lastDiv);
			this->lvAssetsTrade->SetSubItem(1, 1, CSTRP(sbuff, sptr));
		}
		sptr = Text::StrDouble(sbuff, currPrice);
		this->lvAssetsTrade->SetSubItem(4, 1, CSTRP(sbuff, sptr));
		i = 0;
		while (i < buyCnt)
		{
			sptr = Text::StrDoubleGDP(sbuff, summ[i].currCost, 0, 0, 4);
			this->lvAssetsTrade->SetSubItem(4, 3 + i, CSTRP(sbuff, sptr));
			if (summ[i].amountLeft > 0)
			{
				sptr = Text::StrDoubleDP(sbuff, (currPrice - summ[i].currCost) * 100.0 / summ[i].trade->cost * 360 / ts.Diff(summ[i].trade->tranBeginDate).GetTotalDays(), 4, 4);
				this->lvAssetsTrade->SetSubItem(5, 3 + i, CSTRP(sbuff, sptr));
			}
			i++;
		}


		MemFreeArr(summ);
	}
	else
	{
		this->lvAssetsTrade->ClearAll();
	}

	this->DisplayAssetImg(ass);
}

void SSWR::AVIRead::AVIRInvestmentForm::DisplayAssetImg(NN<Data::Invest::Asset> ass)
{
	Math::Size2D<UOSInt> sz = this->pbAssets->GetSizeP();
	NN<Media::DrawImage> dimg;
	NN<Media::StaticImage> simg;
	if (this->deng->CreateImage32(sz, Media::AT_IGNORE_ALPHA).SetTo(dimg))
	{
		dimg->SetHDPI(this->pbAssets->GetHDPI());
		dimg->SetVDPI(this->pbAssets->GetHDPI());
		if (ass->tsList.GetCount() < 2)
		{
			NN<Media::DrawBrush> b = dimg->NewBrushARGB(0xffffffff);
			dimg->DrawRect(Math::Coord2DDbl(0, 0), sz.ToDouble(), nullptr, b);
			dimg->DelBrush(b);
		}
		else
		{
			Data::ChartPlotter chart(0);
			NN<Data::ChartPlotter::DoubleData> yData = Data::ChartPlotter::NewData(ass->valList);
			NN<Data::ChartPlotter::TimeData> xData = Data::ChartPlotter::NewData(ass->tsList);
			UOSInt range = this->cboAssetsRange->GetSelectedIndex();
			if (range == 1)
			{
				xData->KeepAfter<Double>(Data::Timestamp::Now().AddYear(-1), yData);
			}
			else if (range == 2)
			{
				xData->KeepAfter<Double>(Data::Timestamp::Now().ClearMonthAndDay(), yData);
			}
			else if (range == 3)
			{
				xData->KeepAfter<Double>(Data::Timestamp::Now().AddMonth(-6), yData);
			}
			else if (range == 4)
			{
				xData->KeepAfter<Double>(Data::Timestamp::Now().AddMonth(-3), yData);
			}
			else if (range == 5)
			{
				xData->KeepAfter<Double>(Data::Timestamp::Now().AddMonth(-1), yData);
			}
			chart.AddLineChart(CSTR(""), yData, xData, 0xffff0000);
			chart.Plot(dimg, 0, 0, (Double)sz.GetWidth(), (Double)sz.GetHeight());
		}
		if (dimg->ToStaticImage().SetTo(simg))
		{
			this->assetsImg.Delete();
			this->assetsImg = simg;
			this->pbAssets->SetImage(simg);
		}
		this->deng->DeleteImage(dimg);
	}
}

void SSWR::AVIRead::AVIRInvestmentForm::DisplayTransactions(NN<Data::Invest::InvestmentManager> mgr)
{
	UTF8Char sbuff[64];
	UnsafeArray<UTF8Char> sptr;
	this->lvTransaction->ClearItems();
	UOSInt i = 0;
	UOSInt j = mgr->GetTransactionCount();
	NN<Data::Invest::TradeEntry> ent;
	UOSInt k;
	UInt32 fromC;
	UInt32 toC;
	while (i < j)
	{
		if (mgr->GetTransactionEntry(i).SetTo(ent))
		{
			sptr = ent->fromDetail.tranBeginDate.ToStringNoZone(sbuff);
			k = this->lvTransaction->AddItem(CSTRP(sbuff, sptr), ent);
			if (!ent->toDetail.tranEndDate.IsNull())
			{
				sptr = ent->toDetail.tranEndDate.ToStringNoZone(sbuff);
				this->lvTransaction->SetSubItem(k, 1, CSTRP(sbuff, sptr));
			}
			this->lvTransaction->SetSubItem(k, 2, Data::Invest::InvestmentManager::TradeTypeGetName(ent->type));
			if (ent->type == Data::Invest::TradeType::ForeignExchange)
			{
				fromC = (UInt32)ent->fromIndex;
				this->lvTransaction->SetSubItem(k, 3, CURRENCYSTR(fromC));
				toC = (UInt32)ent->toIndex;
				this->lvTransaction->SetSubItem(k, 6, CURRENCYSTR(toC));
			}
			else if (ent->type == Data::Invest::TradeType::FixedDeposit)
			{
				fromC = (UInt32)ent->fromIndex;
				this->lvTransaction->SetSubItem(k, 3, CURRENCYSTR(fromC));
				toC = (UInt32)ent->toIndex;
				this->lvTransaction->SetSubItem(k, 6, CURRENCYSTR(toC));
			}
			else if (ent->type == Data::Invest::TradeType::CashToAsset)
			{
				fromC = (UInt32)ent->fromIndex;
				this->lvTransaction->SetSubItem(k, 3, CURRENCYSTR(fromC));
				NN<Data::Invest::Asset> ass;
				if (mgr->GetAsset(ent->toIndex).SetTo(ass))
				{
					this->lvTransaction->SetSubItem(k, 6, ass->shortName);
					toC = ass->currency;
				}
				else
				{
					toC = fromC;
				}
			}
			else if (ent->type == Data::Invest::TradeType::AssetInterest)
			{
				NN<Data::Invest::Asset> ass;
				if (mgr->GetAsset(ent->fromIndex).SetTo(ass))
				{
					this->lvTransaction->SetSubItem(k, 3, ass->shortName);
					fromC = ass->currency;
				}
				else
				{
					fromC = (UInt32)ent->toIndex;
				}
				toC = (UInt32)ent->toIndex;
				this->lvTransaction->SetSubItem(k, 6, CURRENCYSTR(toC));
			}
			else if (ent->type == Data::Invest::TradeType::AccountInterest)
			{
				fromC = (UInt32)ent->fromIndex;
				this->lvTransaction->SetSubItem(k, 3, CURRENCYSTR(fromC));
				toC = (UInt32)ent->toIndex;
				this->lvTransaction->SetSubItem(k, 6, CURRENCYSTR(toC));
			}
			else
			{
				fromC = 0;
				toC = 0;
			}
			sptr = Text::StrDouble(sbuff, ent->fromDetail.amount);
			this->lvTransaction->SetSubItem(k, 4, CSTRP(sbuff, sptr));
			if (ent->fromDetail.cost <= 0.001 && ent->fromDetail.cost >= -0.001)
			{
				sptr = Text::StrDouble(Text::StrConcatC(sbuff, UTF8STRC("1 / ")), 1 / ent->fromDetail.cost);
			}
			else
			{
				sptr = Text::StrDouble(sbuff, ent->fromDetail.cost);
			}
			this->lvTransaction->SetSubItem(k, 5, CSTRP(sbuff, sptr));
			sptr = Text::StrDouble(sbuff, ent->toDetail.amount);
			this->lvTransaction->SetSubItem(k, 7, CSTRP(sbuff, sptr));
			if (ent->toDetail.cost <= 0.001 && ent->toDetail.cost >= -0.001)
			{
				sptr = Text::StrDouble(Text::StrConcatC(sbuff, UTF8STRC("1 / ")), 1 / ent->toDetail.cost);
			}
			else
			{
				sptr = Text::StrDouble(sbuff, ent->toDetail.cost);
			}
			this->lvTransaction->SetSubItem(k, 8, CSTRP(sbuff, sptr));
			sptr = Text::StrDouble(sbuff, ent->refRate);
			this->lvTransaction->SetSubItem(k, 9, CSTRP(sbuff, sptr));
		}
		i++;
	}
}

void SSWR::AVIRead::AVIRInvestmentForm::DisplayMonthly(NN<Data::Invest::InvestmentManager> mgr, Int32 year, UInt8 month)
{
	Data::Date startDate = Data::Date(year, month, 1);
	Data::Date endDate = startDate;
	endDate.SetMonth(startDate.GetDateValue().month + 1);
	NN<Data::ChartPlotter> chart;
	if (GenerateSummary(mgr, startDate, endDate, this->lvMonthly, nullptr).SetTo(chart))
	{
		this->monthlyChart.Delete();
		this->monthlyChart = chart;
		this->DisplayMonthlyImg();
	}
}

void SSWR::AVIRead::AVIRInvestmentForm::DisplayYearly(NN<Data::Invest::InvestmentManager> mgr, Int32 year)
{
	Data::Date startDate = Data::Date(year, 1, 1);
	Data::Date endDate = startDate;
	endDate.SetYear(year + 1);
	NN<Data::ChartPlotter> chart;
	if (GenerateSummary(mgr, startDate, endDate, this->lvYearly, this->txtYearlyAvgInvestment).SetTo(chart))
	{
		this->yearlyChart.Delete();
		this->yearlyChart = chart;
		this->DisplayYearlyImg();
	}
}

void SSWR::AVIRead::AVIRInvestmentForm::DisplayMonthlyImg()
{
	Math::Size2D<UOSInt> sz = this->pbMonthly->GetSizeP();
	NN<Data::ChartPlotter> chart;
	NN<Media::DrawImage> dimg;
	NN<Media::StaticImage> simg;
	if (this->monthlyChart.SetTo(chart) && this->deng->CreateImage32(sz, Media::AT_IGNORE_ALPHA).SetTo(dimg))
	{
		dimg->SetHDPI(this->pbMonthly->GetHDPI());
		dimg->SetVDPI(this->pbMonthly->GetHDPI());
		chart->Plot(dimg, 0, 0, (Double)sz.GetWidth(), (Double)sz.GetHeight());
		if (dimg->ToStaticImage().SetTo(simg))
		{
			this->monthlyImg.Delete();
			this->monthlyImg = simg;
			this->pbMonthly->SetImage(simg);
		}
		this->deng->DeleteImage(dimg);
	}
}

void SSWR::AVIRead::AVIRInvestmentForm::DisplayYearlyImg()
{
	Math::Size2D<UOSInt> sz = this->pbYearly->GetSizeP();
	NN<Data::ChartPlotter> chart;
	NN<Media::DrawImage> dimg;
	NN<Media::StaticImage> simg;
	if (this->yearlyChart.SetTo(chart) && this->deng->CreateImage32(sz, Media::AT_IGNORE_ALPHA).SetTo(dimg))
	{
		chart->SetGridType((Data::ChartPlotter::GridType)this->cboYearlyGrid->GetSelectedItem().GetOSInt());
		dimg->SetHDPI(this->pbYearly->GetHDPI());
		dimg->SetVDPI(this->pbYearly->GetHDPI());
		chart->Plot(dimg, 0, 0, (Double)sz.GetWidth(), (Double)sz.GetHeight());
		if (dimg->ToStaticImage().SetTo(simg))
		{
			this->yearlyImg.Delete();
			this->yearlyImg = simg;
			this->pbYearly->SetImage(simg);
		}
		this->deng->DeleteImage(dimg);
	}
}

Optional<Data::ChartPlotter> SSWR::AVIRead::AVIRInvestmentForm::GenerateSummary(NN<Data::Invest::InvestmentManager> mgr, Data::Date startDate, Data::Date endDate, NN<UI::GUIListView> listView, Optional<UI::GUITextBox> txtAverage)
{
	NN<Data::Invest::Currency> curr;
	NN<Data::Invest::Asset> ass;
	UTF8Char sbuff[64];
	UnsafeArray<UTF8Char> sptr;
	UOSInt i;
	UOSInt j;
	UOSInt k;
	UOSInt assetCnt = mgr->GetAssetCount();
	Data::ArrayListNN<Data::Invest::Asset> assList;
	Data::Timestamp startTime = Data::Timestamp::FromDate(startDate, Data::DateTimeUtil::GetLocalTzQhr());
	Data::Timestamp endTime = Data::Timestamp::FromDate(endDate, Data::DateTimeUtil::GetLocalTzQhr());
	NN<Data::Invest::TradeDetail> t;
	i = 0;
	while (i < assetCnt)
	{
		if (mgr->GetAsset(i).SetTo(ass))
		{
			Double amount = 0;
			j = 0;
			k = ass->trades.GetCount();
			while (j < k)
			{
				t = ass->trades.GetItemNoCheck(j);
				if (t->tranBeginDate <= endTime && (t->tranEndDate.IsNull() || t->tranEndDate > startTime))
				{
					amount = 1;
					break;
				}
				if (t->tranBeginDate < startTime)
				{
					amount += t->amount;
				}
				j++;
			}
			if (amount > 0)
			{
				assList.Add(ass);
			}
		}
		i++;
	}
	assetCnt = assList.GetCount();
	UOSInt currencyCnt = mgr->GetCurrencyCount();
	listView->ClearAll();
	listView->ChangeColumnCnt(currencyCnt + assetCnt + 3);
	listView->AddColumn(CSTR("Date"), 100);
	listView->AddColumn(CSTR("Gain"), 100);
	listView->AddColumn(CSTR("Total Gain"), 100);
	i = 0;
	while (i < currencyCnt)
	{
		if (mgr->GetCurrencyInfo(i).SetTo(curr))
		{
			listView->AddColumn(CURRENCYSTR(curr->c), 70);
		}
		i++;
	}
	i = 0;
	while (i < assetCnt)
	{
		ass = assList.GetItemNoCheck(i);
		listView->AddColumn(ass->shortName, 100);
		i++;
	}
	Data::Date now = Data::Date::Today();
	if (startDate >= now)
	{
		return nullptr;
	}
	if (endDate > now)
	{
		endDate = now;
	}
	Double investTotal = 0;
	NN<UI::GUITextBox> nntxtAverage;
	NN<Data::Invest::Currency> localCurr;
	Double initValue;
	Double rate;
	Double initTotal;
	UOSInt dp;
	Data::ArrayListTS dateList;
	Data::ArrayListNative<Double> totalList;
	Data::ArrayListNative<Double> valList;
	if (mgr->GetCurrencyInfo(0).SetTo(curr) && mgr->FindCurrency(mgr->GetLocalCurrency()).SetTo(localCurr))
	{
		mgr->CurrencyCalcValues(curr, startDate, endDate, dateList, valList, initValue);
		i = 0;
		j = dateList.GetCount();
		while (i < j)
		{
			sptr = dateList.GetItem(i).ToString(sbuff, "yyyy-MM-dd");
			listView->AddItem(CSTRP(sbuff, sptr), 0);
			totalList.Add(0);
			i++;
		}
		initTotal = 0;
		k = 0;
		while (k < currencyCnt)
		{
			if (mgr->GetCurrencyInfo(k).SetTo(curr))
			{
				dateList.Clear();
				valList.Clear();
				mgr->CurrencyCalcValues(curr, startDate, endDate, dateList, valList, initValue);
				if (localCurr->c == curr->c)
				{
					initTotal += initValue;
				}
				else if (curr->c == mgr->GetRefCurrency())
				{
					rate = mgr->CurrencyGetRate(localCurr, Data::Timestamp::FromDate(startDate.AddDay(-1), Data::DateTimeUtil::GetLocalTzQhr()));
					initTotal += initValue * rate;
				}
				else
				{
					rate = mgr->CurrencyGetRate(localCurr, Data::Timestamp::FromDate(startDate.AddDay(-1), Data::DateTimeUtil::GetLocalTzQhr())) / mgr->CurrencyGetRate(curr, Data::Timestamp::FromDate(startDate.AddDay(-1), Data::DateTimeUtil::GetLocalTzQhr()));
					initTotal += initValue * rate;
				}

				dp = Data::Currency::GetDecimal(curr->c);
				i = 0;
				j = valList.GetCount();
				while (i < j)
				{
					sptr = Text::StrDoubleGDP(sbuff, valList.GetItem(i), 3, 0, dp);
					listView->SetSubItem(i, 3 + k, CSTRP(sbuff, sptr));
					if (localCurr->c == curr->c)
					{
						investTotal += valList.GetItem(i);
						totalList.SetItem(i, totalList.GetItem(i) + valList.GetItem(i));
					}
					else if (curr->c == mgr->GetRefCurrency())
					{
						rate = mgr->CurrencyGetRate(localCurr, dateList.GetItem(i));
						totalList.SetItem(i, totalList.GetItem(i) + valList.GetItem(i) * rate);
					}
					else
					{
						rate = mgr->CurrencyGetRate(localCurr, dateList.GetItem(i)) / mgr->CurrencyGetRate(curr, dateList.GetItem(i));
						totalList.SetItem(i, totalList.GetItem(i) + valList.GetItem(i) * rate);
					}
					i++;
				}
				if (localCurr->c == curr->c && txtAverage.SetTo(nntxtAverage) && valList.GetCount() > 0)
				{
					sptr = Text::StrDoubleGDP(sbuff, -investTotal / (Double)valList.GetCount(), 0, 0, dp);
					nntxtAverage->SetText(CSTRP(sbuff, sptr));
				}
			}
			k++;
		}
		k = 0;
		while (k < assetCnt)
		{
			ass = assList.GetItemNoCheck(k);
			if (mgr->FindCurrency(ass->currency).SetTo(curr))
			{
				dateList.Clear();
				valList.Clear();
				mgr->AssetCalcValues(ass, startDate, endDate, dateList, valList, initValue);
				if (localCurr->c == curr->c)
				{
					initTotal += initValue;
				}
				else if (curr->c == mgr->GetRefCurrency())
				{
					rate = mgr->CurrencyGetRate(localCurr, Data::Timestamp::FromDate(startDate.AddDay(-1), Data::DateTimeUtil::GetLocalTzQhr()));
					initTotal += initValue * rate;
				}
				else
				{
					rate = mgr->CurrencyGetRate(localCurr, Data::Timestamp::FromDate(startDate.AddDay(-1), Data::DateTimeUtil::GetLocalTzQhr())) / mgr->CurrencyGetRate(curr, Data::Timestamp::FromDate(startDate.AddDay(-1), Data::DateTimeUtil::GetLocalTzQhr()));
					initTotal += initValue * rate;
				}

				i = 0;
				j = valList.GetCount();
				while (i < j)
				{
					dp = Data::Currency::GetDecimal(curr->c);
					sptr = Text::StrDoubleGDP(sbuff, valList.GetItem(i), 3, 0, dp);
					listView->SetSubItem(i, 3 + currencyCnt + k, CSTRP(sbuff, sptr));
					if (localCurr->c == curr->c)
					{
						totalList.SetItem(i, totalList.GetItem(i) + valList.GetItem(i));
					}
					else if (curr->c == mgr->GetRefCurrency())
					{
						rate = mgr->CurrencyGetRate(localCurr, dateList.GetItem(i));
						totalList.SetItem(i, totalList.GetItem(i) + valList.GetItem(i) * rate);
					}
					else
					{
						rate = mgr->CurrencyGetRate(localCurr, dateList.GetItem(i)) / mgr->CurrencyGetRate(curr, dateList.GetItem(i));
						totalList.SetItem(i, totalList.GetItem(i) + valList.GetItem(i) * rate);
					}
					i++;
				}
			}
			k++;
		}
		dp = Data::Currency::GetDecimal(localCurr->c);
		i = 0;
		j = totalList.GetCount();
		while (i < j)
		{
			sptr = Text::StrDoubleGDP(sbuff, totalList.GetItem(i) - initTotal, 3, 0, dp);
			listView->SetSubItem(i, 1, CSTRP(sbuff, sptr));
			sptr = Text::StrDoubleGDP(sbuff, totalList.GetItem(i), 3, dp, dp);
			listView->SetSubItem(i, 2, CSTRP(sbuff, sptr));
			totalList.SetItem(i, totalList.GetItem(i) - initTotal);
			i++;
		}
		NN<Data::ChartPlotter> chart;
		NEW_CLASSNN(chart, Data::ChartPlotter(0));
		chart->AddLineChart(CSTR(""), Data::ChartPlotter::NewData(totalList), Data::ChartPlotter::NewData(dateList), 0xffff0000);
		return chart;
	}
	return nullptr;
}

SSWR::AVIRead::AVIRInvestmentForm::AVIRInvestmentForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core) : UI::GUIForm(parent, 1024, 768, ui)
{
	this->SetFont(nullptr, 8.25, false);
	this->SetText(TITLE);

	this->core = core;
	this->mgr = nullptr;
	this->currencyImg = nullptr;
	this->assetsImg = nullptr;
	this->monthlyChart = nullptr;
	this->monthlyImg = nullptr;
	this->yearlyChart = nullptr;
	this->yearlyImg = nullptr;
	this->deng = core->GetDrawEngine();
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
	
	this->pnlDir = ui->NewPanel(*this);
	this->pnlDir->SetRect(0, 0, 100, 31, false);
	this->pnlDir->SetDockType(UI::GUIControl::DOCK_TOP);
	this->lblDir = ui->NewLabel(this->pnlDir, CSTR("Directory"));
	this->lblDir->SetRect(4, 4, 100, 23, false);
	this->txtDir = ui->NewTextBox(this->pnlDir, CSTR(""));
	this->txtDir->SetRect(104, 4, 400, 23, false);
	this->btnDir = ui->NewButton(this->pnlDir, CSTR("Load"));
	this->btnDir->SetRect(504, 4, 75, 23, false);
	this->btnDir->HandleButtonClick(OnDirClicked, this);
	this->tcMain = ui->NewTabControl(*this);
	this->tcMain->SetDockType(UI::GUIControl::DOCK_FILL);

	this->tpYearly = this->tcMain->AddTabPage(CSTR("Yearly"));
	this->pnlYearly = ui->NewPanel(this->tpYearly);
	this->pnlYearly->SetRect(0, 0, 100, 31, false);
	this->pnlYearly->SetDockType(UI::GUIControl::DOCK_TOP);
	this->lblYearly = ui->NewLabel(this->pnlYearly, CSTR("Year"));
	this->lblYearly->SetRect(4, 4, 100, 23, false);
	this->cboYearlyYear = ui->NewComboBox(this->pnlYearly, false);
	this->cboYearlyYear->SetRect(104, 4, 100, 23, false);
	this->cboYearlyYear->HandleSelectionChange(OnYearlySelChg, this);
	this->lblYearlyAvgInvestment = ui->NewLabel(this->pnlYearly, CSTR("Avg Invest"));
	this->lblYearlyAvgInvestment->SetRect(204, 4, 100, 23, false);
	this->txtYearlyAvgInvestment = ui->NewTextBox(this->pnlYearly, CSTR(""));
	this->txtYearlyAvgInvestment->SetRect(304, 4, 100, 23, false);
	this->txtYearlyAvgInvestment->SetReadOnly(true);
	this->lvYearly = ui->NewListView(this->tpYearly, UI::ListViewStyle::Table, 3);
	this->lvYearly->SetRect(0, 0, 100, 300, false);
	this->lvYearly->SetDockType(UI::GUIControl::DOCK_TOP);
	this->lvYearly->SetShowGrid(true);
	this->lvYearly->SetFullRowSelect(true);
	this->lvYearly->AddColumn(CSTR("Date"), 100);
	this->lvYearly->AddColumn(CSTR("Gain"), 100);
	this->lvYearly->AddColumn(CSTR("Total Gain"), 100);
	this->vspYearly = ui->NewVSplitter(this->tpYearly, 3, false);
	this->pnlYearlyGraph = ui->NewPanel(this->tpYearly);
	this->pnlYearlyGraph->SetRect(0, 0, 100, 31, false);
	this->pnlYearlyGraph->SetDockType(UI::GUIControl::DOCK_TOP);
	this->lblYearlyGrid = ui->NewLabel(this->pnlYearlyGraph, CSTR("Grid Lines"));
	this->lblYearlyGrid->SetRect(4, 4, 100, 23, false);
	this->cboYearlyGrid = ui->NewComboBox(this->pnlYearlyGraph, false);
	this->cboYearlyGrid->SetRect(104, 4, 150, 23, false);
	this->cboYearlyGrid->AddItem(CSTR("None"), (void*)Data::ChartPlotter::GridType::None);
	this->cboYearlyGrid->AddItem(CSTR("Horizontal"), (void*)Data::ChartPlotter::GridType::Horizontal);
	this->cboYearlyGrid->AddItem(CSTR("Vertical"), (void*)Data::ChartPlotter::GridType::Vertical);
	this->cboYearlyGrid->AddItem(CSTR("Both"), (void*)Data::ChartPlotter::GridType::Both);
	this->cboYearlyGrid->SetSelectedIndex(1);
	this->cboYearlyGrid->HandleSelectionChange(OnYearlyGridSelChg, this);
	this->pbYearly = ui->NewPictureBox(this->tpYearly, this->deng, false, false);
	this->pbYearly->SetDockType(UI::GUIControl::DOCK_FILL);
	this->pbYearly->HandleSizeChanged(OnYearlySizeChg, this);

	this->tpMonthly = this->tcMain->AddTabPage(CSTR("Monthly"));
	this->pnlMonthly = ui->NewPanel(this->tpMonthly);
	this->pnlMonthly->SetRect(0, 0, 100, 31, false);
	this->pnlMonthly->SetDockType(UI::GUIControl::DOCK_TOP);
	this->lblMonthly = ui->NewLabel(this->pnlMonthly, CSTR("Month"));
	this->lblMonthly->SetRect(4, 4, 100, 23, false);
	this->cboMonthlyYear = ui->NewComboBox(this->pnlMonthly, false);
	this->cboMonthlyYear->SetRect(104, 4, 100, 23, false);
	this->cboMonthlyYear->HandleSelectionChange(OnMonthlySelChg, this);
	this->cboMonthlyMonth = ui->NewComboBox(this->pnlMonthly, false);
	this->cboMonthlyMonth->SetRect(204, 4, 50, 23, false);
	this->cboMonthlyMonth->HandleSelectionChange(OnMonthlySelChg, this);
	this->lvMonthly = ui->NewListView(this->tpMonthly, UI::ListViewStyle::Table, 3);
	this->lvMonthly->SetRect(0, 0, 100, 300, false);
	this->lvMonthly->SetDockType(UI::GUIControl::DOCK_TOP);
	this->lvMonthly->SetShowGrid(true);
	this->lvMonthly->SetFullRowSelect(true);
	this->lvMonthly->AddColumn(CSTR("Date"), 100);
	this->lvMonthly->AddColumn(CSTR("Monthly Gain"), 100);
	this->lvMonthly->AddColumn(CSTR("Total Gain"), 100);
	this->vspMonthly = ui->NewVSplitter(this->tpMonthly, 3, false);
	this->pbMonthly = ui->NewPictureBox(this->tpMonthly, this->deng, false, false);
	this->pbMonthly->SetDockType(UI::GUIControl::DOCK_FILL);
	this->pbMonthly->HandleSizeChanged(OnMonthlySizeChg, this);

	this->tpTransaction = this->tcMain->AddTabPage(CSTR("Transaction"));
	this->pnlTransaction = ui->NewPanel(this->tpTransaction);
	this->pnlTransaction->SetRect(0, 0, 100, 31, false);
	this->pnlTransaction->SetDockType(UI::GUIControl::DOCK_TOP);
	this->btnTransactionFX = ui->NewButton(this->pnlTransaction, CSTR("FX"));
	this->btnTransactionFX->SetRect(4, 4, 75, 23, false);
	this->btnTransactionFX->HandleButtonClick(OnTransactionFXClicked, this);
	this->btnTransactionDeposit = ui->NewButton(this->pnlTransaction, CSTR("Deposit"));
	this->btnTransactionDeposit->SetRect(84, 4, 75, 23, false);
	this->btnTransactionDeposit->HandleButtonClick(OnTransactionDepositClicked, this);
	this->btnTransactionAsset = ui->NewButton(this->pnlTransaction, CSTR("Asset"));
	this->btnTransactionAsset->SetRect(164, 4, 75, 23, false);
	this->btnTransactionAsset->HandleButtonClick(OnTransactionAssetClicked, this);
	this->btnTransactionAInterest = ui->NewButton(this->pnlTransaction, CSTR("Asset Interest"));
	this->btnTransactionAInterest->SetRect(244, 4, 115, 23, false);
	this->btnTransactionAInterest->HandleButtonClick(OnTransactionAInterestClicked, this);
	this->btnTransactionCInterest = ui->NewButton(this->pnlTransaction, CSTR("Cash Interest"));
	this->btnTransactionCInterest->SetRect(364, 4, 115, 23, false);
	this->btnTransactionCInterest->HandleButtonClick(OnTransactionCInterestClicked, this);
	this->lvTransaction = ui->NewListView(this->tpTransaction, UI::ListViewStyle::Table, 10);
	this->lvTransaction->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lvTransaction->SetShowGrid(true);
	this->lvTransaction->SetFullRowSelect(true);
	this->lvTransaction->AddColumn(CSTR("Begin Date"), 70);
	this->lvTransaction->AddColumn(CSTR("End Date"), 70);
	this->lvTransaction->AddColumn(CSTR("Type"), 70);
	this->lvTransaction->AddColumn(CSTR("Asset 1"), 120);
	this->lvTransaction->AddColumn(CSTR("Amount 1"), 70);
	this->lvTransaction->AddColumn(CSTR("Cost 1"), 70);
	this->lvTransaction->AddColumn(CSTR("Asset 2"), 120);
	this->lvTransaction->AddColumn(CSTR("Amount 2"), 70);
	this->lvTransaction->AddColumn(CSTR("Cost 2"), 70);
	this->lvTransaction->AddColumn(CSTR("Ref Rate"), 70);
	this->lvTransaction->HandleDblClk(OnTransactionDblClk, this);

	this->tpCurrency = this->tcMain->AddTabPage(CSTR("Currency"));
	this->lbCurrency = ui->NewListBox(this->tpCurrency, false);
	this->lbCurrency->SetRect(0, 0, 75, 23, false);
	this->lbCurrency->SetDockType(UI::GUIControl::DOCK_LEFT);
	this->lbCurrency->HandleSelectionChange(OnCurrencySelChg, this);
	this->tcCurrency = ui->NewTabControl(this->tpCurrency);
	this->tcCurrency->SetDockType(UI::GUIControl::DOCK_FILL);
	this->tpCurrencySummary = this->tcCurrency->AddTabPage(CSTR("Summary"));
	this->pnlCurrency = ui->NewPanel(this->tpCurrencySummary);
	this->pnlCurrency->SetRect(0, 0, 100, 127, false);
	this->pnlCurrency->SetDockType(UI::GUIControl::DOCK_TOP);
	this->lblCurrencyCurr = ui->NewLabel(this->pnlCurrency, CSTR("Current Rate"));
	this->lblCurrencyCurr->SetRect(4, 4, 100, 23, false);
	this->txtCurrencyCurr = ui->NewTextBox(this->pnlCurrency, CSTR("-"));
	this->txtCurrencyCurr->SetRect(104, 4, 75, 23, false);
	this->txtCurrencyCurr->SetReadOnly(true);
	this->lblCurrencyTotal = ui->NewLabel(this->pnlCurrency, CSTR("Total Amount"));
	this->lblCurrencyTotal->SetRect(4, 28, 100, 23, false);
	this->txtCurrencyTotal = ui->NewTextBox(this->pnlCurrency, CSTR("-"));
	this->txtCurrencyTotal->SetRect(104, 28, 75, 23, false);
	this->txtCurrencyTotal->SetReadOnly(true);
	this->lblCurrencyValue = ui->NewLabel(this->pnlCurrency, CSTR("Ref Value"));
	this->lblCurrencyValue->SetRect(4, 52, 100, 23, false);
	this->txtCurrencyValue = ui->NewTextBox(this->pnlCurrency, CSTR("-"));
	this->txtCurrencyValue->SetRect(104, 52, 75, 23, false);
	this->txtCurrencyValue->SetReadOnly(true);
	this->btnCurrencyImport = ui->NewButton(this->pnlCurrency, CSTR("Import"));
	this->btnCurrencyImport->SetRect(4, 76, 75, 23, false);
	this->btnCurrencyImport->HandleButtonClick(OnCurrencyImportClicked, this);
	this->lblCurrencyRange = ui->NewLabel(this->pnlCurrency, CSTR("Range"));
	this->lblCurrencyRange->SetRect(4, 100, 100, 23, false);
	this->cboCurrencyRange = ui->NewComboBox(this->pnlCurrency, false);
	this->cboCurrencyRange->SetRect(104, 100, 150, 23, false);
	this->cboCurrencyRange->AddItem(CSTR("Max"), 0);
	this->cboCurrencyRange->AddItem(CSTR("1 Year"), 0);
	this->cboCurrencyRange->AddItem(CSTR("YTD"), 0);
	this->cboCurrencyRange->AddItem(CSTR("6 Months"), 0);
	this->cboCurrencyRange->AddItem(CSTR("3 Months"), 0);
	this->cboCurrencyRange->AddItem(CSTR("1 Months"), 0);
	this->cboCurrencyRange->SetSelectedIndex(0);
	this->cboCurrencyRange->HandleSelectionChange(OnCurrencySizeChg, this);
	this->pbCurrency = ui->NewPictureBox(this->tpCurrencySummary, this->deng, false, false);
	this->pbCurrency->SetDockType(UI::GUIControl::DOCK_FILL);
	this->pbCurrency->HandleSizeChanged(OnCurrencySizeChg, this);
	this->tpCurrencyHist = this->tcCurrency->AddTabPage(CSTR("History"));
	this->pnlCurrencyHist = ui->NewPanel(this->tpCurrencyHist);
	this->pnlCurrencyHist->SetRect(0, 0, 100, 31, false);
	this->pnlCurrencyHist->SetDockType(UI::GUIControl::DOCK_TOP);
	this->lblCurrencyHistDate = ui->NewLabel(this->pnlCurrencyHist, CSTR("Date"));
	this->lblCurrencyHistDate->SetRect(4, 4, 100, 23, false);
	this->txtCurrencyHistDate = ui->NewTextBox(this->pnlCurrencyHist, CSTR(""));
	this->txtCurrencyHistDate->SetRect(104, 4, 100, 23, false);
	this->lblCurrencyHistValue = ui->NewLabel(this->pnlCurrencyHist, CSTR("Value"));
	this->lblCurrencyHistValue->SetRect(204, 4, 100, 23, false);
	this->txtCurrencyHistValue = ui->NewTextBox(this->pnlCurrencyHist, CSTR(""));
	this->txtCurrencyHistValue->SetRect(304, 4, 100, 23, false);
	this->btnCurrencyHistUpdate = ui->NewButton(this->pnlCurrencyHist, CSTR("Update"));
	this->btnCurrencyHistUpdate->SetRect(404, 4, 75, 23, false);
	this->btnCurrencyHistUpdate->HandleButtonClick(OnCurrencyHistUpdateClicked, this);
	this->lvCurrencyHist = ui->NewListView(this->tpCurrencyHist, UI::ListViewStyle::Table, 2);
	this->lvCurrencyHist->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lvCurrencyHist->AddColumn(CSTR("Date"), 100);
	this->lvCurrencyHist->AddColumn(CSTR("Value"), 100);
	this->lvCurrencyHist->SetFullRowSelect(true);
	this->lvCurrencyHist->SetShowGrid(true);
	this->lvCurrencyHist->HandleSelChg(OnCurrencyHistSelChg, this);
	this->tpCurrencyTrade = this->tcCurrency->AddTabPage(CSTR("Trade"));
	this->lvCurrencyTrade = ui->NewListView(this->tpCurrencyTrade, UI::ListViewStyle::Table, 4);
	this->lvCurrencyTrade->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lvCurrencyTrade->SetFullRowSelect(true);
	this->lvCurrencyTrade->SetShowGrid(true);
	this->lvCurrencyTrade->AddColumn(CSTR("Date"), 70);
	this->lvCurrencyTrade->AddColumn(CSTR("Amount"), 100);
	this->lvCurrencyTrade->AddColumn(CSTR("Rate"), 100);
	this->lvCurrencyTrade->AddColumn(CSTR("Ref Amount"), 100);
	this->tpCurrencyTotal = this->tcCurrency->AddTabPage(CSTR("Total"));
	this->pnlCurrencyTotal = ui->NewPanel(this->tpCurrencyTotal);
	this->pnlCurrencyTotal->SetRect(0, 0, 100, 31, false);
	this->pnlCurrencyTotal->SetDockType(UI::GUIControl::DOCK_TOP);
	this->lblCurrencyTotalValue = ui->NewLabel(this->pnlCurrencyTotal, CSTR("Total"));
	this->lblCurrencyTotalValue->SetRect(4, 4, 100, 23, false);
	this->txtCurrencyTotalValue = ui->NewTextBox(this->pnlCurrencyTotal, CSTR(""));
	this->txtCurrencyTotalValue->SetRect(104, 4, 100, 23, false);
	this->txtCurrencyTotalValue->SetReadOnly(true);
	this->lvCurrencyTotal = ui->NewListView(this->tpCurrencyTotal, UI::ListViewStyle::Table, 2);
	this->lvCurrencyTotal->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lvCurrencyTotal->SetFullRowSelect(true);
	this->lvCurrencyTotal->SetShowGrid(true);
	this->lvCurrencyTotal->AddColumn(CSTR("Asset"), 150);
	this->lvCurrencyTotal->AddColumn(CSTR("Amount"), 100);

	this->tpAssets = this->tcMain->AddTabPage(CSTR("Assets"));
	this->pnlAssets = ui->NewPanel(this->tpAssets);
	this->pnlAssets->SetRect(0, 0, 150, 23, false);
	this->pnlAssets->SetDockType(UI::GUIControl::DOCK_LEFT);
	this->btnAssetsAdd = ui->NewButton(this->pnlAssets, CSTR("New"));
	this->btnAssetsAdd->SetRect(0, 0, 100, 23, false);
	this->btnAssetsAdd->SetDockType(UI::GUIControl::DOCK_TOP);
	this->btnAssetsAdd->HandleButtonClick(OnAssetsAddClicked, this);
	this->lbAssets = ui->NewListBox(this->pnlAssets, false);
	this->lbAssets->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lbAssets->HandleSelectionChange(OnAssetsSelChg, this);
	this->tcAssets = ui->NewTabControl(this->tpAssets);
	this->tcAssets->SetDockType(UI::GUIControl::DOCK_FILL);
	this->tpAssetsSummary = this->tcAssets->AddTabPage(CSTR("Summary"));
	this->pnlAssetsDetail = ui->NewPanel(this->tpAssetsSummary);
	this->pnlAssetsDetail->SetRect(0, 0, 100, 199, false);
	this->pnlAssetsDetail->SetDockType(UI::GUIControl::DOCK_TOP);
	this->lblAssetsShortName = ui->NewLabel(this->pnlAssetsDetail, CSTR("Short Name"));
	this->lblAssetsShortName->SetRect(4, 4, 100, 23, false);
	this->txtAssetsShortName = ui->NewTextBox(this->pnlAssetsDetail, CSTR(""));
	this->txtAssetsShortName->SetRect(104, 4, 150, 23, false);
	this->txtAssetsShortName->SetReadOnly(true);
	this->lblAssetsFullName = ui->NewLabel(this->pnlAssetsDetail, CSTR("Full Name"));
	this->lblAssetsFullName->SetRect(4, 28, 100, 23, false);
	this->txtAssetsFullName = ui->NewTextBox(this->pnlAssetsDetail, CSTR(""));
	this->txtAssetsFullName->SetRect(104, 28, 400, 23, false);
	this->txtAssetsFullName->SetReadOnly(true);
	this->lblAssetsCurrency = ui->NewLabel(this->pnlAssetsDetail, CSTR("Currency"));
	this->lblAssetsCurrency->SetRect(4, 52, 100, 23, false);
	this->txtAssetsCurrency = ui->NewTextBox(this->pnlAssetsDetail, CSTR(""));
	this->txtAssetsCurrency->SetRect(104, 52, 50, 23, false);
	this->txtAssetsCurrency->SetReadOnly(true);
	this->lblAssetsCurrent = ui->NewLabel(this->pnlAssetsDetail, CSTR("Current"));
	this->lblAssetsCurrent->SetRect(4, 76, 100, 23, false);
	this->txtAssetsCurrent = ui->NewTextBox(this->pnlAssetsDetail, CSTR(""));
	this->txtAssetsCurrent->SetRect(104, 76, 50, 23, false);
	this->txtAssetsCurrent->SetReadOnly(true);
	this->lblAssetsAmount = ui->NewLabel(this->pnlAssetsDetail, CSTR("Amount"));
	this->lblAssetsAmount->SetRect(4, 100, 100, 23, false);
	this->txtAssetsAmount = ui->NewTextBox(this->pnlAssetsDetail, CSTR(""));
	this->txtAssetsAmount->SetRect(104, 100, 50, 23, false);
	this->txtAssetsAmount->SetReadOnly(true);
	this->lblAssetsValue = ui->NewLabel(this->pnlAssetsDetail, CSTR("Value"));
	this->lblAssetsValue->SetRect(4, 124, 100, 23, false);
	this->txtAssetsValue = ui->NewTextBox(this->pnlAssetsDetail, CSTR(""));
	this->txtAssetsValue->SetRect(104, 124, 50, 23, false);
	this->txtAssetsValue->SetReadOnly(true);
	this->btnAssetsImport = ui->NewButton(this->pnlAssetsDetail, CSTR("Import"));
	this->btnAssetsImport->SetRect(104, 148, 75, 23, false);
	this->btnAssetsImport->HandleButtonClick(OnAssetsImportClicked, this);
	this->btnAssetsImportDiv = ui->NewButton(this->pnlAssetsDetail, CSTR("Import Div"));
	this->btnAssetsImportDiv->SetRect(184, 148, 75, 23, false);
	this->btnAssetsImportDiv->HandleButtonClick(OnAssetsImportDivClicked, this);
	this->lblAssetsRange = ui->NewLabel(this->pnlAssetsDetail, CSTR("Range"));
	this->lblAssetsRange->SetRect(4, 172, 100, 23, false);
	this->cboAssetsRange = ui->NewComboBox(this->pnlAssetsDetail, false);
	this->cboAssetsRange->SetRect(104, 172, 150, 23, false);
	this->cboAssetsRange->AddItem(CSTR("Max"), 0);
	this->cboAssetsRange->AddItem(CSTR("1 Year"), 0);
	this->cboAssetsRange->AddItem(CSTR("YTD"), 0);
	this->cboAssetsRange->AddItem(CSTR("6 Months"), 0);
	this->cboAssetsRange->AddItem(CSTR("3 Months"), 0);
	this->cboAssetsRange->AddItem(CSTR("1 Months"), 0);
	this->cboAssetsRange->SetSelectedIndex(0);
	this->cboAssetsRange->HandleSelectionChange(OnAssetsSizeChg, this);
	this->pbAssets = ui->NewPictureBox(this->tpAssetsSummary, this->deng, false, false);
	this->pbAssets->SetDockType(UI::GUIControl::DOCK_FILL);
	this->pbAssets->HandleSizeChanged(OnAssetsSizeChg, this);
	this->tpAssetsHist = this->tcAssets->AddTabPage(CSTR("History"));
	this->pnlAssetsHist = ui->NewPanel(this->tpAssetsHist);
	this->pnlAssetsHist->SetRect(0, 0, 100, 47, false);
	this->pnlAssetsHist->SetDockType(UI::GUIControl::DOCK_TOP);
	this->lblAssetsHistDate = ui->NewLabel(this->pnlAssetsHist, CSTR("Date"));
	this->lblAssetsHistDate->SetRect(0, 0, 100, 23, false);
	this->txtAssetsHistDate = ui->NewTextBox(this->pnlAssetsHist, CSTR(""));
	this->txtAssetsHistDate->SetRect(0, 24, 100, 23, false);
	this->lblAssetsHistValue = ui->NewLabel(this->pnlAssetsHist, CSTR("Value"));
	this->lblAssetsHistValue->SetRect(100, 0, 50, 23, false);
	this->txtAssetsHistValue = ui->NewTextBox(this->pnlAssetsHist, CSTR(""));
	this->txtAssetsHistValue->SetRect(100, 24, 50, 23, false);
	this->lblAssetsHistDiv = ui->NewLabel(this->pnlAssetsHist, CSTR("Div"));
	this->lblAssetsHistDiv->SetRect(150, 0, 50, 23, false);
	this->txtAssetsHistDiv = ui->NewTextBox(this->pnlAssetsHist, CSTR(""));
	this->txtAssetsHistDiv->SetRect(150, 24, 50, 23, false);
	this->btnAssetsHistUpdate = ui->NewButton(this->pnlAssetsHist, CSTR("Update"));
	this->btnAssetsHistUpdate->SetRect(200, 24, 75, 23, false);
	this->btnAssetsHistUpdate->HandleButtonClick(OnAssetsHistUpdateClicked, this);
	this->lvAssetsHist = ui->NewListView(this->tpAssetsHist, UI::ListViewStyle::Table, 3);
	this->lvAssetsHist->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lvAssetsHist->AddColumn(CSTR("Date"), 100);
	this->lvAssetsHist->AddColumn(CSTR("Value"), 100);
	this->lvAssetsHist->AddColumn(CSTR("Div"), 100);
	this->lvAssetsHist->SetShowGrid(true);
	this->lvAssetsHist->SetFullRowSelect(true);
	this->lvAssetsHist->HandleSelChg(OnAssetsHistSelChg, this);
	this->tpAssetsTrade = this->tcAssets->AddTabPage(CSTR("Trade"));
	this->lvAssetsTrade = ui->NewListView(this->tpAssetsTrade, UI::ListViewStyle::Table, 1);
	this->lvAssetsTrade->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lvAssetsTrade->SetShowGrid(true);
	this->lvAssetsTrade->SetFullRowSelect(true);

	this->tpSettings = this->tcMain->AddTabPage(CSTR("Settings"));
	this->lblLocalCurrency = ui->NewLabel(this->tpSettings, CSTR("Local Currency"));
	this->lblLocalCurrency->SetRect(4, 4, 100, 23, false);
	this->txtLocalCurrency = ui->NewTextBox(this->tpSettings, CSTR(""));
	this->txtLocalCurrency->SetRect(104, 4, 100, 23, false);
	this->txtLocalCurrency->SetReadOnly(true);
	this->lblRefCurrency = ui->NewLabel(this->tpSettings, CSTR("Ref Currency"));
	this->lblRefCurrency->SetRect(4, 28, 100, 23, false);
	this->txtRefCurrency = ui->NewTextBox(this->tpSettings, CSTR(""));
	this->txtRefCurrency->SetRect(104, 28, 100, 23, false);
	this->txtRefCurrency->SetReadOnly(true);

	NN<IO::Registry> reg;
	if (IO::Registry::OpenSoftware(IO::Registry::REG_USER_THIS, L"SSWR", L"AVIRead").SetTo(reg))
	{
		WChar wbuff[512];
		UnsafeArray<WChar> wptr;
		if (reg->GetValueStr(L"Investment", wbuff).SetTo(wptr))
		{
			NN<Text::String> s = Text::String::NewW(wbuff, (UOSInt)(wptr - wbuff));
			this->txtDir->SetText(s->ToCString());
			s->Release();
			OnDirClicked(this);
		}
		IO::Registry::CloseRegistry(reg);
	}
}

SSWR::AVIRead::AVIRInvestmentForm::~AVIRInvestmentForm()
{
	this->ClearChildren();
	this->mgr.Delete();
	this->currencyImg.Delete();
	this->assetsImg.Delete();
	this->monthlyImg.Delete();
	this->monthlyChart.Delete();
	this->yearlyImg.Delete();
	this->yearlyChart.Delete();
}

void SSWR::AVIRead::AVIRInvestmentForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
