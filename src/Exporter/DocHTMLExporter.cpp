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

IO::FileExporter::SupportType Exporter::DocHTMLExporter::IsObjectSupported(IO::ParsedObject *pobj)
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

Bool Exporter::DocHTMLExporter::ExportFile(IO::SeekableStream *stm, const UTF8Char *fileName, IO::ParsedObject *pobj, void *param)
{
	if (pobj->GetParserType() != IO::ParserType::TextDocument)
	{
		return false;
	}

	Text::Doc::TextDocument *doc = (Text::Doc::TextDocument*)pobj;
	IO::StreamWriter *writer;
	UTF8Char *lineBuff1;
	UTF8Char *lineBuff2;
	UTF8Char *sptr;
	UInt32 color;

	NEW_CLASS(writer, IO::StreamWriter(stm, this->codePage));

	lineBuff1 = MemAlloc(UTF8Char, 65536);
	lineBuff2 = MemAlloc(UTF8Char, 65536);

	//writer->WriteLineC(UTF8STRC("<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.01//EN\" \"http://www.w3.org/TR/html4/strict.dtd\">"));
	//writer->WriteLineC(UTF8STRC("<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.01 Transitional//EN\" \"http://www.w3.org/TR/html4/loose.dtd\">"));
	//writer->WriteLineC(UTF8STRC("<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.01 Frameset//EN\" \"http://www.w3.org/TR/html4/frameset.dtd\">"));
	//writer->WriteLineC(UTF8STRC("<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Strict//EN\" \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd\">"));
	//writer->WriteLineC(UTF8STRC("<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Transitional//EN\" \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd\">"));
	//writer->WriteLineC(UTF8STRC("<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Frameset//EN\" \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-frameset.dtd\">"));
	writer->WriteLineC(UTF8STRC("<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.1//EN\" \"http://www.w3.org/TR/xhtml11/DTD/xhtml11.dtd\">"));
	writer->WriteLineC(UTF8STRC("<html xmlns=\"http://www.w3.org/1999/xhtml\">"));
	writer->WriteLineC(UTF8STRC("<head>"));
	writer->WriteStrC(UTF8STRC("<title>"));
	doc->GetDocumentName(lineBuff1);
	sptr = Text::XML::ToXMLText(lineBuff2, lineBuff1);
	writer->WriteStrC(lineBuff2, (UOSInt)(sptr - lineBuff2));
	writer->WriteLineC(UTF8STRC("</title>"));
	sptr = Text::StrConcatC(Text::EncodingFactory::GetInternetName(Text::StrConcatC(lineBuff1, UTF8STRC("<meta http-equiv=\"Content-Type\" content=\"text/html; charset=")), this->codePage), UTF8STRC("\" />"));
	writer->WriteLineC(lineBuff1, (UOSInt)(sptr - lineBuff1));
	writer->WriteLineC(UTF8STRC("<style type=\"text/css\">"));
/*
a:hover {color:#FF00FF;}
*/
	if (doc->GetTextColor(&color))
	{
		writer->WriteStrC(UTF8STRC("body {color:"));
		WriteColor(writer, color);
		writer->WriteLineC(UTF8STRC(";}"));
	}
	if (doc->GetLinkColor(&color))
	{
		writer->WriteStrC(UTF8STRC("a:link {color:"));
		WriteColor(writer, color);
		writer->WriteLineC(UTF8STRC(";}"));
	}
	if (doc->GetVisitedLinkColor(&color))
	{
		writer->WriteStrC(UTF8STRC("a:visited {color:"));
		WriteColor(writer, color);
		writer->WriteLineC(UTF8STRC(";}"));
	}
	if (doc->GetActiveLinkColor(&color))
	{
		writer->WriteStrC(UTF8STRC("a:active {color:"));
		WriteColor(writer, color);
		writer->WriteLineC(UTF8STRC(";}"));
	}
	if (doc->GetBGColor(&color))
	{
		writer->WriteStrC(UTF8STRC("body {background-color:"));
		WriteColor(writer, color);
		writer->WriteLineC(UTF8STRC(";}"));
	}
	writer->WriteLineC(UTF8STRC("</style>"));

	writer->WriteLineC(UTF8STRC("</head>"));
	writer->WriteStrC(UTF8STRC("<body>"));

	UOSInt i;
	UOSInt j;
	i = 0;
	j = doc->GetCount();
	while (i < j)
	{
		WriteItems(writer, doc->GetItem(i), UTF8STRC("body"));
		i++;
	}

	writer->WriteLineC(UTF8STRC("</body></html>"));

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
	writer->WriteStrC(sbuff, (UOSInt)(sptr - sbuff)); 
}

