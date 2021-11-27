#include "Stdafx.h"
#include "MyMemory.h"
#include "Text/MyString.h"
#include "Text/SpreadSheet/OfficeChart.h"

using namespace Text::SpreadSheet;

PresetColor Text::SpreadSheet::OfficeChart::seriesColor[] = {
	PresetColor::DarkBlue,
	PresetColor::Aqua,
	PresetColor::Fuchsia,
	PresetColor::BlueViolet,
	PresetColor::Lavender,
	PresetColor::GreenYellow,
	PresetColor::Khaki,
	PresetColor::Honeydew,
	PresetColor::Magenta,
	PresetColor::Orchid,
	PresetColor::Thistle
};

Text::SpreadSheet::OfficeChart::OfficeChart(Math::Unit::Distance::DistanceUnit du, Double x, Double y, Double w, Double h)
{
	this->xInch = Math::Unit::Distance::Convert(du, Math::Unit::Distance::DU_INCH, x);
	this->yInch = Math::Unit::Distance::Convert(du, Math::Unit::Distance::DU_INCH, y);
	this->wInch = Math::Unit::Distance::Convert(du, Math::Unit::Distance::DU_INCH, w);
	this->hInch = Math::Unit::Distance::Convert(du, Math::Unit::Distance::DU_INCH, h);
	this->titleText = 0;
	this->shapeProp = 0;
	this->hasLegend = false;
	this->legendPos = LegendPos::Bottom;
	this->legendOverlay = false;
	this->displayBlankAs = BlankAs::Default;
	this->chartType = ChartType::Unknown;
	this->categoryAxis = 0;
	this->valueAxis = 0;
	NEW_CLASS(this->axes, Data::ArrayList<OfficeChartAxis*>());
	NEW_CLASS(this->series, Data::ArrayList<OfficeChartSeries*>());
}

Text::SpreadSheet::OfficeChart::~OfficeChart()
{
	SDEL_TEXT(this->titleText);
	SDEL_CLASS(this->shapeProp);
	UOSInt i = this->axes->GetCount();
	while (i-- > 0)
	{
		OfficeChartAxis *axis = this->axes->GetItem(i);
		DEL_CLASS(axis);
	}
	DEL_CLASS(this->axes);
	i = this->series->GetCount();
	while (i-- > 0)
	{
		OfficeChartSeries *series = this->series->GetItem(i);
		DEL_CLASS(series);
	}
	DEL_CLASS(this->series);
}

Double Text::SpreadSheet::OfficeChart::GetXInch()
{
	return this->xInch;
}

Double Text::SpreadSheet::OfficeChart::GetYInch()
{
	return this->yInch;
}

Double Text::SpreadSheet::OfficeChart::GetWInch()
{
	return this->wInch;
}

Double Text::SpreadSheet::OfficeChart::GetHInch()
{
	return this->hInch;
}

void Text::SpreadSheet::OfficeChart::SetTitleText(const UTF8Char *titleText)
{
	SDEL_TEXT(this->titleText);
	this->titleText = Text::StrCopyNew(titleText);
}

const UTF8Char *Text::SpreadSheet::OfficeChart::GetTitleText()
{
	return this->titleText;
}

Text::SpreadSheet::OfficeShapeProp *Text::SpreadSheet::OfficeChart::GetShapeProp()
{
	return this->shapeProp;
}

void Text::SpreadSheet::OfficeChart::SetShapeProp(OfficeShapeProp *shapeProp)
{
	SDEL_CLASS(this->shapeProp);
	this->shapeProp = shapeProp;
}

void Text::SpreadSheet::OfficeChart::AddLegend(LegendPos pos)
{
	this->hasLegend = true;
	this->legendPos = pos;
	this->legendOverlay = false;
}

Bool Text::SpreadSheet::OfficeChart::HasLegend()
{
	return this->hasLegend;
}

Text::SpreadSheet::LegendPos Text::SpreadSheet::OfficeChart::GetLegendPos()
{
	return this->legendPos;
}

Bool Text::SpreadSheet::OfficeChart::IsLegendOverlay()
{
	return this->legendOverlay;
}

void Text::SpreadSheet::OfficeChart::SetDisplayBlankAs(BlankAs displayBlankAs)
{
	this->displayBlankAs = displayBlankAs;
}

