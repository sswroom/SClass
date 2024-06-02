#include "Stdafx.h"
#include "Data/ArrayListStrUTF8.h"
#include "Data/Sort/ArtificialQuickSortCmp.h"
#include "DB/ColDef.h"
#include "DB/DBExporter.h"
#include "DB/DBManager.h"
#include "DB/JavaDBUtil.h"
#include "DB/SQLFileReader.h"
#include "DB/SQLGenerator.h"
#include "DB/SQLiteFile.h"
#include "IO/DirectoryPackage.h"
#include "IO/FileStream.h"
#include "IO/Path.h"
#include "IO/StmData/FileData.h"
#include "Map/BaseMapLayer.h"
#include "Math/CoordinateSystemManager.h"
#include "Math/Math.h"
#include "Net/SSLEngineFactory.h"
#include "SSWR/AVIRead/AVIRAccessConnForm.h"
#include "SSWR/AVIRead/AVIRDBCheckChgForm.h"
#include "SSWR/AVIRead/AVIRDBCopyTablesForm.h"
#include "SSWR/AVIRead/AVIRDBExportForm.h"
#include "SSWR/AVIRead/AVIRDBManagerForm.h"
#include "SSWR/AVIRead/AVIRMySQLConnForm.h"
#include "SSWR/AVIRead/AVIRMSSQLConnForm.h"
#include "SSWR/AVIRead/AVIRODBCDSNForm.h"
#include "SSWR/AVIRead/AVIRODBCStrForm.h"
#include "SSWR/AVIRead/AVIRPostgreSQLForm.h"
#include "Text/CharUtil.h"
#include "Text/MyString.h"
#include "UI/Clipboard.h"
#include "UI/GUIFileDialog.h"
#include "UtilUI/TextInputDialog.h"

#define MAX_ROW_CNT 1000
#define DBCONNFILE CSTR("DBConn.dat")

typedef enum
{
	MNU_CONN_ODBCDSN = 100,
	MNU_CONN_ODBCSTR,
	MNU_CONN_MYSQL,
	MNU_CONN_MSSQL,
	MNU_CONN_ACCESS,
	MNU_CONN_POSTGRESQL,
	MNU_TOOL_COPY_TABLES,

	MNU_CONN_REMOVE,
	MNU_CONN_COPY_STR,

	MNU_SCHEMA_NEW,
	MNU_SCHEMA_DELETE,

	MNU_TABLE_JAVA,
	MNU_TABLE_CPP_HEADER,
	MNU_TABLE_CPP_SOURCE,
	MNU_TABLE_CREATE_MYSQL,
	MNU_TABLE_CREATE_MYSQL8,
	MNU_TABLE_CREATE_MSSQL,
	MNU_TABLE_CREATE_POSTGRESQL,
	MNU_TABLE_EXPORT_MYSQL,
	MNU_TABLE_EXPORT_MYSQL8,
	MNU_TABLE_EXPORT_MSSQL,
	MNU_TABLE_EXPORT_POSTGRESQL,
	MNU_TABLE_EXPORT_OPTION,
	MNU_TABLE_EXPORT_CSV,
	MNU_TABLE_EXPORT_SQLITE,
	MNU_TABLE_EXPORT_HTML,
	MNU_TABLE_EXPORT_PLIST,
	MNU_TABLE_EXPORT_EXCELXML,
	MNU_TABLE_EXPORT_XLSX,
	MNU_TABLE_CHECK_CHANGE
} MenuEvent;

void __stdcall SSWR::AVIRead::AVIRDBManagerForm::OnConnSelChg(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRDBManagerForm> me = userObj.GetNN<SSWR::AVIRead::AVIRDBManagerForm>();
	DB::DBManagerCtrl *ctrl = (DB::DBManagerCtrl*)me->lbConn->GetSelectedItem().p;
	if (ctrl == 0)
	{
		me->currDB = 0;
	}
	else
	{
		me->currDB = ctrl->GetDB();
	}
	me->lvVariable->ClearItems();
	me->lvSvrConn->ClearItems();
	me->UpdateDatabaseList();
	me->UpdateSchemaList();
}

Bool __stdcall SSWR::AVIRead::AVIRDBManagerForm::OnConnRClicked(AnyType userObj, Math::Coord2D<OSInt> scnPos, MouseButton btn)
{
	NN<SSWR::AVIRead::AVIRDBManagerForm> me = userObj.GetNN<SSWR::AVIRead::AVIRDBManagerForm>();
	UOSInt i = me->lbConn->GetSelectedIndex();
	if (i != INVALID_INDEX)
	{
		me->mnuConn->ShowMenu(me->lbConn, scnPos);
	}
	return false;
}

void __stdcall SSWR::AVIRead::AVIRDBManagerForm::OnConnDblClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRDBManagerForm> me = userObj.GetNN<SSWR::AVIRead::AVIRDBManagerForm>();
	DB::DBManagerCtrl *ctrl = (DB::DBManagerCtrl*)me->lbConn->GetSelectedItem().p;
	if (ctrl)
	{
		if (ctrl->Connect())
		{
			OnConnSelChg(me);
		}
		else
		{
			me->ui->ShowMsgOK(CSTR("Error in connecting to database"), CSTR("DB Manager"), me);
		}
	}
}

void __stdcall SSWR::AVIRead::AVIRDBManagerForm::OnSchemaSelChg(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRDBManagerForm> me = userObj.GetNN<SSWR::AVIRead::AVIRDBManagerForm>();
	me->UpdateTableList();
}

Bool __stdcall SSWR::AVIRead::AVIRDBManagerForm::OnSchemaRClicked(AnyType userObj, Math::Coord2D<OSInt> scnPos, MouseButton btn)
{
	NN<SSWR::AVIRead::AVIRDBManagerForm> me = userObj.GetNN<SSWR::AVIRead::AVIRDBManagerForm>();
	UOSInt i = me->lbSchema->GetSelectedIndex();
	if (i != INVALID_INDEX)
	{
		me->mnuSchema->ShowMenu(me->lbSchema, scnPos);
	}
	return false;
}

void __stdcall SSWR::AVIRead::AVIRDBManagerForm::OnTableSelChg(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRDBManagerForm> me = userObj.GetNN<SSWR::AVIRead::AVIRDBManagerForm>();
	Optional<Text::String> tableName = me->lbTable->GetSelectedItemTextNew();
	Optional<Text::String> schemaName = me->lbSchema->GetSelectedItemTextNew();
	me->UpdateTableData(OPTSTR_CSTR(schemaName), tableName);
	OPTSTR_DEL(schemaName);
	OPTSTR_DEL(tableName);
}

Bool __stdcall SSWR::AVIRead::AVIRDBManagerForm::OnTableRClicked(AnyType userObj, Math::Coord2D<OSInt> scnPos, MouseButton btn)
{
	NN<SSWR::AVIRead::AVIRDBManagerForm> me = userObj.GetNN<SSWR::AVIRead::AVIRDBManagerForm>();
	UOSInt i = me->lbTable->GetSelectedIndex();
	if (i != INVALID_INDEX)
	{
		me->mnuTable->ShowMenu(me->lbTable, scnPos);
	}
	return false;
}

void __stdcall SSWR::AVIRead::AVIRDBManagerForm::OnMapSchemaSelChg(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRDBManagerForm> me = userObj.GetNN<SSWR::AVIRead::AVIRDBManagerForm>();
	me->UpdateMapTableList();
}

void __stdcall SSWR::AVIRead::AVIRDBManagerForm::OnMapTableSelChg(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRDBManagerForm> me = userObj.GetNN<SSWR::AVIRead::AVIRDBManagerForm>();
	NN<DB::ReadingDB> currDB;
	if (!me->currDB.SetTo(currDB))
	{
		return;
	}
	Optional<Text::String> schemaName = me->lbMapSchema->GetSelectedItemTextNew();
	Optional<Text::String> tableName = me->lbMapTable->GetSelectedItemTextNew();
	NN<Text::String> nnschemaName;
	NN<Text::String> nntableName;
	if (schemaName.SetTo(nnschemaName) && tableName.SetTo(nntableName))
	{
		me->dbLayer->SetDatabase(currDB, nnschemaName->ToCString(), nntableName->ToCString(), false);
		Math::RectAreaDbl rect;
		if (me->dbLayer->GetBounds(rect))
		{
			Math::Coord2DDbl center = rect.GetCenter();
			if (center.x != 0 || center.y != 0)
			{
				NN<Math::CoordinateSystem> csysLayer = me->dbLayer->GetCoordinateSystem();
				NN<Math::CoordinateSystem> csysEnv = me->mapEnv->GetCoordinateSystem();
				if (!csysLayer->Equals(csysEnv))
				{
					center = Math::CoordinateSystem::Convert(csysLayer, csysEnv, center);
				}
				me->mapMain->PanToMapXY(center);
			}
		}
		OnLayerUpdated(me);
	}
	OPTSTR_DEL(schemaName);
	OPTSTR_DEL(tableName);
}

Bool __stdcall SSWR::AVIRead::AVIRDBManagerForm::OnMapMouseDown(AnyType userObj, Math::Coord2D<OSInt> scnPos, MouseButton button)
{
	NN<SSWR::AVIRead::AVIRDBManagerForm> me = userObj.GetNN<SSWR::AVIRead::AVIRDBManagerForm>();
	if (button == MBTN_LEFT)
	{
		me->mapDownPos = scnPos;
	}
	return false;
}

