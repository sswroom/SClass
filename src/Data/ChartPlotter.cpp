#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ChartPlotter.h"
#include "IO/FileStream.h"
#include "Math/Math.h"
#include "Text/MyString.h"
#include "Text/MyStringFloat.h"

Data::ChartPlotter::TimeData::TimeData(UnsafeArray<Data::Timestamp> timeArr, UOSInt dataCnt) : ChartData(dataCnt)
{
	this->timeArr = MemAllocArr(Data::TimeInstant, dataCnt);
	UOSInt i = 0;
	while (i < dataCnt)
	{
		this->timeArr[i] = timeArr[i].inst;
		i++;
	}
}

Data::ChartPlotter::TimeData::TimeData(UnsafeArray<Data::TimeInstant> timeArr, UOSInt dataCnt) : ChartData(dataCnt)
{
	this->timeArr = MemAllocArr(Data::TimeInstant, dataCnt);
	UOSInt i = 0;
	while (i < dataCnt)
	{
		this->timeArr[i] = timeArr[i];
		i++;
	}
}

Data::ChartPlotter::TimeData::TimeData(UnsafeArray<Int64> ticksArr, UOSInt dataCnt) : ChartData(dataCnt)
{
	this->timeArr = MemAllocArr(Data::TimeInstant, dataCnt);
	UOSInt i = 0;
	while (i < dataCnt)
	{
		this->timeArr[i] = Data::TimeInstant::FromTicks(ticksArr[i]);
		i++;
	}
}

Data::ChartPlotter::TimeData::TimeData(NN<ReadingList<Timestamp>> timeArr) : ChartData(timeArr->GetCount())
{
	this->timeArr = MemAllocArr(Data::TimeInstant, this->dataCnt);
	UOSInt i = 0;
	while (i < this->dataCnt)
	{
		this->timeArr[i] = timeArr->GetItem(i).inst;
		i++;
	}
}

Data::ChartPlotter::TimeData::~TimeData()
{
	MemFreeArr(this->timeArr);
}

Data::ChartPlotter::DataType Data::ChartPlotter::TimeData::GetType() const
{
	return DataType::Time;
}

NN<Data::ChartPlotter::ChartData> Data::ChartPlotter::TimeData::Clone() const
{
	NN<TimeData> newData;
	NEW_CLASSNN(newData, TimeData(this->timeArr, this->dataCnt));
	return newData;
}

UnsafeArray<Data::TimeInstant> Data::ChartPlotter::TimeData::GetData() const
{
	return this->timeArr;
}

Data::ChartPlotter::Int32Data::Int32Data(UnsafeArray<Int32> intArr, UOSInt dataCnt) : ChartData(dataCnt)
{
	this->intArr = MemAllocArr(Int32, dataCnt);
	MemCopyNO(this->intArr.Ptr(), intArr.Ptr(), dataCnt * sizeof(Int32));
}

Data::ChartPlotter::Int32Data::Int32Data(NN<ReadingList<Int32>> intArr) : ChartData(intArr->GetCount())
{
	this->intArr = MemAllocArr(Int32, dataCnt);
	UOSInt i = 0;
	UOSInt j = dataCnt;
	while (i < j)
	{
		this->intArr[i] = intArr->GetItem(i);
		i++;
	}
}

Data::ChartPlotter::Int32Data::~Int32Data()
{
	MemFreeArr(this->intArr);
}

Data::ChartPlotter::DataType Data::ChartPlotter::Int32Data::GetType() const
{
	return DataType::Integer;
}

NN<Data::ChartPlotter::ChartData> Data::ChartPlotter::Int32Data::Clone() const
{
	NN<Int32Data> newData;
	NEW_CLASSNN(newData, Int32Data(this->intArr, this->dataCnt));
	return newData;
}

UnsafeArray<Int32> Data::ChartPlotter::Int32Data::GetData() const
{
	return this->intArr;
}

Data::ChartPlotter::UInt32Data::UInt32Data(UnsafeArray<UInt32> intArr, UOSInt dataCnt) : ChartData(dataCnt)
{
	this->intArr = MemAllocArr(UInt32, dataCnt);
	MemCopyNO(this->intArr.Ptr(), intArr.Ptr(), dataCnt * sizeof(UInt32));
}

Data::ChartPlotter::UInt32Data::UInt32Data(NN<ReadingList<UInt32>> intArr) : ChartData(intArr->GetCount())
{
	this->intArr = MemAllocArr(UInt32, dataCnt);
	UOSInt i = 0;
	UOSInt j = dataCnt;
	while (i < j)
	{
		this->intArr[i] = intArr->GetItem(i);
		i++;
	}
}

Data::ChartPlotter::UInt32Data::~UInt32Data()
{
	MemFreeArr(this->intArr);
}

Data::ChartPlotter::DataType Data::ChartPlotter::UInt32Data::GetType() const
{
	return DataType::UInteger;
}

NN<Data::ChartPlotter::ChartData> Data::ChartPlotter::UInt32Data::Clone() const
{
	NN<UInt32Data> newData;
	NEW_CLASSNN(newData, UInt32Data(this->intArr, this->dataCnt));
	return newData;
}

UnsafeArray<UInt32> Data::ChartPlotter::UInt32Data::GetData() const
{
	return this->intArr;
}

Data::ChartPlotter::DoubleData::DoubleData(UnsafeArray<Double> dblArr, UOSInt dataCnt) : ChartData(dataCnt)
{
	this->dblArr = MemAllocArr(Double, dataCnt);
	MemCopyNO(this->dblArr.Ptr(), dblArr.Ptr(), dataCnt * sizeof(Double));
}

Data::ChartPlotter::DoubleData::DoubleData(NN<ReadingList<Double>> dblArr) : ChartData(dblArr->GetCount())
{
	this->dblArr = MemAllocArr(Double, dataCnt);
	UOSInt i = 0;
	UOSInt j = this->dataCnt;
	while (i < j)
	{
		this->dblArr[i] = dblArr->GetItem(i);
		i++;
	}
}

Data::ChartPlotter::DoubleData::~DoubleData()
{
	MemFreeArr(this->dblArr);
}

Data::ChartPlotter::DataType Data::ChartPlotter::DoubleData::GetType() const
{
	return DataType::DOUBLE;
}

NN<Data::ChartPlotter::ChartData> Data::ChartPlotter::DoubleData::Clone() const
{
	NN<DoubleData> newData;
	NEW_CLASSNN(newData, DoubleData(this->dblArr, this->dataCnt));
	return newData;
}

UnsafeArray<Double> Data::ChartPlotter::DoubleData::GetData() const
{
	return this->dblArr;
}

Data::ChartPlotter::TimeAxis::TimeAxis(NN<TimeData> data)
{
	UnsafeArray<Data::TimeInstant> dataArr = data->GetData();
	if (data->GetCount() > 0)
	{
		this->min = this->max = dataArr[0];
		this->ExtendRange(data);
	}
	else
	{
		this->min = Data::TimeInstant::Now();
		this->max = this->min;
	}
}

Data::ChartPlotter::TimeAxis::TimeAxis(Data::TimeInstant val)
{
	this->min = val;
	this->max = val;
}

Data::ChartPlotter::TimeAxis::~TimeAxis()
{
}

Data::ChartPlotter::DataType Data::ChartPlotter::TimeAxis::GetType() const
{
	return DataType::Time;
}
void Data::ChartPlotter::TimeAxis::CalcX(NN<ChartData> data, UnsafeArray<Math::Coord2DDbl> pos, Double minX, Double maxX) const
{
	if (data->GetType() == DataType::Time)
	{
		Double leng = (maxX - minX);
		Double ratio = leng / max.DiffSecDbl(min);
		UOSInt i = 0;
		UOSInt j = data->GetCount();
		UnsafeArray<Data::TimeInstant> tArr = NN<TimeData>::ConvertFrom(data)->GetData();
		while (i < j)
		{
			pos[i].x = minX + tArr[i].DiffSecDbl(min) * ratio;
			i++;
		}
	}
}

void Data::ChartPlotter::TimeAxis::CalcY(NN<ChartData> data, UnsafeArray<Math::Coord2DDbl> pos, Double minY, Double maxY) const
{
	if (data->GetType() == DataType::Time)
	{
		Double leng = (maxY - minY);
		Double ratio = leng / max.DiffSecDbl(min);
		UOSInt i = 0;
		UOSInt j = data->GetCount();
		UnsafeArray<Data::TimeInstant> tArr = NN<TimeData>::ConvertFrom(data)->GetData();
		while (i < j)
		{
			pos[i].y = minY + tArr[i].DiffSecDbl(min) * ratio;
			i++;
		}
	}
}

void Data::ChartPlotter::TimeAxis::ExtendRange(NN<TimeData> data)
{
	UnsafeArray<Data::TimeInstant> dataArr = data->GetData();
	Data::TimeInstant v;
	UOSInt i = 0;
	UOSInt j = data->GetCount();
	while (i < j)
	{
		v = dataArr[i];
		if (v < min) min = v;
		if (v > max) max = v;
		i++;
	}
}

void Data::ChartPlotter::TimeAxis::ExtendRange(Data::TimeInstant inst)
{
	if (inst < min) min = inst;
	if (inst > max) max = inst;
}

Data::ChartPlotter::Int32Axis::Int32Axis(NN<Int32Data> data)
{
	UnsafeArray<Int32> dataArr = data->GetData();
	if (data->GetCount() > 0)
	{
		this->min = this->max = dataArr[0];
		this->ExtendRange(data);
	}
	else
	{
		this->min = 0;
		this->max = this->min;
	}
}

Data::ChartPlotter::Int32Axis::~Int32Axis()
{
}

Data::ChartPlotter::DataType Data::ChartPlotter::Int32Axis::GetType() const
{
	return DataType::Integer;
}

void Data::ChartPlotter::Int32Axis::CalcX(NN<ChartData> data, UnsafeArray<Math::Coord2DDbl> pos, Double minX, Double maxX) const
{
	Double leng = (maxX - minX);
	Double ratio = leng / (Double)(max - min);
	UOSInt i = 0;
	UOSInt j = data->GetCount();
	UnsafeArray<Int32> iArr = NN<Int32Data>::ConvertFrom(data)->GetData();
	while (i < j)
	{
		pos[i].x = minX + (iArr[i] - min) * ratio;
		i++;
	}
}

void Data::ChartPlotter::Int32Axis::CalcY(NN<ChartData> data, UnsafeArray<Math::Coord2DDbl> pos, Double minY, Double maxY) const
{
	Double leng = (maxY - minY);
	Double ratio = leng / (Double)(max - min);
	UOSInt i = 0;
	UOSInt j = data->GetCount();
	UnsafeArray<Int32> iArr = NN<Int32Data>::ConvertFrom(data)->GetData();
	while (i < j)
	{
		pos[i].y = minY + (iArr[i] - min) * ratio;
		i++;
	}
}

void Data::ChartPlotter::Int32Axis::ExtendRange(NN<Int32Data> data)
{
	UnsafeArray<Int32> dataArr = data->GetData();
	Int32 v;
	UOSInt i = 0;
	UOSInt j = data->GetCount();
	while (i < j)
	{
		v = dataArr[i];
		if (v < min) min = v;
		if (v > max) max = v;
		i++;
	}
}

void Data::ChartPlotter::Int32Axis::ExtendRange(Int32 v)
{
	if (v < min) min = v;
	if (v > max) max = v;
}

Data::ChartPlotter::UInt32Axis::UInt32Axis(NN<UInt32Data> data)
{
	UnsafeArray<UInt32> dataArr = data->GetData();
	if (data->GetCount() > 0)
	{
		this->min = this->max = dataArr[0];
		this->ExtendRange(data);
	}
	else
	{
		this->min = 0;
		this->max = this->min;
	}
}

Data::ChartPlotter::UInt32Axis::~UInt32Axis()
{
}

Data::ChartPlotter::DataType Data::ChartPlotter::UInt32Axis::GetType() const
{
	return DataType::UInteger;
}

void Data::ChartPlotter::UInt32Axis::CalcX(NN<ChartData> data, UnsafeArray<Math::Coord2DDbl> pos, Double minX, Double maxX) const
{
	Double leng = (maxX - minX);
	Double ratio = leng / (Double)(max - min);
	UOSInt i = 0;
	UOSInt j = data->GetCount();
	UnsafeArray<UInt32> iArr = NN<UInt32Data>::ConvertFrom(data)->GetData();
	while (i < j)
	{
		pos[i].x = minX + (iArr[i] - min) * ratio;
		i++;
	}
}

