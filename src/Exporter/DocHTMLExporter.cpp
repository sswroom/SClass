#include "Stdafx.h"
#include "MyMemory.h"
#include "Text/Encoding.h"
#include "Text/XML.h"
#include "Text/Doc/TextDocument.h"
#include "Text/Doc/DocText.h"
#include "Text/Doc/DocLink.h"
#include "Text/Doc/DocHeading.h"
#include "IO/StreamWriter.h"
#include "Exporter/DocHTMLExporter.h"

Exporter::DocHTMLExporter::DocHTMLExporter()
{
	this->codePage = 65001;
}

Exporter::DocHTMLExporter::~DocHTMLExporter()
{
}

Int32 Exporter::DocHTMLExporter::GetName()
{
	return *(Int32*)"DOHT";
}

IO::FileExporter::SupportType Exporter::DocHTMLExporter::IsObjectSupported(NN<IO::ParsedObject> pobj)
{
	if (pobj->GetParserType() != IO::ParserType::TextDocument)
	{
		return IO::FileExporter::SupportType::NotSupported;
	}
	return IO::FileExporter::SupportType::NormalStream;
}

Bool Exporter::DocHTMLExporter::GetOutputName(UOSInt index, UTF8Char *nameBuff, UTF8Char *fileNameBuff)
{
	if (index == 0)
	{
		Text::StrConcatC(nameBuff, UTF8STRC("Document HTML File"));
		Text::StrConcatC(fileNameBuff, UTF8STRC("*.html"));
		return true;
	}
	return false;
}

void Exporter::DocHTMLExporter::SetCodePage(UInt32 codePage)
{
	this->codePage = codePage;
}

Bool Exporter::DocHTMLExporter::ExportFile(NN<IO::SeekableStream> stm, Text::CStringNN fileName, NN<IO::ParsedObject> pobj, Optional<ParamData> param)
{
	if (pobj->GetParserType() != IO::ParserType::TextDocument)
	{
		return false;
	}

	NN<Text::Doc::TextDocument> doc = NN<Text::Doc::TextDocument>::ConvertFrom(pobj);
	IO::StreamWriter *writer;
	UTF8Char *lineBuff1;
	UTF8Char *lineBuff2;
	UTF8Char *sptr;
	UInt32 color;

	NEW_CLASS(writer, IO::StreamWriter(stm, this->codePage));

	lineBuff1 = MemAlloc(UTF8Char, 65536);
	lineBuff2 = MemAlloc(UTF8Char, 65536);

	//writer->WriteLine(CSTR("<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.01//EN\" \"http://www.w3.org/TR/html4/strict.dtd\">"));
	//writer->WriteLine(CSTR("<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.01 Transitional//EN\" \"http://www.w3.org/TR/html4/loose.dtd\">"));
	//writer->WriteLine(CSTR("<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.01 Frameset//EN\" \"http://www.w3.org/TR/html4/frameset.dtd\">"));
	//writer->WriteLine(CSTR("<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Strict//EN\" \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd\">"));
	//writer->WriteLine(CSTR("<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Transitional//EN\" \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd\">"));
	//writer->WriteLine(CSTR("<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Frameset//EN\" \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-frameset.dtd\">"));
	writer->WriteLine(CSTR("<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.1//EN\" \"http://www.w3.org/TR/xhtml11/DTD/xhtml11.dtd\">"));
	writer->WriteLine(CSTR("<html xmlns=\"http://www.w3.org/1999/xhtml\">"));
	writer->WriteLine(CSTR("<head>"));
	writer->Write(CSTR("<title>"));
	doc->GetDocumentName(lineBuff1);
	sptr = Text::XML::ToXMLText(lineBuff2, lineBuff1);
	writer->Write(CSTRP(lineBuff2, sptr));
	writer->WriteLine(CSTR("</title>"));
	sptr = Text::StrConcatC(Text::EncodingFactory::GetInternetName(Text::StrConcatC(lineBuff1, UTF8STRC("<meta http-equiv=\"Content-Type\" content=\"text/html; charset=")), this->codePage), UTF8STRC("\" />"));
	writer->WriteLine(CSTRP(lineBuff1, sptr));
	writer->WriteLine(CSTR("<style type=\"text/css\">"));
/*
a:hover {color:#FF00FF;}
*/
	if (doc->GetTextColor(&color))
	{
		writer->Write(CSTR("body {color:"));
		WriteColor(writer, color);
		writer->WriteLine(CSTR(";}"));
	}
	if (doc->GetLinkColor(&color))
	{
		writer->Write(CSTR("a:link {color:"));
		WriteColor(writer, color);
		writer->WriteLine(CSTR(";}"));
	}
	if (doc->GetVisitedLinkColor(&color))
	{
		writer->Write(CSTR("a:visited {color:"));
		WriteColor(writer, color);
		writer->WriteLine(CSTR(";}"));
	}
	if (doc->GetActiveLinkColor(&color))
	{
		writer->Write(CSTR("a:active {color:"));
		WriteColor(writer, color);
		writer->WriteLine(CSTR(";}"));
	}
	if (doc->GetBGColor(&color))
	{
		writer->Write(CSTR("body {background-color:"));
		WriteColor(writer, color);
		writer->WriteLine(CSTR(";}"));
	}
	writer->WriteLine(CSTR("</style>"));

	writer->WriteLine(CSTR("</head>"));
	writer->Write(CSTR("<body>"));

	UOSInt i;
	UOSInt j;
	i = 0;
	j = doc->GetCount();
	while (i < j)
	{
		WriteItems(writer, doc->GetItemNoCheck(i), CSTR("body"));
		i++;
	}

	writer->WriteLine(CSTR("</body></html>"));

	MemFree(lineBuff1);
	MemFree(lineBuff2);
	DEL_CLASS(writer);
	return true;
}

