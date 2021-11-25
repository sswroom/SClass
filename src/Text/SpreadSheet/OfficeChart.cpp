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
	this->shapeLine = 0;
	this->shapeFill = 0;
}

Text::SpreadSheet::OfficeChart::~OfficeChart()
{
	SDEL_TEXT(this->titleText);
	SDEL_CLASS(this->shapeLine);
	SDEL_CLASS(this->shapeFill);
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

Bool Text::SpreadSheet::OfficeChart::HasShapeProp()
{
	return this->shapeLine != 0 || this->shapeFill != 0;
}

void Text::SpreadSheet::OfficeChart::SetShapeLineStyle(OfficeLineStyle *lineStyle)
{
	SDEL_CLASS(this->shapeLine);
	this->shapeLine = lineStyle;
}

Text::SpreadSheet::OfficeLineStyle *Text::SpreadSheet::OfficeChart::GetShapeLineStyle()
{
	return this->shapeLine;
}

void Text::SpreadSheet::OfficeChart::SetShapeFillStyle(OfficeFill *fill)
{
	SDEL_CLASS(this->shapeFill);
	this->shapeFill = fill;
}

Text::SpreadSheet::OfficeFill *Text::SpreadSheet::OfficeChart::GetShapeFillStyle()
{
	return this->shapeFill;
}
