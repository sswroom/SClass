#include "Stdafx.h"
#include "DB/SQLGenerator.h"
#include "SSWR/AVIRead/AVIRDBCopyTablesForm.h"

#define VERBOSE
#if defined(VERBOSE)
#include <stdio.h>
#endif

void __stdcall SSWR::AVIRead::AVIRDBCopyTablesForm::OnSourceDBChg(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRDBCopyTablesForm> me = userObj.GetNN<SSWR::AVIRead::AVIRDBCopyTablesForm>();
	NN<DB::DBManagerCtrl> ctrl;
	NN<DB::ReadingDB> db;
	if (me->cboSourceConn->GetSelectedItem().GetOpt<DB::DBManagerCtrl>().SetTo(ctrl))
	{
		me->cboSourceSchema->ClearItems();
		if (!ctrl->Connect() || !ctrl->GetDB().SetTo(db))
		{
			me->ui->ShowMsgOK(CSTR("Error in connecting to database"), CSTR("Copy Tables"), me);
			return;
		}
		if (db->IsDBTool())
		{
			UTF8Char sbuff[128];
			UnsafeArray<UTF8Char> sptr;
			NN<DB::ReadingDBTool> dbt = NN<DB::ReadingDBTool>::ConvertFrom(db);
			DB::Collation collation;
			NN<Text::String> dbName = Text::String::OrEmpty(dbt->GetCurrDBName());
			me->txtSourceDB->SetText(dbName->ToCString());
			if (dbt->GetDBCollation(dbName->ToCString(), collation))
			{
				sptr = DB::DBUtil::SDBCollation(sbuff, collation, dbt->GetSQLType());
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
		Data::ArrayListStringNN schemaNames;
		NN<Text::String> s;
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

void __stdcall SSWR::AVIRead::AVIRDBCopyTablesForm::OnSourceSelectClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRDBCopyTablesForm> me = userObj.GetNN<SSWR::AVIRead::AVIRDBCopyTablesForm>();
	NN<DB::DBManagerCtrl> ctrl;
	NN<DB::ReadingDB> db;
	if (!me->cboSourceConn->GetSelectedItem().GetOpt<DB::DBManagerCtrl>().SetTo(ctrl) || !ctrl->GetDB().SetTo(db))
		return;
	UTF8Char sbuff[512];
	UnsafeArray<UTF8Char> sptr;
	if (!me->cboSourceSchema->GetSelectedItemText(sbuff).SetTo(sptr))
		return;
	Data::ArrayListStringNN tableNames;
	db->QueryTableNames(CSTRP(sbuff, sptr), tableNames);
	if (tableNames.GetCount() == 0)
	{
		me->ui->ShowMsgOK(CSTR("Tables not found"), CSTR("Copy Tables"), me);
	}
	else
	{
		me->dataConn = db;
		OPTSTR_DEL(me->dataSchema);
		me->dataTables.FreeAll();
		me->dataTableSelected.Clear();
		me->dataSchema = Text::String::NewP(sbuff, sptr);
		me->dataTables.AddAll(tableNames);
		me->lvData->ClearItems();
		UOSInt i = 0;
		UOSInt j = tableNames.GetCount();
		while (i < j)
		{
			me->dataTableSelected.Add(true);
			me->lvData->AddItem(CSTR("Selected"), 0);
			me->lvData->SetSubItem(i, 1, Text::String::OrEmpty(tableNames.GetItem(i)));
			i++;
		}
		me->tcMain->SetSelectedIndex(1);
	}
}

void __stdcall SSWR::AVIRead::AVIRDBCopyTablesForm::OnDestDBChg(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRDBCopyTablesForm> me = userObj.GetNN<SSWR::AVIRead::AVIRDBCopyTablesForm>();
	NN<DB::DBManagerCtrl> ctrl;
	if (me->cboDestDB->GetSelectedItem().GetOpt<DB::DBManagerCtrl>().SetTo(ctrl))
	{
		me->cboDestSchema->ClearItems();
		if (!ctrl->Connect())
		{
			me->ui->ShowMsgOK(CSTR("Error in connecting to database"), CSTR("Copy Tables"), me);
			return;
		}
		NN<DB::ReadingDB> db;
		if (!ctrl->GetDB().SetTo(db))
		{
			me->ui->ShowMsgOK(CSTR("Error in getting database"), CSTR("Copy Tables"), me);
			return;
		}
		Data::ArrayListStringNN schemaNames;
		NN<Text::String> s;
		db->QuerySchemaNames(schemaNames);
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
		if (!db->IsDBTool() || !((DB::ReadingDBTool*)db.Ptr())->CanModify())
		{
			me->txtSQLType->SetText(CSTR("Read-only database"));
		}
		else
		{
			NN<DB::DBTool> dbt = NN<DB::DBTool>::ConvertFrom(db);
			me->txtSQLType->SetText(DB::SQLTypeGetName(dbt->GetSQLType()));
		}
	}
}

void __stdcall SSWR::AVIRead::AVIRDBCopyTablesForm::OnCopyClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRDBCopyTablesForm> me = userObj.GetNN<SSWR::AVIRead::AVIRDBCopyTablesForm>();
	NN<DB::ReadingDB> dataConn;
	if (!me->dataConn.SetTo(dataConn))
	{
		return;
	}
	NN<DB::DBManagerCtrl> destDB;
	if (!me->cboDestDB->GetSelectedItem().GetOpt<DB::DBManagerCtrl>().SetTo(destDB))
	{
		me->ui->ShowMsgOK(CSTR("Please select a destination DB first"), CSTR("Copy Tables"), me);
		return;
	}
	if (!destDB->Connect())
	{
		me->ui->ShowMsgOK(CSTR("Error in connecting destination DB"), CSTR("Copy Tables"), me);
		return;
	}
	NN<DB::ReadingDB> destConn;
	if (!destDB->GetDB().SetTo(destConn))
	{
		me->ui->ShowMsgOK(CSTR("Error in getting destination DB"), CSTR("Copy Tables"), me);
		return;
	}
	NN<DB::DBTool> destDBTool;
	if (!destConn->IsDBTool() || !((DB::ReadingDBTool*)destConn.Ptr())->CanModify())
	{
		me->ui->ShowMsgOK(CSTR("Destination DB is read-onlyl"), CSTR("Copy Tables"), me);
		return;
	}
	destDBTool = NN<DB::DBTool>::ConvertFrom(destConn);
	UTF8Char destSchema[512];
	UnsafeArray<UTF8Char> destSchemaEnd;
	if (!me->cboDestSchema->GetSelectedItemText(destSchema).SetTo(destSchemaEnd))
	{
		me->ui->ShowMsgOK(CSTR("Please select a destination Schema first"), CSTR("Copy Tables"), me);
		return;
	}
	UOSInt destTableType = me->cboDestTableType->GetSelectedIndex();
	Bool copyData = me->chkDestCopyData->IsChecked();
	Bool createDB = me->chkDestCreateDDB->IsChecked();
	DB::SQLBuilder sql(destDBTool);
	Text::StringBuilderUTF8 sb;
	Optional<DB::TableDef> tabDef;
	NN<DB::TableDef> nntabDef;
	NN<Text::String> tableName;
	NN<DB::DBReader> r;
	if (destDBTool->GetSQLType() == DB::SQLType::MySQL)
	{
		destDBTool->ExecuteNonQuery(CSTR("set sql_mode=CONCAT(@@Session.sql_mode,',NO_AUTO_VALUE_ON_ZERO')"));
	}
	if (createDB)
	{
		if (dataConn->IsDBTool())
		{
			NN<DB::ReadingDBTool> dbt = NN<DB::ReadingDBTool>::ConvertFrom(dataConn);
			DB::Collation collation;
			Optional<Text::String> dbName = dbt->GetCurrDBName();
			Text::CStringNN sdbName = OPTSTR_CSTR(dbName).OrEmpty();
			me->txtSourceDB->SetText(sdbName);
			if (dbt->GetDBCollation(sdbName, collation))
			{
				if (!destDBTool->CreateDatabase(sdbName, &collation))
				{
					me->ui->ShowMsgOK(CSTR("Error in creating database"), CSTR("Copy Tables"), me);
					return;
				}
				if (!destDBTool->ChangeDatabase(sdbName))
				{
					me->ui->ShowMsgOK(CSTR("Error in changing to new database"), CSTR("Copy Tables"), me);
					return;
				}
			}
			else
			{
				me->ui->ShowMsgOK(CSTR("Error in parsing database collation"), CSTR("Copy Tables"), me);
				return;
			}
		}
		else
		{
			me->ui->ShowMsgOK(CSTR("Source Database does not support getting database collation"), CSTR("Copy Tables"), me);
			return;
		}
	}
	UOSInt i = 0;
	UOSInt j = me->dataTables.GetCount();
	while (i < j)
	{
		if (me->dataTables.GetItem(i).SetTo(tableName) && me->dataTableSelected.GetItem(i))
		{
			Bool succ = true;
			tabDef = dataConn->GetTableDef(OPTSTR_CSTR(me->dataSchema), tableName->ToCString());
			if (succ && destTableType == 3)
			{
				if (!tabDef.SetTo(nntabDef))
				{
					me->lvData->SetSubItem(i, 2, CSTR("Error in getting table definition"));
					succ = false;
				}
				else
				{
					sql.Clear();
					if (!DB::SQLGenerator::GenDropTableCmd(sql, CSTRP(destSchema, destSchemaEnd), tableName->ToCString()))
					{
						me->lvData->SetSubItem(i, 2, CSTR("Error in generating drop command"));
						succ = false;
					}
					else if (destDBTool->ExecuteNonQuery(sql.ToCString()) <= -2)
					{
						sb.ClearStr();
						destDBTool->GetLastErrorMsg(sb);
						me->lvData->SetSubItem(i, 2, sb.ToCString());
						succ = false;
					}
				}

			}
			if (succ && (destTableType == 0 || destTableType == 3))
			{
				if (!tabDef.SetTo(nntabDef))
				{
					me->lvData->SetSubItem(i, 2, CSTR("Error in getting table definition"));
					succ = false;
				}
				else
				{
					sql.Clear();
					if (!DB::SQLGenerator::GenCreateTableCmd(sql, CSTRP(destSchema, destSchemaEnd), tableName->ToCString(), nntabDef, false))
					{
						me->lvData->SetSubItem(i, 2, CSTR("Error in generating create command"));
						succ = false;
					}
					else if (destDBTool->ExecuteNonQuery(sql.ToCString()) <= -2)
					{
						sb.ClearStr();
						destDBTool->GetLastErrorMsg(sb);
						me->lvData->SetSubItem(i, 2, sb.ToCString());
						succ = false;
					}
					else
					{
						me->lvData->SetSubItem(i, 2, CSTR("Table created"));
					}
				}
			}
			else if (succ && destTableType == 1)
			{
				succ = destDBTool->DeleteTableData(CSTRP(destSchema, destSchemaEnd), tableName->ToCString());
				if (!succ)
				{
					me->lvData->SetSubItem(i, 2, CSTR("Error in deleting table data"));
				}
			}
			if (succ && copyData)
			{
				UOSInt rowCopied = 0;
				if (dataConn->QueryTableData(OPTSTR_CSTR(me->dataSchema), tableName->ToCString(), nullptr, 0, 0, 0, nullptr).SetTo(r))
				{
					Text::StringBuilderUTF8 sbInsert;
					UOSInt nInsert = 0;

					while (r->ReadNext())
					{
						sql.Clear();
						if (tabDef.NotNull())
							DB::SQLGenerator::GenInsertCmd(sql, CSTRP(destSchema, destSchemaEnd), tableName->ToCString(), tabDef, r);
						else
							DB::SQLGenerator::GenInsertCmd(sql, CSTRP(destSchema, destSchemaEnd), tableName->ToCString(), r);
						UOSInt k = sql.ToCString().IndexOf(UTF8STRC(" values ("));
						if (true) //k == INVALID_INDEX)
						{
							if (destDBTool->ExecuteNonQuery(sql.ToCString()) != 1)
							{
	#if defined(VERBOSE)
								printf("DBCopyTables: Error SQL: %s\r\n", sql.ToString().Ptr());
	#endif
								sb.ClearStr();
								destDBTool->GetLastErrorMsg(sb);
								me->lvData->SetSubItem(i, 2, sb.ToCString());
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
									me->lvData->SetSubItem(i, 2, sb.ToCString());
									{
										IO::FileStream fs(CSTR("CopyTableFail.sql"), IO::FileMode::Append, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
										sbInsert.Append(CSTR("\r\n"));
										fs.Write(sbInsert.ToByteArray());
									}
									succ = false;
									break;
								}
							}
						}
					}
					dataConn->CloseReader(r);
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
							me->lvData->SetSubItem(i, 2, sb.ToCString());
							succ = false;
						}
					}
					if (succ)
					{
						sb.ClearStr();
						sb.AppendUOSInt(rowCopied);
						sb.AppendC(UTF8STRC(" rows copied"));
						me->lvData->SetSubItem(i, 2, sb.ToCString());
					}
				}
				else
				{
					sb.ClearStr();
					dataConn->GetLastErrorMsg(sb);
					me->lvData->SetSubItem(i, 2, sb.ToCString());
				}
			}
			tabDef.Delete();
		}
		i++;
	}
}

