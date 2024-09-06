#include "Stdafx.h"
#include "MyMemory.h"
#include "Text/SpreadSheet/OfficeLineStyle.h"

Text::SpreadSheet::OfficeLineStyle::OfficeLineStyle(Optional<OfficeFill> fill)
{
	this->fill = fill;
}

Text::SpreadSheet::OfficeLineStyle::~OfficeLineStyle()
{
	this->fill.Delete();
}

Optional<Text::SpreadSheet::OfficeFill> Text::SpreadSheet::OfficeLineStyle::GetFillStyle()
{
	return this->fill;
}
