#include "Stdafx.h"
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

Media::PDFParameter *Media::PDFObject::GetParameter()
{
	return this->parameter;
}
