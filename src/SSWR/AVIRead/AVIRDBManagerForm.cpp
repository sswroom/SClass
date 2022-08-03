#include "Stdafx.h"
#include "Data/ArrayListStrUTF8.h"
#include "Data/Sort/ArtificialQuickSortCmp.h"
#include "DB/ColDef.h"
#include "DB/DBManager.h"
#include "DB/JavaDBUtil.h"
#include "Math/Math.h"
#include "SSWR/AVIRead/AVIRAccessConnForm.h"
#include "SSWR/AVIRead/AVIRDBManagerForm.h"
#include "SSWR/AVIRead/AVIRMySQLConnForm.h"
#include "SSWR/AVIRead/AVIRMSSQLConnForm.h"
#include "SSWR/AVIRead/AVIRODBCDSNForm.h"
#include "SSWR/AVIRead/AVIRODBCStrForm.h"
#include "Text/CharUtil.h"
#include "Text/MyString.h"
#include "UI/MessageDialog.h"
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

	MNU_CONN_REMOVE,
	MNU_CONN_COPY_STR,

	MNU_TABLE_JAVA,
	MNU_TABLE_CPP_HEADER,
	MNU_TABLE_CPP_SOURCE
} MenuEvent;

void __stdcall SSWR::AVIRead::AVIRDBManagerForm::OnConnSelChg(void *userObj)
{
	SSWR::AVIRead::AVIRDBManagerForm *me = (SSWR::AVIRead::AVIRDBManagerForm*)userObj;
	me->currDB = (DB::DBTool*)me->lbConn->GetSelectedItem();
	me->UpdateDatabaseList();
	me->UpdateTableList();
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

void __stdcall SSWR::AVIRead::AVIRDBManagerForm::OnTableSelChg(void *userObj)
{
	SSWR::AVIRead::AVIRDBManagerForm *me = (SSWR::AVIRead::AVIRDBManagerForm*)userObj;
	Text::String *tableName = me->lbTable->GetSelectedItemTextNew();
	me->UpdateTableData(CSTR_NULL, tableName);
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

void __stdcall SSWR::AVIRead::AVIRDBManagerForm::OnDatabaseClicked(void *userObj)
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
				me->UpdateTableList();
			}
			dbName->Release();
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

void SSWR::AVIRead::AVIRDBManagerForm::UpdateTableList()
{
	this->lbTable->ClearItems();
	if (this->currDB == 0)
	{
		return;
	}
	Text::String *tableName;
	Data::ArrayList<Text::String*> tableNames;
	UOSInt i = 0;
	UOSInt j = this->currDB->QueryTableNames(CSTR_NULL, &tableNames);
	ArtificialQuickSort_Sort(&tableNames, 0, (OSInt)j - 1);
	while (i < j)
	{
		tableName = tableNames.GetItem(i);
		this->lbTable->AddItem(tableName, 0);
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
	DB::TableDef *tabDef = 0;
	DB::DBReader *r;
	tabDef = this->currDB->GetTableDef(schemaName, STR_CSTR(tableName));

	r = this->currDB->QueryTableData(schemaName, STR_CSTR(tableName), 0, 0, MAX_ROW_CNT, CSTR_NULL, 0);
	if (r)
	{
		this->UpdateResult(r);

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
				this->lvTable->SetSubItem(k, 2, col->IsNotNull()?CSTR("NOT NULL"):CSTR("NULL"));
				this->lvTable->SetSubItem(k, 3, col->IsPK()?CSTR("PK"):CSTR(""));
				this->lvTable->SetSubItem(k, 4, col->IsAutoInc()?CSTR("AUTO_INCREMENT"):CSTR(""));
				if (col->GetDefVal())
					this->lvTable->SetSubItem(k, 5, col->GetDefVal());
				if (col->GetAttr())
					this->lvTable->SetSubItem(k, 6, col->GetAttr());

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
				this->lvTable->SetSubItem(k, 2, col->IsNotNull()?CSTR("NOT NULL"):CSTR("NULL"));
				this->lvTable->SetSubItem(k, 3, col->IsPK()?CSTR("PK"):CSTR(""));
				this->lvTable->SetSubItem(k, 4, col->IsAutoInc()?CSTR("AUTO_INCREMENT"):CSTR(""));
				if (col->GetDefVal())
					this->lvTable->SetSubItem(k, 5, col->GetDefVal());
				if (col->GetAttr())
					this->lvTable->SetSubItem(k, 6, col->GetAttr());

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

void SSWR::AVIRead::AVIRDBManagerForm::UpdateResult(DB::DBReader *r)
{
	UOSInt i;
	UOSInt j;
	UOSInt k;
	UOSInt *colSize;

	this->lvTableResult->ClearAll();

	Text::StringBuilderUTF8 sb;
	{
		DB::ColDef col(CSTR(""));
		j = r->ColCount();
		this->lvTableResult->ChangeColumnCnt(j);
		i = 0;
		colSize = MemAlloc(UOSInt, j);
		while (i < j)
		{
			if (r->GetColDef(i, &col))
			{
				this->lvTableResult->AddColumn(col.GetColName(), 100);
			}
			else
			{
				this->lvTableResult->AddColumn(CSTR("Unnamed"), 100);
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
		k = this->lvTableResult->AddItem(sb.ToCString(), 0);
		while (i < j)
		{
			sb.ClearStr();
			r->GetStr(i, &sb);
			this->lvTableResult->SetSubItem(k, i, sb.ToCString());

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
		this->lvTableResult->GetSize(&w, &h);
		w -= 20 + UOSInt2Double(j) * 6;
		if (w < 0)
			w = 0;
		i = 0;
		while (i < j)
		{
			this->lvTableResult->SetColumnWidth(i, (UOSInt2Double(colSize[i]) * w / UOSInt2Double(k) + 6));
			i++;
		}
	}
	MemFree(colSize);
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

SSWR::AVIRead::AVIRDBManagerForm::AVIRDBManagerForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core) : UI::GUIForm(parent, 1024, 768, ui)
{
	this->SetFont(0, 0, 8.25, false);
	this->SetText(CSTR("Database Manager"));
	this->core = core;
	this->currDB = 0;
	NEW_CLASS(this->dbList, Data::ArrayList<DB::DBTool*>());
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
	NEW_CLASS(this->log, IO::LogTool());

	NEW_CLASS(this->lbConn, UI::GUIListBox(ui, this, false));
	this->lbConn->SetRect(0, 0, 150, 23, false);
	this->lbConn->SetDockType(UI::GUIControl::DOCK_LEFT);
	this->lbConn->HandleSelectionChange(OnConnSelChg, this);
	this->lbConn->HandleRightClicked(OnConnRClicked, this);
	NEW_CLASS(this->hspConn, UI::GUIHSplitter(ui, this, 3, false));
	NEW_CLASS(this->tcMain, UI::GUITabControl(ui, this));
	this->tcMain->SetDockType(UI::GUIControl::DOCK_FILL);

	this->tpDetail = this->tcMain->AddTabPage(CSTR("Detail"));

	this->tpDatabase = this->tcMain->AddTabPage(CSTR("Database"));
	NEW_CLASS(this->lbDatabase, UI::GUIListBox(ui, this->tpDatabase, false));
	this->lbDatabase->SetRect(0, 0, 150, 23, false);
	this->lbDatabase->SetDockType(UI::GUIControl::DOCK_LEFT);
	NEW_CLASS(this->btnDatabase, UI::GUIButton(ui, this->tpDatabase, CSTR("Change")));
	this->btnDatabase->SetRect(154, 4, 75, 23, false);
	this->btnDatabase->HandleButtonClick(OnDatabaseClicked, this);

	this->tpTable = this->tcMain->AddTabPage(CSTR("Table"));
	NEW_CLASS(this->pnlTable, UI::GUIPanel(ui, this->tpTable));
	this->pnlTable->SetRect(0, 0, 100, 250, false);
	this->pnlTable->SetDockType(UI::GUIControl::DOCK_TOP);
	NEW_CLASS(this->lbTable, UI::GUIListBox(ui, this->pnlTable, false));
	this->lbTable->SetRect(0, 0, 150, 23, false);
	this->lbTable->SetDockType(UI::GUIControl::DOCK_LEFT);
	this->lbTable->HandleSelectionChange(OnTableSelChg, this);
	this->lbTable->HandleRightClicked(OnTableRClicked, this);
	NEW_CLASS(this->hspTable, UI::GUIHSplitter(ui, this->pnlTable, 3, false));
	NEW_CLASS(this->lvTable, UI::GUIListView(ui, this->pnlTable, UI::GUIListView::LVSTYLE_TABLE, 7));
	this->lvTable->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lvTable->SetFullRowSelect(true);
	this->lvTable->SetShowGrid(true);
	this->lvTable->AddColumn(CSTR("Name"), 200);
	this->lvTable->AddColumn(CSTR("Type"), 100);
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
	this->SetMenu(this->mnuMain);

	NEW_CLASS(this->mnuConn, UI::GUIPopupMenu());
	this->mnuConn->AddItem(CSTR("Remove"), MNU_CONN_REMOVE, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	this->mnuConn->AddItem(CSTR("Copy Conn String"), MNU_CONN_COPY_STR, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);

	NEW_CLASS(this->mnuTable, UI::GUIPopupMenu());
	this->mnuTable->AddItem(CSTR("Copy as Java Entity"), MNU_TABLE_JAVA, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	this->mnuTable->AddItem(CSTR("Copy as C++ Header"), MNU_TABLE_CPP_HEADER, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	this->mnuTable->AddItem(CSTR("Copy as C++ Source"), MNU_TABLE_CPP_SOURCE, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	if (DB::DBManager::RestoreConn(DBCONNFILE, this->dbList, this->log, this->core->GetSocketFactory()))
	{
		Text::StringBuilderUTF8 sb;
		DB::DBTool *db;
		UOSInt i = 0;
		UOSInt j = this->dbList->GetCount();
		while (i < j)
		{
			db = this->dbList->GetItem(i);
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
	DEL_CLASS(this->mnuTable);
	DEL_CLASS(this->mnuConn);
	DB::DBManager::StoreConn(DBCONNFILE, this->dbList);
	UOSInt i = this->dbList->GetCount();
	DB::DBTool *db;
	while (i-- > 0)
	{
		db = this->dbList->GetItem(i);
		DEL_CLASS(db);
	}
	DEL_CLASS(this->dbList);
	DEL_CLASS(this->log);
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
	case MNU_CONN_REMOVE:
		{
			UOSInt i = this->lbConn->GetSelectedIndex();
			if (i != INVALID_INDEX)
			{
				DB::DBTool *db = this->dbList->RemoveAt(i);
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
	case MNU_TABLE_JAVA:
		{
			Text::String *tableName = this->lbTable->GetSelectedItemTextNew();
			Text::StringBuilderUTF8 sb;
			DB::JavaDBUtil::ToJavaEntity(&sb, 0, tableName, this->currDB);
			tableName->Release();
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
	}
}

void SSWR::AVIRead::AVIRDBManagerForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}

void SSWR::AVIRead::AVIRDBManagerForm::ConnAdd(DB::DBConn *conn)
{
	DB::DBTool *db;
	NEW_CLASS(db, DB::DBTool(conn, true, this->log, CSTR("DB: ")));
	this->dbList->Add(db);
	Text::StringBuilderUTF8 sb;
	conn->GetConnName(&sb);
	this->lbConn->AddItem(sb.ToCString(), db);
}
