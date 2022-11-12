#include "Stdafx.h"
#include "MyMemory.h"
#include "Text/MyString.h"
#include "Text/SpreadSheet/CellStyle.h"

Text::SpreadSheet::CellStyle::BorderStyle::BorderStyle(UInt32 borderColor, BorderType borderType)
{
	this->borderColor = borderColor;
	this->borderType = borderType;
}

Bool Text::SpreadSheet::CellStyle::BorderStyle::operator!=(const BorderStyle &border) const
{
	return !(*this == border);
}

Bool Text::SpreadSheet::CellStyle::BorderStyle::operator==(const BorderStyle &border) const
{
	if (border.borderType == BorderType::None && this->borderType == BorderType::None)
		return true;
	else if (border.borderType != this->borderType)
		return false;
	else if (border.borderColor != this->borderColor)
		return false;
	else
		return true;
}

Text::SpreadSheet::CellStyle::CellStyle(UOSInt index)
{
	this->index = index;
	this->id = 0;
	this->halign = HAlignment::Unknown;
	this->valign = VAlignment::Unknown;
	this->wordWrap = false;
	this->borderBottom.borderType = Text::SpreadSheet::BorderType::None;
	this->borderLeft.borderType = Text::SpreadSheet::BorderType::None;
	this->borderRight.borderType = Text::SpreadSheet::BorderType::None;
	this->borderTop.borderType = Text::SpreadSheet::BorderType::None;
	this->font = 0;
	this->fillColor = 0xffffff;
	this->fillPattern = FP_NO_FILL;
	this->dataFormat = 0;
	this->protection = false;
}

Text::SpreadSheet::CellStyle::~CellStyle()
{
	SDEL_TEXT(this->id);
	SDEL_STRING(this->dataFormat);
}

Text::SpreadSheet::CellStyle *Text::SpreadSheet::CellStyle::Clone() const
{
	Text::SpreadSheet::CellStyle *style;
	NEW_CLASS(style, Text::SpreadSheet::CellStyle(this->index));
	style->id = SCOPY_TEXT(this->id);
	style->halign = this->halign;
	style->valign = this->valign;
	style->wordWrap = this->wordWrap;
	style->borderBottom = this->borderBottom;
	style->borderLeft = this->borderLeft;
	style->borderRight = this->borderRight;
	style->borderTop = this->borderTop;
	style->font = this->font;
	style->fillColor = this->fillColor;
	style->fillPattern = this->fillPattern;
	style->dataFormat = SCOPY_STRING(this->dataFormat);
	style->protection = this->protection;
	return style;
}

void Text::SpreadSheet::CellStyle::CopyFrom(CellStyle *style)
{
	this->index = style->index;
	SDEL_TEXT(this->id);
	this->id = SCOPY_TEXT(style->id);
	this->halign = style->halign;
	this->valign = style->valign;
	this->wordWrap = style->wordWrap;
	this->borderBottom = style->borderBottom;
	this->borderLeft = style->borderLeft;
	this->borderRight = style->borderRight;
	this->borderTop = style->borderTop;
	this->font = style->font;
	this->fillColor = style->fillColor;
	this->fillPattern = style->fillPattern;
	SDEL_STRING(this->dataFormat);
	this->dataFormat = SCOPY_STRING(style->dataFormat);
	this->protection = style->protection;
}

Bool Text::SpreadSheet::CellStyle::Equals(const CellStyle *style) const
{
	if (style->halign != this->halign)
		return false;
	if (style->valign != this->valign)
		return false;
	if (style->wordWrap != this->wordWrap)
		return false;

	if (style->borderBottom != this->borderBottom)
		return false;
	if (style->borderLeft != this->borderLeft)
		return false;
	if (style->borderRight != this->borderRight)
		return false;
	if (style->borderTop != this->borderTop)
		return false;

	if (this->font == 0)
	{
		if (style->font != 0)
			return false;
	}
	else if (style->font == 0)
	{
		return false;
	}
	else
	{
		if (!style->font->Equals(this->font))
			return false;
	}
	if (style->fillColor != this->fillColor)
		return false;
	if (style->fillPattern != this->fillPattern)
		return false;
	if (this->dataFormat != style->dataFormat)
	{
		if (this->dataFormat == 0 || style->dataFormat == 0 || !this->dataFormat->Equals(style->dataFormat))
			return false;
	}
	if (style->protection != this->protection)
		return false;
	return true;
}

