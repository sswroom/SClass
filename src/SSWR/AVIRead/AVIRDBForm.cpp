#include "Stdafx.h"
#include "Data/ArrayListStrUTF8.h"
#include "DB/ColDef.h"
#include "DB/JavaDBUtil.h"
#include "Math/Math.h"
#include "SSWR/AVIRead/AVIRChartForm.h"
#include "SSWR/AVIRead/AVIRDBForm.h"
#include "SSWR/AVIRead/AVIRLineChartForm.h"
#include "Text/CharUtil.h"
#include "UI/MessageDialog.h"
#include "Win32/Clipboard.h"

#define MAX_ROW_CNT 1000

typedef enum
{
	MNU_FILE_SAVE = 100,
	MNU_TABLE_CPP_HEADER,
	MNU_TABLE_CPP_SOURCE,
	MNU_TABLE_JAVA,
	MNU_CHART_LINE,
	MNU_DATABASE_START = 1000
} MenuEvent;

void __stdcall SSWR::AVIRead::AVIRDBForm::OnSchemaSelChg(void *userObj)
{
	SSWR::AVIRead::AVIRDBForm *me = (SSWR::AVIRead::AVIRDBForm*)userObj;
	me->UpdateTables();
}

void __stdcall SSWR::AVIRead::AVIRDBForm::OnTableSelChg(void *userObj)
{
	UTF8Char sbuff[512];
	UTF8Char *sptr;
	SSWR::AVIRead::AVIRDBForm *me = (SSWR::AVIRead::AVIRDBForm*)userObj;
	sptr = me->lbTable->GetSelectedItemText(sbuff);
	if (sptr == 0)
	{
		me->lvTable->ClearItems();
		return;
	}
	Text::String *schemaName = me->lbSchema->GetSelectedItemTextNew();
	

	DB::TableDef *tabDef = 0;
	DB::DBReader *r;
	if (me->dbt)
	{
		tabDef = me->dbt->GetTableDef(STR_CSTR(schemaName), CSTRP(sbuff, sptr));

		r = me->db->QueryTableData(STR_CSTR(schemaName), CSTRP(sbuff, sptr), 0, 0, MAX_ROW_CNT, CSTR_NULL, 0);
	}
	else
	{
		r = me->db->QueryTableData(STR_CSTR(schemaName), CSTRP(sbuff, sptr), 0, 0, MAX_ROW_CNT, CSTR_NULL, 0);
		if (r)
		{
			tabDef = r->GenTableDef(CSTRP(sbuff, sptr));
		}
	}
	SDEL_STRING(schemaName);
	if (r)
	{
		me->UpdateResult(r);

		me->lvTable->ClearItems();
		DB::ColDef *col;
		Text::String *s;
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
				k = me->lvTable->AddItem(col->GetColName(), 0);
				sptr = col->ToColTypeStr(sbuff);
				me->lvTable->SetSubItem(k, 1, CSTRP(sbuff, sptr));
				s = col->GetNativeType();
				if (s)
					me->lvTable->SetSubItem(k, 2, s->ToCString());
				me->lvTable->SetSubItem(k, 3, col->IsNotNull()?CSTR("NOT NULL"):CSTR("NULL"));
				me->lvTable->SetSubItem(k, 4, col->IsPK()?CSTR("PK"):CSTR(""));
				me->lvTable->SetSubItem(k, 5, col->IsAutoInc()?CSTR("AUTO_INCREMENT"):CSTR(""));
				if (col->GetDefVal())
					me->lvTable->SetSubItem(k, 6, col->GetDefVal());
				if (col->GetAttr())
					me->lvTable->SetSubItem(k, 7, col->GetAttr());

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
				k = me->lvTable->AddItem(col->GetColName(), 0);
				sptr = col->ToColTypeStr(sbuff);
				me->lvTable->SetSubItem(k, 1, CSTRP(sbuff, sptr));
				s = col->GetNativeType();
				if (s)
					me->lvTable->SetSubItem(k, 2, s->ToCString());
				me->lvTable->SetSubItem(k, 3, col->IsNotNull()?CSTR("NOT NULL"):CSTR("NULL"));
				me->lvTable->SetSubItem(k, 4, col->IsPK()?CSTR("PK"):CSTR(""));
				me->lvTable->SetSubItem(k, 5, col->IsAutoInc()?CSTR("AUTO_INCREMENT"):CSTR(""));
				if (col->GetDefVal())
					me->lvTable->SetSubItem(k, 6, col->GetDefVal());
				if (col->GetAttr())
					me->lvTable->SetSubItem(k, 7, col->GetAttr());

				i++;
			}
			DEL_CLASS(col);
		}

		me->db->CloseReader(r);
	}
	else
	{
		if (tabDef)
		{
			DEL_CLASS(tabDef);
		}
	}
}

