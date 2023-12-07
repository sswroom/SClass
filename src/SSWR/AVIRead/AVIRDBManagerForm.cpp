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
#include "UI/FileDialog.h"
#include "UI/MessageDialog.h"
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

void __stdcall SSWR::AVIRead::AVIRDBManagerForm::OnConnSelChg(void *userObj)
{
	SSWR::AVIRead::AVIRDBManagerForm *me = (SSWR::AVIRead::AVIRDBManagerForm*)userObj;
	DB::DBManagerCtrl *ctrl = (DB::DBManagerCtrl*)me->lbConn->GetSelectedItem();
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

Bool __stdcall SSWR::AVIRead::AVIRDBManagerForm::OnConnRClicked(void *userObj, Math::Coord2D<OSInt> scnPos, MouseButton btn)
{
	SSWR::AVIRead::AVIRDBManagerForm *me = (SSWR::AVIRead::AVIRDBManagerForm*)userObj;
	UOSInt i = me->lbConn->GetSelectedIndex();
	if (i != INVALID_INDEX)
	{
		me->mnuConn->ShowMenu(me->lbConn, scnPos);
	}
	return false;
}

void __stdcall SSWR::AVIRead::AVIRDBManagerForm::OnConnDblClicked(void *userObj)
{
	SSWR::AVIRead::AVIRDBManagerForm *me = (SSWR::AVIRead::AVIRDBManagerForm*)userObj;
	DB::DBManagerCtrl *ctrl = (DB::DBManagerCtrl*)me->lbConn->GetSelectedItem();
	if (ctrl)
	{
		if (ctrl->Connect())
		{
			OnConnSelChg(me);
		}
		else
		{
			UI::MessageDialog::ShowDialog(CSTR("Error in connecting to database"), CSTR("DB Manager"), me);
		}
	}
}

void __stdcall SSWR::AVIRead::AVIRDBManagerForm::OnSchemaSelChg(void *userObj)
{
	SSWR::AVIRead::AVIRDBManagerForm *me = (SSWR::AVIRead::AVIRDBManagerForm*)userObj;
	me->UpdateTableList();
}

Bool __stdcall SSWR::AVIRead::AVIRDBManagerForm::OnSchemaRClicked(void *userObj, Math::Coord2D<OSInt> scnPos, MouseButton btn)
{
	SSWR::AVIRead::AVIRDBManagerForm *me = (SSWR::AVIRead::AVIRDBManagerForm*)userObj;
	UOSInt i = me->lbSchema->GetSelectedIndex();
	if (i != INVALID_INDEX)
	{
		me->mnuSchema->ShowMenu(me->lbSchema, scnPos);
	}
	return false;
}

void __stdcall SSWR::AVIRead::AVIRDBManagerForm::OnTableSelChg(void *userObj)
{
	SSWR::AVIRead::AVIRDBManagerForm *me = (SSWR::AVIRead::AVIRDBManagerForm*)userObj;
	Text::String *tableName = me->lbTable->GetSelectedItemTextNew();
	Text::String *schemaName = me->lbSchema->GetSelectedItemTextNew();
	me->UpdateTableData(STR_CSTR(schemaName), tableName);
	SDEL_STRING(schemaName);
	SDEL_STRING(tableName);
}

Bool __stdcall SSWR::AVIRead::AVIRDBManagerForm::OnTableRClicked(void *userObj, Math::Coord2D<OSInt> scnPos, MouseButton btn)
{
	SSWR::AVIRead::AVIRDBManagerForm *me = (SSWR::AVIRead::AVIRDBManagerForm*)userObj;
	UOSInt i = me->lbTable->GetSelectedIndex();
	if (i != INVALID_INDEX)
	{
		me->mnuTable->ShowMenu(me->lbTable, scnPos);
	}
	return false;
}

void __stdcall SSWR::AVIRead::AVIRDBManagerForm::OnMapSchemaSelChg(void *userObj)
{
	SSWR::AVIRead::AVIRDBManagerForm *me = (SSWR::AVIRead::AVIRDBManagerForm*)userObj;
	me->UpdateMapTableList();
}

void __stdcall SSWR::AVIRead::AVIRDBManagerForm::OnMapTableSelChg(void *userObj)
{
	SSWR::AVIRead::AVIRDBManagerForm *me = (SSWR::AVIRead::AVIRDBManagerForm*)userObj;
	if (me->currDB == 0)
	{
		return;
	}
	Text::String *schemaName = me->lbMapSchema->GetSelectedItemTextNew();
	Text::String *tableName = me->lbMapTable->GetSelectedItemTextNew();
	if (schemaName && tableName)
	{
		me->dbLayer->SetDatabase(me->currDB, schemaName->ToCString(), tableName->ToCString(), false);
		Math::RectAreaDbl rect;
		if (me->dbLayer->GetBounds(rect))
		{
			Math::Coord2DDbl center = rect.GetCenter();
			if (center.x != 0 || center.y != 0)
			{
				NotNullPtr<Math::CoordinateSystem> csysLayer = me->dbLayer->GetCoordinateSystem();
				NotNullPtr<Math::CoordinateSystem> csysEnv = me->mapEnv->GetCoordinateSystem();
				if (!csysLayer->Equals(csysEnv))
				{
					center = Math::CoordinateSystem::Convert(csysLayer, csysEnv, center);
				}
				me->mapMain->PanToMapXY(center);
			}
		}
		OnLayerUpdated(me);
	}
	SDEL_STRING(schemaName);
	SDEL_STRING(tableName);
}

Bool __stdcall SSWR::AVIRead::AVIRDBManagerForm::OnMapMouseDown(void *userObj, Math::Coord2D<OSInt> scnPos, MouseButton button)
{
	SSWR::AVIRead::AVIRDBManagerForm *me = (SSWR::AVIRead::AVIRDBManagerForm*)userObj;
	if (button == MBTN_LEFT)
	{
		me->mapDownPos = scnPos;
	}
	return false;
}

Bool __stdcall SSWR::AVIRead::AVIRDBManagerForm::OnMapMouseUp(void *userObj, Math::Coord2D<OSInt> scnPos, MouseButton button)
{
	SSWR::AVIRead::AVIRDBManagerForm *me = (SSWR::AVIRead::AVIRDBManagerForm*)userObj;
	if (button == MBTN_LEFT)
	{
		if (me->mapDownPos == scnPos)
		{
			Map::GetObjectSess *sess;
			Int64 id;
			UOSInt i;
			UOSInt j;
			UTF8Char sbuff[512];
			UTF8Char *sptr;
			Math::Coord2DDbl mapPt = me->mapMain->ScnXY2MapXY(scnPos);
			NotNullPtr<Math::CoordinateSystem> csys = me->mapEnv->GetCoordinateSystem();
			NotNullPtr<Math::CoordinateSystem> lyrCSys = me->dbLayer->GetCoordinateSystem();
			if (!csys->Equals(lyrCSys))
			{
				mapPt = Math::CoordinateSystem::ConvertXYZ(csys, lyrCSys, Math::Vector3(mapPt, 0)).GetXY();
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
					sptr = me->dbLayer->GetColumnName(sbuff, i);
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

void __stdcall SSWR::AVIRead::AVIRDBManagerForm::OnDatabaseChangeClicked(void *userObj)
{
	SSWR::AVIRead::AVIRDBManagerForm *me = (SSWR::AVIRead::AVIRDBManagerForm*)userObj;
	if (me->currDB)
	{
		Text::String *dbName = me->lbDatabase->GetSelectedItemTextNew();
		if (dbName)
		{
			if (me->currDB->ChangeDatabase(dbName->ToCString()))
			{
				me->UpdateTableData(CSTR_NULL, 0);
				me->UpdateSchemaList();
			}
			dbName->Release();
		}
	}
}

void __stdcall SSWR::AVIRead::AVIRDBManagerForm::OnDatabaseDeleteClicked(void *userObj)
{
	SSWR::AVIRead::AVIRDBManagerForm *me = (SSWR::AVIRead::AVIRDBManagerForm*)userObj;
	if (me->currDB)
	{
		if (me->currDB->IsDBTool() && ((DB::ReadingDBTool*)me->currDB)->CanModify())
		{
			Text::String *dbName = me->lbDatabase->GetSelectedItemTextNew();
			if (dbName)
			{
				Text::StringBuilderUTF8 sb;
				sb.AppendC(UTF8STRC("Are you sure to delete database "));
				sb.Append(dbName);
				sb.AppendC(UTF8STRC("? This process cannot be undo."));
				if (UI::MessageDialog::ShowYesNoDialog(sb.ToCString(), CSTR("DB Manager"), me))
				{
					if (((DB::DBTool*)me->currDB)->DeleteDatabase(dbName->ToCString()))
					{
						me->UpdateDatabaseList();
					}
					else
					{
						sb.ClearStr();
						sb.AppendC(UTF8STRC("Error in deleting database "));
						sb.Append(dbName);
						sb.AppendC(UTF8STRC(": "));
						me->currDB->GetLastErrorMsg(sb);
						UI::MessageDialog::ShowDialog(sb.ToCString(), CSTR("DB Manager"), me);
					}
				}
				dbName->Release();
			}
		}
		else
		{
			UI::MessageDialog::ShowDialog(CSTR("Selected database is read-only"), CSTR("DB Manager"), me);
		}
	}
}

void __stdcall SSWR::AVIRead::AVIRDBManagerForm::OnDatabaseNewClicked(void *userObj)
{
	SSWR::AVIRead::AVIRDBManagerForm *me = (SSWR::AVIRead::AVIRDBManagerForm*)userObj;
	if (me->currDB)
	{
		if (me->currDB->IsDBTool() && ((DB::ReadingDBTool*)me->currDB)->CanModify())
		{
			UtilUI::TextInputDialog dlg(0, me->ui, me->core->GetMonitorMgr(), CSTR("DB Manager"), CSTR("Please enter database name to create"));
			if (dlg.ShowDialog(me))
			{
				Text::StringBuilderUTF8 sb;
				if (dlg.GetInputString(sb))
				{
					if (((DB::DBTool*)me->currDB)->CreateDatabase(sb.ToCString(), 0))
					{
						me->UpdateDatabaseList();
					}
					else
					{
						sb.ClearStr();
						sb.AppendC(UTF8STRC("Error in creating database: "));
						me->currDB->GetLastErrorMsg(sb);
						UI::MessageDialog::ShowDialog(sb.ToCString(), CSTR("DB Manager"), me);
					}
				}
			}
		}
		else
		{
			UI::MessageDialog::ShowDialog(CSTR("Selected database is read-only"), CSTR("DB Manager"), me);
		}
	}
}

void __stdcall SSWR::AVIRead::AVIRDBManagerForm::OnSQLExecClicked(void *userObj)
{
	SSWR::AVIRead::AVIRDBManagerForm *me = (SSWR::AVIRead::AVIRDBManagerForm*)userObj;
	if (me->currDB)
	{
		if (me->currDB->IsDBTool())
		{
			Text::StringBuilderUTF8 sb;
			me->txtSQL->GetText(sb);
			if (sb.GetLength() > 0)
			{
				NotNullPtr<DB::DBReader> r;
				if (r.Set(((DB::ReadingDBTool*)me->currDB)->ExecuteReader(sb.ToCString())))
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
					me->currDB->CloseReader(r);
				}
				else
				{
					sb.ClearStr();
					me->currDB->GetLastErrorMsg(sb);
					UI::MessageDialog::ShowDialog(sb.ToCString(), CSTR("DB Manager"), me);
				}
			}
		}
	}
}

void __stdcall SSWR::AVIRead::AVIRDBManagerForm::OnSQLFileClicked(void *userObj)
{
	SSWR::AVIRead::AVIRDBManagerForm *me = (SSWR::AVIRead::AVIRDBManagerForm*)userObj;
	if (me->currDB && me->currDB->IsDBTool())
	{
		DB::ReadingDBTool *db = (DB::ReadingDBTool*)me->currDB;
		{
			UI::FileDialog dlg(L"SSWR", L"AVIRead", L"DBManagerSQLFile", false);
			dlg.SetAllowMultiSel(false);
			dlg.AddFilter(CSTR("*.sql"), CSTR("SQL file"));
			if (dlg.ShowDialog(me->GetHandle()))
			{
				me->RunSQLFile(db, dlg.GetFileName());
			}
			else
			{
				return;
			}
		}
	}
}

void __stdcall SSWR::AVIRead::AVIRDBManagerForm::OnLayerUpdated(void *userObj)
{
	SSWR::AVIRead::AVIRDBManagerForm *me = (SSWR::AVIRead::AVIRDBManagerForm*)userObj;
	me->mapMain->UpdateMap();
	me->mapMain->Redraw();
}

void __stdcall SSWR::AVIRead::AVIRDBManagerForm::OnVariableClicked(void *userObj)
{
	SSWR::AVIRead::AVIRDBManagerForm *me = (SSWR::AVIRead::AVIRDBManagerForm*)userObj;
	me->UpdateVariableList();
}

void __stdcall SSWR::AVIRead::AVIRDBManagerForm::OnSvrConnClicked(void *userObj)
{
	SSWR::AVIRead::AVIRDBManagerForm *me = (SSWR::AVIRead::AVIRDBManagerForm*)userObj;
	me->UpdateSvrConnList();
}

void __stdcall SSWR::AVIRead::AVIRDBManagerForm::OnSvrConnKillClicked(void *userObj)
{
	SSWR::AVIRead::AVIRDBManagerForm *me = (SSWR::AVIRead::AVIRDBManagerForm*)userObj;
	if (me->currDB->IsDBTool() && ((DB::ReadingDBTool*)me->currDB)->CanModify())
	{
		UOSInt index = me->lvSvrConn->GetSelectedIndex();
		if (index != INVALID_INDEX)
		{
			Int32 id = (Int32)(OSInt)me->lvSvrConn->GetItem(index);
			if (((DB::DBTool*)me->currDB)->KillConnection(id))
			{
				me->UpdateSvrConnList();
			}
		}
	}
}

void __stdcall SSWR::AVIRead::AVIRDBManagerForm::OnFileHandler(void *userObj, NotNullPtr<Text::String> *files, UOSInt nFiles)
{
	SSWR::AVIRead::AVIRDBManagerForm *me = (SSWR::AVIRead::AVIRDBManagerForm*)userObj;
	DB::ReadingDBTool *db = 0;
	if (me->currDB && me->currDB->IsDBTool())
	{
		db = (DB::ReadingDBTool*)me->currDB;
	}
	Bool isSQLTab = (me->tcMain->GetSelectedPage() == me->tpSQL.Ptr());
	UOSInt i = 0;
	while (i < nFiles)
	{
		if (isSQLTab && db && files[i]->EndsWith(UTF8STRC(".sql")))
		{
			me->RunSQLFile(db, files[i]);
		}
		else
		{
			IO::Path::PathType pt = IO::Path::GetPathType(files[i]->ToCString());
			DB::ReadingDB *db;
			NotNullPtr<DB::ReadingDB> nndb;
			if (pt == IO::Path::PathType::Directory)
			{
				IO::DirectoryPackage dpkg(files[i]);
				db = (DB::ReadingDB*)me->core->GetParserList()->ParseObjectType(dpkg, IO::ParserType::ReadingDB);
			}
			else
			{
				IO::StmData::FileData fd(files[i], false);
				db = (DB::ReadingDB*)me->core->GetParserList()->ParseFileType(fd, IO::ParserType::ReadingDB);
			}
			if (nndb.Set(db))
			{
				DB::DBManagerCtrl *ctrl = DB::DBManagerCtrl::CreateFromFile(nndb, files[i], me->log, me->core->GetSocketFactory(), me->core->GetParserList());
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
	if (this->currDB == 0)
	{
		return;
	}
	Text::String *dbName;
	Data::ArrayListNN<Text::String> dbNames;
	UOSInt i = 0;
	UOSInt j = this->currDB->GetDatabaseNames(dbNames);
	ArtificialQuickSort_Sort(&dbNames, 0, (OSInt)(j - 1));
	while (i < j)
	{
		dbName = dbNames.GetItem(i);
		this->lbDatabase->AddItem(Text::String::OrEmpty(dbName), 0);
		i++;
	}
	this->currDB->ReleaseDatabaseNames(dbNames);
}

void SSWR::AVIRead::AVIRDBManagerForm::UpdateSchemaList()
{
	this->lbSchema->ClearItems();
	this->lbMapSchema->ClearItems();
	if (this->currDB == 0)
	{
		return;
	}
	Data::ArrayListNN<Text::String> schemaNames;
	UOSInt i;
	UOSInt j;

	this->currDB->QuerySchemaNames(schemaNames);
	if (schemaNames.GetCount() == 0)
	{
		this->lbSchema->AddItem(CSTR(""), 0);
		this->lbMapSchema->AddItem(CSTR(""), 0);
	}
	i = 0;
	j = schemaNames.GetCount();
	while (i < j)
	{
		NotNullPtr<Text::String> schemaName = Text::String::OrEmpty(schemaNames.GetItem(i));
		this->lbSchema->AddItem(schemaName, 0);
		this->lbMapSchema->AddItem(schemaName, 0);
		i++;
	}

	LIST_FREE_STRING(&schemaNames);
	this->lbSchema->SetSelectedIndex(0);
	this->lbMapSchema->SetSelectedIndex(0);
}

void SSWR::AVIRead::AVIRDBManagerForm::UpdateTableList()
{
	this->lbTable->ClearItems();
	if (this->currDB == 0)
	{
		return;
	}
	Text::String *schemaName = this->lbSchema->GetSelectedItemTextNew();
	NotNullPtr<Text::String> tableName;
	Data::ArrayListNN<Text::String> tableNames;
	UOSInt i = 0;
	UOSInt j = this->currDB->QueryTableNames(STR_CSTR(schemaName), tableNames);
	SDEL_STRING(schemaName);
	ArtificialQuickSort_Sort(&tableNames, 0, (OSInt)j - 1);
	while (i < j)
	{
		tableName = Text::String::OrEmpty(tableNames.GetItem(i));
		this->lbTable->AddItem(tableName, 0);
		i++;
	}
	LIST_FREE_STRING(&tableNames);
}

void SSWR::AVIRead::AVIRDBManagerForm::UpdateMapTableList()
{
	this->lbMapTable->ClearItems();
	if (this->currDB == 0)
	{
		return;
	}
	Text::String *schemaName = this->lbMapSchema->GetSelectedItemTextNew();
	Text::String *tableName;
	Data::ArrayListNN<Text::String> tableNames;
	UOSInt i = 0;
	UOSInt j = this->currDB->QueryTableNames(STR_CSTR(schemaName), tableNames);
	SDEL_STRING(schemaName);
	ArtificialQuickSort_Sort(&tableNames, 0, (OSInt)j - 1);
	while (i < j)
	{
		tableName = tableNames.GetItem(i);
		this->lbMapTable->AddItem(Text::String::OrEmpty(tableName), 0);
		i++;
	}
	LIST_FREE_STRING(&tableNames);
}

void SSWR::AVIRead::AVIRDBManagerForm::UpdateTableData(Text::CString schemaName, Text::String *tableName)
{
	this->lvTable->ClearItems();
	this->lvTableResult->ClearItems();
	if (tableName == 0 || this->currDB == 0)
	{
		return;
	}

	UTF8Char sbuff[256];
	UTF8Char *sptr;
	NotNullPtr<Text::String> s;
	DB::TableDef *tabDef = 0;
	NotNullPtr<DB::DBReader> r;
	tabDef = this->currDB->GetTableDef(schemaName, STR_CSTR(tableName));

	if (r.Set(this->currDB->QueryTableData(schemaName, STR_CSTR(tableName), 0, 0, MAX_ROW_CNT, CSTR_NULL, 0)))
	{
		UpdateResult(r, this->lvTableResult);

		UOSInt i;
		UOSInt j;
		UOSInt k;
		if (tabDef)
		{
			DB::ColDef *col;
			j = tabDef->GetColCnt();
			i = 0;
			while (i < j)
			{
				col = tabDef->GetCol(i);
				k = this->lvTable->AddItem(col->GetColName(), 0);
				sptr = col->ToColTypeStr(sbuff);
				this->lvTable->SetSubItem(k, 1, CSTRP(sbuff, sptr));
				if (s.Set(col->GetNativeType()))
					this->lvTable->SetSubItem(k, 2, s);
				this->lvTable->SetSubItem(k, 3, col->IsNotNull()?CSTR("NOT NULL"):CSTR("NULL"));
				this->lvTable->SetSubItem(k, 4, col->IsPK()?CSTR("PK"):CSTR(""));
				this->lvTable->SetSubItem(k, 5, col->IsAutoInc()?CSTR("AUTO_INCREMENT"):CSTR(""));
				if (s.Set(col->GetDefVal()))
					this->lvTable->SetSubItem(k, 6, s);
				if (s.Set(col->GetAttr()))
					this->lvTable->SetSubItem(k, 7, s);

				i++;
			}
			
			DEL_CLASS(tabDef);
		}
		else
		{
			DB::ColDef col(Text::String::NewEmpty());
			j = r->ColCount();
			i = 0;
			while (i < j)
			{
				r->GetColDef(i, col);
				k = this->lvTable->AddItem(col.GetColName(), 0);
				sptr = col.ToColTypeStr(sbuff);
				this->lvTable->SetSubItem(k, 1, CSTRP(sbuff, sptr));
				if (s.Set(col.GetNativeType()))
					this->lvTable->SetSubItem(k, 2, s);
				this->lvTable->SetSubItem(k, 3, col.IsNotNull()?CSTR("NOT NULL"):CSTR("NULL"));
				this->lvTable->SetSubItem(k, 4, col.IsPK()?CSTR("PK"):CSTR(""));
				this->lvTable->SetSubItem(k, 5, col.IsAutoInc()?CSTR("AUTO_INCREMENT"):CSTR(""));
				if (s.Set(col.GetDefVal()))
					this->lvTable->SetSubItem(k, 6, s);
				if (s.Set(col.GetAttr()))
					this->lvTable->SetSubItem(k, 7, s);

				i++;
			}
		}

		this->currDB->CloseReader(r);
	}
	else
	{
		if (tabDef)
		{
			DEL_CLASS(tabDef);
		}
	}
}

void SSWR::AVIRead::AVIRDBManagerForm::UpdateResult(NotNullPtr<DB::DBReader> r, UI::GUIListView *lv)
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
	Data::ArrayList<Data::TwinItem<Text::String*, Text::String*>> vars;
	Data::TwinItem<Text::String*, Text::String*> item(0);
	this->lvVariable->ClearItems();
	if (this->currDB->IsDBTool())
	{
		NotNullPtr<Text::String> s;
		UOSInt i = 0;
		UOSInt j = ((DB::ReadingDBTool*)this->currDB)->GetVariables(&vars);
		while (i < j)
		{
			item = vars.GetItem(i);
			this->lvVariable->AddItem(Text::String::OrEmpty(item.key), 0);
			if (s.Set(item.value)) this->lvVariable->SetSubItem(i, 1, s);
			i++;
		}
		((DB::ReadingDBTool*)this->currDB)->FreeVariables(&vars);
	}
}

void SSWR::AVIRead::AVIRDBManagerForm::UpdateSvrConnList()
{
	UTF8Char sbuff[32];
	UTF8Char *sptr;
	this->lvSvrConn->ClearItems();
	if (this->currDB->IsDBTool())
	{
		Data::ArrayList<DB::ReadingDBTool::ConnectionInfo*> conns;
		DB::ReadingDBTool::ConnectionInfo *conn;
		NotNullPtr<Text::String> s;
		UOSInt i = 0;
		UOSInt j = ((DB::ReadingDBTool*)this->currDB)->GetConnectionInfo(&conns);
		while (i < j)
		{
			conn = conns.GetItem(i);
			sptr = Text::StrInt32(sbuff, conn->id);
			this->lvSvrConn->AddItem(CSTRP(sbuff, sptr), (void*)(OSInt)conn->id);
			if (s.Set(conn->status)) this->lvSvrConn->SetSubItem(i, 1, s);
			if (s.Set(conn->user)) this->lvSvrConn->SetSubItem(i, 2, s);
			if (s.Set(conn->clientHostName)) this->lvSvrConn->SetSubItem(i, 3, s);
			if (s.Set(conn->dbName)) this->lvSvrConn->SetSubItem(i, 4, s);
			if (s.Set(conn->cmd)) this->lvSvrConn->SetSubItem(i, 5, s);
			sptr = Text::StrInt32(sbuff, conn->timeUsed);
			this->lvSvrConn->SetSubItem(i, 6, CSTRP(sbuff, sptr));
			if (s.Set(conn->sql)) this->lvSvrConn->SetSubItem(i, 7, s);
			i++;
		}
		((DB::ReadingDBTool*)this->currDB)->FreeConnectionInfo(&conns);
	}
}

void SSWR::AVIRead::AVIRDBManagerForm::RunSQLFile(DB::ReadingDBTool *db, NotNullPtr<Text::String> fileName)
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
			NotNullPtr<DB::DBReader> r;
			if (r.Set(db->ExecuteReader(sb.ToCString())))
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
				this->currDB->CloseReader(r);
			}
			else
			{
				sb.ClearStr();
				this->currDB->GetLastErrorMsg(sb);
				UI::MessageDialog::ShowDialog(sb.ToCString(), CSTR("DB Manager"), this);
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

Data::Class *SSWR::AVIRead::AVIRDBManagerForm::CreateTableClass(Text::CString schemaName, Text::CStringNN tableName)
{
	NotNullPtr<DB::ReadingDB> db;
	if (db.Set(this->currDB))
	{
		return DB::DBExporter::CreateTableClass(db, schemaName, tableName);
	}
	return 0;
}

void SSWR::AVIRead::AVIRDBManagerForm::CopyTableCreate(DB::SQLType sqlType, Bool axisAware)
{
	Text::String *schemaName = this->lbSchema->GetSelectedItemTextNew();
	Text::String *tableName = this->lbTable->GetSelectedItemTextNew();
	DB::SQLBuilder sql(sqlType, axisAware, 0);
	NotNullPtr<DB::TableDef> tabDef;
	if (tabDef.Set(this->currDB->GetTableDef(STR_CSTR(schemaName), tableName->ToCString())))
	{
		if (!DB::SQLGenerator::GenCreateTableCmd(sql, STR_CSTR(schemaName), tableName->ToCString(), tabDef, true))
		{
			UI::MessageDialog::ShowDialog(CSTR("Error in generating Create SQL command"), CSTR("DB Manager"), this);
		}
		else
		{
			UI::Clipboard::SetString(this->GetHandle(), sql.ToCString());
		}
		tabDef.Delete();
	}
	else
	{
		UI::MessageDialog::ShowDialog(CSTR("Error in getting table definition"), CSTR("DB Manager"), this);
	}
	tableName->Release();
	SDEL_STRING(schemaName);
}

void SSWR::AVIRead::AVIRDBManagerForm::ExportTableData(DB::SQLType sqlType, Bool axisAware)
{
	UTF8Char sbuff[512];
	UTF8Char *sptr;
	NotNullPtr<DB::ReadingDB> db;
	if (!db.Set(this->currDB))
	{
		UI::MessageDialog::ShowDialog(CSTR("Please select database first"), CSTR("DB Manager"), this);
		return;
	}
	Text::String *schemaName = this->lbSchema->GetSelectedItemTextNew();
	Text::String *tableName = this->lbTable->GetSelectedItemTextNew();
	sptr = sbuff;
	if (schemaName->leng > 0)
	{
		sptr = schemaName->ConcatTo(sptr);
		*sptr++ = '_';
	}
	sptr = tableName->ConcatTo(sptr);
	*sptr++ = '_';
	sptr = Data::Timestamp::Now().ToString(sptr, "yyyyMMdd_HHmmss");
	sptr = Text::StrConcatC(sptr, UTF8STRC(".sql"));
	UI::FileDialog dlg(L"SSWR", L"AVIRead", L"DBManagerExportTable", true);
	dlg.AddFilter(CSTR("*.sql"), CSTR("SQL File"));
	dlg.SetFileName(CSTRP(sbuff, sptr));
	if (dlg.ShowDialog(this->GetHandle()))
	{
		IO::FileStream fs(dlg.GetFileName(), IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
		if (!DB::DBExporter::GenerateInsertSQLs(db, sqlType, axisAware, STR_CSTR(schemaName), tableName->ToCString(), this->currCond, fs))
		{
			UI::MessageDialog::ShowDialog(CSTR("Error in reading table data"), CSTR("DB Manager"), this);
		}
	}
	tableName->Release();
	SDEL_STRING(schemaName);
}

void SSWR::AVIRead::AVIRDBManagerForm::ExportTableCSV()
{
	UTF8Char sbuff[512];
	UTF8Char *sptr;
	NotNullPtr<DB::ReadingDB> db;
	if (!db.Set(this->currDB))
	{
		UI::MessageDialog::ShowDialog(CSTR("Please select database first"), CSTR("DB Manager"), this);
		return;
	}
	Text::String *schemaName = this->lbSchema->GetSelectedItemTextNew();
	Text::String *tableName = this->lbTable->GetSelectedItemTextNew();
	sptr = sbuff;
	if (schemaName->leng > 0)
	{
		sptr = schemaName->ConcatTo(sptr);
		*sptr++ = '_';
	}
	sptr = tableName->ConcatTo(sptr);
	*sptr++ = '_';
	sptr = Data::Timestamp::Now().ToString(sptr, "yyyyMMdd_HHmmss");
	sptr = Text::StrConcatC(sptr, UTF8STRC(".csv"));
	UI::FileDialog dlg(L"SSWR", L"AVIRead", L"DBManagerExportCSV", true);
	dlg.AddFilter(CSTR("*.csv"), CSTR("Comma-Seperated-Value File"));
	dlg.SetFileName(CSTRP(sbuff, sptr));
	if (dlg.ShowDialog(this->GetHandle()))
	{
		IO::FileStream fs(dlg.GetFileName(), IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
		if (!DB::DBExporter::GenerateCSV(db, STR_CSTR(schemaName), tableName->ToCString(), this->currCond, CSTR("\"\""), fs, 65001))
		{
			UI::MessageDialog::ShowDialog(CSTR("Error in reading table data"), CSTR("DB Manager"), this);
		}
	}
	tableName->Release();
	SDEL_STRING(schemaName);
}

void SSWR::AVIRead::AVIRDBManagerForm::ExportTableSQLite()
{
	UTF8Char sbuff[512];
	UTF8Char *sptr;
	NotNullPtr<DB::ReadingDB> db;
	if (!db.Set(this->currDB))
	{
		UI::MessageDialog::ShowDialog(CSTR("Please select database first"), CSTR("DB Manager"), this);
		return;
	}
	Text::String *schemaName = this->lbSchema->GetSelectedItemTextNew();
	Text::String *tableName = this->lbTable->GetSelectedItemTextNew();
	sptr = sbuff;
	if (schemaName->leng > 0)
	{
		sptr = schemaName->ConcatTo(sptr);
		*sptr++ = '_';
	}
	sptr = tableName->ConcatTo(sptr);
	*sptr++ = '_';
	sptr = Data::Timestamp::Now().ToString(sptr, "yyyyMMdd_HHmmss");
	sptr = Text::StrConcatC(sptr, UTF8STRC(".sqlite"));
	UI::FileDialog dlg(L"SSWR", L"AVIRead", L"DBManagerExportSQLite", true);
	dlg.AddFilter(CSTR("*.sqlite"), CSTR("SQLite File"));
	dlg.SetFileName(CSTRP(sbuff, sptr));
	if (dlg.ShowDialog(this->GetHandle()))
	{
		Text::StringBuilderUTF8 sb;
		DB::SQLiteFile sqlite(dlg.GetFileName());
		if (!DB::DBExporter::GenerateSQLite(db, STR_CSTR(schemaName), tableName->ToCString(), this->currCond, sqlite, &sb))
		{
			UI::MessageDialog::ShowDialog(sb.ToCString(), CSTR("DB Manager"), this);
		}
	}
	tableName->Release();
	SDEL_STRING(schemaName);
}

void SSWR::AVIRead::AVIRDBManagerForm::ExportTableHTML()
{
	UTF8Char sbuff[512];
	UTF8Char *sptr;
	NotNullPtr<DB::ReadingDB> db;
	if (!db.Set(this->currDB))
	{
		UI::MessageDialog::ShowDialog(CSTR("Please select database first"), CSTR("DB Manager"), this);
		return;
	}
	Text::String *schemaName = this->lbSchema->GetSelectedItemTextNew();
	Text::String *tableName = this->lbTable->GetSelectedItemTextNew();
	sptr = sbuff;
	if (schemaName->leng > 0)
	{
		sptr = schemaName->ConcatTo(sptr);
		*sptr++ = '_';
	}
	sptr = tableName->ConcatTo(sptr);
	*sptr++ = '_';
	sptr = Data::Timestamp::Now().ToString(sptr, "yyyyMMdd_HHmmss");
	sptr = Text::StrConcatC(sptr, UTF8STRC(".html"));
	UI::FileDialog dlg(L"SSWR", L"AVIRead", L"DBManagerExportHTML", true);
	dlg.AddFilter(CSTR("*.html"), CSTR("HTML File"));
	dlg.SetFileName(CSTRP(sbuff, sptr));
	if (dlg.ShowDialog(this->GetHandle()))
	{
		IO::FileStream fs(dlg.GetFileName(), IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
		if (!DB::DBExporter::GenerateHTML(db, STR_CSTR(schemaName), tableName->ToCString(), this->currCond, fs, 65001))
		{
			UI::MessageDialog::ShowDialog(CSTR("Error in exporting as PList"), CSTR("DB Manager"), this);
		}
	}
	tableName->Release();
	SDEL_STRING(schemaName);
}

void SSWR::AVIRead::AVIRDBManagerForm::ExportTablePList()
{
	UTF8Char sbuff[512];
	UTF8Char *sptr;
	NotNullPtr<DB::ReadingDB> db;
	if (!db.Set(this->currDB))
	{
		UI::MessageDialog::ShowDialog(CSTR("Please select database first"), CSTR("DB Manager"), this);
		return;
	}
	Text::String *schemaName = this->lbSchema->GetSelectedItemTextNew();
	Text::String *tableName = this->lbTable->GetSelectedItemTextNew();
	sptr = sbuff;
	if (schemaName->leng > 0)
	{
		sptr = schemaName->ConcatTo(sptr);
		*sptr++ = '_';
	}
	sptr = tableName->ConcatTo(sptr);
	*sptr++ = '_';
	sptr = Data::Timestamp::Now().ToString(sptr, "yyyyMMdd_HHmmss");
	sptr = Text::StrConcatC(sptr, UTF8STRC(".plist"));
	UI::FileDialog dlg(L"SSWR", L"AVIRead", L"DBManagerExportPList", true);
	dlg.AddFilter(CSTR("*.plist"), CSTR("PList File"));
	dlg.SetFileName(CSTRP(sbuff, sptr));
	if (dlg.ShowDialog(this->GetHandle()))
	{
		IO::FileStream fs(dlg.GetFileName(), IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
		if (!DB::DBExporter::GeneratePList(db, STR_CSTR(schemaName), tableName->ToCString(), this->currCond, fs, 65001))
		{
			UI::MessageDialog::ShowDialog(CSTR("Error in exporting as PList"), CSTR("DB Manager"), this);
		}
	}
	tableName->Release();
	SDEL_STRING(schemaName);
}

void SSWR::AVIRead::AVIRDBManagerForm::ExportTableXLSX()
{
	UTF8Char sbuff[512];
	UTF8Char *sptr;
	NotNullPtr<DB::ReadingDB> db;
	if (!db.Set(this->currDB))
	{
		UI::MessageDialog::ShowDialog(CSTR("Please select database first"), CSTR("DB Manager"), this);
		return;
	}
	Text::String *schemaName = this->lbSchema->GetSelectedItemTextNew();
	Text::String *tableName = this->lbTable->GetSelectedItemTextNew();
	sptr = sbuff;
	if (schemaName->leng > 0)
	{
		sptr = schemaName->ConcatTo(sptr);
		*sptr++ = '_';
	}
	sptr = tableName->ConcatTo(sptr);
	*sptr++ = '_';
	sptr = Data::Timestamp::Now().ToString(sptr, "yyyyMMdd_HHmmss");
	sptr = Text::StrConcatC(sptr, UTF8STRC(".xlsx"));
	UI::FileDialog dlg(L"SSWR", L"AVIRead", L"DBManagerExportXLSX", true);
	dlg.AddFilter(CSTR("*.xlsx"), CSTR("Excel 2007 File"));
	dlg.SetFileName(CSTRP(sbuff, sptr));
	if (dlg.ShowDialog(this->GetHandle()))
	{
		Text::StringBuilderUTF8 sb;
		IO::FileStream fs(dlg.GetFileName(), IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
		if (!DB::DBExporter::GenerateXLSX(db, STR_CSTR(schemaName), tableName->ToCString(), this->currCond, fs, &sb))
		{
			UI::MessageDialog::ShowDialog(sb.ToCString(), CSTR("DB Manager"), this);
		}
	}
	tableName->Release();
	SDEL_STRING(schemaName);
}

void SSWR::AVIRead::AVIRDBManagerForm::ExportTableExcelXML()
{
	UTF8Char sbuff[512];
	UTF8Char *sptr;
	NotNullPtr<DB::ReadingDB> db;
	if (!db.Set(this->currDB))
	{
		UI::MessageDialog::ShowDialog(CSTR("Please select database first"), CSTR("DB Manager"), this);
		return;
	}
	Text::String *schemaName = this->lbSchema->GetSelectedItemTextNew();
	Text::String *tableName = this->lbTable->GetSelectedItemTextNew();
	sptr = sbuff;
	if (schemaName->leng > 0)
	{
		sptr = schemaName->ConcatTo(sptr);
		*sptr++ = '_';
	}
	sptr = tableName->ConcatTo(sptr);
	*sptr++ = '_';
	sptr = Data::Timestamp::Now().ToString(sptr, "yyyyMMdd_HHmmss");
	sptr = Text::StrConcatC(sptr, UTF8STRC(".xml"));
	UI::FileDialog dlg(L"SSWR", L"AVIRead", L"DBManagerExportXML", true);
	dlg.AddFilter(CSTR("*.xml"), CSTR("Excel XML File"));
	dlg.SetFileName(CSTRP(sbuff, sptr));
	if (dlg.ShowDialog(this->GetHandle()))
	{
		Text::StringBuilderUTF8 sb;
		IO::FileStream fs(dlg.GetFileName(), IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
		if (!DB::DBExporter::GenerateExcelXML(db, STR_CSTR(schemaName), tableName->ToCString(), this->currCond, fs, &sb))
		{
			UI::MessageDialog::ShowDialog(sb.ToCString(), CSTR("DB Manager"), this);
		}
	}
	tableName->Release();
	SDEL_STRING(schemaName);
}

SSWR::AVIRead::AVIRDBManagerForm::AVIRDBManagerForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core) : UI::GUIForm(parent, 1024, 768, ui)
{
	this->SetFont(0, 0, 8.25, false);
	this->SetText(CSTR("Database Manager"));
	this->core = core;
	this->ssl = Net::SSLEngineFactory::Create(core->GetSocketFactory(), true);
	this->currDB = 0;
	this->currCond = 0;
	this->sqlFileMode = false;
	NEW_CLASSNN(this->mapEnv, Map::MapEnv(CSTR("DB"), 0xffc0c0ff, Math::CoordinateSystemManager::CreateDefaultCsys()));
	NotNullPtr<Map::MapDrawLayer> layer;
	if (layer.Set(Map::BaseMapLayer::CreateLayer(Map::BaseMapLayer::BLT_OSM_TILE, this->core->GetSocketFactory(), this->ssl, this->core->GetParserList())))
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

	NEW_CLASS(this->lbConn, UI::GUIListBox(ui, *this, false));
	this->lbConn->SetRect(0, 0, 150, 23, false);
	this->lbConn->SetDockType(UI::GUIControl::DOCK_LEFT);
	this->lbConn->HandleSelectionChange(OnConnSelChg, this);
	this->lbConn->HandleRightClicked(OnConnRClicked, this);
	this->lbConn->HandleDoubleClicked(OnConnDblClicked, this);
	NEW_CLASS(this->hspConn, UI::GUIHSplitter(ui, *this, 3, false));
	NEW_CLASS(this->tcMain, UI::GUITabControl(ui, *this));
	this->tcMain->SetDockType(UI::GUIControl::DOCK_FILL);

	this->tpDatabase = this->tcMain->AddTabPage(CSTR("Database"));
	NEW_CLASS(this->lbDatabase, UI::GUIListBox(ui, this->tpDatabase, false));
	this->lbDatabase->SetRect(0, 0, 150, 23, false);
	this->lbDatabase->SetDockType(UI::GUIControl::DOCK_LEFT);
	NEW_CLASS(this->btnDatabaseChange, UI::GUIButton(ui, this->tpDatabase, CSTR("Change")));
	this->btnDatabaseChange->SetRect(154, 4, 75, 23, false);
	this->btnDatabaseChange->HandleButtonClick(OnDatabaseChangeClicked, this);
	NEW_CLASS(this->btnDatabaseDelete, UI::GUIButton(ui, this->tpDatabase, CSTR("Delete")));
	this->btnDatabaseDelete->SetRect(154, 28, 75, 23, false);
	this->btnDatabaseDelete->HandleButtonClick(OnDatabaseDeleteClicked, this);
	NEW_CLASS(this->btnDatabaseNew, UI::GUIButton(ui, this->tpDatabase, CSTR("New")));
	this->btnDatabaseNew->SetRect(154, 52, 75, 23, false);
	this->btnDatabaseNew->HandleButtonClick(OnDatabaseNewClicked, this);

	this->tpTable = this->tcMain->AddTabPage(CSTR("Table"));
	NEW_CLASSNN(this->pnlTable, UI::GUIPanel(ui, this->tpTable));
	this->pnlTable->SetRect(0, 0, 100, 250, false);
	this->pnlTable->SetDockType(UI::GUIControl::DOCK_TOP);
	NEW_CLASS(this->lbSchema, UI::GUIListBox(ui, this->pnlTable, false));
	this->lbSchema->SetRect(0, 0, 150, 100, false);
	this->lbSchema->SetDockType(UI::GUIControl::DOCK_LEFT);
	this->lbSchema->HandleSelectionChange(OnSchemaSelChg, this);
	this->lbSchema->HandleRightClicked(OnSchemaRClicked, this);
	NEW_CLASS(this->hspSchema, UI::GUIHSplitter(ui, this->pnlTable, 3, false));
	NEW_CLASS(this->lbTable, UI::GUIListBox(ui, this->pnlTable, false));
	this->lbTable->SetRect(0, 0, 150, 23, false);
	this->lbTable->SetDockType(UI::GUIControl::DOCK_LEFT);
	this->lbTable->HandleSelectionChange(OnTableSelChg, this);
	this->lbTable->HandleRightClicked(OnTableRClicked, this);
	NEW_CLASS(this->hspTable, UI::GUIHSplitter(ui, this->pnlTable, 3, false));
	NEW_CLASS(this->lvTable, UI::GUIListView(ui, this->pnlTable, UI::GUIListView::LVSTYLE_TABLE, 8));
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
	NEW_CLASS(this->vspTable, UI::GUIVSplitter(ui, this->tpTable, 3, false));
	NEW_CLASS(this->lvTableResult, UI::GUIListView(ui, this->tpTable, UI::GUIListView::LVSTYLE_TABLE, 1));
	this->lvTableResult->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lvTableResult->SetFullRowSelect(true);
	this->lvTableResult->SetShowGrid(true);

	this->tpSQL = this->tcMain->AddTabPage(CSTR("SQL"));
	NEW_CLASSNN(this->pnlSQL, UI::GUIPanel(ui, this->tpSQL));
	this->pnlSQL->SetRect(0, 0, 100, 152, false);
	this->pnlSQL->SetDockType(UI::GUIControl::DOCK_TOP);
	NEW_CLASSNN(this->pnlSQLCtrl, UI::GUIPanel(ui, this->pnlSQL));
	this->pnlSQLCtrl->SetRect(0, 0, 100, 31, false);
	this->pnlSQLCtrl->SetDockType(UI::GUIControl::DOCK_BOTTOM);
	NEW_CLASS(this->btnSQLExec, UI::GUIButton(ui, this->pnlSQLCtrl, CSTR("Execute")));
	this->btnSQLExec->SetRect(4, 4, 75, 23, false);
	this->btnSQLExec->HandleButtonClick(OnSQLExecClicked, this);
	NEW_CLASS(this->btnSQLFile, UI::GUIButton(ui, this->pnlSQLCtrl, CSTR("SQL File")));
	this->btnSQLFile->SetRect(84, 4, 75, 23, false);
	this->btnSQLFile->HandleButtonClick(OnSQLFileClicked, this);
	NEW_CLASS(this->txtSQL, UI::GUITextBox(ui, this->pnlSQL, CSTR(""), true));
	this->txtSQL->SetDockType(UI::GUIControl::DOCK_FILL);
	NEW_CLASS(this->vspSQL, UI::GUIVSplitter(ui, this->tpSQL, 3, false));
	NEW_CLASS(this->lvSQLResult, UI::GUIListView(ui, this->tpSQL, UI::GUIListView::LVSTYLE_TABLE, 1));
	this->lvSQLResult->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lvSQLResult->SetFullRowSelect(true);
	this->lvSQLResult->SetShowGrid(true);

	this->tpMap = this->tcMain->AddTabPage(CSTR("Map"));
	NEW_CLASSNN(this->pnlMap, UI::GUIPanel(ui, this->tpMap));
	this->pnlMap->SetRect(0, 0, 250, 23, false);
	this->pnlMap->SetDockType(UI::GUIControl::DOCK_LEFT);
	NEW_CLASS(this->hspMap, UI::GUIHSplitter(ui, this->tpMap, 3, false));
	NEW_CLASS(this->mapMain, UI::GUIMapControl(ui, this->tpMap, this->core->GetDrawEngine(), this->mapEnv, this->colorSess));
	this->mapMain->SetDockType(UI::GUIControl::DOCK_FILL);
	this->mapMain->HandleMouseDown(OnMapMouseDown, this);
	this->mapMain->HandleMouseUp(OnMapMouseUp, this);
	NEW_CLASSNN(this->pnlMapTable, UI::GUIPanel(ui, this->pnlMap));
	this->pnlMapTable->SetRect(0, 0, 100, 100, false);
	this->pnlMapTable->SetDockType(UI::GUIControl::DOCK_TOP);
	NEW_CLASS(this->vspMapRecord, UI::GUIVSplitter(ui, this->pnlMap, 3, false));
	NEW_CLASS(this->lvMapRecord, UI::GUIListView(ui, this->pnlMap, UI::GUIListView::LVSTYLE_TABLE, 2));
	this->lvMapRecord->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lvMapRecord->SetFullRowSelect(true);
	this->lvMapRecord->SetShowGrid(true);
	this->lvMapRecord->AddColumn(CSTR("Name"), 100);
	this->lvMapRecord->AddColumn(CSTR("Value"), 100);
	NEW_CLASS(this->lbMapSchema, UI::GUIListBox(ui, this->pnlMapTable, false));
	this->lbMapSchema->SetRect(0, 0, 100, 23, false);
	this->lbMapSchema->SetDockType(UI::GUIControl::DOCK_LEFT);
	this->lbMapSchema->HandleSelectionChange(OnMapSchemaSelChg, this);
	NEW_CLASS(this->hspMapTable, UI::GUIHSplitter(ui, this->pnlMapTable, 3, false));
	NEW_CLASS(this->lbMapTable, UI::GUIListBox(ui, this->pnlMapTable, false));
	this->lbMapTable->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lbMapTable->HandleSelectionChange(OnMapTableSelChg, this);

	this->tpVariable = this->tcMain->AddTabPage(CSTR("Variable"));
	NEW_CLASSNN(this->pnlVariable, UI::GUIPanel(ui, this->tpVariable));
	this->pnlVariable->SetRect(0, 0, 100, 31, false);
	this->pnlVariable->SetDockType(UI::GUIControl::DOCK_TOP);
	NEW_CLASS(this->btnVariable, UI::GUIButton(ui, this->pnlVariable, CSTR("Show")));
	this->btnVariable->SetRect(4, 4, 75, 23, false);
	this->btnVariable->HandleButtonClick(OnVariableClicked, this);
	NEW_CLASS(this->lvVariable, UI::GUIListView(ui, this->tpVariable, UI::GUIListView::LVSTYLE_TABLE, 2));
	this->lvVariable->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lvVariable->SetShowGrid(true);
	this->lvVariable->SetFullRowSelect(true);
	this->lvVariable->AddColumn(CSTR("Name"), 150);
	this->lvVariable->AddColumn(CSTR("Value"), 300);

	this->tpSvrConn = this->tcMain->AddTabPage(CSTR("Connection"));
	NEW_CLASSNN(this->pnlSvrConn, UI::GUIPanel(ui, this->tpSvrConn));
	this->pnlSvrConn->SetRect(0, 0, 100, 31, false);
	this->pnlSvrConn->SetDockType(UI::GUIControl::DOCK_TOP);
	NEW_CLASS(this->btnSvrConn, UI::GUIButton(ui, this->pnlSvrConn, CSTR("Show")));
	this->btnSvrConn->SetRect(4, 4, 75, 23, false);
	this->btnSvrConn->HandleButtonClick(OnSvrConnClicked, this);
	NEW_CLASS(this->btnSvrConnKill, UI::GUIButton(ui, this->pnlSvrConn, CSTR("Kill")));
	this->btnSvrConnKill->SetRect(84, 4, 75, 23, false);
	this->btnSvrConnKill->HandleButtonClick(OnSvrConnKillClicked, this);
	NEW_CLASS(this->lvSvrConn, UI::GUIListView(ui, this->tpSvrConn, UI::GUIListView::LVSTYLE_TABLE, 8));
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
	NEW_CLASS(this->txtLog, UI::GUITextBox(ui, this->tpLog, CSTR("")));
	this->txtLog->SetRect(0, 0, 100, 23, false);
	this->txtLog->SetDockType(UI::GUIControl::DOCK_BOTTOM);
	NEW_CLASSNN(this->lbLog, UI::GUIListBox(ui, this->tpLog, false));
	this->lbLog->SetDockType(UI::GUIControl::DOCK_FILL);

	NEW_CLASSNN(this->logger, UI::ListBoxLogger(*this, this->lbLog, 100, true));
	this->log.AddLogHandler(this->logger, IO::LogHandler::LogLevel::Raw);


	UI::GUIMenu *mnu;
	UI::GUIMenu *mnu2;
	NEW_CLASS(this->mnuMain, UI::GUIMainMenu());
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

	NEW_CLASS(this->mnuConn, UI::GUIPopupMenu());
	this->mnuConn->AddItem(CSTR("Remove"), MNU_CONN_REMOVE, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	this->mnuConn->AddItem(CSTR("Copy Conn String"), MNU_CONN_COPY_STR, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);

	NEW_CLASS(this->mnuSchema, UI::GUIPopupMenu());
	this->mnuSchema->AddItem(CSTR("New"), MNU_SCHEMA_NEW, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	this->mnuSchema->AddItem(CSTR("Delete"), MNU_SCHEMA_DELETE, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);

	NEW_CLASS(this->mnuTable, UI::GUIPopupMenu());
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
	UTF8Char *sptr;
	sptr = IO::Path::GetProcessFileName(sbuff);
	sptr = IO::Path::AppendPath(sbuff, sptr, DBCONNFILE);
	if (DB::DBManager::RestoreConn(CSTRP(sbuff, sptr), &this->dbList, this->log, this->core->GetSocketFactory(), this->core->GetParserList()))
	{
		Text::StringBuilderUTF8 sb;
		DB::DBManagerCtrl *ctrl;
		UOSInt i = 0;
		UOSInt j = this->dbList.GetCount();
		while (i < j)
		{
			ctrl = this->dbList.GetItem(i);
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
	DEL_CLASS(this->mnuTable);
	DEL_CLASS(this->mnuSchema);
	DEL_CLASS(this->mnuConn);
	this->mapEnv.Delete();
	this->core->GetColorMgr()->DeleteSess(this->colorSess);
	UTF8Char sbuff[512];
	UTF8Char *sptr;
	sptr = IO::Path::GetProcessFileName(sbuff);
	sptr = IO::Path::AppendPath(sbuff, sptr, DBCONNFILE);
	DB::DBManager::StoreConn(CSTRP(sbuff, sptr), &this->dbList);
	UOSInt i = this->dbList.GetCount();
	DB::DBManagerCtrl *ctrl;
	while (i-- > 0)
	{
		ctrl = this->dbList.GetItem(i);
		DEL_CLASS(ctrl);
	}
	SDEL_CLASS(this->currCond);
	this->ssl.Delete();
}

void SSWR::AVIRead::AVIRDBManagerForm::EventMenuClicked(UInt16 cmdId)
{
	UTF8Char sbuff[512];
	UTF8Char sbuff2[512];
	UTF8Char *sptr;
	NotNullPtr<DB::DBConn> conn;
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
				if (conn.Set(dlg.GetDBConn()))
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
			SSWR::AVIRead::AVIRDBCopyTablesForm dlg(0, this->ui, this->core, &this->dbList);
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
				DB::DBManagerCtrl *ctrl = this->dbList.RemoveAt(i);
				DEL_CLASS(ctrl);
				this->lbConn->RemoveItem(i);
			}
		}
		break;
	case MNU_CONN_COPY_STR:
		{
			Text::StringBuilderUTF8 sb;
			DB::DBManagerCtrl *ctrl = (DB::DBManagerCtrl*)this->lbConn->GetSelectedItem();
			if (ctrl)
			{
				Text::String *connStr = ctrl->GetConnStr();
				if (connStr)
				{
					UI::Clipboard::SetString(this->GetHandle(), connStr->ToCString());
				}
				else
				{
					UI::MessageDialog::ShowDialog(CSTR("This connection is not supported"), CSTR("DB Manager"), this);
				}
			}
		}
		break;
	case MNU_SCHEMA_NEW:
		if (this->currDB && this->currDB->IsDBTool() && ((DB::ReadingDBTool*)this->currDB)->CanModify())
		{
			UtilUI::TextInputDialog dlg(0, this->ui, this->core->GetMonitorMgr(), CSTR("DB Manager"), CSTR("Please enter schema name to create"));
			if (dlg.ShowDialog(this))
			{
				Text::StringBuilderUTF8 sb;
				if (dlg.GetInputString(sb))
				{
					if (((DB::DBTool*)this->currDB)->CreateSchema(sb.ToCString()))
					{
						this->UpdateSchemaList();
					}
					else
					{
						sb.ClearStr();
						sb.AppendC(UTF8STRC("Error in creating schema: "));
						this->currDB->GetLastErrorMsg(sb);
						UI::MessageDialog::ShowDialog(sb.ToCString(), CSTR("DB Manager"), this);
					}
				}
			}
		}
		break;
	case MNU_SCHEMA_DELETE:
		if (this->currDB && this->currDB->IsDBTool() && ((DB::ReadingDBTool*)this->currDB)->CanModify())
		{
			Text::String *schemaName = this->lbSchema->GetSelectedItemTextNew();
			if (schemaName)
			{
				if (schemaName->leng > 0)
				{
					Text::StringBuilderUTF8 sb;
					sb.AppendC(UTF8STRC("Are you sure to delete schema "));
					sb.Append(schemaName);
					sb.AppendC(UTF8STRC("? This process cannot be undo."));
					if (UI::MessageDialog::ShowYesNoDialog(sb.ToCString(), CSTR("DB Manager"), this))
					{
						if (((DB::DBTool*)this->currDB)->DeleteSchema(schemaName->ToCString()))
						{
							this->UpdateSchemaList();
						}
						else
						{
							sb.ClearStr();
							sb.AppendC(UTF8STRC("Error in deleting schema "));
							sb.Append(schemaName);
							sb.AppendC(UTF8STRC(": "));
							this->currDB->GetLastErrorMsg(sb);
							UI::MessageDialog::ShowDialog(sb.ToCString(), CSTR("DB Manager"), this);
						}
					}
				}
				schemaName->Release();
			}
		}
		break;
	case MNU_TABLE_JAVA:
		{
			Text::String *schemaName = this->lbSchema->GetSelectedItemTextNew();
			Text::String *tableName = this->lbTable->GetSelectedItemTextNew();
			Text::String *databaseName = 0;
			NotNullPtr<DB::ReadingDB> db;
			if (db.Set(this->currDB))
			{
				databaseName = this->currDB->GetCurrDBName();
				Text::StringBuilderUTF8 sb;
				DB::JavaDBUtil::ToJavaEntity(sb, schemaName, tableName, databaseName, db);
				UI::Clipboard::SetString(this->GetHandle(), sb.ToCString());
			}
			else
			{
				UI::MessageDialog::ShowDialog(CSTR("Select database first"), CSTR("DB Manager"), this);
			}
			tableName->Release();
			schemaName->Release();
		}
		break;
	case MNU_TABLE_CPP_HEADER:
		if ((sptr = this->lbTable->GetSelectedItemText(sbuff)) != 0)
		{
			Data::Class *cls = this->CreateTableClass(CSTR_NULL, CSTRP(sbuff, sptr));
			if (cls)
			{
				Text::PString hdr = {sbuff2, 0};
				UOSInt i = Text::StrLastIndexOfCharC(sbuff, (UOSInt)(sptr - sbuff), '.');
				hdr.leng = (UOSInt)(DB::DBUtil::DB2FieldName(sbuff2, &sbuff[i + 1]) - sbuff2);
				sbuff2[0] = Text::CharUtil::ToUpper(sbuff2[0]);
				Text::StringBuilderUTF8 sb;
				cls->ToCppClassHeader(&hdr, 0, sb);
				UI::Clipboard::SetString(this->GetHandle(), sb.ToCString());
				DEL_CLASS(cls);
			}
		}
		break;
	case MNU_TABLE_CPP_SOURCE:
		if ((sptr = this->lbTable->GetSelectedItemText(sbuff)) != 0)
		{
			Data::Class *cls = this->CreateTableClass(CSTR_NULL, CSTRP(sbuff, sptr));
			if (cls)
			{
				Text::PString hdr = {sbuff2, 0};
				UOSInt i = Text::StrLastIndexOfCharC(sbuff, (UOSInt)(sptr - sbuff), '.');
				hdr.leng = (UOSInt)(DB::DBUtil::DB2FieldName(sbuff2, &sbuff[i + 1]) - sbuff2);
				sbuff2[0] = Text::CharUtil::ToUpper(sbuff2[0]);
				Text::StringBuilderUTF8 sb;
				cls->ToCppClassSource(0, &hdr, 0, sb);
				UI::Clipboard::SetString(this->GetHandle(), sb.ToCString());
				DEL_CLASS(cls);
			}
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
			Text::String *schemaName = this->lbSchema->GetSelectedItemTextNew();
			Text::String *tableName = this->lbTable->GetSelectedItemTextNew();
			SSWR::AVIRead::AVIRDBExportForm dlg(0, ui, this->core, this->currDB, STR_CSTR(schemaName), tableName->ToCString());
			dlg.ShowDialog(this);
			tableName->Release();
			SDEL_STRING(schemaName);
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
			Text::String *schemaName = this->lbSchema->GetSelectedItemTextNew();
			Text::String *tableName = this->lbTable->GetSelectedItemTextNew();
			SSWR::AVIRead::AVIRDBCheckChgForm dlg(0, ui, this->core, this->currDB, STR_CSTR(schemaName), tableName->ToCString());
			dlg.ShowDialog(this);
			tableName->Release();
			SDEL_STRING(schemaName);
		}
		break;
	}
}

void SSWR::AVIRead::AVIRDBManagerForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}

void SSWR::AVIRead::AVIRDBManagerForm::ConnAdd(NotNullPtr<DB::DBConn> conn)
{
	NotNullPtr<DB::DBTool> db;
	NEW_CLASSNN(db, DB::DBTool(conn, true, this->log, CSTR("DB: ")));
	DB::DBManagerCtrl *ctrl = DB::DBManagerCtrl::Create(db, this->log, this->core->GetSocketFactory(), this->core->GetParserList());
	this->dbList.Add(ctrl);
	Text::StringBuilderUTF8 sb;
	conn->GetConnName(sb);
	this->lbConn->AddItem(sb.ToCString(), ctrl);
}
