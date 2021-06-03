#include "Stdafx.h"
#include "Data/ByteTool.h"
#include "Exporter/CURExporter.h"
#include "Exporter/ICOExporter.h"
#include "IO/MemoryStream.h"
#include "Math/Math.h"
#include "Media/ImageList.h"
#include "Media/StaticImage.h"
#include "Text/MyString.h"

Exporter::ICOExporter::ICOExporter()
{
}

Exporter::ICOExporter::~ICOExporter()
{
}

Int32 Exporter::ICOExporter::GetName()
{
	return *(Int32*)"ICOE";
}

IO::FileExporter::SupportType Exporter::ICOExporter::IsObjectSupported(IO::ParsedObject *pobj)
{
	if (pobj->GetParserType() != IO::ParsedObject::PT_IMAGE_LIST_PARSER)
		return IO::FileExporter::ST_NOT_SUPPORTED;
	Media::ImageList *imgList = (Media::ImageList*)pobj;
	Int32 imgTime;
	Media::Image *img;
	UOSInt i = imgList->GetCount();
	if (i <= 0)
	{
		return IO::FileExporter::ST_NOT_SUPPORTED;
	}
	
	while (i-- > 0)
	{
		img = imgList->GetImage(0, &imgTime);
		if (!Exporter::CURExporter::ImageSupported(img))
		{
			return IO::FileExporter::ST_NOT_SUPPORTED;
		}
	}
	return IO::FileExporter::ST_NORMAL_STREAM;
}

Bool Exporter::ICOExporter::GetOutputName(UOSInt index, UTF8Char *nameBuff, UTF8Char *fileNameBuff)
{
	if (index == 0)
	{
		Text::StrConcat(nameBuff, (const UTF8Char*)"Icon File");
		Text::StrConcat(fileNameBuff, (const UTF8Char*)"*.ico");
		return true;
	}
	return false;
}

void Exporter::ICOExporter::SetCodePage(UInt32 codePage)
{
}

Bool Exporter::ICOExporter::ExportFile(IO::SeekableStream *stm, const UTF8Char *fileName, IO::ParsedObject *pobj, void *param)
{
	if (pobj->GetParserType() != IO::ParsedObject::PT_IMAGE_LIST_PARSER)
		return 0;
	Media::ImageList *imgList = (Media::ImageList*)pobj;
	UOSInt buffSize = Exporter::CURExporter::CalcBuffSize(imgList);
	if (buffSize == 0)
		return false;
	UInt8 *buff = MemAlloc(UInt8, buffSize);
	if (buffSize != Exporter::CURExporter::BuildBuff(buff, imgList, false))
	{
		MemFree(buff);
		return false;
	}
	stm->Write(buff, buffSize);
	MemFree(buff);
	return true;
}
