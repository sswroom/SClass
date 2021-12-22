#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/Core.h"
#include "Exporter/DocHTMLExporter.h"
#include "IO/FileStream.h"
#include "IO/Path.h"
#include "Text/Doc/DocHeading.h"
#include "Text/Doc/DocLink.h"
#include "Text/Doc/DocText.h"
#include "Text/Doc/DocValidator.h"
#include "Text/Doc/TextDocument.h"

Int32 MyMain(Core::IProgControl *progCtrl)
{
	Text::Doc::TextDocument *doc;
	Text::Doc::DocValidator *validator;
	Exporter::DocHTMLExporter *exporter;
	IO::FileStream *fs;
	const UTF8Char *fileName;
	UTF8Char sbuff[512];

	fileName = (const UTF8Char*)"test.html";
	IO::Path::GetProcessFileName(sbuff);
	IO::Path::AppendPath(sbuff, fileName);

	NEW_CLASS(exporter, Exporter::DocHTMLExporter());
	NEW_CLASS(doc, Text::Doc::TextDocument());
	doc->SetDocumentName((const UTF8Char*)"sswroom's test");
	doc->SetTextColor(0xffc0e0ff);
	doc->SetLinkColor(0xff6080ff);
	doc->SetVisitedLinkColor(0xff4060ff);
	doc->SetActiveLinkColor(0xff4040ff);
	doc->SetBGColor(0xff000000);
	
	Text::Doc::DocHeading *heading;
	Text::Doc::DocText *txt;
	Text::Doc::DocLink *link;
	Text::Doc::DocSection *section;
	NEW_CLASS(section, Text::Doc::DocSection(Media::PaperSize::PT_A4, false));
	doc->Add(section);

	NEW_CLASS(heading, Text::Doc::DocHeading());
	NEW_CLASS(txt, Text::Doc::DocText((const UTF8Char*)"sswroom's test"));
	heading->SetHAlignment(Text::Doc::DocItem::HALIGN_CENTER);
	heading->Add(txt);
	section->Add(heading);
	NEW_CLASS(link, Text::Doc::DocLink((const UTF8Char*)"index1.html"));
	NEW_CLASS(txt, Text::Doc::DocText((const UTF8Char*)"test Eukaryota (5959)"));
	link->Add(txt);
	section->Add(link);
	NEW_CLASS(validator, Text::Doc::DocValidator());
	section->Add(validator);

	NEW_CLASS(fs, IO::FileStream(sbuff, IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::NoWriteBuffer));
	exporter->ExportFile(fs, sbuff, doc, 0);
	DEL_CLASS(fs);

	DEL_CLASS(doc);
	DEL_CLASS(exporter);
	return 0;
}
