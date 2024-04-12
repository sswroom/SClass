#include "Stdafx.h"
#include "DB/SQLGenerator.h"
#include "IO/FileStream.h"
#include "SSWR/AVIRead/AVIRDBExportForm.h"
#include "UI/GUIFileDialog.h"

void __stdcall SSWR::AVIRead::AVIRDBExportForm::OnTablesDblClk(AnyType userObj, UOSInt itemIndex)
{
	NotNullPtr<SSWR::AVIRead::AVIRDBExportForm> me = userObj.GetNN<SSWR::AVIRead::AVIRDBExportForm>();
	UOSInt currVal = me->lvTables->GetItem(itemIndex).GetUOSInt();
	if (currVal == 0)
	{
		me->lvTables->SetSubItem(itemIndex, 1, CSTR("yes"));
		me->lvTables->SetItem(itemIndex, (void*)1);
	}
	else
	{
		me->lvTables->SetSubItem(itemIndex, 1, CSTR("no"));
		me->lvTables->SetItem(itemIndex, (void*)0);
	}
}

void __stdcall SSWR::AVIRead::AVIRDBExportForm::OnExportClicked(AnyType userObj)
{
	NotNullPtr<SSWR::AVIRead::AVIRDBExportForm> me = userObj.GetNN<SSWR::AVIRead::AVIRDBExportForm>();
	UTF8Char sbuff[512];
	UTF8Char *sptr;
	Text::StringBuilderUTF8 sbSchema;
	Text::StringBuilderUTF8 sbTable;
	me->txtSchema->GetText(sbSchema);
	me->txtTable->GetText(sbTable);
	sptr = sbuff;
	if (sbSchema.GetLength() > 0)
	{
		sptr = sbSchema.ConcatTo(sptr);
		*sptr++ = '_';
	}
	sptr = sbTable.ConcatTo(sptr);
	*sptr++ = '_';
	sptr = Data::Timestamp::Now().ToString(sptr, "yyyyMMdd_HHmmss");
	sptr = Text::StrConcatC(sptr, UTF8STRC(".sql"));
	NotNullPtr<UI::GUIFileDialog> dlg = me->ui->NewFileDialog(L"SSWR", L"AVIRead", L"DBExport", true);
	dlg->AddFilter(CSTR("*.sql"), CSTR("SQL File"));
	dlg->SetFileName(CSTRP(sbuff, sptr));
	if (dlg->ShowDialog(me->GetHandle()))
	{
		DB::SQLBuilder sql((DB::SQLType)me->cboDBType->GetSelectedItem().GetOSInt(), me->chkAxisAware->IsChecked(), 0);
		Data::ArrayListStringNN cols;
		UOSInt i = 0;
		UOSInt j = me->lvTables->GetCount();
		while (i < j)
		{
			if (me->lvTables->GetItem(i).IsNull())
			{
				cols.Add(Text::String::OrEmpty(me->lvTables->GetItemTextNew(i)));
			}
			i++;
		}
		NotNullPtr<DB::DBReader> r;
		if (!me->db->QueryTableData(me->schema, me->table, &cols, 0, 0, CSTR_NULL, 0).SetTo(r))
		{
			me->ui->ShowMsgOK(CSTR("Error in reading table data"), CSTR("Export Table Data"), me);
			dlg.Delete();
			return;
		}
		IO::FileStream fs(dlg->GetFileName(), IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
		while (r->ReadNext())
		{
			sql.Clear();			
			DB::SQLGenerator::GenInsertCmd(sql, sbSchema.ToCString(), sbTable.ToCString(), r);
			sql.AppendCmdC(CSTR(";\r\n"));
			fs.Write(sql.ToString(), sql.GetLength());
		}
		me->db->CloseReader(r);
		cols.FreeAll();
		me->SetDialogResult(UI::GUIForm::DR_OK);
	}
	dlg.Delete();
}

SSWR::AVIRead::AVIRDBExportForm::AVIRDBExportForm(Optional<UI::GUIClientControl> parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core, NotNullPtr<DB::ReadingDB> db, Text::CString schema, Text::CStringNN table) : UI::GUIForm(parent, 1024, 768, ui)
{
	this->SetFont(0, 0, 8.25, false);
	this->SetText(CSTR("Export Table Data as SQL"));
	this->core = core;
	this->db = db;
	this->schema = schema;
	this->table = table;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	this->pnlMain = ui->NewPanel(*this);
	this->pnlMain->SetRect(0, 0, 100, 96, false);
	this->pnlMain->SetDockType(UI::GUIControl::DOCK_BOTTOM);
	this->lvTables = ui->NewListView(*this, UI::ListViewStyle::Table, 3);
	this->lvTables->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lvTables->SetFullRowSelect(true);
	this->lvTables->SetShowGrid(true);
	this->lvTables->AddColumn(CSTR("Name"), 150);
	this->lvTables->AddColumn(CSTR("Export?"), 60);
	this->lvTables->AddColumn(CSTR("Column Type"), 100);
	this->lvTables->HandleDblClk(OnTablesDblClk, this);
	this->lblDBType = ui->NewLabel(this->pnlMain, CSTR("DB Type"));
	this->lblDBType->SetRect(0, 0, 100, 23, false);
	this->cboDBType = ui->NewComboBox(this->pnlMain, false);
	this->cboDBType->SetRect(100, 0, 100, 23, false);
	this->cboDBType->AddItem(CSTR("MySQL"), (void*)DB::SQLType::MySQL);
	this->cboDBType->AddItem(CSTR("SQL Server"), (void*)DB::SQLType::MSSQL);
	this->cboDBType->AddItem(CSTR("PostgreSQL"), (void*)DB::SQLType::PostgreSQL);
	this->cboDBType->SetSelectedIndex(0);
	this->chkAxisAware = ui->NewCheckBox(this->pnlMain, CSTR("Axis-Aware (MySQL >=8)"), false);
	this->chkAxisAware->SetRect(200, 0, 150, 23, false);
	this->lblSchema = ui->NewLabel(this->pnlMain, CSTR("Schema"));
	this->lblSchema->SetRect(0, 24, 100, 23, false);
	if (schema.v == 0)
	{
		schema = CSTR("");
	}
	this->txtSchema = ui->NewTextBox(this->pnlMain, schema.OrEmpty());
	this->txtSchema->SetRect(100, 24, 200, 23, false);
	this->lblTable = ui->NewLabel(this->pnlMain, CSTR("Table"));
	this->lblTable->SetRect(0, 48, 100, 23, false);
	this->txtTable = ui->NewTextBox(this->pnlMain, table.OrEmpty());
	this->txtTable->SetRect(100, 48, 200, 23, false);
	this->btnExport = ui->NewButton(this->pnlMain, CSTR("Export"));
	this->btnExport->SetRect(100, 72, 75, 23, false);
	this->btnExport->HandleButtonClick(OnExportClicked, this);

	DB::TableDef *tab = this->db->GetTableDef(schema, table);
	if (tab)
	{
		UTF8Char sbuff[128];
		UTF8Char *sptr;
		NotNullPtr<DB::ColDef> col;
		Data::ArrayIterator<NotNullPtr<DB::ColDef>> it = tab->ColIterator();
		UOSInt i;
		while (it.HasNext())
		{
			col = it.Next();
			i = this->lvTables->AddItem(col->GetColName(), (void*)1);
			this->lvTables->SetSubItem(i, 1, CSTR("yes"));
			sptr = DB::DBUtil::ColTypeGetString(sbuff, col->GetColType(), col->GetColSize(), col->GetColDP());
			this->lvTables->SetSubItem(i, 2, CSTRP(sbuff, sptr));
		}
		DEL_CLASS(tab);
	}
}

SSWR::AVIRead::AVIRDBExportForm::~AVIRDBExportForm()
{
}

void SSWR::AVIRead::AVIRDBExportForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
