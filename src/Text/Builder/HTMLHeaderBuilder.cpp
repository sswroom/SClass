#include "Stdafx.h"
#include "Text/XML.h"
#include "Text/Builder/HTMLHeaderBuilder.h"

Text::Builder::HTMLHeaderBuilder::HTMLHeaderBuilder(NN<Text::StringBuilderUTF8> sb, Text::CString title)
{
	this->sb = sb;
	Text::CStringNN nntitle;
	if (title.SetTo(nntitle) && nntitle.leng > 0)
	{
		this->sb->AppendC(UTF8STRC("<title>"));
		NN<Text::String> s = Text::XML::ToNewHTMLElementText(nntitle.v);
		this->sb->Append(s);
		s->Release();
		this->sb->AppendC(UTF8STRC("</title>\r\n"));
	}
}

Text::Builder::HTMLHeaderBuilder::~HTMLHeaderBuilder()
{

}
