#include "Stdafx.h"
#include "Data/ArrayList.h"
#include "Data/LineChart.h"
#include "DB/ColDef.h"
#include "SSWR/AVIRead/AVIRLineChartForm.h"
#include "Text/MyString.h"
#include "UI/MessageDialog.h"

void __stdcall SSWR::AVIRead::AVIRLineChartForm::OnPlotClicked(void *userObj)
{
	SSWR::AVIRead::AVIRLineChartForm *me = (SSWR::AVIRead::AVIRLineChartForm *)userObj;
	OSInt xCol;
	UOSInt i;
	UOSInt j;
	Data::DateTime dt;
	if (me->yCols->GetCount() == 0)
	{
		UI::MessageDialog::ShowDialog(CSTR("Please add a data column first"), CSTR("Error"), me);
		return;
	}
	xCol = (OSInt)me->cboXAxis->GetSelectedItem();
	if (xCol < 0)
	{
		UI::MessageDialog::ShowDialog(CSTR("Current Version does not support integer X-Axis"), CSTR("Error"), me);
		return;
	}

	UOSInt colCount;
	ColInfo *colInfos;
	DB::DBReader *reader = me->db->QueryTableData(STR_CSTR(me->schemaName), me->tableName->ToCString(), 0, 0, 0, CSTR_NULL, 0);
	if (reader == 0)
	{
		UI::MessageDialog::ShowDialog(CSTR("Error in getting database data"), CSTR("Error"), me);
		return;
	}
	colCount = me->yCols->GetCount() + 1;
	colInfos = MemAlloc(ColInfo, colCount);
	i = colCount;
	while (i-- > 0)
	{
		if (i == 0)
		{
			colInfos[i].colIndex = (UOSInt)xCol;
		}
		else
		{
			colInfos[i].colIndex = me->yCols->GetItem(i - 1);
		}
		NEW_CLASSNN(colInfos[i].colDef, DB::ColDef(CSTR("")));

		if (me->strTypes[colInfos[i].colIndex] == DB::DBUtil::CT_Unknown)
		{
			reader->GetColDef(colInfos[i].colIndex, colInfos[i].colDef);
		}
		else
		{
			colInfos[i].colDef->SetColType(me->strTypes[colInfos[i].colIndex]);
		}
		if (colInfos[i].colDef->GetColType() == DB::DBUtil::CT_Double)
		{
			Data::ArrayList<Double> *dblVals;
			NEW_CLASS(dblVals, Data::ArrayList<Double>());
			colInfos[i].datas = dblVals;
		}
		else if (colInfos[i].colDef->GetColType() == DB::DBUtil::CT_Int32)
		{
			Data::ArrayList<Int32> *i32Vals;
			NEW_CLASS(i32Vals, Data::ArrayList<Int32>());
			colInfos[i].datas = i32Vals;
		}
		else if (colInfos[i].colDef->GetColType() == DB::DBUtil::CT_Int64)
		{
			Data::ArrayList<Int64> *i64Vals;
			NEW_CLASS(i64Vals, Data::ArrayList<Int64>());
			colInfos[i].datas = i64Vals;
		}
		else if (colInfos[i].colDef->GetColType() == DB::DBUtil::CT_DateTime)
		{
			Data::ArrayList<Int64> *i64Vals;
			NEW_CLASS(i64Vals, Data::ArrayList<Int64>());
			colInfos[i].datas = i64Vals;
		}
		else
		{
			colInfos[i].datas = 0;
		}
	}
	
	while (reader->ReadNext())
	{
		i = colCount;
		while (i-- > 0)
		{
			if (colInfos[i].colDef->GetColType() == DB::DBUtil::CT_Double)
			{
				Data::ArrayList<Double> *dblVals = (Data::ArrayList<Double>*)colInfos[i].datas;
				dblVals->Add(reader->GetDbl(colInfos[i].colIndex));
			}
			else if (colInfos[i].colDef->GetColType() == DB::DBUtil::CT_Int32)
			{
				Data::ArrayList<Int32> *i32Vals = (Data::ArrayList<Int32>*)colInfos[i].datas;
				i32Vals->Add(reader->GetInt32(colInfos[i].colIndex));
			}
			else if (colInfos[i].colDef->GetColType() == DB::DBUtil::CT_Int64)
			{
				Data::ArrayList<Int64> *i64Vals = (Data::ArrayList<Int64>*)colInfos[i].datas;
				i64Vals->Add(reader->GetInt64(colInfos[i].colIndex));
			}
			else if (colInfos[i].colDef->GetColType() == DB::DBUtil::CT_DateTime)
			{
				Data::ArrayList<Int64> *i64Vals = (Data::ArrayList<Int64>*)colInfos[i].datas;
				i64Vals->Add(reader->GetTimestamp(colInfos[i].colIndex).ToTicks());
			}
		}
	}

	Data::LineChart *chart;
	if (colInfos[0].colDef->GetColType() == DB::DBUtil::CT_DateTime)
	{
		Int64 *i64Data;
		NEW_CLASS(chart, Data::LineChart(me->tableName->ToCString()));
		i64Data = ((Data::ArrayList<Int64>*)colInfos[0].datas)->GetArray(&j);
		chart->AddXDataDate(i64Data, j);
	}
	else if (colInfos[0].colDef->GetColType() == DB::DBUtil::CT_Double)
	{
		Double *dblData;
		NEW_CLASS(chart, Data::LineChart(me->tableName->ToCString()));
		dblData = ((Data::ArrayList<Double>*)colInfos[0].datas)->GetArray(&j);
		chart->AddXData(dblData, j);

		Double dblDiff = dblData[j - 1] - dblData[0];
		if (dblDiff < 0.0001)
		{
			chart->SetDblFormat(CSTR("0.000000"));
		}
		else if (dblDiff < 0.001)
		{
			chart->SetDblFormat(CSTR("0.00000"));
		}
		else if (dblDiff < 0.01)
		{
			chart->SetDblFormat(CSTR("0.0000"));
		}
		else if (dblDiff < 0.1)
		{
			chart->SetDblFormat(CSTR("0.000"));
		}
	}
	else if (colInfos[0].colDef->GetColType() == DB::DBUtil::CT_Int32)
	{
		Int32 *i32Data;
		NEW_CLASS(chart, Data::LineChart(me->tableName->ToCString()));
		i32Data = ((Data::ArrayList<Int32>*)colInfos[0].datas)->GetArray(&j);
		chart->AddXData(i32Data, j);
	}
	else
	{
		chart = 0;
	}

	if (chart)
	{
		i = 1;
		while (i < colCount)
		{
			if (colInfos[i].colDef->GetColType() == DB::DBUtil::CT_Double)
			{
				Double *dblData;
				Data::ArrayList<Double> *dblVals = (Data::ArrayList<Double>*)colInfos[i].datas;
				dblData = dblVals->GetArray(&j);
				chart->AddYData(colInfos[i].colDef->GetColName().Ptr(), dblData, j, chart->GetRndColor(), Data::LineChart::LS_LINE);
			}
			else if (colInfos[i].colDef->GetColType() == DB::DBUtil::CT_Int32)
			{
				Int32 *i32Data;
				Data::ArrayList<Int32> *i32Vals = (Data::ArrayList<Int32>*)colInfos[i].datas;
				i32Data = i32Vals->GetArray(&j);
				chart->AddYData(colInfos[i].colDef->GetColName().Ptr(), i32Data, j, chart->GetRndColor(), Data::LineChart::LS_LINE);
			}
			else if (colInfos[i].colDef->GetColType() == DB::DBUtil::CT_Int64)
			{
				Int64 *i64Data;
				Data::ArrayList<Int64> *i64Vals = (Data::ArrayList<Int64>*)colInfos[i].datas;
				i64Data = i64Vals->GetArray(&j);
				chart->AddYDataDate(colInfos[i].colDef->GetColName().Ptr(), i64Data, j, chart->GetRndColor(), Data::LineChart::LS_LINE);
			}
			else if (colInfos[i].colDef->GetColType() == DB::DBUtil::CT_DateTime)
			{
				Int64 *i64Data;
				Data::ArrayList<Int64> *i64Vals = (Data::ArrayList<Int64>*)colInfos[i].datas;
				i64Data = i64Vals->GetArray(&j);
				chart->AddYDataDate(colInfos[i].colDef->GetColName().Ptr(), i64Data, j, chart->GetRndColor(), Data::LineChart::LS_LINE);
			}
			i++;
		}
		me->chart = chart;
	}

	me->db->CloseReader(reader);
	i = colCount;
	while (i-- > 0)
	{
		if (colInfos[i].colDef->GetColType() == DB::DBUtil::CT_Double)
		{
			Data::ArrayList<Double> *dblVals = (Data::ArrayList<Double>*)colInfos[i].datas;
			DEL_CLASS(dblVals);
		}
		else if (colInfos[i].colDef->GetColType() == DB::DBUtil::CT_Int32)
		{
			Data::ArrayList<Int32> *i32Vals = (Data::ArrayList<Int32>*)colInfos[i].datas;
			DEL_CLASS(i32Vals);
		}
		else if (colInfos[i].colDef->GetColType() == DB::DBUtil::CT_Int64)
		{
			Data::ArrayList<Int64> *i64Vals = (Data::ArrayList<Int64>*)colInfos[i].datas;
			DEL_CLASS(i64Vals);
		}
		else if (colInfos[i].colDef->GetColType() == DB::DBUtil::CT_DateTime)
		{
			Data::ArrayList<Int64> *i64Vals = (Data::ArrayList<Int64>*)colInfos[i].datas;
			DEL_CLASS(i64Vals);
		}
		colInfos[i].colDef.Delete();
	}
	MemFree(colInfos);
	if (me->chart)
	{
		me->SetDialogResult(DR_OK);
	}
}

