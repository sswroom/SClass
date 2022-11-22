#include "Stdafx.h"
#include "Data/ArrayListStrUTF8.h"
#include "Data/Sort/ArtificialQuickSortCmp.h"
#include "DB/ColDef.h"
#include "DB/DBManager.h"
#include "DB/JavaDBUtil.h"
#include "DB/SQLGenerator.h"
#include "IO/FileStream.h"
#include "Map/BaseMapLayer.h"
#include "Math/CoordinateSystemManager.h"
#include "Math/Math.h"
#include "Net/SSLEngineFactory.h"
#include "SSWR/AVIRead/AVIRAccessConnForm.h"
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
#include "UI/FileDialog.h"
#include "UI/MessageDialog.h"
#include "UtilUI/TextInputDialog.h"
#include "Win32/Clipboard.h"

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
	MNU_TABLE_CREATE_MSSQL,
	MNU_TABLE_CREATE_POSTGRESQL,
	MNU_TABLE_EXPORT_MYSQL,
	MNU_TABLE_EXPORT_MSSQL,
	MNU_TABLE_EXPORT_POSTGRESQL,
	MNU_TABLE_EXPORT_OPTION
} MenuEvent;

void __stdcall SSWR::AVIRead::AVIRDBManagerForm::OnConnSelChg(void *userObj)
{
	SSWR::AVIRead::AVIRDBManagerForm *me = (SSWR::AVIRead::AVIRDBManagerForm*)userObj;
	me->currDB = (DB::DBTool*)me->lbConn->GetSelectedItem();
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
		me->dbLayer->SetDatabase(me->currDB, schemaName->ToCString(), tableName->ToCString());
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
			void *sess;
			Int64 id;
			UOSInt i;
			UOSInt j;
			UTF8Char sbuff[512];
			UTF8Char *sptr;
			Math::Coord2DDbl mapPt = me->mapMain->ScnXY2MapXY(scnPos);
			Math::CoordinateSystem *csys = me->mapEnv->GetCoordinateSystem();
			Math::CoordinateSystem *lyrCSys = me->dbLayer->GetCoordinateSystem();
			Double tmp;
			if (csys && lyrCSys && !csys->Equals(lyrCSys))
			{
				Math::CoordinateSystem::ConvertXYZ(csys, lyrCSys, mapPt.x, mapPt.y, 0, &mapPt.x, &mapPt.y, &tmp);
			}
			sess = me->dbLayer->BeginGetObject();
			id = me->dbLayer->GetNearestObjectId(sess, mapPt, &mapPt);
			me->lvMapRecord->ClearItems();
			if (id == -1)
			{
			}
			else
			{
				Data::ArrayListInt64 arr;
				void *nameArr;
				me->dbLayer->GetObjectIdsMapXY(&arr, &nameArr, Math::RectAreaDbl(mapPt, mapPt), true);
				i = 0;
				j = me->dbLayer->GetColumnCnt();
				while (i < j)
				{
					sptr = me->dbLayer->GetColumnName(sbuff, i);
					me->lvMapRecord->AddItem(CSTRP(sbuff, sptr), 0);
					sbuff[0] = 0;
					sptr = me->dbLayer->GetString(sbuff, sizeof(sbuff), nameArr, id, i);
					me->lvMapRecord->SetSubItem(i, 1, CSTRP(sbuff, sptr));
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
			if (me->currDB->ChangeDatabase(dbName->v))
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
		Text::String *dbName = me->lbDatabase->GetSelectedItemTextNew();
		if (dbName)
		{
			Text::StringBuilderUTF8 sb;
			sb.AppendC(UTF8STRC("Are you sure to delete database "));
			sb.Append(dbName);
			sb.AppendC(UTF8STRC("? This process cannot be undo."));
			if (UI::MessageDialog::ShowYesNoDialog(sb.ToCString(), CSTR("DB Manager"), me))
			{
				if (me->currDB->DeleteDatabase(dbName->ToCString()))
				{
					me->UpdateDatabaseList();
				}
				else
				{
					sb.ClearStr();
					sb.AppendC(UTF8STRC("Error in deleting database "));
					sb.Append(dbName);
					sb.AppendC(UTF8STRC(": "));
					me->currDB->GetLastErrorMsg(&sb);
					UI::MessageDialog::ShowDialog(sb.ToCString(), CSTR("DB Manager"), me);
				}
			}
			dbName->Release();
		}
	}
}

void __stdcall SSWR::AVIRead::AVIRDBManagerForm::OnDatabaseNewClicked(void *userObj)
{
	SSWR::AVIRead::AVIRDBManagerForm *me = (SSWR::AVIRead::AVIRDBManagerForm*)userObj;
	if (me->currDB)
	{
		UtilUI::TextInputDialog dlg(0, me->ui, me->core->GetMonitorMgr(), CSTR("DB Manager"), CSTR("Please enter database name to create"));
		if (dlg.ShowDialog(me))
		{
			Text::StringBuilderUTF8 sb;
			if (dlg.GetInputString(&sb))
			{
				if (me->currDB->CreateDatabase(sb.ToCString()))
				{
					me->UpdateDatabaseList();
				}
				else
				{
					sb.ClearStr();
					sb.AppendC(UTF8STRC("Error in creating database: "));
					me->currDB->GetLastErrorMsg(&sb);
					UI::MessageDialog::ShowDialog(sb.ToCString(), CSTR("DB Manager"), me);
				}
			}
		}
	}
}

void __stdcall SSWR::AVIRead::AVIRDBManagerForm::OnSQLExecClicked(void *userObj)
{
	SSWR::AVIRead::AVIRDBManagerForm *me = (SSWR::AVIRead::AVIRDBManagerForm*)userObj;
	if (me->currDB)
	{
		Text::StringBuilderUTF8 sb;
		me->txtSQL->GetText(&sb);
		if (sb.GetLength() > 0)
		{
			DB::DBReader *r = me->currDB->ExecuteReader(sb.ToCString());
			if (r)
			{
				if (r->ColCount() == 0)
				{
					me->lvSQLResult->ClearAll();
					me->lvSQLResult->ChangeColumnCnt(1);
					me->lvSQLResult->AddColumn(CSTR("Row Changed"), 100);
					sb.ClearStr();
					sb.AppendOSInt(r->GetRowChanged());
					me->lvSQLResult->AddItem(sb.ToCString(), 0);
				}
				else
				{
					UpdateResult(r, me->lvSQLResult);
				}
				me->currDB->CloseReader(r);
			}
			else
			{
				sb.ClearStr();
				me->currDB->GetLastErrorMsg(&sb);
				UI::MessageDialog::ShowDialog(sb.ToCString(), CSTR("DB Manager"), me);
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
	UOSInt index = me->lvSvrConn->GetSelectedIndex();
	if (index != INVALID_INDEX)
	{
		Int32 id = (Int32)(OSInt)me->lvSvrConn->GetItem(index);
		if (me->currDB->KillConnection(id))
		{
			me->UpdateSvrConnList();
		}
	}
}

void SSWR::AVIRead::AVIRDBManagerForm::UpdateDatabaseList()
{
	this->lbDatabase->ClearItems();
	if (this->currDB == 0)
	{
		return;
	}
	const UTF8Char *dbName;
	Data::ArrayList<const UTF8Char*> dbNames;
	UOSInt i = 0;
	UOSInt j = this->currDB->GetDatabaseNames(&dbNames);
	ArtificialQuickSort_Sort(&dbNames, 0, (OSInt)(j - 1));
	while (i < j)
	{
		dbName = dbNames.GetItem(i);
		this->lbDatabase->AddItem({dbName, Text::StrCharCnt(dbName)}, 0);
		i++;
	}
	this->currDB->ReleaseDatabaseNames(&dbNames);
}

void SSWR::AVIRead::AVIRDBManagerForm::UpdateSchemaList()
{
	this->lbSchema->ClearItems();
	this->lbMapSchema->ClearItems();
	if (this->currDB == 0)
	{
		return;
	}
	Data::ArrayList<Text::String*> schemaNames;
	UOSInt i;
	UOSInt j;

	this->currDB->QuerySchemaNames(&schemaNames);
	if (schemaNames.GetCount() == 0)
	{
		this->lbSchema->AddItem(CSTR(""), 0);
		this->lbMapSchema->AddItem(CSTR(""), 0);
	}
	i = 0;
	j = schemaNames.GetCount();
	while (i < j)
	{
		Text::String *schemaName = schemaNames.GetItem(i);
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
	Text::String *tableName;
	Data::ArrayList<Text::String*> tableNames;
	UOSInt i = 0;
	UOSInt j = this->currDB->QueryTableNames(STR_CSTR(schemaName), &tableNames);
	SDEL_STRING(schemaName);
	ArtificialQuickSort_Sort(&tableNames, 0, (OSInt)j - 1);
	while (i < j)
	{
		tableName = tableNames.GetItem(i);
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
	Data::ArrayList<Text::String*> tableNames;
	UOSInt i = 0;
	UOSInt j = this->currDB->QueryTableNames(STR_CSTR(schemaName), &tableNames);
	SDEL_STRING(schemaName);
	ArtificialQuickSort_Sort(&tableNames, 0, (OSInt)j - 1);
	while (i < j)
	{
		tableName = tableNames.GetItem(i);
		this->lbMapTable->AddItem(tableName, 0);
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
	Text::String *s;
	DB::TableDef *tabDef = 0;
	DB::DBReader *r;
	tabDef = this->currDB->GetTableDef(schemaName, STR_CSTR(tableName));

	r = this->currDB->QueryTableData(schemaName, STR_CSTR(tableName), 0, 0, MAX_ROW_CNT, CSTR_NULL, 0);
	if (r)
	{
		UpdateResult(r, this->lvTableResult);

		DB::ColDef *col;
		UOSInt i;
		UOSInt j;
		UOSInt k;
		if (tabDef)
		{
			j = tabDef->GetColCnt();
			i = 0;
			while (i < j)
			{
				col = tabDef->GetCol(i);
				k = this->lvTable->AddItem(col->GetColName(), 0);
				sptr = col->ToColTypeStr(sbuff);
				this->lvTable->SetSubItem(k, 1, CSTRP(sbuff, sptr));
				s = col->GetNativeType();
				if (s)
					this->lvTable->SetSubItem(k, 2, s);
				this->lvTable->SetSubItem(k, 3, col->IsNotNull()?CSTR("NOT NULL"):CSTR("NULL"));
				this->lvTable->SetSubItem(k, 4, col->IsPK()?CSTR("PK"):CSTR(""));
				this->lvTable->SetSubItem(k, 5, col->IsAutoInc()?CSTR("AUTO_INCREMENT"):CSTR(""));
				if (col->GetDefVal())
					this->lvTable->SetSubItem(k, 6, col->GetDefVal());
				if (col->GetAttr())
					this->lvTable->SetSubItem(k, 7, col->GetAttr());

				i++;
			}
			
			DEL_CLASS(tabDef);
		}
		else
		{
			NEW_CLASS(col, DB::ColDef(CSTR("")));
			j = r->ColCount();
			i = 0;
			while (i < j)
			{
				r->GetColDef(i, col);
				k = this->lvTable->AddItem(col->GetColName(), 0);
				sptr = col->ToColTypeStr(sbuff);
				this->lvTable->SetSubItem(k, 1, CSTRP(sbuff, sptr));
				s = col->GetNativeType();
				if (s)
					this->lvTable->SetSubItem(k, 2, s);
				this->lvTable->SetSubItem(k, 3, col->IsNotNull()?CSTR("NOT NULL"):CSTR("NULL"));
				this->lvTable->SetSubItem(k, 4, col->IsPK()?CSTR("PK"):CSTR(""));
				this->lvTable->SetSubItem(k, 5, col->IsAutoInc()?CSTR("AUTO_INCREMENT"):CSTR(""));
				if (col->GetDefVal())
					this->lvTable->SetSubItem(k, 6, col->GetDefVal());
				if (col->GetAttr())
					this->lvTable->SetSubItem(k, 7, col->GetAttr());

				i++;
			}
			DEL_CLASS(col);
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

void SSWR::AVIRead::AVIRDBManagerForm::UpdateResult(DB::DBReader *r, UI::GUIListView *lv)
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
			if (r->GetColDef(i, &col))
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
		r->GetStr(0, &sb);
		if (sb.GetLength() > colSize[0])
			colSize[0] = sb.GetLength();
		k = lv->AddItem(sb.ToCString(), 0);
		while (i < j)
		{
			sb.ClearStr();
			r->GetStr(i, &sb);
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
	Double w;
	Double h;
	if (k > 0)
	{
		lv->GetSize(&w, &h);
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
	UOSInt i = 0;
	UOSInt j = this->currDB->GetVariables(&vars);
	this->lvVariable->ClearItems();
	while (i < j)
	{
		item = vars.GetItem(i);
		this->lvVariable->AddItem(item.key, 0);
		if (item.value) this->lvVariable->SetSubItem(i, 1, item.value);
		i++;
	}
	this->currDB->FreeVariables(&vars);
}

void SSWR::AVIRead::AVIRDBManagerForm::UpdateSvrConnList()
{
	UTF8Char sbuff[32];
	UTF8Char *sptr;
	Data::ArrayList<DB::ReadingDBTool::ConnectionInfo*> conns;
	DB::ReadingDBTool::ConnectionInfo *conn;
	UOSInt i = 0;
	UOSInt j = this->currDB->GetConnectionInfo(&conns);
	this->lvSvrConn->ClearItems();
	while (i < j)
	{
		conn = conns.GetItem(i);
		sptr = Text::StrInt32(sbuff, conn->id);
		this->lvSvrConn->AddItem(CSTRP(sbuff, sptr), (void*)(OSInt)conn->id);
		if (conn->status) this->lvSvrConn->SetSubItem(i, 1, conn->status);
		if (conn->user) this->lvSvrConn->SetSubItem(i, 2, conn->user);
		if (conn->clientHostName) this->lvSvrConn->SetSubItem(i, 3, conn->clientHostName);
		if (conn->dbName) this->lvSvrConn->SetSubItem(i, 4, conn->dbName);
		if (conn->cmd) this->lvSvrConn->SetSubItem(i, 5, conn->cmd);
		sptr = Text::StrInt32(sbuff, conn->timeUsed);
		this->lvSvrConn->SetSubItem(i, 6, CSTRP(sbuff, sptr));
		if (conn->sql) this->lvSvrConn->SetSubItem(i, 7, conn->sql);
		i++;
	}
	this->currDB->FreeConnectionInfo(&conns);
}

Data::Class *SSWR::AVIRead::AVIRDBManagerForm::CreateTableClass(Text::CString schemaName, Text::CString tableName)
{
	if (this->currDB)
	{
		DB::TableDef *tab = this->currDB->GetTableDef(CSTR_NULL, tableName);
		if (tab)
		{
			Data::Class *cls = tab->CreateTableClass();
			DEL_CLASS(tab);
			return cls;
		}

		DB::DBReader *r = this->currDB->QueryTableData(schemaName, tableName, 0, 0, 0, CSTR_NULL, 0);
		if (r)
		{
			Data::Class *cls = r->CreateClass();
			this->currDB->CloseReader(r);
			return cls;
		}
	}
	return 0;
}

void SSWR::AVIRead::AVIRDBManagerForm::CopyTableCreate(DB::DBUtil::ServerType svrType)
{
	Text::String *schemaName = this->lbSchema->GetSelectedItemTextNew();
	Text::String *tableName = this->lbTable->GetSelectedItemTextNew();
	DB::SQLBuilder sql(svrType, 0);
	DB::TableDef *tabDef = this->currDB->GetTableDef(STR_CSTR(schemaName), tableName->ToCString());
	if (tabDef)
	{
		if (!DB::SQLGenerator::GenCreateTableCmd(&sql, STR_CSTR(schemaName), tableName->ToCString(), tabDef))
		{
			UI::MessageDialog::ShowDialog(CSTR("Error in generating Create SQL command"), CSTR("DB Manager"), this);
		}
		else
		{
			Win32::Clipboard::SetString(this->GetHandle(), sql.ToCString());
		}
		DEL_CLASS(tabDef);
	}
	else
	{
		UI::MessageDialog::ShowDialog(CSTR("Error in getting table definition"), CSTR("DB Manager"), this);
	}
	tableName->Release();
	SDEL_STRING(schemaName);
}

void SSWR::AVIRead::AVIRDBManagerForm::ExportTableData(DB::DBUtil::ServerType svrType)
{
	UTF8Char sbuff[512];
	UTF8Char *sptr;
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
	UI::FileDialog dlg(L"sswr", L"AVIRead", L"DBManagerExportTable", true);
	dlg.AddFilter(CSTR("*.sql"), CSTR("SQL File"));
	dlg.SetFileName(CSTRP(sbuff, sptr));
	if (dlg.ShowDialog(this->GetHandle()))
	{
		DB::SQLBuilder sql(svrType, 0);
		DB::DBReader *r = this->currDB->QueryTableData(STR_CSTR(schemaName), tableName->ToCString(), 0, 0, 0, CSTR_NULL, 0);
		if (r == 0)
		{
			UI::MessageDialog::ShowDialog(CSTR("Error in reading table data"), CSTR("DB Manager"), this);
			return;
		}
		IO::FileStream fs(dlg.GetFileName(), IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
		while (r->ReadNext())
		{
			sql.Clear();			
			DB::SQLGenerator::GenInsertCmd(&sql, schemaName->ToCString(), tableName->ToCString(), r);
			sql.AppendCmdC(CSTR(";\r\n"));
			fs.Write(sql.ToString(), sql.GetLength());
		}
	}
	tableName->Release();
	SDEL_STRING(schemaName);
}

SSWR::AVIRead::AVIRDBManagerForm::AVIRDBManagerForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core) : UI::GUIForm(parent, 1024, 768, ui)
{
	this->SetFont(0, 0, 8.25, false);
	this->SetText(CSTR("Database Manager"));
	this->core = core;
	this->ssl = Net::SSLEngineFactory::Create(core->GetSocketFactory(), true);
	this->currDB = 0;
	NEW_CLASS(this->mapEnv, Map::MapEnv(CSTR("DB"), 0xffc0c0ff, Math::CoordinateSystemManager::CreateGeogCoordinateSystemDefName(Math::CoordinateSystemManager::GCST_WGS84)));
	Map::IMapDrawLayer *layer = Map::BaseMapLayer::CreateLayer(Map::BaseMapLayer::BLT_OSM_TILE, this->core->GetSocketFactory(), this->ssl, this->core->GetParserList());
	this->mapEnv->AddLayer(0, layer, true);
	NEW_CLASS(this->dbLayer, Map::DBMapLayer(CSTR("Database")));
	UOSInt i = this->mapEnv->AddLayer(0, this->dbLayer, true);
	Map::MapEnv::LayerItem settings;
	this->mapEnv->GetLayerProp(&settings, 0, i);
	settings.fillStyle = 0xc0ff905b;
	this->mapEnv->SetLayerProp(&settings, 0, i);
	layer->AddUpdatedHandler(OnLayerUpdated, this);
	this->colorSess = core->GetColorMgr()->CreateSess(this->GetHMonitor());
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	NEW_CLASS(this->lbConn, UI::GUIListBox(ui, this, false));
	this->lbConn->SetRect(0, 0, 150, 23, false);
	this->lbConn->SetDockType(UI::GUIControl::DOCK_LEFT);
	this->lbConn->HandleSelectionChange(OnConnSelChg, this);
	this->lbConn->HandleRightClicked(OnConnRClicked, this);
	NEW_CLASS(this->hspConn, UI::GUIHSplitter(ui, this, 3, false));
	NEW_CLASS(this->tcMain, UI::GUITabControl(ui, this));
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
	NEW_CLASS(this->pnlTable, UI::GUIPanel(ui, this->tpTable));
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
	NEW_CLASS(this->pnlSQL, UI::GUIPanel(ui, this->tpSQL));
	this->pnlSQL->SetRect(0, 0, 100, 152, false);
	this->pnlSQL->SetDockType(UI::GUIControl::DOCK_TOP);
	NEW_CLASS(this->pnlSQLCtrl, UI::GUIPanel(ui, this->pnlSQL));
	this->pnlSQLCtrl->SetRect(0, 0, 100, 31, false);
	this->pnlSQLCtrl->SetDockType(UI::GUIControl::DOCK_BOTTOM);
	NEW_CLASS(this->btnSQLExec, UI::GUIButton(ui, this->pnlSQLCtrl, CSTR("Execute")));
	this->btnSQLExec->SetRect(4, 4, 75, 23, false);
	this->btnSQLExec->HandleButtonClick(OnSQLExecClicked, this);
	NEW_CLASS(this->txtSQL, UI::GUITextBox(ui, this->pnlSQL, CSTR(""), true));
	this->txtSQL->SetDockType(UI::GUIControl::DOCK_FILL);
	NEW_CLASS(this->vspSQL, UI::GUIVSplitter(ui, this->tpSQL, 3, false));
	NEW_CLASS(this->lvSQLResult, UI::GUIListView(ui, this->tpSQL, UI::GUIListView::LVSTYLE_TABLE, 1));
	this->lvSQLResult->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lvSQLResult->SetFullRowSelect(true);
	this->lvSQLResult->SetShowGrid(true);

	this->tpMap = this->tcMain->AddTabPage(CSTR("Map"));
	NEW_CLASS(this->pnlMap, UI::GUIPanel(ui, this->tpMap));
	this->pnlMap->SetRect(0, 0, 250, 23, false);
	this->pnlMap->SetDockType(UI::GUIControl::DOCK_LEFT);
	NEW_CLASS(this->hspMap, UI::GUIHSplitter(ui, this->tpMap, 3, false));
	NEW_CLASS(this->mapMain, UI::GUIMapControl(ui, this->tpMap, this->core->GetDrawEngine(), this->mapEnv, this->colorSess));
	this->mapMain->SetDockType(UI::GUIControl::DOCK_FILL);
	this->mapMain->HandleMouseDown(OnMapMouseDown, this);
	this->mapMain->HandleMouseUp(OnMapMouseUp, this);
	NEW_CLASS(this->pnlMapTable, UI::GUIPanel(ui, this->pnlMap));
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
	NEW_CLASS(this->pnlVariable, UI::GUIPanel(ui, this->tpVariable));
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
	NEW_CLASS(this->pnlSvrConn, UI::GUIPanel(ui, this->tpSvrConn));
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
	NEW_CLASS(this->lbLog, UI::GUIListBox(ui, this->tpLog, false));
	this->lbLog->SetDockType(UI::GUIControl::DOCK_FILL);

	NEW_CLASS(this->logger, UI::ListBoxLogger(this, this->lbLog, 100, true));
	this->log.AddLogHandler(this->logger, IO::ILogHandler::LogLevel::Raw);


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
	mnu->AddItem(CSTR("SQL Server"), MNU_TABLE_CREATE_MSSQL, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu->AddItem(CSTR("PostgreSQL"), MNU_TABLE_CREATE_POSTGRESQL, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu = this->mnuTable->AddSubMenu(CSTR("Export Table Data"));
	mnu->AddItem(CSTR("MySQL"), MNU_TABLE_EXPORT_MYSQL, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu->AddItem(CSTR("SQL Server"), MNU_TABLE_EXPORT_MSSQL, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu->AddItem(CSTR("PostgreSQL"), MNU_TABLE_EXPORT_POSTGRESQL, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu->AddItem(CSTR("Export Table Data..."), MNU_TABLE_EXPORT_OPTION, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);

	if (DB::DBManager::RestoreConn(DBCONNFILE, &this->dbList, &this->log, this->core->GetSocketFactory()))
	{
		Text::StringBuilderUTF8 sb;
		DB::DBTool *db;
		UOSInt i = 0;
		UOSInt j = this->dbList.GetCount();
		while (i < j)
		{
			db = this->dbList.GetItem(i);
			sb.ClearStr();
			db->GetConn()->GetConnName(&sb);
			this->lbConn->AddItem(sb.ToCString(), db);

			i++;
		}
		if (j > 0)
		{
			this->lbConn->SetSelectedIndex(0);
		}
	}
}

SSWR::AVIRead::AVIRDBManagerForm::~AVIRDBManagerForm()
{
	this->ClearChildren();
	this->log.RemoveLogHandler(this->logger);
	DEL_CLASS(this->logger);
	DEL_CLASS(this->mnuTable);
	DEL_CLASS(this->mnuSchema);
	DEL_CLASS(this->mnuConn);
	DEL_CLASS(this->mapEnv);
	this->core->GetColorMgr()->DeleteSess(this->colorSess);
	DB::DBManager::StoreConn(DBCONNFILE, &this->dbList);
	UOSInt i = this->dbList.GetCount();
	DB::DBTool *db;
	while (i-- > 0)
	{
		db = this->dbList.GetItem(i);
		DEL_CLASS(db);
	}
	SDEL_CLASS(this->ssl);
}

void SSWR::AVIRead::AVIRDBManagerForm::EventMenuClicked(UInt16 cmdId)
{
	UTF8Char sbuff[512];
	UTF8Char sbuff2[512];
	UTF8Char *sptr;
	switch (cmdId)
	{
	case MNU_CONN_ODBCDSN:
		{
			SSWR::AVIRead::AVIRODBCDSNForm dlg(0, this->ui, this->core);
			if (dlg.ShowDialog(this) == UI::GUIForm::DR_OK)
			{
				this->ConnAdd(dlg.GetDBConn());
			}
		}
		break;
	case MNU_CONN_ODBCSTR:
		{
			SSWR::AVIRead::AVIRODBCStrForm dlg(0, this->ui, this->core);
			if (dlg.ShowDialog(this) == UI::GUIForm::DR_OK)
			{
				this->ConnAdd(dlg.GetDBConn());
			}
		}
		break;
	case MNU_CONN_MYSQL:
		{
			SSWR::AVIRead::AVIRMySQLConnForm dlg(0, this->ui, this->core);
			if (dlg.ShowDialog(this) == UI::GUIForm::DR_OK)
			{
				this->ConnAdd(dlg.GetDBConn());
			}
		}
		break;
	case MNU_CONN_MSSQL:
		{
			SSWR::AVIRead::AVIRMSSQLConnForm dlg(0, this->ui, this->core);
			if (dlg.ShowDialog(this) == UI::GUIForm::DR_OK)
			{
				this->ConnAdd(dlg.GetDBConn());
			}
		}
		break;
	case MNU_CONN_ACCESS:
		{
			SSWR::AVIRead::AVIRAccessConnForm dlg(0, this->ui, this->core);
			if (dlg.ShowDialog(this) == UI::GUIForm::DR_OK)
			{
				this->ConnAdd(dlg.GetDBConn());
			}
		}
		break;
	case MNU_CONN_POSTGRESQL:
		{
			SSWR::AVIRead::AVIRPostgreSQLForm dlg(0, this->ui, this->core);
			if (dlg.ShowDialog(this) == UI::GUIForm::DR_OK)
			{
				this->ConnAdd(dlg.GetDBConn());
			}
		}
		break;
	case MNU_TOOL_COPY_TABLES:
		{
			SSWR::AVIRead::AVIRDBCopyTablesForm dlg(0, this->ui, this->core, &this->dbList);
			dlg.ShowDialog(this);
		}
		break;
	case MNU_CONN_REMOVE:
		{
			UOSInt i = this->lbConn->GetSelectedIndex();
			if (i != INVALID_INDEX)
			{
				DB::DBTool *db = this->dbList.RemoveAt(i);
				DEL_CLASS(db);
				this->lbConn->RemoveItem(i);
			}
		}
		break;
	case MNU_CONN_COPY_STR:
		{
			Text::StringBuilderUTF8 sb;
			DB::DBTool *db = (DB::DBTool*)this->lbConn->GetSelectedItem();
			if (db)
			{
				Text::StringBuilderUTF8 connStr;
				if (DB::DBManager::GetConnStr(db, &connStr))
				{
					Win32::Clipboard::SetString(this->GetHandle(), connStr.ToCString());
				}
				else
				{
					UI::MessageDialog::ShowDialog(CSTR("This connection is not supported"), CSTR("DB Manager"), this);
				}
			}
		}
		break;
	case MNU_SCHEMA_NEW:
		if (this->currDB)
		{
			UtilUI::TextInputDialog dlg(0, this->ui, this->core->GetMonitorMgr(), CSTR("DB Manager"), CSTR("Please enter schema name to create"));
			if (dlg.ShowDialog(this))
			{
				Text::StringBuilderUTF8 sb;
				if (dlg.GetInputString(&sb))
				{
					if (this->currDB->CreateSchema(sb.ToCString()))
					{
						this->UpdateSchemaList();
					}
					else
					{
						sb.ClearStr();
						sb.AppendC(UTF8STRC("Error in creating schema: "));
						this->currDB->GetLastErrorMsg(&sb);
						UI::MessageDialog::ShowDialog(sb.ToCString(), CSTR("DB Manager"), this);
					}
				}
			}
		}
		break;
	case MNU_SCHEMA_DELETE:
		if (this->currDB)
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
						if (this->currDB->DeleteSchema(schemaName->ToCString()))
						{
							this->UpdateSchemaList();
						}
						else
						{
							sb.ClearStr();
							sb.AppendC(UTF8STRC("Error in deleting schema "));
							sb.Append(schemaName);
							sb.AppendC(UTF8STRC(": "));
							this->currDB->GetLastErrorMsg(&sb);
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
			Text::StringBuilderUTF8 sb;
			DB::JavaDBUtil::ToJavaEntity(&sb, schemaName, tableName, this->currDB);
			tableName->Release();
			schemaName->Release();
			Win32::Clipboard::SetString(this->GetHandle(), sb.ToCString());
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
				cls->ToCppClassHeader(&hdr, 0, &sb);
				Win32::Clipboard::SetString(this->GetHandle(), sb.ToCString());
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
				cls->ToCppClassSource(0, &hdr, 0, &sb);
				Win32::Clipboard::SetString(this->GetHandle(), sb.ToCString());
				DEL_CLASS(cls);
			}
		}
		break;
	case MNU_TABLE_CREATE_MYSQL:
		this->CopyTableCreate(DB::DBUtil::ServerType::MySQL);
		break;
	case MNU_TABLE_CREATE_MSSQL:
		this->CopyTableCreate(DB::DBUtil::ServerType::MSSQL);
		break;
	case MNU_TABLE_CREATE_POSTGRESQL:
		this->CopyTableCreate(DB::DBUtil::ServerType::PostgreSQL);
		break;
	case MNU_TABLE_EXPORT_MYSQL:
		this->ExportTableData(DB::DBUtil::ServerType::MySQL);
		break;
	case MNU_TABLE_EXPORT_MSSQL:
		this->ExportTableData(DB::DBUtil::ServerType::MSSQL);
		break;
	case MNU_TABLE_EXPORT_POSTGRESQL:
		this->ExportTableData(DB::DBUtil::ServerType::PostgreSQL);
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
	}
}

void SSWR::AVIRead::AVIRDBManagerForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}

void SSWR::AVIRead::AVIRDBManagerForm::ConnAdd(DB::DBConn *conn)
{
	DB::DBTool *db;
	NEW_CLASS(db, DB::DBTool(conn, true, &this->log, CSTR("DB: ")));
	this->dbList.Add(db);
	Text::StringBuilderUTF8 sb;
	conn->GetConnName(&sb);
	this->lbConn->AddItem(sb.ToCString(), db);
}