Bool __stdcall SSWR::AVIRead::AVIRDBManagerForm::OnMapMouseUp(AnyType userObj, Math::Coord2D<OSInt> scnPos, MouseButton button)
{
	NN<SSWR::AVIRead::AVIRDBManagerForm> me = userObj.GetNN<SSWR::AVIRead::AVIRDBManagerForm>();
	if (button == MBTN_LEFT)
	{
		if (me->mapDownPos == scnPos)
		{
			Map::GetObjectSess *sess;
			Int64 id;
			UOSInt i;
			UOSInt j;
			UTF8Char sbuff[512];
			UnsafeArray<UTF8Char> sptr;
			Math::Coord2DDbl mapPt = me->mapMain->ScnXY2MapXY(scnPos);
			NN<Math::CoordinateSystem> csys = me->mapEnv->GetCoordinateSystem();
			NN<Math::CoordinateSystem> lyrCSys = me->dbLayer->GetCoordinateSystem();
			if (!csys->Equals(lyrCSys))
			{
				mapPt = Math::CoordinateSystem::Convert(csys, lyrCSys, mapPt);
			}
			sess = me->dbLayer->BeginGetObject();
			id = me->dbLayer->GetNearestObjectId(sess, mapPt, mapPt);
			me->lvMapRecord->ClearItems();
			if (id == -1)
			{
			}
			else
			{
				Data::ArrayListInt64 arr;
				Map::NameArray *nameArr;
				Text::StringBuilderUTF8 sb;
				me->dbLayer->GetObjectIdsMapXY(arr, &nameArr, Math::RectAreaDbl(mapPt, mapPt), true);
				i = 0;
				j = me->dbLayer->GetColumnCnt();
				while (i < j)
				{
					sbuff[0] = 0;
					sptr = me->dbLayer->GetColumnName(sbuff, i).Or(sbuff);
					me->lvMapRecord->AddItem(CSTRP(sbuff, sptr), 0);
					sb.ClearStr();
					me->dbLayer->GetString(sb, nameArr, id, i);
					me->lvMapRecord->SetSubItem(i, 1, sb.ToCString());
					i++;
				}
				me->dbLayer->ReleaseNameArr(nameArr);
			}
			me->dbLayer->EndGetObject(sess);
			return false;
		}
	}
	return false;
}

void __stdcall SSWR::AVIRead::AVIRDBManagerForm::OnDatabaseChangeClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRDBManagerForm> me = userObj.GetNN<SSWR::AVIRead::AVIRDBManagerForm>();
	NN<DB::ReadingDB> currDB;
	if (me->currDB.SetTo(currDB))
	{
		NN<Text::String> dbName;
		if (me->lbDatabase->GetSelectedItemTextNew().SetTo(dbName))
		{
			if (currDB->ChangeDatabase(dbName->ToCString()))
			{
				me->UpdateTableData(CSTR_NULL, 0);
				me->UpdateSchemaList();
			}
			dbName->Release();
		}
	}
}

void __stdcall SSWR::AVIRead::AVIRDBManagerForm::OnDatabaseDeleteClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRDBManagerForm> me = userObj.GetNN<SSWR::AVIRead::AVIRDBManagerForm>();
	NN<DB::ReadingDB> currDB;
	if (me->currDB.SetTo(currDB))
	{
		if (currDB->IsDBTool() && NN<DB::ReadingDBTool>::ConvertFrom(currDB)->CanModify())
		{
			NN<Text::String> dbName;
			if (me->lbDatabase->GetSelectedItemTextNew().SetTo(dbName))
			{
				Text::StringBuilderUTF8 sb;
				sb.AppendC(UTF8STRC("Are you sure to delete database "));
				sb.Append(dbName);
				sb.AppendC(UTF8STRC("? This process cannot be undo."));
				if (me->ui->ShowMsgYesNo(sb.ToCString(), CSTR("DB Manager"), me))
				{
					if (NN<DB::DBTool>::ConvertFrom(currDB)->DeleteDatabase(dbName->ToCString()))
					{
						me->UpdateDatabaseList();
					}
					else
					{
						sb.ClearStr();
						sb.AppendC(UTF8STRC("Error in deleting database "));
						sb.Append(dbName);
						sb.AppendC(UTF8STRC(": "));
						currDB->GetLastErrorMsg(sb);
						me->ui->ShowMsgOK(sb.ToCString(), CSTR("DB Manager"), me);
					}
				}
				dbName->Release();
			}
		}
		else
		{
			me->ui->ShowMsgOK(CSTR("Selected database is read-only"), CSTR("DB Manager"), me);
		}
	}
}

void __stdcall SSWR::AVIRead::AVIRDBManagerForm::OnDatabaseNewClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRDBManagerForm> me = userObj.GetNN<SSWR::AVIRead::AVIRDBManagerForm>();
	NN<DB::ReadingDB> currDB;
	if (me->currDB.SetTo(currDB))
	{
		if (currDB->IsDBTool() && NN<DB::ReadingDBTool>::ConvertFrom(currDB)->CanModify())
		{
			UtilUI::TextInputDialog dlg(0, me->ui, me->core->GetMonitorMgr(), CSTR("DB Manager"), CSTR("Please enter database name to create"));
			if (dlg.ShowDialog(me))
			{
				Text::StringBuilderUTF8 sb;
				if (dlg.GetInputString(sb))
				{
					if (NN<DB::DBTool>::ConvertFrom(currDB)->CreateDatabase(sb.ToCString(), 0))
					{
						me->UpdateDatabaseList();
					}
					else
					{
						sb.ClearStr();
						sb.AppendC(UTF8STRC("Error in creating database: "));
						currDB->GetLastErrorMsg(sb);
						me->ui->ShowMsgOK(sb.ToCString(), CSTR("DB Manager"), me);
					}
				}
			}
		}
		else
		{
			me->ui->ShowMsgOK(CSTR("Selected database is read-only"), CSTR("DB Manager"), me);
		}
	}
}

void __stdcall SSWR::AVIRead::AVIRDBManagerForm::OnSQLExecClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRDBManagerForm> me = userObj.GetNN<SSWR::AVIRead::AVIRDBManagerForm>();
	NN<DB::ReadingDB> currDB;
	if (me->currDB.SetTo(currDB))
	{
		if (currDB->IsDBTool())
		{
			Text::StringBuilderUTF8 sb;
			me->txtSQL->GetText(sb);
			if (sb.GetLength() > 0)
			{
				NN<DB::DBReader> r;
				if (NN<DB::ReadingDBTool>::ConvertFrom(currDB)->ExecuteReader(sb.ToCString()).SetTo(r))
				{
					if (r->ColCount() == 0)
					{
						me->sqlFileMode = false;
						me->lvSQLResult->ClearAll();
						me->lvSQLResult->ChangeColumnCnt(1);
						me->lvSQLResult->AddColumn(CSTR("Row Changed"), 100);
						sb.ClearStr();
						sb.AppendOSInt(r->GetRowChanged());
						me->lvSQLResult->AddItem(sb.ToCString(), 0);
					}
					else
					{
						me->sqlFileMode = false;
						UpdateResult(r, me->lvSQLResult);
					}
					currDB->CloseReader(r);
				}
				else
				{
					sb.ClearStr();
					currDB->GetLastErrorMsg(sb);
					me->ui->ShowMsgOK(sb.ToCString(), CSTR("DB Manager"), me);
				}
			}
		}
	}
}

void __stdcall SSWR::AVIRead::AVIRDBManagerForm::OnSQLFileClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRDBManagerForm> me = userObj.GetNN<SSWR::AVIRead::AVIRDBManagerForm>();
	NN<DB::ReadingDB> currDB;
	if (me->currDB.SetTo(currDB) && currDB->IsDBTool())
	{
		NN<DB::ReadingDBTool> db = NN<DB::ReadingDBTool>::ConvertFrom(currDB);
		{
			NN<UI::GUIFileDialog> dlg = me->ui->NewFileDialog(L"SSWR", L"AVIRead", L"DBManagerSQLFile", false);
			dlg->SetAllowMultiSel(false);
			dlg->AddFilter(CSTR("*.sql"), CSTR("SQL file"));
			if (dlg->ShowDialog(me->GetHandle()))
			{
				me->RunSQLFile(db, dlg->GetFileName());
			}
			dlg.Delete();
		}
	}
}

void __stdcall SSWR::AVIRead::AVIRDBManagerForm::OnLayerUpdated(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRDBManagerForm> me = userObj.GetNN<SSWR::AVIRead::AVIRDBManagerForm>();
	me->mapMain->UpdateMap();
	me->mapMain->Redraw();
}

void __stdcall SSWR::AVIRead::AVIRDBManagerForm::OnVariableClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRDBManagerForm> me = userObj.GetNN<SSWR::AVIRead::AVIRDBManagerForm>();
	me->UpdateVariableList();
}

void __stdcall SSWR::AVIRead::AVIRDBManagerForm::OnSvrConnClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRDBManagerForm> me = userObj.GetNN<SSWR::AVIRead::AVIRDBManagerForm>();
	me->UpdateSvrConnList();
}

void __stdcall SSWR::AVIRead::AVIRDBManagerForm::OnSvrConnKillClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRDBManagerForm> me = userObj.GetNN<SSWR::AVIRead::AVIRDBManagerForm>();
	NN<DB::ReadingDB> currDB;
	if (me->currDB.SetTo(currDB) && currDB->IsDBTool() && NN<DB::ReadingDBTool>::ConvertFrom(currDB)->CanModify())
	{
		UOSInt index = me->lvSvrConn->GetSelectedIndex();
		if (index != INVALID_INDEX)
		{
			Int32 id = (Int32)me->lvSvrConn->GetItem(index).GetOSInt();
			if (NN<DB::DBTool>::ConvertFrom(currDB)->KillConnection(id))
			{
				me->UpdateSvrConnList();
			}
		}
	}
}

void __stdcall SSWR::AVIRead::AVIRDBManagerForm::OnFileHandler(AnyType userObj, Data::DataArray<NN<Text::String>> files)
{
	NN<SSWR::AVIRead::AVIRDBManagerForm> me = userObj.GetNN<SSWR::AVIRead::AVIRDBManagerForm>();
	Optional<DB::ReadingDBTool> db = 0;
	NN<DB::ReadingDBTool> nndb;
	NN<DB::ReadingDB> currDB;
	if (me->currDB.SetTo(currDB) && currDB->IsDBTool())
	{
		db = NN<DB::ReadingDBTool>::ConvertFrom(currDB);
	}
	Bool isSQLTab = (me->tcMain->GetSelectedPage().OrNull() == me->tpSQL.Ptr());
	UOSInt i = 0;
	UOSInt nFiles = files.GetCount();
	while (i < nFiles)
	{
		if (isSQLTab && db.SetTo(nndb) && files[i]->EndsWith(UTF8STRC(".sql")))
		{
			me->RunSQLFile(nndb, files[i]);
		}
		else
		{
			IO::Path::PathType pt = IO::Path::GetPathType(files[i]->ToCString());
			Optional<DB::ReadingDB> db;
			NN<DB::ReadingDB> nndb;
			if (pt == IO::Path::PathType::Directory)
			{
				IO::DirectoryPackage dpkg(files[i]);
				db = Optional<DB::ReadingDB>::ConvertFrom(me->core->GetParserList()->ParseObjectType(dpkg, IO::ParserType::ReadingDB));
			}
			else
			{
				IO::StmData::FileData fd(files[i], false);
				db = Optional<DB::ReadingDB>::ConvertFrom(me->core->GetParserList()->ParseFileType(fd, IO::ParserType::ReadingDB));
			}
			if (db.SetTo(nndb))
			{
				NN<DB::DBManagerCtrl> ctrl = DB::DBManagerCtrl::CreateFromFile(nndb, files[i], me->log, me->core->GetSocketFactory(), me->core->GetParserList());
				me->dbList.Add(ctrl);
				Text::StringBuilderUTF8 sb;
				ctrl->GetConnName(sb);
				me->lbConn->AddItem(sb.ToCString(), ctrl);
			}
		}
		i++;
	}
}

