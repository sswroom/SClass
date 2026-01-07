#include "Stdafx.h"
#include "DB/CSVFile.h"
#include "SSWR/AVIRead/AVIRInvestmentImportForm.h"

#define TITLE CSTR("Investment Import")

void __stdcall SSWR::AVIRead::AVIRInvestmentImportForm::OnOKClicked(AnyType userObj)
{
	NN<AVIRInvestmentImportForm> me = userObj.GetNN<AVIRInvestmentImportForm>();
	if (me->db.NotNull())
	{
		me->timeCol = me->cboTimeCol->GetSelectedIndex();
		me->valueCol = me->cboValueCol->GetSelectedIndex();
		me->fmt = (Data::Invest::DateFormat)me->cboDateFormat->GetSelectedItem().GetOSInt();
		me->invert = me->chkInvert->IsChecked();
		me->SetDialogResult(UI::GUIForm::DR_OK);
	}
}

void __stdcall SSWR::AVIRead::AVIRInvestmentImportForm::OnCancelClicked(AnyType userObj)
{
	NN<AVIRInvestmentImportForm> me = userObj.GetNN<AVIRInvestmentImportForm>();
	me->SetDialogResult(UI::GUIForm::DR_CANCEL);
}

void __stdcall SSWR::AVIRead::AVIRInvestmentImportForm::OnFile(AnyType userObj, Data::DataArray<NN<Text::String>> files)
{
	NN<AVIRInvestmentImportForm> me = userObj.GetNN<AVIRInvestmentImportForm>();
	UOSInt i = 0;
	UOSInt j = files.GetCount();
	while (i < j)
	{
		me->LoadFile(files[i]->ToCString());
		i++;
	}
}

void SSWR::AVIRead::AVIRInvestmentImportForm::LoadFile(Text::CStringNN fileName)
{
	if (fileName.EndsWith(CSTR(".csv")))
	{
		NN<DB::CSVFile> db;
		NEW_CLASSNN(db, DB::CSVFile(fileName, 0));
		NN<DB::DBReader> r;
		if (db->QueryTableData(nullptr, CSTR(""), 0, 0, 0, nullptr, nullptr).SetTo(r))
		{
			UTF8Char sbuff[512];
			UnsafeArray<UTF8Char> sptr;
			NN<Text::String> s;
			Bool succ = false;
			UOSInt i = 0;
			UOSInt j = r->ColCount();
			UOSInt k;
			UOSInt l;
			if (j > 0)
			{
				UOSInt timeCol = 0;
				UOSInt priceCol = 0;
				Data::Invest::DateFormat fmt = Data::Invest::DateFormat::MMDDYYYY;
				succ = true;
				this->cboTimeCol->ClearItems();
				this->cboValueCol->ClearItems();
				this->lvMain->ChangeColumnCnt(j);
				this->lvMain->ClearAll();
				while (i < j)
				{
					if (r->GetName(i, sbuff).SetTo(sptr))
					{
						this->cboTimeCol->AddItem(CSTRP(sbuff, sptr), 0);
						this->cboValueCol->AddItem(CSTRP(sbuff, sptr), 0);
						this->lvMain->AddColumn(CSTRP(sbuff, sptr), 60);
						if (CSTRP(sbuff, sptr).IndexOfICase(CSTR("DATE")) != INVALID_INDEX)
						{
							timeCol = i;
							fmt = Data::Invest::DateFormat::MMDDYYYY;
						}
						else if (CSTRP(sbuff, sptr).IndexOfICase(CSTR("TIME")) != INVALID_INDEX)
						{
							timeCol = i;
							fmt = Data::Invest::DateFormat::MMDDYYYY;
						}
						else if (CSTRP(sbuff, sptr).Equals(CSTR("日期")))
						{
							timeCol = i;
							fmt = Data::Invest::DateFormat::DDMMYYYY;
						}
						else if (CSTRP(sbuff, sptr).Equals(CSTR("除息日")))
						{
							timeCol = i;
							fmt = Data::Invest::DateFormat::DDMMYYYY;
						}
						else if (CSTRP(sbuff, sptr).IndexOfICase(CSTR("PRICE")) != INVALID_INDEX)
						{
							priceCol = i;
						}
						else if (CSTRP(sbuff, sptr).Equals(CSTR("資產淨值")))
						{
							priceCol = i;
						}
						else if (CSTRP(sbuff, sptr).Equals(CSTR("單位派息")))
						{
							priceCol = i;
						}
					}
					else
					{
						this->cboTimeCol->AddItem(CSTR(""), 0);
						this->cboValueCol->AddItem(CSTR(""), 0);
						this->lvMain->AddColumn(CSTR("Column"), 60);
					}
					i++;
				}
				this->cboTimeCol->SetSelectedIndex(timeCol);
				this->cboValueCol->SetSelectedIndex(priceCol);
				this->cboDateFormat->SetSelectedIndex((UOSInt)fmt);
				k = 100;
				while (k-- > 0 && r->ReadNext())
				{
					if (r->GetNewStr(0).SetTo(s))
					{
						l = this->lvMain->AddItem(s->ToCString(), 0);
						s->Release();
					}
					else
					{
						l = this->lvMain->AddItem(CSTR("(null)"), 0);
					}
					i = 1;
					while (i < j)
					{
						if (r->GetNewStr(i).SetTo(s))
						{
							this->lvMain->SetSubItem(l, i, s->ToCString());
							s->Release();
						}
						else
						{
							this->lvMain->SetSubItem(l, i, CSTR("(null)"));
						}
						i++;
					}
				}
			}
			db->CloseReader(r);
			if (succ)
			{
				this->db.Delete();
				this->db = db;
			}
			else
			{
				db.Delete();
			}
		}
	}
}

