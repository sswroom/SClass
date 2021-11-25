#include "Stdafx.h"
#include "MyMemory.h"
#include "Text/SpreadSheet/OfficeLineStyle.h"

Text::SpreadSheet::OfficeLineStyle::OfficeLineStyle(OfficeFill *fill)
{
	this->fill = fill;
}

Text::SpreadSheet::OfficeLineStyle::~OfficeLineStyle()
{
	SDEL_CLASS(this->fill);
}

Text::SpreadSheet::OfficeFill *Text::SpreadSheet::OfficeLineStyle::GetFillStyle()
{
	return this->fill;
}
