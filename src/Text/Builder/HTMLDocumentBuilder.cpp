#include "Stdafx.h"
#include "Text/XML.h"
#include "Text/Builder/HTMLDocumentBuilder.h"

Text::Builder::HTMLDocumentBuilder::HTMLDocumentBuilder(DocType docType, Text::CString title)
{
	Text::CString s = DocTypeGetText(docType);
	if (s.leng > 0)
	{
		sb.Append(s);
		sb.AppendC(UTF8STRC("\r\n"));
	}
	sb.AppendC(UTF8STRC("<html>\r\n"));
	sb.AppendC(UTF8STRC("<head>\r\n"));
	NEW_CLASS(this->headerBuilder, HTMLHeaderBuilder(&this->sb, title));
	this->bodyBuilder = 0;
}

Text::Builder::HTMLDocumentBuilder::~HTMLDocumentBuilder()
{

}

Text::Builder::HTMLBodyBuilder *Text::Builder::HTMLDocumentBuilder::StartBody(Text::CString onLoadScript)
{
	if (this->headerBuilder)
	{
		DEL_CLASS(this->headerBuilder);
		this->sb.AppendC(UTF8STRC("</head>\r\n"));
		this->sb.AppendC(UTF8STRC("<body"));
		if (onLoadScript.leng > 0)
		{
			this->sb.AppendC(UTF8STRC(" onLoad="));
			Text::String *s = Text::XML::ToNewAttrText(onLoadScript.v);
			this->sb.Append(s);
			s->Release();
		}
		this->sb.AppendUTF8Char('>');
		NEW_CLASS(this->bodyBuilder, HTMLBodyBuilder(&this->sb));
	}
	return this->bodyBuilder;
}

Text::CString Text::Builder::HTMLDocumentBuilder::Build()
{
	if (this->headerBuilder)
	{
		this->StartBody(CSTR_NULL);
	}
	if (this->bodyBuilder)
	{
		DEL_CLASS(this->bodyBuilder);
		this->sb.AppendC(UTF8STRC("</body>\r\n"));
		this->sb.AppendC(UTF8STRC("</html>"));
	}
	return this->sb.ToCString();
}

Text::CString Text::Builder::HTMLDocumentBuilder::DocTypeGetText(DocType docType)
{
	switch (docType)
	{
	case DocType::Unknown:
	default:
		return CSTR("");
	case DocType::HTML5:
		return CSTR("<!DOCTYPE html>");
	case DocType::HTML4_01:
		return CSTR("<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.01 Transitional//EN\" \"http://www.w3.org/TR/html4/loose.dtd\">");
	case DocType::XHTML1_1:
		return CSTR("<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.1//EN\" \"http://www.w3.org/TR/xhtml11/DTD/xhtml11.dtd\">");
	}
}
