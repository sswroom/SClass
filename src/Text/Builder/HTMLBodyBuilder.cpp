#include "Stdafx.h"
#include "Text/XML.h"
#include "Text/Builder/HTMLBodyBuilder.h"

Text::Builder::HTMLBodyBuilder::HTMLBodyBuilder(Text::StringBuilderUTF8 *sb) : HTMLBuilder(sb, true)
{
}

Text::Builder::HTMLBodyBuilder::~HTMLBodyBuilder()
{
	UOSInt i = this->elements.GetCount();
	while (i-- > 0)
	{
		this->sb->AppendC(UTF8STRC("</"));
		this->sb->Append(this->elements.RemoveAt(i));
		this->sb->AppendUTF8Char('>');
	}	
}

void Text::Builder::HTMLBodyBuilder::BeginLink(Text::CString url)
{
	this->sb->AppendC(UTF8STRC("<a href="));
	Text::String *s = Text::XML::ToNewAttrText(url.v);
	this->sb->Append(s);
	s->Release();
	sb->AppendUTF8Char('>');
	this->elements.Add(CSTR("a"));
}

void Text::Builder::HTMLBodyBuilder::BeginHeading1()
{
	this->sb->AppendC(UTF8STRC("<h1>"));
	this->elements.Add(CSTR("h1"));
}

void Text::Builder::HTMLBodyBuilder::BeginHeading2()
{
	this->sb->AppendC(UTF8STRC("<h2>"));
	this->elements.Add(CSTR("h2"));
}

void Text::Builder::HTMLBodyBuilder::BeginHeading3()
{
	this->sb->AppendC(UTF8STRC("<h3>"));
	this->elements.Add(CSTR("h3"));
}

void Text::Builder::HTMLBodyBuilder::BeginHeading4()
{
	this->sb->AppendC(UTF8STRC("<h4>"));
	this->elements.Add(CSTR("h4"));
}

void Text::Builder::HTMLBodyBuilder::BeginTable()
{
	this->sb->AppendC(UTF8STRC("<table>"));
	this->elements.Add(CSTR("table"));
}

void Text::Builder::HTMLBodyBuilder::BeginTableRow()
{
	this->sb->AppendC(UTF8STRC("<tr>"));
	this->elements.Add(CSTR("tr"));
}

void Text::Builder::HTMLBodyBuilder::BeginTableRowPixelHeight(UOSInt pxHeight)
{
	this->sb->AppendC(UTF8STRC("<tr height=\""));
	this->sb->AppendUOSInt(pxHeight);
	this->sb->AppendC(UTF8STRC("px\">"));
	this->elements.Add(CSTR("tr"));
}

void Text::Builder::HTMLBodyBuilder::EndElement()
{
	UOSInt i = this->elements.GetCount();
	if (i-- > 0)
	{
		this->sb->AppendC(UTF8STRC("</"));
		this->sb->Append(this->elements.RemoveAt(i));
		this->sb->AppendUTF8Char('>');
	}	
}

void Text::Builder::HTMLBodyBuilder::AddTableHeader(Text::CString content)
{
	this->sb->AppendC(UTF8STRC("<th>"));
	this->WriteStr(content);
	this->sb->AppendC(UTF8STRC("</th>"));
}

void Text::Builder::HTMLBodyBuilder::AddTableData(Text::CString content)
{
	this->sb->AppendC(UTF8STRC("<td>"));
	this->WriteStr(content);
	this->sb->AppendC(UTF8STRC("</td>"));
}

void Text::Builder::HTMLBodyBuilder::AddTableData(Text::CString content, UOSInt colSpan, UOSInt rowSpan, HAlignment halign, VAlignment valign)
{
	this->sb->AppendC(UTF8STRC("<td"));
	if (colSpan > 1)
	{
		this->sb->AppendC(UTF8STRC(" colspan=\""));
		this->sb->AppendUOSInt(colSpan);
		this->sb->AppendUTF8Char('\"');
	}
	if (rowSpan > 1)
	{
		this->sb->AppendC(UTF8STRC(" rowspan=\""));
		this->sb->AppendUOSInt(rowSpan);
		this->sb->AppendUTF8Char('\"');
	}
	switch (halign)
	{
	case HAlignment::Unknown:
	case HAlignment::Fill:
	default:
		break;
	case HAlignment::Left:
		this->sb->AppendC(UTF8STRC(" align=\"left\""));
		break;
	case HAlignment::Center:
		this->sb->AppendC(UTF8STRC(" align=\"center\""));
		break;
	case HAlignment::Right:
		this->sb->AppendC(UTF8STRC(" align=\"right\""));
		break;
	case HAlignment::Justify:
		this->sb->AppendC(UTF8STRC(" align=\"justify\""));
		break;
	}
	switch (valign)
	{
	default:
	case VAlignment::Unknown:
	case VAlignment::Justify:
		break;
	case VAlignment::Top:
		this->sb->AppendC(UTF8STRC(" valign=\"top\""));
		break;
	case VAlignment::Center:
		this->sb->AppendC(UTF8STRC(" valign=\"middle\""));
		break;
	case VAlignment::Bottom:
		this->sb->AppendC(UTF8STRC(" valign=\"bottom\""));
		break;
	}
	this->sb->AppendUTF8Char('>');
	this->WriteStr(content);
	this->sb->AppendC(UTF8STRC("</td>"));
}

void Text::Builder::HTMLBodyBuilder::WriteHeading1(Text::CString content)
{
	this->BeginHeading1();
	this->WriteStr(content);
	this->EndElement();
}
