#include "Stdafx.h"
#include "Data/ArrayListStrUTF8.h"
#include "DB/ColDef.h"
#include "SSWR/AVIRead/AVIRChartForm.h"
#include "SSWR/AVIRead/AVIRDBForm.h"
#include "SSWR/AVIRead/AVIRLineChartForm.h"

#define MAX_ROW_CNT 1000

typedef enum
{
	MNU_FILE_SAVE = 100,
	MNU_CHART_LINE,
	MNU_DATABASE_START = 1000
} MenuEvent;

void __stdcall SSWR::AVIRead::AVIRDBForm::OnTableSelChg(void *userObj)
{
	UTF8Char sbuff[512];
	SSWR::AVIRead::AVIRDBForm *me = (SSWR::AVIRead::AVIRDBForm*)userObj;
	me->lbTable->GetSelectedItemText(sbuff);

	DB::TableDef *tabDef = 0;
	DB::DBReader *r;
	if (me->dbt)
	{
		tabDef = me->dbt->GetTableDef(sbuff);

		r = me->db->GetTableData(sbuff, MAX_ROW_CNT, 0, 0);
	}
	else
	{
		r = me->db->GetTableData(sbuff, MAX_ROW_CNT, 0, 0);
		if (r)
		{
			tabDef = r->GenTableDef(sbuff);
		}
	}
	if (r)
	{
		me->UpdateResult(r);

		me->lvTable->ClearItems();
		DB::ColDef *col;
		OSInt i;
		OSInt j;
		OSInt k;
		if (tabDef)
		{
			j = tabDef->GetColCnt();
			i = 0;
			while (i < j)
			{
				col = tabDef->GetCol(i);
				k = me->lvTable->AddItem(col->GetColName(), 0);
				col->ToColTypeStr(sbuff);
				me->lvTable->SetSubItem(k, 1, sbuff);
				me->lvTable->SetSubItem(k, 2, col->IsNotNull()?(const UTF8Char*)"NOT NULL":(const UTF8Char*)"NULL");
				me->lvTable->SetSubItem(k, 3, col->IsPK()?(const UTF8Char*)"PK":(const UTF8Char*)"");
				me->lvTable->SetSubItem(k, 4, col->IsAutoInc()?(const UTF8Char*)"AUTO_INCREMENT":(const UTF8Char*)"");
				if (col->GetDefVal())
					me->lvTable->SetSubItem(k, 5, col->GetDefVal());
				if (col->GetAttr())
					me->lvTable->SetSubItem(k, 6, col->GetAttr());

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
				k = me->lvTable->AddItem(col->GetColName(), 0);
				col->ToColTypeStr(sbuff);
				me->lvTable->SetSubItem(k, 1, sbuff);
				me->lvTable->SetSubItem(k, 2, col->IsNotNull()?(const UTF8Char*)"NOT NULL":(const UTF8Char*)"NULL");
				me->lvTable->SetSubItem(k, 3, col->IsPK()?(const UTF8Char*)"PK":(const UTF8Char*)"");
				me->lvTable->SetSubItem(k, 4, col->IsAutoInc()?(const UTF8Char*)"AUTO_INCREMENT":(const UTF8Char*)"");
				if (col->GetDefVal())
					me->lvTable->SetSubItem(k, 5, col->GetDefVal());
				if (col->GetAttr())
					me->lvTable->SetSubItem(k, 6, col->GetAttr());

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

void SSWR::AVIRead::AVIRDBForm::UpdateResult(DB::DBReader *r)
{
	OSInt i;
	OSInt j;
	UOSInt k;
	DB::ColDef *col;
	Text::StringBuilderUTF8 *sb;
	UOSInt *colSize;

	this->lvResult->ClearAll();

	NEW_CLASS(sb, Text::StringBuilderUTF8());
	NEW_CLASS(col, DB::ColDef((const UTF8Char*)""));
	j = r->ColCount();
	this->lvResult->ChangeColumnCnt(j);
	i = 0;
	colSize = MemAlloc(UOSInt, j);
	while (i < j)
	{
		if (r->GetColDef(i, col))
		{
			this->lvResult->AddColumn(col->GetColName(), 100);
		}
		else
		{
			this->lvResult->AddColumn(L"Unnamed", 100);
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
		k = this->lvResult->AddItem(sb->ToString(), 0);
		while (i < j)
		{
			sb->ClearStr();
			r->GetStr(i, sb);
			this->lvResult->SetSubItem(k, i, sb->ToString());

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
		this->lvResult->GetSize(&w, &h);
		w -= 20 + j * 6;
		if (w < 0)
			w = 0;
		i = 0;
		while (i < j)
		{
			this->lvResult->SetColumnWidth(i, (colSize[i] * w / k + 6));
			i++;
		}
	}
	MemFree(colSize);
}

SSWR::AVIRead::AVIRDBForm::AVIRDBForm(UI::GUIClientControl *parent, UI::GUICore *ui, SSWR::AVIRead::AVIRCore *core, DB::ReadingDB *db, Bool needRelease) : UI::GUIForm(parent, 1024, 768, ui)
{
	UTF8Char sbuff[512];
	this->SetFont(0, 8.25, false);
	Text::StrConcat(Text::StrConcat(sbuff, (const UTF8Char*)"Database - "), db->GetSourceNameObj());
	this->SetText(sbuff);
	this->core = core;
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
	NEW_CLASS(this->log, IO::LogTool());
	this->db = db;
	this->needRelease = needRelease;
	this->dbt = 0;
	if (db->IsFullConn())
	{
		NEW_CLASS(this->dbt, DB::ReadingDBTool((DB::DBConn*)this->db, needRelease, this->log, (const UTF8Char*)"DB: "));
	}

	NEW_CLASS(this->tcDB, UI::GUITabControl(ui, this));
	this->tcDB->SetRect(0, 0, 100, 400, false);
	this->tcDB->SetDockType(UI::GUIControl::DOCK_TOP);
	this->tpTable = this->tcDB->AddTabPage((const UTF8Char*)"Tables");
//	this->tpSQL = this->tcDB->AddTabPage(L"SQL");
	NEW_CLASS(this->vspDB, UI::GUIVSplitter(ui, this, 3, false));
	NEW_CLASS(this->lvResult, UI::GUIListView(ui, this, UI::GUIListView::LVSTYLE_TABLE, 1));
	this->lvResult->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lvResult->SetFullRowSelect(true);
	this->lvResult->SetShowGrid(true);

	NEW_CLASS(this->lbTable, UI::GUIListBox(ui, this->tpTable, false));
	this->lbTable->SetRect(0, 0, 200, 100, false);
	this->lbTable->SetDockType(UI::GUIControl::DOCK_LEFT);
	this->lbTable->HandleSelectionChange(OnTableSelChg, this);
	NEW_CLASS(this->hspTable, UI::GUIHSplitter(ui, this->tpTable, 3, false));
	NEW_CLASS(this->lvTable, UI::GUIListView(ui, this->tpTable, UI::GUIListView::LVSTYLE_TABLE, 7));
	this->lvTable->SetDockType(UI::GUIControl::DOCK_FILL);
	this->lvTable->SetFullRowSelect(true);
	this->lvTable->AddColumn((const UTF8Char*)"Name", 200);
	this->lvTable->AddColumn((const UTF8Char*)"Type", 100);
	this->lvTable->AddColumn((const UTF8Char*)"Null?", 100);
	this->lvTable->AddColumn((const UTF8Char*)"PK?", 30);
	this->lvTable->AddColumn((const UTF8Char*)"Auto_Inc", 100);
	this->lvTable->AddColumn((const UTF8Char*)"Default Val", 100);
	this->lvTable->AddColumn((const UTF8Char*)"Attribute", 100);

	UI::GUIMenu *mnu;
	NEW_CLASS(this->mnuMain, UI::GUIMainMenu());
	mnu = this->mnuMain->AddSubMenu((const UTF8Char*)"&File");
	mnu->AddItem((const UTF8Char*)"&Save as", MNU_FILE_SAVE, UI::GUIMenu::KM_CONTROL, UI::GUIControl::GK_S);
	mnu = this->mnuMain->AddSubMenu((const UTF8Char*)"&Chart");
	mnu->AddItem((const UTF8Char*)"&Line Chart", MNU_CHART_LINE, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
	NEW_CLASS(this->dbNames, Data::ArrayList<const UTF8Char*>());
	if (this->dbt && this->dbt->GetDatabaseNames(this->dbNames) > 0)
	{
		UOSInt i = 0;
		UOSInt j = this->dbNames->GetCount();
		mnu = this->mnuMain->AddSubMenu((const UTF8Char*)"&Database");
		while (i < j)
		{
			mnu->AddItem(this->dbNames->GetItem(i), MNU_DATABASE_START + i, UI::GUIMenu::KM_NONE, UI::GUIControl::GK_NONE);
			i++;
		}
	}
	this->SetMenu(this->mnuMain);

	this->UpdateTables();
}

SSWR::AVIRead::AVIRDBForm::~AVIRDBForm()
{
	if (this->dbt)
	{
		this->dbt->ReleaseDatabaseNames(this->dbNames);
		DEL_CLASS(this->dbt);
	}
	else if (this->needRelease)
	{
		DEL_CLASS(this->db);
	}
	DEL_CLASS(this->dbNames);
	DEL_CLASS(this->log);
}

void SSWR::AVIRead::AVIRDBForm::UpdateTables()
{
	Data::ArrayListStrUTF8 *tableNames;
	OSInt i;
	OSInt j;

	this->lbTable->ClearItems();
	NEW_CLASS(tableNames, Data::ArrayListStrUTF8());
	this->dbt->GetTableNames(tableNames);
	i = 0;
	j = tableNames->GetCount();
	while (i < j)
	{
		this->lbTable->AddItem(tableNames->GetItem(i), 0);
		i++;
	}
	DEL_CLASS(tableNames);
}

void SSWR::AVIRead::AVIRDBForm::EventMenuClicked(UInt16 cmdId)
{
	if (cmdId >= MNU_DATABASE_START)
	{
		if (this->dbt->ChangeDatabase(this->dbNames->GetItem(cmdId - MNU_DATABASE_START)))
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
		{
			UTF8Char sbuff[512];
			if (this->lbTable->GetSelectedItemText(sbuff))
			{
				SSWR::AVIRead::AVIRLineChartForm *frm;
				Data::IChart *chart = 0;
				NEW_CLASS(frm, SSWR::AVIRead::AVIRLineChartForm(0, this->ui, this->core, this->db, sbuff));
				if (frm->ShowDialog(this) == DR_OK)
				{
					chart = frm->GetChart();
				}
				DEL_CLASS(frm);
				if (chart)
				{
					SSWR::AVIRead::AVIRChartForm *chartFrm;
					NEW_CLASS(chartFrm, SSWR::AVIRead::AVIRChartForm(0, this->ui, this->core, chart));
					this->core->ShowForm(chartFrm);
				}
			}
		}
		break;
	}
}

void SSWR::AVIRead::AVIRDBForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
