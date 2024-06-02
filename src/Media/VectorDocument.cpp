#include "Stdafx.h"
#include "MyMemory.h"
#include "Media/VectorDocument.h"
#include "Text/MyString.h"

Media::VectorDocument::VectorDocument(UInt32 srid, NN<Media::DrawEngine> refEng) : IO::ParsedObject(CSTR("Untitled"))
{
	NEW_CLASS(this->items, Data::ArrayList<Media::VectorGraph*>());
	this->currDoc = 0;
	this->srid = srid;
	this->refEng = refEng;
	this->docName = 0;
	this->author = 0;
	this->subject = 0;
	this->keywords = 0;
	this->creator = 0;
	this->producer = 0;
	this->createTimeTicks = 0;
	this->modTimeTicks = 0;
}

Media::VectorDocument::VectorDocument(UInt32 srid, Text::CStringNN name, NN<Media::DrawEngine> refEng) : IO::ParsedObject(name)
{
	NEW_CLASS(this->items, Data::ArrayList<Media::VectorGraph*>());
	this->currDoc = 0;
	this->srid = srid;
	this->refEng = refEng;
	this->docName = Text::String::New(name).Ptr();
	this->author = 0;
	this->subject = 0;
	this->keywords = 0;
	this->creator = 0;
	this->producer = 0;
	this->createTimeTicks = 0;
	this->modTimeTicks = 0;
}

Media::VectorDocument::~VectorDocument()
{
	UOSInt i;
	Media::VectorGraph *graph;
	i = this->items->GetCount();
	while (i-- > 0)
	{
		graph = this->items->GetItem(i);
		DEL_CLASS(graph);
	}
	DEL_CLASS(this->items);
	SDEL_STRING(this->docName);
	SDEL_TEXT(this->author);
	SDEL_TEXT(this->subject);
	SDEL_TEXT(this->keywords);
	SDEL_TEXT(this->creator);
	SDEL_TEXT(this->producer);
}

NN<Media::VectorGraph> Media::VectorDocument::AddGraph(Double width, Double height, Math::Unit::Distance::DistanceUnit unit)
{
	NN<Media::VectorGraph> graph;
	Media::ColorProfile color(Media::ColorProfile::CPT_SRGB);
	NEW_CLASSNN(graph, Media::VectorGraph(this->srid, width, height, unit, this->refEng, color));
	this->items->Add(graph.Ptr());
	return graph;
}

void Media::VectorDocument::SetDocName(Text::CString docName)
{
	Text::CStringNN nndocName;
	if (docName.SetTo(nndocName))
	{
		SDEL_STRING(this->docName);
		this->docName = Text::String::New(nndocName).Ptr();
	}
}

Text::String *Media::VectorDocument::GetDocName() const
{
	return this->docName;
}

void Media::VectorDocument::SetCreateTime(Int64 createTimeTicks)
{
	this->createTimeTicks = createTimeTicks;
}

Int64 Media::VectorDocument::GetCreateTime() const
{
	return this->createTimeTicks;
}

void Media::VectorDocument::SetModifyTime(Int64 modTimeTicks)
{
	this->modTimeTicks = modTimeTicks;
}

Int64 Media::VectorDocument::GetModifyTime() const
{
	return this->modTimeTicks;
}

void Media::VectorDocument::SetAuthor(UnsafeArrayOpt<const UTF8Char> author)
{
	SDEL_TEXT(this->author);
	UnsafeArray<const UTF8Char> nns;
	if (author.SetTo(nns))
	{
		this->author = Text::StrCopyNew(nns);
	}
}

UnsafeArrayOpt<const UTF8Char> Media::VectorDocument::GetAuthor() const
{
	return this->author;
}

void Media::VectorDocument::SetSubject(UnsafeArrayOpt<const UTF8Char> subject)
{
	SDEL_TEXT(this->subject);
	UnsafeArray<const UTF8Char> nns;
	if (subject.SetTo(nns))
	{
		this->subject = Text::StrCopyNew(nns);
	}
}