void Exporter::DocHTMLExporter::WriteItems(IO::Writer *writer, Data::ReadingList<Text::Doc::DocItem *> *items, const UTF8Char *parentNodeName, UOSInt nameLen)
{
	Text::Doc::DocItem *item;
	UOSInt i = 0;
	UOSInt j = items->GetCount();
	Text::String *s;
	Text::Doc::DocItem::HorizontalAlign halign;
	Text::Doc::DocHeading *heading;
	while (i < j)
	{
		item = items->GetItem(i);
		switch (item->GetItemType())
		{
		case Text::Doc::DocItem::DIT_URL:
			if (Text::StrEqualsC(parentNodeName, nameLen, UTF8STRC("body")))
			{
				writer->WriteStrC(UTF8STRC("<p>"));
			}
			s = Text::XML::ToNewAttrText(((Text::Doc::DocLink*)item)->GetLink());
			writer->WriteStrC(UTF8STRC("<a href="));
			writer->WriteStrC(s->v, s->leng);
			s->Release();
			writer->WriteStrC(UTF8STRC(">"));
			WriteItems(writer, item, UTF8STRC("a"));
			writer->WriteStrC(UTF8STRC("</a>"));
			if (Text::StrEqualsC(parentNodeName, nameLen, UTF8STRC("body")))
			{
				writer->WriteStrC(UTF8STRC("</p>"));
			}
			break;
		case Text::Doc::DocItem::DIT_HEADING:
			heading = (Text::Doc::DocHeading*)item;
			halign = heading->GetHAlignment();
			writer->WriteStrC(UTF8STRC("<h1"));
			switch (halign)
			{
			case Text::Doc::DocItem::HALIGN_CENTER:
				writer->WriteStrC(UTF8STRC(" style=\"text-align: center;\""));
				break;
			case Text::Doc::DocItem::HALIGN_NONE:
			default:
				break;
			}
			writer->WriteStrC(UTF8STRC(">"));
			WriteItems(writer, heading, UTF8STRC("h1"));
			writer->WriteStrC(UTF8STRC("</h1>"));
			break;
		case Text::Doc::DocItem::DIT_TEXT:
			s = Text::XML::ToNewXMLText(((Text::Doc::DocText*)item)->GetText());
			writer->WriteStrC(s->v, s->leng);
			s->Release();
			break;
		case Text::Doc::DocItem::DIT_VALIDATOR:
			writer->WriteLineC(UTF8STRC("<p>"));
			writer->WriteLineC(UTF8STRC("\t<a href=\"http://validator.w3.org/check?uri=referer\"><img"));
			writer->WriteLineC(UTF8STRC("\tsrc=\"http://www.w3.org/Icons/valid-xhtml11\" alt=\"Valid XHTML 1.1\" height=\"31\" width=\"88\" /></a>"));
			writer->WriteLineC(UTF8STRC("</p>"));
			break;
		case Text::Doc::DocItem::DIT_HORICENTER:
			writer->WriteStrC(UTF8STRC("<center>"));
			s = Text::XML::ToNewXMLText(((Text::Doc::DocText*)item)->GetText());
			writer->WriteStrC(s->v, s->leng);
			s->Release();
			writer->WriteStrC(UTF8STRC("</center>"));
			break;
		default:
			break;
		}
		i++;
	}
}
