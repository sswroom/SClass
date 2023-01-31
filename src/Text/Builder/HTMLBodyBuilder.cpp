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

void Text::Builder::HTMLBodyBuilder::WriteHeading1(Text::CString content)
{
	this->BeginHeading1();
	this->WriteStr(content);
	this->EndElement();
}