UnsafeArrayOpt<const UTF8Char> Media::VectorDocument::GetSubject() const
{
	return this->subject;
}

void Media::VectorDocument::SetKeywords(UnsafeArrayOpt<const UTF8Char> keywords)
{
	SDEL_TEXT(this->keywords);
	UnsafeArray<const UTF8Char> nns;
	if (keywords.SetTo(nns))
	{
		this->keywords = Text::StrCopyNew(nns);
	}
}

UnsafeArrayOpt<const UTF8Char> Media::VectorDocument::GetKeywords() const
{
	return this->keywords;
}

void Media::VectorDocument::SetCreator(UnsafeArrayOpt<const UTF8Char> creator)
{
	SDEL_TEXT(this->creator);
	UnsafeArray<const UTF8Char> nns;
	if (creator.SetTo(nns))
	{
		this->creator = Text::StrCopyNew(nns);
	}
}

UnsafeArrayOpt<const UTF8Char> Media::VectorDocument::GetCreator() const
{
	return this->creator;
}

void Media::VectorDocument::SetProducer(UnsafeArrayOpt<const UTF8Char> producer)
{
	SDEL_TEXT(this->producer);
	UnsafeArray<const UTF8Char> nns;
	if (producer.SetTo(nns))
	{
		this->producer = Text::StrCopyNew(nns);
	}
}

UnsafeArrayOpt<const UTF8Char> Media::VectorDocument::GetProducer() const
{
	return this->producer;
}

UOSInt Media::VectorDocument::GetCount() const
{
	return this->items->GetCount();
}

Media::VectorGraph *Media::VectorDocument::GetItem(UOSInt Index) const
{
	return this->items->GetItem(Index);
}

Bool Media::VectorDocument::BeginPrint(NN<IPrintDocument> doc)
{
	Media::VectorGraph *graph;
	Double width;
	Double height;
	this->currGraph = 0;
	this->currDoc = doc;
	graph = this->items->GetItem(this->currGraph);
	if (graph == 0)
		return false;
	if (this->docName)
		doc->SetDocName(this->docName->ToCString());
	width = graph->GetVisibleWidthMM();
	height = graph->GetVisibleHeightMM();
	if (width > height)
	{
		doc->SetNextPagePaperSizeMM(height, width);
		doc->SetNextPageOrientation(Media::IPrintDocument::PageOrientation::Landscape);
	}
	else
	{
		doc->SetNextPagePaperSizeMM(width, height);
		doc->SetNextPageOrientation(Media::IPrintDocument::PageOrientation::Portrait);
	}
	return true;
}

Bool Media::VectorDocument::PrintPage(NN<Media::DrawImage> printPage)
{
	Media::VectorGraph *graph;
	Double width;
	Double height;
	graph = this->items->GetItem(this->currGraph);
	NN<Media::IPrintDocument> doc;
	if (!this->currDoc.SetTo(doc) || graph == 0)
		return false;
	graph->DrawTo(printPage, 0);
	this->currGraph++;
	graph = this->items->GetItem(this->currGraph);
	if (graph == 0)
		return false;
	width = graph->GetVisibleWidthMM();
	height = graph->GetVisibleHeightMM();
	if (width > height)
	{
		doc->SetNextPagePaperSizeMM(height, width);
		doc->SetNextPageOrientation(Media::IPrintDocument::PageOrientation::Landscape);
	}
	else
	{
		doc->SetNextPagePaperSizeMM(width, height);
		doc->SetNextPageOrientation(Media::IPrintDocument::PageOrientation::Portrait);
	}
	return true;
}

Bool Media::VectorDocument::EndPrint(NN<IPrintDocument> doc)
{
	this->currDoc = 0;
	return true;
}

IO::ParserType Media::VectorDocument::GetParserType() const
{
	return IO::ParserType::VectorDocument;
}
