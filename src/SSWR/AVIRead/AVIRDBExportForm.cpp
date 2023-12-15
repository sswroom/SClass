#include "Stdafx.h"
#include "DB/SQLGenerator.h"
#include "IO/FileStream.h"
#include "SSWR/AVIRead/AVIRDBExportForm.h"
#include "UI/FileDialog.h"
#include "UI/MessageDialog.h"

void __stdcall SSWR::AVIRead::AVIRDBExportForm::OnTablesDblClk(void *userObj, UOSInt itemIndex)
{
	SSWR::AVIRead::AVIRDBExportForm *me = (SSWR::AVIRead::AVIRDBExportForm*)userObj;
	UOSInt currVal = (UOSInt)me->lvTables->GetItem(itemIndex);
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

void __stdcall SSWR::AVIRead::AVIRDBExportForm::OnExportClicked(void *userObj)
{
	SSWR::AVIRead::AVIRDBExportForm *me = (SSWR::AVIRead::AVIRDBExportForm*)userObj;
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
	UI::FileDialog dlg(L"SSWR", L"AVIRead", L"DBExport", true);
	dlg.AddFilter(CSTR("*.sql"), CSTR("SQL File"));
	dlg.SetFileName(CSTRP(sbuff, sptr));
	if (dlg.ShowDialog(me->GetHandle()))
	{
		DB::SQLBuilder sql((DB::SQLType)(OSInt)me->cboDBType->GetSelectedItem(), me->chkAxisAware->IsChecked(), 0);
		Data::ArrayListNN<Text::String> cols;
		UOSInt i = 0;
		UOSInt j = me->lvTables->GetCount();
		while (i < j)
		{
			if (me->lvTables->GetItem(i) != 0)
			{
				cols.Add(Text::String::OrEmpty(me->lvTables->GetItemTextNew(i)));
			}
			i++;
		}
		NotNullPtr<DB::DBReader> r;
		if (!r.Set(me->db->QueryTableData(me->schema, me->table, &cols, 0, 0, CSTR_NULL, 0)))
		{
			UI::MessageDialog::ShowDialog(CSTR("Error in reading table data"), CSTR("Export Table Data"), me);
			return;
		}
		IO::FileStream fs(dlg.GetFileName(), IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
		while (r->ReadNext())
		{
			sql.Clear();			
			DB::SQLGenerator::GenInsertCmd(sql, sbSchema.ToCString(), sbTable.ToCString(), r);
			sql.AppendCmdC(CSTR(";\r\n"));
			fs.Write(sql.ToString(), sql.GetLength());
		}
		me->db->CloseReader(r);
		LISTNN_FREE_STRING(&cols);
		me->SetDialogResult(UI::GUIForm::DR_OK);
	}
}

SSWR::AVIRead::AVIRDBExportForm::AVIRDBExportForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core, DB::ReadingDB *db, Text::CString schema, Text::CString table) : UI::GUIForm(parent, 1024, 768, ui)
{
	this->SetFont(0, 0, 8.25, false);
	this->SetText(CSTR("Export Table Data as SQL"));
	this->core = core;
	this->db = db;
	this->schema = schema;
	this->table = table;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	NEW_CLASSNN(this->pnlMain, UI::GUIPanel(ui, *this));
	this->pnlMain->SetRect(0, 0, 100, 96, false);
	this->pnlMain->SetDockType(UI::GUIControl::DOCK_BOTTOM);
	NEW_CLASS(this->lvTables, UI::GUIListView(ui, *this, UI::GUIListView::LVSTYLE_TABLE, 3));
	this->lvTables->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lvTables->SetFullRowSelect(true);
	this->lvTables->SetShowGrid(true);
	this->lvTables->AddColumn(CSTR("Name"), 150);
	this->lvTables->AddColumn(CSTR("Export?"), 60);
	this->lvTables->AddColumn(CSTR("Column Type"), 100);
	this->lvTables->HandleDblClk(OnTablesDblClk, this);
	NEW_CLASS(this->lblDBType, UI::GUILabel(ui, this->pnlMain, CSTR("DB Type")));
	this->lblDBType->SetRect(0, 0, 100, 23, false);
	NEW_CLASS(this->cboDBType, UI::GUIComboBox(ui, this->pnlMain, false));
	this->cboDBType->SetRect(100, 0, 100, 23, false);
	this->cboDBType->AddItem(CSTR("MySQL"), (void*)DB::SQLType::MySQL);
	this->cboDBType->AddItem(CSTR("SQL Server"), (void*)DB::SQLType::MSSQL);
	this->cboDBType->AddItem(CSTR("PostgreSQL"), (void*)DB::SQLType::PostgreSQL);
	this->cboDBType->SetSelectedIndex(0);
	NEW_CLASS(this->chkAxisAware, UI::GUICheckBox(ui, this->pnlMain, CSTR("Axis-Aware (MySQL >=8)"), false));
	this->chkAxisAware->SetRect(200, 0, 150, 23, false);
	NEW_CLASS(this->lblSchema, UI::GUILabel(ui, this->pnlMain, CSTR("Schema")));
	this->lblSchema->SetRect(0, 24, 100, 23, false);
	if (schema.v == 0)
	{
		schema = CSTR("");
	}
	NEW_CLASS(this->txtSchema, UI::GUITextBox(ui, this->pnlMain, schema.OrEmpty()));
	this->txtSchema->SetRect(100, 24, 200, 23, false);
	NEW_CLASS(this->lblTable, UI::GUILabel(ui, this->pnlMain, CSTR("Table")));
	this->lblTable->SetRect(0, 48, 100, 23, false);
	NEW_CLASS(this->txtTable, UI::GUITextBox(ui, this->pnlMain, table.OrEmpty()));
	this->txtTable->SetRect(100, 48, 200, 23, false);
	NEW_CLASS(this->btnExport, UI::GUIButton(ui, this->pnlMain, CSTR("Export")));
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