void Data::ChartPlotter::UInt32Axis::CalcY(NN<ChartData> data, UnsafeArray<Math::Coord2DDbl> pos, Double minY, Double maxY) const
{
	Double leng = (maxY - minY);
	Double ratio = leng / (Double)(max - min);
	UOSInt i = 0;
	UOSInt j = data->GetCount();
	UnsafeArray<UInt32> iArr = NN<UInt32Data>::ConvertFrom(data)->GetData();
	while (i < j)
	{
		pos[i].y = minY + (iArr[i] - min) * ratio;
		i++;
	}
}

void Data::ChartPlotter::UInt32Axis::ExtendRange(NN<UInt32Data> data)
{
	UnsafeArray<UInt32> dataArr = data->GetData();
	UInt32 v;
	UOSInt i = 0;
	UOSInt j = data->GetCount();
	while (i < j)
	{
		v = dataArr[i];
		if (v < min) min = v;
		if (v > max) max = v;
		i++;
	}
}

void Data::ChartPlotter::UInt32Axis::ExtendRange(UInt32 v)
{
	if (v < min) min = v;
	if (v > max) max = v;
}

Data::ChartPlotter::DoubleAxis::DoubleAxis(NN<DoubleData> data)
{
	UnsafeArray<Double> dataArr = data->GetData();
	if (data->GetCount() > 0)
	{
		this->min = this->max = dataArr[0];
		this->ExtendRange(data);
	}
	else
	{
		this->min = 0;
		this->max = this->min;
	}
}

Data::ChartPlotter::DoubleAxis::~DoubleAxis()
{

}

Data::ChartPlotter::DataType Data::ChartPlotter::DoubleAxis::GetType() const
{
	return DataType::DOUBLE;
}

void Data::ChartPlotter::DoubleAxis::CalcX(NN<ChartData> data, UnsafeArray<Math::Coord2DDbl> pos, Double minX, Double maxX) const
{
	Double leng = (maxX - minX);
	Double ratio = leng / (Double)(max - min);
	UOSInt i = 0;
	UOSInt j = data->GetCount();
	UnsafeArray<Double> dArr = NN<DoubleData>::ConvertFrom(data)->GetData();
	while (i < j)
	{
		pos[i].x = minX + (dArr[i] - min) * ratio;
		i++;
	}
}

void Data::ChartPlotter::DoubleAxis::CalcY(NN<ChartData> data, UnsafeArray<Math::Coord2DDbl> pos, Double minY, Double maxY) const
{
	Double leng = (maxY - minY);
	Double ratio = leng / (Double)(max - min);
	UOSInt i = 0;
	UOSInt j = data->GetCount();
	UnsafeArray<Double> dArr = NN<DoubleData>::ConvertFrom(data)->GetData();
	while (i < j)
	{
		pos[i].y = minY + (dArr[i] - min) * ratio;
		i++;
	}
}

void Data::ChartPlotter::DoubleAxis::ExtendRange(NN<DoubleData> data)
{
	UnsafeArray<Double> dataArr = data->GetData();
	Double v;
	UOSInt i = 0;
	UOSInt j = data->GetCount();
	while (i < j)
	{
		v = dataArr[i];
		if (v < min) min = v;
		if (v > max) max = v;
		i++;
	}
}

void Data::ChartPlotter::DoubleAxis::ExtendRange(Double v)
{
	if (v < min) min = v;
	if (v > max) max = v;
}

Data::ChartPlotter::ChartParam::ChartParam(NN<Text::String> name, NN<ChartData> yData, NN<Axis> yAxis, NN<ChartData> xData, UInt32 lineColor, UInt32 fillColor, ChartType chartType)
{
	this->name = name->Clone();
	this->yData = yData;
	this->yAxis = yAxis;
	this->xData = xData;
	this->lineColor = lineColor;
	this->fillColor = fillColor;
	this->chartType = chartType;
	this->labels = 0;
}

Data::ChartPlotter::ChartParam::ChartParam(Text::CStringNN name, NN<ChartData> yData, NN<Axis> yAxis, NN<ChartData> xData, UInt32 lineColor, UInt32 fillColor, ChartType chartType)
{
	this->name = Text::String::New(name);
	this->yData = yData;
	this->yAxis = yAxis;
	this->xData = xData;
	this->lineColor = lineColor;
	this->fillColor = fillColor;
	this->chartType = chartType;
	this->labels = 0;
}

Data::ChartPlotter::ChartParam::~ChartParam()
{
	UnsafeArray<Optional<Text::String>> labels;
	if (this->labels.SetTo(labels))
	{
		UOSInt i = this->yData->GetCount();
		while (i-- > 0)
		{
			OPTSTR_DEL(labels[i]);
		}
		MemFreeArr(labels);
	}
	this->name->Release();
	this->yData.Delete();
	this->xData.Delete();
	
}

Optional<Data::ChartPlotter::Axis> Data::ChartPlotter::GetXAxis(NN<ChartData> data)
{
	NN<Axis> axis;
	if (this->xAxis.SetTo(axis))
	{
		if (axis->GetType() != data->GetType())
			return 0;
		if (axis->GetType() == DataType::Integer)
		{
			NN<Int32Axis>::ConvertFrom(axis)->ExtendRange(NN<Int32Data>::ConvertFrom(data));
			return axis;
		}
		else if (axis->GetType() == DataType::UInteger)
		{
			NN<UInt32Axis>::ConvertFrom(axis)->ExtendRange(NN<UInt32Data>::ConvertFrom(data));
			return axis;
		}
		else if (axis->GetType() == DataType::DOUBLE)
		{
			NN<DoubleAxis>::ConvertFrom(axis)->ExtendRange(NN<DoubleData>::ConvertFrom(data));
			return axis;
		}
		else if (axis->GetType() == DataType::Time)
		{
			NN<TimeAxis>::ConvertFrom(axis)->ExtendRange(NN<TimeData>::ConvertFrom(data));
			return axis;
		}
		return 0;
	}
	this->xAxis = NewAxis(data);
	if (this->xAxis.SetTo(axis))
	{
		axis->SetLabelRotate(90);
	}
	return this->xAxis;
}

Optional<Data::ChartPlotter::Axis> Data::ChartPlotter::GetYAxis(NN<ChartData> data)
{
	NN<Axis> axis;
	if (!this->y1Axis.SetTo(axis))
	{
		this->y1Axis = NewAxis(data);
		return this->y1Axis;
	}
	else if (axis->GetType() == data->GetType())
	{
		if (axis->GetType() == DataType::Integer)
		{
			NN<Int32Axis>::ConvertFrom(axis)->ExtendRange(NN<Int32Data>::ConvertFrom(data));
			return axis;
		}
		else if (axis->GetType() == DataType::UInteger)
		{
			NN<UInt32Axis>::ConvertFrom(axis)->ExtendRange(NN<UInt32Data>::ConvertFrom(data));
			return axis;
		}
		else if (axis->GetType() == DataType::DOUBLE)
		{
			NN<DoubleAxis>::ConvertFrom(axis)->ExtendRange(NN<DoubleData>::ConvertFrom(data));
			return axis;
		}
		else if (axis->GetType() == DataType::Time)
		{
			NN<TimeAxis>::ConvertFrom(axis)->ExtendRange(NN<TimeData>::ConvertFrom(data));
			return axis;
		}
		return 0;
	}
	else if (!this->y2Axis.SetTo(axis))
	{
		this->y2Axis = NewAxis(data);
		return this->y2Axis;
	}
	else if (axis->GetType() == data->GetType())
	{
		if (axis->GetType() == DataType::Integer)
		{
			NN<Int32Axis>::ConvertFrom(axis)->ExtendRange(NN<Int32Data>::ConvertFrom(data));
			return axis;
		}
		else if (axis->GetType() == DataType::UInteger)
		{
			NN<UInt32Axis>::ConvertFrom(axis)->ExtendRange(NN<UInt32Data>::ConvertFrom(data));
			return axis;
		}
		else if (axis->GetType() == DataType::DOUBLE)
		{
			NN<DoubleAxis>::ConvertFrom(axis)->ExtendRange(NN<DoubleData>::ConvertFrom(data));
			return axis;
		}
		else if (axis->GetType() == DataType::Time)
		{
			NN<TimeAxis>::ConvertFrom(axis)->ExtendRange(NN<TimeData>::ConvertFrom(data));
			return axis;
		}
		return 0;
	}
	return 0;
}

Data::ChartPlotter::ChartPlotter(Text::CString title)
{
	this->title = 0;

	this->timeFormat = Text::String::New(UTF8STRC("HH:mm"));
	this->dateFormat = Text::String::New(UTF8STRC("yyyy/MM/dd"));
	this->dblFormat = Text::String::New(UTF8STRC("0.00"));
	this->minDblVal = 0.01;

	this->titleBuff = 0;
	this->SetTitle(title);
	this->xAxis = 0;
	this->y1Axis = 0;
	this->y2Axis = 0;
	this->refTime = {0, 0};
	this->timeZoneQHR = 0;
	this->barLength = 3.0;
	this->pointType = PointType::Null;
	this->pointSize = 0;
	this->yUnit = 0;
	Data::DateTime dt;
	dt.ToLocalTime();
	this->timeZoneQHR = dt.GetTimeZoneQHR();

	bgColor = 0xffffffff;
	boundColor = 0xff000000;
	fontColor = 0xff000000;
	gridColor = 0xffebebeb;
	refLineColor = 0xffff0000;
	this->lineThick = 1.0;
    
	this->fntName = Text::String::New(UTF8STRC("SimHei"));
	fntSizePt = 12.0;
	
	this->refDbl = 0;
	this->refInt = 0;
	this->refTime = {0, 0};
	this->refExist = false;
}

Data::ChartPlotter::~ChartPlotter()
{
	OPTSTR_DEL(this->title);

	this->dateFormat->Release();
	this->timeFormat->Release();
	this->dblFormat->Release();
	this->charts.DeleteAll();
	
	this->xAxis.Delete();
	this->y1Axis.Delete();
	this->y2Axis.Delete();

	OPTSTR_DEL(this->yUnit);
	OPTSTR_DEL(this->titleBuff);
	this->fntName->Release();
}

void Data::ChartPlotter::SetFontHeightPt(Double ptSize)
{
	if (ptSize > 0)
		fntSizePt = ptSize;
}

void Data::ChartPlotter::SetFontName(Text::CStringNN name)
{
	this->fntName->Release();
	this->fntName = Text::String::New(name);
}

void Data::ChartPlotter::SetYRefVal(Int32 refVal, UInt32 col)
{
	this->refInt = refVal;
	this->refLineColor = col;
	this->refExist = true;
}

void Data::ChartPlotter::SetYRefVal(Double refVal, UInt32 col)
{
	this->refDbl = refVal;
	this->refLineColor = col;
	this->refExist = true;
}

void Data::ChartPlotter::SetYRefVal(NN<Data::DateTime> refVal, UInt32 col)
{
	this->refTime = refVal->ToInstant();
	this->refLineColor = col;
	this->refExist = true;
}

void Data::ChartPlotter::SetYRefType(RefType refType)
{
	this->refType = refType;
}

void Data::ChartPlotter::SetYUnit(Text::CString yUnit)
{
	OPTSTR_DEL(this->yUnit);
	this->yUnit = Text::String::NewOrNull(yUnit);
}

void Data::ChartPlotter::SetLineThick(Double lineThick)
{
	this->lineThick = lineThick;
}

void Data::ChartPlotter::SetTimeZoneQHR(Int8 timeZoneQHR)
{
	this->timeZoneQHR = timeZoneQHR;
}

void Data::ChartPlotter::SetBarLength(Double barLength)
{
	this->barLength = barLength;
}

void Data::ChartPlotter::SetPointType(PointType pointType, Double pointSize)
{
	this->pointType = pointType;
	this->pointSize = pointSize;
}

UInt32 Data::ChartPlotter::GetRndColor()
{
	UInt32 r;
	UInt32 g;
	UInt32 b;
	
	r = (UInt32)(64 + (this->rnd.NextInt15() % 192));
	g = (UInt32)(64 + (this->rnd.NextInt15() % 192));
	b = 512 - r - g;
	if (b < 0)
		b = 0;
	else if (b > 255)
		b = 255;
	return 0xff000000 | (r << 16) | (g << 8) | b;
}

Bool Data::ChartPlotter::AddLineChart(NN<Text::String> name, NN<ChartData> yData, NN<ChartData> xData, UInt32 lineColor)
{
	NN<Axis> xAxis;
	NN<Axis> yAxis;
	if (!GetXAxis(xData).SetTo(xAxis) || !GetYAxis(yData).SetTo(yAxis))
	{
		yData.Delete();
		xData.Delete();
		return false;
	}
	NN<ChartParam> chart;
	NEW_CLASSNN(chart, ChartParam(name, yData, yAxis, xData, lineColor, 0, ChartType::Line));
	this->charts.Add(chart);
	return true;
}

