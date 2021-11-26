#include "Stdafx.h"
#include "MyMemory.h"
#include "Text/MyString.h"
#include "Text/SpreadSheet/OfficeChart.h"

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
}

Text::SpreadSheet::OfficeChart::~OfficeChart()
{
	SDEL_TEXT(this->titleText);
	SDEL_CLASS(this->shapeProp);
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