void Exporter::DocHTMLExporter::WriteColor(IO::Writer *writer, UInt32 color)
{
	UTF8Char sbuff[8];
	UTF8Char *sptr;
	sbuff[0] = '#';
	sptr = Text::StrHexByte(Text::StrHexByte(Text::StrHexByte(&sbuff[1], (color >> 16) & 0xff), (color >> 8) & 0xff), color & 0xff);
	writer->Write(CSTRP(sbuff, sptr)); 
}

void Exporter::DocHTMLExporter::WriteItems(IO::Writer *writer, NN<Data::ReadingListNN<Text::Doc::DocItem>> items, Text::CStringNN parentNodeName)
{
	NN<Text::Doc::DocItem> item;
	UOSInt i = 0;
	UOSInt j = items->GetCount();
	NN<Text::String> s;
	Text::Doc::DocItem::HorizontalAlign halign;
	NN<Text::Doc::DocHeading> heading;
	while (i < j)
	{
		item = items->GetItemNoCheck(i);
		switch (item->GetItemType())
		{
		case Text::Doc::DocItem::DIT_URL:
			if (parentNodeName.Equals(CSTR("body")))
			{
				writer->Write(CSTR("<p>"));
			}
			s = Text::XML::ToNewAttrText(NN<Text::Doc::DocLink>::ConvertFrom(item)->GetLink());
			writer->Write(CSTR("<a href="));
			writer->Write(s->ToCString());
			s->Release();
			writer->Write(CSTR(">"));
			WriteItems(writer, item, CSTR("a"));
			writer->Write(CSTR("</a>"));
			if (parentNodeName.Equals(UTF8STRC("body")))
			{
				writer->Write(CSTR("</p>"));
			}
			break;
		case Text::Doc::DocItem::DIT_HEADING:
			heading = NN<Text::Doc::DocHeading>::ConvertFrom(item);
			halign = heading->GetHAlignment();
			writer->Write(CSTR("<h1"));
			switch (halign)
			{
			case Text::Doc::DocItem::HALIGN_CENTER:
				writer->Write(CSTR(" style=\"text-align: center;\""));
				break;
			case Text::Doc::DocItem::HALIGN_NONE:
			default:
				break;
			}
			writer->Write(CSTR(">"));
			WriteItems(writer, heading, CSTR("h1"));
			writer->Write(CSTR("</h1>"));
			break;
		case Text::Doc::DocItem::DIT_TEXT:
			s = Text::XML::ToNewXMLText(NN<Text::Doc::DocText>::ConvertFrom(item)->GetText());
			writer->Write(s->ToCString());
			s->Release();
			break;
		case Text::Doc::DocItem::DIT_VALIDATOR:
			writer->WriteLine(CSTR("<p>"));
			writer->WriteLine(CSTR("\t<a href=\"http://validator.w3.org/check?uri=referer\"><img"));
			writer->WriteLine(CSTR("\tsrc=\"http://www.w3.org/Icons/valid-xhtml11\" alt=\"Valid XHTML 1.1\" height=\"31\" width=\"88\" /></a>"));
			writer->WriteLine(CSTR("</p>"));
			break;
		case Text::Doc::DocItem::DIT_HORICENTER:
			writer->Write(CSTR("<center>"));
			s = Text::XML::ToNewXMLText(NN<Text::Doc::DocText>::ConvertFrom(item)->GetText());
			writer->Write(s->ToCString());
			s->Release();
			writer->Write(CSTR("</center>"));
			break;
		default:
			break;
		}
		i++;
	}
}
