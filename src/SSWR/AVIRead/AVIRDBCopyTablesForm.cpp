#include "Stdafx.h"
#include "DB/SQLGenerator.h"
#include "SSWR/AVIRead/AVIRDBCopyTablesForm.h"
#include "UI/MessageDialog.h"

#define VERBOSE
#if defined(VERBOSE)
#include <stdio.h>
#endif

void __stdcall SSWR::AVIRead::AVIRDBCopyTablesForm::OnSourceDBChg(void *userObj)
{
	SSWR::AVIRead::AVIRDBCopyTablesForm *me = (SSWR::AVIRead::AVIRDBCopyTablesForm*)userObj;
	DB::DBTool *db = (DB::DBTool*)me->cboSourceDB->GetSelectedItem();
	if (db)
	{
		Data::ArrayList<Text::String*> schemaNames;
		Text::String *s;
		db->QuerySchemaNames(&schemaNames);
		me->cboSourceSchema->ClearItems();
		UOSInt i = 0;
		UOSInt j = schemaNames.GetCount();
		if (j > 0)
		{
			while (i < j)
			{
				s = schemaNames.GetItem(i);
				me->cboSourceSchema->AddItem(s, 0);
				s->Release();
				i++;
			}
		}
		else
		{
			me->cboSourceSchema->AddItem(CSTR(""), 0);
		}
		me->cboSourceSchema->SetSelectedIndex(0);
	}
}

void __stdcall SSWR::AVIRead::AVIRDBCopyTablesForm::OnSourceSelectClicked(void *userObj)
{
	SSWR::AVIRead::AVIRDBCopyTablesForm *me = (SSWR::AVIRead::AVIRDBCopyTablesForm*)userObj;
	DB::DBTool *db = (DB::DBTool*)me->cboSourceDB->GetSelectedItem();
	if (db == 0)
		return;
	UTF8Char sbuff[512];
	UTF8Char *sptr;
	sptr = me->cboSourceSchema->GetSelectedItemText(sbuff);
	if (sptr == 0)
		return;
	Data::ArrayList<Text::String*> tableNames;
	db->QueryTableNames(CSTRP(sbuff, sptr), &tableNames);
	if (tableNames.GetCount() == 0)
	{
		UI::MessageDialog::ShowDialog(CSTR("Tables not found"), CSTR("Copy Tables"), me);
	}
	else
	{
		me->dataConn = db;
		SDEL_STRING(me->dataSchema);
		LIST_FREE_STRING(&me->dataTables);
		me->dataSchema = Text::String::NewP(sbuff, sptr);
		me->dataTables.AddAll(&tableNames);
		me->lvData->ClearItems();
		UOSInt i = 0;
		UOSInt j = tableNames.GetCount();
		while (i < j)
		{
			me->lvData->AddItem(tableNames.GetItem(i), 0);
			i++;
		}
		me->tcMain->SetSelectedIndex(1);
	}
}

void __stdcall SSWR::AVIRead::AVIRDBCopyTablesForm::OnDestDBChg(void *userObj)
{
	SSWR::AVIRead::AVIRDBCopyTablesForm *me = (SSWR::AVIRead::AVIRDBCopyTablesForm*)userObj;
	DB::DBTool *db = (DB::DBTool*)me->cboDestDB->GetSelectedItem();
	if (db)
	{
		Data::ArrayList<Text::String*> schemaNames;
		Text::String *s;
		db->QuerySchemaNames(&schemaNames);
		me->cboDestSchema->ClearItems();
		UOSInt i = 0;
		UOSInt j = schemaNames.GetCount();
		if (j > 0)
		{
			while (i < j)
			{
				s = schemaNames.GetItem(i);
				me->cboDestSchema->AddItem(s, 0);
				s->Release();
				i++;
			}
		}
		else
		{
			me->cboDestSchema->AddItem(CSTR(""), 0);
		}
		me->cboDestSchema->SetSelectedIndex(0);
	}
}

