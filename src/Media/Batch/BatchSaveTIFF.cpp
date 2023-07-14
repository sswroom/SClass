#include "Stdafx.h"
#include "MyMemory.h"
#include "Text/MyString.h"
#include "IO/FileStream.h"
#include "IO/Path.h"
#include "Media/Batch/BatchSaveTIFF.h"

Media::Batch::BatchSaveTIFF::BatchSaveTIFF(Bool isCompressed)
{
	this->isCompressed = isCompressed;
}

Media::Batch::BatchSaveTIFF::~BatchSaveTIFF()
{
}

void Media::Batch::BatchSaveTIFF::ImageOutput(Media::ImageList *imgList, const UTF8Char *fileId, const UTF8Char *targetId)
{
	UTF8Char sbuff[512];
	UTF8Char *sptr;
	sptr = Text::StrConcat(sbuff, fileId);
	void *param;
	sptr = Text::StrConcatC(Text::StrConcat(sptr, targetId), UTF8STRC(".tif"));
	IO::FileStream fs(CSTRP(sbuff, sptr), IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::NoWriteBuffer);
	param = this->exporter.CreateParam(imgList);
	this->exporter.SetParamBool(param, 0, this->isCompressed);
	this->exporter.ExportFile(fs, CSTRP(sbuff, sptr), imgList, param);
	this->exporter.DeleteParam(param);
}
