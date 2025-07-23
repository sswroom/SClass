#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ChartPlotter.h"
#include "Math/Math.h"
#include "Text/MyString.h"
#include "Text/MyStringFloat.h"

Data::ChartPlotter::ChartData::ChartData(NN<Text::String> name, void *data, UOSInt dataCnt, DataType dataType, UInt32 lineColor, LineStyle lineStyle, ChartType chartType)
{
	this->name = name->Clone();
	this->data = data;
	this->dataCnt = dataCnt;
	this->dataType = dataType;
	this->lineColor = lineColor;
	this->lineStyle = lineStyle;
	this->chartType = chartType;
}

Data::ChartPlotter::ChartData::ChartData(Text::CStringNN name, void *data, UOSInt dataCnt, DataType dataType, UInt32 lineColor, LineStyle lineStyle, ChartType chartType)
{
	this->name = Text::String::New(name);
	this->data = data;
	this->dataCnt = dataCnt;
	this->dataType = dataType;
	this->lineColor = lineColor;
	this->lineStyle = lineStyle;
	this->chartType = chartType;
}

Data::ChartPlotter::ChartData::~ChartData()
{
	this->name->Release();
	MemFree(this->data);
}

Data::ChartPlotter::ChartPlotter(Text::CString title)
{
	this->title = 0;
	this->xAxisName = 0;
	this->yAxisName = 0;

	this->timeFormat = Text::String::New(UTF8STRC("HH:mm"));
	this->dateFormat = Text::String::New(UTF8STRC("yyyy/MM/dd"));
	this->dblFormat = Text::String::New(UTF8STRC("0.00"));
	this->minDblVal = 0.01;

	this->titleBuff = 0;
	this->SetTitle(title);
	this->xType = DataType::None;
	this->refTime = 0;
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
	this->refTime = 0;
	this->refExist = false;

	this->xRangeDateMin = 0;
	this->xRangeDateMax = 0;

	this->hasXRangeDate = false;
	this->hasYRangeDbl = false;
	this->hasYRangeInt = false;
}

Data::ChartPlotter::~ChartPlotter()
{
	OPTSTR_DEL(this->title);
	OPTSTR_DEL(this->xAxisName);
	OPTSTR_DEL(this->yAxisName);

	this->dateFormat->Release();
	this->timeFormat->Release();
	this->dblFormat->Release();

	UOSInt i;

	i = this->xDatas.GetCount();
	while (i-- > 0)
	{
		MemFree(this->xDatas.GetItem(i));
	}

	i = this->yCharts.GetCount();
	while (i-- > 0)
	{
		this->yCharts.RemoveAt(i).Delete();
	}
	
	this->refTime = 0;
	this->xRangeDateMax = 0;
	this->xRangeDateMin = 0;

	OPTSTR_DEL(this->yUnit);
	OPTSTR_DEL(this->titleBuff);
	this->fntName->Release();
}


Bool Data::ChartPlotter::AddXData(UnsafeArray<Data::DateTime*> data, UOSInt dataCnt)
{
	if (xType == DataType::None)
	{
		Int64 *dateData = MemAlloc(Int64, dataCnt);
		UOSInt i = dataCnt;
		while (i-- > 0)
		{
			dateData[i] = data[i]->ToTicks();
		}
		
		xType = DataType::DateTicks;
		this->xDatas.Add(dateData);
		this->xDataCnt.Add(dataCnt);
		return true;
	}
	else if (xType == DataType::DateTicks)
	{
		Int64 *dateData = MemAlloc(Int64, dataCnt);
		UOSInt i = dataCnt;
		while (i-- > 0)
		{
			dateData[i] = data[i]->ToTicks();
		}

		this->xDatas.Add(dateData);
		this->xDataCnt.Add(dataCnt);
		return true;
	}
	else
	{
		return false;
	}
}

Bool Data::ChartPlotter::AddXData(UnsafeArray<Data::Timestamp> data, UOSInt dataCnt)
{
	if (xType == DataType::None)
	{
		Int64 *dateData = MemAlloc(Int64, dataCnt);
		UOSInt i = dataCnt;
		while (i-- > 0)
		{
			dateData[i] = data[i].ToTicks();
		}
		
		xType = DataType::DateTicks;
		this->xDatas.Add(dateData);
		this->xDataCnt.Add(dataCnt);
		return true;
	}
	else if (xType == DataType::DateTicks)
	{
		Int64 *dateData = MemAlloc(Int64, dataCnt);
		UOSInt i = dataCnt;
		while (i-- > 0)
		{
			dateData[i] = data[i].ToTicks();
		}

		this->xDatas.Add(dateData);
		this->xDataCnt.Add(dataCnt);
		return true;
	}
	else
	{
		return false;
	}
}

Bool Data::ChartPlotter::AddXData(UnsafeArray<Double> data, UOSInt dataCnt)
{
	if (xType == DataType::None)
	{
		Double *dblData = MemAlloc(Double, dataCnt);
		MemCopyNO(dblData, data.Ptr(), sizeof(Double) * dataCnt);
		xType = DataType::DOUBLE;
		this->xDatas.Add(dblData);
		this->xDataCnt.Add(dataCnt);
		return true;
	}
	else if (xType == DataType::DOUBLE)
	{
		Double *dblData = MemAlloc(Double, dataCnt);
		MemCopyNO(dblData, data.Ptr(), sizeof(Double) * dataCnt);
		this->xDatas.Add(dblData);
		this->xDataCnt.Add(dataCnt);
		return true;
	}
	else
	{
		return false;
	}
}

Bool Data::ChartPlotter::AddXData(UnsafeArray<Int32> data, UOSInt dataCnt)
{
	if (xType == DataType::None)
	{
		Int32 *iData = MemAlloc(Int32, dataCnt);
		MemCopyNO(iData, data.Ptr(), sizeof(Int32) * dataCnt);
		xType = DataType::Integer;
		this->xDatas.Add(iData);
		this->xDataCnt.Add(dataCnt);
		return true;
	}
	else if (xType == DataType::Integer)
	{
		Int32 *iData = MemAlloc(Int32, dataCnt);
		MemCopyNO(iData, data.Ptr(), sizeof(Int32) * dataCnt);
		this->xDatas.Add(iData);
		this->xDataCnt.Add(dataCnt);
		return true;
	}
	else
	{
		return false;
	}
}