void __stdcall SSWR::AVIRead::AVIRDBCopyTablesForm::OnCopyClicked(void *userObj)
{
	SSWR::AVIRead::AVIRDBCopyTablesForm *me = (SSWR::AVIRead::AVIRDBCopyTablesForm*)userObj;
	if (me->dataConn == 0)
	{
		return;
	}
	DB::DBTool *destDB = (DB::DBTool *)me->cboDestDB->GetSelectedItem();
	if (destDB == 0)
	{
		UI::MessageDialog::ShowDialog(CSTR("Please select a destination DB first"), CSTR("Copy Tables"), me);
		return;
	}
	UTF8Char destSchema[512];
	UTF8Char *destSchemaEnd = me->cboDestSchema->GetSelectedItemText(destSchema);
	if (destSchemaEnd == 0)
	{
		UI::MessageDialog::ShowDialog(CSTR("Please select a destination Schema first"), CSTR("Copy Tables"), me);
		return;
	}
	Bool createTable = me->chkDestCreateTable->IsChecked();
	Bool copyData = me->chkDestCopyData->IsChecked();
	DB::SQLBuilder sql(destDB);
	Text::StringBuilderUTF8 sb;
	DB::TableDef *tabDef;
	Text::String *tableName;
	DB::DBReader *r;
	UOSInt i = 0;
	UOSInt j = me->dataTables.GetCount();
	while (i < j)
	{
		Bool succ = true;
		tableName = me->dataTables.GetItem(i);
		if (succ && createTable)
		{
			tabDef = me->dataConn->GetTableDef(STR_CSTR(me->dataSchema), tableName->ToCString());
			if (tabDef == 0)
			{
				me->lvData->SetSubItem(i, 1, CSTR("Error in getting table definition"));
				succ = false;
			}
			else
			{
				sql.Clear();
				if (!DB::SQLGenerator::GenCreateTableCmd(&sql, CSTRP(destSchema, destSchemaEnd), tableName->ToCString(), tabDef))
				{
					me->lvData->SetSubItem(i, 1, CSTR("Error in generating create command"));
					succ = false;
				}
				else if (destDB->ExecuteNonQuery(sql.ToCString()) <= -2)
				{
					sb.ClearStr();
					destDB->GetLastErrorMsg(&sb);
					me->lvData->SetSubItem(i, 1, sb.ToCString());
					succ = false;
				}
				else
				{
					me->lvData->SetSubItem(i, 1, CSTR("Table created"));
				}
				DEL_CLASS(tabDef);
			}
		}
		if (succ && copyData)
		{
			UOSInt rowCopied = 0;
			r = me->dataConn->QueryTableData(STR_CSTR(me->dataSchema), tableName->ToCString(), 0, 0, 0, 0, 0);
			if (r)
			{
				Text::StringBuilderUTF8 sbInsert;
				UOSInt nInsert = 0;

				while (r->ReadNext())
				{
					sql.Clear();
					DB::SQLGenerator::GenInsertCmd(&sql, CSTRP(destSchema, destSchemaEnd), tableName->ToCString(), r);
					UOSInt k = sql.ToCString().IndexOf(UTF8STRC(") values ("));
					if (k == INVALID_INDEX)
					{
						if (destDB->ExecuteNonQuery(sql.ToCString()) != 1)
						{
#if defined(VERBOSE)
							printf("DBCopyTables: Error SQL: %s\r\n", sql.ToString());
#endif
							sb.ClearStr();
							destDB->GetLastErrorMsg(&sb);
							me->lvData->SetSubItem(i, 1, sb.ToCString());
							succ = false;
							break;
						}
						else
						{
							rowCopied++;
						}
					}
					else if (nInsert == 0)
					{
						sbInsert.Append(sql.ToCString());
						nInsert = 1;
					}
					else
					{
						sbInsert.AppendUTF8Char(',');
						sbInsert.Append(sql.ToCString().Substring(k + 9));
						nInsert++;

						if (nInsert >= 250)
						{
							if (destDB->ExecuteNonQuery(sbInsert.ToCString()) >= 0)
							{
								rowCopied += nInsert;
								nInsert = 0;
								sbInsert.ClearStr();
							}
							else
							{
								sb.ClearStr();
								destDB->GetLastErrorMsg(&sb);
								me->lvData->SetSubItem(i, 1, sb.ToCString());
								succ = false;
								break;
							}
						}
					}
				}
				me->dataConn->CloseReader(r);
				if (succ && nInsert > 0)
				{
					if (destDB->ExecuteNonQuery(sbInsert.ToCString()) >= 0)
					{
						rowCopied += nInsert;
						nInsert = 0;
						sbInsert.ClearStr();
					}
					else
					{
						sb.ClearStr();
						destDB->GetLastErrorMsg(&sb);
						me->lvData->SetSubItem(i, 1, sb.ToCString());
						succ = false;
					}
				}
				if (succ)
				{
					sb.ClearStr();
					sb.AppendUOSInt(rowCopied);
					sb.AppendC(UTF8STRC(" rows copied"));
					me->lvData->SetSubItem(i, 1, sb.ToCString());
				}
			}
			else
			{
				sb.ClearStr();
				me->dataConn->GetLastErrorMsg(&sb);
				me->lvData->SetSubItem(i, 1, sb.ToCString());
			}
		}
		i++;
	}
}

