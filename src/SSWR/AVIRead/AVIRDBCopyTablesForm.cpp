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
	DB::DBManagerCtrl *ctrl = (DB::DBManagerCtrl*)me->cboSourceConn->GetSelectedItem();
	if (ctrl)
	{
		me->cboSourceSchema->ClearItems();
		if (!ctrl->Connect())
		{
			UI::MessageDialog::ShowDialog(CSTR("Error in connecting to database"), CSTR("Copy Tables"), me);
			return;
		}
		DB::ReadingDB *db = ctrl->GetDB();
		if (db->IsDBTool())
		{
			UTF8Char sbuff[128];
			UTF8Char *sptr;
			DB::ReadingDBTool *dbt = (DB::ReadingDBTool*)db;
			DB::Collation collation;
			NotNullPtr<Text::String> dbName = Text::String::OrEmpty(dbt->GetCurrDBName());
			me->txtSourceDB->SetText(dbName->ToCString());
			if (dbt->GetDBCollation(dbName->ToCString(), &collation))
			{
				sptr = DB::DBUtil::SDBCollation(sbuff, &collation, dbt->GetSQLType());
				me->txtSourceCollation->SetText(CSTRP(sbuff, sptr));
			}
			else
			{
				me->txtSourceCollation->SetText(CSTR(""));
			}
		}
		else
		{
			me->txtSourceDB->SetText(CSTR(""));
			me->txtSourceCollation->SetText(CSTR(""));
		}
		Data::ArrayListNN<Text::String> schemaNames;
		NotNullPtr<Text::String> s;
		db->QuerySchemaNames(schemaNames);
		UOSInt i = 0;
		UOSInt j = schemaNames.GetCount();
		if (j > 0)
		{
			while (i < j)
			{
				s = Text::String::OrEmpty(schemaNames.GetItem(i));
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
	DB::DBManagerCtrl *db = (DB::DBManagerCtrl*)me->cboSourceConn->GetSelectedItem();
	if (db == 0)
		return;
	UTF8Char sbuff[512];
	UTF8Char *sptr;
	sptr = me->cboSourceSchema->GetSelectedItemText(sbuff);
	if (sptr == 0)
		return;
	Data::ArrayListNN<Text::String> tableNames;
	db->GetDB()->QueryTableNames(CSTRP(sbuff, sptr), tableNames);
	if (tableNames.GetCount() == 0)
	{
		UI::MessageDialog::ShowDialog(CSTR("Tables not found"), CSTR("Copy Tables"), me);
	}
	else
	{
		me->dataConn = db->GetDB();
		SDEL_STRING(me->dataSchema);
		LISTNN_FREE_STRING(&me->dataTables);
		me->dataSchema = Text::String::NewP(sbuff, sptr).Ptr();
		me->dataTables.AddAll(tableNames);
		me->lvData->ClearItems();
		UOSInt i = 0;
		UOSInt j = tableNames.GetCount();
		while (i < j)
		{
			me->lvData->AddItem(Text::String::OrEmpty(tableNames.GetItem(i)), 0);
			i++;
		}
		me->tcMain->SetSelectedIndex(1);
	}
}

void __stdcall SSWR::AVIRead::AVIRDBCopyTablesForm::OnDestDBChg(void *userObj)
{
	SSWR::AVIRead::AVIRDBCopyTablesForm *me = (SSWR::AVIRead::AVIRDBCopyTablesForm*)userObj;
	DB::DBManagerCtrl *ctrl = (DB::DBManagerCtrl*)me->cboDestDB->GetSelectedItem();
	if (ctrl)
	{
		me->cboDestSchema->ClearItems();
		if (!ctrl->Connect())
		{
			UI::MessageDialog::ShowDialog(CSTR("Error in connecting to database"), CSTR("Copy Tables"), me);
			return;
		}
		Data::ArrayListNN<Text::String> schemaNames;
		NotNullPtr<Text::String> s;
		ctrl->GetDB()->QuerySchemaNames(schemaNames);
		UOSInt i = 0;
		UOSInt j = schemaNames.GetCount();
		if (j > 0)
		{
			while (i < j)
			{
				s = Text::String::OrEmpty(schemaNames.GetItem(i));
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
	DB::DBManagerCtrl *destDB = (DB::DBManagerCtrl *)me->cboDestDB->GetSelectedItem();
	if (destDB == 0)
	{
		UI::MessageDialog::ShowDialog(CSTR("Please select a destination DB first"), CSTR("Copy Tables"), me);
		return;
	}
	if (!destDB->Connect())
	{
		UI::MessageDialog::ShowDialog(CSTR("Error in connecting destination DB"), CSTR("Copy Tables"), me);
		return;
	}
	NotNullPtr<DB::ReadingDB> destConn;
	if (!destConn.Set(destDB->GetDB()))
	{
		UI::MessageDialog::ShowDialog(CSTR("Error in getting destination DB"), CSTR("Copy Tables"), me);
		return;
	}
	NotNullPtr<DB::DBTool> destDBTool;
	if (!destConn->IsDBTool() || !((DB::ReadingDBTool*)destConn.Ptr())->CanModify())
	{
		UI::MessageDialog::ShowDialog(CSTR("Destination DB is read-onlyl"), CSTR("Copy Tables"), me);
		return;
	}
	destDBTool = NotNullPtr<DB::DBTool>::ConvertFrom(destConn);
	UTF8Char destSchema[512];
	UTF8Char *destSchemaEnd = me->cboDestSchema->GetSelectedItemText(destSchema);
	if (destSchemaEnd == 0)
	{
		UI::MessageDialog::ShowDialog(CSTR("Please select a destination Schema first"), CSTR("Copy Tables"), me);
		return;
	}
	UOSInt destTableType = me->cboDestTableType->GetSelectedIndex();
	Bool copyData = me->chkDestCopyData->IsChecked();
	Bool createDB = me->chkDestCreateDDB->IsChecked();
	DB::SQLBuilder sql(destDBTool);
	Text::StringBuilderUTF8 sb;
	DB::TableDef *tabDef;
	NotNullPtr<DB::TableDef> nntabDef;
	NotNullPtr<Text::String> tableName;
	NotNullPtr<DB::DBReader> r;
	if (destDBTool->GetSQLType() == DB::SQLType::MySQL)
	{
		destDBTool->ExecuteNonQuery(CSTR("set sql_mode=CONCAT(@@Session.sql_mode,',NO_AUTO_VALUE_ON_ZERO')"));
	}
	if (createDB)
	{
		if (me->dataConn->IsDBTool())
		{
			DB::ReadingDBTool *dbt = (DB::ReadingDBTool*)me->dataConn;
			DB::Collation collation;
			Text::String *dbName = dbt->GetCurrDBName();
			me->txtSourceDB->SetText(dbName->ToCString());
			if (dbt->GetDBCollation(dbName->ToCString(), &collation))
			{
				if (!destDBTool->CreateDatabase(dbName->ToCString(), &collation))
				{
					UI::MessageDialog::ShowDialog(CSTR("Error in creating database"), CSTR("Copy Tables"), me);
					return;
				}
				if (!destDBTool->ChangeDatabase(dbName->ToCString()))
				{
					UI::MessageDialog::ShowDialog(CSTR("Error in changing to new database"), CSTR("Copy Tables"), me);
					return;
				}
			}
			else
			{
				UI::MessageDialog::ShowDialog(CSTR("Error in parsing database collation"), CSTR("Copy Tables"), me);
				return;
			}
		}
		else
		{
			UI::MessageDialog::ShowDialog(CSTR("Source Database does not support getting database collation"), CSTR("Copy Tables"), me);
			return;
		}
	}
	Data::ArrayIterator<NotNullPtr<Text::String>> it = me->dataTables.Iterator();
	UOSInt i = 0;
	while (it.HasNext())
	{
		Bool succ = true;
		tableName = it.Next();
		tabDef = me->dataConn->GetTableDef(STR_CSTR(me->dataSchema), tableName->ToCString());
		if (succ && destTableType == 0)
		{
			if (!nntabDef.Set(tabDef))
			{
				me->lvData->SetSubItem(i, 1, CSTR("Error in getting table definition"));
				succ = false;
			}
			else
			{
				sql.Clear();
				if (!DB::SQLGenerator::GenCreateTableCmd(sql, CSTRP(destSchema, destSchemaEnd), tableName->ToCString(), nntabDef, false))
				{
					me->lvData->SetSubItem(i, 1, CSTR("Error in generating create command"));
					succ = false;
				}
				else if (destDBTool->ExecuteNonQuery(sql.ToCString()) <= -2)
				{
					sb.ClearStr();
					destDBTool->GetLastErrorMsg(sb);
					me->lvData->SetSubItem(i, 1, sb.ToCString());
					succ = false;
				}
				else
				{
					me->lvData->SetSubItem(i, 1, CSTR("Table created"));
				}
			}
		}
		else if (succ && destTableType == 1)
		{
			succ = destDBTool->DeleteTableData(CSTRP(destSchema, destSchemaEnd), tableName->ToCString());
			if (!succ)
			{
				me->lvData->SetSubItem(i, 1, CSTR("Error in deleting table data"));
			}
		}
		if (succ && copyData)
		{
			UOSInt rowCopied = 0;
			if (r.Set(me->dataConn->QueryTableData(STR_CSTR(me->dataSchema), tableName->ToCString(), 0, 0, 0, 0, 0)))
			{
				Text::StringBuilderUTF8 sbInsert;
				UOSInt nInsert = 0;

				while (r->ReadNext())
				{
					sql.Clear();
					if (tabDef)
						DB::SQLGenerator::GenInsertCmd(sql, CSTRP(destSchema, destSchemaEnd), tableName->ToCString(), tabDef, r);
					else
						DB::SQLGenerator::GenInsertCmd(sql, CSTRP(destSchema, destSchemaEnd), tableName->ToCString(), r);
					UOSInt k = sql.ToCString().IndexOf(UTF8STRC(" values ("));
					if (k == INVALID_INDEX)
					{
						if (destDBTool->ExecuteNonQuery(sql.ToCString()) != 1)
						{
#if defined(VERBOSE)
							printf("DBCopyTables: Error SQL: %s\r\n", sql.ToString());
#endif
							sb.ClearStr();
							destDBTool->GetLastErrorMsg(sb);
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
						sbInsert.Append(sql.ToCString().Substring(k + 8));
						nInsert++;

						if (nInsert >= 250)
						{
							if (destDBTool->ExecuteNonQuery(sbInsert.ToCString()) >= 0)
							{
								rowCopied += nInsert;
								nInsert = 0;
								sbInsert.ClearStr();
							}
							else
							{
								sb.ClearStr();
								destDBTool->GetLastErrorMsg(sb);
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
					if (destDBTool->ExecuteNonQuery(sbInsert.ToCString()) >= 0)
					{
						rowCopied += nInsert;
						nInsert = 0;
						sbInsert.ClearStr();
					}
					else
					{
						sb.ClearStr();
						destDBTool->GetLastErrorMsg(sb);
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
				me->dataConn->GetLastErrorMsg(sb);
				me->lvData->SetSubItem(i, 1, sb.ToCString());
			}
		}
		SDEL_CLASS(tabDef);
		i++;
	}
}

SSWR::AVIRead::AVIRDBCopyTablesForm::AVIRDBCopyTablesForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core, Data::ArrayList<DB::DBManagerCtrl*> *dbList) : UI::GUIForm(parent, 1024, 768, ui)
{
	this->SetFont(0, 0, 8.25, false);
	this->SetText(CSTR("Copy Tables"));
	this->core = core;
	this->dbList = dbList;
	this->dataSchema = 0;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));


	NEW_CLASS(this->tcMain, UI::GUITabControl(ui, *this));
	this->tcMain->SetDockType(UI::GUIControl::DOCK_FILL);

	this->tpSource = this->tcMain->AddTabPage(CSTR("Source"));
	NEW_CLASS(this->lblSourceConn, UI::GUILabel(ui, this->tpSource, CSTR("Connection")));
	this->lblSourceConn->SetRect(4, 4, 100, 23, false);
	NEW_CLASS(this->cboSourceConn, UI::GUIComboBox(ui, this->tpSource, false));
	this->cboSourceConn->SetRect(104, 4, 400, 23, false);
	NEW_CLASS(this->lblSourceDB, UI::GUILabel(ui, this->tpSource, CSTR("Database")));
	this->lblSourceDB->SetRect(4, 28, 100, 23, false);
	NEW_CLASS(this->txtSourceDB, UI::GUITextBox(ui, this->tpSource, CSTR("")));
	this->txtSourceDB->SetRect(104, 28, 200, 23, false);
	this->txtSourceDB->SetReadOnly(true);
	NEW_CLASS(this->lblSourceCollation, UI::GUILabel(ui, this->tpSource, CSTR("Collation")));
	this->lblSourceCollation->SetRect(4, 52, 100, 23, false);
	NEW_CLASS(this->txtSourceCollation, UI::GUITextBox(ui, this->tpSource, CSTR("")));
	this->txtSourceCollation->SetRect(104, 52, 200, 23, false);
	this->txtSourceCollation->SetReadOnly(true);
	NEW_CLASS(this->lblSourceSchema, UI::GUILabel(ui, this->tpSource, CSTR("Schema")));
	this->lblSourceSchema->SetRect(4, 76, 100, 23, false);
	NEW_CLASS(this->cboSourceSchema, UI::GUIComboBox(ui, this->tpSource, false));
	this->cboSourceSchema->SetRect(104, 76, 200, 23, false);
	NEW_CLASS(this->btnSourceSelect, UI::GUIButton(ui, this->tpSource, CSTR("Select")));
	this->btnSourceSelect->SetRect(104, 100, 75, 23, false);
	this->btnSourceSelect->HandleButtonClick(OnSourceSelectClicked, this);	
	this->cboSourceConn->HandleSelectionChange(OnSourceDBChg, this);

	this->tpData = this->tcMain->AddTabPage(CSTR("Data"));
	NEW_CLASSNN(this->grpDest, UI::GUIGroupBox(ui, this->tpData, CSTR("Destination")));
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
	NEW_CLASS(this->cboDestTableType, UI::GUIComboBox(ui, this->grpDest, false));
	this->cboDestTableType->SetRect(104, 52, 100, 23, false);
	this->cboDestTableType->AddItem(CSTR("Create Table"), 0);
	this->cboDestTableType->AddItem(CSTR("Remove existing"), 0);
	this->cboDestTableType->AddItem(CSTR("Append Data"), 0);
	this->cboDestTableType->SetSelectedIndex(0);
	NEW_CLASS(this->chkDestCopyData, UI::GUICheckBox(ui, this->grpDest, CSTR("Copy Data"), true));
	this->chkDestCopyData->SetRect(204, 52, 100, 23, false);
	NEW_CLASS(this->chkDestCreateDDB, UI::GUICheckBox(ui, this->grpDest, CSTR("Create DB"), false));
	this->chkDestCreateDDB->SetRect(304, 52, 100, 23, false);
	NEW_CLASS(this->btnCopy, UI::GUIButton(ui, this->grpDest, CSTR("Copy")));
	this->btnCopy->SetRect(104, 76, 75, 23, false);
	this->btnCopy->HandleButtonClick(OnCopyClicked, this);

	Text::StringBuilderUTF8 sb;
	DB::DBManagerCtrl *ctrl;
	UOSInt firstActive = INVALID_INDEX;
	UOSInt i = 0;
	UOSInt j = this->dbList->GetCount();
	while (i < j)
	{
		ctrl = this->dbList->GetItem(i);
		if (firstActive == INVALID_INDEX && ctrl->GetStatus() == DB::DBManagerCtrl::ConnStatus::Connected)
		{
			firstActive = i;
		}
		sb.ClearStr();
		ctrl->GetConnName(sb);
		this->cboSourceConn->AddItem(sb.ToCString(), ctrl);
		this->cboDestDB->AddItem(sb.ToCString(), ctrl);
		i++;
	}
	if (j > 0)
	{
		if (firstActive == INVALID_INDEX)
			firstActive = 0;
		this->cboSourceConn->SetSelectedIndex(firstActive);
		this->cboDestDB->SetSelectedIndex(firstActive);
	}
}

SSWR::AVIRead::AVIRDBCopyTablesForm::~AVIRDBCopyTablesForm()
{
	SDEL_STRING(this->dataSchema);
	LISTNN_FREE_STRING(&this->dataTables);
}

void SSWR::AVIRead::AVIRDBCopyTablesForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}

void SSWR::AVIRead::AVIRDBCopyTablesForm::SetSourceDB(UOSInt index)
{
	this->cboSourceConn->SetSelectedIndex(index);
}
