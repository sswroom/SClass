#include "Stdafx.h"
#include "Text/CSSBuilder.h"
#include "Text/JSText.h"
#include "Text/MyString.h"
#include "Text/MyStringFloat.h"

Text::CSSBuilder::CSSBuilder(PrettyMode pm)
{
	NEW_CLASS(this->sb, Text::StringBuilderUTF8());
	this->pm = pm;
	this->bstate = BS_ROOT;
}

Text::CSSBuilder::~CSSBuilder()
{
	DEL_CLASS(this->sb);
}

Bool Text::CSSBuilder::NewStyle(const Char *name, const Char *className)
{
	this->EndStyle();
	if (name == 0)
	{
		if (className == 0)
		{
			this->sb->AppendC(UTF8STRC("*"));
		}
		else
		{
			this->sb->AppendChar('.', 1);
			this->sb->AppendSlow((const UTF8Char*)className);
		}
	}
	else
	{
		this->sb->AppendSlow((const UTF8Char*)name);
		if (className)
		{
			this->sb->AppendChar('.', 1);
			this->sb->AppendSlow((const UTF8Char*)className);
		}
	}
	if (this->pm != PM_COMPACT)
	{
		this->sb->AppendChar(' ', 1);
	}
	this->sb->AppendChar('{', 1);
	if (this->pm == PM_LINES)
	{
		this->AppendNewLine();
	}
	else if (this->pm == PM_SPACE)
	{
		this->sb->AppendChar(' ', 1);
	}
	this->bstate = BS_ENTRY_FIRST;
	return true;
}

Bool Text::CSSBuilder::EndStyle()
{
	if (this->bstate == BS_ENTRY || this->bstate == BS_ENTRY_FIRST)
	{
		this->sb->AppendChar('}', 1);
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
	this->AppendStyleName("color");
	this->AppendRGBAColor(argb);
	return true;
}

Bool Text::CSSBuilder::AddColorRGB(UInt32 rgb)
{
	if (this->bstate == BS_ROOT) return false;
	this->AppendStyleName("color");
	this->sb->AppendChar('#', 1);
	this->sb->AppendHex24(rgb);
	return true;
}

Bool Text::CSSBuilder::AddBGColorRGBA(UInt32 argb)
{
	if (this->bstate == BS_ROOT) return false;
	this->AppendStyleName("background-color");
	this->AppendRGBAColor(argb);
	return true;
}

Bool Text::CSSBuilder::AddFontFamily(const UTF8Char *family)
{
	if (this->bstate == BS_ROOT) return false;
	this->AppendStyleName("font-family");
	Text::String *s = Text::JSText::ToNewJSTextDQuote(family);
	this->sb->Append(s);
	s->Release();
	return true;
}

Bool Text::CSSBuilder::AddFontSize(Double size, Math::Unit::Distance::DistanceUnit du)
{
	if (this->bstate == BS_ROOT) return false;
	this->AppendStyleName("font-size");
	if (du == Math::Unit::Distance::DU_PIXEL)
	{
		Text::SBAppendF64(this->sb, size);
		this->sb->AppendC(UTF8STRC("px"));
	}
	else if (du == Math::Unit::Distance::DU_POINT)
	{
		Text::SBAppendF64(this->sb, size);
		this->sb->AppendC(UTF8STRC("pt"));
	}
	else
	{
		Text::SBAppendF64(this->sb, Math::Unit::Distance::Convert(du, Math::Unit::Distance::DU_PIXEL, size));
		this->sb->AppendC(UTF8STRC("px"));
	}
	return true;
}

Bool Text::CSSBuilder::AddFontWeight(FontWeight weight)
{
	if (this->bstate == BS_ROOT) return false;
	this->AppendStyleName("font-weight");
	switch (weight)
	{
		case FONT_WEIGHT_NORMAL:
			this->sb->AppendC(UTF8STRC("normal"));
			break;
		case FONT_WEIGHT_BOLD:
			this->sb->AppendC(UTF8STRC("bold"));
			break;
		case FONT_WEIGHT_BOLDER:
			this->sb->AppendC(UTF8STRC("bolder"));
			break;
		case FONT_WEIGHT_LIGHTER:
			this->sb->AppendC(UTF8STRC("lighter"));
			break;
		case FONT_WEIGHT_INHERIT:
			this->sb->AppendC(UTF8STRC("inherit"));
			break;
		case FONT_WEIGHT_INITIAL:
			this->sb->AppendC(UTF8STRC("initial"));
			break;
		default:
			this->sb->AppendI32(weight);
			break;
	}
	return true;
}

const UTF8Char *Text::CSSBuilder::ToString()
{
	this->EndStyle();
	return this->sb->ToString();
}

void Text::CSSBuilder::AppendNewLine()
{
	this->sb->AppendC(UTF8STRC("\r\n"));
}

void Text::CSSBuilder::AppendStyleName(const Char *name)
{
	this->NextEntry();
	if (this->pm == PM_LINES)
	{
		this->sb->AppendChar('\t', 1);
	}
	this->sb->AppendSlow((const UTF8Char*)name);
	if (this->pm == PM_COMPACT)
	{
		this->sb->AppendChar(':', 1);
	}
	else
	{
		this->sb->AppendC(UTF8STRC(": "));
	}
}

void Text::CSSBuilder::AppendRGBAColor(UInt32 argb)
{
	this->sb->AppendC(UTF8STRC("rgba("));
	this->sb->AppendU32((argb >> 16) & 0xff);
	this->sb->AppendChar(',', 1);
	this->sb->AppendU32((argb >> 8) & 0xff);
	this->sb->AppendChar(',', 1);
	this->sb->AppendU32(argb & 0xff);
	this->sb->AppendChar(',', 1);
	Text::SBAppendF64(this->sb, (argb >> 24) / 255.0);
	this->sb->AppendC(UTF8STRC(")"));
}

void Text::CSSBuilder::NextEntry()
{
	if (this->bstate == BS_ENTRY_FIRST)
	{
		this->bstate = BS_ENTRY;
	}
	else if (this->pm == PM_COMPACT)
	{
		this->sb->AppendChar(';', 1);
	}
	else if (this->pm == PM_SPACE)
	{
		this->sb->AppendC(UTF8STRC("; "));
	}
	else if (this->pm == PM_LINES)
	{
		this->sb->AppendC(UTF8STRC(";\r\n"));
	}
}
