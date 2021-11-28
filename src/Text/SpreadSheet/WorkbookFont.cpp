#include "Stdafx.h"
#include "MyMemory.h"
#include "Text/MyString.h"
#include "Text/SpreadSheet/WorkbookFont.h"

Text::SpreadSheet::WorkbookFont::WorkbookFont()
{
	this->name = 0;
	this->size = 0;
	this->bold = false;
	this->italic = false;
	this->underline = false;
	this->color = 0;
	this->family = FontFamily::NA;
}

Text::SpreadSheet::WorkbookFont::~WorkbookFont()
{
	SDEL_TEXT(this->name);
}

Text::SpreadSheet::WorkbookFont *Text::SpreadSheet::WorkbookFont::SetName(const UTF8Char *name)
{
	SDEL_TEXT(this->name);
	this->name = SCOPY_TEXT(name);
	return this;
}

Text::SpreadSheet::WorkbookFont *Text::SpreadSheet::WorkbookFont::SetSize(Double size)
{
	this->size = size;
	return this;
}

Text::SpreadSheet::WorkbookFont *Text::SpreadSheet::WorkbookFont::SetBold(Bool bold)
{
	this->bold = bold;
	return this;
}

Text::SpreadSheet::WorkbookFont *Text::SpreadSheet::WorkbookFont::SetItalic(Bool italic)
{
	this->italic = italic;
	return this;
}

Text::SpreadSheet::WorkbookFont *Text::SpreadSheet::WorkbookFont::SetUnderline(Bool underline)
{
	this->underline = underline;
	return this;
}

Text::SpreadSheet::WorkbookFont *Text::SpreadSheet::WorkbookFont::SetColor(UInt32 color)
{
	this->color = color;
	return this;
}

Text::SpreadSheet::WorkbookFont *Text::SpreadSheet::WorkbookFont::SetFamily(FontFamily family)
{
	this->family = family;
	return this;
}

const UTF8Char *Text::SpreadSheet::WorkbookFont::GetName()
{
	return this->name;
}

Double Text::SpreadSheet::WorkbookFont::GetSize()
{
	return this->size;
}

Bool Text::SpreadSheet::WorkbookFont::IsBold()
{
	return this->bold;
}

Bool Text::SpreadSheet::WorkbookFont::IsItalic()
{
	return this->italic;
}

Bool Text::SpreadSheet::WorkbookFont::IsUnderline()
{
	return this->underline;
}

UInt32 Text::SpreadSheet::WorkbookFont::GetColor()
{
	return this->color;
}

Text::SpreadSheet::FontFamily Text::SpreadSheet::WorkbookFont::GetFamily()
{
	return this->family;
}

Text::SpreadSheet::WorkbookFont *Text::SpreadSheet::WorkbookFont::Clone()
{
	Text::SpreadSheet::WorkbookFont *font;
	NEW_CLASS(font, Text::SpreadSheet::WorkbookFont());
	font->name = SCOPY_TEXT(this->name);
	font->size = this->size;
	font->bold = this->bold;
	font->italic = this->italic;
	font->underline = this->underline;
	font->color = this->color;
	font->family = this->family;
	return font;
}

Bool Text::SpreadSheet::WorkbookFont::Equals(WorkbookFont *font)
{
	return Text::StrEqualsN(this->name, font->name) &&
		this->size == font->size &&
		this->bold == font->bold &&
		this->italic == font->italic &&
		this->underline == font->underline &&
		this->color == font->color;
}
