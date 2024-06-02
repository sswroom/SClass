#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/FileStream.h"
#include "IO/Path.h"
#include "Media/Batch/BatchSavePNG.h"
#include "Text/MyString.h"

Media::Batch::BatchSavePNG::BatchSavePNG()
{
}

Media::Batch::BatchSavePNG::~BatchSavePNG()
{
}

void Media::Batch::BatchSavePNG::ImageOutput(NN<Media::ImageList> imgList, Text::CStringNN fileId, Text::CStringNN targetId)
{
	UTF8Char sbuff[512];
	UnsafeArray<UTF8Char> sptr;
	sptr = fileId.ConcatTo(sbuff);
	Optional<IO::FileExporter::ParamData> param;
	sptr = Text::StrConcatC(targetId.ConcatTo(sptr), UTF8STRC(".png"));
	IO::FileStream fs(CSTRP(sbuff, sptr), IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::NoWriteBuffer);
	param = this->exporter.CreateParam(imgList);
	this->exporter.ExportFile(fs, CSTRP(sbuff, sptr), imgList, param);
	this->exporter.DeleteParam(param);
}
