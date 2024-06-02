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

Int32 MyMain(NN<Core::IProgControl> progCtrl)
{
	NN<Text::Doc::DocValidator> validator;
	Text::CStringNN fileName;
	UTF8Char sbuff[512];
	UnsafeArray<UTF8Char> sptr;

	fileName = CSTR("test.html");
	sptr = IO::Path::GetProcessFileName(sbuff).Or(sbuff);
	sptr = IO::Path::AppendPath(sbuff, sptr, fileName);

	Exporter::DocHTMLExporter exporter;
	Text::Doc::TextDocument doc;
	doc.SetDocumentName(CSTR("sswroom's test"));
	doc.SetTextColor(0xffc0e0ff);
	doc.SetLinkColor(0xff6080ff);
	doc.SetVisitedLinkColor(0xff4060ff);
	doc.SetActiveLinkColor(0xff4040ff);
	doc.SetBGColor(0xff000000);
	
	NN<Text::Doc::DocHeading> heading;
	NN<Text::Doc::DocText> txt;
	NN<Text::Doc::DocLink> link;
	NN<Text::Doc::DocSection> section;
	NEW_CLASSNN(section, Text::Doc::DocSection(Media::PaperSize::PT_A4, false));
	doc.Add(section);

	NEW_CLASSNN(heading, Text::Doc::DocHeading());
	NEW_CLASSNN(txt, Text::Doc::DocText((const UTF8Char*)"sswroom's test"));
	heading->SetHAlignment(Text::Doc::DocItem::HALIGN_CENTER);
	heading->Add(txt);
	section->Add(heading);
	NEW_CLASSNN(link, Text::Doc::DocLink((const UTF8Char*)"index1.html"));
	NEW_CLASSNN(txt, Text::Doc::DocText((const UTF8Char*)"test Eukaryota (5959)"));
	link->Add(txt);
	section->Add(link);
	NEW_CLASSNN(validator, Text::Doc::DocValidator());
	section->Add(validator);

	{
		IO::FileStream fs(CSTRP(sbuff, sptr), IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::NoWriteBuffer);
		exporter.ExportFile(fs, CSTRP(sbuff, sptr), doc, 0);
	}
	return 0;
}
