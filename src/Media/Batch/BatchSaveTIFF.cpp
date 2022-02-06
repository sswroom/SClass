#include "Stdafx.h"
#include "MyMemory.h"
#include "Text/MyString.h"
#include "IO/FileStream.h"
#include "IO/Path.h"
#include "Media/Batch/BatchSaveTIFF.h"

Media::Batch::BatchSaveTIFF::BatchSaveTIFF(Bool isCompressed)
{
	NEW_CLASS(exporter, Exporter::GUITIFExporter());
	NEW_CLASS(mut, Sync::Mutex());
	this->isCompressed = isCompressed;
}

Media::Batch::BatchSaveTIFF::~BatchSaveTIFF()
{
	DEL_CLASS(mut);
	DEL_CLASS(exporter);
}

void Media::Batch::BatchSaveTIFF::ImageOutput(Media::ImageList *imgList, const UTF8Char *fileId, const UTF8Char *targetId)
{
	UTF8Char sbuff[512];
	UTF8Char *sptr;
	IO::FileStream *fs;
	sptr = Text::StrConcat(sbuff, fileId);
	void *param;
	sptr = Text::StrConcatC(Text::StrConcat(sptr, targetId), UTF8STRC(".tif"));
	NEW_CLASS(fs, IO::FileStream(CSTRP(sbuff, sptr), IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::NoWriteBuffer));
	param = exporter->CreateParam(imgList);
	exporter->SetParamBool(param, 0, this->isCompressed);
	exporter->ExportFile(fs, sbuff, imgList, param);
	exporter->DeleteParam(param);
	DEL_CLASS(fs);
}