Bool Data::ChartPlotter::AddLineChart(Text::CStringNN name, NN<ChartData> yData, NN<ChartData> xData, UInt32 lineColor)
{
	NN<Axis> xAxis;
	NN<Axis> yAxis;
	if (!GetXAxis(xData).SetTo(xAxis) || !GetYAxis(yData).SetTo(yAxis) || xData->GetCount() != yData->GetCount())
	{
		yData.Delete();
		xData.Delete();
		return false;
	}
	NN<ChartParam> chart;
	NEW_CLASSNN(chart, ChartParam(name, yData, yAxis, xData, lineColor, 0, ChartType::Line));
	this->charts.Add(chart);
	return true;
}

Bool Data::ChartPlotter::AddFilledLineChart(NN<Text::String> name, NN<ChartData> yData, NN<ChartData> xData, UInt32 lineColor, UInt32 fillColor)
{
	NN<Axis> xAxis;
	NN<Axis> yAxis;
	if (!GetXAxis(xData).SetTo(xAxis) || !GetYAxis(yData).SetTo(yAxis) || xData->GetCount() != yData->GetCount())
	{
		yData.Delete();
		xData.Delete();
		return false;
	}
	NN<ChartParam> chart;
	NEW_CLASSNN(chart, ChartParam(name, yData, yAxis, xData, lineColor, fillColor, ChartType::FilledLine));
	this->charts.Add(chart);
	return true;
}

Bool Data::ChartPlotter::AddFilledLineChart(Text::CStringNN name, NN<ChartData> yData, NN<ChartData> xData, UInt32 lineColor, UInt32 fillColor)
{
	NN<Axis> xAxis;
	NN<Axis> yAxis;
	if (!GetXAxis(xData).SetTo(xAxis) || !GetYAxis(yData).SetTo(yAxis) || xData->GetCount() != yData->GetCount())
	{
		yData.Delete();
		xData.Delete();
		return false;
	}
	NN<ChartParam> chart;
	NEW_CLASSNN(chart, ChartParam(name, yData, yAxis, xData, lineColor, fillColor, ChartType::FilledLine));
	this->charts.Add(chart);
	return true;
}

Bool Data::ChartPlotter::AddScatter(Text::CStringNN name, NN<ChartData> yData, NN<ChartData> xData, UInt32 lineColor)
{
	return AddScatter(name, yData, xData, 0, lineColor);
}

Bool Data::ChartPlotter::AddScatter(Text::CStringNN name, NN<ChartData> yData, NN<ChartData> xData, UnsafeArrayOpt<Optional<Text::String>> labels, UInt32 lineColor)
{
	NN<Axis> xAxis;
	NN<Axis> yAxis;
	if (!GetXAxis(xData).SetTo(xAxis) || !GetYAxis(yData).SetTo(yAxis) || xData->GetCount() != yData->GetCount())
	{
		yData.Delete();
		xData.Delete();
		return false;
	}
	UnsafeArray<Optional<Text::String>> nnlabels;
	UnsafeArray<Optional<Text::String>> chartlabels;
	NN<ChartParam> chart;
	NEW_CLASSNN(chart, ChartParam(name, yData, yAxis, xData, lineColor, 0, ChartType::Scatter));
	if (labels.SetTo(nnlabels))
	{
		chartlabels = MemAllocArr(Optional<Text::String>, yData->GetCount());
		MemCopyNO(chartlabels.Ptr(), nnlabels.Ptr(), yData->GetCount() * sizeof(Optional<Text::String>));
		chart->labels = chartlabels;
	}
	this->charts.Add(chart);
	if (this->pointType == PointType::Null)
	{
		this->pointType = PointType::Circle;
	}
	return true;
}

Bool Data::ChartPlotter::AddHistogramCount(Text::CStringNN name, NN<ChartData> data, UOSInt barCount, UInt32 lineColor, UInt32 fillColor)
{
	NN<ChartData> xData;
	NN<ChartData> yData;
	UOSInt i;
	Double dmin;
	Double dmax;
	if (data->GetType() == DataType::Integer)
	{
		NN<Int32Data> vdata = NN<Int32Data>::ConvertFrom(data);
		UnsafeArray<Int32> dataArr = vdata->GetData();
		UOSInt dataCnt = vdata->GetCount();
		Int32 min = dataArr[0];
		Int32 max = min;
		i = 1;
		while (i < dataCnt)
		{
			if (dataArr[i] < min) min = dataArr[i];
			if (dataArr[i] > max) max = dataArr[i];
			i++;
		}
		dmin = (Double)min;
		dmax = (Double)max;
		Double interval = (dmax - dmin) / (Double)barCount;
		UnsafeArray<Double> valArr = MemAllocArr(Double, barCount + 1);
		UnsafeArray<UInt32> cntArr = MemAllocArr(UInt32, barCount + 1);
		i = 0;
		while (i < barCount)
		{
			cntArr[i] = 0;
			valArr[i] = dmin + interval * (Double)(i + 1);
			i++;
		}
		cntArr[barCount] = 0;
		i = 0;
		while (i < dataCnt)
		{
			Double v = (Double)dataArr[i];
			cntArr[(Int32)((v - dmin) / interval)]++;
			i++;
		}
		cntArr[barCount - 1] += cntArr[barCount];
		xData = NewData(valArr, barCount);
		yData = NewData(cntArr, barCount);
		MemFreeArr(valArr);
		MemFreeArr(cntArr);
	}
	else if (data->GetType() == DataType::UInteger)
	{
		NN<UInt32Data> vdata = NN<UInt32Data>::ConvertFrom(data);
		UnsafeArray<UInt32> dataArr = vdata->GetData();
		UOSInt dataCnt = vdata->GetCount();
		UInt32 min = dataArr[0];
		UInt32 max = min;
		i = 1;
		while (i < dataCnt)
		{
			if (dataArr[i] < min) min = dataArr[i];
			if (dataArr[i] > max) max = dataArr[i];
			i++;
		}
		dmin = (Double)min;
		dmax = (Double)max;
		printf("min = %lf, max = %lf\r\n", dmin, dmax);
		Double interval = (dmax - dmin) / (Double)barCount;
		UnsafeArray<Double> valArr = MemAllocArr(Double, barCount + 1);
		UnsafeArray<UInt32> cntArr = MemAllocArr(UInt32, barCount + 1);
		i = 0;
		while (i < barCount)
		{
			cntArr[i] = 0;
			valArr[i] = dmin + interval * (Double)(i + 1);
			i++;
		}
		cntArr[barCount] = 0;
		i = 0;
		while (i < dataCnt)
		{
			Double v = (Double)dataArr[i];
			cntArr[(Int32)((v - dmin) / interval)]++;
			i++;
		}
		cntArr[barCount - 1] += cntArr[barCount];
		xData = NewData(valArr, barCount);
		yData = NewData(cntArr, barCount);
		MemFreeArr(valArr);
		MemFreeArr(cntArr);
	}
	else if (data->GetType() == DataType::DOUBLE)
	{
		NN<DoubleData> vdata = NN<DoubleData>::ConvertFrom(data);
		UnsafeArray<Double> dataArr = vdata->GetData();
		UOSInt dataCnt = vdata->GetCount();
		dmin = dataArr[0];
		dmax = dmin;
		i = 1;
		while (i < dataCnt)
		{
			if (dataArr[i] < dmin) dmin = dataArr[i];
			if (dataArr[i] > dmax) dmax = dataArr[i];
			i++;
		}
		Double interval = (dmax - dmin) / (Double)barCount;
		UnsafeArray<Double> valArr = MemAllocArr(Double, barCount + 1);
		UnsafeArray<UInt32> cntArr = MemAllocArr(UInt32, barCount + 1);
		i = 0;
		while (i < barCount)
		{
			cntArr[i] = 0;
			valArr[i] = dmin + interval * (Double)(i + 1);
			i++;
		}
		cntArr[barCount] = 0;
		i = 0;
		while (i < dataCnt)
		{
			Double v = dataArr[i];
			cntArr[(Int32)((v - dmin) / interval)]++;
			i++;
		}
		cntArr[barCount - 1] += cntArr[barCount];
		xData = NewData(valArr, barCount);
		yData = NewData(cntArr, barCount);
		MemFreeArr(valArr);
		MemFreeArr(cntArr);
	}
	else
	{
		data.Delete();
		return false;
	}
	data.Delete();
	NN<Axis> xAxis;
	NN<Axis> yAxis;
	if (!GetXAxis(xData).SetTo(xAxis) || !GetYAxis(yData).SetTo(yAxis))
	{
		yData.Delete();
		xData.Delete();
		return false;
	}
	NN<DoubleAxis>::ConvertFrom(xAxis)->ExtendRange(dmin);
	NN<ChartParam> chart;
	NEW_CLASSNN(chart, ChartParam(name, yData, yAxis, xData, lineColor, fillColor, ChartType::Histogram));
	this->charts.Add(chart);
	return true;
}

void Data::ChartPlotter::SetXRangeDate(NN<Data::DateTime> xVal)
{
	NN<Axis> axis;
	if (!this->xAxis.SetTo(axis))
	{
		NN<TimeAxis> tAxis;
		NEW_CLASSNN(tAxis, TimeAxis(xVal->ToInstant()));
		this->xAxis = tAxis;
	}
	else if (axis->GetType() == DataType::Time)
	{
		NN<TimeAxis>::ConvertFrom(axis)->ExtendRange(xVal->ToInstant());
	}
}

void Data::ChartPlotter::SetYRangeInt(Int32 yVal)
{
	NN<Axis> axis;
	if (this->y1Axis.SetTo(axis) && axis->GetType() == DataType::Integer)
	{
		NN<Int32Axis>::ConvertFrom(axis)->ExtendRange(yVal);
	}
	else if (this->y2Axis.SetTo(axis) && axis->GetType() == DataType::Integer)
	{
		NN<Int32Axis>::ConvertFrom(axis)->ExtendRange(yVal);
	}
}

void Data::ChartPlotter::SetYRangeDbl(Double yVal)
{
	NN<Axis> axis;
	if (this->y1Axis.SetTo(axis) && axis->GetType() == DataType::DOUBLE)
	{
		NN<DoubleAxis>::ConvertFrom(axis)->ExtendRange(yVal);
	}
	else if (this->y2Axis.SetTo(axis) && axis->GetType() == DataType::DOUBLE)
	{
		NN<DoubleAxis>::ConvertFrom(axis)->ExtendRange(yVal);
	}
}

/*void Data::LineChart::SetStyle(Data::LineChart::LineStyle style)
{
	this->style = style;
}*/

void Data::ChartPlotter::SetTitle(Text::CString title)
{
	NN<Text::String> s;
	OPTSTR_DEL(this->title);
	this->title = Text::String::NewOrNull(title);

	OPTSTR_DEL(this->titleBuff);
	if (!this->title.SetTo(s))
	{
		this->titleBuff = 0;
		this->titleLineCnt = 0;
	}
	else
	{
		this->titleBuff = s = Text::String::New(s->v, s->leng);
		this->titleLineCnt = Text::StrSplitLineP(this->titleLine, 3, s.Ptr()[0]);
	}
}

Optional<Text::String> Data::ChartPlotter::GetTitle() const
{
	return this->title;
}

void Data::ChartPlotter::SetDateFormat(Text::CStringNN format)
{
	this->dateFormat->Release();
	this->dateFormat = Text::String::New(format);
}

NN<Text::String> Data::ChartPlotter::GetDateFormat() const
{
	return this->dateFormat;
}

void Data::ChartPlotter::SetTimeFormat(Text::CStringNN format)
{
	this->timeFormat->Release();
	this->timeFormat = Text::String::New(format);
}

NN<Text::String> Data::ChartPlotter::GetTimeFormat() const
{
	return this->timeFormat;
}

void Data::ChartPlotter::SetDblFormat(Text::CStringNN format)
{
	this->dblFormat->Release();
	this->dblFormat = Text::String::New(format);
	UOSInt i = format.IndexOf('.');
	if (i == INVALID_INDEX)
	{
		this->minDblVal = 1.0;
	}
	else
	{
		i = format.leng - i - 1;
		this->minDblVal = 1.0;
		while (i-- > 0)
		{
			this->minDblVal = this->minDblVal * 0.1;
		}
	}
}

NN<Text::String> Data::ChartPlotter::GetDblFormat() const
{
	return this->dblFormat;
}

void Data::ChartPlotter::SetXAxisName(Text::CString xAxisName)
{
	NN<Axis> axis;
	if (this->xAxis.SetTo(axis)) axis->SetName(xAxisName);
}

Optional<Text::String> Data::ChartPlotter::GetXAxisName() const
{
	NN<Axis> axis;
	if (this->xAxis.SetTo(axis)) return axis->GetName();
	return 0;
}

