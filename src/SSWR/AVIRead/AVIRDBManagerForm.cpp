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
#define DBCONNFILE ((const UTF8Char*)"DBConn.dat")

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

Bool __stdcall SSWR::AVIRead::AVIRDBManagerForm::OnConnRClicked(void *userObj, OSInt scnX, OSInt scnY, MouseButton btn)
{
	SSWR::AVIRead::AVIRDBManagerForm *me = (SSWR::AVIRead::AVIRDBManagerForm*)userObj;
	UOSInt i = me->lbConn->GetSelectedIndex();
	if (i != INVALID_INDEX)
	{
		me->mnuConn->ShowMenu(me->lbConn, scnX, scnY);
	}
	return false;
}

void __stdcall SSWR::AVIRead::AVIRDBManagerForm::OnTableSelChg(void *userObj)
{
	SSWR::AVIRead::AVIRDBManagerForm *me = (SSWR::AVIRead::AVIRDBManagerForm*)userObj;
	Text::String *tableName = me->lbTable->GetSelectedItemTextNew();
	me->UpdateTableData(tableName);
	SDEL_STRING(tableName);
}

Bool __stdcall SSWR::AVIRead::AVIRDBManagerForm::OnTableRClicked(void *userObj, OSInt scnX, OSInt scnY, MouseButton btn)
{
	SSWR::AVIRead::AVIRDBManagerForm *me = (SSWR::AVIRead::AVIRDBManagerForm*)userObj;
	UOSInt i = me->lbTable->GetSelectedIndex();
	if (i != INVALID_INDEX)
	{
		me->mnuTable->ShowMenu(me->lbTable, scnX, scnY);
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
				me->UpdateTableData(0);
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
		this->lbDatabase->AddItem(dbName, 0);
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
	const UTF8Char *tableName;
	Data::ArrayList<const UTF8Char*> tableNames;
	UOSInt i = 0;
	UOSInt j = this->currDB->GetTableNames(&tableNames);
	ArtificialQuickSort_Sort(&tableNames, 0, (OSInt)j - 1);
	while (i < j)
	{
		tableName = tableNames.GetItem(i);
		this->lbTable->AddItem(tableName, 0);
		i++;
	}
	this->currDB->ReleaseTableNames(&tableNames);
}

void SSWR::AVIRead::AVIRDBManagerForm::UpdateTableData(Text::String *tableName)
{
	this->lvTable->ClearItems();
	this->lvTableResult->ClearItems();
	if (tableName == 0 || this->currDB == 0)
	{
		return;
	}

	UTF8Char sbuff[256];
	DB::TableDef *tabDef = 0;
	DB::DBReader *r;
	tabDef = this->currDB->GetTableDef(tableName?tableName->v:0);

	r = this->currDB->GetTableData(tableName?tableName->v:0, 0, 0, MAX_ROW_CNT, 0, 0);
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
				col->ToColTypeStr(sbuff);
				this->lvTable->SetSubItem(k, 1, sbuff);
				this->lvTable->SetSubItem(k, 2, col->IsNotNull()?(const UTF8Char*)"NOT NULL":(const UTF8Char*)"NULL");
				this->lvTable->SetSubItem(k, 3, col->IsPK()?(const UTF8Char*)"PK":(const UTF8Char*)"");
				this->lvTable->SetSubItem(k, 4, col->IsAutoInc()?(const UTF8Char*)"AUTO_INCREMENT":(const UTF8Char*)"");
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
			NEW_CLASS(col, DB::ColDef((const UTF8Char*)""));
			j = r->ColCount();
			i = 0;
			while (i < j)
			{
				r->GetColDef(i, col);
				k = this->lvTable->AddItem(col->GetColName(), 0);
				col->ToColTypeStr(sbuff);
				this->lvTable->SetSubItem(k, 1, sbuff);
				this->lvTable->SetSubItem(k, 2, col->IsNotNull()?(const UTF8Char*)"NOT NULL":(const UTF8Char*)"NULL");
				this->lvTable->SetSubItem(k, 3, col->IsPK()?(const UTF8Char*)"PK":(const UTF8Char*)"");
				this->lvTable->SetSubItem(k, 4, col->IsAutoInc()?(const UTF8Char*)"AUTO_INCREMENT":(const UTF8Char*)"");
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
	DB::ColDef *col;
	Text::StringBuilderUTF8 *sb;
	UOSInt *colSize;

	this->lvTableResult->ClearAll();

	NEW_CLASS(sb, Text::StringBuilderUTF8());
	NEW_CLASS(col, DB::ColDef((const UTF8Char*)""));
	j = r->ColCount();
	this->lvTableResult->ChangeColumnCnt(j);
	i = 0;
	colSize = MemAlloc(UOSInt, j);
	while (i < j)
	{
		if (r->GetColDef(i, col))
		{
			this->lvTableResult->AddColumn(col->GetColName(), 100);
		}
		else
		{
			this->lvTableResult->AddColumn((const UTF8Char*)"Unnamed", 100);
		}
		colSize[i] = 0;
		i++;
	}
	DEL_CLASS(col);

	OSInt rowCnt = 0;
	while (r->ReadNext())
	{
		i = 1;
		sb->ClearStr();
		r->GetStr(0, sb);
		if (sb->GetLength() > colSize[0])
			colSize[0] = sb->GetLength();
		k = this->lvTableResult->AddItem(sb->ToString(), 0);
		while (i < j)
		{
			sb->ClearStr();
			r->GetStr(i, sb);
			this->lvTableResult->SetSubItem(k, i, sb->ToString());

			if (sb->GetLength() > colSize[i])
				colSize[i] = sb->GetLength();
			i++;
		}
		rowCnt++;
		if (rowCnt > MAX_ROW_CNT)
			break;
	}
	DEL_CLASS(sb);

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

Data::Class *SSWR::AVIRead::AVIRDBManagerForm::CreateTableClass(const UTF8Char *tableName)
{
	if (this->currDB)
	{
		DB::TableDef *tab = this->currDB->GetTableDef(tableName);
		if (tab)
		{
			Data::Class *cls = tab->CreateTableClass();
			DEL_CLASS(tab);
			return cls;
		}

		DB::DBReader *r = this->currDB->GetTableData(tableName, 0, 0, 0, 0, 0);
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
	this->SetText((const UTF8Char*)"Database Manager");
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

	this->tpDetail = this->tcMain->AddTabPage((const UTF8Char*)"Detail");

	this->tpDatabase = this->tcMain->AddTabPage((const UTF8Char*)"Database");
	NEW_CLASS(this->lbDatabase, UI::GUIListBox(ui, this->tpDatabase, false));
	this->lbDatabase->SetRect(0, 0, 150, 23, false);
	this->lbDatabase->SetDockType(UI::GUIControl::DOCK_LEFT);
	NEW_CLASS(this->btnDatabase, UI::GUIButton(ui, this->tpDatabase, (const UTF8Char*)"Change"));
	this->btnDatabase->SetRect(154, 4, 75, 23, false);
	this->btnDatabase->HandleButtonClick(OnDatabaseClicked, this);

	this->tpTable = this->tcMain->AddTabPage((const UTF8Char*)"Table");
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
	this->lvTable->AddColumn((const UTF8Char*)"Name", 200);
	this->lvTable->AddColumn((const UTF8Char*)"Type", 100);
	this->lvTable->AddColumn((const UTF8Char*)"Null?", 100);
	this->lvTable->AddColumn((const UTF8Char*)"PK?", 30);
	this->lvTable->AddColumn((const UTF8Char*)"Auto_Inc", 100);
	this->lvTable->AddColumn((const UTF8Char*)"Default Val", 100);
	this->lvTable->AddColumn((const UTF8Char*)"Attribute", 100);
	NEW_CLASS(this->vspTable, UI::GUIVSplitter(ui, this->tpTable, 3, false));
	NEW_CLASS(this->lvTableResult, UI::GUIListView(ui, this->tpTable, UI::GUIListView::LVSTYLE_TABLE, 1));
	this->lvTableResult->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lvTableResult->SetFullRowSelect(true);
	this->lvTableResult->SetShowGrid(true);

	UI::GUIMenu *mnu;
	UI::GUIMenu *mnu2;
	NEW_CLASS(this->mnuMain, UI::GUIMainMenu());
	mnu = this->mnuMain->AddSubMenu((const UTF8Char*)"&Connection");
	mnu2 = mnu->AddSubMenu((const UTF8Char*)"&New Connection");
	mnu2->AddItem((const UTF8Char*)"ODBC DSN", MNU_CONN_ODBCDSN, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu2->AddItem((const UTF8Char*)"ODBC Connection String", MNU_CONN_ODBCSTR, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu2->AddItem((const UTF8Char*)"MySQL TCP", MNU_CONN_MYSQL, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu2->AddItem((const UTF8Char*)"SQL Server", MNU_CONN_MSSQL, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu2->AddItem((const UTF8Char*)"Access File", MNU_CONN_ACCESS, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	this->SetMenu(this->mnuMain);

	NEW_CLASS(this->mnuConn, UI::GUIPopupMenu());
	this->mnuConn->AddItem((const UTF8Char*)"Remove", MNU_CONN_REMOVE, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	this->mnuConn->AddItem((const UTF8Char*)"Copy Conn String", MNU_CONN_COPY_STR, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);

	NEW_CLASS(this->mnuTable, UI::GUIPopupMenu());
	this->mnuTable->AddItem((const UTF8Char*)"Copy as Java Entity", MNU_TABLE_JAVA, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	this->mnuTable->AddItem((const UTF8Char*)"Copy as C++ Header", MNU_TABLE_CPP_HEADER, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	this->mnuTable->AddItem((const UTF8Char*)"Copy as C++ Source", MNU_TABLE_CPP_SOURCE, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
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
			this->lbConn->AddItem(sb.ToString(), db);

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
	switch (cmdId)
	{
	case MNU_CONN_ODBCDSN:
		{
			SSWR::AVIRead::AVIRODBCDSNForm *dlg;
			NEW_CLASS(dlg, SSWR::AVIRead::AVIRODBCDSNForm(0, this->ui, this->core));
			if (dlg->ShowDialog(this) == UI::GUIForm::DR_OK)
			{
				this->ConnAdd(dlg->GetDBConn());
			}
			DEL_CLASS(dlg);
		}
		break;
	case MNU_CONN_ODBCSTR:
		{
			SSWR::AVIRead::AVIRODBCStrForm *dlg;
			NEW_CLASS(dlg, SSWR::AVIRead::AVIRODBCStrForm(0, this->ui, this->core));
			if (dlg->ShowDialog(this) == UI::GUIForm::DR_OK)
			{
				this->ConnAdd(dlg->GetDBConn());
			}
			DEL_CLASS(dlg);
		}
		break;
	case MNU_CONN_MYSQL:
		{
			SSWR::AVIRead::AVIRMySQLConnForm *dlg;
			NEW_CLASS(dlg, SSWR::AVIRead::AVIRMySQLConnForm(0, this->ui, this->core));
			if (dlg->ShowDialog(this) == UI::GUIForm::DR_OK)
			{
				this->ConnAdd(dlg->GetDBConn());
			}
			DEL_CLASS(dlg);
		}
		break;
	case MNU_CONN_MSSQL:
		{
			SSWR::AVIRead::AVIRMSSQLConnForm *dlg;
			NEW_CLASS(dlg, SSWR::AVIRead::AVIRMSSQLConnForm(0, this->ui, this->core));
			if (dlg->ShowDialog(this) == UI::GUIForm::DR_OK)
			{
				this->ConnAdd(dlg->GetDBConn());
			}
			DEL_CLASS(dlg);
		}
		break;
	case MNU_CONN_ACCESS:
		{
			SSWR::AVIRead::AVIRAccessConnForm *dlg;
			NEW_CLASS(dlg, SSWR::AVIRead::AVIRAccessConnForm(0, this->ui, this->core));
			if (dlg->ShowDialog(this) == UI::GUIForm::DR_OK)
			{
				this->ConnAdd(dlg->GetDBConn());
			}
			DEL_CLASS(dlg);
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
					Win32::Clipboard::SetString(this->GetHandle(), connStr.ToString());
				}
				else
				{
					UI::MessageDialog::ShowDialog((const UTF8Char*)"This connection is not supported", (const UTF8Char*)"DB Manager", this);
				}
			}
		}
		break;
	case MNU_TABLE_JAVA:
		{
			Text::String *tableName = this->lbTable->GetSelectedItemTextNew();
			Text::StringBuilderUTF8 sb;
			DB::JavaDBUtil::ToJavaEntity(&sb, tableName, this->currDB);
			tableName->Release();
			Win32::Clipboard::SetString(this->GetHandle(), sb.ToString());
		}
		break;
	case MNU_TABLE_CPP_HEADER:
		if (this->lbTable->GetSelectedItemText(sbuff))
		{
			Data::Class *cls = this->CreateTableClass(sbuff);
			if (cls)
			{
				UOSInt i = Text::StrLastIndexOfChar(sbuff, '.');
				DB::DBUtil::DB2FieldName(sbuff2, &sbuff[i + 1]);
				sbuff2[0] = Text::CharUtil::ToUpper(sbuff2[0]);
				Text::StringBuilderUTF8 sb;
				cls->ToCppClassHeader(sbuff2, 0, &sb);
				Win32::Clipboard::SetString(this->GetHandle(), sb.ToString());
				DEL_CLASS(cls);
			}
		}
		break;
	case MNU_TABLE_CPP_SOURCE:
		if (this->lbTable->GetSelectedItemText(sbuff))
		{
			Data::Class *cls = this->CreateTableClass(sbuff);
			if (cls)
			{
				UOSInt i = Text::StrLastIndexOfChar(sbuff, '.');
				DB::DBUtil::DB2FieldName(sbuff2, &sbuff[i + 1]);
				sbuff2[0] = Text::CharUtil::ToUpper(sbuff2[0]);
				Text::StringBuilderUTF8 sb;
				cls->ToCppClassSource(0, sbuff2, 0, &sb);
				Win32::Clipboard::SetString(this->GetHandle(), sb.ToString());
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
	NEW_CLASS(db, DB::DBTool(conn, true, this->log, (const UTF8Char*)"DB: "));
	this->dbList->Add(db);
	Text::StringBuilderUTF8 sb;
	conn->GetConnName(&sb);
	this->lbConn->AddItem(sb.ToString(), db);
}