Text::SpreadSheet::BlankAs Text::SpreadSheet::OfficeChart::GetDisplayBlankAs()
{
	return this->displayBlankAs;
}

void Text::SpreadSheet::OfficeChart::InitChart(ChartType chartType, OfficeChartAxis *categoryAxis, OfficeChartAxis *valueAxis)
{
	this->chartType = chartType;
	this->categoryAxis = categoryAxis;
	this->valueAxis = valueAxis;
}

void Text::SpreadSheet::OfficeChart::InitLineChart(const UTF8Char *leftAxisName, const UTF8Char *bottomAxisName, AxisType bottomType)
{
	OfficeChartAxis *leftAxis = this->CreateAxis(AxisType::Numeric, AxisPosition::Left);
	if (leftAxisName) leftAxis->SetTitle(leftAxisName);
	leftAxis->SetCrosses(AxisCrosses::AutoZero);
	leftAxis->SetMajorGridProp(NEW_CLASS_D(OfficeShapeProp(NEW_CLASS_D(OfficeLineStyle(OfficeFill::NewSolidFill(OfficeColor::NewPreset(PresetColor::LightGray)))))));
	leftAxis->SetShapeProp(NEW_CLASS_D(OfficeShapeProp(NEW_CLASS_D(OfficeLineStyle(OfficeFill::NewSolidFill(OfficeColor::NewPreset(PresetColor::Black)))))));
	OfficeChartAxis *bottomAxis = this->CreateAxis(bottomType, AxisPosition::Bottom);
	if (bottomAxisName) bottomAxis->SetTitle(bottomAxisName);
	bottomAxis->SetShapeProp(NEW_CLASS_D(OfficeShapeProp(NEW_CLASS_D(OfficeLineStyle(OfficeFill::NewSolidFill(OfficeColor::NewPreset(PresetColor::Black)))))));
	bottomAxis->SetTickLblPos(TickLabelPosition::Low);

	this->InitChart(ChartType::LineChart, bottomAxis, leftAxis);
}

Text::SpreadSheet::ChartType Text::SpreadSheet::OfficeChart::GetChartType()
{
	return this->chartType;
}

Text::SpreadSheet::OfficeChartAxis *Text::SpreadSheet::OfficeChart::CreateAxis(AxisType axisType, AxisPosition axisPos)
{
	OfficeChartAxis *axis = NEW_CLASS_D(OfficeChartAxis(axisType, axisPos));
	this->axes->Add(axis);
	return axis;
}

UOSInt Text::SpreadSheet::OfficeChart::GetAxisCount()
{
	return this->axes->GetCount();
}

OfficeChartAxis *Text::SpreadSheet::OfficeChart::GetAxis(UOSInt index)
{
	return this->axes->GetItem(index);
}

UOSInt Text::SpreadSheet::OfficeChart::GetAxisIndex(OfficeChartAxis *axis)
{
	return this->axes->IndexOf(axis);
}

OfficeChartAxis *Text::SpreadSheet::OfficeChart::GetCategoryAxis()
{
	return this->categoryAxis;
}

OfficeChartAxis *Text::SpreadSheet::OfficeChart::GetValueAxis()
{
	return this->valueAxis;
}

void Text::SpreadSheet::OfficeChart::AddSeries(WorkbookDataSource *categoryData, WorkbookDataSource *valueData, const UTF8Char *name, Bool showMarker)
{
	UOSInt i = this->series->GetCount();
	OfficeChartSeries *series = NEW_CLASS_D(OfficeChartSeries(categoryData, valueData));
	if (name)
		series->SetTitle(name, 0);
	series->SetSmooth(false);
	if (showMarker)
	{
		series->SetMarkerSize(3);
		series->SetMarkerStyle(MarkerStyle::Circle);
	}
	else
	{
		series->SetMarkerStyle(MarkerStyle::None);
	}
	series->SetLineStyle(NEW_CLASS_D(OfficeLineStyle(OfficeFill::NewSolidFill(OfficeColor::NewPreset(seriesColor[i % (sizeof(seriesColor) / sizeof(seriesColor[0]))])))));
	this->series->Add(series);
}

UOSInt Text::SpreadSheet::OfficeChart::GetSeriesCount()
{
	return this->series->GetCount();
}

OfficeChartSeries *Text::SpreadSheet::OfficeChart::GetSeries(UOSInt index)
{
	return this->series->GetItem(index);
}
