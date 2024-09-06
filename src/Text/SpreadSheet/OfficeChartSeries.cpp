#include "Stdafx.h"
#include "MyMemory.h"
#include "Text/MyString.h"
#include "Text/SpreadSheet/OfficeChartSeries.h"

Text::SpreadSheet::OfficeChartSeries::OfficeChartSeries(NN<WorkbookDataSource> categoryData, NN<WorkbookDataSource> valueData)
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
	this->categoryData.Delete();
	this->valueData.Delete();
	OPTSTR_DEL(this->title);
	this->shapeProp.Delete();
}

NN<Text::SpreadSheet::WorkbookDataSource> Text::SpreadSheet::OfficeChartSeries::GetCategoryData()
{
	return this->categoryData;
}

NN<Text::SpreadSheet::WorkbookDataSource> Text::SpreadSheet::OfficeChartSeries::GetValueData()
{
	return this->valueData;
}

Optional<Text::String> Text::SpreadSheet::OfficeChartSeries::GetTitle()
{
	return this->title;
}

void Text::SpreadSheet::OfficeChartSeries::SetTitle(Optional<Text::String> title, Text::String *dataSource)
{
	OPTSTR_DEL(this->title);
	this->title = Text::String::CopyOrNull(title);
}

void Text::SpreadSheet::OfficeChartSeries::SetTitle(Text::CString title, const UTF8Char *dataSource)
{
	OPTSTR_DEL(this->title);
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

Optional<Text::SpreadSheet::OfficeShapeProp> Text::SpreadSheet::OfficeChartSeries::GetShapeProp()
{
	return this->shapeProp;
}

void Text::SpreadSheet::OfficeChartSeries::SetShapeProp(Optional<OfficeShapeProp> shapeProp)
{
	this->shapeProp.Delete();
	this->shapeProp = shapeProp;
}

void Text::SpreadSheet::OfficeChartSeries::SetLineStyle(Optional<OfficeLineStyle> lineStyle)
{
	NN<OfficeShapeProp> shapeProp;
	if (this->shapeProp.SetTo(shapeProp))
	{
		shapeProp->SetLineStyle(lineStyle);
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