void Data::ChartPlotter::SetY1AxisName(Text::CString y1AxisName)
{
	NN<Axis> axis;
	if (this->y1Axis.SetTo(axis)) axis->SetName(y1AxisName);
}

Optional<Text::String> Data::ChartPlotter::GetY1AxisName() const
{
	NN<Axis> axis;
	if (this->y1Axis.SetTo(axis)) return axis->GetName();
	return 0;
}

void Data::ChartPlotter::SetY2AxisName(Text::CString y2AxisName)
{
	NN<Axis> axis;
	if (this->y2Axis.SetTo(axis)) axis->SetName(y2AxisName);
}

Optional<Text::String> Data::ChartPlotter::GetY2AxisName() const
{
	NN<Axis> axis;
	if (this->y2Axis.SetTo(axis)) return axis->GetName();
	return 0;
}

Optional<Data::ChartPlotter::Axis> Data::ChartPlotter::GetXAxis() const
{
	return this->xAxis;
}

Optional<Data::ChartPlotter::Axis> Data::ChartPlotter::GetY1Axis() const
{
	return this->y1Axis;
}

Optional<Data::ChartPlotter::Axis> Data::ChartPlotter::GetY2Axis() const
{
	return this->y2Axis;
}

Data::ChartPlotter::DataType Data::ChartPlotter::GetXAxisType() const
{
	NN<Axis> axis;
	if (this->xAxis.SetTo(axis)) return axis->GetType();
	return DataType::None;
}

UOSInt Data::ChartPlotter::GetChartCount() const
{
	return this->charts.GetCount();
}

Optional<Data::ChartPlotter::ChartParam> Data::ChartPlotter::GetChart(UOSInt index) const
{
	return this->charts.GetItem(index);
}

