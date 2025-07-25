#include "Stdafx.h"
#include "Data/ArrayList.h"
#include "DB/ColDef.h"
#include "SSWR/AVIRead/AVIRLineChartForm.h"
#include "Text/MyString.h"

void __stdcall SSWR::AVIRead::AVIRLineChartForm::OnPlotClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRLineChartForm> me = userObj.GetNN<SSWR::AVIRead::AVIRLineChartForm>();
	UnsafeArray<DB::DBUtil::ColType> strTypes;
	OSInt xCol;
	UOSInt i;
	UOSInt j;
	Data::DateTime dt;
	if (!me->strTypes.SetTo(strTypes))
	{
		me->ui->ShowMsgOK(CSTR("Error in reading database data"), CSTR("Error"), me);
		return;
	}
	if (me->yCols.GetCount() == 0)
	{
		me->ui->ShowMsgOK(CSTR("Please add a data column first"), CSTR("Error"), me);
		return;
	}
	xCol = me->cboXAxis->GetSelectedItem().GetOSInt();
	if (xCol < 0)
	{
		me->ui->ShowMsgOK(CSTR("Current Version does not support integer X-Axis"), CSTR("Error"), me);
		return;
	}

	UOSInt colCount;
	ColInfo *colInfos;
	NN<DB::DBReader> reader;
	if (!me->db->QueryTableData(OPTSTR_CSTR(me->schemaName), me->tableName->ToCString(), 0, 0, 0, CSTR_NULL, 0).SetTo(reader))
	{
		me->ui->ShowMsgOK(CSTR("Error in getting database data"), CSTR("Error"), me);
		return;
	}
	colCount = me->yCols.GetCount() + 1;
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
			colInfos[i].colIndex = me->yCols.GetItem(i - 1);
		}
		NEW_CLASSNN(colInfos[i].colDef, DB::ColDef(CSTR("")));

		if (strTypes[colInfos[i].colIndex] == DB::DBUtil::CT_Unknown)
		{
			reader->GetColDef(colInfos[i].colIndex, colInfos[i].colDef);
		}
		else
		{
			colInfos[i].colDef->SetColType(strTypes[colInfos[i].colIndex]);
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
				NN<Data::ArrayList<Double>> dblVals = colInfos[i].datas.GetNN<Data::ArrayList<Double>>();
				dblVals->Add(reader->GetDblOrNAN(colInfos[i].colIndex));
			}
			else if (colInfos[i].colDef->GetColType() == DB::DBUtil::CT_Int32)
			{
				NN<Data::ArrayList<Int32>> i32Vals = colInfos[i].datas.GetNN<Data::ArrayList<Int32>>();
				i32Vals->Add(reader->GetInt32(colInfos[i].colIndex));
			}
			else if (colInfos[i].colDef->GetColType() == DB::DBUtil::CT_Int64)
			{
				NN<Data::ArrayList<Int64>> i64Vals = colInfos[i].datas.GetNN<Data::ArrayList<Int64>>();
				i64Vals->Add(reader->GetInt64(colInfos[i].colIndex));
			}
			else if (colInfos[i].colDef->GetColType() == DB::DBUtil::CT_DateTime)
			{
				NN<Data::ArrayList<Int64>> i64Vals = colInfos[i].datas.GetNN<Data::ArrayList<Int64>>();
				i64Vals->Add(reader->GetTimestamp(colInfos[i].colIndex).ToTicks());
			}
		}
	}

	Data::ChartPlotter *chart;
	Optional<Data::ChartPlotter::ChartData> xData = 0;
	NN<Data::ChartPlotter::ChartData> nnData;
	if (colInfos[0].colDef->GetColType() == DB::DBUtil::CT_DateTime)
	{
		Int64 *i64Data;
		NEW_CLASS(chart, Data::ChartPlotter(me->tableName->ToCString()));
		i64Data = colInfos[0].datas.GetNN<Data::ArrayList<Int64>>()->GetArr(j).Ptr();
		xData = Data::ChartPlotter::NewDataDate(i64Data, j);
	}
	else if (colInfos[0].colDef->GetColType() == DB::DBUtil::CT_Double)
	{
		Double *dblData;
		NEW_CLASS(chart, Data::ChartPlotter(me->tableName->ToCString()));
		dblData = colInfos[0].datas.GetNN<Data::ArrayList<Double>>()->GetArr(j).Ptr();
		xData = Data::ChartPlotter::NewData(dblData, j);

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
		NEW_CLASS(chart, Data::ChartPlotter(me->tableName->ToCString()));
		i32Data = colInfos[0].datas.GetNN<Data::ArrayList<Int32>>()->GetArr(j).Ptr();
		xData = Data::ChartPlotter::NewData(i32Data, j);
	}
	else
	{
		chart = 0;
	}

	if (chart && xData.SetTo(nnData))
	{
		i = 1;
		while (i < colCount)
		{
			if (colInfos[i].colDef->GetColType() == DB::DBUtil::CT_Double)
			{
				Double *dblData;
				NN<Data::ArrayList<Double>> dblVals = colInfos[i].datas.GetNN<Data::ArrayList<Double>>();
				dblData = dblVals->GetArr(j).Ptr();
				chart->AddLineChart(colInfos[i].colDef->GetColName(), Data::ChartPlotter::NewData(dblData, j), nnData->Clone(), chart->GetRndColor());
			}
			else if (colInfos[i].colDef->GetColType() == DB::DBUtil::CT_Int32)
			{
				Int32 *i32Data;
				NN<Data::ArrayList<Int32>> i32Vals = colInfos[i].datas.GetNN<Data::ArrayList<Int32>>();
				i32Data = i32Vals->GetArr(j).Ptr();
				chart->AddLineChart(colInfos[i].colDef->GetColName(), Data::ChartPlotter::NewData(i32Data, j), nnData->Clone(), chart->GetRndColor());
			}
			else if (colInfos[i].colDef->GetColType() == DB::DBUtil::CT_Int64)
			{
				Int64 *i64Data;
				NN<Data::ArrayList<Int64>> i64Vals = colInfos[i].datas.GetNN<Data::ArrayList<Int64>>();
				i64Data = i64Vals->GetArr(j).Ptr();
				chart->AddLineChart(colInfos[i].colDef->GetColName(), Data::ChartPlotter::NewDataDate(i64Data, j), nnData->Clone(), chart->GetRndColor());
			}
			else if (colInfos[i].colDef->GetColType() == DB::DBUtil::CT_DateTime)
			{
				Int64 *i64Data;
				NN<Data::ArrayList<Int64>> i64Vals = colInfos[i].datas.GetNN<Data::ArrayList<Int64>>();
				i64Data = i64Vals->GetArr(j).Ptr();
				chart->AddLineChart(colInfos[i].colDef->GetColName(), Data::ChartPlotter::NewDataDate(i64Data, j), nnData->Clone(), chart->GetRndColor());
			}
			i++;
		}
		me->chart = chart;
		xData.Delete();
	}

	me->db->CloseReader(reader);
	i = colCount;
	while (i-- > 0)
	{
		if (colInfos[i].colDef->GetColType() == DB::DBUtil::CT_Double)
		{
			NN<Data::ArrayList<Double>> dblVals = colInfos[i].datas.GetNN<Data::ArrayList<Double>>();
			dblVals.Delete();
		}
		else if (colInfos[i].colDef->GetColType() == DB::DBUtil::CT_Int32)
		{
			NN<Data::ArrayList<Int32>> i32Vals = colInfos[i].datas.GetNN<Data::ArrayList<Int32>>();
			i32Vals.Delete();
		}
		else if (colInfos[i].colDef->GetColType() == DB::DBUtil::CT_Int64)
		{
			NN<Data::ArrayList<Int64>> i64Vals = colInfos[i].datas.GetNN<Data::ArrayList<Int64>>();
			i64Vals.Delete();
		}
		else if (colInfos[i].colDef->GetColType() == DB::DBUtil::CT_DateTime)
		{
			NN<Data::ArrayList<Int64>> i64Vals = colInfos[i].datas.GetNN<Data::ArrayList<Int64>>();
			i64Vals.Delete();
		}
		colInfos[i].colDef.Delete();
	}
	MemFree(colInfos);
	if (me->chart.NotNull())
	{
		me->SetDialogResult(DR_OK);
	}
}

