#include "Stdafx.h"
#include "MyMemory.h"
#include "Text/SpreadSheet/OfficeShapeProp.h"

Text::SpreadSheet::OfficeShapeProp::OfficeShapeProp()
{
	this->fill = 0;
	this->lineStyle = 0;
}

Text::SpreadSheet::OfficeShapeProp::OfficeShapeProp(OfficeFill *fill)
{
	this->fill = fill;
	this->lineStyle = 0;
}

Text::SpreadSheet::OfficeShapeProp::OfficeShapeProp(OfficeLineStyle *lineStyle)
{
	this->fill = 0;
	this->lineStyle = lineStyle;
}

Text::SpreadSheet::OfficeShapeProp::OfficeShapeProp(OfficeFill *fill, OfficeLineStyle *lineStyle)
{
	this->fill = fill;
	this->lineStyle = lineStyle;
}

Text::SpreadSheet::OfficeShapeProp::~OfficeShapeProp()
{
	SDEL_CLASS(this->fill);
	SDEL_CLASS(this->lineStyle);
}

Text::SpreadSheet::OfficeFill *Text::SpreadSheet::OfficeShapeProp::GetFill()
{
	return this->fill;
}

void Text::SpreadSheet::OfficeShapeProp::SetFill(OfficeFill *fill)
{
	SDEL_CLASS(this->fill);
	this->fill = fill;
}

Text::SpreadSheet::OfficeLineStyle *Text::SpreadSheet::OfficeShapeProp::GetLineStyle()
{
	return this->lineStyle;
}

void Text::SpreadSheet::OfficeShapeProp::SetLineStyle(OfficeLineStyle *lineStyle)
{
	SDEL_CLASS(this->lineStyle);
	this->lineStyle = lineStyle;
}