void __stdcall SSWR::AVIRead::AVIRLineChartForm::OnCancelClicked(void *userObj)
{
	SSWR::AVIRead::AVIRLineChartForm *me = (SSWR::AVIRead::AVIRLineChartForm *)userObj;
	me->SetDialogResult(DR_CANCEL);
}

void __stdcall SSWR::AVIRead::AVIRLineChartForm::OnYAxisClicked(void *userObj)
{
	SSWR::AVIRead::AVIRLineChartForm *me = (SSWR::AVIRead::AVIRLineChartForm *)userObj;
	UTF8Char sbuff[512];
	UTF8Char *sptr;
	UOSInt i = me->cboYAxis->GetSelectedIndex();
	UOSInt col;
	if (i == INVALID_INDEX)
	{
		UI::MessageDialog::ShowDialog(CSTR("Please select a column first"), CSTR("Error"), me);
		return;
	}
	col = (UOSInt)me->cboYAxis->GetItem(i);
	sptr = me->cboYAxis->GetItemText(sbuff, i);
	me->lbYAxis->AddItem(CSTRP(sbuff, sptr), (void*)col);
	me->yCols->Add((UInt32)col);
}

void __stdcall SSWR::AVIRead::AVIRLineChartForm::OnStrColsDblClicked(void *userObj)
{
	SSWR::AVIRead::AVIRLineChartForm *me = (SSWR::AVIRead::AVIRLineChartForm *)userObj;
	UOSInt selInd = me->lbStrCols->GetSelectedIndex();
	if (selInd != INVALID_INDEX)
	{
		UOSInt colInd = (UOSInt)me->lbStrCols->GetItem(selInd);
		me->strTypes[colInd] = DB::DBUtil::CT_Double;
		NotNullPtr<Text::String> s = Text::String::OrEmpty(me->lbStrCols->GetItemTextNew(selInd));
		me->cboXAxis->AddItem(s, (void*)colInd);
		me->cboYAxis->AddItem(s, (void*)colInd);
		s->Release();
		me->lbStrCols->RemoveItem(selInd);
	}
}

