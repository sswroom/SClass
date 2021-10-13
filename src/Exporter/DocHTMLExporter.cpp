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
	if (pobj->GetParserType() != IO::ParsedObject::PT_TEXT_DOCUMENT)
	{
		return IO::FileExporter::SupportType::NotSupported;
	}
	return IO::FileExporter::SupportType::NormalStream;
}

Bool Exporter::DocHTMLExporter::GetOutputName(UOSInt index, UTF8Char *nameBuff, UTF8Char *fileNameBuff)
{
	if (index == 0)
	{
		Text::StrConcat(nameBuff, (const UTF8Char*)"Document HTML File");
		Text::StrConcat(fileNameBuff, (const UTF8Char*)"*.html");
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
	if (pobj->GetParserType() != IO::ParsedObject::PT_TEXT_DOCUMENT)
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

	//writer->WriteLine((const UTF8Char*)"<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.01//EN\" \"http://www.w3.org/TR/html4/strict.dtd\">");
	//writer->WriteLine((const UTF8Char*)"<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.01 Transitional//EN\" \"http://www.w3.org/TR/html4/loose.dtd\">");
	//writer->WriteLine((const UTF8Char*)"<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.01 Frameset//EN\" \"http://www.w3.org/TR/html4/frameset.dtd\">");
	//writer->WriteLine((const UTF8Char*)"<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Strict//EN\" \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd\">");
	//writer->WriteLine((const UTF8Char*)"<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Transitional//EN\" \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd\">");
	//writer->WriteLine((const UTF8Char*)"<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Frameset//EN\" \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-frameset.dtd\">");
	writer->WriteLine((const UTF8Char*)"<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.1//EN\" \"http://www.w3.org/TR/xhtml11/DTD/xhtml11.dtd\">");
	writer->WriteLine((const UTF8Char*)"<html xmlns=\"http://www.w3.org/1999/xhtml\">");
	writer->WriteLine((const UTF8Char*)"<head>");
	writer->Write((const UTF8Char*)"<title>");
	doc->GetDocumentName(lineBuff1);
	Text::XML::ToXMLText(lineBuff2, lineBuff1);
	writer->Write(lineBuff2);
	writer->WriteLine((const UTF8Char*)"</title>");
	sptr = Text::StrConcat(Text::EncodingFactory::GetInternetName(Text::StrConcat(lineBuff1, (const UTF8Char*)"<meta http-equiv=\"Content-Type\" content=\"text/html; charset="), this->codePage), (const UTF8Char*)"\" />");
	writer->WriteLine(lineBuff1, (UOSInt)(sptr - lineBuff1));
	writer->WriteLine((const UTF8Char*)"<style type=\"text/css\">");
/*
a:hover {color:#FF00FF;}
*/
	if (doc->GetTextColor(&color))
	{
		writer->Write((const UTF8Char*)"body {color:");
		WriteColor(writer, color);
		writer->WriteLine((const UTF8Char*)";}");
	}
	if (doc->GetLinkColor(&color))
	{
		writer->Write((const UTF8Char*)"a:link {color:");
		WriteColor(writer, color);
		writer->WriteLine((const UTF8Char*)";}");
	}
	if (doc->GetVisitedLinkColor(&color))
	{
		writer->Write((const UTF8Char*)"a:visited {color:");
		WriteColor(writer, color);
		writer->WriteLine((const UTF8Char*)";}");
	}
	if (doc->GetActiveLinkColor(&color))
	{
		writer->Write((const UTF8Char*)"a:active {color:");
		WriteColor(writer, color);
		writer->WriteLine((const UTF8Char*)";}");
	}
	if (doc->GetBGColor(&color))
	{
		writer->Write((const UTF8Char*)"body {background-color:");
		WriteColor(writer, color);
		writer->WriteLine((const UTF8Char*)";}");
	}
	writer->WriteLine((const UTF8Char*)"</style>");

	writer->WriteLine((const UTF8Char*)"</head>");
	writer->Write((const UTF8Char*)"<body>");

	UOSInt i;
	UOSInt j;
	i = 0;
	j = doc->GetCount();
	while (i < j)
	{
		WriteItems(writer, doc->GetItem(i), (const UTF8Char*)"body");
		i++;
	}

	writer->WriteLine((const UTF8Char*)"</body></html>");

	MemFree(lineBuff1);
	MemFree(lineBuff2);
	DEL_CLASS(writer);
	return true;
}

void Exporter::DocHTMLExporter::WriteColor(IO::Writer *writer, UInt32 color)
{
	UTF8Char sbuff[8];
	sbuff[0] = '#';
	Text::StrHexByte(Text::StrHexByte(Text::StrHexByte(&sbuff[1], (color >> 16) & 0xff), (color >> 8) & 0xff), color & 0xff);
	writer->Write(sbuff); 
}

void Exporter::DocHTMLExporter::WriteItems(IO::Writer *writer, Data::ReadingList<Text::Doc::DocItem *> *items, const UTF8Char *parentNodeName)
{
	Text::Doc::DocItem *item;
	UOSInt i = 0;
	UOSInt j = items->GetCount();
	const UTF8Char *text;
	Text::Doc::DocItem::HorizontalAlign halign;
	Text::Doc::DocHeading *heading;
	while (i < j)
	{
		item = items->GetItem(i);
		switch (item->GetItemType())
		{
		case Text::Doc::DocItem::DIT_URL:
			if (Text::StrCompare(parentNodeName, (const UTF8Char*)"body") == 0)
			{
				writer->Write((const UTF8Char*)"<p>");
			}
			text = Text::XML::ToNewAttrText(((Text::Doc::DocLink*)item)->GetLink());
			writer->Write((const UTF8Char*)"<a href=");
			writer->Write(text);
			Text::XML::FreeNewText(text);
			writer->Write((const UTF8Char*)">");
			WriteItems(writer, item, (const UTF8Char*)"a");
			writer->Write((const UTF8Char*)"</a>");
			if (Text::StrCompare(parentNodeName, (const UTF8Char*)"body") == 0)
			{
				writer->Write((const UTF8Char*)"</p>");
			}
			break;
		case Text::Doc::DocItem::DIT_HEADING:
			heading = (Text::Doc::DocHeading*)item;
			halign = heading->GetHAlignment();
			writer->Write((const UTF8Char*)"<h1");
			switch (halign)
			{
			case Text::Doc::DocItem::HALIGN_CENTER:
				writer->Write((const UTF8Char*)" style=\"text-align: center;\"");
				break;
			case Text::Doc::DocItem::HALIGN_NONE:
			default:
				break;
			}
			writer->Write((const UTF8Char*)">");
			WriteItems(writer, heading, (const UTF8Char*)"h1");
			writer->Write((const UTF8Char*)"</h1>");
			break;
		case Text::Doc::DocItem::DIT_TEXT:
			text = Text::XML::ToNewXMLText(((Text::Doc::DocText*)item)->GetText());
			writer->Write(text);
			Text::XML::FreeNewText(text);
			break;
		case Text::Doc::DocItem::DIT_VALIDATOR:
			writer->WriteLine((const UTF8Char*)"<p>");
			writer->WriteLine((const UTF8Char*)"\t<a href=\"http://validator.w3.org/check?uri=referer\"><img");
			writer->WriteLine((const UTF8Char*)"\tsrc=\"http://www.w3.org/Icons/valid-xhtml11\" alt=\"Valid XHTML 1.1\" height=\"31\" width=\"88\" /></a>");
			writer->WriteLine((const UTF8Char*)"</p>");
			break;
		case Text::Doc::DocItem::DIT_HORICENTER:
			writer->Write((const UTF8Char*)"<center>");
			text = Text::XML::ToNewXMLText(((Text::Doc::DocText*)item)->GetText());
			writer->Write(text);
			Text::XML::FreeNewText(text);
			writer->Write((const UTF8Char*)"</center>");
			break;
		default:
			break;
		}
		i++;
	}
}
