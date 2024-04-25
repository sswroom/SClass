#include "Stdafx.h"
#include "SSWR/AVIRead/AVIRDBAssignColumnForm.h"

void __stdcall SSWR::AVIRead::AVIRDBAssignColumnForm::OnOKClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRDBAssignColumnForm> me = userObj.GetNN<SSWR::AVIRead::AVIRDBAssignColumnForm>();
	if (me->colsCbo == 0 || me->dbTable->GetColCnt() != me->colInd->GetCount() || me->colInd->GetCount() == 0)
	{
		me->ui->ShowMsgOK(CSTR("Database is not valid"), CSTR("DB Assign Column"), me);
		return;
	}
	UOSInt i = 0;
	UOSInt j = me->dbTable->GetColCnt();
	while (i < j)
	{
		if (me->colsCbo[i]->GetSelectedItem() == (void*)-2)
		{
			NN<DB::ColDef> col;
			Text::StringBuilderUTF8 sb;
			sb.Append(CSTR("Column "));
			if (me->dbTable->GetCol(i).SetTo(col))
			{
				sb.Append(col->GetColName());
			}
			else
			{
				sb.AppendUOSInt(i);
			}
			sb.Append(CSTR(" is unknown"));
			me->ui->ShowMsgOK(sb.ToCString(), CSTR("DB Assign Column"), me);
			return;
		}
		i++;
	}
	i = 0;
	while (i < j)
	{
		me->colInd->SetItem(i, me->colsCbo[i]->GetSelectedItem().GetUOSInt());
		i++;
	}
	me->SetDialogResult(UI::GUIForm::DR_OK);
}

void __stdcall SSWR::AVIRead::AVIRDBAssignColumnForm::OnCancelClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRDBAssignColumnForm> me = userObj.GetNN<SSWR::AVIRead::AVIRDBAssignColumnForm>();
	me->SetDialogResult(UI::GUIForm::DR_CANCEL);
}

SSWR::AVIRead::AVIRDBAssignColumnForm::AVIRDBAssignColumnForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core, NN<DB::TableDef> dbTable, NN<DB::ReadingDB> dataFile, Text::CString schema, Text::CStringNN table, Bool noHeader, Int8 dataFileTz, NN<Data::ArrayList<UOSInt>> colInd) : UI::GUIForm(parent, 1024, 768, ui)
{
	this->SetFont(0, 0, 8.25, false);
	this->SetText(CSTR("DB Assign Column"));
	this->core = core;
	this->dbTable = dbTable;
	this->dataFile = dataFile;
	this->schema = schema;
	this->table = table;
	this->dataFileNoHeader = noHeader;
	this->dataFileTz = dataFileTz;
	this->colInd = colInd;
	this->colsCbo = 0;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	this->pnlButtons = ui->NewPanel(*this);
	this->pnlButtons->SetRect(0, 0, 100, 31, false);
	this->pnlButtons->SetDockType(UI::GUIControl::DOCK_BOTTOM);
	this->pnlColumns = ui->NewPanel(*this);
	this->pnlColumns->SetDockType(UI::GUIControl::DOCK_FILL);
	this->btnCancel = ui->NewButton(this->pnlButtons, CSTR("Cancel"));
	this->btnCancel->SetRect(100, 4, 75, 23, false);
	this->btnCancel->HandleButtonClick(OnCancelClicked, this);
	this->btnOK = ui->NewButton(this->pnlButtons, CSTR("OK"));
	this->btnOK->SetRect(180, 4, 75, 23, false);
	this->btnOK->HandleButtonClick(OnOKClicked, this);
	this->SetCancelButton(this->btnCancel);
	this->SetDefaultButton(this->btnOK);

	if (dbTable->GetColCnt() != colInd->GetCount() || colInd->GetCount() == 0)
	{
		return;
	}
	UOSInt i = 0;
	UOSInt j = dbTable->GetColCnt();
	UOSInt k;
	UOSInt l;
	NN<DB::ColDef> col;
	NN<DB::DBReader> r;
	NN<UI::GUILabel> lbl;
	UTF8Char sbuff[512];
	UTF8Char *sptr;
	this->colsCbo = MemAlloc(NN<UI::GUIComboBox>, j);
	while (i < j)
	{
		if (dbTable->GetCol(i).SetTo(col))
		{
			lbl = ui->NewLabel(this->pnlColumns, col->GetColName()->ToCString());
			lbl->SetRect(0, UOSInt2Double(i * 24), 100, 23, false);
		}
		this->colsCbo[i] = ui->NewComboBox(this->pnlColumns, false);
		this->colsCbo[i]->SetRect(100, UOSInt2Double(i * 24), 100, 23, false);
		this->colsCbo[i]->AddItem(CSTR("Unknown"), (void*)-2);
		this->colsCbo[i]->AddItem(CSTR("Ignore"), (void*)-1);

		i++;
	}

	if (this->dataFile->QueryTableData(this->schema, this->table, 0, 0, 1, CSTR_NULL, 0).SetTo(r))
	{
		k = 0;
		l = r->ColCount();
		while (k < l)
		{
			sptr = r->GetName(k, sbuff);
			i = 0;
			while (i < j)
			{
				this->colsCbo[i]->AddItem(CSTRP(sbuff, sptr), (void*)k);
				i++;
			}
			k++;
		}
		this->dataFile->CloseReader(r);

		i = 0;
		while (i < j)
		{
			this->colsCbo[i]->SetSelectedIndex(this->colInd->GetItem(i) + 2);
			i++;
		}
	}
}

SSWR::AVIRead::AVIRDBAssignColumnForm::~AVIRDBAssignColumnForm()
{
	if (this->colsCbo)
	{
		MemFree(this->colsCbo);
		this->colsCbo = 0;
	}
}

void SSWR::AVIRead::AVIRDBAssignColumnForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