void SSWR::AVIRead::AVIRDBManagerForm::UpdateDatabaseList()
{
	this->lbDatabase->ClearItems();
	NN<DB::ReadingDB> currDB;
	if (!this->currDB.SetTo(currDB))
	{
		return;
	}
	NN<Text::String> dbName;
	Data::ArrayListStringNN dbNames;
	currDB->GetDatabaseNames(dbNames);
	ArtificialQuickSort_SortAList(&dbNames);
	Data::ArrayIterator<NN<Text::String>> it = dbNames.Iterator();
	while (it.HasNext())
	{
		dbName = it.Next();
		this->lbDatabase->AddItem(dbName, 0);
	}
	currDB->ReleaseDatabaseNames(dbNames);
}

void SSWR::AVIRead::AVIRDBManagerForm::UpdateSchemaList()
{
	this->lbSchema->ClearItems();
	this->lbMapSchema->ClearItems();
	NN<DB::ReadingDB> currDB;
	if (!this->currDB.SetTo(currDB))
	{
		return;
	}
	Data::ArrayListStringNN schemaNames;
	UOSInt i;
	UOSInt j;

	currDB->QuerySchemaNames(schemaNames);
	if (schemaNames.GetCount() == 0)
	{
		this->lbSchema->AddItem(CSTR(""), 0);
		this->lbMapSchema->AddItem(CSTR(""), 0);
	}
	i = 0;
	j = schemaNames.GetCount();
	while (i < j)
	{
		NN<Text::String> schemaName = Text::String::OrEmpty(schemaNames.GetItem(i));
		this->lbSchema->AddItem(schemaName, 0);
		this->lbMapSchema->AddItem(schemaName, 0);
		i++;
	}

	schemaNames.FreeAll();
	this->lbSchema->SetSelectedIndex(0);
	this->lbMapSchema->SetSelectedIndex(0);
}

void SSWR::AVIRead::AVIRDBManagerForm::UpdateTableList()
{
	this->lbTable->ClearItems();
	NN<DB::ReadingDB> currDB;
	if (!this->currDB.SetTo(currDB))
	{
		return;
	}
	Optional<Text::String> schemaName = this->lbSchema->GetSelectedItemTextNew();
	NN<Text::String> tableName;
	Data::ArrayListStringNN tableNames;
	UOSInt i = 0;
	UOSInt j = currDB->QueryTableNames(OPTSTR_CSTR(schemaName), tableNames);
	OPTSTR_DEL(schemaName);
	ArtificialQuickSort_SortAList(&tableNames);
	while (i < j)
	{
		tableName = Text::String::OrEmpty(tableNames.GetItem(i));
		this->lbTable->AddItem(tableName, 0);
		i++;
	}
	tableNames.FreeAll();
}

void SSWR::AVIRead::AVIRDBManagerForm::UpdateMapTableList()
{
	this->lbMapTable->ClearItems();
	NN<DB::ReadingDB> currDB;
	if (!this->currDB.SetTo(currDB))
	{
		return;
	}
	Optional<Text::String> schemaName = this->lbMapSchema->GetSelectedItemTextNew();
	NN<Text::String> tableName;
	Data::ArrayListStringNN tableNames;
	currDB->QueryTableNames(OPTSTR_CSTR(schemaName), tableNames);
	OPTSTR_DEL(schemaName);
	ArtificialQuickSort_SortAList(&tableNames);
	Data::ArrayIterator<NN<Text::String>> it = tableNames.Iterator();
	while (it.HasNext())
	{
		tableName = it.Next();
		this->lbMapTable->AddItem(tableName, 0);
	}
	tableNames.FreeAll();
}

void SSWR::AVIRead::AVIRDBManagerForm::UpdateTableData(Text::CString schemaName, Optional<Text::String> tableName)
{
	this->lvTable->ClearItems();
	this->lvTableResult->ClearItems();
	NN<Text::String> nntableName;
	NN<DB::ReadingDB> currDB;
	if (!tableName.SetTo(nntableName) || !this->currDB.SetTo(currDB))
	{
		return;
	}

	UTF8Char sbuff[256];
	UnsafeArray<UTF8Char> sptr;
	NN<Text::String> s;
	Optional<DB::TableDef> tabDef = 0;
	NN<DB::TableDef> nntabDef;
	NN<DB::DBReader> r;
	tabDef = currDB->GetTableDef(schemaName, nntableName->ToCString());

	if (currDB->QueryTableData(schemaName, nntableName->ToCString(), 0, 0, MAX_ROW_CNT, CSTR_NULL, 0).SetTo(r))
	{
		UpdateResult(r, this->lvTableResult);

		UOSInt k;
		if (tabDef.SetTo(nntabDef))
		{
			NN<DB::ColDef> col;
			Data::ArrayIterator<NN<DB::ColDef>> it = nntabDef->ColIterator();
			while (it.HasNext())
			{
				col = it.Next();
				k = this->lvTable->AddItem(col->GetColName(), 0);
				sptr = col->ToColTypeStr(sbuff);
				this->lvTable->SetSubItem(k, 1, CSTRP(sbuff, sptr));
				if (col->GetNativeType().SetTo(s))
					this->lvTable->SetSubItem(k, 2, s);
				this->lvTable->SetSubItem(k, 3, col->IsNotNull()?CSTR("NOT NULL"):CSTR("NULL"));
				this->lvTable->SetSubItem(k, 4, col->IsPK()?CSTR("PK"):CSTR(""));
				if (col->IsAutoInc())
				{
					if (col->GetAutoIncType() == DB::ColDef::AutoIncType::Always)
						sptr = Text::StrConcatC(sbuff, UTF8STRC("Always ("));
					else
						sptr = Text::StrConcatC(sbuff, UTF8STRC("Default ("));
					sptr = Text::StrInt64(sptr, col->GetAutoIncStartIndex());
					sptr = Text::StrConcatC(sptr, UTF8STRC(", "));
					sptr = Text::StrInt64(sptr, col->GetAutoIncStep());
					sptr = Text::StrConcatC(sptr, UTF8STRC(")"));
					this->lvTable->SetSubItem(k, 5, CSTRP(sbuff, sptr));
				}
				if (col->GetDefVal().SetTo(s))
					this->lvTable->SetSubItem(k, 6, s);
				if (col->GetAttr().SetTo(s))
					this->lvTable->SetSubItem(k, 7, s);
			}
			nntabDef.Delete();
		}
		else
		{
			DB::ColDef col(Text::String::NewEmpty());
			UOSInt j = r->ColCount();
			UOSInt i = 0;
			while (i < j)
			{
				r->GetColDef(i, col);
				k = this->lvTable->AddItem(col.GetColName(), 0);
				sptr = col.ToColTypeStr(sbuff);
				this->lvTable->SetSubItem(k, 1, CSTRP(sbuff, sptr));
				if (col.GetNativeType().SetTo(s))
					this->lvTable->SetSubItem(k, 2, s);
				this->lvTable->SetSubItem(k, 3, col.IsNotNull()?CSTR("NOT NULL"):CSTR("NULL"));
				this->lvTable->SetSubItem(k, 4, col.IsPK()?CSTR("PK"):CSTR(""));
				this->lvTable->SetSubItem(k, 5, col.IsAutoInc()?CSTR("AUTO_INCREMENT"):CSTR(""));
				if (col.GetDefVal().SetTo(s))
					this->lvTable->SetSubItem(k, 6, s);
				if (col.GetAttr().SetTo(s))
					this->lvTable->SetSubItem(k, 7, s);

				i++;
			}
		}

		currDB->CloseReader(r);
	}
	else
	{
		tabDef.Delete();
	}
}

void SSWR::AVIRead::AVIRDBManagerForm::UpdateResult(NN<DB::DBReader> r, NN<UI::GUIListView> lv)
{
	UOSInt i;
	UOSInt j;
	UOSInt k;
	UOSInt *colSize;

	lv->ClearAll();

	Text::StringBuilderUTF8 sb;
	{
		DB::ColDef col(CSTR(""));
		j = r->ColCount();
		lv->ChangeColumnCnt(j);
		i = 0;
		colSize = MemAlloc(UOSInt, j);
		while (i < j)
		{
			if (r->GetColDef(i, col))
			{
				lv->AddColumn(col.GetColName(), 100);
			}
			else
			{
				lv->AddColumn(CSTR("Unnamed"), 100);
			}
			colSize[i] = 0;
			i++;
		}
	}

	OSInt rowCnt = 0;
	while (r->ReadNext())
	{
		i = 1;
		sb.ClearStr();
		r->GetStr(0, sb);
		if (sb.GetLength() > colSize[0])
			colSize[0] = sb.GetLength();
		k = lv->AddItem(sb.ToCString(), 0);
		while (i < j)
		{
			sb.ClearStr();
			r->GetStr(i, sb);
			lv->SetSubItem(k, i, sb.ToCString());

			if (sb.GetLength() > colSize[i])
				colSize[i] = sb.GetLength();
			i++;
		}
		rowCnt++;
		if (rowCnt > MAX_ROW_CNT)
			break;
	}

	k = 0;
	i = j;
	while (i-- > 0)
	{
		k += colSize[i];
	}
	if (k > 0)
	{
		Double w = lv->GetSize().x;
		w -= 20 + UOSInt2Double(j) * 6;
		if (w < 0)
			w = 0;
		i = 0;
		while (i < j)
		{
			lv->SetColumnWidth(i, (UOSInt2Double(colSize[i]) * w / UOSInt2Double(k) + 6));
			i++;
		}
	}
	MemFree(colSize);
}

