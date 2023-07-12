#include "Stdafx.h"
#include "Text/XML.h"
#include "Text/Builder/HTMLHeaderBuilder.h"

Text::Builder::HTMLHeaderBuilder::HTMLHeaderBuilder(Text::StringBuilderUTF8 *sb, Text::CString title)
{
	this->sb = sb;
	if (title.leng > 0)
	{
		this->sb->AppendC(UTF8STRC("<title>"));
		Text::String *s = Text::XML::ToNewHTMLElementText(title.v);
		this->sb->Append(s);
		s->Release();
		this->sb->AppendC(UTF8STRC("</title>\r\n"));
	}
}

Text::Builder::HTMLHeaderBuilder::~HTMLHeaderBuilder()
{

}
