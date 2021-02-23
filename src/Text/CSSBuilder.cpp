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
			this->sb->Append((const UTF8Char*)"*");
		}
		else
		{
			this->sb->AppendChar('.', 1);
			this->sb->Append((const UTF8Char*)className);
		}
	}
	else
	{
		this->sb->Append((const UTF8Char*)name);
		if (className)
		{
			this->sb->AppendChar('.', 1);
			this->sb->Append((const UTF8Char*)className);
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
	const UTF8Char *txt = Text::JSText::ToNewJSTextDQuote(family);
	this->sb->Append(txt);
	Text::JSText::FreeNewText(txt);
	return true;
}

Bool Text::CSSBuilder::AddFontSize(Double size, Math::Unit::Distance::DistanceUnit du)
{
	if (this->bstate == BS_ROOT) return false;
	this->AppendStyleName("font-size");
	if (du == Math::Unit::Distance::DU_PIXEL)
	{
		Text::SBAppendF64(this->sb, size);
		this->sb->Append((const UTF8Char*)"px");
	}
	else if (du == Math::Unit::Distance::DU_POINT)
	{
		Text::SBAppendF64(this->sb, size);
		this->sb->Append((const UTF8Char*)"pt");
	}
	else
	{
		Text::SBAppendF64(this->sb, Math::Unit::Distance::Convert(du, Math::Unit::Distance::DU_PIXEL, size));
		this->sb->Append((const UTF8Char*)"px");
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
			this->sb->Append((const UTF8Char*)"normal");
			break;
		case FONT_WEIGHT_BOLD:
			this->sb->Append((const UTF8Char*)"bold");
			break;
		case FONT_WEIGHT_BOLDER:
			this->sb->Append((const UTF8Char*)"bolder");
			break;
		case FONT_WEIGHT_LIGHTER:
			this->sb->Append((const UTF8Char*)"lighter");
			break;
		case FONT_WEIGHT_INHERIT:
			this->sb->Append((const UTF8Char*)"inherit");
			break;
		case FONT_WEIGHT_INITIAL:
			this->sb->Append((const UTF8Char*)"initial");
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
	this->sb->Append((const UTF8Char*)"\r\n");
}

void Text::CSSBuilder::AppendStyleName(const Char *name)
{
	this->NextEntry();
	if (this->pm == PM_LINES)
	{
		this->sb->AppendChar('\t', 1);
	}
	this->sb->Append((const UTF8Char*)name);
	if (this->pm == PM_COMPACT)
	{
		this->sb->AppendChar(':', 1);
	}
	else
	{
		this->sb->Append((const UTF8Char*)": ");
	}
}

void Text::CSSBuilder::AppendRGBAColor(UInt32 argb)
{
	this->sb->Append((const UTF8Char*)"rgba(");
	this->sb->AppendU32((argb >> 16) & 0xff);
	this->sb->AppendChar(',', 1);
	this->sb->AppendU32((argb >> 8) & 0xff);
	this->sb->AppendChar(',', 1);
	this->sb->AppendU32(argb & 0xff);
	this->sb->AppendChar(',', 1);
	Text::SBAppendF64(this->sb, (argb >> 24) / 255.0);
	this->sb->Append((const UTF8Char*)")");	
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
		this->sb->Append((const UTF8Char*)"; ");
	}
	else if (this->pm == PM_LINES)
	{
		this->sb->Append((const UTF8Char*)";\r\n");
	}
}
