#include "Stdafx.h"
#include "MyMemory.h"
#include "Text/MyString.h"
#include "Text/SpreadSheet/OfficeChartSeries.h"

Text::SpreadSheet::OfficeChartSeries::OfficeChartSeries(WorkbookDataSource *categoryData, WorkbookDataSource *valueData)
{
	this->categoryData = categoryData;
	this->valueData = valueData;
	this->title = 0;
	this->smooth = false;
	this->shapeProp = 0;
	this->markerSize = 0;
	this->markerStyle = MarkerStyle::None;
}

Text::SpreadSheet::OfficeChartSeries::~OfficeChartSeries()
{
	SDEL_CLASS(this->categoryData);
	SDEL_CLASS(this->valueData);
	SDEL_STRING(this->title);
	SDEL_CLASS(this->shapeProp);
}

Text::SpreadSheet::WorkbookDataSource *Text::SpreadSheet::OfficeChartSeries::GetCategoryData()
{
	return this->categoryData;
}

Text::SpreadSheet::WorkbookDataSource *Text::SpreadSheet::OfficeChartSeries::GetValueData()
{
	return this->valueData;
}

Text::String *Text::SpreadSheet::OfficeChartSeries::GetTitle()
{
	return this->title;
}

void Text::SpreadSheet::OfficeChartSeries::SetTitle(Text::String *title, Text::String *dataSource)
{
	SDEL_STRING(this->title);
	this->title = SCOPY_STRING(title);
}

void Text::SpreadSheet::OfficeChartSeries::SetTitle(Text::CString title, const UTF8Char *dataSource)
{
	SDEL_STRING(this->title);
	this->title = Text::String::NewOrNull(title);
}

Bool Text::SpreadSheet::OfficeChartSeries::IsSmooth()
{
	return this->smooth;
}

void Text::SpreadSheet::OfficeChartSeries::SetSmooth(Bool smooth)
{
	this->smooth = smooth;
}

Text::SpreadSheet::OfficeShapeProp *Text::SpreadSheet::OfficeChartSeries::GetShapeProp()
{
	return this->shapeProp;
}

void Text::SpreadSheet::OfficeChartSeries::SetShapeProp(OfficeShapeProp *shapeProp)
{
	SDEL_CLASS(this->shapeProp);
	this->shapeProp = shapeProp;
}

void Text::SpreadSheet::OfficeChartSeries::SetLineStyle(OfficeLineStyle *lineStyle)
{
	if (this->shapeProp)
	{
		this->shapeProp->SetLineStyle(lineStyle);
	}
	else
	{
		this->shapeProp = NEW_CLASS_D(OfficeShapeProp(lineStyle));
	}
}

UInt32 Text::SpreadSheet::OfficeChartSeries::GetMarkerSize()
{
	return this->markerSize;
}

void Text::SpreadSheet::OfficeChartSeries::SetMarkerSize(UInt32 markerSize)
{
	this->markerSize = markerSize;
}

Text::SpreadSheet::MarkerStyle Text::SpreadSheet::OfficeChartSeries::GetMarkerStyle()
{
	return this->markerStyle;
}

void Text::SpreadSheet::OfficeChartSeries::SetMarkerStyle(MarkerStyle markerStyle)
{
	this->markerStyle = markerStyle;
}
