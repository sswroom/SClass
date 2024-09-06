#include "Stdafx.h"
#include "MyMemory.h"
#include "Text/SpreadSheet/OfficeShapeProp.h"

Text::SpreadSheet::OfficeShapeProp::OfficeShapeProp()
{
	this->fill = 0;
	this->lineStyle = 0;
}

Text::SpreadSheet::OfficeShapeProp::OfficeShapeProp(Optional<OfficeFill> fill)
{
	this->fill = fill;
	this->lineStyle = 0;
}

Text::SpreadSheet::OfficeShapeProp::OfficeShapeProp(Optional<OfficeLineStyle> lineStyle)
{
	this->fill = 0;
	this->lineStyle = lineStyle;
}

Text::SpreadSheet::OfficeShapeProp::OfficeShapeProp(Optional<OfficeFill> fill, Optional<OfficeLineStyle> lineStyle)
{
	this->fill = fill;
	this->lineStyle = lineStyle;
}

Text::SpreadSheet::OfficeShapeProp::~OfficeShapeProp()
{
	this->fill.Delete();
	this->lineStyle.Delete();
}

Optional<Text::SpreadSheet::OfficeFill> Text::SpreadSheet::OfficeShapeProp::GetFill()
{
	return this->fill;
}

void Text::SpreadSheet::OfficeShapeProp::SetFill(Optional<OfficeFill> fill)
{
	this->fill.Delete();
	this->fill = fill;
}

Optional<Text::SpreadSheet::OfficeLineStyle> Text::SpreadSheet::OfficeShapeProp::GetLineStyle()
{
	return this->lineStyle;
}

void Text::SpreadSheet::OfficeShapeProp::SetLineStyle(Optional<OfficeLineStyle> lineStyle)
{
	this->lineStyle.Delete();
	this->lineStyle = lineStyle;
}
