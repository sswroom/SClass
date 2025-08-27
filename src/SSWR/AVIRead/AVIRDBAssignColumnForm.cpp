#include "Stdafx.h"
#include "SSWR/AVIRead/AVIRDBAssignColumnForm.h"

void __stdcall SSWR::AVIRead::AVIRDBAssignColumnForm::OnCboSelChg(AnyType userObj)
{
	NN<ColumnItem> colItem = userObj.GetNN<ColumnItem>();
	colItem->txt->SetReadOnly(colItem->cbo->GetSelectedItem().GetOSInt() != -3);
}

void __stdcall SSWR::AVIRead::AVIRDBAssignColumnForm::OnOKClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRDBAssignColumnForm> me = userObj.GetNN<SSWR::AVIRead::AVIRDBAssignColumnForm>();
	UnsafeArray<ColumnItem> colsItem;
	NN<Text::String> s;
	if (!me->colsItem.SetTo(colsItem) || me->dbTable->GetColCnt() != me->colInd->GetCount() || me->colInd->GetCount() == 0)
	{
		me->ui->ShowMsgOK(CSTR("Database is not valid"), CSTR("DB Assign Column"), me);
		return;
	}
	UOSInt i = 0;
	UOSInt j = me->dbTable->GetColCnt();
	while (i < j)
	{
		if (colsItem[i].cbo->GetSelectedItem() == (void*)-2)
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
	i = me->colStr->GetCount();
	while (i-- > 0)
	{
		if (me->colStr->RemoveAt(i).SetTo(s))
			s->Release();
	}
	Text::StringBuilderUTF8 sb;
	i = 0;
	while (i < j)
	{
		me->colInd->SetItem(i, colsItem[i].cbo->GetSelectedItem().GetUOSInt());
		sb.ClearStr();
		colsItem[i].txt->GetText(sb);
		me->colStr->Add(Text::String::New(sb.ToCString()));
		i++;
	}
	me->SetDialogResult(UI::GUIForm::DR_OK);
}

void __stdcall SSWR::AVIRead::AVIRDBAssignColumnForm::OnCancelClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRDBAssignColumnForm> me = userObj.GetNN<SSWR::AVIRead::AVIRDBAssignColumnForm>();
	me->SetDialogResult(UI::GUIForm::DR_CANCEL);
}

SSWR::AVIRead::AVIRDBAssignColumnForm::AVIRDBAssignColumnForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core, NN<DB::TableDef> dbTable, NN<DB::ReadingDB> dataFile, Text::CString schema, Text::CStringNN table, Bool noHeader, Int8 dataFileTz, NN<Data::ArrayList<UOSInt>> colInd, NN<Data::ArrayListStringNN> colStr) : UI::GUIForm(parent, 1024, 768, ui)
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
	this->colStr = colStr;
	this->colsItem = 0;
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
	UnsafeArray<UTF8Char> sptr;
	UnsafeArray<ColumnItem> colsItem;
	this->colsItem = colsItem = MemAllocArr(ColumnItem, j);
	while (i < j)
	{
		if (dbTable->GetCol(i).SetTo(col))
		{
			lbl = ui->NewLabel(this->pnlColumns, col->GetColName()->ToCString());
			lbl->SetRect(0, UOSInt2Double(i * 24), 100, 23, false);
		}
		colsItem[i].cbo = ui->NewComboBox(this->pnlColumns, false);
		colsItem[i].cbo->SetRect(100, UOSInt2Double(i * 24), 100, 23, false);
		colsItem[i].cbo->AddItem(CSTR("Fix Text"), (void*)-3);
		colsItem[i].cbo->AddItem(CSTR("Unknown"), (void*)-2);
		colsItem[i].cbo->AddItem(CSTR("Ignore"), (void*)-1);
		colsItem[i].cbo->HandleSelectionChange(OnCboSelChg, &colsItem[i]);
		colsItem[i].txt = ui->NewTextBox(this->pnlColumns, Text::String::OrEmpty(this->colStr->GetItem(i))->ToCString());
		colsItem[i].txt->SetRect(200, UOSInt2Double(i * 24), 100, 23, false);
		colsItem[i].txt->SetReadOnly(true);
		colsItem[i].me = *this;

		i++;
	}

	if (this->dataFile->QueryTableData(this->schema, this->table, 0, 0, 1, nullptr, 0).SetTo(r))
	{
		k = 0;
		l = r->ColCount();
		while (k < l)
		{
			if (r->GetName(k, sbuff).SetTo(sptr))
			{
				i = 0;
				while (i < j)
				{
					colsItem[i].cbo->AddItem(CSTRP(sbuff, sptr), (void*)k);
					i++;
				}
			}
			k++;
		}
		this->dataFile->CloseReader(r);

		i = 0;
		while (i < j)
		{
			colsItem[i].cbo->SetSelectedIndex(k = this->colInd->GetItem(i) + 3);
			if (k == 0)
			{
				colsItem[i].txt->SetReadOnly(false);
			}
			i++;
		}
	}
}

SSWR::AVIRead::AVIRDBAssignColumnForm::~AVIRDBAssignColumnForm()
{
	UnsafeArray<ColumnItem> colsItem;
	if (this->colsItem.SetTo(colsItem))
	{
		MemFreeArr(colsItem);
		this->colsItem = 0;
	}
}

void SSWR::AVIRead::AVIRDBAssignColumnForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
