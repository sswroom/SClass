#include "Stdafx.h"
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

void Media::PDFObject::SetStream(IO::StreamData *fd, UInt64 ofst, UInt64 len)
{
	SDEL_CLASS(this->fd);
	this->streamData = true;
	this->fd = fd->GetPartialData(ofst, len);
}

void Media::PDFObject::SetData(IO::StreamData *fd, UInt64 ofst, UInt64 len)
{
	SDEL_CLASS(this->fd);
	this->streamData = false;
	this->fd = fd->GetPartialData(ofst, len);
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

Bool Media::PDFObject::SaveFile(Text::CString fileName)
{
	if (this->fd)
	{
		IO::FileStream fs(fileName, IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
		return fs.WriteFromData(this->fd, 1048576);
	}
	return false;
}
