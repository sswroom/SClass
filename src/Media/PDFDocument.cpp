#include "Stdafx.h"
#include "Media/PDFDocument.h"

Media::PDFDocument::PDFDocument(Text::String *sourceName, Text::CString version) : IO::ParsedObject(sourceName)
{
	this->version = Text::String::New(version);
}

Media::PDFDocument::~PDFDocument()
{
	SDEL_STRING(this->version);
	UOSInt i = this->objMap.GetCount();
	Media::PDFObject *obj;
	while (i-- > 0)
	{
		obj = this->objMap.GetItem(i);
		DEL_CLASS(obj);
	}
}

IO::ParserType Media::PDFDocument::GetParserType() const
{
	return IO::ParserType::PDFDocument;
}

UOSInt Media::PDFDocument::GetCount() const
{
	return this->objMap.GetCount();
}

Media::PDFObject *Media::PDFDocument::GetItem(UOSInt index) const
{
	return this->objMap.GetItem(index);
}

Media::PDFObject *Media::PDFDocument::AddObject(UInt32 id)
{
	Media::PDFObject *obj = this->objMap.Get(id);
	if (obj)
		return obj;
	NEW_CLASS(obj, Media::PDFObject(id));
	this->objMap.Put(id, obj);
	return obj;
}
