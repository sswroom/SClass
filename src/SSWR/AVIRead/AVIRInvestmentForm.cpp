#include "Stdafx.h"
#include "Data/ChartPlotter.h"
#include "IO/Path.h"
#include "IO/Registry.h"
#include "SSWR/AVIRead/AVIRInvestmentAccountForm.h"
#include "SSWR/AVIRead/AVIRInvestmentForm.h"
#include "SSWR/AVIRead/AVIRInvestmentImportForm.h"
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
	UOSInt i = 0;
	UOSInt j = mgr->GetAccountCount();
	while (i < j)
	{
		me->lbAccounts->AddItem(Text::String::OrEmpty(mgr->GetAccount(i)), 0);
		i++;
	}
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
			if (mgr->CurrencyImport(curr, db, frm->GetTimeCol(), frm->GetValueCol()))
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

void __stdcall SSWR::AVIRead::AVIRInvestmentForm::OnAccountsClicked(AnyType userObj)
{
	NN<AVIRInvestmentForm> me = userObj.GetNN<AVIRInvestmentForm>();
	NN<Data::Invest::InvestmentManager> mgr;
	if (me->mgr.SetTo(mgr))
	{
		AVIRInvestmentAccountForm frm(0, me->ui, me->core);
		NN<Text::String> s;
		if (frm.ShowDialog(me) == UI::GUIForm::DR_OK && frm.GetInputName().SetTo(s))
		{
			if (mgr->AddAccount(s))
			{
				me->lbAccounts->AddItem(s, 0);
			}
		}
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

void SSWR::AVIRead::AVIRInvestmentForm::DisplayCurrency(NN<Data::Invest::Currency> curr)
{
	UTF8Char sbuff[64];
	UnsafeArray<UTF8Char> sptr;
	sptr = Text::StrDouble(sbuff, curr->current);
	this->txtCurrencyCurr->SetText(CSTRP(sbuff, sptr));
	this->DisplayCurrencyImg(curr);
}

void SSWR::AVIRead::AVIRInvestmentForm::DisplayCurrencyImg(NN<Data::Invest::Currency> curr)
{
	Math::Size2D<UOSInt> sz = this->pbCurrency->GetSizeP();
	NN<Media::DrawImage> dimg;
	NN<Media::StaticImage> simg;
	if (this->deng->CreateImage32(sz, Media::AT_IGNORE_ALPHA).SetTo(dimg))
	{
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

SSWR::AVIRead::AVIRInvestmentForm::AVIRInvestmentForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core) : UI::GUIForm(parent, 1024, 768, ui)
{
	this->SetFont(0, 0, 8.25, false);
	this->SetText(TITLE);

	this->core = core;
	this->mgr = 0;
	this->currencyImg = 0;
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

	this->tpCurrency = this->tcMain->AddTabPage(CSTR("Currency"));
	this->lbCurrency = ui->NewListBox(this->tpCurrency, false);
	this->lbCurrency->SetRect(0, 0, 75, 23, false);
	this->lbCurrency->SetDockType(UI::GUIControl::DOCK_LEFT);
	this->lbCurrency->HandleSelectionChange(OnCurrencySelChg, this);
	this->pnlCurrency = ui->NewPanel(this->tpCurrency);
	this->pnlCurrency->SetRect(0, 0, 100, 55, false);
	this->pnlCurrency->SetDockType(UI::GUIControl::DOCK_TOP);
	this->lblCurrencyCurr = ui->NewLabel(this->pnlCurrency, CSTR("Current Rate"));
	this->lblCurrencyCurr->SetRect(4, 4, 100, 23, false);
	this->txtCurrencyCurr = ui->NewTextBox(this->pnlCurrency, CSTR("-"));
	this->txtCurrencyCurr->SetRect(104, 4, 75, 23, false);
	this->txtCurrencyCurr->SetReadOnly(true);
	this->btnCurrencyImport = ui->NewButton(this->pnlCurrency, CSTR("Import"));
	this->btnCurrencyImport->SetRect(4, 28, 75, 23, false);
	this->btnCurrencyImport->HandleButtonClick(OnCurrencyImportClicked, this);
	this->pbCurrency = ui->NewPictureBox(this->tpCurrency, this->deng, false, false);
	this->pbCurrency->SetDockType(UI::GUIControl::DOCK_FILL);
	this->pbCurrency->HandleSizeChanged(OnCurrencySizeChg, this);

	this->tpAccounts = this->tcMain->AddTabPage(CSTR("Accounts"));
	this->pnlAccounts = ui->NewPanel(this->tpAccounts);
	this->pnlAccounts->SetRect(0, 0, 100, 31, false);
	this->pnlAccounts->SetDockType(UI::GUIControl::DOCK_TOP);
	this->btnAccounts = ui->NewButton(this->pnlAccounts, CSTR("Add"));
	this->btnAccounts->SetRect(4, 4, 75, 23, false);
	this->btnAccounts->HandleButtonClick(OnAccountsClicked, this);
	this->lbAccounts = ui->NewListBox(this->tpAccounts, false);
	this->lbAccounts->SetDockType(UI::GUIControl::DOCK_FILL);

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
}

void SSWR::AVIRead::AVIRInvestmentForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
