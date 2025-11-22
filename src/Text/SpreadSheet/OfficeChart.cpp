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
}

Text::SpreadSheet::OfficeChart::~OfficeChart()
{
	OPTSTR_DEL(this->titleText);
	this->shapeProp.Delete();
	this->axes.DeleteAll();
	this->series.DeleteAll();
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

void Text::SpreadSheet::OfficeChart::SetTitleText(Text::CString titleText)
{
	OPTSTR_DEL(this->titleText);
	this->titleText = Text::String::NewOrNull(titleText);
}

Optional<Text::String> Text::SpreadSheet::OfficeChart::GetTitleText()
{
	return this->titleText;
}

Optional<Text::SpreadSheet::OfficeShapeProp> Text::SpreadSheet::OfficeChart::GetShapeProp()
{
	return this->shapeProp;
}

void Text::SpreadSheet::OfficeChart::SetShapeProp(Optional<OfficeShapeProp> shapeProp)
{
	this->shapeProp.Delete();
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

void Text::SpreadSheet::OfficeChart::InitChart(ChartType chartType, NN<OfficeChartAxis> categoryAxis, NN<OfficeChartAxis> valueAxis)
{
	this->chartType = chartType;
	this->categoryAxis = categoryAxis;
	this->valueAxis = valueAxis;
}

void Text::SpreadSheet::OfficeChart::InitLineChart(Text::CString leftAxisName, Text::CString bottomAxisName, AxisType bottomType)
{
	NN<OfficeChartAxis> leftAxis = this->CreateAxis(AxisType::Numeric, AxisPosition::Left);
	if (leftAxisName.leng > 0) leftAxis->SetTitle(leftAxisName);
	leftAxis->SetCrosses(AxisCrosses::AutoZero);
	leftAxis->SetMajorGridProp(NEW_CLASS_D(OfficeShapeProp(Optional<OfficeLineStyle>(NEW_CLASS_D(OfficeLineStyle(OfficeFill::NewSolidFill(OfficeColor::NewPreset(PresetColor::LightGray))))))));
	leftAxis->SetShapeProp(NEW_CLASS_D(OfficeShapeProp(Optional<OfficeLineStyle>(NEW_CLASS_D(OfficeLineStyle(OfficeFill::NewSolidFill(OfficeColor::NewPreset(PresetColor::Black))))))));
	NN<OfficeChartAxis> bottomAxis = this->CreateAxis(bottomType, AxisPosition::Bottom);
	if (bottomAxisName.leng > 0) bottomAxis->SetTitle(bottomAxisName);
	bottomAxis->SetShapeProp(NEW_CLASS_D(OfficeShapeProp(Optional<OfficeLineStyle>(NEW_CLASS_D(OfficeLineStyle(OfficeFill::NewSolidFill(OfficeColor::NewPreset(PresetColor::Black))))))));
	bottomAxis->SetTickLblPos(TickLabelPosition::Low);

	this->InitChart(ChartType::LineChart, bottomAxis, leftAxis);
}

Text::SpreadSheet::ChartType Text::SpreadSheet::OfficeChart::GetChartType()
{
	return this->chartType;
}

NN<Text::SpreadSheet::OfficeChartAxis> Text::SpreadSheet::OfficeChart::CreateAxis(AxisType axisType, AxisPosition axisPos)
{
	NN<OfficeChartAxis> axis;
	NEW_CLASSNN(axis, OfficeChartAxis(axisType, axisPos));
	this->axes.Add(axis);
	return axis;
}

UOSInt Text::SpreadSheet::OfficeChart::GetAxisCount()
{
	return this->axes.GetCount();
}

Optional<OfficeChartAxis> Text::SpreadSheet::OfficeChart::GetAxis(UOSInt index)
{
	return this->axes.GetItem(index);
}

UOSInt Text::SpreadSheet::OfficeChart::GetAxisIndex(NN<OfficeChartAxis> axis)
{
	return this->axes.IndexOf(axis);
}

Optional<OfficeChartAxis> Text::SpreadSheet::OfficeChart::GetCategoryAxis()
{
	return this->categoryAxis;
}

Optional<OfficeChartAxis> Text::SpreadSheet::OfficeChart::GetValueAxis()
{
	return this->valueAxis;
}

void Text::SpreadSheet::OfficeChart::AddSeries(NN<WorkbookDataSource> categoryData, NN<WorkbookDataSource> valueData, Optional<Text::String> name, Bool showMarker)
{
	UOSInt i = this->series.GetCount();
	NN<OfficeChartSeries> series;
	NEW_CLASSNN(series, OfficeChartSeries(categoryData, valueData));
	if (name.NotNull())
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
	this->series.Add(series);
}

void Text::SpreadSheet::OfficeChart::AddSeries(NN<WorkbookDataSource> categoryData, NN<WorkbookDataSource> valueData, Text::CString name, Bool showMarker)
{
	UOSInt i = this->series.GetCount();
	NN<OfficeChartSeries> series;
	NEW_CLASSNN(series, OfficeChartSeries(categoryData, valueData));
	if (name.leng > 0)
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
	this->series.Add(series);
}

UOSInt Text::SpreadSheet::OfficeChart::GetSeriesCount() const
{
	return this->series.GetCount();
}

NN<OfficeChartSeries> Text::SpreadSheet::OfficeChart::GetSeriesNoCheck(UOSInt index) const
{
	return this->series.GetItemNoCheck(index);
}

Optional<OfficeChartSeries> Text::SpreadSheet::OfficeChart::GetSeries(UOSInt index) const
{
	return this->series.GetItem(index);
}