void __stdcall SSWR::AVIRead::AVIRDBCopyTablesForm::OnDataDblClk(AnyType userObj, UOSInt index)
{
	NN<SSWR::AVIRead::AVIRDBCopyTablesForm> me = userObj.GetNN<SSWR::AVIRead::AVIRDBCopyTablesForm>();
	if (index != INVALID_INDEX)
	{
		Bool lastSel = me->dataTableSelected.GetItem(index);
		lastSel = !lastSel;
		me->dataTableSelected.SetItem(index, lastSel);
		me->lvData->SetSubItem(index, 0, lastSel?CSTR("Selected"):CSTR(""));
	}
}

void __stdcall SSWR::AVIRead::AVIRDBCopyTablesForm::OnDataSelectAllClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRDBCopyTablesForm> me = userObj.GetNN<SSWR::AVIRead::AVIRDBCopyTablesForm>();
	me->SetAllSelect(true);
}

void __stdcall SSWR::AVIRead::AVIRDBCopyTablesForm::OnDataSelectNoneClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRDBCopyTablesForm> me = userObj.GetNN<SSWR::AVIRead::AVIRDBCopyTablesForm>();
	me->SetAllSelect(false);
}

void SSWR::AVIRead::AVIRDBCopyTablesForm::SetAllSelect(Bool selected)
{
	UOSInt i = 0;
	UOSInt j = this->dataTableSelected.GetCount();
	Bool currSel;
	while (i < j)
	{
		currSel = this->dataTableSelected.GetItem(i);
		if (currSel != selected)
		{
			this->dataTableSelected.SetItem(i, selected);
			this->lvData->SetSubItem(i, 0, selected?CSTR("Selected"):CSTR(""));
		}
		i++;
	}
}