void __stdcall SSWR::AVIRead::AVIRLineChartForm::OnCancelClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRLineChartForm> me = userObj.GetNN<SSWR::AVIRead::AVIRLineChartForm>();
	me->SetDialogResult(DR_CANCEL);
}

void __stdcall SSWR::AVIRead::AVIRLineChartForm::OnYAxisClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRLineChartForm> me = userObj.GetNN<SSWR::AVIRead::AVIRLineChartForm>();
	UTF8Char sbuff[512];
	UnsafeArray<UTF8Char> sptr;
	UOSInt i = me->cboYAxis->GetSelectedIndex();
	UOSInt col;
	if (i == INVALID_INDEX)
	{
		me->ui->ShowMsgOK(CSTR("Please select a column first"), CSTR("Error"), me);
		return;
	}
	col = me->cboYAxis->GetItem(i).GetUOSInt();
	sbuff[0] = 0;
	sptr = me->cboYAxis->GetItemText(sbuff, i).Or(sbuff);
	me->lbYAxis->AddItem(CSTRP(sbuff, sptr), (void*)col);
	me->yCols.Add((UInt32)col);
}

void __stdcall SSWR::AVIRead::AVIRLineChartForm::OnStrColsDblClicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRLineChartForm> me = userObj.GetNN<SSWR::AVIRead::AVIRLineChartForm>();
	UnsafeArray<DB::DBUtil::ColType> strTypes;
	UOSInt selInd = me->lbStrCols->GetSelectedIndex();
	if (selInd != INVALID_INDEX && me->strTypes.SetTo(strTypes))
	{
		UOSInt colInd = (UOSInt)me->lbStrCols->GetItem(selInd).p;
		strTypes[colInd] = DB::DBUtil::CT_Double;
		NN<Text::String> s = Text::String::OrEmpty(me->lbStrCols->GetItemTextNew(selInd));
		me->cboXAxis->AddItem(s, (void*)colInd);
		me->cboYAxis->AddItem(s, (void*)colInd);
		s->Release();
		me->lbStrCols->RemoveItem(selInd);
	}
}

