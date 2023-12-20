#include "Stdafx.h"
#include "MyMemory.h"
#include "Text/MyString.h"
#include "Text/SpreadSheet/OfficeChartAxis.h"

Text::SpreadSheet::OfficeChartAxis::OfficeChartAxis(AxisType axisType, AxisPosition axisPos)
{
	this->axisType = axisType;
	this->axisPos = axisPos;
	this->title = 0;
	this->shapeProp = 0;
	this->majorGridProp = 0;
	this->tickLblPos = TickLabelPosition::NextTo;
	this->crosses = AxisCrosses::AutoZero;
}

Text::SpreadSheet::OfficeChartAxis::~OfficeChartAxis()
{
	OPTSTR_DEL(this->title);
	SDEL_CLASS(this->shapeProp);
	SDEL_CLASS(this->majorGridProp);
}

Text::SpreadSheet::AxisType Text::SpreadSheet::OfficeChartAxis::GetAxisType()
{
	return this->axisType;
}

Text::SpreadSheet::AxisPosition Text::SpreadSheet::OfficeChartAxis::GetAxisPos()
{
	return this->axisPos;
}

Optional<Text::String> Text::SpreadSheet::OfficeChartAxis::GetTitle()
{
	return this->title;
}

void Text::SpreadSheet::OfficeChartAxis::SetTitle(Text::CString title)
{
	OPTSTR_DEL(this->title);
	this->title = Text::String::NewOrNull(title);
}

Text::SpreadSheet::OfficeShapeProp *Text::SpreadSheet::OfficeChartAxis::GetShapeProp()
{
	return this->shapeProp;
}

void Text::SpreadSheet::OfficeChartAxis::SetShapeProp(OfficeShapeProp *shapeProp)
{
	SDEL_CLASS(this->shapeProp);
	this->shapeProp = shapeProp;
}

Text::SpreadSheet::OfficeShapeProp *Text::SpreadSheet::OfficeChartAxis::GetMajorGridProp()
{
	return this->majorGridProp;
}

void Text::SpreadSheet::OfficeChartAxis::SetMajorGridProp(OfficeShapeProp *majorGridProp)
{
	SDEL_CLASS(this->majorGridProp);
	this->majorGridProp = majorGridProp;
}

Text::SpreadSheet::TickLabelPosition Text::SpreadSheet::OfficeChartAxis::GetTickLblPos()
{
	return this->tickLblPos;
}

void Text::SpreadSheet::OfficeChartAxis::SetTickLblPos(TickLabelPosition tickLblPos)
{
	this->tickLblPos = tickLblPos;
}

Text::SpreadSheet::AxisCrosses Text::SpreadSheet::OfficeChartAxis::GetCrosses()
{
	return this->crosses;
}

void Text::SpreadSheet::OfficeChartAxis::SetCrosses(AxisCrosses axisCrosses)
{
	this->crosses = axisCrosses;
}
