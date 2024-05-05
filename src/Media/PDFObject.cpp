#include "Stdafx.h"
#include "Data/Compress/InflateStream.h"
#include "IO/FileStream.h"
#include "Media/PDFObject.h"

Media::PDFObject::PDFObject(UInt32 id)
{
	this->id = id;
	this->streamData = false;
	this->fd = 0;
	this->parameter = 0;
}

Media::PDFObject::~PDFObject()
{
	SDEL_CLASS(this->fd);
	SDEL_CLASS(this->parameter);
}

void Media::PDFObject::SetStream(NN<IO::StreamData> fd, UInt64 ofst, UInt64 len)
{
	SDEL_CLASS(this->fd);
	this->streamData = true;
	this->fd = fd->GetPartialData(ofst, len).Ptr();
}

void Media::PDFObject::SetData(NN<IO::StreamData> fd, UInt64 ofst, UInt64 len)
{
	SDEL_CLASS(this->fd);
	this->streamData = false;
	this->fd = fd->GetPartialData(ofst, len).Ptr();
}

void Media::PDFObject::SetParameter(PDFParameter *parameter)
{
	SDEL_CLASS(this->parameter);
	this->parameter = parameter;
}

Media::PDFParameter *Media::PDFObject::GetParameter() const
{
	return this->parameter;
}

UInt32 Media::PDFObject::GetId() const
{
	return this->id;
}

Bool Media::PDFObject::IsImage() const
{
	return this->parameter && this->parameter->ContainsEntry(CSTR("Image"));
}

Optional<Text::String> Media::PDFObject::GetType() const
{
	if (this->parameter == 0)
		return 0;
	UOSInt i = this->parameter->GetEntryIndex(CSTR("Type"));
	if (i != INVALID_INDEX)
		return this->parameter->GetEntryType(i + 1);
	return 0;
}

Optional<Text::String> Media::PDFObject::GetSubtype() const
{
	if (this->parameter == 0)
		return 0;
	UOSInt i = this->parameter->GetEntryIndex(CSTR("Subtype"));
	if (i != INVALID_INDEX)
		return this->parameter->GetEntryType(i + 1);
	return 0;
}

Optional<Text::String> Media::PDFObject::GetFilter() const
{
	if (this->parameter == 0)
		return 0;
	NN<Text::String> s;
	UOSInt i = this->parameter->GetEntryIndex(CSTR("Filter"));
	if (i != INVALID_INDEX)
	{
		NN<Media::PDFParameter::ParamEntry> entry = this->parameter->GetItemNoCheck(i);
		if (entry->value.SetTo(s))
			return s;
		return this->parameter->GetEntryType(i + 1);
	}
	return 0;
}

Optional<Text::String> Media::PDFObject::GetColorSpace() const
{
	if (this->parameter == 0)
		return 0;
	NN<Text::String> s;
	UOSInt i = this->parameter->GetEntryIndex(CSTR("ColorSpace"));
	if (i != INVALID_INDEX)
	{
		NN<Media::PDFParameter::ParamEntry> entry = this->parameter->GetItemNoCheck(i);
		if (entry->value.SetTo(s))
			return s;
		return this->parameter->GetEntryType(i + 1);
	}
	return 0;
}

UOSInt Media::PDFObject::GetBitPerComponent() const
{
	if (this->parameter == 0)
		return 0;
	NN<Text::String> s;
	if (this->parameter->GetEntryValue(CSTR("BitPerComponent")).SetTo(s))
		return s->ToUOSInt();
	return 0;
}

UOSInt Media::PDFObject::GetWidth() const
{
	if (this->parameter == 0)
		return 0;
	NN<Text::String> s;
	if (this->parameter->GetEntryValue(CSTR("Width")).SetTo(s))
		return s->ToUOSInt();
	return 0;
}

UOSInt Media::PDFObject::GetHeight() const
{
	if (this->parameter == 0)
		return 0;
	NN<Text::String> s;
	if (this->parameter->GetEntryValue(CSTR("Height")).SetTo(s))
		return s->ToUOSInt();
	return 0;
}

IO::StreamData *Media::PDFObject::GetData() const
{
	return this->fd;
}

Bool Media::PDFObject::SaveFile(Text::CStringNN fileName)
{
	if (this->fd)
	{
		IO::FileStream fs(fileName, IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
		return this->SaveStream(fs);
	}
	return false;
}

Bool Media::PDFObject::SaveStream(NN<IO::Stream> stm)
{
	NN<IO::StreamData> fd;
	if (fd.Set(this->fd))
	{
		NN<Text::String> filter;
		if (this->GetFilter().SetTo(filter) && filter->Equals(UTF8STRC("FlateDecode")))
		{
			Data::Compress::InflateStream infStm(stm, true);
			return infStm.WriteFromData(fd, 1048576);
		}
		else
		{
			return stm->WriteFromData(fd, 1048576);
		}
	}
	return false;
}