void Data::ChartPlotter::Plot(NN<Media::DrawImage> img, Double x, Double y, Double width, Double height)
{
	if (height <= 0 || width <= 0)
		return;
	NN<Axis> xAxis;
	NN<Axis> y1Axis;
	if (!this->xAxis.SetTo(xAxis) || !this->y1Axis.SetTo(y1Axis))
		return;

	NN<Media::DrawFont> fnt;
	Double fntH;
	Double barLeng = this->barLength;
	Double xLeng;
	Double y1Leng;
	Double y2Leng;
	Bool y2show;
	UTF8Char sbuff[256];
	UnsafeArray<UTF8Char> sptr;
	NN<Text::String> s;

	UOSInt i;
	UOSInt j;
	Data::DateTime dt1;
	Data::DateTime dt2;

	NN<Media::DrawBrush> bgBrush = img->NewBrushARGB(bgColor);
	NN<Media::DrawPen> boundPen = img->NewPenARGB(boundColor, this->lineThick, 0, 0);
	NN<Media::DrawBrush> fontBrush = img->NewBrushARGB(fontColor);
	NN<Media::DrawPen> gridPen = img->NewPenARGB(gridColor, this->lineThick, 0, 0);
	NN<Media::DrawPen> refLinePen = img->NewPenARGB(refLineColor, this->lineThick, 0, 0);

	fnt = img->NewFontPt(fntName->ToCString(), (Double)fntSizePt, Media::DrawEngine::DFS_ANTIALIAS, 0);
	img->DrawRect(Math::Coord2DDbl(x, y), Math::Size2DDbl(width, height), 0, bgBrush);

	Math::Size2DDbl rcSize = img->GetTextSize(fnt, CSTR("AA"));
	fntH = rcSize.y;
	if (this->titleLineCnt > 0)
	{
		i = 0;
		while (i < this->titleLineCnt)
		{
			rcSize = img->GetTextSize(fnt, this->titleLine[i].ToCString());
			img->DrawString(Math::Coord2DDbl((x + (width / 2) - (rcSize.x * 0.5)), y), this->titleLine[i].ToCString(), fnt, fontBrush);
			y += fntH;
			height -= fntH;
			i++;
		}
	}

	Double minXInt = fntH;
	UOSInt xMode;
	Double labelRotate = xAxis->GetLabelRotate();
	if (labelRotate < 45)
	{
		xMode = 1;
	}
	else
	{
		xMode = 0;
	}
	Double sRotate = Math_Sin(labelRotate * Math::PI / 180.0);
	Double cRotate = Math_Cos(labelRotate * Math::PI / 180.0);
	Double rotLeng;
	xLeng = 0;
	y1Leng = 0;
	y2Leng = 0;
	if (xAxis->GetType() == DataType::Integer)
	{
		NN<Int32Axis> iAxis = NN<Int32Axis>::ConvertFrom(xAxis);
		sptr = Text::StrInt32(sbuff, iAxis->GetMax());
		rcSize = img->GetTextSize(fnt, CSTRP(sbuff, sptr));
		if (xMode == 1)
		{
			rotLeng = rcSize.x * 0.5 * sRotate + rcSize.y * cRotate;
			if (rcSize.x > minXInt) minXInt = rcSize.x;
		}
		else
		{
			rotLeng = rcSize.x * sRotate + rcSize.y * 0.5 * cRotate;
		}
		xLeng = rotLeng;

		sptr = Text::StrInt32(sbuff, iAxis->GetMin());
		rcSize = img->GetTextSize(fnt, CSTRP(sbuff, sptr));
		if (xMode == 1)
		{
			rotLeng = rcSize.x * 0.5 * sRotate + rcSize.y * cRotate;
			if (rcSize.x > minXInt) minXInt = rcSize.x;
		}
		else
		{
			rotLeng = rcSize.x * sRotate + rcSize.y * 0.5 * cRotate;
		}
		if (rotLeng > xLeng)
			xLeng = rotLeng;
	}
	else if (xAxis->GetType() == DataType::UInteger)
	{
		NN<UInt32Axis> iAxis = NN<UInt32Axis>::ConvertFrom(xAxis);
		sptr = Text::StrUInt32(sbuff, iAxis->GetMax());
		rcSize = img->GetTextSize(fnt, CSTRP(sbuff, sptr));
		if (xMode == 1)
		{
			rotLeng = rcSize.x * 0.5 * sRotate + rcSize.y * cRotate;
			if (rcSize.x > minXInt) minXInt = rcSize.x;
		}
		else
		{
			rotLeng = rcSize.x * sRotate + rcSize.y * 0.5 * cRotate;
		}
		xLeng = rotLeng;

		sptr = Text::StrUInt32(sbuff, iAxis->GetMin());
		rcSize = img->GetTextSize(fnt, CSTRP(sbuff, sptr));
		if (xMode == 1)
		{
			rotLeng = rcSize.x * 0.5 * sRotate + rcSize.y * cRotate;
			if (rcSize.x > minXInt) minXInt = rcSize.x;
		}
		else
		{
			rotLeng = rcSize.x * sRotate + rcSize.y * 0.5 * cRotate;
		}
		if (rotLeng > xLeng)
			xLeng = rotLeng;
	}
	else if (xAxis->GetType() == DataType::DOUBLE)
	{
		NN<DoubleAxis> dAxis = NN<DoubleAxis>::ConvertFrom(xAxis);
		sptr = Text::StrDoubleFmt(sbuff, dAxis->GetMax(), UnsafeArray<const Char>::ConvertFrom(this->dblFormat->v));
		rcSize = img->GetTextSize(fnt, CSTRP(sbuff, sptr));
		if (xMode == 1)
		{
			rotLeng = rcSize.x * 0.5 * sRotate + rcSize.y * cRotate;
			if (rcSize.x > minXInt) minXInt = rcSize.x;
		}
		else
		{
			rotLeng = rcSize.x * sRotate + rcSize.y * 0.5 * cRotate;
		}
		xLeng = rotLeng;
		
		sptr = Text::StrDoubleFmt(sbuff, dAxis->GetMin(), UnsafeArray<const Char>::ConvertFrom(this->dblFormat->v));
		rcSize = img->GetTextSize(fnt, CSTRP(sbuff, sptr));
		if (xMode == 1)
		{
			rotLeng = rcSize.x * 0.5 * sRotate + rcSize.y * cRotate;
			if (rcSize.x > minXInt) minXInt = rcSize.x;
		}
		else
		{
			rotLeng = rcSize.x * sRotate + rcSize.y * 0.5 * cRotate;
		}
		if (rotLeng > xLeng)
			xLeng = rotLeng;
	}
	else if (xAxis->GetType() == DataType::Time)
	{
		NN<TimeAxis> tAxis = NN<TimeAxis>::ConvertFrom(xAxis);
		dt1.SetInstant(tAxis->GetMax());
		dt1.ConvertTimeZoneQHR(this->timeZoneQHR);
		dt2.SetInstant(tAxis->GetMin());
		dt2.ConvertTimeZoneQHR(this->timeZoneQHR);
		if (dt1.IsSameDay(dt2))
		{
			sptr = dt1.ToString(sbuff, UnsafeArray<const Char>::ConvertFrom(this->timeFormat->v));
			rcSize = img->GetTextSize(fnt, CSTRP(sbuff, sptr));
			if (xMode == 1)
			{
				rotLeng = rcSize.x * 0.5 * sRotate + rcSize.y * cRotate;
				if (rcSize.x > minXInt) minXInt = rcSize.x;
			}
			else
			{
				rotLeng = rcSize.x * sRotate + rcSize.y * 0.5 * cRotate;
			}
			xLeng = rotLeng;
			if (dt2.GetMSPassedDate() == 0)
			{
				sptr = dt2.ToString(sbuff, UnsafeArray<const Char>::ConvertFrom(this->dateFormat->v));
			}
			else
			{
				sptr = dt2.ToString(sbuff, UnsafeArray<const Char>::ConvertFrom(this->timeFormat->v));
			}
			rcSize = img->GetTextSize(fnt, CSTRP(sbuff, sptr));
			if (xMode == 1)
			{
				rotLeng = rcSize.x * 0.5 * sRotate + rcSize.y * cRotate;
				if (rcSize.x > minXInt) minXInt = rcSize.x;
			}
			else
			{
				rotLeng = rcSize.x * sRotate + rcSize.y * 0.5 * cRotate;
			}
			if (rotLeng > xLeng)
				xLeng = rotLeng;
		}
		else
		{
			sptr = dt1.ToString(sbuff, UnsafeArray<const Char>::ConvertFrom(this->dateFormat->v));
			rcSize = img->GetTextSize(fnt, CSTRP(sbuff, sptr));
			if (xMode == 1)
			{
				rotLeng = rcSize.x * 0.5 * sRotate + rcSize.y * cRotate;
				if (rcSize.x > minXInt) minXInt = rcSize.x;
			}
			else
			{
				rotLeng = rcSize.x * sRotate + rcSize.y * 0.5 * cRotate;
			}
			xLeng = rotLeng;
			sptr = dt1.ToString(sbuff, UnsafeArray<const Char>::ConvertFrom(this->timeFormat->v));
			rcSize = img->GetTextSize(fnt, CSTRP(sbuff, sptr));
			if (xMode == 1)
			{
				rotLeng = rcSize.x * 0.5 * sRotate + rcSize.y * cRotate;
				if (rcSize.x > minXInt) minXInt = rcSize.x;
			}
			else
			{
				rotLeng = rcSize.x * sRotate + rcSize.y * 0.5 * cRotate;
			}
			if (rotLeng > xLeng)
				xLeng = rotLeng;
		}
	}
	if (xAxis->GetName().SetTo(s))
	{
		rcSize = img->GetTextSize(fnt, s->ToCString());
		xLeng += rcSize.y;
	}
	xLeng += barLeng;

	if (y1Axis->GetType() == DataType::Integer)
	{
		NN<Int32Axis> iAxis = NN<Int32Axis>::ConvertFrom(y1Axis);
		sptr = Text::StrInt32(sbuff, iAxis->GetMax());
		if (this->yUnit.SetTo(s))
			sptr = s->ConcatTo(sptr);
		rcSize = img->GetTextSize(fnt, CSTRP(sbuff, sptr));
		y1Leng = rcSize.x;

		sptr = Text::StrInt32(sbuff, iAxis->GetMin());
		if (this->yUnit.SetTo(s))
			sptr = s->ConcatTo(sptr);
		rcSize = img->GetTextSize(fnt, CSTRP(sbuff, sptr));
		if (rcSize.x > y1Leng)
			y1Leng = rcSize.x;
	}
	else if (y1Axis->GetType() == DataType::UInteger)
	{
		NN<UInt32Axis> iAxis = NN<UInt32Axis>::ConvertFrom(y1Axis);
		sptr = Text::StrUInt32(sbuff, iAxis->GetMax());
		if (this->yUnit.SetTo(s))
			sptr = s->ConcatTo(sptr);
		rcSize = img->GetTextSize(fnt, CSTRP(sbuff, sptr));
		y1Leng = rcSize.x;

		sptr = Text::StrUInt32(sbuff, iAxis->GetMin());
		if (this->yUnit.SetTo(s))
			sptr = s->ConcatTo(sptr);
		rcSize = img->GetTextSize(fnt, CSTRP(sbuff, sptr));
		if (rcSize.x > y1Leng)
			y1Leng = rcSize.x;
	}
	else if (y1Axis->GetType() == DataType::DOUBLE)
	{
		NN<DoubleAxis> dAxis = NN<DoubleAxis>::ConvertFrom(y1Axis);
		sptr = Text::StrDoubleFmt(sbuff, dAxis->GetMax(), UnsafeArray<const Char>::ConvertFrom(this->dblFormat->v));
		if (this->yUnit.SetTo(s))
			sptr = s->ConcatTo(sptr);
		rcSize = img->GetTextSize(fnt, CSTRP(sbuff, sptr));
		y1Leng = rcSize.x;

		sptr = Text::StrDoubleFmt(sbuff, dAxis->GetMin(), UnsafeArray<const Char>::ConvertFrom(this->dblFormat->v));
		if (this->yUnit.SetTo(s))
			sptr = s->ConcatTo(sptr);
		rcSize = img->GetTextSize(fnt, CSTRP(sbuff, sptr));
		if (rcSize.x > y1Leng)
			y1Leng = rcSize.x;
	}
	else if (y1Axis->GetType() == DataType::Time)
	{
		NN<TimeAxis> tAxis = NN<TimeAxis>::ConvertFrom(y1Axis);
		dt1.SetInstant(tAxis->GetMax());
		dt1.ConvertTimeZoneQHR(this->timeZoneQHR);
		sptr = dt1.ToString(sbuff, UnsafeArray<const Char>::ConvertFrom(this->dateFormat->v));
		rcSize = img->GetTextSize(fnt, CSTRP(sbuff, sptr));
		y1Leng = rcSize.x;

		dt1.SetInstant(tAxis->GetMin());
		dt1.ConvertTimeZoneQHR(this->timeZoneQHR);
		sptr = dt1.ToString(sbuff, UnsafeArray<const Char>::ConvertFrom(this->dateFormat->v));
		rcSize = img->GetTextSize(fnt, CSTRP(sbuff, sptr));;
		if (rcSize.x > y1Leng)
			y1Leng = rcSize.x;
	}
	if (y1Axis->GetName().SetTo(s))
	{
		rcSize = img->GetTextSize(fnt, s->ToCString());
		y1Leng += rcSize.y;
	}
	y1Leng += barLeng;

	NN<Axis> y2Axis;
	if (this->y2Axis.SetTo(y2Axis))
	{
		if (y2Axis->GetType() == DataType::Integer)
		{
			NN<Int32Axis> iAxis = NN<Int32Axis>::ConvertFrom(iAxis);
			sptr = Text::StrInt32(sbuff, iAxis->GetMax());
			if (this->yUnit.SetTo(s))
				sptr = s->ConcatTo(sptr);
			rcSize = img->GetTextSize(fnt, CSTRP(sbuff, sptr));
			y2Leng = rcSize.x;

			sptr = Text::StrInt32(sbuff, iAxis->GetMin());
			if (this->yUnit.SetTo(s))
				sptr = s->ConcatTo(sptr);
			rcSize = img->GetTextSize(fnt, CSTRP(sbuff, sptr));
			if (rcSize.x > y2Leng)
				y2Leng = rcSize.x;

			y2Leng += barLeng;
			y2show = true;
		}
		else if (y2Axis->GetType() == DataType::UInteger)
		{
			NN<Int32Axis> iAxis = NN<Int32Axis>::ConvertFrom(iAxis);
			sptr = Text::StrInt32(sbuff, iAxis->GetMax());
			if (this->yUnit.SetTo(s))
				sptr = s->ConcatTo(sptr);
			rcSize = img->GetTextSize(fnt, CSTRP(sbuff, sptr));
			y2Leng = rcSize.x;

			sptr = Text::StrInt32(sbuff, iAxis->GetMin());
			if (this->yUnit.SetTo(s))
				sptr = s->ConcatTo(sptr);
			rcSize = img->GetTextSize(fnt, CSTRP(sbuff, sptr));
			if (rcSize.x > y2Leng)
				y2Leng = rcSize.x;

			y2Leng += barLeng;
			y2show = true;
		}
		else if (y2Axis->GetType() == DataType::DOUBLE)
		{
			NN<DoubleAxis> dAxis = NN<DoubleAxis>::ConvertFrom(y2Axis);
			sptr = Text::StrDoubleFmt(sbuff, dAxis->GetMax(), UnsafeArray<const Char>::ConvertFrom(this->dblFormat->v));
			if (this->yUnit.SetTo(s))
				sptr = s->ConcatTo(sptr);
			rcSize = img->GetTextSize(fnt, CSTRP(sbuff, sptr));
			y2Leng = rcSize.x;

			sptr = Text::StrDoubleFmt(sbuff, dAxis->GetMin(), UnsafeArray<const Char>::ConvertFrom(this->dblFormat->v));
			if (this->yUnit.SetTo(s))
				sptr = s->ConcatTo(sptr);
			rcSize = img->GetTextSize(fnt, CSTRP(sbuff, sptr));
			if (rcSize.x > y2Leng)
				y2Leng = rcSize.x;

			y2Leng += barLeng;
			y2show = true;
		}
		else if (y2Axis->GetType() == DataType::Time)
		{
			NN<TimeAxis> tAxis = NN<TimeAxis>::ConvertFrom(y2Axis);
			dt1.SetInstant(tAxis->GetMax());
			dt1.ConvertTimeZoneQHR(this->timeZoneQHR);
			sptr = dt1.ToString(sbuff, UnsafeArray<const Char>::ConvertFrom(this->dateFormat->v));
			rcSize = img->GetTextSize(fnt, CSTRP(sbuff, sptr));
			y2Leng = rcSize.x;

			dt1.SetInstant(tAxis->GetMin());
			dt1.ConvertTimeZoneQHR(this->timeZoneQHR);
			sptr = dt1.ToString(sbuff, UnsafeArray<const Char>::ConvertFrom(this->dateFormat->v));
			rcSize = img->GetTextSize(fnt, CSTRP(sbuff, sptr));
			if (rcSize.x > y2Leng)
				y2Leng = rcSize.x;

			y2Leng += barLeng;
			y2show = true;
		}
		else
		{
			y2Leng = (rcSize.y / 2.0);
			y2show = false;
		}
	}
	else
	{
		y2Leng = (rcSize.y / 2.0);
		y2show = false;
	}

	img->DrawLine((Double)(x + y1Leng), (Double)y, (Double)(x + y1Leng), (Double)(y + height - xLeng), boundPen);
	img->DrawLine((Double)(x + y1Leng), (Double)(y + height - xLeng), (Double)(x + width - y2Leng), (Double)(y + height - xLeng), boundPen);
	if (y2show)
	{
		img->DrawLine((Double)(x + width - y2Leng), (Double)y, (Double)(x + width - y2Leng), (Double)(y + height - xLeng), boundPen);
	}
	
	Data::ArrayListDbl locations;
	Data::ArrayListStringNN labels;
	if (xAxis->GetType() == DataType::Integer)
	{
		NN<Int32Axis> iAxis = NN<Int32Axis>::ConvertFrom(xAxis);
		CalScaleMarkInt(locations, labels, iAxis->GetMin(), iAxis->GetMax(), width - y1Leng - y2Leng - this->pointSize * 2, minXInt, 0);
	}
	else if (xAxis->GetType() == DataType::UInteger)
	{
		NN<UInt32Axis> iAxis = NN<UInt32Axis>::ConvertFrom(xAxis);
		CalScaleMarkUInt(locations, labels, iAxis->GetMin(), iAxis->GetMax(), width - y1Leng - y2Leng - this->pointSize * 2, minXInt, 0);
	}
	else if (xAxis->GetType() == DataType::DOUBLE)
	{
		NN<DoubleAxis> dAxis = NN<DoubleAxis>::ConvertFrom(xAxis);
		CalScaleMarkDbl(locations, labels, dAxis->GetMin(), dAxis->GetMax(), width - y1Leng - y2Leng - this->pointSize * 2, minXInt, UnsafeArray<const Char>::ConvertFrom(this->dblFormat->v), minDblVal, 0);
	}
	else if (xAxis->GetType() == DataType::Time)
	{
		NN<TimeAxis> tAxis = NN<TimeAxis>::ConvertFrom(xAxis);
		dt1.SetInstant(tAxis->GetMin());
		dt1.ConvertTimeZoneQHR(this->timeZoneQHR);
		dt2.SetInstant(tAxis->GetMax());
		dt2.ConvertTimeZoneQHR(this->timeZoneQHR);
		CalScaleMarkDate(locations, labels, dt1, dt2, width - y1Leng - y2Leng - this->pointSize * 2, minXInt, UnsafeArray<const Char>::ConvertFrom(this->dateFormat->v), UnsafeArray<const Char>::ConvertFrom(this->timeFormat->v));
	}
	else
	{
	}

	i = 0;
	while (i < locations.GetCount())
	{
		img->DrawLine((x + y1Leng + this->pointSize + locations.GetItem(i)), (y + height - xLeng), (x + y1Leng + this->pointSize + locations.GetItem(i)), (y + height - xLeng + barLeng), boundPen);
		i++;
	}

	if (xMode == 1)
	{
		i = 0;
		while (i < locations.GetCount())
		{
			s = Text::String::OrEmpty(labels.GetItem(i));
			Math::Size2DDbl strSize = img->GetTextSize(fnt, s->ToCString());
			img->DrawStringRot(Math::Coord2DDbl((x + y1Leng + this->pointSize + locations.GetItem(i)) - strSize.y * sRotate - strSize.x * 0.5 * cRotate, (y + height + barLeng) + strSize.x * 0.5 * sRotate - strSize.y * cRotate), Text::String::OrEmpty(labels.GetItem(i)), fnt, fontBrush, labelRotate);
			i += 1;
		}
	}
	else
	{
		i = 0;
		while (i < locations.GetCount())
		{
			s = Text::String::OrEmpty(labels.GetItem(i));
			Math::Size2DDbl strSize = img->GetTextSize(fnt, s->ToCString());
			img->DrawStringRot(Math::Coord2DDbl((x + y1Leng + this->pointSize + locations.GetItem(i)) - strSize.y * 0.5 * sRotate - strSize.x * cRotate, (y + height - xLeng + barLeng) + strSize.x * sRotate - strSize.y * 0.5 * cRotate), Text::String::OrEmpty(labels.GetItem(i)), fnt, fontBrush, labelRotate);
			i += 1;
		}
	}

	locations.Clear();
	i = labels.GetCount();
	while (i-- > 0)
	{
		OPTSTR_DEL(labels.GetItem(i));
	}
	labels.Clear();

	if (y1Axis->GetType() == DataType::Integer)
	{
		NN<Int32Axis> iAxis = NN<Int32Axis>::ConvertFrom(y1Axis);
		CalScaleMarkInt(locations, labels, iAxis->GetMin(), iAxis->GetMax(), height - xLeng - fntH / 2 - this->pointSize * 2, fntH, this->yUnit);
	}
	else if (y1Axis->GetType() == DataType::UInteger)
	{
		NN<UInt32Axis> iAxis = NN<UInt32Axis>::ConvertFrom(y1Axis);
		CalScaleMarkUInt(locations, labels, iAxis->GetMin(), iAxis->GetMax(), height - xLeng - fntH / 2 - this->pointSize * 2, fntH, this->yUnit);
	}
	else if (y1Axis->GetType() == DataType::DOUBLE)
	{
		NN<DoubleAxis> dAxis = NN<DoubleAxis>::ConvertFrom(y1Axis);
		CalScaleMarkDbl(locations, labels, dAxis->GetMin(), dAxis->GetMax(), height - xLeng - fntH / 2 - this->pointSize * 2, fntH, UnsafeArray<const Char>::ConvertFrom(this->dblFormat->v), minDblVal, this->yUnit);
	}
	else if (y1Axis->GetType() == DataType::Time)
	{
		NN<TimeAxis> tAxis = NN<TimeAxis>::ConvertFrom(y1Axis);
		dt1.SetInstant(tAxis->GetMin());
		dt1.ConvertTimeZoneQHR(this->timeZoneQHR);
		dt2.SetInstant(tAxis->GetMax());
		dt2.ConvertTimeZoneQHR(this->timeZoneQHR);
		CalScaleMarkDate(locations, labels, dt1, dt2, height - xLeng - fntH / 2 - this->pointSize * 2, fntH, UnsafeArray<const Char>::ConvertFrom(this->dateFormat->v), UnsafeArray<const Char>::ConvertFrom(this->timeFormat->v));
	}
	else
	{
	}

	i = 0;
	while (i < locations.GetCount())
	{
		if (locations.GetItem(i))
		{
			img->DrawLine((Double)(x + y1Leng), (Double)(y + height - this->pointSize - xLeng - locations.GetItem(i)), (Double)(x + width - y2Leng), (Double)(y + height - this->pointSize - xLeng - locations.GetItem(i)), gridPen);
		}
		img->DrawLine((Double)(x + y1Leng), (Double)(y + height - this->pointSize - xLeng - locations.GetItem(i)), (Double)(x + y1Leng - barLeng), (Double)(y + height - this->pointSize - xLeng - locations.GetItem(i)), boundPen);
		s = Text::String::OrEmpty(labels.GetItem(i));
		rcSize = img->GetTextSize(fnt, s->ToCString());
		img->DrawString(Math::Coord2DDbl(x + y1Leng - barLeng - rcSize.x, y + height - this->pointSize - xLeng - locations.GetItem(i) - fntH / 2), s->ToCString(), fnt, fontBrush);
		i++;
	}

	if (y1Axis->GetName().SetTo(s))
	{
		Math::Size2DDbl sz = img->GetTextSize(fnt, s->ToCString());
		img->DrawStringRot(Math::Coord2DDbl((x + fntH / 2) - sz.y * 0.5, (y + (height - xLeng) / 2) + sz.x * 0.5), s->ToCString(), fnt, fontBrush, 90);
	}

	if (xAxis->GetName().SetTo(s))
	{
		rcSize = img->GetTextSize(fnt, s->ToCString());
		img->DrawString(Math::Coord2DDbl((x + y1Leng + (width - y1Leng - y2Leng) / 2 - rcSize.x / 2), (y + height - rcSize.y)), s->ToCString(), fnt, fontBrush);
	}

	locations.Clear();
	i = labels.GetCount();
	while (i-- > 0)
	{
		OPTSTR_DEL(labels.GetItem(i));
	}
	labels.Clear();


//	System::Drawing::PointF currPos[];
	Math::Coord2DDbl *currPos;
	UOSInt currPosLen;
    
	i = 0;
	while (i < this->charts.GetCount())
	{
		NN<ChartParam> chart = this->charts.GetItemNoCheck(i);
		if (chart->chartType == ChartType::FilledLine)
		{
			currPosLen = chart->yData->GetCount() + 2;
			currPos = MemAllocA(Math::Coord2DDbl, currPosLen);
		}
		else
		{
			currPosLen = chart->yData->GetCount();
			currPos = MemAllocA(Math::Coord2DDbl, currPosLen);
		}

		Double xChartLeng = width - y1Leng - y2Leng - this->pointSize * 2.0;
		xAxis->CalcX(chart->xData, currPos, x + y1Leng + this->pointSize, x + y1Leng + this->pointSize + xChartLeng);
		xChartLeng = height - xLeng - fntH / 2 - this->pointSize * 2;
		chart->yAxis->CalcY(chart->yData, currPos, y + height - this->pointSize - xLeng, y + height - this->pointSize - xLeng - xChartLeng);

		if (chart->chartType == ChartType::FilledLine)
		{
			if (currPosLen >= 4)
			{
				j = currPosLen;
				currPos[j - 2].x = currPos[j - 3].x;
				currPos[j - 2].y = (Double)(y + height - xLeng);
				currPos[j - 1].x = currPos[0].x;
				currPos[j - 1].y = (Double)(y + height - xLeng);
				NN<Media::DrawPen> p = img->NewPenARGB(chart->lineColor, 1, 0, 0);
				NN<Media::DrawBrush> b = img->NewBrushARGB(chart->fillColor);
				img->DrawPolygon(currPos, currPosLen, p, b);
				img->DelBrush(b);
				img->DelPen(p);
			}
		}
		else if (chart->chartType == ChartType::Line)
		{
			if (currPosLen >= 2)
			{
				NN<Media::DrawPen> pen = img->NewPenARGB(chart->lineColor, this->lineThick, 0, 0);
				img->DrawPolyline(currPos, currPosLen, pen);
				img->DelPen(pen);

				if (this->pointType == PointType::Circle && this->pointSize > 0)
				{
					NN<Media::DrawBrush> b = img->NewBrushARGB(chart->lineColor);
					j = currPosLen;
					while (j-- > 0)
					{
						img->DrawEllipse(currPos[j] - this->pointSize, Math::Size2DDbl(this->pointSize * 2.0, this->pointSize * 2.0), 0, b);
					}
					img->DelBrush(b);
				}
			}
		}
		else if (chart->chartType == ChartType::Histogram)
		{
			if (currPosLen > 0)
			{
				NN<Media::DrawPen> p = img->NewPenARGB(chart->lineColor, 1, 0, 0);
				NN<Media::DrawBrush> b = img->NewBrushARGB(chart->fillColor);
				Double lastX;
				if (currPosLen >= 2)
				{
					lastX = currPos[0].x - currPos[1].x + currPos[0].x;
				}
				else
				{
					lastX = x + y1Leng + this->pointSize;
				}
				Double yBottom = y + height - this->pointSize - xLeng;
				Math::Coord2DDbl pg[5];
				j = 0;
				while (j < currPosLen)
				{
					pg[0] = Math::Coord2DDbl(lastX, yBottom);
					pg[1] = Math::Coord2DDbl(lastX, currPos[j].y);
					pg[2] = currPos[j];
					pg[3] = Math::Coord2DDbl(pg[2].x, yBottom);
					pg[4] = pg[0];
					img->DrawPolygon(pg, 5, p, b);
					lastX = pg[2].x;
					j++;
				}
				img->DelBrush(b);
				img->DelPen(p);
			}
		}
		else if (chart->chartType == ChartType::Scatter)
		{
			if (this->pointType == PointType::Circle)
			{
				Double pointSize = this->pointSize;
				if (pointSize <= 0)
				{
					pointSize = 3;
				}
				NN<Media::DrawBrush> b = img->NewBrushARGB(chart->lineColor);
				UnsafeArray<Optional<Text::String>> labels;
				NN<Text::String> s;
				if (chart->labels.SetTo(labels))
				{
					j = currPosLen;
					while (j-- > 0)
					{
						img->DrawEllipse(currPos[j] - pointSize, Math::Size2DDbl(pointSize * 2.0, pointSize * 2.0), 0, b);
						if (labels[j].SetTo(s))
						{
							img->DrawString(Math::Coord2DDbl(currPos[j].x + pointSize, currPos[j].y - fntH * 0.5), s, fnt, b);
						}
					}
				}
				else
				{
					j = currPosLen;
					while (j-- > 0)
					{
						img->DrawEllipse(currPos[j] - pointSize, Math::Size2DDbl(pointSize * 2.0, pointSize * 2.0), 0, b);
					}
				}
				img->DelBrush(b);
			}
		}


		MemFreeA(currPos);
		i += 1;
	}

	if (this->refExist)
	{
		Double xChartLeng = height - xLeng - fntH / 2;
		Int32 iMax = 0;
		Int32 iMin = 0;
		Single yPos;
		Double dMax;
		Double dMin;
		Data::TimeInstant tMax;
		Data::TimeInstant tMin;

		if (y1Axis->GetType() == DataType::Integer)
		{
			NN<Int32Axis> iAxis = NN<Int32Axis>::ConvertFrom(y1Axis);
			iMax = iAxis->GetMax();
			iMin = iAxis->GetMin();
			if (this->refInt >= iMin && this->refInt <= iMax)
			{
				yPos = (Single)(y + height - xLeng - (Double)(this->refInt - iMin) / (Single)(iMax - iMin) * xChartLeng);
				img->DrawLine((Double)(x + y1Leng), (Double)yPos, (Double)(x + width - y2Leng), (Double)yPos, refLinePen);

				sptr = Text::StrInt32(sbuff, this->refInt);
				if (this->yUnit.SetTo(s))
					sptr = s->ConcatTo(sptr);
				if (this->refType == RefType::LeftAlign)
				{
					rcSize = img->GetTextSize(fnt, CSTRP(sbuff, sptr));
					img->DrawString(Math::Coord2DDbl(x + y1Leng, yPos - rcSize.y), CSTRP(sbuff, sptr), fnt, fontBrush);
				}
				else if (this->refType == RefType::RightAlign)
				{
					rcSize = img->GetTextSize(fnt, CSTRP(sbuff, sptr));
					img->DrawString(Math::Coord2DDbl(x + width - y2Leng - rcSize.x, yPos - rcSize.y), CSTRP(sbuff, sptr), fnt, fontBrush);
				}
			}
		}
		else if (this->y2Axis.SetTo(y2Axis) && y2Axis->GetType() == DataType::Integer)
		{
			NN<Int32Axis> iAxis = NN<Int32Axis>::ConvertFrom(y2Axis);
			iMax = iAxis->GetMax();
			iMin = iAxis->GetMin();
			if (this->refInt >= iMin && this->refInt <= iMax)
			{
				yPos = (Single)(y + height - xLeng - (Double)(this->refInt - iMin) / (Single)(iMax - iMin) * xChartLeng);
				img->DrawLine((Double)(x + y1Leng), (Double)yPos, (Double)(x + width - y2Leng), (Double)yPos, refLinePen);

				sptr = Text::StrInt32(sbuff, this->refInt);
				if (this->yUnit.SetTo(s))
					sptr = s->ConcatTo(sptr);
				if (this->refType == RefType::LeftAlign)
				{
					rcSize = img->GetTextSize(fnt, CSTRP(sbuff, sptr));
					img->DrawString(Math::Coord2DDbl(x + y1Leng, yPos - rcSize.y), CSTRP(sbuff, sptr), fnt, fontBrush);
				}
				else if (this->refType == RefType::RightAlign)
				{
					rcSize = img->GetTextSize(fnt, CSTRP(sbuff, sptr));
					img->DrawString(Math::Coord2DDbl(x + width - y2Leng - rcSize.x, yPos - rcSize.y), CSTRP(sbuff, sptr), fnt, fontBrush);
				}
			}
		}

		if (y1Axis->GetType() == DataType::DOUBLE)
		{
			NN<DoubleAxis> dAxis = NN<DoubleAxis>::ConvertFrom(y1Axis);
			dMax = dAxis->GetMax();
			dMin = dAxis->GetMin();
			if (this->refDbl >= dMin && this->refDbl <= dMax)
			{
				yPos = (Single)(y + height - xLeng - (this->refDbl - dMin) / (dMax - dMin) * xChartLeng);
				img->DrawLine((Double)(x + y1Leng), (Double)yPos, (Double)(x + width - y2Leng), (Double)yPos, refLinePen);

				sptr = Text::StrDoubleFmt(sbuff, this->refDbl, UnsafeArray<const Char>::ConvertFrom(this->dblFormat->v));
				if (this->yUnit.SetTo(s))
					sptr = s->ConcatTo(sptr);
				if (this->refType == RefType::LeftAlign)
				{
					rcSize = img->GetTextSize(fnt, CSTRP(sbuff, sptr));
					img->DrawString(Math::Coord2DDbl(x + y1Leng, yPos - rcSize.y), CSTRP(sbuff, sptr), fnt, fontBrush);
				}
				else if (this->refType == RefType::RightAlign)
				{
					rcSize = img->GetTextSize(fnt, CSTRP(sbuff, sptr));
					img->DrawString(Math::Coord2DDbl(x + width - y2Leng - rcSize.x, yPos - rcSize.y), CSTRP(sbuff, sptr), fnt, fontBrush);
				}
			}
		}
		else if (this->y2Axis.SetTo(y2Axis) && y2Axis->GetType() == DataType::DOUBLE)
		{
			NN<DoubleAxis> dAxis = NN<DoubleAxis>::ConvertFrom(y2Axis);
			dMax = dAxis->GetMax();
			dMin = dAxis->GetMin();
			if (this->refDbl >= dMin && this->refDbl <= dMax)
			{
				yPos = (Single)(y + height - xLeng - (this->refDbl - dMin) / (dMax - dMin) * xChartLeng);
				img->DrawLine((Double)(x + y1Leng), (Double)yPos, (Double)(x + width - y2Leng), (Double)yPos, refLinePen);

				sptr = Text::StrDoubleFmt(sbuff, this->refDbl, UnsafeArray<const Char>::ConvertFrom(this->dblFormat->v));
				if (this->yUnit.SetTo(s))
					sptr = s->ConcatTo(sptr);
				if (this->refType == RefType::LeftAlign)
				{
					rcSize = img->GetTextSize(fnt, CSTRP(sbuff, sptr));
					img->DrawString(Math::Coord2DDbl(x + y1Leng, yPos - rcSize.y), CSTRP(sbuff, sptr), fnt, fontBrush);
				}
				else if (this->refType == RefType::RightAlign)
				{
					rcSize = img->GetTextSize(fnt, CSTRP(sbuff, sptr));
					img->DrawString(Math::Coord2DDbl(x + width - y2Leng - rcSize.x, yPos - rcSize.y), CSTRP(sbuff, sptr), fnt, fontBrush);
				}
			}
		}

		if (!this->refTime.IsZero())
		{
			if (y1Axis->GetType() == DataType::Time)
			{
				NN<TimeAxis> tAxis = NN<TimeAxis>::ConvertFrom(y1Axis);
				tMax = tAxis->GetMax();
				tMin = tAxis->GetMin();
				if (this->refTime >= tMin && this->refTime <= tMax)
				{
					yPos = (Single)(y + height - xLeng - this->refTime.DiffSecDbl(tMin) / tMax.DiffSecDbl(tMin) * xChartLeng);
					img->DrawLine((Double)(x + y1Leng), (Double)yPos, (Double)(x + width - y2Leng), (Double)yPos, refLinePen);
				}
			}
			else if (this->y2Axis.SetTo(y2Axis) && y2Axis->GetType() == DataType::Time)
			{
				NN<TimeAxis> tAxis = NN<TimeAxis>::ConvertFrom(y2Axis);
				tMax = tAxis->GetMax();
				tMin = tAxis->GetMin();
				if (this->refTime >= tMin && this->refTime <= tMax)
				{
					yPos = (Single)(y + height - xLeng - this->refTime.DiffSecDbl(tMin) / tMax.DiffSecDbl(tMin) * xChartLeng);
					img->DrawLine((Double)(x + y1Leng), (Double)yPos, (Double)(x + width - y2Leng), (Double)yPos, refLinePen);
				}
			}
		}
	}

	img->DelFont(fnt);
	img->DelBrush(bgBrush);
	img->DelPen(boundPen);
	img->DelBrush(fontBrush);
	img->DelPen(gridPen);
	img->DelPen(refLinePen);
}

