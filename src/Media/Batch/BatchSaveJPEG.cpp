#include "Stdafx.h"
#include "MyMemory.h"
#include "Text/MyString.h"
#include "IO/FileStream.h"
#include "IO/Path.h"
#include "Media/Batch/BatchSaveJPEG.h"

Media::Batch::BatchSaveJPEG::BatchSaveJPEG(UInt32 quality)
{
	if (quality > 100)
		this->quality = 100;
	else
		this->quality = quality;
}

Media::Batch::BatchSaveJPEG::~BatchSaveJPEG()
{
}

void Media::Batch::BatchSaveJPEG::ImageOutput(Media::ImageList *imgList, const UTF8Char *fileId, const UTF8Char *targetId)
{
	UTF8Char sbuff[512];
	UTF8Char *sptr;
	sptr = Text::StrConcat(sbuff, fileId);
	void *param;
	sptr = Text::StrConcatC(Text::StrConcat(sptr, targetId), UTF8STRC(".jpg"));
	IO::FileStream fs(CSTRP(sbuff, sptr), IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::NoWriteBuffer);
	param = this->exporter.CreateParam(imgList);
	this->exporter.SetParamInt32(param, 0, (Int32)this->quality);
	this->exporter.ExportFile(&fs, CSTRP(sbuff, sptr), imgList, param);
	this->exporter.DeleteParam(param);
}
