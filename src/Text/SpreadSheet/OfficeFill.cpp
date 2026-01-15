#include "Stdafx.h"
#include "MyMemory.h"
#include "Text/SpreadSheet/OfficeFill.h"

Text::SpreadSheet::OfficeFill::OfficeFill(FillType fillType, Optional<OfficeColor> color)
{
	this->fillType = fillType;
	this->color = color;
}

Text::SpreadSheet::OfficeFill::~OfficeFill()
{
	this->color.Delete();
}

Text::SpreadSheet::FillType Text::SpreadSheet::OfficeFill::GetFillType()
{
	return this->fillType;
}

Optional<Text::SpreadSheet::OfficeColor> Text::SpreadSheet::OfficeFill::GetColor()
{
	return this->color;
}

NN<Text::SpreadSheet::OfficeFill> Text::SpreadSheet::OfficeFill::NewSolidFill()
{
	NN<OfficeFill> fill;
	NEW_CLASSNN(fill, Text::SpreadSheet::OfficeFill(FillType::SolidFill, nullptr));
	return fill;
}

NN<Text::SpreadSheet::OfficeFill> Text::SpreadSheet::OfficeFill::NewSolidFill(Optional<OfficeColor> color)
{
	NN<OfficeFill> fill;
	NEW_CLASSNN(fill, Text::SpreadSheet::OfficeFill(FillType::SolidFill, color));
	return fill;
}