void __stdcall SSWR::AVIRead::AVIRDBForm::OnSQLClicked(void *userObj)
{
	SSWR::AVIRead::AVIRDBForm *me = (SSWR::AVIRead::AVIRDBForm*)userObj;
	Text::StringBuilderUTF8 sb;
	me->txtSQL->GetText(&sb);
	if (sb.GetLength() > 0)
	{
		DB::DBReader *r = me->dbt->ExecuteReader(sb.ToCString());
		if (r)
		{
			me->UpdateResult(r);
			me->dbt->CloseReader(r);
		}
		else
		{
			sb.ClearStr();
			me->dbt->GetLastErrorMsg(&sb);
			UI::MessageDialog::ShowDialog(sb.ToCString(), CSTR("Database"), me);
		}
	}
}

void SSWR::AVIRead::AVIRDBForm::UpdateResult(DB::DBReader *r)
{
	UOSInt i;
	UOSInt j;
	UOSInt k;
	UOSInt *colSize;

	this->lvResult->ClearAll();

	Text::StringBuilderUTF8 sb;
	{
		DB::ColDef col(CSTR(""));
		j = r->ColCount();
		this->lvResult->ChangeColumnCnt(j);
		i = 0;
		colSize = MemAlloc(UOSInt, j);
		while (i < j)
		{
			if (r->GetColDef(i, &col))
			{
				this->lvResult->AddColumn(col.GetColName(), 100);
			}
			else
			{
				this->lvResult->AddColumn(CSTR("Unnamed"), 100);
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
		k = this->lvResult->AddItem(sb.ToCString(), 0);
		while (i < j)
		{
			sb.ClearStr();
			r->GetStr(i, &sb);
			this->lvResult->SetSubItem(k, i, sb.ToCString());

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
		this->lvResult->GetSize(&w, &h);
		w -= UOSInt2Double(20 + j * 6);
		if (w < 0)
			w = 0;
		i = 0;
		while (i < j)
		{
			this->lvResult->SetColumnWidth(i, (UOSInt2Double(colSize[i]) * w / UOSInt2Double(k) + 6));
			i++;
		}
	}
	MemFree(colSize);
}

Data::Class *SSWR::AVIRead::AVIRDBForm::CreateTableClass(Text::CString schemaName, Text::CString tableName)
{
	if (this->dbt)
	{
		DB::TableDef *tab = this->dbt->GetTableDef(schemaName, tableName);
		if (tab)
		{
			Data::Class *cls = tab->CreateTableClass();
			DEL_CLASS(tab);
			return cls;
		}
	}
	DB::DBReader *r = this->db->QueryTableData(schemaName, tableName, 0, 0, 0, CSTR_NULL, 0);
	if (r)
	{
		Data::Class *cls = r->CreateClass();
		this->db->CloseReader(r);
		return cls;
	}
	return 0;
}

SSWR::AVIRead::AVIRDBForm::AVIRDBForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core, DB::ReadingDB *db, Bool needRelease) : UI::GUIForm(parent, 1024, 768, ui)
{
	UTF8Char sbuff[512];
	UTF8Char *sptr;
	this->SetFont(0, 0, 8.25, false);
	sptr = db->GetSourceNameObj()->ConcatTo(Text::StrConcatC(sbuff, UTF8STRC("Database - ")));
	this->SetText(CSTRP(sbuff, sptr));
	this->core = core;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
	this->db = db;
	this->needRelease = needRelease;
	this->dbt = 0;
	if (db->IsFullConn())
	{
		NEW_CLASS(this->dbt, DB::ReadingDBTool((DB::DBConn*)this->db, needRelease, &this->log, CSTR("DB: ")));
	}

	NEW_CLASS(this->tcDB, UI::GUITabControl(ui, this));
	this->tcDB->SetRect(0, 0, 100, 400, false);
	this->tcDB->SetDockType(UI::GUIControl::DOCK_TOP);
	NEW_CLASS(this->vspDB, UI::GUIVSplitter(ui, this, 3, false));
	NEW_CLASS(this->lvResult, UI::GUIListView(ui, this, UI::GUIListView::LVSTYLE_TABLE, 1));
	this->lvResult->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lvResult->SetFullRowSelect(true);
	this->lvResult->SetShowGrid(true);

	this->tpTable = this->tcDB->AddTabPage(CSTR("Tables"));
	NEW_CLASS(this->lbSchema, UI::GUIListBox(ui, this->tpTable, false));
	this->lbSchema->SetRect(0, 0, 150, 100, false);
	this->lbSchema->SetDockType(UI::GUIControl::DOCK_LEFT);
	this->lbSchema->HandleSelectionChange(OnSchemaSelChg, this);
	NEW_CLASS(this->hspSchema, UI::GUIHSplitter(ui, this->tpTable, 3, false));
	NEW_CLASS(this->lbTable, UI::GUIListBox(ui, this->tpTable, false));
	this->lbTable->SetRect(0, 0, 150, 100, false);
	this->lbTable->SetDockType(UI::GUIControl::DOCK_LEFT);
	this->lbTable->HandleSelectionChange(OnTableSelChg, this);
	NEW_CLASS(this->hspTable, UI::GUIHSplitter(ui, this->tpTable, 3, false));
	NEW_CLASS(this->lvTable, UI::GUIListView(ui, this->tpTable, UI::GUIListView::LVSTYLE_TABLE, 8));
	this->lvTable->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lvTable->SetFullRowSelect(true);
	this->lvTable->AddColumn(CSTR("Name"), 200);
	this->lvTable->AddColumn(CSTR("Type"), 100);
	this->lvTable->AddColumn(CSTR("NType"), 100);
	this->lvTable->AddColumn(CSTR("Null?"), 50);
	this->lvTable->AddColumn(CSTR("PK?"), 30);
	this->lvTable->AddColumn(CSTR("Auto_Inc"), 100);
	this->lvTable->AddColumn(CSTR("Default Val"), 100);
	this->lvTable->AddColumn(CSTR("Attribute"), 100);

	if (this->dbt)
	{
		this->tpSQL = this->tcDB->AddTabPage(CSTR("SQL"));
		NEW_CLASS(this->pnlSQLCtrl, UI::GUIPanel(ui, this->tpSQL));
		this->pnlSQLCtrl->SetRect(0, 0, 100, 31, false);
		this->pnlSQLCtrl->SetDockType(UI::GUIControl::DOCK_BOTTOM);
		NEW_CLASS(this->btnSQL, UI::GUIButton(ui, this->pnlSQLCtrl, CSTR("Execute")));
		this->btnSQL->SetRect(4, 4, 75, 23, false);
		this->btnSQL->HandleButtonClick(OnSQLClicked, this);
		NEW_CLASS(this->txtSQL, UI::GUITextBox(ui, this->tpSQL, CSTR(""), true));
		this->txtSQL->SetDockType(UI::GUIControl::DOCK_FILL);
	}

	UI::GUIMenu *mnu;
	NEW_CLASS(this->mnuMain, UI::GUIMainMenu());
	mnu = this->mnuMain->AddSubMenu(CSTR("&File"));
	mnu->AddItem(CSTR("&Save as"), MNU_FILE_SAVE, UI::GUIMenu::KM_CONTROL, UI::GUIControl::GK_S);
	mnu = this->mnuMain->AddSubMenu(CSTR("&Table"));
	mnu->AddItem(CSTR("Copy as CPP Header"), MNU_TABLE_CPP_HEADER, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu->AddItem(CSTR("Copy as CPP Source"), MNU_TABLE_CPP_SOURCE, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu->AddItem(CSTR("Copy as Java Entity"), MNU_TABLE_JAVA, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	mnu = this->mnuMain->AddSubMenu(CSTR("&Chart"));
	mnu->AddItem(CSTR("&Line Chart"), MNU_CHART_LINE, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	if (this->dbt && this->dbt->GetDatabaseNames(&this->dbNames) > 0)
	{
		UOSInt i = 0;
		UOSInt j = this->dbNames.GetCount();
		mnu = this->mnuMain->AddSubMenu(CSTR("&Database"));
		while (i < j)
		{
			const UTF8Char *dbName = this->dbNames.GetItem(i);
			mnu->AddItem({dbName, Text::StrCharCnt(dbName)}, (UInt16)(MNU_DATABASE_START + i), UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
			i++;
		}
	}
	this->SetMenu(this->mnuMain);

	this->UpdateSchemas();
}

SSWR::AVIRead::AVIRDBForm::~AVIRDBForm()
{
	if (this->dbt)
	{
		this->dbt->ReleaseDatabaseNames(&this->dbNames);
		DEL_CLASS(this->dbt);
	}
	else if (this->needRelease)
	{
		DEL_CLASS(this->db);
	}
}

void SSWR::AVIRead::AVIRDBForm::UpdateSchemas()
{
	Data::ArrayList<Text::String*> schemaNames;
	UOSInt i;
	UOSInt j;

	this->lbSchema->ClearItems();
	if (this->dbt)
	{
		this->dbt->QuerySchemaNames(&schemaNames);
	}
	else
	{
		this->db->QuerySchemaNames(&schemaNames);
	}
	if (schemaNames.GetCount() == 0)
	{
		this->lbSchema->AddItem(CSTR(""), 0);
	}
	i = 0;
	j = schemaNames.GetCount();
	while (i < j)
	{
		Text::String *schemaName = schemaNames.GetItem(i);
		this->lbSchema->AddItem(schemaName, 0);
		i++;
	}

	LIST_FREE_STRING(&schemaNames);
	this->lbSchema->SetSelectedIndex(0);
}

void SSWR::AVIRead::AVIRDBForm::UpdateTables()
{
	Text::StringBuilderUTF8 sb;
	Text::String *schemaName = this->lbSchema->GetSelectedItemTextNew();
	Data::ArrayList<Text::String*> tableNames;
	UOSInt i;
	UOSInt j;

	this->lbTable->ClearItems();
	if (this->dbt)
	{
		this->dbt->QueryTableNames(STR_CSTR(schemaName), &tableNames);
	}
	else
	{
		this->db->QueryTableNames(STR_CSTR(schemaName), &tableNames);
	}
	SDEL_STRING(schemaName);
	i = 0;
	j = tableNames.GetCount();
	while (i < j)
	{
		Text::String *tableName = tableNames.GetItem(i);
		this->lbTable->AddItem(tableName, 0);
		i++;
	}

	LIST_FREE_STRING(&tableNames);
}

void SSWR::AVIRead::AVIRDBForm::EventMenuClicked(UInt16 cmdId)
{
	UTF8Char sbuff[512];
	UTF8Char sbuff2[512];
	UTF8Char *sptr;
	if (cmdId >= MNU_DATABASE_START)
	{
		if (this->dbt->ChangeDatabase(this->dbNames.GetItem((UOSInt)cmdId - MNU_DATABASE_START)))
		{
			this->UpdateTables();
		}
		return;
	}
	switch (cmdId)
	{
	case MNU_FILE_SAVE:
		this->core->SaveData(this, this->db, L"DBSave");
		break;
	case MNU_CHART_LINE:
		if ((sptr = this->lbTable->GetSelectedItemText(sbuff)) != 0)
		{
			Data::Chart *chart = 0;
			{
				Text::String *schemaName = this->lbSchema->GetSelectedItemTextNew();
				SSWR::AVIRead::AVIRLineChartForm frm(0, this->ui, this->core, this->db, STR_CSTR(schemaName), CSTRP(sbuff, sptr));
				SDEL_STRING(schemaName);
				if (frm.ShowDialog(this) == DR_OK)
				{
					chart = frm.GetChart();
				}
			}
			if (chart)
			{
				SSWR::AVIRead::AVIRChartForm *chartFrm;
				NEW_CLASS(chartFrm, SSWR::AVIRead::AVIRChartForm(0, this->ui, this->core, chart));
				this->core->ShowForm(chartFrm);
			}
		}
		break;
	case MNU_TABLE_CPP_HEADER:
		if ((sptr = this->lbTable->GetSelectedItemText(sbuff)) != 0)
		{
			Text::String *schemaName = this->lbSchema->GetSelectedItemTextNew();
			Data::Class *cls = this->CreateTableClass(STR_CSTR(schemaName), CSTRP(sbuff, sptr));
			SDEL_STRING(schemaName);
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
			Text::String *schemaName = this->lbSchema->GetSelectedItemTextNew();
			Data::Class *cls = this->CreateTableClass(STR_CSTR(schemaName), CSTRP(sbuff, sptr));
			SDEL_STRING(schemaName);
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
	case MNU_TABLE_JAVA:
		{
			Text::String *schemaName = this->lbSchema->GetSelectedItemTextNew();
			Text::String *tableName = this->lbTable->GetSelectedItemTextNew();
			Text::StringBuilderUTF8 sb;
			DB::JavaDBUtil::ToJavaEntity(&sb, schemaName, tableName, this->dbt);
			tableName->Release();
			SDEL_STRING(schemaName);
			Win32::Clipboard::SetString(this->GetHandle(), sb.ToCString());
		}
		break;
	}
}

void SSWR::AVIRead::AVIRDBForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