void SSWR::AVIRead::AVIRDBManagerForm::UpdateVariableList()
{
	Data::ArrayList<Data::TwinItem<Optional<Text::String>, Optional<Text::String>>> vars;
	Data::TwinItem<Optional<Text::String>, Optional<Text::String>> item(0);
	this->lvVariable->ClearItems();
	NN<DB::ReadingDB> currDB;
	if (this->currDB.SetTo(currDB) && currDB->IsDBTool())
	{
		NN<Text::String> s;
		UOSInt i = 0;
		UOSInt j = NN<DB::ReadingDBTool>::ConvertFrom(currDB)->GetVariables(vars);
		while (i < j)
		{
			item = vars.GetItem(i);
			this->lvVariable->AddItem(Text::String::OrEmpty(item.key), 0);
			if (item.value.SetTo(s)) this->lvVariable->SetSubItem(i, 1, s);
			i++;
		}
		NN<DB::ReadingDBTool>::ConvertFrom(currDB)->FreeVariables(vars);
	}
}

void SSWR::AVIRead::AVIRDBManagerForm::UpdateSvrConnList()
{
	UTF8Char sbuff[32];
	UnsafeArray<UTF8Char> sptr;
	this->lvSvrConn->ClearItems();
	NN<DB::ReadingDB> currDB;
	if (this->currDB.SetTo(currDB) && currDB->IsDBTool())
	{
		Data::ArrayListNN<DB::ReadingDBTool::ConnectionInfo> conns;
		NN<DB::ReadingDBTool::ConnectionInfo> conn;
		NN<Text::String> s;
		UOSInt i = 0;
		UOSInt j = NN<DB::ReadingDBTool>::ConvertFrom(currDB)->GetConnectionInfo(conns);
		while (i < j)
		{
			conn = conns.GetItemNoCheck(i);
			sptr = Text::StrInt32(sbuff, conn->id);
			this->lvSvrConn->AddItem(CSTRP(sbuff, sptr), (void*)(OSInt)conn->id);
			if (conn->status.SetTo(s)) this->lvSvrConn->SetSubItem(i, 1, s);
			if (conn->user.SetTo(s)) this->lvSvrConn->SetSubItem(i, 2, s);
			if (conn->clientHostName.SetTo(s)) this->lvSvrConn->SetSubItem(i, 3, s);
			if (conn->dbName.SetTo(s)) this->lvSvrConn->SetSubItem(i, 4, s);
			if (conn->cmd.SetTo(s)) this->lvSvrConn->SetSubItem(i, 5, s);
			sptr = Text::StrInt32(sbuff, conn->timeUsed);
			this->lvSvrConn->SetSubItem(i, 6, CSTRP(sbuff, sptr));
			if (conn->sql.SetTo(s)) this->lvSvrConn->SetSubItem(i, 7, s);
			i++;
		}
		NN<DB::ReadingDBTool>::ConvertFrom(currDB)->FreeConnectionInfo(conns);
	}
}

