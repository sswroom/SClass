#include "Stdafx.h"
#include "Text/Builder/HTMLBuilder.h"

Text::Builder::HTMLBuilder::HTMLBuilder(NN<Text::StringBuilderUTF8> sb, Bool bodyContent)
{
	this->sb = sb;
	this->bodyContent = bodyContent;
	this->fontStarted = false;
}

Text::Builder::HTMLBuilder::~HTMLBuilder()
{
	this->ResetTextColor();
}

Bool Text::Builder::HTMLBuilder::WriteChar(UTF8Char c)
{
	switch (c)
	{
	case 0:
		return false;
	case '&':
		this->sb->AppendC(UTF8STRC("&#38;"));
		return true;
	case '<':
		this->sb->AppendC(UTF8STRC("&lt;"));
		return true;
	case '>':
		this->sb->AppendC(UTF8STRC("&gt;"));
		return true;
	case '\'':
		this->sb->AppendC(UTF8STRC("&#39;"));
		return true;
	case '"':
		this->sb->AppendC(UTF8STRC("&quot;"));
		return true;
	case '\t':
		this->sb->AppendC(UTF8STRC("&nbsp;&nbsp;&nbsp;&nbsp;"));
		return true;
	case '\r':
		if (this->bodyContent)
		{
			return true;
		}
	case '\n':
		if (this->bodyContent)
		{
			this->sb->AppendC(UTF8STRC("<br/>"));
			return true;
		}
	default:
		this->sb->AppendUTF8Char(c);
		return true;
	}
}

void Text::Builder::HTMLBuilder::SetTextColor(StandardColor fgColor)
{
	this->ResetTextColor();
	this->sb->AppendC(UTF8STRC("<font color=\""));
	this->sb->Append(StandardColorGetName(fgColor));
	this->sb->AppendC(UTF8STRC("\">"));
	this->fontStarted = true;
}

void Text::Builder::HTMLBuilder::ResetTextColor()
{
	if (this->fontStarted)
	{
		this->sb->AppendC(UTF8STRC("</font>"));
		this->fontStarted = false;
	}
}
