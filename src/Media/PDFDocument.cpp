#include "Stdafx.h"
#include "Media/PDFDocument.h"

IO::ParsedObject *Media::PDFDocument::SetPObjName(IO::ParsedObject *pobj, UInt32 objId, Text::CString ext)
{
	if (pobj)
	{
		Text::StringBuilderUTF8 sb;
		sb.Append(this->sourceName);
		sb.AppendUTF8Char('.');
		sb.AppendU32(objId);
		if (ext.leng > 0)
		{
			sb.Append(ext);
		}
		pobj->SetSourceName(sb.ToCString());
	}
	return pobj;
}

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

Media::ImageList *Media::PDFDocument::CreateImage(UInt32 id, Parser::ParserList *parsers)
{
	Media::PDFObject *obj = this->objMap.Get(id);
	if (obj == 0)
		return 0;
	if (!obj->IsImage())
		return 0;
	IO::StreamData *fd = obj->GetData();
	Text::String *filter = obj->GetFilter();
	if (fd && filter)
	{
		if (filter->Equals(UTF8STRC("DCTDecode")))
		{
			return (Media::ImageList*)SetPObjName(parsers->ParseFileType(fd, IO::ParserType::ImageList), id, CSTR(".jpg"));
		}
		if (filter->Equals(UTF8STRC("[/DCTDecode]")))
		{
			return (Media::ImageList*)SetPObjName(parsers->ParseFileType(fd, IO::ParserType::ImageList), id, CSTR(".jpg"));
		}
		if (filter->Equals(UTF8STRC("FlateDecode")))
		{
			UOSInt bpc = obj->GetBitPerComponent();
			UOSInt width = obj->GetWidth();
			UOSInt height = obj->GetHeight();
			Text::String *colorSpace = obj->GetColorSpace();
			if (bpc == 0 || width == 0 || height == 0)
				return 0;
			if (bpc == 8)
			{
				if (colorSpace == 0 || colorSpace->Equals(UTF8STRC("DeviceGray")))
				{

				}
			}
		}
	}
	return 0;
}