SSWR::AVIRead::AVIRInvestmentImportForm::AVIRInvestmentImportForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core, Bool invert) : UI::GUIForm(parent, 1024, 768, ui)
{
	this->SetFont(nullptr, 8.25, false);
	this->SetText(TITLE);

	this->core = core;
	this->db = 0;
	this->timeCol = 0;
	this->valueCol = 0;
	this->invert = invert;
	this->fmt = Data::Invest::DateFormat::MMDDYYYY;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	this->pnlMain = ui->NewPanel(*this);
	this->pnlMain->SetRect(0, 0, 100, 127, false);
	this->pnlMain->SetDockType(UI::GUIControl::DOCK_BOTTOM);
	this->lvMain = ui->NewListView(*this, UI::ListViewStyle::Table, 1);
	this->lvMain->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lvMain->SetFullRowSelect(true);
	this->lvMain->SetShowGrid(true);
	this->lvMain->AddColumn(CSTR(""), 100);
	this->lblTimeCol = ui->NewLabel(this->pnlMain, CSTR("Time Column"));
	this->lblTimeCol->SetRect(4, 4, 100, 23, false);
	this->cboTimeCol = ui->NewComboBox(this->pnlMain, false);
	this->cboTimeCol->SetRect(104, 4, 200, 23, false);
	this->lblValueCol = ui->NewLabel(this->pnlMain, CSTR("Value Column"));
	this->lblValueCol->SetRect(4, 28, 100, 23, false);
	this->cboValueCol = ui->NewComboBox(this->pnlMain, false);
	this->cboValueCol->SetRect(104, 28, 200, 23, false);
	this->lblDateFormat = ui->NewLabel(this->pnlMain, CSTR("Date Format"));
	this->lblDateFormat->SetRect(4, 52, 100, 23, false);
	this->cboDateFormat = ui->NewComboBox(this->pnlMain, false);
	this->cboDateFormat->SetRect(104, 52, 200, 23, false);
	this->cboDateFormat->AddItem(CSTR("MMDDYYYY"), (void*)(OSInt)Data::Invest::DateFormat::MMDDYYYY);
	this->cboDateFormat->AddItem(CSTR("DDMMYYYY"), (void*)(OSInt)Data::Invest::DateFormat::DDMMYYYY);
	this->cboDateFormat->SetSelectedIndex(0);
	this->chkInvert = ui->NewCheckBox(this->pnlMain, CSTR("Invert"), invert);
	this->chkInvert->SetRect(104, 76, 100, 23, false);
	this->btnCancel = ui->NewButton(this->pnlMain, CSTR("Cancel"));
	this->btnCancel->SetRect(104, 100, 75, 23, false);
	this->btnCancel->HandleButtonClick(OnCancelClicked, this);
	this->btnOK = ui->NewButton(this->pnlMain, CSTR("OK"));
	this->btnOK->SetRect(184, 100, 75, 23, false);
	this->btnOK->HandleButtonClick(OnOKClicked, this);
	this->SetDefaultButton(this->btnOK);
	this->SetDefaultButton(this->btnCancel);

	this->HandleDropFiles(OnFile, this);
}

SSWR::AVIRead::AVIRInvestmentImportForm::~AVIRInvestmentImportForm()
{
	this->db.Delete();
}

void SSWR::AVIRead::AVIRInvestmentImportForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
