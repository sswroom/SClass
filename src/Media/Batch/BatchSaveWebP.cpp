#include "Stdafx.h"
#include "MyMemory.h"
#include "Text/MyString.h"
#include "IO/FileStream.h"
#include "IO/Path.h"
#include "Media/Batch/BatchSaveWebP.h"

Media::Batch::BatchSaveWebP::BatchSaveWebP(UInt32 quality)
{
	if (quality > 100)
		this->quality = 100;
	else
		this->quality = quality;
}

Media::Batch::BatchSaveWebP::~BatchSaveWebP()
{
}

void Media::Batch::BatchSaveWebP::ImageOutput(NotNullPtr<Media::ImageList> imgList, Text::CStringNN fileId, Text::CStringNN targetId)
{
	UTF8Char sbuff[512];
	UTF8Char *sptr;
	sptr = fileId.ConcatTo(sbuff);
	Optional<IO::FileExporter::ParamData> param;
	sptr = Text::StrConcatC(targetId.ConcatTo(sptr), UTF8STRC(".webp"));
	IO::FileStream fs(CSTRP(sbuff, sptr), IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::NoWriteBuffer);
	param = this->exporter.CreateParam(imgList);
	this->exporter.SetParamInt32(param, 0, (Int32)this->quality);
	this->exporter.ExportFile(fs, CSTRP(sbuff, sptr), imgList, param);
	this->exporter.DeleteParam(param);
}