UOSInt Data::ChartPlotter::GetLegendCount() const
{
	return this->charts.GetCount();
}

UnsafeArrayOpt<UTF8Char> Data::ChartPlotter::GetLegend(UnsafeArray<UTF8Char> sbuff, OutParam<UInt32> color, UOSInt index) const
{
	NN<ChartParam> cdata;
	if (!this->charts.GetItem(index).SetTo(cdata))
		return 0;
	color.Set(cdata->lineColor);
	return Text::StrConcatC(sbuff, cdata->name->v, cdata->name->leng);
}

Bool Data::ChartPlotter::SavePng(NN<Media::DrawEngine> deng, Math::Size2D<UOSInt> size, Text::CStringNN fileName)
{
	NN<Media::DrawImage> img;
	if (!deng->CreateImage32(size, Media::AlphaType::AT_IGNORE_ALPHA).SetTo(img))
	{
		return false;
	}
	IO::FileStream fs(fileName, IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
	if (fs.IsError())
	{
		deng->DeleteImage(img);
		return false;
	}
	this->Plot(img, 0, 0, UOSInt2Double(size.GetWidth()), UOSInt2Double(size.GetHeight()));
	if (img->SavePng(fs) > 0)
	{
		deng->DeleteImage(img);
		return true;
	}
	deng->DeleteImage(img);
	return false;
}

UOSInt Data::ChartPlotter::CalScaleMarkDbl(NN<Data::ArrayListDbl> locations, NN<Data::ArrayListStringNN> labels, Double min, Double max, Double leng, Double minLeng, UnsafeArray<const Char> dblFormat, Double minDblVal, Optional<Text::String> unit)
{
	UOSInt retCnt = 2;
	UTF8Char sbuff[128];
	UnsafeArray<UTF8Char> sptr;
	Double scale;
	Double lScale;
	Double dScale;
	Double pos;
	NN<Text::String> s;

	sptr = Text::StrDoubleFmt(sbuff, min, dblFormat);
	locations->Add(0);
	if (unit.SetTo(s))
		sptr = s->ConcatTo(sptr);
	labels->Add(Text::String::New(sbuff, (UOSInt)(sptr - sbuff)));

	scale = minLeng * (max - min) / leng;
	lScale = (Int32)(Math_Log10(scale));
	dScale = Math_Pow(10, lScale);
	if (scale / dScale <= 2)
		dScale = Math_Pow(10, lScale) * 2;
	else if (scale / dScale <= 5)
		dScale = Math_Pow(10, lScale) * 5;
	else
		dScale = Math_Pow(10, lScale) * 10;

	if (dScale <= 0 || lScale <= -10)
	{
	}
	else
	{
		scale = (((Int32)(min / dScale)) + 1) * dScale;
		while (scale < max)
		{
			pos = ((scale - min) * leng / (max - min));
			if ((pos > minLeng) && (pos < leng - minLeng))
			{
				sptr = Text::StrDoubleFmt(sbuff, scale, dblFormat);
				locations->Add(pos);
				if (unit.SetTo(s))
					sptr = s->ConcatTo(sptr);
				labels->Add(Text::String::New(sbuff, (UOSInt)(sptr - sbuff)));
				retCnt++;
			}
			scale += dScale;
		}
	}

	sptr = Text::StrDoubleFmt(sbuff, max, dblFormat);
	locations->Add(leng);
	if (unit.SetTo(s))
		sptr = s->ConcatTo(sptr);
	labels->Add(Text::String::New(sbuff, (UOSInt)(sptr - sbuff)));
	return retCnt;
}

UOSInt Data::ChartPlotter::CalScaleMarkInt(NN<Data::ArrayListDbl> locations, NN<Data::ArrayListStringNN> labels, Int32 min, Int32 max, Double leng, Double minLeng, Optional<Text::String> unit)
{
	UOSInt retCnt = 2;
	UTF8Char sbuff[64];
	UnsafeArray<UTF8Char> sptr;
	Double scale;
	Double lScale;
	Double dScale;
	Single pos;
	NN<Text::String> s;

	sptr = Text::StrInt32(sbuff, min);
	locations->Add(0);
	if (unit.SetTo(s))
		sptr = s->ConcatTo(sptr);
	labels->Add(Text::String::New(sbuff, (UOSInt)(sptr - sbuff)));

	scale = minLeng * (Double)(max - min) / leng;
	lScale = (Int32)(Math_Log10(scale));
	if (scale < 1)
		dScale = 1;
	else
	{
		dScale = Math_Pow(10, lScale);
		if (scale / dScale <= 2)
			dScale = Math_Pow(10, lScale) * 2;
		else if (scale / dScale <= 5)
			dScale = Math_Pow(10, lScale) * 5;
		else
			dScale = Math_Pow(10, lScale) * 10;
	}

	scale = (((Int32)(min / dScale)) + 1) * dScale;
	while (scale < max)
	{
		pos = (Single)((scale - min) * leng / (Single)(max - min));
		if ((pos > minLeng) && (pos < leng - minLeng))
		{
			sptr = Text::StrInt32(sbuff, Double2Int32(scale));
			locations->Add(pos);
			if (unit.SetTo(s))
				sptr = s->ConcatTo(sptr);
			labels->Add(Text::String::New(sbuff, (UOSInt)(sptr - sbuff)));
			retCnt++;
		}
		scale += dScale;
	}

	sptr = Text::StrInt32(sbuff, max);
	locations->Add(leng);
	if (unit.SetTo(s))
		sptr = s->ConcatTo(sptr);
	labels->Add(Text::String::New(sbuff, (UOSInt)(sptr - sbuff)));
	return retCnt;
}

UOSInt Data::ChartPlotter::CalScaleMarkUInt(NN<Data::ArrayListDbl> locations, NN<Data::ArrayListStringNN> labels, UInt32 min, UInt32 max, Double leng, Double minLeng, Optional<Text::String> unit)
{
	UOSInt retCnt = 2;
	UTF8Char sbuff[64];
	UnsafeArray<UTF8Char> sptr;
	Double scale;
	Double lScale;
	Double dScale;
	Single pos;
	NN<Text::String> s;

	sptr = Text::StrUInt32(sbuff, min);
	locations->Add(0);
	if (unit.SetTo(s))
		sptr = s->ConcatTo(sptr);
	labels->Add(Text::String::New(sbuff, (UOSInt)(sptr - sbuff)));

	scale = minLeng * (Double)(max - min) / leng;
	lScale = (Int32)(Math_Log10(scale));
	if (scale < 1)
		dScale = 1;
	else
	{
		dScale = Math_Pow(10, lScale);
		if (scale / dScale <= 2)
			dScale = Math_Pow(10, lScale) * 2;
		else if (scale / dScale <= 5)
			dScale = Math_Pow(10, lScale) * 5;
		else
			dScale = Math_Pow(10, lScale) * 10;
	}

	scale = (((Int32)(min / dScale)) + 1) * dScale;
	while (scale < max)
	{
		pos = (Single)((scale - min) * leng / (Single)(max - min));
		if ((pos > minLeng) && (pos < leng - minLeng))
		{
			sptr = Text::StrInt32(sbuff, Double2Int32(scale));
			locations->Add(pos);
			if (unit.SetTo(s))
				sptr = s->ConcatTo(sptr);
			labels->Add(Text::String::New(sbuff, (UOSInt)(sptr - sbuff)));
			retCnt++;
		}
		scale += dScale;
	}

	sptr = Text::StrUInt32(sbuff, max);
	locations->Add(leng);
	if (unit.SetTo(s))
		sptr = s->ConcatTo(sptr);
	labels->Add(Text::String::New(sbuff, (UOSInt)(sptr - sbuff)));
	return retCnt;
}

UOSInt Data::ChartPlotter::CalScaleMarkDate(NN<Data::ArrayListDbl> locations, NN<Data::ArrayListStringNN> labels, NN<Data::DateTime> min, NN<Data::DateTime> max, Double leng, Double minLeng, UnsafeArray<const Char> dateFormat, UnsafeArrayOpt<const Char> timeFormat)
{
	UOSInt retCnt = 2;
	UTF8Char sbuff[64];
	UnsafeArray<UTF8Char> sptr;
	Int64 timeDif;
	Double scale;
	Double lScale;
	Int32 iScale;
//	Double dScale;
	Data::DateTime currDate;
	Single pos;
	Bool hasSecond = true;
	UnsafeArray<const Char> nntimeFormat;
	if (timeFormat.SetTo(nntimeFormat))
	{
		if (Text::StrIndexOfCharCh(nntimeFormat, 's') == INVALID_INDEX)
		{
			hasSecond = false;
		}
	}
    
	timeDif = max->DiffMS(min);
	if (Data::DateTimeUtil::MS2Days(timeDif) * minLeng / leng >= 20)
	{
		sptr = min->ToString(sbuff, dateFormat);
		locations->Add(0);
		labels->Add(Text::String::New(sbuff, (UOSInt)(sptr - sbuff)));
		
		Double lastPos = 0;
		currDate = min;
		currDate.ClearTime();
		currDate.SetDay(1);
		while (currDate < max)
		{
			currDate.AddMonth(1);
			pos = (Single)(Data::DateTimeUtil::MS2Minutes(currDate.DiffMS(min)) * leng / Data::DateTimeUtil::MS2Minutes(max->DiffMS(min)));
			if ((pos >= lastPos + minLeng) && (pos < leng - minLeng))
			{
				sptr = currDate.ToString(sbuff, dateFormat);
				locations->Add(pos);
				labels->Add(Text::String::New(sbuff, (UOSInt)(sptr - sbuff)));
				retCnt++;
			}
		}

		sptr = max->ToString(sbuff, dateFormat);
		locations->Add(leng);
		labels->Add(Text::String::New(sbuff, (UOSInt)(sptr - sbuff)));
	}
	else if (!timeFormat.SetTo(nntimeFormat) || Data::DateTimeUtil::MS2Days(timeDif) * minLeng / leng >= 1)
	{
		sptr = min->ToString(sbuff, dateFormat);
		locations->Add(0);
		labels->Add(Text::String::New(sbuff, (UOSInt)(sptr - sbuff)));

		scale = Data::DateTimeUtil::MS2Days(timeDif) * minLeng / leng;
		lScale = (Int32)(Math_Log10(scale));
		iScale = Double2Int32(Math_Pow(10, lScale));
		if (scale / iScale <= 2)
			iScale = Double2Int32(Math_Pow(10, lScale) * 2);
		else if (scale / iScale <= 5)
			iScale = Double2Int32(Math_Pow(10, lScale) * 5);
		else
			iScale = Double2Int32(Math_Pow(10, lScale) * 10);

		currDate = min;
		currDate.ClearTime();
		currDate.AddDay(iScale - (currDate.GetDay() % (iScale)));
		if (((Double)currDate.DiffMS(min) / (Double)timeDif) < minLeng / leng)
		{
			currDate.AddDay(iScale);
		}
		while (currDate < max)
		{
			pos = (Single)(Data::DateTimeUtil::MS2Minutes(currDate.DiffMS(min)) * leng / Data::DateTimeUtil::MS2Minutes(max->DiffMS(min)));
			if ((pos > minLeng) && (pos < leng - minLeng))
			{
				sptr = currDate.ToString(sbuff, dateFormat);
				locations->Add(pos);
				labels->Add(Text::String::New(sbuff, (UOSInt)(sptr - sbuff)));
				retCnt++;
			}
			currDate.AddDay(iScale);
		}

		sptr = max->ToString(sbuff, dateFormat);
		locations->Add(leng);
		labels->Add(Text::String::New(sbuff, (UOSInt)(sptr - sbuff)));
	}
	else if (Data::DateTimeUtil::MS2Hours(timeDif) * minLeng / leng >= 1)
	{
		if (min->GetMSPassedDate() == 0)
		{
			sptr = min->ToString(sbuff, dateFormat);
			locations->Add(0);
			labels->Add(Text::String::New(sbuff, (UOSInt)(sptr - sbuff)));
		}
		else
		{
			sptr = min->ToString(sbuff, nntimeFormat);
			locations->Add(0);
			labels->Add(Text::String::New(sbuff, (UOSInt)(sptr - sbuff)));
		}
		
		scale = Data::DateTimeUtil::MS2Hours(timeDif) * minLeng / leng;
		if (scale <= 2)
			iScale = 2;
		else if (scale <= 3)
			iScale = 3;
		else if (scale <= 6)
			iScale = 6;
		else if (scale <= 12)
			iScale = 12;
		else
			iScale = 24;

		currDate = min;
		currDate.ClearTime();
		currDate.AddHour(iScale + (Int32)(Data::DateTimeUtil::MS2Hours(min->GetMSPassedDate()) / iScale) * iScale);
		while (currDate < max)
		{
			pos = (Single)(Data::DateTimeUtil::MS2Minutes(currDate.DiffMS(min)) * leng / Data::DateTimeUtil::MS2Minutes(max->DiffMS(min)));
			if ((pos > minLeng) && (pos < leng - minLeng))
			{
				if (currDate.GetMSPassedDate() == 0)
				{
					sptr = currDate.ToString(sbuff, dateFormat);
					locations->Add(pos);
					labels->Add(Text::String::New(sbuff, (UOSInt)(sptr - sbuff)));
				}
				else
				{
					sptr = currDate.ToString(sbuff, nntimeFormat);
					locations->Add(pos);
					labels->Add(Text::String::New(sbuff, (UOSInt)(sptr - sbuff)));
				}
				retCnt++;
			}
			currDate.AddHour(iScale);
		}

		if (max->GetMSPassedDate() == 0)
		{
			sptr = max->ToString(sbuff, dateFormat);
			locations->Add(leng);
			labels->Add(Text::String::New(sbuff, (UOSInt)(sptr - sbuff)));
		}
		else
		{
			sptr = max->ToString(sbuff, nntimeFormat);
			locations->Add(leng);
			labels->Add(Text::String::New(sbuff, (UOSInt)(sptr - sbuff)));
		}
	}
	else if (!hasSecond || Data::DateTimeUtil::MS2Minutes(timeDif) * minLeng / leng >= 1)
	{
		if (min->GetMSPassedDate() == 0)
		{
			sptr = min->ToString(sbuff, dateFormat);
			locations->Add(0);
			labels->Add(Text::String::New(sbuff, (UOSInt)(sptr - sbuff)));
		}
		else
		{
			sptr = min->ToString(sbuff, nntimeFormat);
			locations->Add(0);
			labels->Add(Text::String::New(sbuff, (UOSInt)(sptr - sbuff)));
		}

		scale = Data::DateTimeUtil::MS2Minutes(timeDif) * minLeng / leng;
		if (scale <= 1)
			iScale = 1;
		else if (scale <= 2)
			iScale = 2;
		else if (scale <= 5)
			iScale = 5;
		else if (scale <= 10)
			iScale = 10;
		else if (scale <= 20)
			iScale = 20;
		else if (scale <= 30)
			iScale = 30;
		else
			iScale = 60;

		currDate = min;
		currDate.ClearTime();
		currDate.AddMinute(iScale + (Int32)(Data::DateTimeUtil::MS2Minutes(min->GetMSPassedDate()) / iScale) * iScale);
		while (currDate < max)
		{
			pos = (Single)(Data::DateTimeUtil::MS2Minutes(currDate.DiffMS(min)) * leng / Data::DateTimeUtil::MS2Minutes(max->DiffMS(min)));
			if ((pos > minLeng) && (pos < leng - minLeng))
			{
				if (currDate.GetMSPassedDate() == 0)
				{
					sptr = currDate.ToString(sbuff, dateFormat);
					locations->Add(pos);
					labels->Add(Text::String::New(sbuff, (UOSInt)(sptr - sbuff)));
				}
				else
				{
					sptr = currDate.ToString(sbuff, nntimeFormat);
					locations->Add(pos);
					labels->Add(Text::String::New(sbuff, (UOSInt)(sptr - sbuff)));
				}
				retCnt++;
			}
			currDate.AddMinute(iScale);
		}

		if (max->GetMSPassedDate() == 0)
		{
			sptr = max->ToString(sbuff, dateFormat);
			locations->Add(leng);
			labels->Add(Text::String::New(sbuff, (UOSInt)(sptr - sbuff)));
		}
		else
		{
			sptr = max->ToString(sbuff, nntimeFormat);
			locations->Add(leng);
			labels->Add(Text::String::New(sbuff, (UOSInt)(sptr - sbuff)));
		}
	}
	else if (Data::DateTimeUtil::MS2Seconds(timeDif) >= 1)
	{
		if (min->GetMSPassedDate() == 0)
		{
			sptr = min->ToString(sbuff, dateFormat);
			locations->Add(0);
			labels->Add(Text::String::New(sbuff, (UOSInt)(sptr - sbuff)));
		}
		else
		{
			sptr = min->ToString(sbuff, nntimeFormat);
			locations->Add(0);
			labels->Add(Text::String::New(sbuff, (UOSInt)(sptr - sbuff)));
		}

		scale = Data::DateTimeUtil::MS2Seconds(timeDif) * minLeng / leng;
		if (scale <= 1)
			iScale = 1;
		else if (scale <= 2)
			iScale = 2;
		else if (scale <= 5)
			iScale = 5;
		else if (scale <= 10)
			iScale = 10;
		else if (scale <= 20)
			iScale = 20;
		else if (scale <= 30)
			iScale = 30;
		else
			iScale = 60;

		currDate = min;
		currDate.ClearTime();
		currDate.AddSecond(iScale + (Int32)(Data::DateTimeUtil::MS2Seconds(min->GetMSPassedDate()) / iScale) * iScale);
		while (currDate < max)
		{
			pos = (Single)(Data::DateTimeUtil::MS2Minutes(currDate.DiffMS(min)) * leng / Data::DateTimeUtil::MS2Minutes(max->DiffMS(min)));
			if ((pos > minLeng) && (pos < leng - minLeng))
			{
				if (currDate.GetMSPassedDate() == 0)
				{
					sptr = currDate.ToString(sbuff, dateFormat);
					locations->Add(pos);
					labels->Add(Text::String::New(sbuff, (UOSInt)(sptr - sbuff)));
				}
				else
				{
					sptr = currDate.ToString(sbuff, nntimeFormat);
					locations->Add(pos);
					labels->Add(Text::String::New(sbuff, (UOSInt)(sptr - sbuff)));
				}
				retCnt++;
			}
			currDate.AddSecond(iScale);
		}

		if (max->GetMSPassedDate() == 0)
		{
			sptr = max->ToString(sbuff, dateFormat);
			locations->Add(leng);
			labels->Add(Text::String::New(sbuff, (UOSInt)(sptr - sbuff)));
		}
		else
		{
			sptr = max->ToString(sbuff, nntimeFormat);
			locations->Add(leng);
			labels->Add(Text::String::New(sbuff, (UOSInt)(sptr - sbuff)));
		}
	}
	else
	{
		sptr = min->ToString(sbuff, nntimeFormat);
		locations->Add(0);
		labels->Add(Text::String::New(sbuff, (UOSInt)(sptr - sbuff)));

		sptr = max->ToString(sbuff, nntimeFormat);
		locations->Add(leng);
		labels->Add(Text::String::New(sbuff, (UOSInt)(sptr - sbuff)));
	}
	return retCnt;
}

NN<Data::ChartPlotter::TimeData> Data::ChartPlotter::NewData(UnsafeArray<Data::Timestamp> data, UOSInt dataCnt)
{
	NN<TimeData> d;
	NEW_CLASSNN(d, TimeData(data, dataCnt));
	return d;
}

NN<Data::ChartPlotter::Int32Data> Data::ChartPlotter::NewData(UnsafeArray<Int32> data, UOSInt dataCnt)
{
	NN<Int32Data> d;
	NEW_CLASSNN(d, Int32Data(data, dataCnt));
	return d;
}

NN<Data::ChartPlotter::UInt32Data> Data::ChartPlotter::NewData(UnsafeArray<UInt32> data, UOSInt dataCnt)
{
	NN<UInt32Data> d;
	NEW_CLASSNN(d, UInt32Data(data, dataCnt));
	return d;
}

NN<Data::ChartPlotter::DoubleData> Data::ChartPlotter::NewData(UnsafeArray<Double> data, UOSInt dataCnt)
{
	NN<DoubleData> d;
	NEW_CLASSNN(d, DoubleData(data, dataCnt));
	return d;
}

NN<Data::ChartPlotter::TimeData> Data::ChartPlotter::NewDataDate(UnsafeArray<Int64> ticksData, UOSInt dataCnt)
{
	NN<TimeData> d;
	NEW_CLASSNN(d, TimeData(ticksData, dataCnt));
	return d;
}

NN<Data::ChartPlotter::TimeData> Data::ChartPlotter::NewData(NN<Data::ReadingList<Data::Timestamp>> data)
{
	NN<TimeData> d;
	NEW_CLASSNN(d, TimeData(data));
	return d;
}

NN<Data::ChartPlotter::Int32Data> Data::ChartPlotter::NewData(NN<Data::ReadingList<Int32>> data)
{
	NN<Int32Data> d;
	NEW_CLASSNN(d, Int32Data(data));
	return d;
}

NN<Data::ChartPlotter::UInt32Data> Data::ChartPlotter::NewData(NN<Data::ReadingList<UInt32>> data)
{
	NN<UInt32Data> d;
	NEW_CLASSNN(d, UInt32Data(data));
	return d;
}

NN<Data::ChartPlotter::DoubleData> Data::ChartPlotter::NewData(NN<Data::ReadingList<Double>> data)
{
	NN<DoubleData> d;
	NEW_CLASSNN(d, DoubleData(data));
	return d;
}

NN<Data::ChartPlotter::Int32Data> Data::ChartPlotter::NewDataSeq(Int32 startSeq, UOSInt count)
{
	UnsafeArray<Int32> iArr = MemAllocArr(Int32, count);
	UOSInt i = 0;
	while (i < count)
	{
		iArr[i] = startSeq;
		i++;
		startSeq++;
	}
	NN<Int32Data> d;
	NEW_CLASSNN(d, Int32Data(iArr, count));
	MemFreeArr(iArr);
	return d;
}

Optional<Data::ChartPlotter::Axis> Data::ChartPlotter::NewAxis(NN<ChartData> data)
{
	if (data->GetType() == DataType::Integer)
	{
		NN<Int32Axis> iAxis;
		NEW_CLASSNN(iAxis, Int32Axis(NN<Int32Data>::ConvertFrom(data)));
		return iAxis;
	}
	else if (data->GetType() == DataType::UInteger)
	{
		NN<UInt32Axis> iAxis;
		NEW_CLASSNN(iAxis, UInt32Axis(NN<UInt32Data>::ConvertFrom(data)));
		return iAxis;
	}
	else if (data->GetType() == DataType::DOUBLE)
	{
		NN<DoubleAxis> dAxis;
		NEW_CLASSNN(dAxis, DoubleAxis(NN<DoubleData>::ConvertFrom(data)));
		return dAxis;
	}
	else if (data->GetType() == DataType::Time)
	{
		NN<TimeAxis> tAxis;
		NEW_CLASSNN(tAxis, TimeAxis(NN<TimeData>::ConvertFrom(data)));
		return tAxis;
	}
	return 0;
}
