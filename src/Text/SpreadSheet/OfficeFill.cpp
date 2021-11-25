#include "Stdafx.h"
#include "MyMemory.h"
#include "Text/SpreadSheet/OfficeFill.h"

Text::SpreadSheet::OfficeFill::OfficeFill(FillType fillType, OfficeColor *color)
{
	this->fillType = fillType;
	this->color = color;
}

Text::SpreadSheet::OfficeFill::~OfficeFill()
{
	SDEL_CLASS(this->color);
}

Text::SpreadSheet::FillType Text::SpreadSheet::OfficeFill::GetFillType()
{
	return this->fillType;
}

Text::SpreadSheet::OfficeColor *Text::SpreadSheet::OfficeFill::GetColor()
{
	return this->color;
}

Text::SpreadSheet::OfficeFill *Text::SpreadSheet::OfficeFill::NewSolidFill()
{
	return NEW_CLASS_D(Text::SpreadSheet::OfficeFill(FillType::SolidFill, 0));
}

Text::SpreadSheet::OfficeFill *Text::SpreadSheet::OfficeFill::NewSolidFill(OfficeColor *color)
{
	return NEW_CLASS_D(Text::SpreadSheet::OfficeFill(FillType::SolidFill, color));
}
