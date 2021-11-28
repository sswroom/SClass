#include "Stdafx.h"
#include "MyMemory.h"
#include "Text/MyString.h"
#include "Text/SpreadSheet/CellStyle.h"

Bool Text::SpreadSheet::CellStyle::BorderStyle::Equals(BorderStyle *border)
{
	if (border->borderType == BorderType::None && this->borderType == BorderType::None)
		return true;
	else if (border->borderType != this->borderType)
		return false;
	else if (border->borderColor != this->borderColor)
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
	SDEL_TEXT(this->dataFormat);
}

Text::SpreadSheet::CellStyle *Text::SpreadSheet::CellStyle::Clone()
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
	style->dataFormat = SCOPY_TEXT(this->dataFormat);
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
	SDEL_TEXT(this->dataFormat);
	this->dataFormat = SCOPY_TEXT(style->dataFormat);
	this->protection = style->protection;
}

Bool Text::SpreadSheet::CellStyle::Equals(CellStyle *style)
{
	if (style->index != this->index)
		return false;
	if (style->halign != this->halign)
		return false;
	if (style->valign != this->valign)
		return false;
	if (style->wordWrap != this->wordWrap)
		return false;

	if (!style->borderBottom.Equals(&this->borderBottom))
		return false;
	if (!style->borderLeft.Equals(&this->borderLeft))
		return false;
	if (!style->borderRight.Equals(&this->borderRight))
		return false;
	if (!style->borderTop.Equals(&this->borderTop))
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
	if (!Text::StrEqualsN(this->dataFormat, style->dataFormat))
		return false;
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

Text::SpreadSheet::CellStyle *Text::SpreadSheet::CellStyle::SetBorderLeft(Text::SpreadSheet::CellStyle::BorderStyle *border)
{
	MemCopyNO(&this->borderLeft, border, sizeof(BorderStyle));
	return this;
}

Text::SpreadSheet::CellStyle *Text::SpreadSheet::CellStyle::SetBorderRight(Text::SpreadSheet::CellStyle::BorderStyle *border)
{
	MemCopyNO(&this->borderRight, border, sizeof(BorderStyle));
	return this;
}

Text::SpreadSheet::CellStyle *Text::SpreadSheet::CellStyle::SetBorderTop(Text::SpreadSheet::CellStyle::BorderStyle *border)
{
	MemCopyNO(&this->borderTop, border, sizeof(BorderStyle));
	return this;
}

Text::SpreadSheet::CellStyle *Text::SpreadSheet::CellStyle::SetBorderBottom(Text::SpreadSheet::CellStyle::BorderStyle *border)
{
	MemCopyNO(&this->borderBottom, border, sizeof(BorderStyle));
	return this;
}

Text::SpreadSheet::CellStyle *Text::SpreadSheet::CellStyle::SetDataFormat(const UTF8Char *dataFormat)
{
	SDEL_TEXT(this->dataFormat);
	this->dataFormat = SCOPY_TEXT(dataFormat);
	return this;
}

UOSInt Text::SpreadSheet::CellStyle::GetIndex()
{
	return this->index;
}

const UTF8Char *Text::SpreadSheet::CellStyle::GetID()
{
	return this->id;
}

Text::SpreadSheet::HAlignment Text::SpreadSheet::CellStyle::GetHAlign()
{
	return this->halign;
}

Text::SpreadSheet::VAlignment Text::SpreadSheet::CellStyle::GetVAlign()
{
	return this->valign;
}

Bool Text::SpreadSheet::CellStyle::GetWordWrap()
{
	return this->wordWrap;
}

UInt32 Text::SpreadSheet::CellStyle::GetFillColor()
{
	return this->fillColor;
}

Text::SpreadSheet::CellStyle::FillPattern Text::SpreadSheet::CellStyle::GetFillPattern()
{
	return this->fillPattern;
}

Text::SpreadSheet::WorkbookFont *Text::SpreadSheet::CellStyle::GetFont()
{
	return this->font;
}

Text::SpreadSheet::CellStyle::BorderStyle *Text::SpreadSheet::CellStyle::GetBorderLeft()
{
	return &this->borderLeft;
}

Text::SpreadSheet::CellStyle::BorderStyle *Text::SpreadSheet::CellStyle::GetBorderRight()
{
	return &this->borderRight;
}

Text::SpreadSheet::CellStyle::BorderStyle *Text::SpreadSheet::CellStyle::GetBorderTop()
{
	return &this->borderTop;
}

Text::SpreadSheet::CellStyle::BorderStyle *Text::SpreadSheet::CellStyle::GetBorderBottom()
{
	return &this->borderBottom;
}

const UTF8Char *Text::SpreadSheet::CellStyle::GetDataFormat()
{
	return this->dataFormat;
}
