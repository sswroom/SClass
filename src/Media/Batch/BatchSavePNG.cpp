#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/FileStream.h"
#include "IO/Path.h"
#include "Media/Batch/BatchSavePNG.h"
#include "Text/MyString.h"

Media::Batch::BatchSavePNG::BatchSavePNG()
{
	NEW_CLASS(exporter, Exporter::GUIPNGExporter());
	NEW_CLASS(mut, Sync::Mutex());
}

Media::Batch::BatchSavePNG::~BatchSavePNG()
{
	DEL_CLASS(mut);
	DEL_CLASS(exporter);
}

void Media::Batch::BatchSavePNG::ImageOutput(Media::ImageList *imgList, const UTF8Char *fileId, const UTF8Char *targetId)
{
	UTF8Char sbuff[512];
	UTF8Char *sptr;
	IO::FileStream *fs;
	sptr = Text::StrConcat(sbuff, fileId);
	void *param;
	Text::StrConcat(Text::StrConcat(sptr, targetId), (const UTF8Char*)".png");
	NEW_CLASS(fs, IO::FileStream(sbuff, IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::NoWriteBuffer));
	param = exporter->CreateParam(imgList);
	exporter->ExportFile(fs, sbuff, imgList, param);
	exporter->DeleteParam(param);
	DEL_CLASS(fs);
}