Bool Data::ChartPlotter::AddXDataDate(UnsafeArray<Int64> data, UOSInt dataCnt)
{
	if (xType == DataType::None)
	{
		Int64 *newData = MemAlloc(Int64, dataCnt);
		MemCopyNO(newData, data.Ptr(), sizeof(Int64) * dataCnt);
		xType = DataType::DateTicks;
		this->xDatas.Add(newData);
		this->xDataCnt.Add(dataCnt);
		return true;
	}
	else if (xType == DataType::DateTicks)
	{
		Int64 *newData = MemAlloc(Int64, dataCnt);
		MemCopyNO(newData, data.Ptr(), sizeof(Int64) * dataCnt);
		this->xDatas.Add(newData);
		this->xDataCnt.Add(dataCnt);
		return true;
	}
	else
	{
		return false;
	}
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
	this->refTime = refVal->ToTicks();
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

void Data::ChartPlotter::AddYDataDate(NN<Text::String> name, UnsafeArray<Int64> value, UOSInt valCnt, UInt32 lineColor, LineStyle lineStyle)
{
	Int64 *newVals;
	newVals = MemAlloc(Int64, valCnt);
	MemCopyNO(newVals, value.Ptr(), sizeof(Int64) * valCnt);
	NN<ChartData> data;
	NEW_CLASSNN(data, ChartData(name, newVals, valCnt, DataType::DateTicks, lineColor, lineStyle, ChartType::Line));
	this->yCharts.Add(data);
}

void Data::ChartPlotter::AddYDataDate(Text::CStringNN name, UnsafeArray<Int64> value, UOSInt valCnt, UInt32 lineColor, LineStyle lineStyle)
{
	Int64 *newVals;
	newVals = MemAlloc(Int64, valCnt);
	MemCopyNO(newVals, value.Ptr(), sizeof(Int64) * valCnt);
	NN<ChartData> data;
	NEW_CLASSNN(data, ChartData(name, newVals, valCnt, DataType::DateTicks, lineColor, lineStyle, ChartType::Line));
	this->yCharts.Add(data);
}

void Data::ChartPlotter::AddYData(NN<Text::String> name, UnsafeArray<Int32> value, UOSInt valCnt, UInt32 lineColor, LineStyle lineStyle)
{
	Int32 *newVals;
	newVals = MemAlloc(Int32, valCnt);
	MemCopyNO(newVals, value.Ptr(), sizeof(Int32) * valCnt);
	NN<ChartData> data;
	NEW_CLASSNN(data, ChartData(name, newVals, valCnt, DataType::Integer, lineColor, lineStyle, ChartType::Line));
	this->yCharts.Add(data);
}

void Data::ChartPlotter::AddYData(Text::CStringNN name, UnsafeArray<Int32> value, UOSInt valCnt, UInt32 lineColor, LineStyle lineStyle)
{
	Int32 *newVals;
	newVals = MemAlloc(Int32, valCnt);
	MemCopyNO(newVals, value.Ptr(), sizeof(Int32) * valCnt);
	NN<ChartData> data;
	NEW_CLASSNN(data, ChartData(name, newVals, valCnt, DataType::Integer, lineColor, lineStyle, ChartType::Line));
	this->yCharts.Add(data);
}

void Data::ChartPlotter::AddYData(NN<Text::String> name, UnsafeArray<Double> value, UOSInt valCnt, UInt32 lineColor, LineStyle lineStyle)
{
	Double *newVals;
	newVals = MemAlloc(Double, valCnt);
	MemCopyNO(newVals, value.Ptr(), sizeof(Double) * valCnt);
	NN<ChartData> data;
	NEW_CLASSNN(data, ChartData(name, newVals, valCnt, DataType::DOUBLE, lineColor, lineStyle, ChartType::Line));
	this->yCharts.Add(data);
}

void Data::ChartPlotter::AddYData(Text::CStringNN name, UnsafeArray<Double> value, UOSInt valCnt, UInt32 lineColor, LineStyle lineStyle)
{
	Double *newVals;
	newVals = MemAlloc(Double, valCnt);
	MemCopyNO(newVals, value.Ptr(), sizeof(Double) * valCnt);
	NN<ChartData> data;
	NEW_CLASSNN(data, ChartData(name, newVals, valCnt, DataType::DOUBLE, lineColor, lineStyle, ChartType::Line));
	this->yCharts.Add(data);
}

void Data::ChartPlotter::SetXRangeDate(NN<Data::DateTime> xVal)
{
	if (hasXRangeDate)
	{
		if (this->xRangeDateMin > xVal->ToTicks())
		{
			this->xRangeDateMin = xVal->ToTicks();
		}
		if (this->xRangeDateMax < xVal->ToTicks())
		{
			this->xRangeDateMax = xVal->ToTicks();
		}
	}
	else
	{
		this->xRangeDateMin = xVal->ToTicks();
		this->xRangeDateMax = xVal->ToTicks();
		this->hasXRangeDate = true;
	}
}

void Data::ChartPlotter::SetYRangeInt(Int32 yVal)
{
	if (hasYRangeInt)
	{
		if (yRangeIntMin > yVal)
		{
			yRangeIntMin = yVal;
		}
		if (yRangeIntMax < yVal)
		{
			yRangeIntMax = yVal;
		}
	}
	else
	{
		yRangeIntMin = yVal;
		yRangeIntMax = yVal;
		hasYRangeInt = true;
	}
}

void Data::ChartPlotter::SetYRangeDbl(Double yVal)
{
	if (hasYRangeDbl)
	{
		if (yRangeDblMin > yVal)
		{
			yRangeDblMin = yVal;
		}
		if (yRangeDblMax < yVal)
		{
			yRangeDblMax = yVal;
		}
	}
	else
	{
		yRangeDblMin = yVal;
		yRangeDblMax = yVal;
		hasYRangeDbl = true;
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
	OPTSTR_DEL(this->xAxisName);
	this->xAxisName = Text::String::NewOrNull(xAxisName);
}

Optional<Text::String> Data::ChartPlotter::GetXAxisName() const
{
	return this->xAxisName;
}

void Data::ChartPlotter::SetYAxisName(Text::CString yAxisName)
{
	OPTSTR_DEL(this->yAxisName);
	this->yAxisName = Text::String::NewOrNull(yAxisName);
}

Optional<Text::String> Data::ChartPlotter::GetYAxisName() const
{
	return this->yAxisName;
}

Data::ChartPlotter::DataType Data::ChartPlotter::GetXAxisType() const
{
	return this->xType;
}

UOSInt Data::ChartPlotter::GetXDataCount() const
{
	return this->xDatas.GetCount();
}

UnsafeArrayOpt<Int64> Data::ChartPlotter::GetXDateTicks(UOSInt index, OutParam<UOSInt> cnt) const
{
	if (this->xType != DataType::DateTicks)
		return 0;
	cnt.Set(this->xDataCnt.GetItem(index));
	return (Int64*)this->xDatas.GetItem(index);
}

UnsafeArrayOpt<Double> Data::ChartPlotter::GetXDouble(UOSInt index, OutParam<UOSInt> cnt) const
{
	if (this->xType != DataType::DOUBLE)
		return 0;
	cnt.Set(this->xDataCnt.GetItem(index));
	return (Double*)this->xDatas.GetItem(index);
}

UnsafeArrayOpt<Int32> Data::ChartPlotter::GetXInt32(UOSInt index, OutParam<UOSInt> cnt) const
{
	if (this->xType != DataType::Integer)
		return 0;
	cnt.Set(this->xDataCnt.GetItem(index));
	return (Int32*)this->xDatas.GetItem(index);
}

UOSInt Data::ChartPlotter::GetYDataCount() const
{
	return this->yCharts.GetCount();
}

UnsafeArrayOpt<Int64> Data::ChartPlotter::GetYDateTicks(UOSInt index, OutParam<UOSInt> cnt) const
{
	NN<ChartData> data;
	if (!this->yCharts.GetItem(index).SetTo(data) || data->dataType != DataType::DateTicks)
		return 0;
	cnt.Set(data->dataCnt);
	return (Int64*)data->data;
}

UnsafeArrayOpt<Double> Data::ChartPlotter::GetYDouble(UOSInt index, OutParam<UOSInt> cnt) const
{
	NN<ChartData> data;
	if (!this->yCharts.GetItem(index).SetTo(data) || data->dataType != DataType::DOUBLE)
		return 0;
	cnt.Set(data->dataCnt);
	return (Double*)data->data;
}

UnsafeArrayOpt<Int32> Data::ChartPlotter::GetYInt32(UOSInt index, OutParam<UOSInt> cnt) const
{
	NN<ChartData> data;
	if (!this->yCharts.GetItem(index).SetTo(data) || data->dataType != DataType::Integer)
		return 0;
	cnt.Set(data->dataCnt);
	return (Int32*)data->data;
}

Optional<Text::String> Data::ChartPlotter::GetYName(UOSInt index) const
{
	NN<ChartData> data;
	if (!this->yCharts.GetItem(index).SetTo(data))
		return 0;
	return data->name;
}

Data::ChartPlotter::DataType Data::ChartPlotter::GetYType(UOSInt index) const
{
	NN<ChartData> data;
	if (!this->yCharts.GetItem(index).SetTo(data))
		return DataType::None;
	return data->dataType;
}

void Data::ChartPlotter::Plot(NN<Media::DrawImage> img, Double x, Double y, Double width, Double height)
{
	if (height == 0 || width == 0)
		return;
	if (this->yCharts.GetCount() == 0)
	{
		if (!this->hasXRangeDate || this->xRangeDateMax == this->xRangeDateMin)
			return;

		Bool found = false;
		if (this->hasYRangeDbl && this->yRangeDblMax != this->yRangeDblMin)
			found = true;
		if (this->hasYRangeInt && this->yRangeIntMax != this->yRangeIntMin)
			found = true;
		if (!found)
			return;
	}

	NN<Media::DrawFont> fnt;
	Double fntH;
	Bool customX = false;

//	Int32 xAxisPos;
	Int32 xMaxInt = 0;
	Int32 xMinInt = 0;
	Double xMaxDbl = 0;
	Double xMinDbl = 0;
	Int64 xMaxDate = 0;
	Int64 xMinDate = 0;
	DataType xType = this->xType;
//	Int32 yAxis1Pos;
	DataType yAxis1Type;
//	Int32 yAxis2Pos;
	DataType yAxis2Type;
	Int32 y1MaxInt = 0;
	Int32 y1MinInt = 0;
	Int32 y2MaxInt = 0;
	Int32 y2MinInt = 0;
	Double y1MaxDbl = 0;
	Double y1MinDbl = 0;
	Double y2MaxDbl = 0;
	Double y2MinDbl = 0;
	Int64 y1MaxDate = 0;
	Int64 y1MinDate = 0;
	Int64 y2MaxDate = 0;
	Int64 y2MinDate = 0;

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


	if (xType == DataType::None || this->xDatas.GetCount() == 0)
	{
		if (this->yCharts.GetCount() == 0)
		{
			if (this->hasXRangeDate)
			{
				xType = DataType::DateTicks;
				xMaxDate = this->xRangeDateMax;
				xMinDate = this->xRangeDateMin;
			}
		}
		else
		{
			xType = DataType::Integer;
			UOSInt dataLeng;
			NN<ChartData> data = this->yCharts.GetItemNoCheck(0);
			Int32 *tmpData = MemAlloc(Int32, dataLeng = (i = data->dataCnt));
			while (i-- > 0)
				tmpData[i] = (Int32)i;
			this->xDatas.Add(tmpData);
			this->xDataCnt.Add(dataLeng);
			xMaxInt = (Int32)dataLeng - 1;
			xMinInt = 0;
			if (data->dataCnt == 2)
				xMaxInt = 1;
			customX = true;
		}
	}
	else if (xType == DataType::DateTicks)
	{
		Int64 *tmpdata;
		UOSInt tmpdataCnt;
		xMaxDate = ((Int64*)this->xDatas.GetItem(0))[0];
		xMinDate = ((Int64*)this->xDatas.GetItem(0))[0];
		i = 0;
		while (i < this->xDatas.GetCount())
		{
			tmpdata = (Int64*)this->xDatas.GetItem(i);
			tmpdataCnt = this->xDataCnt.GetItem(i);
			j = 0;
			while (j < tmpdataCnt)
			{
				if (tmpdata[j] > xMaxDate)
					xMaxDate = tmpdata[j];

				if (tmpdata[j] < xMinDate)
					xMinDate = tmpdata[j];
				j++;
			}
			i++;
		}
		if (xMaxDate == xMinDate)
		{
			xMaxDate += 3600000;
		}
	}
	else if (xType == DataType::DOUBLE)
	{
		Double *tmpdata;
		UOSInt tmpdataCnt;
		xMinDbl = xMaxDbl = ((Double*)this->xDatas.GetItem(0))[0];
		i = 0;
		while (i < this->xDatas.GetCount())
		{
			tmpdata = (Double*)this->xDatas.GetItem(i);
			tmpdataCnt = this->xDataCnt.GetItem(i);
			j = 0;
			while (j < tmpdataCnt)
			{
				if (tmpdata[j] > xMaxDbl)
					xMaxDbl = tmpdata[j];
				if (tmpdata[j] < xMinDbl)
					xMinDbl = tmpdata[j];
				j++;
			}
			i++;
		}
		if (xMaxDbl == xMinDbl)
		{
			xMaxDbl = xMaxDbl + 1;
		}
	}
	else if (xType == DataType::Integer)
	{
		Int32 *tmpdata;
		UOSInt tmpdataCnt;
		xMinInt = xMaxInt = ((Int32*)this->xDatas.GetItem(0))[0];
		i = 0;
		while (i < this->xDatas.GetCount())
		{
			tmpdata = (Int32*)this->xDatas.GetItem(i);
			tmpdataCnt = this->xDataCnt.GetItem(i);
			j = 0;
			while (j < tmpdataCnt)
			{
				if (tmpdata[j] > xMaxInt)
					xMaxInt = tmpdata[j];
				if (tmpdata[j] < xMinInt)
					xMinInt = tmpdata[j];
				j++;
			}
			i++;
		}
		if (xMaxInt == xMinInt)
			xMaxInt = xMaxInt + 1;
	}



	yAxis1Type = DataType::None;
	yAxis2Type = DataType::None;
	if (this->yCharts.GetCount() > 0)
	{
		i = 0;
		while (i < this->yCharts.GetCount())
		{
			NN<ChartData> data = this->yCharts.GetItemNoCheck(i);
			if (data->dataType == DataType::Integer)
			{
				Int32 *datas = (Int32*)data->data;
				if (yAxis1Type == DataType::None)
				{
					if (this->hasYRangeInt)
					{
						y1MinInt = this->yRangeIntMin;
						y1MaxInt = this->yRangeIntMax;
					}
					else if (refExist)
					{
						y1MinInt = y1MaxInt = refInt;
					}
					else
					{
						y1MaxInt = datas[0];
						y1MinInt = datas[0];
					}
					yAxis1Type = DataType::Integer;
					j = 0;
					while (j < data->dataCnt)
					{
						if (datas[j] > y1MaxInt)
							y1MaxInt = datas[j];
						if (datas[j] < y1MinInt)
							y1MinInt = datas[j];
						j++;
					}
					if (y1MaxInt == y1MinInt)
						y1MaxInt = y1MinInt + 1;
				}
				else if (yAxis1Type == DataType::Integer)
				{
					j = 0;
					while (j < data->dataCnt)
					{
						if (datas[j]> y1MaxInt)
							y1MaxInt = datas[j];
						if (datas[j] < y1MinInt)
							y1MinInt = datas[j];
						j++;
					}
					if (y1MaxInt == y1MinInt)
						y1MaxInt = y1MinInt + 1;
				}
				else if (yAxis2Type == DataType::None)
				{
					yAxis2Type = DataType::Integer;
					if (this->hasYRangeInt)
					{
						y2MinInt = this->yRangeIntMin;
						y2MaxInt = this->yRangeIntMax;
					}
					else if (refExist)
					{
						y2MinInt = y2MaxInt = refInt;
					}
					else
					{
						y2MaxInt = datas[0];
						y2MinInt = datas[0];
					}
					j = 0;
					while (j < data->dataCnt)
					{
						if (datas[j] > y2MaxInt)
							y2MaxInt = datas[j];
						if (datas[j] < y2MinInt)
							y2MinInt = datas[j];
						j++;
					}
					if (y2MaxInt == y2MinInt)
						y2MaxInt = y2MinInt + 1;
				}
				else if (yAxis2Type == DataType::Integer)
				{
					j = 0;
					while (j < data->dataCnt)
					{
						if (datas[j] > y2MaxInt)
							y2MaxInt = datas[j];
						if (datas[j] < y2MinInt)
							y2MinInt = datas[j];
						j++;
					}
					if (y2MaxInt == y2MinInt)
						y2MaxInt = y2MinInt + 1;
				}
				else
				{
					//throw new System::Exception("Unsupported chart");
					return;
				}
			}
			else if (data->dataType == DataType::DOUBLE)
			{
				Double *datas = (Double*)data->data;
				if (yAxis1Type == DataType::None)
				{
					if (this->hasYRangeDbl)
					{
						y1MinDbl = this->yRangeDblMin;
						y1MaxDbl = this->yRangeDblMax;
					}
					else if (refExist)
					{
						y1MinDbl = y1MaxDbl = refDbl;
					}
					else
					{
						y1MaxDbl = datas[0];
						y1MinDbl = datas[0];
					}
					yAxis1Type = DataType::DOUBLE;
					j = 0;
					while (j < data->dataCnt)
					{
						if (datas[j] > y1MaxDbl)
							y1MaxDbl = datas[j];
						if (datas[j] < y1MinDbl)
							y1MinDbl = datas[j];
						j++;
					}
					if (y1MaxDbl == y1MinDbl)
						y1MaxDbl = y1MinDbl + minDblVal;
				}
				else if (yAxis1Type == DataType::DOUBLE)
				{
					j = 0;
					while (j < data->dataCnt)
					{
						if (datas[j] > y1MaxDbl)
							y1MaxDbl = datas[j];
						if (datas[j] < y1MinDbl)
							y1MinDbl = datas[j];
						j++;
					}
					if (y1MaxDbl == y1MinDbl)
						y1MaxDbl = y1MinDbl + minDblVal;
				}
				else if (yAxis2Type == DataType::None)
				{
					yAxis2Type = DataType::DOUBLE;
					if (this->hasYRangeDbl)
					{
						y2MinDbl = this->yRangeDblMin;
						y2MaxDbl = this->yRangeDblMax;
					}
					else if (refExist)
					{
						y2MinDbl = y2MaxDbl = refDbl;
					}
					else
					{
						y2MaxDbl = datas[0];
						y2MinDbl = datas[0];
					}
					j = 0;
					while (j < data->dataCnt)
					{
						if (datas[j] > y2MaxDbl)
							y2MaxDbl = datas[j];
						if (datas[j] < y2MinDbl)
							y2MinDbl = datas[j];
						j++;
					}
					if (y2MaxDbl == y2MinDbl)
						y2MaxDbl = y2MinDbl + minDblVal;
				}
				else if (yAxis2Type == DataType::DOUBLE)
				{
					j = 0;
					while (j < data->dataCnt)
					{
						if (datas[j] > y2MaxDbl)
							y2MaxDbl = datas[j];
						if (datas[j] < y2MinDbl)
							y2MinDbl = datas[j];
						j++;
					}
					if (y2MaxDbl == y2MinDbl)
						y2MaxDbl = y2MinDbl + minDblVal;

				}
				else
				{
				//	throw new System::Exception("Unsupported chart");
					return;
				}
			}
			else if (data->dataType == DataType::DateTicks)
			{
				Int64 *datas = (Int64*)data->data;
				if (yAxis1Type == DataType::None)
				{
					if (refExist)
					{
						y1MinDate = y1MaxDate = this->refTime;
					}
					else
					{
						y1MaxDate = datas[0];
						y1MinDate = datas[0];
					}
					yAxis1Type = DataType::DateTicks;
					j = 0;
					while (j < data->dataCnt)
					{
						if (datas[j] > y1MaxDate)
							y1MaxDate = datas[j];
						if (datas[j] < y1MinDate)
							y1MinDate = datas[j];
						j++;
					}
					if (y1MaxDate == y1MinDate)
					{
						y1MaxDate = y1MinDate;
						y1MaxDate += 3600000;
					}
				}
				else if (yAxis1Type == DataType::DateTicks)
				{
					j = 0;
					while (j < data->dataCnt)
					{
						if (datas[j] > y1MaxDate)
							y1MaxDate = datas[j];
						if (datas[j] < y1MinDate)
							y1MinDate = datas[j];
						j++;
					}
					if (y1MaxDate == y1MinDate)
					{
						y1MaxDate = y1MinDate;
						y1MaxDate += 3600000;
					}
				}
				else if (yAxis2Type == DataType::None)
				{
					yAxis2Type = DataType::DateTicks;
					if (refExist)
					{
						y2MinDate = y2MaxDate = refTime;
					}
					else
					{
						y2MaxDate = datas[0];
						y2MinDate = datas[0];
					}
					j = 0;
					while (j < data->dataCnt)
					{
						if (datas[j] > y2MaxDate)
							y2MaxDate = datas[j];
						if (datas[j] < y2MinDate)
							y2MinDate = datas[j];
						j++;
					}
					if (y2MaxDate == y2MinDate)
					{
						y2MaxDate = y2MinDate;
						y2MaxDate += 3600000;
					}
				}
				else if (yAxis2Type == DataType::DateTicks)
				{
					j = 0;
					while (j < data->dataCnt)
					{
						if (datas[j] > y2MaxDate)
							y2MaxDate = datas[j];
						if (datas[j] < y2MinDate)
							y2MinDate = datas[j];
						j++;
					}
					if (y2MaxDate == y2MinDate)
					{
						y2MaxDate = y2MinDate;
						y2MaxDate += 3600000;
					}
				}
				else
				{
				//	throw new System::Exception("Unsupported chart");
				}
			}
			i++;
		}
	}
	else
	{
		if (this->hasYRangeDbl && this->yRangeDblMax != this->yRangeDblMin)
		{
			yAxis1Type = DataType::DOUBLE;
			y1MaxDbl = this->yRangeDblMax;
			y1MinDbl = this->yRangeDblMin;
		}
		else if (this->hasYRangeInt && this->yRangeIntMax != this->yRangeIntMin)
		{
			yAxis1Type = DataType::Integer;
			y1MaxInt = this->yRangeIntMax;
			y1MinInt = this->yRangeIntMin;
		}
	}

	xLeng = 0;
	y1Leng = 0;
	y2Leng = 0;
	if (xType == DataType::Integer)
	{
		sptr = Text::StrInt32(sbuff, xMaxInt);
		rcSize = img->GetTextSize(fnt, CSTRP(sbuff, sptr));
		xLeng = (Single)rcSize.x;

		sptr = Text::StrInt32(sbuff, xMinInt);
		rcSize = img->GetTextSize(fnt, CSTRP(sbuff, sptr));
		if (rcSize.x > xLeng)
			xLeng = (Single)rcSize.x;
	}
	else if (xType == DataType::DOUBLE)
	{
		sptr = Text::StrDoubleFmt(sbuff, xMaxDbl, UnsafeArray<const Char>::ConvertFrom(this->dblFormat->v));
		rcSize = img->GetTextSize(fnt, CSTRP(sbuff, sptr));
		xLeng = (Single)rcSize.x;
		
		sptr = Text::StrDoubleFmt(sbuff, xMinDbl, UnsafeArray<const Char>::ConvertFrom(this->dblFormat->v));
		rcSize = img->GetTextSize(fnt, CSTRP(sbuff, sptr));
		if (rcSize.x > xLeng)
			xLeng = (Single)rcSize.x;
	}
	else if (xType == DataType::DateTicks)
	{
		dt1.SetTicks(xMaxDate);
		dt1.ConvertTimeZoneQHR(this->timeZoneQHR);
		dt2.SetTicks(xMinDate);
		dt2.ConvertTimeZoneQHR(this->timeZoneQHR);
		if (dt1.IsSameDay(dt2))
		{
			sptr = dt1.ToString(sbuff, UnsafeArray<const Char>::ConvertFrom(this->timeFormat->v));
			rcSize = img->GetTextSize(fnt, CSTRP(sbuff, sptr));
			xLeng = (Single)rcSize.x;
			if (dt2.GetMSPassedDate() == 0)
			{
				sptr = dt2.ToString(sbuff, UnsafeArray<const Char>::ConvertFrom(this->dateFormat->v));
			}
			else
			{
				sptr = dt2.ToString(sbuff, UnsafeArray<const Char>::ConvertFrom(this->timeFormat->v));
			}
			rcSize = img->GetTextSize(fnt, CSTRP(sbuff, sptr));
			if (rcSize.x > xLeng)
				xLeng = (Single)rcSize.x;
		}
		else
		{
			sptr = dt1.ToString(sbuff, UnsafeArray<const Char>::ConvertFrom(this->dateFormat->v));
			rcSize = img->GetTextSize(fnt, CSTRP(sbuff, sptr));
			xLeng = (Single)rcSize.x;
			sptr = dt1.ToString(sbuff, UnsafeArray<const Char>::ConvertFrom(this->timeFormat->v));
			rcSize = img->GetTextSize(fnt, CSTRP(sbuff, sptr));
			if (rcSize.x > xLeng)
				xLeng = (Single)rcSize.x;
		}
	}
	if (this->xAxisName.SetTo(s))
	{
		rcSize = img->GetTextSize(fnt, s->ToCString());
		xLeng += rcSize.y;
	}
	xLeng += barLeng;


	if (yAxis1Type == DataType::Integer)
	{
		sptr = Text::StrInt32(sbuff, y1MaxInt);
		if (this->yUnit.SetTo(s))
			sptr = s->ConcatTo(sptr);
		rcSize = img->GetTextSize(fnt, CSTRP(sbuff, sptr));
		y1Leng = rcSize.x;

		sptr = Text::StrInt32(sbuff, y1MinInt);
		if (this->yUnit.SetTo(s))
			sptr = s->ConcatTo(sptr);
		rcSize = img->GetTextSize(fnt, CSTRP(sbuff, sptr));
		if (rcSize.x > y1Leng)
			y1Leng = rcSize.x;
	}
	else if (yAxis1Type == DataType::DOUBLE)
	{
		sptr = Text::StrDoubleFmt(sbuff, y1MaxDbl, UnsafeArray<const Char>::ConvertFrom(this->dblFormat->v));
		if (this->yUnit.SetTo(s))
			sptr = s->ConcatTo(sptr);
		rcSize = img->GetTextSize(fnt, CSTRP(sbuff, sptr));
		y1Leng = rcSize.x;

		sptr = Text::StrDoubleFmt(sbuff, y1MinDbl, UnsafeArray<const Char>::ConvertFrom(this->dblFormat->v));
		if (this->yUnit.SetTo(s))
			sptr = s->ConcatTo(sptr);
		rcSize = img->GetTextSize(fnt, CSTRP(sbuff, sptr));
		if (rcSize.x > y1Leng)
			y1Leng = rcSize.x;
	}
	else if (yAxis1Type == DataType::DateTicks)
	{
		dt1.SetTicks(y1MaxDate);
		dt1.ConvertTimeZoneQHR(this->timeZoneQHR);
		sptr = dt1.ToString(sbuff, UnsafeArray<const Char>::ConvertFrom(this->dateFormat->v));
		rcSize = img->GetTextSize(fnt, CSTRP(sbuff, sptr));
		y1Leng = rcSize.x;

		dt1.SetTicks(y1MinDate);
		dt1.ConvertTimeZoneQHR(this->timeZoneQHR);
		sptr = dt1.ToString(sbuff, UnsafeArray<const Char>::ConvertFrom(this->dateFormat->v));
		rcSize = img->GetTextSize(fnt, CSTRP(sbuff, sptr));;
		if (rcSize.x > y1Leng)
			y1Leng = rcSize.x;
	}
	if (this->yAxisName.SetTo(s))
	{
		rcSize = img->GetTextSize(fnt, s->ToCString());
		y1Leng += rcSize.y;
	}
	y1Leng += barLeng;



	if (yAxis2Type == DataType::Integer)
	{
		sptr = Text::StrInt32(sbuff, y2MaxInt);
		if (this->yUnit.SetTo(s))
			sptr = s->ConcatTo(sptr);
		rcSize = img->GetTextSize(fnt, CSTRP(sbuff, sptr));
		y2Leng = rcSize.x;

		sptr = Text::StrInt32(sbuff, y2MinInt);
		if (this->yUnit.SetTo(s))
			sptr = s->ConcatTo(sptr);
		rcSize = img->GetTextSize(fnt, CSTRP(sbuff, sptr));
		if (rcSize.x > y2Leng)
			y2Leng = rcSize.x;

		y2Leng += barLeng;
		y2show = true;
	}
	else if (yAxis2Type == DataType::DOUBLE)
	{
		sptr = Text::StrDoubleFmt(sbuff, y2MaxDbl, UnsafeArray<const Char>::ConvertFrom(this->dblFormat->v));
		if (this->yUnit.SetTo(s))
			sptr = s->ConcatTo(sptr);
		rcSize = img->GetTextSize(fnt, CSTRP(sbuff, sptr));
		y2Leng = rcSize.x;

		sptr = Text::StrDoubleFmt(sbuff, y2MinDbl, UnsafeArray<const Char>::ConvertFrom(this->dblFormat->v));
		if (this->yUnit.SetTo(s))
			sptr = s->ConcatTo(sptr);
		rcSize = img->GetTextSize(fnt, CSTRP(sbuff, sptr));
		if (rcSize.x > y2Leng)
			y2Leng = rcSize.x;

		y2Leng += barLeng;
		y2show = true;
	}
	else if (yAxis2Type == DataType::DateTicks)
	{
		dt1.SetTicks(y2MaxDate);
		dt1.ConvertTimeZoneQHR(this->timeZoneQHR);
		sptr = dt1.ToString(sbuff, UnsafeArray<const Char>::ConvertFrom(this->dateFormat->v));
		rcSize = img->GetTextSize(fnt, CSTRP(sbuff, sptr));
		y2Leng = rcSize.x;

		dt1.SetTicks(y2MinDate);
		dt1.ConvertTimeZoneQHR(this->timeZoneQHR);
		sptr = dt1.ToString(sbuff, UnsafeArray<const Char>::ConvertFrom(this->dateFormat->v));
		rcSize = img->GetTextSize(fnt, CSTRP(sbuff, sptr));
		if (rcSize.x > y2Leng)
			y2Leng = rcSize.x;

		y2Leng += barLeng;
		y2show = true;
	}
	else if (yAxis2Type == DataType::None)
	{
		y2Leng = (rcSize.y / 2.0);
		y2show = false;
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
	if (xType == DataType::Integer)
	{
		CalScaleMarkInt(locations, labels, xMinInt, xMaxInt, width - y1Leng - y2Leng - this->pointSize * 2, fntH, 0);
	}
	else if (xType == DataType::DOUBLE)
	{
		CalScaleMarkDbl(locations, labels, xMinDbl, xMaxDbl, width - y1Leng - y2Leng - this->pointSize * 2, fntH, UnsafeArray<const Char>::ConvertFrom(this->dblFormat->v), minDblVal, 0);
	}
	else if (xType == DataType::DateTicks)
	{
		dt1.SetTicks(xMinDate);
		dt1.ConvertTimeZoneQHR(this->timeZoneQHR);
		dt2.SetTicks(xMaxDate);
		dt2.ConvertTimeZoneQHR(this->timeZoneQHR);
		CalScaleMarkDate(locations, labels, dt1, dt2, width - y1Leng - y2Leng - this->pointSize * 2, fntH, UnsafeArray<const Char>::ConvertFrom(this->dateFormat->v), UnsafeArray<const Char>::ConvertFrom(this->timeFormat->v));
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


	i = 0;
	while (i < locations.GetCount())
	{
		s = Text::String::OrEmpty(labels.GetItem(i));
		Math::Size2DDbl strSize = img->GetTextSize(fnt, s->ToCString());
		img->DrawStringRot(Math::Coord2DDbl((x + y1Leng + this->pointSize + locations.GetItem(i)) - strSize.y * 0.5, (y + height - xLeng + barLeng) + strSize.x), Text::String::OrEmpty(labels.GetItem(i)), fnt, fontBrush, 90);
		i += 1;
	}

	locations.Clear();
	i = labels.GetCount();
	while (i-- > 0)
	{
		OPTSTR_DEL(labels.GetItem(i));
	}
	labels.Clear();

	if (yAxis1Type == DataType::Integer)
	{
		CalScaleMarkInt(locations, labels, y1MinInt, y1MaxInt, height - xLeng - fntH / 2 - this->pointSize * 2, fntH, this->yUnit);
	}
	else if (yAxis1Type == DataType::DOUBLE)
	{
		CalScaleMarkDbl(locations, labels, y1MinDbl, y1MaxDbl, height - xLeng - fntH / 2 - this->pointSize * 2, fntH, UnsafeArray<const Char>::ConvertFrom(this->dblFormat->v), minDblVal, this->yUnit);
	}
	else if (yAxis1Type == DataType::DateTicks)
	{
		dt1.SetTicks(y1MinDate);
		dt1.ConvertTimeZoneQHR(this->timeZoneQHR);
		dt2.SetTicks(y2MaxDate);
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

	if (this->yAxisName.SetTo(s))
	{
		Math::Size2DDbl sz = img->GetTextSize(fnt, s->ToCString());
		img->DrawStringRot(Math::Coord2DDbl((x + fntH / 2) - sz.y * 0.5, (y + (height - xLeng) / 2) - sz.x * 0.5), s->ToCString(), fnt, fontBrush, 90);
	}

	if (this->xAxisName.SetTo(s))
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
	while (i < this->yCharts.GetCount())
	{
		void *xData;
		UOSInt xDataCnt;
		NN<ChartData> chart = this->yCharts.GetItemNoCheck(i);

		if (this->xDatas.GetCount() > i)
		{
			xData = this->xDatas.GetItem(i);
			xDataCnt = this->xDataCnt.GetItem(i);
		}
		else
		{
			xData = this->xDatas.GetItem(0);
			xDataCnt = this->xDataCnt.GetItem(0);
		}

		if (chart->lineStyle == LineStyle::Fill)
		{
			currPosLen = xDataCnt + 2;
			currPos = MemAllocA(Math::Coord2DDbl, currPosLen);
		}
		else
		{
			currPosLen = xDataCnt;
			currPos = MemAllocA(Math::Coord2DDbl, currPosLen);
		}

		Double xChartLeng = width - y1Leng - y2Leng - this->pointSize * 2.0;
		if (xType == DataType::DateTicks)
		{
			Int64 *data = (Int64*)xData;
			j = 0;
			while (j < xDataCnt)
			{
				currPos[j].x = (Double)(x + y1Leng + this->pointSize + Data::DateTimeUtil::MS2Minutes(data[j] - xMinDate) / Data::DateTimeUtil::MS2Minutes(xMaxDate - xMinDate) * xChartLeng);
				j++;
			}
		}
		else if (xType == DataType::DOUBLE)
		{
			Double *data = (Double*)xData;
			j = 0;
			while (j < xDataCnt)
			{
				currPos[j].x = (Double)(x + y1Leng + this->pointSize + (data[j] - xMinDbl) / (xMaxDbl - xMinDbl) * xChartLeng);
				j++;
			}
		}
		else if (xType == DataType::Integer)
		{
			Int32 *data = (Int32*)xData;

			j = 0;
			while (j < xDataCnt)
			{
				currPos[j].x = (Double)(x + y1Leng + this->pointSize + (Double)(data[j] - xMinInt) / (Single)(xMaxInt - xMinInt) * xChartLeng);
				j++;
			}
		}

		xChartLeng = height - xLeng - fntH / 2 - this->pointSize * 2;
		if (chart->dataType == DataType::Integer)
		{
			Int32 *data = (Int32*)chart->data;
			Int32 iMax = 0;
			Int32 iMin = 0;
			if (chart->dataType == yAxis1Type)
			{
				iMax = y1MaxInt;
				iMin = y1MinInt;
			}
			else if  (chart->dataType == yAxis2Type)
			{
				iMax = y2MaxInt;
				iMin = y2MinInt;
			}

			j = 0;
			while (j < chart->dataCnt)
			{
				currPos[j].y = (Double)(y + height - this->pointSize - xLeng - (Double)(data[j] - iMin) / (Single)(iMax - iMin) * xChartLeng);
				j++;
			}
		}
		else if (chart->dataType == DataType::DOUBLE)
		{
			Double *data = (Double*)chart->data;
			Double dMax = 0;
			Double dMin = 0;
			if (chart->dataType == yAxis1Type)
			{
				dMax = y1MaxDbl;
				dMin = y1MinDbl;
			}
			else if  (chart->dataType == yAxis2Type)
			{
				dMax = y2MaxDbl;
				dMin = y2MinDbl;
			}


			j = 0;
			while (j < chart->dataCnt)
			{
				currPos[j].y = (Double)(y + height - this->pointSize - xLeng - (data[j] - dMin) / (dMax - dMin) * xChartLeng);
				j++;
			}
		}
		else if (chart->dataType == DataType::DateTicks)
		{
			Int64 *data = (Int64 *)chart->data;
			Int64 dMax = 0;
			Int64 dMin = 0;
			if (chart->dataType == yAxis1Type)
			{
				dMax = y1MaxDate;
				dMin = y1MinDate;
			}
			else if  (chart->dataType == yAxis2Type)
			{
				dMax = y2MaxDate;
				dMin = y2MinDate;
			}

			j = 0;
			while (j < chart->dataCnt)
			{
				currPos[j].y = (Double)(y + height - this->pointSize - xLeng - Data::DateTimeUtil::MS2Minutes(data[j] - dMin) / Data::DateTimeUtil::MS2Minutes(dMax - dMin) * xChartLeng);
				j++;
			}
		}

		if (chart->lineStyle == LineStyle::Fill)
		{
			if (currPosLen >= 4)
			{
				j = currPosLen;
				currPos[j - 2].x = currPos[j - 3].x;
				currPos[j - 2].y = (Double)(y + height - xLeng);
				currPos[j - 1].x = currPos[0].x;
				currPos[j - 1].y = (Double)(y + height - xLeng);
				NN<Media::DrawPen> p = img->NewPenARGB(chart->lineColor, 1, 0, 0);
				NN<Media::DrawBrush> b = img->NewBrushARGB(chart->lineColor);
				img->DrawPolygon(currPos, currPosLen, p, b);
				img->DelBrush(b);
				img->DelPen(p);
			}
		}
		else
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
		Int64 tMax;
		Int64 tMin;

		if (yAxis1Type == DataType::Integer)
		{
			iMax = y1MaxInt;
			iMin = y1MinInt;
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
		else if (yAxis2Type == DataType::Integer)
		{
			iMax = y2MaxInt;
			iMin = y2MinInt;
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

		if (DataType::DOUBLE == yAxis1Type)
		{
			dMax = y1MaxDbl;
			dMin = y1MinDbl;
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
		else if (DataType::DOUBLE == yAxis2Type)
		{
			dMax = y2MaxDbl;
			dMin = y2MinDbl;
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

		if (this->refTime != 0)
		{
			if (DataType::DateTicks == yAxis1Type)
			{
				tMax = y1MaxDate;
				tMin = y1MinDate;
				if (this->refTime >= tMin && this->refTime <= tMax)
				{
					yPos = (Single)(y + height - xLeng - Data::DateTimeUtil::MS2Minutes(this->refTime - tMin) / Data::DateTimeUtil::MS2Minutes(tMax - tMin) * xChartLeng);
					img->DrawLine((Double)(x + y1Leng), (Double)yPos, (Double)(x + width - y2Leng), (Double)yPos, refLinePen);
				}
			}
			else if (DataType::DateTicks == yAxis2Type)
			{
				tMax = y2MaxDate;
				tMin = y2MinDate;
				if (this->refTime >= tMin && this->refTime <= tMax)
				{
					yPos = (Single)(y + height - xLeng - Data::DateTimeUtil::MS2Minutes(this->refTime - tMin) / Data::DateTimeUtil::MS2Minutes(tMax - tMin) * xChartLeng);
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

	if (customX)
	{
		i = this->xDatas.GetCount();
		while (i-- > 0)
		{
			MemFree(this->xDatas.GetItem(i));
		}
		this->xDatas.Clear();
		this->xDataCnt.Clear();
	}
}

UOSInt Data::ChartPlotter::GetLegendCount() const
{
	return this->yCharts.GetCount();
}

UnsafeArrayOpt<UTF8Char> Data::ChartPlotter::GetLegend(UnsafeArray<UTF8Char> sbuff, OutParam<UInt32> color, UOSInt index) const
{
	NN<ChartData> cdata;
	if (!this->yCharts.GetItem(index).SetTo(cdata))
		return 0;
	color.Set(cdata->lineColor);
	return Text::StrConcatC(sbuff, cdata->name->v, cdata->name->leng);
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
	if (!timeFormat.SetTo(nntimeFormat) || Data::DateTimeUtil::MS2Days(timeDif) * minLeng / leng >= 1)
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
