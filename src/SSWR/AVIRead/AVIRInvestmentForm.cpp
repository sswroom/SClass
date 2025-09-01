#include "Stdafx.h"
#include "Data/ChartPlotter.h"
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
		NEW_CLASSNN(frm, AVIRInvestmentImportForm(0, me->ui, me->core));
		if (frm->ShowDialog(me) == UI::GUIForm::DR_OK && frm->GetDB().SetTo(db))
		{
			if (mgr->CurrencyImport(curr, db, frm->GetTimeCol(), frm->GetValueCol(), frm->GetDateFormat()))
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
		AVIRInvestmentAssetForm frm(0, me->ui, me->core);
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
		NEW_CLASSNN(frm, AVIRInvestmentImportForm(0, me->ui, me->core));
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
		NEW_CLASSNN(frm, AVIRInvestmentImportForm(0, me->ui, me->core));
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
		AVIRInvestmentFXForm frm(0, me->ui, me->core, mgr);
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
		AVIRInvestmentDepositForm frm(0, me->ui, me->core, mgr);
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
		AVIRInvestmentTAssetForm frm(0, me->ui, me->core, mgr);
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
		AVIRInvestmentAInterestForm frm(0, me->ui, me->core, mgr);
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
		AVIRInvestmentCInterestForm frm(0, me->ui, me->core, mgr);
		if (frm.ShowDialog(me) == UI::GUIForm::DR_OK)
		{
			me->DisplayTransactions(mgr);
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

void SSWR::AVIRead::AVIRInvestmentForm::DisplayCurrency(NN<Data::Invest::Currency> curr)
{
	UTF8Char sbuff[64];
	UnsafeArray<UTF8Char> sptr;
	sptr = Text::StrDouble(sbuff, curr->current);
	this->txtCurrencyCurr->SetText(CSTRP(sbuff, sptr));
	Double val = 0;
	UOSInt i;
	UOSInt j;
	i = 0;
	j = curr->trades.GetCount();
	while (i < j)
	{
		val += curr->trades.GetItemNoCheck(i)->amount;
		i++;
	}
	sptr = Text::StrDouble(sbuff, val);
	this->txtCurrencyTotal->SetText(CSTRP(sbuff, sptr));
	NN<Data::Invest::InvestmentManager> mgr;
	if (this->mgr.SetTo(mgr))
	{
		if (curr->c == mgr->GetRefCurrency())
		{
			this->txtCurrencyValue->SetText(CSTRP(sbuff, sptr));
		}
		else
		{
			sptr = Text::StrDouble(sbuff, val / curr->current);
			this->txtCurrencyValue->SetText(CSTRP(sbuff, sptr));
		}
	}
	this->lvCurrencyHist->ClearItems();
	i = 0;
	j = curr->tsList.GetCount();
	UOSInt k;
	if (j > 20)
	{
		i = j - 20;
	}
	while (i < j)
	{
		sptr = curr->tsList.GetItem(i).ToString(sbuff, "yyyy-MM-dd HH:mm");
		k = this->lvCurrencyHist->AddItem(CSTRP(sbuff, sptr), 0);
		sptr = Text::StrDouble(sbuff, curr->valList.GetItem(i));
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
			dimg->DrawRect(Math::Coord2DDbl(0, 0), sz.ToDouble(), 0, b);
			dimg->DelBrush(b);
		}
		else
		{
			Data::ChartPlotter chart(0);
			chart.AddLineChart(CSTR(""), Data::ChartPlotter::NewData(curr->valList), Data::ChartPlotter::NewData(curr->tsList), 0xffff0000);
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

void SSWR::AVIRead::AVIRInvestmentForm::DisplayAsset(NN<Data::Invest::Asset> ass)
{
	UTF8Char sbuff[64];
	UnsafeArray<UTF8Char> sptr;
	UOSInt i;
	UOSInt j;
	UOSInt k;
	NN<Data::Invest::TradeDetail> t;
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
		sptr = ass->tsList.GetItem(i).ToString(sbuff, "yyyy-MM-dd HH:mm");
		k = this->lvAssetsHist->AddItem(CSTRP(sbuff, sptr), 0);
		sptr = Text::StrDouble(sbuff, ass->valList.GetItem(i));
		this->lvAssetsHist->SetSubItem(k, 1, CSTRP(sbuff, sptr));
		sptr = Text::StrDouble(sbuff, ass->divList.GetItem(i));
		this->lvAssetsHist->SetSubItem(k, 2, CSTRP(sbuff, sptr));
		i++;
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
			dimg->DrawRect(Math::Coord2DDbl(0, 0), sz.ToDouble(), 0, b);
			dimg->DelBrush(b);
		}
		else
		{
			Data::ChartPlotter chart(0);
			chart.AddLineChart(CSTR(""), Data::ChartPlotter::NewData(ass->valList), Data::ChartPlotter::NewData(ass->tsList), 0xffff0000);
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
				UInt32 c = (UInt32)ent->fromIndex;
				this->lvTransaction->SetSubItem(k, 3, CURRENCYSTR(c));
				c = (UInt32)ent->toIndex;
				this->lvTransaction->SetSubItem(k, 6, CURRENCYSTR(c));
			}
			else if (ent->type == Data::Invest::TradeType::FixedDeposit)
			{
				UInt32 c = (UInt32)ent->fromIndex;
				this->lvTransaction->SetSubItem(k, 3, CURRENCYSTR(c));
				c = (UInt32)ent->toIndex;
				this->lvTransaction->SetSubItem(k, 6, CURRENCYSTR(c));
			}
			else if (ent->type == Data::Invest::TradeType::CashToAsset)
			{
				UInt32 c = (UInt32)ent->fromIndex;
				this->lvTransaction->SetSubItem(k, 3, CURRENCYSTR(c));
				NN<Data::Invest::Asset> ass;
				if (mgr->GetAsset(ent->toIndex).SetTo(ass))
				{
					this->lvTransaction->SetSubItem(k, 6, ass->shortName);
				}
			}
			else if (ent->type == Data::Invest::TradeType::AssetInterest)
			{
				NN<Data::Invest::Asset> ass;
				if (mgr->GetAsset(ent->fromIndex).SetTo(ass))
				{
					this->lvTransaction->SetSubItem(k, 3, ass->shortName);
				}
				UInt32 c = (UInt32)ent->toIndex;
				this->lvTransaction->SetSubItem(k, 6, CURRENCYSTR(c));
			}
			else if (ent->type == Data::Invest::TradeType::AccountInterest)
			{
				UInt32 c = (UInt32)ent->fromIndex;
				this->lvTransaction->SetSubItem(k, 3, CURRENCYSTR(c));
				c = (UInt32)ent->toIndex;
				this->lvTransaction->SetSubItem(k, 6, CURRENCYSTR(c));
			}
			sptr = Text::StrDouble(sbuff, ent->fromDetail.amount);
			this->lvTransaction->SetSubItem(k, 4, CSTRP(sbuff, sptr));
			sptr = Text::StrDouble(sbuff, ent->fromDetail.cost);
			this->lvTransaction->SetSubItem(k, 5, CSTRP(sbuff, sptr));
			sptr = Text::StrDouble(sbuff, ent->toDetail.amount);
			this->lvTransaction->SetSubItem(k, 7, CSTRP(sbuff, sptr));
			sptr = Text::StrDouble(sbuff, ent->toDetail.cost);
			this->lvTransaction->SetSubItem(k, 8, CSTRP(sbuff, sptr));
			sptr = Text::StrDouble(sbuff, ent->refRate);
			this->lvTransaction->SetSubItem(k, 9, CSTRP(sbuff, sptr));
		}
		i++;
	}
}

void SSWR::AVIRead::AVIRInvestmentForm::DisplayMonthly(NN<Data::Invest::InvestmentManager> mgr, Int32 year, UInt8 month)
{
	NN<Data::Invest::Currency> curr;
	NN<Data::Invest::Asset> ass;
	UTF8Char sbuff[64];
	UnsafeArray<UTF8Char> sptr;
	UOSInt i;
	UOSInt j;
	UOSInt k;
	UOSInt currencyCnt = mgr->GetCurrencyCount();
	UOSInt assetCnt = mgr->GetAssetCount();
	this->lvMonthly->ClearAll();
	this->lvMonthly->ChangeColumnCnt(currencyCnt + assetCnt + 2);
	this->lvMonthly->AddColumn(CSTR("Date"), 100);
	this->lvMonthly->AddColumn(CSTR("Gain"), 100);
	i = 0;
	while (i < currencyCnt)
	{
		if (mgr->GetCurrencyInfo(i).SetTo(curr))
		{
			this->lvMonthly->AddColumn(CURRENCYSTR(curr->c), 50);
		}
		i++;
	}
	i = 0;
	while (i < assetCnt)
	{
		if (mgr->GetAsset(i).SetTo(ass))
		{
			this->lvMonthly->AddColumn(ass->shortName, 100);
		}
		i++;
	}
	Data::Date startDate = Data::Date(year, month, 1);
	Data::Date endDate;
	Data::Date now = Data::Date::Today();
	if (startDate >= now)
	{
		return;
	}
	endDate = startDate;
	endDate.SetMonth(startDate.GetDateValue().month + 1);
	if (endDate > now)
	{
		endDate = now;
	}
	NN<Data::Invest::Currency> localCurr;
	Double rate;
	Data::ArrayListTS dateList;
	Data::ArrayList<Double> totalList;
	Data::ArrayList<Double> valList;
	if (mgr->GetCurrencyInfo(0).SetTo(curr) && mgr->FindCurrency(mgr->GetLocalCurrency()).SetTo(localCurr))
	{
		mgr->CurrencyCalcValues(curr, startDate, endDate, dateList, valList);
		i = 0;
		j = dateList.GetCount();
		while (i < j)
		{
			sptr = dateList.GetItem(i).ToString(sbuff, "yyyy-MM-dd");
			this->lvMonthly->AddItem(CSTRP(sbuff, sptr), 0);
			totalList.Add(0);
			i++;
		}
		k = 0;
		while (k < currencyCnt)
		{
			if (mgr->GetCurrencyInfo(k).SetTo(curr))
			{
				dateList.Clear();
				valList.Clear();
				mgr->CurrencyCalcValues(curr, startDate, endDate, dateList, valList);
				i = 0;
				j = valList.GetCount();
				while (i < j)
				{
					sptr = Text::StrDouble(sbuff, valList.GetItem(i));
					this->lvMonthly->SetSubItem(i, 2 + k, CSTRP(sbuff, sptr));
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
		k = 0;
		while (k < assetCnt)
		{
			if (mgr->GetAsset(k).SetTo(ass) && mgr->FindCurrency(ass->currency).SetTo(curr))
			{
				dateList.Clear();
				valList.Clear();
				mgr->AssetCalcValues(ass, startDate, endDate, dateList, valList);
				i = 0;
				j = valList.GetCount();
				while (i < j)
				{
					sptr = Text::StrDouble(sbuff, valList.GetItem(i));
					this->lvMonthly->SetSubItem(i, 2 + currencyCnt + k, CSTRP(sbuff, sptr));
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
		i = 0;
		j = totalList.GetCount();
		while (i < j)
		{
			sptr = Text::StrDouble(sbuff, totalList.GetItem(i));
			this->lvMonthly->SetSubItem(i, 1, CSTRP(sbuff, sptr));
			i++;
		}
		NN<Data::ChartPlotter> chart;
		NEW_CLASSNN(chart, Data::ChartPlotter(0));
		chart->AddLineChart(CSTR(""), Data::ChartPlotter::NewData(totalList), Data::ChartPlotter::NewData(dateList), 0xffff0000);
		this->monthlyChart.Delete();
		this->monthlyChart = chart;
		this->DisplayMonthlyImg();
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

SSWR::AVIRead::AVIRInvestmentForm::AVIRInvestmentForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core) : UI::GUIForm(parent, 1024, 768, ui)
{
	this->SetFont(0, 0, 8.25, false);
	this->SetText(TITLE);

	this->core = core;
	this->mgr = 0;
	this->currencyImg = 0;
	this->assetsImg = 0;
	this->monthlyChart = 0;
	this->monthlyImg = 0;
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
	this->lvMonthly = ui->NewListView(this->tpMonthly, UI::ListViewStyle::Table, 2);
	this->lvMonthly->SetRect(0, 0, 100, 300, false);
	this->lvMonthly->SetDockType(UI::GUIControl::DOCK_TOP);
	this->lvMonthly->SetShowGrid(true);
	this->lvMonthly->SetFullRowSelect(true);
	this->lvMonthly->AddColumn(CSTR("Date"), 100);
	this->lvMonthly->AddColumn(CSTR("Gain"), 100);
	this->vspMonthly = ui->NewVSplitter(this->tpMonthly, 3, false);
	this->pbMonthly = ui->NewPictureBox(this->tpMonthly, this->deng, false, false);
	this->pbMonthly->SetDockType(UI::GUIControl::DOCK_FILL);

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

	this->tpCurrency = this->tcMain->AddTabPage(CSTR("Currency"));
	this->lbCurrency = ui->NewListBox(this->tpCurrency, false);
	this->lbCurrency->SetRect(0, 0, 75, 23, false);
	this->lbCurrency->SetDockType(UI::GUIControl::DOCK_LEFT);
	this->lbCurrency->HandleSelectionChange(OnCurrencySelChg, this);
	this->tcCurrency = ui->NewTabControl(this->tpCurrency);
	this->tcCurrency->SetDockType(UI::GUIControl::DOCK_FILL);
	this->tpCurrencySummary = this->tcCurrency->AddTabPage(CSTR("Summary"));
	this->pnlCurrency = ui->NewPanel(this->tpCurrencySummary);
	this->pnlCurrency->SetRect(0, 0, 100, 103, false);
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
	this->pnlAssetsDetail->SetRect(0, 0, 100, 175, false);
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
}

void SSWR::AVIRead::AVIRInvestmentForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