void SSWR::AVIRead::AVIRDBManagerForm::RunSQLFile(NN<DB::ReadingDBTool> db, NN<Text::String> fileName)
{
	UOSInt i = fileName->LastIndexOf(IO::Path::PATH_SEPERATOR);
	UOSInt rowInd;
	if (!this->sqlFileMode)
	{
		this->lvSQLResult->ClearAll();
		this->lvSQLResult->ChangeColumnCnt(2);
		this->lvSQLResult->AddColumn(CSTR("File Name"), 400);
		this->lvSQLResult->AddColumn(CSTR("Row Changed"), 100);
		this->sqlFileMode = true;
	}
	rowInd = this->lvSQLResult->AddItem(fileName->ToCString().Substring(i + 1), 0);
	this->lvSQLResult->SetSubItem(rowInd, 1, CSTR("0"));
	this->ui->ProcessMessages();
	Text::StringBuilderUTF8 sb;
	UOSInt rowsChanged = 0;
	Int64 lastShowTime = Data::DateTimeUtil::GetCurrTimeMillis();
	Int64 t;
	{
		IO::FileStream fs(fileName, IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
		DB::SQLFileReader reader(fs, db->GetSQLType(), true);
		OSInt thisRows;
		while (reader.NextSQL(sb))
		{
			NN<DB::DBReader> r;
			if (db->ExecuteReader(sb.ToCString()).SetTo(r))
			{
				if (r->ColCount() == 0)
				{
					thisRows = r->GetRowChanged();
					if (thisRows > 0)
					{
						rowsChanged += (UOSInt)thisRows;
					}
					t = Data::DateTimeUtil::GetCurrTimeMillis();
					if (t - lastShowTime >= 1000)
					{
						lastShowTime = t;							
						sb.ClearStr();
						sb.AppendUOSInt(rowsChanged);
						this->lvSQLResult->SetSubItem(rowInd, 1, sb.ToCString());
						this->ui->ProcessMessages();
					}
				}
				db->CloseReader(r);
			}
			else
			{
				sb.ClearStr();
				db->GetLastErrorMsg(sb);
				this->ui->ShowMsgOK(sb.ToCString(), CSTR("DB Manager"), this);
				break;
			}
			sb.ClearStr();
		}
	}
	sb.ClearStr();
	sb.AppendUOSInt(rowsChanged);
	this->lvSQLResult->SetSubItem(rowInd, 1, sb.ToCString());
	this->ui->ProcessMessages();
}

Optional<Data::Class> SSWR::AVIRead::AVIRDBManagerForm::CreateTableClass(Text::CString schemaName, Text::CStringNN tableName)
{
	NN<DB::ReadingDB> db;
	if (this->currDB.SetTo(db))
	{
		return DB::DBExporter::CreateTableClass(db, schemaName, tableName);
	}
	return 0;
}

void SSWR::AVIRead::AVIRDBManagerForm::CopyTableCreate(DB::SQLType sqlType, Bool axisAware)
{
	Optional<Text::String> schemaName = this->lbSchema->GetSelectedItemTextNew();
	Optional<Text::String> tableName = this->lbTable->GetSelectedItemTextNew();
	DB::SQLBuilder sql(sqlType, axisAware, 0);
	NN<DB::TableDef> tabDef;
	NN<Text::String> nntableName;
	NN<DB::ReadingDB> currDB;
	if (this->currDB.SetTo(currDB) && tableName.SetTo(nntableName) && currDB->GetTableDef(OPTSTR_CSTR(schemaName), nntableName->ToCString()).SetTo(tabDef))
	{
		if (!DB::SQLGenerator::GenCreateTableCmd(sql, OPTSTR_CSTR(schemaName), nntableName->ToCString(), tabDef, true))
		{
			this->ui->ShowMsgOK(CSTR("Error in generating Create SQL command"), CSTR("DB Manager"), this);
		}
		else
		{
			UI::Clipboard::SetString(this->GetHandle(), sql.ToCString());
		}
		tabDef.Delete();
	}
	else
	{
		this->ui->ShowMsgOK(CSTR("Error in getting table definition"), CSTR("DB Manager"), this);
	}
	OPTSTR_DEL(tableName);
	OPTSTR_DEL(schemaName);
}

void SSWR::AVIRead::AVIRDBManagerForm::ExportTableData(DB::SQLType sqlType, Bool axisAware)
{
	UTF8Char sbuff[512];
	UnsafeArray<UTF8Char> sptr;
	NN<DB::ReadingDB> db;
	if (!this->currDB.SetTo(db))
	{
		this->ui->ShowMsgOK(CSTR("Please select database first"), CSTR("DB Manager"), this);
		return;
	}
	Optional<Text::String> schemaName = this->lbSchema->GetSelectedItemTextNew();
	Optional<Text::String> tableName = this->lbTable->GetSelectedItemTextNew();
	NN<Text::String> s;
	sptr = sbuff;
	if (schemaName.SetTo(s) && s->leng > 0)
	{
		sptr = s->ConcatTo(sptr);
		*sptr++ = '_';
	}
	if (!tableName.SetTo(s))
	{
		this->ui->ShowMsgOK(CSTR("Please select table first"), CSTR("DB Manager"), this);
		return;
	}
	sptr = s->ConcatTo(sptr);
	*sptr++ = '_';
	sptr = Data::Timestamp::Now().ToString(sptr, "yyyyMMdd_HHmmss");
	sptr = Text::StrConcatC(sptr, UTF8STRC(".sql"));
	NN<UI::GUIFileDialog> dlg = this->ui->NewFileDialog(L"SSWR", L"AVIRead", L"DBManagerExportTable", true);
	dlg->AddFilter(CSTR("*.sql"), CSTR("SQL File"));
	dlg->SetFileName(CSTRP(sbuff, sptr));
	if (dlg->ShowDialog(this->GetHandle()))
	{
		IO::FileStream fs(dlg->GetFileName(), IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
		if (!DB::DBExporter::GenerateInsertSQLs(db, sqlType, axisAware, OPTSTR_CSTR(schemaName), s->ToCString(), this->currCond, fs))
		{
			this->ui->ShowMsgOK(CSTR("Error in reading table data"), CSTR("DB Manager"), this);
		}
	}
	dlg.Delete();
	OPTSTR_DEL(tableName);
	OPTSTR_CSTR(schemaName);
}

void SSWR::AVIRead::AVIRDBManagerForm::ExportTableCSV()
{
	UTF8Char sbuff[512];
	UnsafeArray<UTF8Char> sptr;
	NN<DB::ReadingDB> db;
	if (!this->currDB.SetTo(db))
	{
		this->ui->ShowMsgOK(CSTR("Please select database first"), CSTR("DB Manager"), this);
		return;
	}
	Optional<Text::String> schemaName = this->lbSchema->GetSelectedItemTextNew();
	NN<Text::String> tableName;
	NN<Text::String> s;
	if (this->lbTable->GetSelectedItemTextNew().SetTo(tableName))
	{
		sptr = sbuff;
		if (schemaName.SetTo(s) && s->leng > 0)
		{
			sptr = s->ConcatTo(sptr);
			*sptr++ = '_';
		}
		sptr = tableName->ConcatTo(sptr);
		*sptr++ = '_';
		sptr = Data::Timestamp::Now().ToString(sptr, "yyyyMMdd_HHmmss");
		sptr = Text::StrConcatC(sptr, UTF8STRC(".csv"));
		NN<UI::GUIFileDialog> dlg = this->ui->NewFileDialog(L"SSWR", L"AVIRead", L"DBManagerExportCSV", true);
		dlg->AddFilter(CSTR("*.csv"), CSTR("Comma-Seperated-Value File"));
		dlg->SetFileName(CSTRP(sbuff, sptr));
		if (dlg->ShowDialog(this->GetHandle()))
		{
			IO::FileStream fs(dlg->GetFileName(), IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
			if (!DB::DBExporter::GenerateCSV(db, OPTSTR_CSTR(schemaName), tableName->ToCString(), this->currCond, CSTR("\"null\""), fs, 65001))
			{
				this->ui->ShowMsgOK(CSTR("Error in reading table data"), CSTR("DB Manager"), this);
			}
		}
		dlg.Delete();
		tableName->Release();
	}
	OPTSTR_DEL(schemaName);
}

void SSWR::AVIRead::AVIRDBManagerForm::ExportTableSQLite()
{
	UTF8Char sbuff[512];
	UnsafeArray<UTF8Char> sptr;
	NN<DB::ReadingDB> db;
	if (!this->currDB.SetTo(db))
	{
		this->ui->ShowMsgOK(CSTR("Please select database first"), CSTR("DB Manager"), this);
		return;
	}
	Optional<Text::String> schemaName = this->lbSchema->GetSelectedItemTextNew();
	NN<Text::String> tableName;
	NN<Text::String> s;
	if (this->lbTable->GetSelectedItemTextNew().SetTo(tableName))
	{
		sptr = sbuff;
		if (schemaName.SetTo(s) && s->leng > 0)
		{
			sptr = s->ConcatTo(sptr);
			*sptr++ = '_';
		}
		sptr = tableName->ConcatTo(sptr);
		*sptr++ = '_';
		sptr = Data::Timestamp::Now().ToString(sptr, "yyyyMMdd_HHmmss");
		sptr = Text::StrConcatC(sptr, UTF8STRC(".sqlite"));
		NN<UI::GUIFileDialog> dlg = this->ui->NewFileDialog(L"SSWR", L"AVIRead", L"DBManagerExportSQLite", true);
		dlg->AddFilter(CSTR("*.sqlite"), CSTR("SQLite File"));
		dlg->SetFileName(CSTRP(sbuff, sptr));
		if (dlg->ShowDialog(this->GetHandle()))
		{
			Text::StringBuilderUTF8 sb;
			DB::SQLiteFile sqlite(dlg->GetFileName());
			if (!DB::DBExporter::GenerateSQLite(db, OPTSTR_CSTR(schemaName), tableName->ToCString(), this->currCond, sqlite, &sb))
			{
				this->ui->ShowMsgOK(sb.ToCString(), CSTR("DB Manager"), this);
			}
		}
		dlg.Delete();
		tableName->Release();
	}
	OPTSTR_DEL(schemaName);
}

void SSWR::AVIRead::AVIRDBManagerForm::ExportTableHTML()
{
	UTF8Char sbuff[512];
	UnsafeArray<UTF8Char> sptr;
	NN<DB::ReadingDB> db;
	if (!this->currDB.SetTo(db))
	{
		this->ui->ShowMsgOK(CSTR("Please select database first"), CSTR("DB Manager"), this);
		return;
	}
	Optional<Text::String> schemaName = this->lbSchema->GetSelectedItemTextNew();
	NN<Text::String> tableName;
	NN<Text::String> s;
	if (this->lbTable->GetSelectedItemTextNew().SetTo(tableName))
	{
		sptr = sbuff;
		if (schemaName.SetTo(s) && s->leng > 0)
		{
			sptr = s->ConcatTo(sptr);
			*sptr++ = '_';
		}
		sptr = tableName->ConcatTo(sptr);
		*sptr++ = '_';
		sptr = Data::Timestamp::Now().ToString(sptr, "yyyyMMdd_HHmmss");
		sptr = Text::StrConcatC(sptr, UTF8STRC(".html"));
		NN<UI::GUIFileDialog> dlg = this->ui->NewFileDialog(L"SSWR", L"AVIRead", L"DBManagerExportHTML", true);
		dlg->AddFilter(CSTR("*.html"), CSTR("HTML File"));
		dlg->SetFileName(CSTRP(sbuff, sptr));
		if (dlg->ShowDialog(this->GetHandle()))
		{
			IO::FileStream fs(dlg->GetFileName(), IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
			if (!DB::DBExporter::GenerateHTML(db, OPTSTR_CSTR(schemaName), tableName->ToCString(), this->currCond, fs, 65001))
			{
				this->ui->ShowMsgOK(CSTR("Error in exporting as PList"), CSTR("DB Manager"), this);
			}
		}
		dlg.Delete();
		tableName->Release();
	}
	OPTSTR_CSTR(schemaName);
}

void SSWR::AVIRead::AVIRDBManagerForm::ExportTablePList()
{
	UTF8Char sbuff[512];
	UnsafeArray<UTF8Char> sptr;
	NN<DB::ReadingDB> db;
	if (!this->currDB.SetTo(db))
	{
		this->ui->ShowMsgOK(CSTR("Please select database first"), CSTR("DB Manager"), this);
		return;
	}
	Optional<Text::String> schemaName = this->lbSchema->GetSelectedItemTextNew();
	NN<Text::String> tableName;
	NN<Text::String> s;
	if (this->lbTable->GetSelectedItemTextNew().SetTo(tableName))
	{
		sptr = sbuff;
		if (schemaName.SetTo(s) && s->leng > 0)
		{
			sptr = s->ConcatTo(sptr);
			*sptr++ = '_';
		}
		sptr = tableName->ConcatTo(sptr);
		*sptr++ = '_';
		sptr = Data::Timestamp::Now().ToString(sptr, "yyyyMMdd_HHmmss");
		sptr = Text::StrConcatC(sptr, UTF8STRC(".plist"));
		NN<UI::GUIFileDialog> dlg = this->ui->NewFileDialog(L"SSWR", L"AVIRead", L"DBManagerExportPList", true);
		dlg->AddFilter(CSTR("*.plist"), CSTR("PList File"));
		dlg->SetFileName(CSTRP(sbuff, sptr));
		if (dlg->ShowDialog(this->GetHandle()))
		{
			IO::FileStream fs(dlg->GetFileName(), IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
			if (!DB::DBExporter::GeneratePList(db, OPTSTR_CSTR(schemaName), tableName->ToCString(), this->currCond, fs, 65001))
			{
				this->ui->ShowMsgOK(CSTR("Error in exporting as PList"), CSTR("DB Manager"), this);
			}
		}
		dlg.Delete();
		tableName->Release();
	}
	OPTSTR_DEL(schemaName);
}

void SSWR::AVIRead::AVIRDBManagerForm::ExportTableXLSX()
{
	UTF8Char sbuff[512];
	UnsafeArray<UTF8Char> sptr;
	NN<DB::ReadingDB> db;
	if (!this->currDB.SetTo(db))
	{
		this->ui->ShowMsgOK(CSTR("Please select database first"), CSTR("DB Manager"), this);
		return;
	}
	Optional<Text::String> schemaName = this->lbSchema->GetSelectedItemTextNew();
	NN<Text::String> tableName;
	NN<Text::String> s;
	if (this->lbTable->GetSelectedItemTextNew().SetTo(tableName))
	{
		sptr = sbuff;
		if (schemaName.SetTo(s) && s->leng > 0)
		{
			sptr = s->ConcatTo(sptr);
			*sptr++ = '_';
		}
		sptr = tableName->ConcatTo(sptr);
		*sptr++ = '_';
		sptr = Data::Timestamp::Now().ToString(sptr, "yyyyMMdd_HHmmss");
		sptr = Text::StrConcatC(sptr, UTF8STRC(".xlsx"));
		NN<UI::GUIFileDialog> dlg = this->ui->NewFileDialog(L"SSWR", L"AVIRead", L"DBManagerExportXLSX", true);
		dlg->AddFilter(CSTR("*.xlsx"), CSTR("Excel 2007 File"));
		dlg->SetFileName(CSTRP(sbuff, sptr));
		if (dlg->ShowDialog(this->GetHandle()))
		{
			Text::StringBuilderUTF8 sb;
			IO::FileStream fs(dlg->GetFileName(), IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
			if (!DB::DBExporter::GenerateXLSX(db, OPTSTR_CSTR(schemaName), tableName->ToCString(), this->currCond, fs, &sb))
			{
				this->ui->ShowMsgOK(sb.ToCString(), CSTR("DB Manager"), this);
			}
		}
		dlg.Delete();
		tableName->Release();
	}
	OPTSTR_DEL(schemaName);
}

void SSWR::AVIRead::AVIRDBManagerForm::ExportTableExcelXML()
{
	UTF8Char sbuff[512];
	UnsafeArray<UTF8Char> sptr;
	NN<DB::ReadingDB> db;
	if (!this->currDB.SetTo(db))
	{
		this->ui->ShowMsgOK(CSTR("Please select database first"), CSTR("DB Manager"), this);
		return;
	}
	Optional<Text::String> schemaName = this->lbSchema->GetSelectedItemTextNew();
	NN<Text::String> tableName;
	NN<Text::String> s;
	if (this->lbTable->GetSelectedItemTextNew().SetTo(tableName))
	{
		sptr = sbuff;
		if (schemaName.SetTo(s) && s->leng > 0)
		{
			sptr = s->ConcatTo(sptr);
			*sptr++ = '_';
		}
		sptr = tableName->ConcatTo(sptr);
		*sptr++ = '_';
		sptr = Data::Timestamp::Now().ToString(sptr, "yyyyMMdd_HHmmss");
		sptr = Text::StrConcatC(sptr, UTF8STRC(".xml"));
		NN<UI::GUIFileDialog> dlg = this->ui->NewFileDialog(L"SSWR", L"AVIRead", L"DBManagerExportXML", true);
		dlg->AddFilter(CSTR("*.xml"), CSTR("Excel XML File"));
		dlg->SetFileName(CSTRP(sbuff, sptr));
		if (dlg->ShowDialog(this->GetHandle()))
		{
			Text::StringBuilderUTF8 sb;
			IO::FileStream fs(dlg->GetFileName(), IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
			if (!DB::DBExporter::GenerateExcelXML(db, OPTSTR_CSTR(schemaName), tableName->ToCString(), this->currCond, fs, &sb))
			{
				this->ui->ShowMsgOK(sb.ToCString(), CSTR("DB Manager"), this);
			}
		}
		dlg.Delete();
		tableName->Release();
	}
	OPTSTR_DEL(schemaName);
}

SSWR::AVIRead::AVIRDBManagerForm::AVIRDBManagerForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core) : UI::GUIForm(parent, 1024, 768, ui)
{
	this->SetFont(0, 0, 8.25, false);
	this->SetText(CSTR("Database Manager"));
	this->core = core;
	this->ssl = Net::SSLEngineFactory::Create(core->GetSocketFactory(), true);
	this->currDB = 0;
	this->currCond = 0;
	this->sqlFileMode = false;
	NEW_CLASSNN(this->mapEnv, Map::MapEnv(CSTR("DB"), 0xffc0c0ff, Math::CoordinateSystemManager::CreateWGS84Csys()));
	NN<Map::MapDrawLayer> layer;
	if (Map::BaseMapLayer::CreateLayer(Map::BaseMapLayer::BLT_OSM_TILE, this->core->GetSocketFactory(), this->ssl, this->core->GetParserList()).SetTo(layer))
	{
		this->mapEnv->AddLayer(0, layer, true);
		layer->AddUpdatedHandler(OnLayerUpdated, this);
	}
	NEW_CLASSNN(this->dbLayer, Map::DBMapLayer(CSTR("Database")));
	UOSInt i = this->mapEnv->AddLayer(0, this->dbLayer, true);
	Map::MapEnv::LayerItem settings;
	this->mapEnv->GetLayerProp(settings, 0, i);
	settings.fillStyle = 0xc0ff905b;
	this->mapEnv->SetLayerProp(settings, 0, i);
	this->colorSess = core->GetColorMgr()->CreateSess(this->GetHMonitor());
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	this->lbConn = ui->NewListBox(*this, false);
	this->lbConn->SetRect(0, 0, 150, 23, false);
	this->lbConn->SetDockType(UI::GUIControl::DOCK_LEFT);
	this->lbConn->HandleSelectionChange(OnConnSelChg, this);
	this->lbConn->HandleRightClicked(OnConnRClicked, this);
	this->lbConn->HandleDoubleClicked(OnConnDblClicked, this);
	this->hspConn = ui->NewHSplitter(*this, 3, false);
	this->tcMain = ui->NewTabControl(*this);
	this->tcMain->SetDockType(UI::GUIControl::DOCK_FILL);

	this->tpDatabase = this->tcMain->AddTabPage(CSTR("Database"));
	this->lbDatabase = ui->NewListBox(this->tpDatabase, false);
	this->lbDatabase->SetRect(0, 0, 150, 23, false);
	this->lbDatabase->SetDockType(UI::GUIControl::DOCK_LEFT);
	this->btnDatabaseChange = ui->NewButton(this->tpDatabase, CSTR("Change"));
	this->btnDatabaseChange->SetRect(154, 4, 75, 23, false);
	this->btnDatabaseChange->HandleButtonClick(OnDatabaseChangeClicked, this);
	this->btnDatabaseDelete = ui->NewButton(this->tpDatabase, CSTR("Delete"));
	this->btnDatabaseDelete->SetRect(154, 28, 75, 23, false);
	this->btnDatabaseDelete->HandleButtonClick(OnDatabaseDeleteClicked, this);
	this->btnDatabaseNew = ui->NewButton(this->tpDatabase, CSTR("New"));
	this->btnDatabaseNew->SetRect(154, 52, 75, 23, false);
	this->btnDatabaseNew->HandleButtonClick(OnDatabaseNewClicked, this);

	this->tpTable = this->tcMain->AddTabPage(CSTR("Table"));
	this->pnlTable = ui->NewPanel(this->tpTable);
	this->pnlTable->SetRect(0, 0, 100, 250, false);
	this->pnlTable->SetDockType(UI::GUIControl::DOCK_TOP);
	this->lbSchema = ui->NewListBox(this->pnlTable, false);
	this->lbSchema->SetRect(0, 0, 150, 100, false);
	this->lbSchema->SetDockType(UI::GUIControl::DOCK_LEFT);
	this->lbSchema->HandleSelectionChange(OnSchemaSelChg, this);
	this->lbSchema->HandleRightClicked(OnSchemaRClicked, this);
	this->hspSchema = ui->NewHSplitter(this->pnlTable, 3, false);
	this->lbTable = ui->NewListBox(this->pnlTable, false);
	this->lbTable->SetRect(0, 0, 150, 23, false);
	this->lbTable->SetDockType(UI::GUIControl::DOCK_LEFT);
	this->lbTable->HandleSelectionChange(OnTableSelChg, this);
	this->lbTable->HandleRightClicked(OnTableRClicked, this);
	this->hspTable = ui->NewHSplitter(this->pnlTable, 3, false);
	this->lvTable = ui->NewListView(this->pnlTable, UI::ListViewStyle::Table, 8);
	this->lvTable->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lvTable->SetFullRowSelect(true);
	this->lvTable->SetShowGrid(true);
	this->lvTable->AddColumn(CSTR("Name"), 200);
	this->lvTable->AddColumn(CSTR("Type"), 100);
	this->lvTable->AddColumn(CSTR("NType"), 100);
	this->lvTable->AddColumn(CSTR("Null?"), 100);
	this->lvTable->AddColumn(CSTR("PK?"), 30);
	this->lvTable->AddColumn(CSTR("Auto_Inc"), 100);
	this->lvTable->AddColumn(CSTR("Default Val"), 100);
	this->lvTable->AddColumn(CSTR("Attribute"), 100);
	this->vspTable = ui->NewVSplitter(this->tpTable, 3, false);
	this->lvTableResult = ui->NewListView(this->tpTable, UI::ListViewStyle::Table, 1);
	this->lvTableResult->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lvTableResult->SetFullRowSelect(true);
	this->lvTableResult->SetShowGrid(true);

	this->tpSQL = this->tcMain->AddTabPage(CSTR("SQL"));
	this->pnlSQL = ui->NewPanel(this->tpSQL);
	this->pnlSQL->SetRect(0, 0, 100, 152, false);
	this->pnlSQL->SetDockType(UI::GUIControl::DOCK_TOP);
	this->pnlSQLCtrl = ui->NewPanel(this->pnlSQL);
	this->pnlSQLCtrl->SetRect(0, 0, 100, 31, false);
	this->pnlSQLCtrl->SetDockType(UI::GUIControl::DOCK_BOTTOM);
	this->btnSQLExec = ui->NewButton(this->pnlSQLCtrl, CSTR("Execute"));
	this->btnSQLExec->SetRect(4, 4, 75, 23, false);
	this->btnSQLExec->HandleButtonClick(OnSQLExecClicked, this);
	this->btnSQLFile = ui->NewButton(this->pnlSQLCtrl, CSTR("SQL File"));
	this->btnSQLFile->SetRect(84, 4, 75, 23, false);
	this->btnSQLFile->HandleButtonClick(OnSQLFileClicked, this);
	this->txtSQL = ui->NewTextBox(this->pnlSQL, CSTR(""), true);
	this->txtSQL->SetDockType(UI::GUIControl::DOCK_FILL);
	this->vspSQL = ui->NewVSplitter(this->tpSQL, 3, false);
	this->lvSQLResult = ui->NewListView(this->tpSQL, UI::ListViewStyle::Table, 1);
	this->lvSQLResult->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lvSQLResult->SetFullRowSelect(true);
	this->lvSQLResult->SetShowGrid(true);

	this->tpMap = this->tcMain->AddTabPage(CSTR("Map"));
	this->pnlMap = ui->NewPanel(this->tpMap);
	this->pnlMap->SetRect(0, 0, 250, 23, false);
	this->pnlMap->SetDockType(UI::GUIControl::DOCK_LEFT);
	this->hspMap = ui->NewHSplitter(this->tpMap, 3, false);
	NEW_CLASS(this->mapMain, UI::GUIMapControl(ui, this->tpMap, this->core->GetDrawEngine(), this->mapEnv, this->colorSess));
	this->mapMain->SetDockType(UI::GUIControl::DOCK_FILL);
	this->mapMain->HandleMouseDown(OnMapMouseDown, this);
	this->mapMain->HandleMouseUp(OnMapMouseUp, this);
	this->pnlMapTable = ui->NewPanel(this->pnlMap);
	this->pnlMapTable->SetRect(0, 0, 100, 100, false);
	this->pnlMapTable->SetDockType(UI::GUIControl::DOCK_TOP);
	this->vspMapRecord = ui->NewVSplitter(this->pnlMap, 3, false);
	this->lvMapRecord = ui->NewListView(this->pnlMap, UI::ListViewStyle::Table, 2);
	this->lvMapRecord->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lvMapRecord->SetFullRowSelect(true);
	this->lvMapRecord->SetShowGrid(true);
	this->lvMapRecord->AddColumn(CSTR("Name"), 100);
	this->lvMapRecord->AddColumn(CSTR("Value"), 100);
	this->lbMapSchema = ui->NewListBox(this->pnlMapTable, false);
	this->lbMapSchema->SetRect(0, 0, 100, 23, false);
	this->lbMapSchema->SetDockType(UI::GUIControl::DOCK_LEFT);
	this->lbMapSchema->HandleSelectionChange(OnMapSchemaSelChg, this);
	this->hspMapTable = ui->NewHSplitter(this->pnlMapTable, 3, false);
	this->lbMapTable = ui->NewListBox(this->pnlMapTable, false);
	this->lbMapTable->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lbMapTable->HandleSelectionChange(OnMapTableSelChg, this);

	this->tpVariable = this->tcMain->AddTabPage(CSTR("Variable"));
	this->pnlVariable = ui->NewPanel(this->tpVariable);
	this->pnlVariable->SetRect(0, 0, 100, 31, false);
	this->pnlVariable->SetDockType(UI::GUIControl::DOCK_TOP);
	this->btnVariable = ui->NewButton(this->pnlVariable, CSTR("Show"));
	this->btnVariable->SetRect(4, 4, 75, 23, false);
	this->btnVariable->HandleButtonClick(OnVariableClicked, this);
	this->lvVariable = ui->NewListView(this->tpVariable, UI::ListViewStyle::Table, 2);
	this->lvVariable->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lvVariable->SetShowGrid(true);
	this->lvVariable->SetFullRowSelect(true);
	this->lvVariable->AddColumn(CSTR("Name"), 150);
	this->lvVariable->AddColumn(CSTR("Value"), 300);

	this->tpSvrConn = this->tcMain->AddTabPage(CSTR("Connection"));
	this->pnlSvrConn = ui->NewPanel(this->tpSvrConn);
	this->pnlSvrConn->SetRect(0, 0, 100, 31, false);
	this->pnlSvrConn->SetDockType(UI::GUIControl::DOCK_TOP);
	this->btnSvrConn = ui->NewButton(this->pnlSvrConn, CSTR("Show"));
	this->btnSvrConn->SetRect(4, 4, 75, 23, false);
	this->btnSvrConn->HandleButtonClick(OnSvrConnClicked, this);
	this->btnSvrConnKill = ui->NewButton(this->pnlSvrConn, CSTR("Kill"));
	this->btnSvrConnKill->SetRect(84, 4, 75, 23, false);
	this->btnSvrConnKill->HandleButtonClick(OnSvrConnKillClicked, this);
	this->lvSvrConn = ui->NewListView(this->tpSvrConn, UI::ListViewStyle::Table, 8);
	this->lvSvrConn->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lvSvrConn->SetShowGrid(true);
	this->lvSvrConn->SetFullRowSelect(true);
	this->lvSvrConn->AddColumn(CSTR("Id"), 60);
	this->lvSvrConn->AddColumn(CSTR("Status"), 100);
	this->lvSvrConn->AddColumn(CSTR("User"), 100);
	this->lvSvrConn->AddColumn(CSTR("Host"), 100);
	this->lvSvrConn->AddColumn(CSTR("DB Name"), 100);
	this->lvSvrConn->AddColumn(CSTR("Command"), 100);
	this->lvSvrConn->AddColumn(CSTR("Time"), 60);
	this->lvSvrConn->AddColumn(CSTR("SQL"), 300);

	this->tpLog = this->tcMain->AddTabPage(CSTR("Log"));
	this->txtLog = ui->NewTextBox(this->tpLog, CSTR(""));
	this->txtLog->SetRect(0, 0, 100, 23, false);
	this->txtLog->SetDockType(UI::GUIControl::DOCK_BOTTOM);
	this->lbLog = ui->NewListBox(this->tpLog, false);
	this->lbLog->SetDockType(UI::GUIControl::DOCK_FILL);

	NEW_CLASSNN(this->logger, UI::ListBoxLogger(*this, this->lbLog, 100, true));
	this->log.AddLogHandler(this->logger, IO::LogHandler::LogLevel::Raw);


	NN<UI::GUIMenu> mnu;
	NN<UI::GUIMenu> mnu2;
	NEW_CLASSNN(this->mnuMain, UI::GUIMainMenu());
	mnu = this->mnuMain->AddSubMenu(CSTR("&Connection"));
	mnu2 = mnu->AddSubMenu(CSTR("&New Connection"));
	mnu2->AddItem(CSTR("ODBC DSN"), MNU_CONN_ODBCDSN, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu2->AddItem(CSTR("ODBC Connection String"), MNU_CONN_ODBCSTR, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu2->AddItem(CSTR("MySQL TCP"), MNU_CONN_MYSQL, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu2->AddItem(CSTR("SQL Server"), MNU_CONN_MSSQL, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu2->AddItem(CSTR("Access File"), MNU_CONN_ACCESS, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu2->AddItem(CSTR("PostgreSQL"), MNU_CONN_POSTGRESQL, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu = this->mnuMain->AddSubMenu(CSTR("&Tool"));
	mnu->AddItem(CSTR("Copy Tables"), MNU_TOOL_COPY_TABLES, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	this->SetMenu(this->mnuMain);

	NEW_CLASSNN(this->mnuConn, UI::GUIPopupMenu());
	this->mnuConn->AddItem(CSTR("Remove"), MNU_CONN_REMOVE, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	this->mnuConn->AddItem(CSTR("Copy Conn String"), MNU_CONN_COPY_STR, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);

	NEW_CLASSNN(this->mnuSchema, UI::GUIPopupMenu());
	this->mnuSchema->AddItem(CSTR("New"), MNU_SCHEMA_NEW, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	this->mnuSchema->AddItem(CSTR("Delete"), MNU_SCHEMA_DELETE, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);

	NEW_CLASSNN(this->mnuTable, UI::GUIPopupMenu());
	this->mnuTable->AddItem(CSTR("Copy as Java Entity"), MNU_TABLE_JAVA, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	this->mnuTable->AddItem(CSTR("Copy as C++ Header"), MNU_TABLE_CPP_HEADER, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	this->mnuTable->AddItem(CSTR("Copy as C++ Source"), MNU_TABLE_CPP_SOURCE, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu = this->mnuTable->AddSubMenu(CSTR("Copy as Create SQL"));
	mnu->AddItem(CSTR("MySQL"), MNU_TABLE_CREATE_MYSQL, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu->AddItem(CSTR("MySQL >=8"), MNU_TABLE_CREATE_MYSQL8, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu->AddItem(CSTR("SQL Server"), MNU_TABLE_CREATE_MSSQL, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu->AddItem(CSTR("PostgreSQL"), MNU_TABLE_CREATE_POSTGRESQL, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu = this->mnuTable->AddSubMenu(CSTR("Export Table Data as"));
	mnu->AddItem(CSTR("SQL (MySQL)"), MNU_TABLE_EXPORT_MYSQL, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu->AddItem(CSTR("SQL (MySQL >=8)"), MNU_TABLE_EXPORT_MYSQL8, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu->AddItem(CSTR("SQL (SQL Server)"), MNU_TABLE_EXPORT_MSSQL, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu->AddItem(CSTR("SQL (PostgreSQL)"), MNU_TABLE_EXPORT_POSTGRESQL, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu->AddItem(CSTR("SQL..."), MNU_TABLE_EXPORT_OPTION, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu->AddItem(CSTR("CSV"), MNU_TABLE_EXPORT_CSV, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu->AddItem(CSTR("SQLite"), MNU_TABLE_EXPORT_SQLITE, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu->AddItem(CSTR("HTML"), MNU_TABLE_EXPORT_HTML, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu->AddItem(CSTR("PList"), MNU_TABLE_EXPORT_PLIST, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu->AddItem(CSTR("Excel 2007"), MNU_TABLE_EXPORT_XLSX, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu->AddItem(CSTR("Excel XML"), MNU_TABLE_EXPORT_EXCELXML, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	this->mnuTable->AddItem(CSTR("Check Table Changes"), MNU_TABLE_CHECK_CHANGE, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);

	UTF8Char sbuff[512];
	UnsafeArray<UTF8Char> sptr;
	sptr = IO::Path::GetProcessFileName(sbuff).Or(sbuff);
	sptr = IO::Path::AppendPath(sbuff, sptr, DBCONNFILE);
	if (DB::DBManager::RestoreConn(CSTRP(sbuff, sptr), this->dbList, this->log, this->core->GetSocketFactory(), this->core->GetParserList()))
	{
		Text::StringBuilderUTF8 sb;
		NN<DB::DBManagerCtrl> ctrl;
		UOSInt i = 0;
		UOSInt j = this->dbList.GetCount();
		while (i < j)
		{
			ctrl = this->dbList.GetItemNoCheck(i);
			sb.ClearStr();
			ctrl->GetConnName(sb);
			this->lbConn->AddItem(sb.ToCString(), ctrl);

			i++;
		}
		if (j > 0)
		{
			this->lbConn->SetSelectedIndex(0);
		}
	}
	this->HandleDropFiles(OnFileHandler, this);
}

SSWR::AVIRead::AVIRDBManagerForm::~AVIRDBManagerForm()
{
	this->ClearChildren();
	this->log.RemoveLogHandler(this->logger);
	this->logger.Delete();
	this->mnuTable.Delete();
	this->mnuSchema.Delete();
	this->mnuConn.Delete();
	this->mapEnv.Delete();
	this->core->GetColorMgr()->DeleteSess(this->colorSess);
	UTF8Char sbuff[512];
	UnsafeArray<UTF8Char> sptr;
	sptr = IO::Path::GetProcessFileName(sbuff).Or(sbuff);
	sptr = IO::Path::AppendPath(sbuff, sptr, DBCONNFILE);
	DB::DBManager::StoreConn(CSTRP(sbuff, sptr), this->dbList);
	UOSInt i = this->dbList.GetCount();
	NN<DB::DBManagerCtrl> ctrl;
	while (i-- > 0)
	{
		ctrl = this->dbList.GetItemNoCheck(i);
		ctrl.Delete();
	}
	SDEL_CLASS(this->currCond);
	this->ssl.Delete();
}

void SSWR::AVIRead::AVIRDBManagerForm::EventMenuClicked(UInt16 cmdId)
{
	UTF8Char sbuff[512];
	UTF8Char sbuff2[512];
	UnsafeArray<UTF8Char> sptr;
	NN<DB::DBConn> conn;
	NN<DB::ReadingDB> db;
	switch (cmdId)
	{
	case MNU_CONN_ODBCDSN:
		{
			SSWR::AVIRead::AVIRODBCDSNForm dlg(0, this->ui, this->core);
			if (dlg.ShowDialog(this) == UI::GUIForm::DR_OK)
			{
				if (conn.Set(dlg.GetDBConn()))
					this->ConnAdd(conn);
			}
		}
		break;
	case MNU_CONN_ODBCSTR:
		{
			SSWR::AVIRead::AVIRODBCStrForm dlg(0, this->ui, this->core);
			if (dlg.ShowDialog(this) == UI::GUIForm::DR_OK)
			{
				if (conn.Set(dlg.GetDBConn()))
					this->ConnAdd(conn);
			}
		}
		break;
	case MNU_CONN_MYSQL:
		{
			SSWR::AVIRead::AVIRMySQLConnForm dlg(0, this->ui, this->core);
			if (dlg.ShowDialog(this) == UI::GUIForm::DR_OK)
			{
				if (conn.Set(dlg.GetDBConn()))
					this->ConnAdd(conn);
			}
		}
		break;
	case MNU_CONN_MSSQL:
		{
			SSWR::AVIRead::AVIRMSSQLConnForm dlg(0, this->ui, this->core);
			if (dlg.ShowDialog(this) == UI::GUIForm::DR_OK)
			{
				if (dlg.GetDBConn().SetTo(conn))
					this->ConnAdd(conn);
			}
		}
		break;
	case MNU_CONN_ACCESS:
		{
			SSWR::AVIRead::AVIRAccessConnForm dlg(0, this->ui, this->core);
			if (dlg.ShowDialog(this) == UI::GUIForm::DR_OK)
			{
				if (conn.Set(dlg.GetDBConn()))
					this->ConnAdd(conn);
			}
		}
		break;
	case MNU_CONN_POSTGRESQL:
		{
			SSWR::AVIRead::AVIRPostgreSQLForm dlg(0, this->ui, this->core);
			if (dlg.ShowDialog(this) == UI::GUIForm::DR_OK)
			{
				if (conn.Set(dlg.GetDBConn()))
					this->ConnAdd(conn);
			}
		}
		break;
	case MNU_TOOL_COPY_TABLES:
		{
			SSWR::AVIRead::AVIRDBCopyTablesForm dlg(0, this->ui, this->core, this->dbList);
			UOSInt i = this->lbConn->GetSelectedIndex();
			if (i != INVALID_INDEX)
			{
				dlg.SetSourceDB(i);
			}
			dlg.ShowDialog(this);
		}
		break;
	case MNU_CONN_REMOVE:
		{
			UOSInt i = this->lbConn->GetSelectedIndex();
			if (i != INVALID_INDEX)
			{
				NN<DB::DBManagerCtrl> ctrl;
				if (this->dbList.RemoveAt(i).SetTo(ctrl))
				{
					ctrl.Delete();
				}
				this->lbConn->RemoveItem(i);
			}
		}
		break;
	case MNU_CONN_COPY_STR:
		{
			Text::StringBuilderUTF8 sb;
			NN<DB::DBManagerCtrl> ctrl;
			if (this->lbConn->GetSelectedItem().GetOpt<DB::DBManagerCtrl>().SetTo(ctrl))
			{
				NN<Text::String> connStr;
				if (ctrl->GetConnStr().SetTo(connStr))
				{
					UI::Clipboard::SetString(this->GetHandle(), connStr->ToCString());
				}
				else
				{
					this->ui->ShowMsgOK(CSTR("This connection is not supported"), CSTR("DB Manager"), this);
				}
			}
		}
		break;
	case MNU_SCHEMA_NEW:
		if (this->currDB.SetTo(db) && db->IsDBTool() && NN<DB::ReadingDBTool>::ConvertFrom(db)->CanModify())
		{
			UtilUI::TextInputDialog dlg(0, this->ui, this->core->GetMonitorMgr(), CSTR("DB Manager"), CSTR("Please enter schema name to create"));
			if (dlg.ShowDialog(this))
			{
				Text::StringBuilderUTF8 sb;
				if (dlg.GetInputString(sb))
				{
					if (NN<DB::DBTool>::ConvertFrom(db)->CreateSchema(sb.ToCString()))
					{
						this->UpdateSchemaList();
					}
					else
					{
						sb.ClearStr();
						sb.AppendC(UTF8STRC("Error in creating schema: "));
						db->GetLastErrorMsg(sb);
						this->ui->ShowMsgOK(sb.ToCString(), CSTR("DB Manager"), this);
					}
				}
			}
		}
		break;
	case MNU_SCHEMA_DELETE:
		if (this->currDB.SetTo(db) && db->IsDBTool() && NN<DB::ReadingDBTool>::ConvertFrom(db)->CanModify())
		{
			NN<Text::String> schemaName;
			if (this->lbSchema->GetSelectedItemTextNew().SetTo(schemaName))
			{
				if (schemaName->leng > 0)
				{
					Text::StringBuilderUTF8 sb;
					sb.AppendC(UTF8STRC("Are you sure to delete schema "));
					sb.Append(schemaName);
					sb.AppendC(UTF8STRC("? This process cannot be undo."));
					if (this->ui->ShowMsgYesNo(sb.ToCString(), CSTR("DB Manager"), this))
					{
						if (NN<DB::DBTool>::ConvertFrom(db)->DeleteSchema(schemaName->ToCString()))
						{
							this->UpdateSchemaList();
						}
						else
						{
							sb.ClearStr();
							sb.AppendC(UTF8STRC("Error in deleting schema "));
							sb.Append(schemaName);
							sb.AppendC(UTF8STRC(": "));
							db->GetLastErrorMsg(sb);
							this->ui->ShowMsgOK(sb.ToCString(), CSTR("DB Manager"), this);
						}
					}
				}
				schemaName->Release();
			}
		}
		break;
	case MNU_TABLE_JAVA:
		{
			Optional<Text::String> schemaName = this->lbSchema->GetSelectedItemTextNew();
			Optional<Text::String> tableName = this->lbTable->GetSelectedItemTextNew();
			Text::String *databaseName = 0;
			NN<Text::String> nntableName;
			if (!tableName.SetTo(nntableName))
			{
				this->ui->ShowMsgOK(CSTR("Select table first"), CSTR("DB Manager"), this);
			}
			else if (this->currDB.SetTo(db))
			{
				databaseName = db->GetCurrDBName();
				Text::StringBuilderUTF8 sb;
				DB::JavaDBUtil::ToJavaEntity(sb, schemaName, nntableName, databaseName, db);
				UI::Clipboard::SetString(this->GetHandle(), sb.ToCString());
			}
			else
			{
				this->ui->ShowMsgOK(CSTR("Select database first"), CSTR("DB Manager"), this);
			}
			OPTSTR_DEL(tableName);
			OPTSTR_DEL(schemaName);
		}
		break;
	case MNU_TABLE_CPP_HEADER:
		if (this->lbTable->GetSelectedItemText(sbuff).SetTo(sptr))
		{
			Optional<Text::String> schemaName = this->lbSchema->GetSelectedItemTextNew();
			NN<Data::Class> cls;
			if (this->CreateTableClass(OPTSTR_CSTR(schemaName), CSTRP(sbuff, sptr)).SetTo(cls))
			{
				Text::PString hdr = {sbuff2, 0};
				UOSInt i = Text::StrLastIndexOfCharC(sbuff, (UOSInt)(sptr - sbuff), '.');
				hdr.leng = (UOSInt)(DB::DBUtil::DB2FieldName(sbuff2, &sbuff[i + 1]) - sbuff2);
				sbuff2[0] = Text::CharUtil::ToUpper(sbuff2[0]);
				Text::StringBuilderUTF8 sb;
				cls->ToCppClassHeader(&hdr, 0, sb);
				UI::Clipboard::SetString(this->GetHandle(), sb.ToCString());
				cls.Delete();
			}
			OPTSTR_DEL(schemaName);
		}
		break;
	case MNU_TABLE_CPP_SOURCE:
		if (this->lbTable->GetSelectedItemText(sbuff).SetTo(sptr))
		{
			Optional<Text::String> schemaName = this->lbSchema->GetSelectedItemTextNew();
			NN<Data::Class> cls;
			if (this->CreateTableClass(OPTSTR_CSTR(schemaName), CSTRP(sbuff, sptr)).SetTo(cls))
			{
				Text::PString hdr = {sbuff2, 0};
				UOSInt i = Text::StrLastIndexOfCharC(sbuff, (UOSInt)(sptr - sbuff), '.');
				hdr.leng = (UOSInt)(DB::DBUtil::DB2FieldName(sbuff2, &sbuff[i + 1]) - sbuff2);
				sbuff2[0] = Text::CharUtil::ToUpper(sbuff2[0]);
				Text::StringBuilderUTF8 sb;
				cls->ToCppClassSource(0, &hdr, 0, sb);
				UI::Clipboard::SetString(this->GetHandle(), sb.ToCString());
				cls.Delete();
			}
			OPTSTR_DEL(schemaName);
		}
		break;
	case MNU_TABLE_CREATE_MYSQL:
		this->CopyTableCreate(DB::SQLType::MySQL, false);
		break;
	case MNU_TABLE_CREATE_MYSQL8:
		this->CopyTableCreate(DB::SQLType::MySQL, true);
		break;
	case MNU_TABLE_CREATE_MSSQL:
		this->CopyTableCreate(DB::SQLType::MSSQL, false);
		break;
	case MNU_TABLE_CREATE_POSTGRESQL:
		this->CopyTableCreate(DB::SQLType::PostgreSQL, false);
		break;
	case MNU_TABLE_EXPORT_MYSQL:
		this->ExportTableData(DB::SQLType::MySQL, false);
		break;
	case MNU_TABLE_EXPORT_MYSQL8:
		this->ExportTableData(DB::SQLType::MySQL, true);
		break;
	case MNU_TABLE_EXPORT_MSSQL:
		this->ExportTableData(DB::SQLType::MSSQL, false);
		break;
	case MNU_TABLE_EXPORT_POSTGRESQL:
		this->ExportTableData(DB::SQLType::PostgreSQL, false);
		break;
	case MNU_TABLE_EXPORT_OPTION:
		{
			Optional<Text::String> schemaName = this->lbSchema->GetSelectedItemTextNew();
			NN<Text::String> tableName;
			if (this->currDB.SetTo(db) && this->lbTable->GetSelectedItemTextNew().SetTo(tableName))
			{
				SSWR::AVIRead::AVIRDBExportForm dlg(0, ui, this->core, db, OPTSTR_CSTR(schemaName), tableName->ToCString());
				dlg.ShowDialog(this);
				tableName->Release();
			}
			OPTSTR_DEL(schemaName);
		}
		break;
	case MNU_TABLE_EXPORT_CSV:
		this->ExportTableCSV();
		break;
	case MNU_TABLE_EXPORT_SQLITE:
		this->ExportTableSQLite();
		break;
	case MNU_TABLE_CHECK_CHANGE:
		{
			Optional<Text::String> schemaName = this->lbSchema->GetSelectedItemTextNew();
			NN<Text::String> tableName;
			if (this->currDB.SetTo(db) && this->lbTable->GetSelectedItemTextNew().SetTo(tableName))
			{
				SSWR::AVIRead::AVIRDBCheckChgForm dlg(0, ui, this->core, db, OPTSTR_CSTR(schemaName), tableName->ToCString());
				dlg.ShowDialog(this);
				tableName->Release();
			}
			OPTSTR_DEL(schemaName);
		}
		break;
	}
}

void SSWR::AVIRead::AVIRDBManagerForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}

void SSWR::AVIRead::AVIRDBManagerForm::ConnAdd(NN<DB::DBConn> conn)
{
	NN<DB::DBTool> db;
	NEW_CLASSNN(db, DB::DBTool(conn, true, this->log, CSTR("DB: ")));
	NN<DB::DBManagerCtrl> ctrl = DB::DBManagerCtrl::Create(db, this->log, this->core->GetSocketFactory(), this->core->GetParserList());
	this->dbList.Add(ctrl);
	Text::StringBuilderUTF8 sb;
	conn->GetConnName(sb);
	this->lbConn->AddItem(sb.ToCString(), ctrl);
}
