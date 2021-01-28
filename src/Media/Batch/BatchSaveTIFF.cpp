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
	Text::StrConcat(Text::StrConcat(sptr, targetId), (const UTF8Char*)".tif");
	NEW_CLASS(fs, IO::FileStream(sbuff, IO::FileStream::FILE_MODE_CREATE, IO::FileStream::FILE_SHARE_DENY_NONE, IO::FileStream::BT_NO_WRITE_BUFFER));
	param = exporter->CreateParam(imgList);
	exporter->SetParamBool(param, 0, this->isCompressed);
	exporter->ExportFile(fs, sbuff, imgList, param);
	exporter->DeleteParam(param);
	DEL_CLASS(fs);
}