SSWR::AVIRead::AVIRDBCopyTablesForm::AVIRDBCopyTablesForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core, Data::ArrayList<DB::DBTool*> *dbList) : UI::GUIForm(parent, 1024, 768, ui)
{
	this->SetFont(0, 0, 8.25, false);
	this->SetText(CSTR("Copy Tables"));
	this->core = core;
	this->dbList = dbList;
	this->dataSchema = 0;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));


	NEW_CLASS(this->tcMain, UI::GUITabControl(ui, this));
	this->tcMain->SetDockType(UI::GUIControl::DOCK_FILL);

	this->tpSource = this->tcMain->AddTabPage(CSTR("Source"));
	NEW_CLASS(this->lblSourceDB, UI::GUILabel(ui, this->tpSource, CSTR("Connection")));
	this->lblSourceDB->SetRect(4, 4, 100, 23, false);
	NEW_CLASS(this->cboSourceDB, UI::GUIComboBox(ui, this->tpSource, false));
	this->cboSourceDB->SetRect(104, 4, 400, 23, false);
	this->cboSourceDB->HandleSelectionChange(OnSourceDBChg, this);
	NEW_CLASS(this->lblSourceSchema, UI::GUILabel(ui, this->tpSource, CSTR("Schema")));
	this->lblSourceSchema->SetRect(4, 28, 100, 23, false);
	NEW_CLASS(this->cboSourceSchema, UI::GUIComboBox(ui, this->tpSource, false));
	this->cboSourceSchema->SetRect(104, 28, 200, 23, false);
	NEW_CLASS(this->btnSourceSelect, UI::GUIButton(ui, this->tpSource, CSTR("Select")));
	this->btnSourceSelect->SetRect(104, 52, 75, 23, false);
	this->btnSourceSelect->HandleButtonClick(OnSourceSelectClicked, this);	

	this->tpData = this->tcMain->AddTabPage(CSTR("Data"));
	NEW_CLASS(this->grpDest, UI::GUIGroupBox(ui, this->tpData, CSTR("Destination")));
	this->grpDest->SetRect(0, 0, 100, 130, false);
	this->grpDest->SetDockType(UI::GUIControl::DOCK_BOTTOM);
	NEW_CLASS(this->lvData, UI::GUIListView(ui, this->tpData, UI::GUIListView::LVSTYLE_TABLE, 2));
	this->lvData->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lvData->SetFullRowSelect(true);
	this->lvData->SetShowGrid(true);
	this->lvData->AddColumn(CSTR("Table"), 100);
	this->lvData->AddColumn(CSTR("Status"), 400);
	NEW_CLASS(this->lblDestDB, UI::GUILabel(ui, this->grpDest, CSTR("Connection")));
	this->lblDestDB->SetRect(4, 4, 100, 23, false);
	NEW_CLASS(this->cboDestDB, UI::GUIComboBox(ui, this->grpDest, false));
	this->cboDestDB->SetRect(104, 4, 400, 23, false);
	this->cboDestDB->HandleSelectionChange(OnDestDBChg, this);
	NEW_CLASS(this->lblDestSchema, UI::GUILabel(ui, this->grpDest, CSTR("Schema")));
	this->lblDestSchema->SetRect(4, 28, 100, 23, false);
	NEW_CLASS(this->cboDestSchema, UI::GUIComboBox(ui, this->grpDest, false));
	this->cboDestSchema->SetRect(104, 28, 200, 23, false);
	NEW_CLASS(this->lblDestOptions, UI::GUILabel(ui, this->grpDest, CSTR("Options")));
	this->lblDestOptions->SetRect(4, 52, 100, 23, false);
	NEW_CLASS(this->chkDestCreateTable, UI::GUICheckBox(ui, this->grpDest, CSTR("Create Table"), true));
	this->chkDestCreateTable->SetRect(104, 52, 100, 23, false);
	NEW_CLASS(this->chkDestCopyData, UI::GUICheckBox(ui, this->grpDest, CSTR("Copy Data"), true));
	this->chkDestCopyData->SetRect(204, 52, 100, 23, false);
	NEW_CLASS(this->btnCopy, UI::GUIButton(ui, this->grpDest, CSTR("Copy")));
	this->btnCopy->SetRect(104, 76, 75, 23, false);
	this->btnCopy->HandleButtonClick(OnCopyClicked, this);

	Text::StringBuilderUTF8 sb;
	DB::DBTool *db;
	UOSInt i = 0;
	UOSInt j = this->dbList->GetCount();
	while (i < j)
	{
		db = this->dbList->GetItem(i);
		sb.ClearStr();
		db->GetConn()->GetConnName(&sb);
		this->cboSourceDB->AddItem(sb.ToCString(), db);
		this->cboDestDB->AddItem(sb.ToCString(), db);
		i++;
	}
	if (j > 0)
	{
		this->cboSourceDB->SetSelectedIndex(0);
		this->cboDestDB->SetSelectedIndex(0);
	}
}

SSWR::AVIRead::AVIRDBCopyTablesForm::~AVIRDBCopyTablesForm()
{
	SDEL_STRING(this->dataSchema);
	LIST_FREE_STRING(&this->dataTables);
}

void SSWR::AVIRead::AVIRDBCopyTablesForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
