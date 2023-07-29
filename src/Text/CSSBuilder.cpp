#include "Stdafx.h"
#include "Text/CSSBuilder.h"
#include "Text/JSText.h"
#include "Text/MyString.h"
#include "Text/MyStringFloat.h"

Text::CSSBuilder::CSSBuilder(PrettyMode pm)
{
	this->pm = pm;
	this->bstate = BS_ROOT;
}

Text::CSSBuilder::~CSSBuilder()
{
}

Bool Text::CSSBuilder::NewStyle(Text::CString name, Text::CString className)
{
	this->EndStyle();
	if (name.v == 0)
	{
		if (className.v == 0)
		{
			this->sb.AppendC(UTF8STRC("*"));
		}
		else
		{
			this->sb.AppendUTF8Char('.');
			this->sb.Append(className);
		}
	}
	else
	{
		this->sb.Append(name);
		if (className.v)
		{
			this->sb.AppendUTF8Char('.');
			this->sb.Append(className);
		}
	}
	if (this->pm != PM_COMPACT)
	{
		this->sb.AppendUTF8Char(' ');
	}
	this->sb.AppendUTF8Char('{');
	if (this->pm == PM_LINES)
	{
		this->AppendNewLine();
	}
	else if (this->pm == PM_SPACE)
	{
		this->sb.AppendUTF8Char(' ');
	}
	this->bstate = BS_ENTRY_FIRST;
	return true;
}

Bool Text::CSSBuilder::EndStyle()
{
	if (this->bstate == BS_ENTRY || this->bstate == BS_ENTRY_FIRST)
	{
		this->sb.AppendUTF8Char('}');
		if (this->pm == PM_LINES)
		{
			this->AppendNewLine();
		}
		this->bstate = BS_ROOT;
	}
	return true;
}

Bool Text::CSSBuilder::AddColorRGBA(UInt32 argb)
{
	if (this->bstate == BS_ROOT) return false;
	this->AppendStyleName(CSTR("color"));
	this->AppendRGBAColor(argb);
	return true;
}

Bool Text::CSSBuilder::AddColorRGB(UInt32 rgb)
{
	if (this->bstate == BS_ROOT) return false;
	this->AppendStyleName(CSTR("color"));
	this->sb.AppendUTF8Char('#');
	this->sb.AppendHex24(rgb);
	return true;
}

Bool Text::CSSBuilder::AddBGColorRGBA(UInt32 argb)
{
	if (this->bstate == BS_ROOT) return false;
	this->AppendStyleName(CSTR("background-color"));
	this->AppendRGBAColor(argb);
	return true;
}

Bool Text::CSSBuilder::AddFontFamily(const UTF8Char *family)
{
	if (this->bstate == BS_ROOT) return false;
	this->AppendStyleName(CSTR("font-family"));
	NotNullPtr<Text::String> s = Text::JSText::ToNewJSTextDQuote(family);
	this->sb.Append(s);
	s->Release();
	return true;
}

Bool Text::CSSBuilder::AddFontSize(Double size, Math::Unit::Distance::DistanceUnit du)
{
	if (this->bstate == BS_ROOT) return false;
	this->AppendStyleName(CSTR("font-size"));
	this->AppendDistance(size, du);
	return true;
}

Bool Text::CSSBuilder::AddFontWeight(FontWeight weight)
{
	if (this->bstate == BS_ROOT) return false;
	this->AppendStyleName(CSTR("font-weight"));
	switch (weight)
	{
		case FONT_WEIGHT_NORMAL:
			this->sb.AppendC(UTF8STRC("normal"));
			break;
		case FONT_WEIGHT_BOLD:
			this->sb.AppendC(UTF8STRC("bold"));
			break;
		case FONT_WEIGHT_BOLDER:
			this->sb.AppendC(UTF8STRC("bolder"));
			break;
		case FONT_WEIGHT_LIGHTER:
			this->sb.AppendC(UTF8STRC("lighter"));
			break;
		case FONT_WEIGHT_INHERIT:
			this->sb.AppendC(UTF8STRC("inherit"));
			break;
		case FONT_WEIGHT_INITIAL:
			this->sb.AppendC(UTF8STRC("initial"));
			break;
		default:
			this->sb.AppendI32(weight);
			break;
	}
	return true;
}

Bool Text::CSSBuilder::AddMinHeight(Double h, Math::Unit::Distance::DistanceUnit du)
{
	if (this->bstate == BS_ROOT) return false;
	this->AppendStyleName(CSTR("min-height"));
	this->AppendDistance(h, du);
	return true;
}

const UTF8Char *Text::CSSBuilder::ToString()
{
	this->EndStyle();
	return this->sb.ToString();
}

void Text::CSSBuilder::AppendNewLine()
{
	this->sb.AppendC(UTF8STRC("\r\n"));
}

void Text::CSSBuilder::AppendStyleName(Text::CString name)
{
	this->NextEntry();
	if (this->pm == PM_LINES)
	{
		this->sb.AppendUTF8Char('\t');
	}
	this->sb.Append(name);
	if (this->pm == PM_COMPACT)
	{
		this->sb.AppendUTF8Char(':');
	}
	else
	{
		this->sb.AppendC(UTF8STRC(": "));
	}
}

void Text::CSSBuilder::AppendDistance(Double size, Math::Unit::Distance::DistanceUnit du)
{
	if (du == Math::Unit::Distance::DU_PIXEL)
	{
		this->sb.AppendDouble(size);
		this->sb.AppendC(UTF8STRC("px"));
	}
	else if (du == Math::Unit::Distance::DU_POINT)
	{
		this->sb.AppendDouble(size);
		this->sb.AppendC(UTF8STRC("pt"));
	}
	else if (du == Math::Unit::Distance::DU_EMU)
	{
		this->sb.AppendDouble(size);
		this->sb.AppendC(UTF8STRC("em"));
	}
	else
	{
		this->sb.AppendDouble(Math::Unit::Distance::Convert(du, Math::Unit::Distance::DU_PIXEL, size));
		this->sb.AppendC(UTF8STRC("px"));
	}
}

void Text::CSSBuilder::AppendRGBAColor(UInt32 argb)
{
	this->sb.AppendC(UTF8STRC("rgba("));
	this->sb.AppendU32((argb >> 16) & 0xff);
	this->sb.AppendUTF8Char(',');
	this->sb.AppendU32((argb >> 8) & 0xff);
	this->sb.AppendUTF8Char(',');
	this->sb.AppendU32(argb & 0xff);
	this->sb.AppendUTF8Char(',');
	this->sb.AppendDouble((argb >> 24) / 255.0);
	this->sb.AppendC(UTF8STRC(")"));
}

void Text::CSSBuilder::NextEntry()
{
	if (this->bstate == BS_ENTRY_FIRST)
	{
		this->bstate = BS_ENTRY;
	}
	else if (this->pm == PM_COMPACT)
	{
		this->sb.AppendUTF8Char(';');
	}
	else if (this->pm == PM_SPACE)
	{
		this->sb.AppendC(UTF8STRC("; "));
	}
	else if (this->pm == PM_LINES)
	{
		this->sb.AppendC(UTF8STRC(";\r\n"));
	}
}