void __stdcall SSWR::AVIRead::AVIRLineChartForm::OnStrColsInt32Clicked(void *userObj)
{
	SSWR::AVIRead::AVIRLineChartForm *me = (SSWR::AVIRead::AVIRLineChartForm *)userObj;
	UOSInt selInd = me->lbStrCols->GetSelectedIndex();
	if (selInd != INVALID_INDEX)
	{
		UOSInt colInd = (UOSInt)me->lbStrCols->GetItem(selInd);
		me->strTypes[colInd] = DB::DBUtil::CT_Int32;
		NotNullPtr<Text::String> s = Text::String::OrEmpty(me->lbStrCols->GetItemTextNew(selInd));
		me->cboXAxis->AddItem(s, (void*)colInd);
		me->cboYAxis->AddItem(s, (void*)colInd);
		s->Release();
		me->lbStrCols->RemoveItem(selInd);
	}
}

SSWR::AVIRead::AVIRLineChartForm::AVIRLineChartForm(UI::GUIClientControl *parent, NotNullPtr<UI::GUICore> ui, NotNullPtr<SSWR::AVIRead::AVIRCore> core, DB::ReadingDB *db, Text::CString schemaName, Text::CString tableName) : UI::GUIForm(parent, 1024, 768, ui)
{
	this->SetText(CSTR("Line Chart"));
	this->SetFont(0, 0, 8.25, false);
	this->core = core;
	this->chart = 0;
	this->db = db;
	this->strTypes = 0;
	this->tableName = Text::String::New(tableName);
	this->schemaName = Text::String::NewOrNull(schemaName);
	NEW_CLASS(this->yCols, Data::ArrayList<UInt32>());
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	NEW_CLASS(this->pnlStrCols, UI::GUIPanel(ui, this));
	this->pnlStrCols->SetRect(0, 0, 100, 128, false);
	this->pnlStrCols->SetDockType(UI::GUIControl::DOCK_TOP);
	NEW_CLASS(this->lblStrCols, UI::GUILabel(ui, this->pnlStrCols, CSTR("String Columns")));
	this->lblStrCols->SetRect(0, 4, 100, 23, false);
	NEW_CLASS(this->lbStrCols, UI::GUIListBox(ui, this->pnlStrCols, false));
	this->lbStrCols->SetRect(100, 0, 200, 128, false);
	NEW_CLASS(this->btnStrColsDbl, UI::GUIButton(ui, this->pnlStrCols, CSTR("Force as Double")));
	this->btnStrColsDbl->SetRect(304, 4, 120, 23, false);
	this->btnStrColsDbl->HandleButtonClick(OnStrColsDblClicked, this);
	NEW_CLASS(this->btnStrColsInt32, UI::GUIButton(ui, this->pnlStrCols, CSTR("Force as Int32")));
	this->btnStrColsInt32->SetRect(304, 32, 120, 23, false);
	this->btnStrColsInt32->HandleButtonClick(OnStrColsInt32Clicked, this);
	NEW_CLASS(this->pnlXAxis, UI::GUIPanel(ui, this));
	this->pnlXAxis->SetRect(0, 0 , 100, 32, false);
	this->pnlXAxis->SetDockType(UI::GUIControl::DOCK_TOP);
	NEW_CLASS(this->lblXAxis, UI::GUILabel(ui, this->pnlXAxis, CSTR("X-Axis")));
	this->lblXAxis->SetRect(4, 4, 100, 23, false);
	NEW_CLASS(this->cboXAxis, UI::GUIComboBox(ui, this->pnlXAxis, false));
	this->cboXAxis->SetRect(104, 4, 200, 23, false);
	NEW_CLASS(this->pnlControl, UI::GUIPanel(ui, this));
	this->pnlControl->SetRect(0, 0, 100, 32, false);
	this->pnlControl->SetDockType(UI::GUIControl::DOCK_BOTTOM);
	NEW_CLASS(this->btnPlot, UI::GUIButton(ui, this->pnlControl, CSTR("Plot")));
	this->btnPlot->SetRect(4, 4, 75, 23, false);
	this->btnPlot->HandleButtonClick(OnPlotClicked, this);
	NEW_CLASS(this->btnCancel, UI::GUIButton(ui, this->pnlControl, CSTR("Cancel")));
	this->btnCancel->SetRect(84, 4, 75, 23, false);
	this->btnCancel->HandleButtonClick(OnCancelClicked, this);
	NEW_CLASS(this->grpYAxis, UI::GUIGroupBox(ui, this, CSTR("Y-Axis")));
	this->grpYAxis->SetDockType(UI::GUIControl::DOCK_FILL);
	NEW_CLASS(this->pnlYAxis, UI::GUIPanel(ui, this->grpYAxis));
	this->pnlYAxis->SetRect(0, 0, 100, 32, false);
	this->pnlYAxis->SetDockType(UI::GUIControl::DOCK_TOP);
	NEW_CLASS(this->lblYAxis, UI::GUILabel(ui, this->pnlYAxis, CSTR("Column")));
	this->lblYAxis->SetRect(4, 4, 100, 23, false);
	NEW_CLASS(this->cboYAxis, UI::GUIComboBox(ui, this->pnlYAxis, false));
	this->cboYAxis->SetRect(104, 4, 200, 23, false);
	NEW_CLASS(this->btnYAxis, UI::GUIButton(ui, this->pnlYAxis, CSTR("Add")));
	this->btnYAxis->SetRect(304, 4, 75, 23, false);
	this->btnYAxis->HandleButtonClick(OnYAxisClicked, this);
	NEW_CLASS(this->lbYAxis, UI::GUIListBox(ui, this->grpYAxis, false));
	this->lbYAxis->SetDockType(UI::GUIControl::DOCK_FILL);

	DB::DBReader *reader = this->db->QueryTableData(STR_CSTR(this->schemaName), tableName, 0, 0, 0, CSTR_NULL, 0);
	if (reader == 0)
	{
	}
	else
	{
		this->cboXAxis->AddItem(CSTR("Auto Number"), (void*)-1);
		this->cboXAxis->SetSelectedIndex(0);
		UOSInt i;
		UOSInt j = reader->ColCount();
		DB::ColDef colDef(CSTR(""));
		this->strTypes = MemAlloc(DB::DBUtil::ColType, j);
		i = 0;
		while (i < j)
		{
			reader->GetColDef(i, colDef);
			if (colDef.GetColType() == DB::DBUtil::CT_DateTime)
			{
				this->cboXAxis->AddItem(colDef.GetColName(), (void*)i);
			}
			else if (colDef.GetColType() == DB::DBUtil::CT_Double)
			{
				this->cboXAxis->AddItem(colDef.GetColName(), (void*)i);
				this->cboYAxis->AddItem(colDef.GetColName(), (void*)i);
			}
			else if (colDef.GetColType() == DB::DBUtil::CT_Int32)
			{
				this->cboXAxis->AddItem(colDef.GetColName(), (void*)i);
				this->cboYAxis->AddItem(colDef.GetColName(), (void*)i);
			}
			else if (colDef.GetColType() == DB::DBUtil::CT_Int64)
			{
//				this->cboXAxis->AddItem(colDef.GetColName(), (void*)i);
//				this->cboYAxis->AddItem(colDef.GetColName(), (void*)i);
			}
			else if (colDef.GetColType() == DB::DBUtil::CT_VarUTF8Char)
			{
				this->lbStrCols->AddItem(colDef.GetColName(), (void*)i);
			}
			this->strTypes[i] = DB::DBUtil::CT_Unknown;
			i++;
		}
		this->db->CloseReader(reader);
	}
}

SSWR::AVIRead::AVIRLineChartForm::~AVIRLineChartForm()
{
	DEL_CLASS(this->yCols);
	this->tableName->Release();
	SDEL_STRING(this->schemaName);
	if (this->strTypes)
	{
		MemFree(this->strTypes);
	}
}

Data::Chart *SSWR::AVIRead::AVIRLineChartForm::GetChart()
{
	return this->chart;
}

void SSWR::AVIRead::AVIRLineChartForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