Text::SpreadSheet::CellStyle *Text::SpreadSheet::CellStyle::SetIndex(UOSInt index)
{
	this->index = index;
	return this;
}

Text::SpreadSheet::CellStyle *Text::SpreadSheet::CellStyle::SetID(const UTF8Char *id)
{
	if (id == 0)
		return this;
	SDEL_TEXT(this->id)
	this->id = Text::StrCopyNew(id);
	return this;
}

Text::SpreadSheet::CellStyle *Text::SpreadSheet::CellStyle::SetHAlign(HAlignment halign)
{
	this->halign = halign;
	return this;
}

Text::SpreadSheet::CellStyle *Text::SpreadSheet::CellStyle::SetVAlign(VAlignment valign)
{
	this->valign = valign;
	return this;
}

Text::SpreadSheet::CellStyle *Text::SpreadSheet::CellStyle::SetWordWrap(Bool wordWrap)
{
	this->wordWrap = wordWrap;
	return this;
}

Text::SpreadSheet::CellStyle *Text::SpreadSheet::CellStyle::SetFillColor(UInt32 color, FillPattern pattern)
{
	this->fillColor = color;
	this->fillPattern = pattern;
	return this;
}

Text::SpreadSheet::CellStyle *Text::SpreadSheet::CellStyle::SetFont(Text::SpreadSheet::WorkbookFont *font)
{
	this->font = font;
	return this;
}

Text::SpreadSheet::CellStyle *Text::SpreadSheet::CellStyle::SetBorderLeft(const BorderStyle &border)
{
	this->borderLeft = border;
	return this;
}

Text::SpreadSheet::CellStyle *Text::SpreadSheet::CellStyle::SetBorderRight(const BorderStyle &border)
{
	this->borderRight = border;
	return this;
}

Text::SpreadSheet::CellStyle *Text::SpreadSheet::CellStyle::SetBorderTop(const BorderStyle &border)
{
	this->borderTop = border;
	return this;
}

Text::SpreadSheet::CellStyle *Text::SpreadSheet::CellStyle::SetBorderBottom(const BorderStyle &border)
{
	this->borderBottom = border;
	return this;
}

Text::SpreadSheet::CellStyle *Text::SpreadSheet::CellStyle::SetDataFormat(Text::String *dataFormat)
{
	SDEL_STRING(this->dataFormat);
	this->dataFormat = SCOPY_STRING(dataFormat);
	return this;
}

Text::SpreadSheet::CellStyle *Text::SpreadSheet::CellStyle::SetDataFormat(Text::CString dataFormat)
{
	SDEL_STRING(this->dataFormat);
	this->dataFormat = Text::String::NewOrNull(dataFormat);
	return this;
}

UOSInt Text::SpreadSheet::CellStyle::GetIndex() const
{
	return this->index;
}

const UTF8Char *Text::SpreadSheet::CellStyle::GetID() const
{
	return this->id;
}

Text::HAlignment Text::SpreadSheet::CellStyle::GetHAlign() const
{
	return this->halign;
}

Text::VAlignment Text::SpreadSheet::CellStyle::GetVAlign() const
{
	return this->valign;
}

Bool Text::SpreadSheet::CellStyle::GetWordWrap() const
{
	return this->wordWrap;
}

UInt32 Text::SpreadSheet::CellStyle::GetFillColor() const
{
	return this->fillColor;
}

Text::SpreadSheet::CellStyle::FillPattern Text::SpreadSheet::CellStyle::GetFillPattern() const
{
	return this->fillPattern;
}

Text::SpreadSheet::WorkbookFont *Text::SpreadSheet::CellStyle::GetFont() const
{
	return this->font;
}

Text::SpreadSheet::CellStyle::BorderStyle Text::SpreadSheet::CellStyle::GetBorderLeft() const
{
	return this->borderLeft;
}

Text::SpreadSheet::CellStyle::BorderStyle Text::SpreadSheet::CellStyle::GetBorderRight() const
{
	return this->borderRight;
}

Text::SpreadSheet::CellStyle::BorderStyle Text::SpreadSheet::CellStyle::GetBorderTop() const
{
	return this->borderTop;
}

Text::SpreadSheet::CellStyle::BorderStyle Text::SpreadSheet::CellStyle::GetBorderBottom() const
{
	return this->borderBottom;
}

Text::String *Text::SpreadSheet::CellStyle::GetDataFormat() const
{
	return this->dataFormat;
}
