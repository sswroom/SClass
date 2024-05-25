#include "Stdafx.h"
#include "Media/PDFDocument.h"

Optional<IO::ParsedObject> Media::PDFDocument::SetPObjName(Optional<IO::ParsedObject> pobj, UInt32 objId, Text::CString ext)
{
	NN<IO::ParsedObject> nnpobj;
	if (pobj.SetTo(nnpobj))
	{
		Text::StringBuilderUTF8 sb;
		sb.Append(this->sourceName);
		sb.AppendUTF8Char('.');
		sb.AppendU32(objId);
		if (ext.leng > 0)
		{
			sb.Append(ext);
		}
		nnpobj->SetSourceName(sb.ToCString());
	}
	return pobj;
}

Media::PDFDocument::PDFDocument(NN<Text::String> sourceName, Text::CString version) : IO::ParsedObject(sourceName)
{
	this->version = Text::String::New(version);
}

Media::PDFDocument::~PDFDocument()
{
	this->version->Release();
	UOSInt i = this->objMap.GetCount();
	NN<Media::PDFObject> obj;
	while (i-- > 0)
	{
		obj = this->objMap.GetItemNoCheck(i);
		obj.Delete();
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

NN<Media::PDFObject> Media::PDFDocument::GetItemNoCheck(UOSInt index) const
{
	return this->objMap.GetItemNoCheck(index);
}

Optional<Media::PDFObject> Media::PDFDocument::GetItem(UOSInt index) const
{
	return this->objMap.GetItem(index);
}

NN<Media::PDFObject> Media::PDFDocument::AddObject(UInt32 id)
{
	NN<Media::PDFObject> obj;
	if (this->objMap.Get(id).SetTo(obj))
		return obj;
	NEW_CLASSNN(obj, Media::PDFObject(id));
	this->objMap.Put(id, obj);
	return obj;
}

Optional<Media::ImageList> Media::PDFDocument::CreateImage(UInt32 id, NN<Parser::ParserList> parsers)
{
	NN<Media::PDFObject> obj;
	if (!this->objMap.Get(id).SetTo(obj))
		return 0;
	if (!obj->IsImage())
		return 0;
	NN<IO::StreamData> fd;
	NN<Text::String> filter;
	if (fd.Set(obj->GetData()) && obj->GetFilter().SetTo(filter))
	{
		if (filter->Equals(UTF8STRC("DCTDecode")))
		{
			return Optional<Media::ImageList>::ConvertFrom(SetPObjName(parsers->ParseFileType(fd, IO::ParserType::ImageList), id, CSTR(".jpg")));
		}
		if (filter->Equals(UTF8STRC("[/DCTDecode]")))
		{
			return Optional<Media::ImageList>::ConvertFrom(SetPObjName(parsers->ParseFileType(fd, IO::ParserType::ImageList), id, CSTR(".jpg")));
		}
		if (filter->Equals(UTF8STRC("FlateDecode")))
		{
			UOSInt bpc = obj->GetBitPerComponent();
			UOSInt width = obj->GetWidth();
			UOSInt height = obj->GetHeight();
			NN<Text::String> colorSpace;
			if (bpc == 0 || width == 0 || height == 0)
				return 0;
			if (bpc == 8)
			{
				if (!obj->GetColorSpace().SetTo(colorSpace) || colorSpace->Equals(UTF8STRC("DeviceGray")))
				{

				}
			}
		}
	}
	return 0;
}