void __stdcall SSWR::AVIRead::AVIRLineChartForm::OnStrColsInt32Clicked(AnyType userObj)
{
	NN<SSWR::AVIRead::AVIRLineChartForm> me = userObj.GetNN<SSWR::AVIRead::AVIRLineChartForm>();
	UnsafeArray<DB::DBUtil::ColType> strTypes;
	UOSInt selInd = me->lbStrCols->GetSelectedIndex();
	if (selInd != INVALID_INDEX && me->strTypes.SetTo(strTypes))
	{
		UOSInt colInd = (UOSInt)me->lbStrCols->GetItem(selInd).p;
		strTypes[colInd] = DB::DBUtil::CT_Int32;
		NN<Text::String> s = Text::String::OrEmpty(me->lbStrCols->GetItemTextNew(selInd));
		me->cboXAxis->AddItem(s, (void*)colInd);
		me->cboYAxis->AddItem(s, (void*)colInd);
		s->Release();
		me->lbStrCols->RemoveItem(selInd);
	}
}

SSWR::AVIRead::AVIRLineChartForm::AVIRLineChartForm(Optional<UI::GUIClientControl> parent, NN<UI::GUICore> ui, NN<SSWR::AVIRead::AVIRCore> core, NN<DB::ReadingDB> db, Text::CString schemaName, Text::CStringNN tableName) : UI::GUIForm(parent, 1024, 768, ui)
{
	this->SetText(CSTR("Line Chart"));
	this->SetFont(0, 0, 8.25, false);
	this->core = core;
	this->chart = 0;
	this->db = db;
	this->strTypes = 0;
	this->tableName = Text::String::New(tableName);
	this->schemaName = Text::String::NewOrNull(schemaName);
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));

	this->pnlStrCols = ui->NewPanel(*this);
	this->pnlStrCols->SetRect(0, 0, 100, 128, false);
	this->pnlStrCols->SetDockType(UI::GUIControl::DOCK_TOP);
	this->lblStrCols = ui->NewLabel(this->pnlStrCols, CSTR("String Columns"));
	this->lblStrCols->SetRect(0, 4, 100, 23, false);
	this->lbStrCols = ui->NewListBox(this->pnlStrCols, false);
	this->lbStrCols->SetRect(100, 0, 200, 128, false);
	this->btnStrColsDbl = ui->NewButton(this->pnlStrCols, CSTR("Force as Double"));
	this->btnStrColsDbl->SetRect(304, 4, 120, 23, false);
	this->btnStrColsDbl->HandleButtonClick(OnStrColsDblClicked, this);
	this->btnStrColsInt32 = ui->NewButton(this->pnlStrCols, CSTR("Force as Int32"));
	this->btnStrColsInt32->SetRect(304, 32, 120, 23, false);
	this->btnStrColsInt32->HandleButtonClick(OnStrColsInt32Clicked, this);
	this->pnlXAxis = ui->NewPanel(*this);
	this->pnlXAxis->SetRect(0, 0 , 100, 32, false);
	this->pnlXAxis->SetDockType(UI::GUIControl::DOCK_TOP);
	this->lblXAxis = ui->NewLabel(this->pnlXAxis, CSTR("X-Axis"));
	this->lblXAxis->SetRect(4, 4, 100, 23, false);
	this->cboXAxis = ui->NewComboBox(this->pnlXAxis, false);
	this->cboXAxis->SetRect(104, 4, 200, 23, false);
	this->pnlControl = ui->NewPanel(*this);
	this->pnlControl->SetRect(0, 0, 100, 32, false);
	this->pnlControl->SetDockType(UI::GUIControl::DOCK_BOTTOM);
	this->btnPlot = ui->NewButton(this->pnlControl, CSTR("Plot"));
	this->btnPlot->SetRect(4, 4, 75, 23, false);
	this->btnPlot->HandleButtonClick(OnPlotClicked, this);
	this->btnCancel = ui->NewButton(this->pnlControl, CSTR("Cancel"));
	this->btnCancel->SetRect(84, 4, 75, 23, false);
	this->btnCancel->HandleButtonClick(OnCancelClicked, this);
	this->grpYAxis = ui->NewGroupBox(*this, CSTR("Y-Axis"));
	this->grpYAxis->SetDockType(UI::GUIControl::DOCK_FILL);
	this->pnlYAxis = ui->NewPanel(this->grpYAxis);
	this->pnlYAxis->SetRect(0, 0, 100, 32, false);
	this->pnlYAxis->SetDockType(UI::GUIControl::DOCK_TOP);
	this->lblYAxis = ui->NewLabel(this->pnlYAxis, CSTR("Column"));
	this->lblYAxis->SetRect(4, 4, 100, 23, false);
	this->cboYAxis = ui->NewComboBox(this->pnlYAxis, false);
	this->cboYAxis->SetRect(104, 4, 200, 23, false);
	this->btnYAxis = ui->NewButton(this->pnlYAxis, CSTR("Add"));
	this->btnYAxis->SetRect(304, 4, 75, 23, false);
	this->btnYAxis->HandleButtonClick(OnYAxisClicked, this);
	this->lbYAxis = ui->NewListBox(this->grpYAxis, false);
	this->lbYAxis->SetDockType(UI::GUIControl::DOCK_FILL);

	NN<DB::DBReader> reader;
	if (!this->db->QueryTableData(OPTSTR_CSTR(this->schemaName), tableName, 0, 0, 0, CSTR_NULL, 0).SetTo(reader))
	{
	}
	else
	{
		this->cboXAxis->AddItem(CSTR("Auto Number"), (void*)-1);
		this->cboXAxis->SetSelectedIndex(0);
		UnsafeArray<DB::DBUtil::ColType> strTypes;
		UOSInt i;
		UOSInt j = reader->ColCount();
		DB::ColDef colDef(CSTR(""));
		this->strTypes = strTypes = MemAllocArr(DB::DBUtil::ColType, j);
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
			strTypes[i] = DB::DBUtil::CT_Unknown;
			i++;
		}
		this->db->CloseReader(reader);
	}
}

SSWR::AVIRead::AVIRLineChartForm::~AVIRLineChartForm()
{
	UnsafeArray<DB::DBUtil::ColType> strTypes;
	this->tableName->Release();
	OPTSTR_DEL(this->schemaName);
	if (this->strTypes.SetTo(strTypes))
	{
		MemFreeArr(strTypes);
	}
}

Optional<Data::ChartPlotter> SSWR::AVIRead::AVIRLineChartForm::GetChart()
{
	return this->chart;
}

void SSWR::AVIRead::AVIRLineChartForm::OnMonitorChanged()
{
	this->SetDPI(this->core->GetMonitorHDPI(this->GetHMonitor()), this->core->GetMonitorDDPI(this->GetHMonitor()));
}
