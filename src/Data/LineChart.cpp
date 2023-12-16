#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/Chart.h"
#include "Data/LineChart.h"
#include "Text/MyString.h"
#include "Text/MyStringFloat.h"

Data::LineChart::LineChart(Text::CString title)
{
	this->titleBuff = 0;
	this->SetTitle(title);
	this->xType = Data::Chart::DataType::None;
	this->refTime = 0;
	this->timeZoneQHR = 0;
	this->barLength = 3.0;
	this->pointType = PT_NULL;
	this->pointSize = 0;
	this->yUnit = 0;
	Data::DateTime dt;
	dt.ToLocalTime();
	this->timeZoneQHR = dt.GetTimeZoneQHR();
	NEW_CLASS(this->rnd, Data::RandomOS());
	NEW_CLASS(xDatas, Data::ArrayList<void*>());
	NEW_CLASS(xDataCnt, Data::ArrayList<UOSInt>());
	NEW_CLASS(yCharts, Data::ArrayList<ChartData*>());

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

Data::LineChart::~LineChart()
{
	UOSInt i;

	i = this->xDatas->GetCount();
	while (i-- > 0)
	{
		MemFree(this->xDatas->GetItem(i));
	}
	DEL_CLASS(this->xDatas);
	this->xDatas = 0;
	DEL_CLASS(this->xDataCnt);
	this->xDataCnt = 0;

	i = this->yCharts->GetCount();
	while (i-- > 0)
	{
		Data::LineChart::ChartData *data = (Data::LineChart::ChartData*)this->yCharts->RemoveAt(i);
		DEL_CLASS(data);
	}
	DEL_CLASS(this->yCharts);
	this->yCharts = 0;

	DEL_CLASS(this->rnd);
	
	this->refTime = 0;
	this->xRangeDateMax = 0;
	this->xRangeDateMin = 0;

	SDEL_STRING(this->yUnit);
	SDEL_STRING(this->titleBuff);
	this->fntName->Release();
}

Bool Data::LineChart::AddXData(Data::DateTime **data, UOSInt dataCnt)
{
	if (xType == Data::Chart::DataType::None)
	{
		Int64 *dateData = MemAlloc(Int64, dataCnt);
		UOSInt i = dataCnt;
		while (i-- > 0)
		{
			dateData[i] = data[i]->ToTicks();
		}
		
		xType = Data::Chart::DataType::DateTicks;
		xDatas->Add(dateData);
		xDataCnt->Add(dataCnt);
		return true;
	}
	else if (xType == Data::Chart::DataType::DateTicks)
	{
		Int64 *dateData = MemAlloc(Int64, dataCnt);
		UOSInt i = dataCnt;
		while (i-- > 0)
		{
			dateData[i] = data[i]->ToTicks();
		}

		xDatas->Add(dateData);
		xDataCnt->Add(dataCnt);
		return true;
	}
	else
	{
		return false;
	}
}

Bool Data::LineChart::AddXData(Data::Timestamp *data, UOSInt dataCnt)
{
	if (xType == Data::Chart::DataType::None)
	{
		Int64 *dateData = MemAlloc(Int64, dataCnt);
		UOSInt i = dataCnt;
		while (i-- > 0)
		{
			dateData[i] = data[i].ToTicks();
		}
		
		xType = Data::Chart::DataType::DateTicks;
		xDatas->Add(dateData);
		xDataCnt->Add(dataCnt);
		return true;
	}
	else if (xType == Data::Chart::DataType::DateTicks)
	{
		Int64 *dateData = MemAlloc(Int64, dataCnt);
		UOSInt i = dataCnt;
		while (i-- > 0)
		{
			dateData[i] = data[i].ToTicks();
		}

		xDatas->Add(dateData);
		xDataCnt->Add(dataCnt);
		return true;
	}
	else
	{
		return false;
	}
}

Bool Data::LineChart::AddXData(Double *data, UOSInt dataCnt)
{
	if (xType == Data::Chart::DataType::None)
	{
		Double *dblData = MemAlloc(Double, dataCnt);
		MemCopyNO(dblData, data, sizeof(Double) * dataCnt);
		xType = Data::Chart::DataType::DOUBLE;
		xDatas->Add(dblData);
		xDataCnt->Add(dataCnt);
		return true;
	}
	else if (xType == Data::Chart::DataType::DOUBLE)
	{
		Double *dblData = MemAlloc(Double, dataCnt);
		MemCopyNO(dblData, data, sizeof(Double) * dataCnt);
		xDatas->Add(dblData);
		xDataCnt->Add(dataCnt);
		return true;
	}
	else
	{
		return false;
	}
}

Bool Data::LineChart::AddXData(Int32 *data, UOSInt dataCnt)
{
	if (xType == Data::Chart::DataType::None)
	{
		Int32 *iData = MemAlloc(Int32, dataCnt);
		MemCopyNO(iData, data, sizeof(Int32) * dataCnt);
		xType = Data::Chart::DataType::Integer;
		xDatas->Add(iData);
		xDataCnt->Add(dataCnt);
		return true;
	}
	else if (xType == Data::Chart::DataType::Integer)
	{
		Int32 *iData = MemAlloc(Int32, dataCnt);
		MemCopyNO(iData, data, sizeof(Int32) * dataCnt);
		xDatas->Add(iData);
		xDataCnt->Add(dataCnt);
		return true;
	}
	else
	{
		return false;
	}
}

Bool Data::LineChart::AddXDataDate(Int64 *data, UOSInt dataCnt)
{
	if (xType == Data::Chart::DataType::None)
	{
		Int64 *newData = MemAlloc(Int64, dataCnt);
		MemCopyNO(newData, data, sizeof(Int64) * dataCnt);
		xType = Data::Chart::DataType::DateTicks;
		xDatas->Add(newData);
		xDataCnt->Add(dataCnt);
		return true;
	}
	else if (xType == Data::Chart::DataType::DateTicks)
	{
		Int64 *newData = MemAlloc(Int64, dataCnt);
		MemCopyNO(newData, data, sizeof(Int64) * dataCnt);
		xDatas->Add(newData);
		xDataCnt->Add(dataCnt);
		return true;
	}
	else
	{
		return false;
	}
}

void Data::LineChart::SetFontHeightPt(Double ptSize)
{
	if (ptSize > 0)
		fntSizePt = ptSize;
}

void Data::LineChart::SetFontName(Text::CString name)
{
	this->fntName->Release();
	this->fntName = Text::String::New(name);
}

void Data::LineChart::SetYRefVal(Int32 refVal, UInt32 col)
{
	this->refInt = refVal;
	this->refLineColor = col;
	this->refExist = true;
}

void Data::LineChart::SetYRefVal(Double refVal, UInt32 col)
{
	this->refDbl = refVal;
	this->refLineColor = col;
	this->refExist = true;
}

void Data::LineChart::SetYRefVal(Data::DateTime *refVal, UInt32 col)
{
	this->refTime = refVal->ToTicks();
	this->refLineColor = col;
	this->refExist = true;
}

void Data::LineChart::SetYRefType(Data::LineChart::RefType refType)
{
	this->refType = refType;
}

void Data::LineChart::SetYUnit(Text::CString yUnit)
{
	SDEL_STRING(this->yUnit);
	this->yUnit = Text::String::NewOrNull(yUnit);
}

void Data::LineChart::SetLineThick(Double lineThick)
{
	this->lineThick = lineThick;
}

void Data::LineChart::SetTimeZoneQHR(Int8 timeZoneQHR)
{
	this->timeZoneQHR = timeZoneQHR;
}

void Data::LineChart::SetBarLength(Double barLength)
{
	this->barLength = barLength;
}

void Data::LineChart::SetPointType(PointType pointType, Double pointSize)
{
	this->pointType = pointType;
	this->pointSize = pointSize;
}

UInt32 Data::LineChart::GetRndColor()
{
	UInt32 r;
	UInt32 g;
	UInt32 b;
	
	r = (UInt32)(64 + (this->rnd->NextInt15() % 192));
	g = (UInt32)(64 + (this->rnd->NextInt15() % 192));
	b = 512 - r - g;
	if (b < 0)
		b = 0;
	else if (b > 255)
		b = 255;
	return 0xff000000 | (r << 16) | (g << 8) | b;
}

void Data::LineChart::AddYDataDate(Text::String *name, Int64 *value, UOSInt valCnt, UInt32 lineColor, Data::LineChart::LineStyle lineStyle)
{
	Int64 *newVals;
	newVals = MemAlloc(Int64, valCnt);
	MemCopyNO(newVals, value, sizeof(Int64) * valCnt);
	yCharts->Add(new Data::LineChart::ChartData(name, newVals, valCnt, Data::Chart::DataType::DateTicks, lineColor, lineStyle));
}

void Data::LineChart::AddYDataDate(Text::CString name, Int64 *value, UOSInt valCnt, UInt32 lineColor, Data::LineChart::LineStyle lineStyle)
{
	Int64 *newVals;
	newVals = MemAlloc(Int64, valCnt);
	MemCopyNO(newVals, value, sizeof(Int64) * valCnt);
	yCharts->Add(new Data::LineChart::ChartData(name, newVals, valCnt, Data::Chart::DataType::DateTicks, lineColor, lineStyle));
}

void Data::LineChart::AddYData(Text::String *name, Int32 *value, UOSInt valCnt, UInt32 lineColor, Data::LineChart::LineStyle lineStyle)
{
	Int32 *newVals;
	newVals = MemAlloc(Int32, valCnt);
	MemCopyNO(newVals, value, sizeof(Int32) * valCnt);
	yCharts->Add(new Data::LineChart::ChartData(name, newVals, valCnt, Data::Chart::DataType::Integer, lineColor, lineStyle));
}

void Data::LineChart::AddYData(Text::CString name, Int32 *value, UOSInt valCnt, UInt32 lineColor, Data::LineChart::LineStyle lineStyle)
{
	Int32 *newVals;
	newVals = MemAlloc(Int32, valCnt);
	MemCopyNO(newVals, value, sizeof(Int32) * valCnt);
	yCharts->Add(new Data::LineChart::ChartData(name, newVals, valCnt, Data::Chart::DataType::Integer, lineColor, lineStyle));
}

void Data::LineChart::AddYData(Text::String *name, Double *value, UOSInt valCnt, UInt32 lineColor, Data::LineChart::LineStyle lineStyle)
{
	Double *newVals;
	newVals = MemAlloc(Double, valCnt);
	MemCopyNO(newVals, value, sizeof(Double) * valCnt);
	yCharts->Add(new Data::LineChart::ChartData(name, newVals, valCnt, Data::Chart::DataType::DOUBLE, lineColor, lineStyle));
}

void Data::LineChart::AddYData(Text::CString name, Double *value, UOSInt valCnt, UInt32 lineColor, Data::LineChart::LineStyle lineStyle)
{
	Double *newVals;
	newVals = MemAlloc(Double, valCnt);
	MemCopyNO(newVals, value, sizeof(Double) * valCnt);
	yCharts->Add(new Data::LineChart::ChartData(name, newVals, valCnt, Data::Chart::DataType::DOUBLE, lineColor, lineStyle));
}

void Data::LineChart::SetXRangeDate(Data::DateTime *xVal)
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

void Data::LineChart::SetYRangeInt(Int32 yVal)
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

void Data::LineChart::SetYRangeDbl(Double yVal)
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

void Data::LineChart::SetTitle(Text::CString title)
{
	SDEL_STRING(this->title);
	this->title = Text::String::NewOrNull(title);

	SDEL_STRING(this->titleBuff);
	if (this->title == 0)
	{
		this->titleBuff = 0;
		this->titleLineCnt = 0;
	}
	else
	{
		this->titleBuff = Text::String::New(this->title->v, this->title->leng).Ptr();
		this->titleLineCnt = Text::StrSplitLineP(this->titleLine, 3, *this->titleBuff);
	}
}


Data::Chart::DataType Data::LineChart::GetXAxisType() const
{
	return this->xType;
}

UOSInt Data::LineChart::GetXDataCount() const
{
	return this->xDatas->GetCount();
}

Int64 *Data::LineChart::GetXDateTicks(UOSInt index, UOSInt *cnt) const
{
	if (this->xType != DataType::DateTicks)
		return 0;
	*cnt = this->xDataCnt->GetItem(index);
	return (Int64*)this->xDatas->GetItem(index);
}

Double *Data::LineChart::GetXDouble(UOSInt index, UOSInt *cnt) const
{
	if (this->xType != DataType::DOUBLE)
		return 0;
	*cnt = this->xDataCnt->GetItem(index);
	return (Double*)this->xDatas->GetItem(index);
}

Int32 *Data::LineChart::GetXInt32(UOSInt index, UOSInt *cnt) const
{
	if (this->xType != DataType::Integer)
		return 0;
	*cnt = this->xDataCnt->GetItem(index);
	return (Int32*)this->xDatas->GetItem(index);
}

UOSInt Data::LineChart::GetYDataCount() const
{
	return this->yCharts->GetCount();
}

Int64 *Data::LineChart::GetYDateTicks(UOSInt index, UOSInt *cnt) const
{
	ChartData *data = this->yCharts->GetItem(index);
	if (data == 0 || data->dataType != DataType::DateTicks)
		return 0;
	*cnt = data->dataCnt;
	return (Int64*)data->data;
}

Double *Data::LineChart::GetYDouble(UOSInt index, UOSInt *cnt) const
{
	ChartData *data = this->yCharts->GetItem(index);
	if (data == 0 || data->dataType != DataType::DOUBLE)
		return 0;
	*cnt = data->dataCnt;
	return (Double*)data->data;
}

Int32 *Data::LineChart::GetYInt32(UOSInt index, UOSInt *cnt) const
{
	ChartData *data = this->yCharts->GetItem(index);
	if (data == 0 || data->dataType != DataType::Integer)
		return 0;
	*cnt = data->dataCnt;
	return (Int32*)data->data;
}

Text::String *Data::LineChart::GetYName(UOSInt index) const
{
	ChartData *data = this->yCharts->GetItem(index);
	if (data == 0)
		return 0;
	return data->name.Ptr();
}

Data::Chart::DataType Data::LineChart::GetYType(UOSInt index) const
{
	ChartData *data = this->yCharts->GetItem(index);
	if (data == 0)
		return DataType::None;
	return data->dataType;
}

void Data::LineChart::Plot(NotNullPtr<Media::DrawImage> img, Double x, Double y, Double width, Double height) const
{
	if (height == 0 || width == 0)
		return;
	if (this->yCharts->GetCount() == 0)
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

	NotNullPtr<Media::DrawFont> fnt;
	Double fntH;
	Bool customX = false;

//	Int32 xAxisPos;
	Int32 xMaxInt = 0;
	Int32 xMinInt = 0;
	Double xMaxDbl = 0;
	Double xMinDbl = 0;
	Int64 xMaxDate = 0;
	Int64 xMinDate = 0;
	Data::Chart::DataType xType = this->xType;
//	Int32 yAxis1Pos;
	Data::Chart::DataType yAxis1Type;
//	Int32 yAxis2Pos;
	Data::Chart::DataType yAxis2Type;
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
	UTF8Char *sptr;
	NotNullPtr<Text::String> s;

	UOSInt i;
	UOSInt j;
	Data::DateTime dt1;
	Data::DateTime dt2;

	NotNullPtr<Media::DrawBrush> bgBrush = img->NewBrushARGB(bgColor);
	NotNullPtr<Media::DrawPen> boundPen = img->NewPenARGB(boundColor, this->lineThick, 0, 0);
	NotNullPtr<Media::DrawBrush> fontBrush = img->NewBrushARGB(fontColor);
	NotNullPtr<Media::DrawPen> gridPen = img->NewPenARGB(gridColor, this->lineThick, 0, 0);
	NotNullPtr<Media::DrawPen> refLinePen = img->NewPenARGB(refLineColor, this->lineThick, 0, 0);

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


	if (xType == Data::Chart::DataType::None || xDatas->GetCount() == 0)
	{
		if (this->yCharts->GetCount() == 0)
		{
			if (this->hasXRangeDate)
			{
				xType = Data::Chart::DataType::DateTicks;
				xMaxDate = this->xRangeDateMax;
				xMinDate = this->xRangeDateMin;
			}
		}
		else
		{
			xType = Data::Chart::DataType::Integer;
			UOSInt dataLeng;
			Data::LineChart::ChartData *data = ((Data::LineChart::ChartData*)yCharts->GetItem(0));
			Int32 *tmpData = MemAlloc(Int32, dataLeng = (i = data->dataCnt));
			while (i-- > 0)
				tmpData[i] = (Int32)i;
			this->xDatas->Add(tmpData);
			this->xDataCnt->Add(dataLeng);
			xMaxInt = (Int32)dataLeng - 1;
			xMinInt = 0;
			if (data->dataCnt == 2)
				xMaxInt = 1;
			customX = true;
		}
	}
	else if (xType == Data::Chart::DataType::DateTicks)
	{
		Int64 *tmpdata;
		UOSInt tmpdataCnt;
		xMaxDate = ((Int64*)xDatas->GetItem(0))[0];
		xMinDate = ((Int64*)xDatas->GetItem(0))[0];
		i = 0;
		while (i < this->xDatas->GetCount())
		{
			tmpdata = (Int64*)xDatas->GetItem(i);
			tmpdataCnt = xDataCnt->GetItem(i);
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
	else if (xType == Data::Chart::DataType::DOUBLE)
	{
		Double *tmpdata;
		UOSInt tmpdataCnt;
		xMinDbl = xMaxDbl = ((Double*)xDatas->GetItem(0))[0];
		i = 0;
		while (i < this->xDatas->GetCount())
		{
			tmpdata = (Double*)xDatas->GetItem(i);
			tmpdataCnt = xDataCnt->GetItem(i);
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
	else if (xType == Data::Chart::DataType::Integer)
	{
		Int32 *tmpdata;
		UOSInt tmpdataCnt;
		xMinInt = xMaxInt = ((Int32*)xDatas->GetItem(0))[0];
		i = 0;
		while (i < this->xDatas->GetCount())
		{
			tmpdata = (Int32*)xDatas->GetItem(i);
			tmpdataCnt = xDataCnt->GetItem(i);
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



	yAxis1Type = Data::Chart::DataType::None;
	yAxis2Type = Data::Chart::DataType::None;
	if (this->yCharts->GetCount() > 0)
	{
		i = 0;
		while (i < this->yCharts->GetCount())
		{
			Data::LineChart::ChartData *data = (Data::LineChart::ChartData*)this->yCharts->GetItem(i);
			if (data->dataType == Data::Chart::DataType::Integer)
			{
				Int32 *datas = (Int32*)data->data;
				if (yAxis1Type == Data::Chart::DataType::None)
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
					yAxis1Type = Data::Chart::DataType::Integer;
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
				else if (yAxis1Type == Data::Chart::DataType::Integer)
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
				else if (yAxis2Type == Data::Chart::DataType::None)
				{
					yAxis2Type = Data::Chart::DataType::Integer;
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
				else if (yAxis2Type == Data::Chart::DataType::Integer)
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
			else if (data->dataType == Data::Chart::DataType::DOUBLE)
			{
				Double *datas = (Double*)data->data;
				if (yAxis1Type == Data::Chart::DataType::None)
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
					yAxis1Type = Data::Chart::DataType::DOUBLE;
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
				else if (yAxis1Type == Data::Chart::DataType::DOUBLE)
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
				else if (yAxis2Type == Data::Chart::DataType::None)
				{
					yAxis2Type = Data::Chart::DataType::DOUBLE;
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
				else if (yAxis2Type == Data::Chart::DataType::DOUBLE)
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
			else if (data->dataType == Data::Chart::DataType::DateTicks)
			{
				Int64 *datas = (Int64*)data->data;
				if (yAxis1Type == Data::Chart::DataType::None)
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
					yAxis1Type = Data::Chart::DataType::DateTicks;
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
				else if (yAxis1Type == Data::Chart::DataType::DateTicks)
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
				else if (yAxis2Type == Data::Chart::DataType::None)
				{
					yAxis2Type = Data::Chart::DataType::DateTicks;
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
				else if (yAxis2Type == Data::Chart::DataType::DateTicks)
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
			yAxis1Type = Data::Chart::DataType::DOUBLE;
			y1MaxDbl = this->yRangeDblMax;
			y1MinDbl = this->yRangeDblMin;
		}
		else if (this->hasYRangeInt && this->yRangeIntMax != this->yRangeIntMin)
		{
			yAxis1Type = Data::Chart::DataType::Integer;
			y1MaxInt = this->yRangeIntMax;
			y1MinInt = this->yRangeIntMin;
		}
	}

	xLeng = 0;
	y1Leng = 0;
	y2Leng = 0;
	if (xType == Data::Chart::DataType::Integer)
	{
		sptr = Text::StrInt32(sbuff, xMaxInt);
		rcSize = img->GetTextSize(fnt, CSTRP(sbuff, sptr));
		xLeng = (Single)rcSize.x;

		sptr = Text::StrInt32(sbuff, xMinInt);
		rcSize = img->GetTextSize(fnt, CSTRP(sbuff, sptr));
		if (rcSize.x > xLeng)
			xLeng = (Single)rcSize.x;
	}
	else if (xType == Data::Chart::DataType::DOUBLE)
	{
		sptr = Text::StrDoubleFmt(sbuff, xMaxDbl, (const Char*)this->dblFormat->v);
		rcSize = img->GetTextSize(fnt, CSTRP(sbuff, sptr));
		xLeng = (Single)rcSize.x;
		
		sptr = Text::StrDoubleFmt(sbuff, xMinDbl, (const Char*)this->dblFormat->v);
		rcSize = img->GetTextSize(fnt, CSTRP(sbuff, sptr));
		if (rcSize.x > xLeng)
			xLeng = (Single)rcSize.x;
	}
	else if (xType == Data::Chart::DataType::DateTicks)
	{
		dt1.SetTicks(xMaxDate);
		dt1.ConvertTimeZoneQHR(this->timeZoneQHR);
		dt2.SetTicks(xMinDate);
		dt2.ConvertTimeZoneQHR(this->timeZoneQHR);
		if (dt1.IsSameDay(dt2))
		{
			sptr = dt1.ToString(sbuff, (const Char*)this->timeFormat->v);
			rcSize = img->GetTextSize(fnt, CSTRP(sbuff, sptr));
			xLeng = (Single)rcSize.x;
			if (dt2.GetMSPassedDate() == 0)
			{
				sptr = dt2.ToString(sbuff, (const Char*)this->dateFormat->v);
			}
			else
			{
				sptr = dt2.ToString(sbuff, (const Char*)this->timeFormat->v);
			}
			rcSize = img->GetTextSize(fnt, CSTRP(sbuff, sptr));
			if (rcSize.x > xLeng)
				xLeng = (Single)rcSize.x;
		}
		else
		{
			sptr = dt1.ToString(sbuff, (const Char*)this->dateFormat->v);
			rcSize = img->GetTextSize(fnt, CSTRP(sbuff, sptr));
			xLeng = (Single)rcSize.x;
			sptr = dt1.ToString(sbuff, (const Char*)this->timeFormat->v);
			rcSize = img->GetTextSize(fnt, CSTRP(sbuff, sptr));
			if (rcSize.x > xLeng)
				xLeng = (Single)rcSize.x;
		}
	}
	if (this->xAxisName)
	{
		rcSize = img->GetTextSize(fnt, this->xAxisName->ToCString());
		xLeng += rcSize.y;
	}
	xLeng += barLeng;


	if (yAxis1Type == Data::Chart::DataType::Integer)
	{
		sptr = Text::StrInt32(sbuff, y1MaxInt);
		if (this->yUnit)
			sptr = this->yUnit->ConcatTo(sptr);
		rcSize = img->GetTextSize(fnt, CSTRP(sbuff, sptr));
		y1Leng = rcSize.x;

		sptr = Text::StrInt32(sbuff, y1MinInt);
		if (this->yUnit)
			sptr = this->yUnit->ConcatTo(sptr);
		rcSize = img->GetTextSize(fnt, CSTRP(sbuff, sptr));
		if (rcSize.x > y1Leng)
			y1Leng = rcSize.x;
	}
	else if (yAxis1Type == Data::Chart::DataType::DOUBLE)
	{
		sptr = Text::StrDoubleFmt(sbuff, y1MaxDbl, (const Char*)this->dblFormat->v);
		if (this->yUnit)
			sptr = this->yUnit->ConcatTo(sptr);
		rcSize = img->GetTextSize(fnt, CSTRP(sbuff, sptr));
		y1Leng = rcSize.x;

		sptr = Text::StrDoubleFmt(sbuff, y1MinDbl, (const Char*)this->dblFormat->v);
		if (this->yUnit)
			sptr = this->yUnit->ConcatTo(sptr);
		rcSize = img->GetTextSize(fnt, CSTRP(sbuff, sptr));
		if (rcSize.x > y1Leng)
			y1Leng = rcSize.x;
	}
	else if (yAxis1Type == Data::Chart::DataType::DateTicks)
	{
		dt1.SetTicks(y1MaxDate);
		dt1.ConvertTimeZoneQHR(this->timeZoneQHR);
		sptr = dt1.ToString(sbuff, (const Char*)this->dateFormat->v);
		rcSize = img->GetTextSize(fnt, CSTRP(sbuff, sptr));
		y1Leng = rcSize.x;

		dt1.SetTicks(y1MinDate);
		dt1.ConvertTimeZoneQHR(this->timeZoneQHR);
		sptr = dt1.ToString(sbuff, (const Char*)this->dateFormat->v);
		rcSize = img->GetTextSize(fnt, CSTRP(sbuff, sptr));;
		if (rcSize.x > y1Leng)
			y1Leng = rcSize.x;
	}
	if (this->yAxisName)
	{
		rcSize = img->GetTextSize(fnt, this->yAxisName->ToCString());
		y1Leng += rcSize.y;
	}
	y1Leng += barLeng;



	if (yAxis2Type == Data::Chart::DataType::Integer)
	{
		sptr = Text::StrInt32(sbuff, y2MaxInt);
		if (this->yUnit)
			sptr = this->yUnit->ConcatTo(sptr);
		rcSize = img->GetTextSize(fnt, CSTRP(sbuff, sptr));
		y2Leng = rcSize.x;

		sptr = Text::StrInt32(sbuff, y2MinInt);
		if (this->yUnit)
			sptr = this->yUnit->ConcatTo(sptr);
		rcSize = img->GetTextSize(fnt, CSTRP(sbuff, sptr));
		if (rcSize.x > y2Leng)
			y2Leng = rcSize.x;

		y2Leng += barLeng;
		y2show = true;
	}
	else if (yAxis2Type == Data::Chart::DataType::DOUBLE)
	{
		sptr = Text::StrDoubleFmt(sbuff, y2MaxDbl, (const Char*)this->dblFormat->v);
		if (this->yUnit)
			sptr = this->yUnit->ConcatTo(sptr);
		rcSize = img->GetTextSize(fnt, CSTRP(sbuff, sptr));
		y2Leng = rcSize.x;

		sptr = Text::StrDoubleFmt(sbuff, y2MinDbl, (const Char*)this->dblFormat->v);
		if (this->yUnit)
			sptr = this->yUnit->ConcatTo(sptr);
		rcSize = img->GetTextSize(fnt, CSTRP(sbuff, sptr));
		if (rcSize.x > y2Leng)
			y2Leng = rcSize.x;

		y2Leng += barLeng;
		y2show = true;
	}
	else if (yAxis2Type == Data::Chart::DataType::DateTicks)
	{
		dt1.SetTicks(y2MaxDate);
		dt1.ConvertTimeZoneQHR(this->timeZoneQHR);
		sptr = dt1.ToString(sbuff, (const Char*)this->dateFormat->v);
		rcSize = img->GetTextSize(fnt, CSTRP(sbuff, sptr));
		y2Leng = rcSize.x;

		dt1.SetTicks(y2MinDate);
		dt1.ConvertTimeZoneQHR(this->timeZoneQHR);
		sptr = dt1.ToString(sbuff, (const Char*)this->dateFormat->v);
		rcSize = img->GetTextSize(fnt, CSTRP(sbuff, sptr));
		if (rcSize.x > y2Leng)
			y2Leng = rcSize.x;

		y2Leng += barLeng;
		y2show = true;
	}
	else if (yAxis2Type == Data::Chart::DataType::None)
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
	if (xType == Data::Chart::DataType::Integer)
	{
		Data::Chart::CalScaleMarkInt(&locations, &labels, xMinInt, xMaxInt, width - y1Leng - y2Leng - this->pointSize * 2, fntH, 0);
	}
	else if (xType == Data::Chart::DataType::DOUBLE)
	{
		Data::Chart::CalScaleMarkDbl(&locations, &labels, xMinDbl, xMaxDbl, width - y1Leng - y2Leng - this->pointSize * 2, fntH, (const Char*)this->dblFormat->v, minDblVal, 0);
	}
	else if (xType == Data::Chart::DataType::DateTicks)
	{
		dt1.SetTicks(xMinDate);
		dt1.ConvertTimeZoneQHR(this->timeZoneQHR);
		dt2.SetTicks(xMaxDate);
		dt2.ConvertTimeZoneQHR(this->timeZoneQHR);
		Data::Chart::CalScaleMarkDate(&locations, &labels, dt1, dt2, width - y1Leng - y2Leng - this->pointSize * 2, fntH, (const Char*)this->dateFormat->v, (const Char*)this->timeFormat->v);
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

	if (yAxis1Type == Data::Chart::DataType::Integer)
	{
		Data::Chart::CalScaleMarkInt(&locations, &labels, y1MinInt, y1MaxInt, height - xLeng - fntH / 2 - this->pointSize * 2, fntH, STR_PTR(this->yUnit));
	}
	else if (yAxis1Type == Data::Chart::DataType::DOUBLE)
	{
		Data::Chart::CalScaleMarkDbl(&locations, &labels, y1MinDbl, y1MaxDbl, height - xLeng - fntH / 2 - this->pointSize * 2, fntH, (const Char*)this->dblFormat->v, minDblVal, STR_PTR(this->yUnit));
	}
	else if (yAxis1Type == Data::Chart::DataType::DateTicks)
	{
		dt1.SetTicks(y1MinDate);
		dt1.ConvertTimeZoneQHR(this->timeZoneQHR);
		dt2.SetTicks(y2MaxDate);
		dt2.ConvertTimeZoneQHR(this->timeZoneQHR);
		Data::Chart::CalScaleMarkDate(&locations, &labels, dt1, dt2, height - xLeng - fntH / 2 - this->pointSize * 2, fntH, (const Char*)this->dateFormat->v, (const Char*)this->timeFormat->v);
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

	if (this->yAxisName)
	{
		Math::Size2DDbl sz = img->GetTextSize(fnt, this->yAxisName->ToCString());
		img->DrawStringRot(Math::Coord2DDbl((x + fntH / 2) - sz.y * 0.5, (y + (height - xLeng) / 2) - sz.x * 0.5), this->yAxisName->ToCString(), fnt, fontBrush, 90);
	}

	if (this->xAxisName)
	{
		rcSize = img->GetTextSize(fnt, this->xAxisName->ToCString());
		img->DrawString(Math::Coord2DDbl((x + y1Leng + (width - y1Leng - y2Leng) / 2 - rcSize.x / 2), (y + height - rcSize.y)), this->xAxisName->ToCString(), fnt, fontBrush);
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
	while (i < yCharts->GetCount())
	{
		void *xData;
		UOSInt xDataCnt;
		Data::LineChart::ChartData *chart = (Data::LineChart::ChartData*)yCharts->GetItem(i);

		if (xDatas->GetCount() > i)
		{
			xData = xDatas->GetItem(i);
			xDataCnt = this->xDataCnt->GetItem(i);
		}
		else
		{
			xData = xDatas->GetItem(0);
			xDataCnt = this->xDataCnt->GetItem(0);
		}

		if (chart->lineStyle == Data::LineChart::LS_FILL)
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
		if (xType == Data::Chart::DataType::DateTicks)
		{
			Int64 *data = (Int64*)xData;
			j = 0;
			while (j < xDataCnt)
			{
				currPos[j].x = (Double)(x + y1Leng + this->pointSize + Data::DateTimeUtil::MS2Minutes(data[j] - xMinDate) / Data::DateTimeUtil::MS2Minutes(xMaxDate - xMinDate) * xChartLeng);
				j++;
			}
		}
		else if (xType == Data::Chart::DataType::DOUBLE)
		{
			Double *data = (Double*)xData;
			j = 0;
			while (j < xDataCnt)
			{
				currPos[j].x = (Double)(x + y1Leng + this->pointSize + (data[j] - xMinDbl) / (xMaxDbl - xMinDbl) * xChartLeng);
				j++;
			}
		}
		else if (xType == Data::Chart::DataType::Integer)
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
		if (chart->dataType == Data::Chart::DataType::Integer)
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
		else if (chart->dataType == Data::Chart::DataType::DOUBLE)
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
		else if (chart->dataType == Data::Chart::DataType::DateTicks)
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

		if (chart->lineStyle == Data::LineChart::LS_FILL)
		{
			if (currPosLen >= 4)
			{
				j = currPosLen;
				currPos[j - 2].x = currPos[j - 3].x;
				currPos[j - 2].y = (Double)(y + height - xLeng);
				currPos[j - 1].x = currPos[0].x;
				currPos[j - 1].y = (Double)(y + height - xLeng);
				NotNullPtr<Media::DrawPen> p = img->NewPenARGB(((Data::LineChart::ChartData*)yCharts->GetItem(i))->lineColor, 1, 0, 0);
				NotNullPtr<Media::DrawBrush> b = img->NewBrushARGB(((Data::LineChart::ChartData*)yCharts->GetItem(i))->lineColor);
				img->DrawPolygon(currPos, currPosLen, p, b);
				img->DelBrush(b);
				img->DelPen(p);
			}
		}
		else
		{
			if (currPosLen >= 2)
			{
				NotNullPtr<Media::DrawPen> pen = img->NewPenARGB(((Data::LineChart::ChartData*)yCharts->GetItem(i))->lineColor, this->lineThick, 0, 0);
				img->DrawPolyline(currPos, currPosLen, pen);
				img->DelPen(pen);

				if (this->pointType == PT_CIRCLE && this->pointSize > 0)
				{
					NotNullPtr<Media::DrawBrush> b = img->NewBrushARGB(((Data::LineChart::ChartData*)yCharts->GetItem(i))->lineColor);
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

		if (yAxis1Type == Data::Chart::DataType::Integer)
		{
			iMax = y1MaxInt;
			iMin = y1MinInt;
			if (this->refInt >= iMin && this->refInt <= iMax)
			{
				yPos = (Single)(y + height - xLeng - (Double)(this->refInt - iMin) / (Single)(iMax - iMin) * xChartLeng);
				img->DrawLine((Double)(x + y1Leng), (Double)yPos, (Double)(x + width - y2Leng), (Double)yPos, refLinePen);

				sptr = Text::StrInt32(sbuff, this->refInt);
				if (this->yUnit)
					sptr = this->yUnit->ConcatTo(sptr);
				if (this->refType == RT_LEFTALIGN)
				{
					rcSize = img->GetTextSize(fnt, CSTRP(sbuff, sptr));
					img->DrawString(Math::Coord2DDbl(x + y1Leng, yPos - rcSize.y), CSTRP(sbuff, sptr), fnt, fontBrush);
				}
				else if (this->refType == RT_RIGHTALIGN)
				{
					rcSize = img->GetTextSize(fnt, CSTRP(sbuff, sptr));
					img->DrawString(Math::Coord2DDbl(x + width - y2Leng - rcSize.x, yPos - rcSize.y), CSTRP(sbuff, sptr), fnt, fontBrush);
				}
			}
		}
		else if (yAxis2Type == Data::Chart::DataType::Integer)
		{
			iMax = y2MaxInt;
			iMin = y2MinInt;
			if (this->refInt >= iMin && this->refInt <= iMax)
			{
				yPos = (Single)(y + height - xLeng - (Double)(this->refInt - iMin) / (Single)(iMax - iMin) * xChartLeng);
				img->DrawLine((Double)(x + y1Leng), (Double)yPos, (Double)(x + width - y2Leng), (Double)yPos, refLinePen);

				sptr = Text::StrInt32(sbuff, this->refInt);
				if (this->yUnit)
					sptr = this->yUnit->ConcatTo(sptr);
				if (this->refType == RT_LEFTALIGN)
				{
					rcSize = img->GetTextSize(fnt, CSTRP(sbuff, sptr));
					img->DrawString(Math::Coord2DDbl(x + y1Leng, yPos - rcSize.y), CSTRP(sbuff, sptr), fnt, fontBrush);
				}
				else if (this->refType == RT_RIGHTALIGN)
				{
					rcSize = img->GetTextSize(fnt, CSTRP(sbuff, sptr));
					img->DrawString(Math::Coord2DDbl(x + width - y2Leng - rcSize.x, yPos - rcSize.y), CSTRP(sbuff, sptr), fnt, fontBrush);
				}
			}
		}

		if (Data::Chart::DataType::DOUBLE == yAxis1Type)
		{
			dMax = y1MaxDbl;
			dMin = y1MinDbl;
			if (this->refDbl >= dMin && this->refDbl <= dMax)
			{
				yPos = (Single)(y + height - xLeng - (this->refDbl - dMin) / (dMax - dMin) * xChartLeng);
				img->DrawLine((Double)(x + y1Leng), (Double)yPos, (Double)(x + width - y2Leng), (Double)yPos, refLinePen);

				sptr = Text::StrDoubleFmt(sbuff, this->refDbl, (const Char*)this->dblFormat->v);
				if (this->yUnit)
					sptr = this->yUnit->ConcatTo(sptr);
				if (this->refType == RT_LEFTALIGN)
				{
					rcSize = img->GetTextSize(fnt, CSTRP(sbuff, sptr));
					img->DrawString(Math::Coord2DDbl(x + y1Leng, yPos - rcSize.y), CSTRP(sbuff, sptr), fnt, fontBrush);
				}
				else if (this->refType == RT_RIGHTALIGN)
				{
					rcSize = img->GetTextSize(fnt, CSTRP(sbuff, sptr));
					img->DrawString(Math::Coord2DDbl(x + width - y2Leng - rcSize.x, yPos - rcSize.y), CSTRP(sbuff, sptr), fnt, fontBrush);
				}
			}
		}
		else if  (Data::Chart::DataType::DOUBLE == yAxis2Type)
		{
			dMax = y2MaxDbl;
			dMin = y2MinDbl;
			if (this->refDbl >= dMin && this->refDbl <= dMax)
			{
				yPos = (Single)(y + height - xLeng - (this->refDbl - dMin) / (dMax - dMin) * xChartLeng);
				img->DrawLine((Double)(x + y1Leng), (Double)yPos, (Double)(x + width - y2Leng), (Double)yPos, refLinePen);

				sptr = Text::StrDoubleFmt(sbuff, this->refDbl, (const Char*)this->dblFormat->v);
				if (this->yUnit)
					sptr = this->yUnit->ConcatTo(sptr);
				if (this->refType == RT_LEFTALIGN)
				{
					rcSize = img->GetTextSize(fnt, CSTRP(sbuff, sptr));
					img->DrawString(Math::Coord2DDbl(x + y1Leng, yPos - rcSize.y), CSTRP(sbuff, sptr), fnt, fontBrush);
				}
				else if (this->refType == RT_RIGHTALIGN)
				{
					rcSize = img->GetTextSize(fnt, CSTRP(sbuff, sptr));
					img->DrawString(Math::Coord2DDbl(x + width - y2Leng - rcSize.x, yPos - rcSize.y), CSTRP(sbuff, sptr), fnt, fontBrush);
				}
			}
		}

		if (this->refTime != 0)
		{
			if (Data::Chart::DataType::DateTicks == yAxis1Type)
			{
				tMax = y1MaxDate;
				tMin = y1MinDate;
				if (this->refTime >= tMin && this->refTime <= tMax)
				{
					yPos = (Single)(y + height - xLeng - Data::DateTimeUtil::MS2Minutes(this->refTime - tMin) / Data::DateTimeUtil::MS2Minutes(tMax - tMin) * xChartLeng);
					img->DrawLine((Double)(x + y1Leng), (Double)yPos, (Double)(x + width - y2Leng), (Double)yPos, refLinePen);
				}
			}
			else if (Data::Chart::DataType::DateTicks == yAxis2Type)
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
		i = this->xDatas->GetCount();
		while (i-- > 0)
		{
			MemFree(this->xDatas->GetItem(i));
		}
		this->xDatas->Clear();
		this->xDataCnt->Clear();
	}
}

UOSInt Data::LineChart::GetLegendCount() const
{
	return this->yCharts->GetCount();
}

UTF8Char *Data::LineChart::GetLegend(UTF8Char *sbuff, UInt32 *color, UOSInt index) const
{
	if (index >= this->yCharts->GetCount())
		return 0;
	Data::LineChart::ChartData *cdata = this->yCharts->GetItem(index);
	*color = cdata->lineColor;
	return Text::StrConcatC(sbuff, cdata->name->v, cdata->name->leng);
}

Data::LineChart::ChartData::ChartData(Text::String *name, void *data, UOSInt dataCnt, Data::Chart::DataType dataType, UInt32 lineColor, Data::LineChart::LineStyle lineStyle)
{
	this->name = name->Clone();
	this->data = data;
	this->dataCnt = dataCnt;
	this->dataType = dataType;
	this->lineColor = lineColor;
	this->lineStyle = lineStyle;
}

Data::LineChart::ChartData::ChartData(Text::CString name, void *data, UOSInt dataCnt, Data::Chart::DataType dataType, UInt32 lineColor, Data::LineChart::LineStyle lineStyle)
{
	this->name = Text::String::New(name);
	this->data = data;
	this->dataCnt = dataCnt;
	this->dataType = dataType;
	this->lineColor = lineColor;
	this->lineStyle = lineStyle;
}

Data::LineChart::ChartData::~ChartData()
{
	this->name->Release();
	MemFree(this->data);
}