SSWR::AVIRead::AVIRDBCopyTablesForm::AVIRDBCopyTablesForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core, NN<Data::ArrayListNN<DB::DBManagerCtrl>> dbList) : UI::GUIForm(parent, 1024, 768, ui)
{
	this->SetFont(nullptr, 8.25, false);
	this->SetText(CSTR("Copy Tables"));
	this->core = core;
	this->dbList = dbList;
	this->dataSchema = nullptr;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	this->tcMain = ui->NewTabControl(*this);
	this->tcMain->SetDockType(UI::GUIControl::DOCK_FILL);

	this->tpSource = this->tcMain->AddTabPage(CSTR("Source"));
	this->lblSourceConn = ui->NewLabel(this->tpSource, CSTR("Connection"));
	this->lblSourceConn->SetRect(4, 4, 100, 23, false);
	this->cboSourceConn = ui->NewComboBox(this->tpSource, false);
	this->cboSourceConn->SetRect(104, 4, 400, 23, false);
	this->lblSourceDB = ui->NewLabel(this->tpSource, CSTR("Database"));
	this->lblSourceDB->SetRect(4, 28, 100, 23, false);
	this->txtSourceDB = ui->NewTextBox(this->tpSource, CSTR(""));
	this->txtSourceDB->SetRect(104, 28, 200, 23, false);
	this->txtSourceDB->SetReadOnly(true);
	this->lblSourceCollation = ui->NewLabel(this->tpSource, CSTR("Collation"));
	this->lblSourceCollation->SetRect(4, 52, 100, 23, false);
	this->txtSourceCollation = ui->NewTextBox(this->tpSource, CSTR(""));
	this->txtSourceCollation->SetRect(104, 52, 200, 23, false);
	this->txtSourceCollation->SetReadOnly(true);
	this->lblSourceSchema = ui->NewLabel(this->tpSource, CSTR("Schema"));
	this->lblSourceSchema->SetRect(4, 76, 100, 23, false);
	this->cboSourceSchema = ui->NewComboBox(this->tpSource, false);
	this->cboSourceSchema->SetRect(104, 76, 200, 23, false);
	this->btnSourceSelect = ui->NewButton(this->tpSource, CSTR("Select"));
	this->btnSourceSelect->SetRect(104, 100, 75, 23, false);
	this->btnSourceSelect->HandleButtonClick(OnSourceSelectClicked, this);	
	this->cboSourceConn->HandleSelectionChange(OnSourceDBChg, this);

	this->tpData = this->tcMain->AddTabPage(CSTR("Data"));
	this->pnlData = ui->NewPanel(this->tpData);
	this->pnlData->SetRect(0, 0, 100, 31, false);
	this->pnlData->SetDockType(UI::GUIControl::DOCK_TOP);
	this->btnDataSelectNone = ui->NewButton(this->pnlData, CSTR("Select None"));
	this->btnDataSelectNone->SetRect(4, 4, 75, 23, false);
	this->btnDataSelectNone->HandleButtonClick(OnDataSelectNoneClicked, this);
	this->btnDataSelectAll = ui->NewButton(this->pnlData, CSTR("Select All"));
	this->btnDataSelectAll->SetRect(84, 4, 75, 23, false);
	this->btnDataSelectAll->HandleButtonClick(OnDataSelectAllClicked, this);
	this->grpDest = ui->NewGroupBox(this->tpData, CSTR("Destination"));
	this->grpDest->SetRect(0, 0, 100, 154, false);
	this->grpDest->SetDockType(UI::GUIControl::DOCK_BOTTOM);
	this->lvData = ui->NewListView(this->tpData, UI::ListViewStyle::Table, 3);
	this->lvData->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lvData->SetFullRowSelect(true);
	this->lvData->SetShowGrid(true);
	this->lvData->AddColumn(CSTR("Select"), 100);
	this->lvData->AddColumn(CSTR("Table"), 100);
	this->lvData->AddColumn(CSTR("Status"), 400);
	this->lvData->HandleDblClk(OnDataDblClk, this);
	this->lblDestDB = ui->NewLabel(this->grpDest, CSTR("Connection"));
	this->lblDestDB->SetRect(4, 4, 100, 23, false);
	this->cboDestDB = ui->NewComboBox(this->grpDest, false);
	this->cboDestDB->SetRect(104, 4, 400, 23, false);
	this->cboDestDB->HandleSelectionChange(OnDestDBChg, this);
	this->lblDestSchema = ui->NewLabel(this->grpDest, CSTR("Schema"));
	this->lblDestSchema->SetRect(4, 28, 100, 23, false);
	this->cboDestSchema = ui->NewComboBox(this->grpDest, false);
	this->cboDestSchema->SetRect(104, 28, 200, 23, false);
	this->lblSQLType = ui->NewLabel(this->grpDest, CSTR("SQL Type"));
	this->lblSQLType->SetRect(4, 52, 100, 23, false);
	this->txtSQLType = ui->NewTextBox(this->grpDest, CSTR(""));
	this->txtSQLType->SetReadOnly(true);
	this->txtSQLType->SetRect(104, 52, 150, 23, false);
	this->lblDestOptions = ui->NewLabel(this->grpDest, CSTR("Options"));
	this->lblDestOptions->SetRect(4, 76, 100, 23, false);
	this->cboDestTableType = ui->NewComboBox(this->grpDest, false);
	this->cboDestTableType->SetRect(104, 76, 100, 23, false);
	this->cboDestTableType->AddItem(CSTR("Create Table"), 0);
	this->cboDestTableType->AddItem(CSTR("Remove Data"), 0);
	this->cboDestTableType->AddItem(CSTR("Append Data"), 0);
	this->cboDestTableType->AddItem(CSTR("Recreate Table"), 0);
	this->cboDestTableType->SetSelectedIndex(0);
	this->chkDestCopyData = ui->NewCheckBox(this->grpDest, CSTR("Copy Data"), true);
	this->chkDestCopyData->SetRect(204, 76, 100, 23, false);
	this->chkDestCreateDDB = ui->NewCheckBox(this->grpDest, CSTR("Create DB"), false);
	this->chkDestCreateDDB->SetRect(304, 76, 100, 23, false);
	this->btnCopy = ui->NewButton(this->grpDest, CSTR("Copy"));
	this->btnCopy->SetRect(104, 100, 75, 23, false);
	this->btnCopy->HandleButtonClick(OnCopyClicked, this);

	Text::StringBuilderUTF8 sb;
	NN<DB::DBManagerCtrl> ctrl;
	UOSInt firstActive = INVALID_INDEX;
	UOSInt i = 0;
	UOSInt j = this->dbList->GetCount();
	while (i < j)
	{
		ctrl = this->dbList->GetItemNoCheck(i);
		if (firstActive == INVALID_INDEX && ctrl->GetStatus() == DB::DBManagerCtrl::ConnStatus::Connected)
		{
			firstActive = i;
		}
		sb.ClearStr();
		ctrl->GetConnName(sb);
		this->cboSourceConn->AddItem(sb.ToCString(), ctrl.Ptr());
		this->cboDestDB->AddItem(sb.ToCString(), ctrl.Ptr());
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
	OPTSTR_DEL(this->dataSchema);
	this->dataTables.FreeAll();
}

void SSWR::AVIRead::AVIRDBCopyTablesForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}

void SSWR::AVIRead::AVIRDBCopyTablesForm::SetSourceDB(UOSInt index)
{
	this->cboSourceConn->SetSelectedIndex(index);
}
