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
	if (pobj->GetParserType() != IO::ParserType::ImageList)
		return IO::FileExporter::SupportType::NotSupported;
	Media::ImageList *imgList = (Media::ImageList*)pobj;
	UInt32 imgTime;
	Media::Image *img;
	UOSInt i = imgList->GetCount();
	if (i <= 0)
	{
		return IO::FileExporter::SupportType::NotSupported;
	}
	
	while (i-- > 0)
	{
		img = imgList->GetImage(0, &imgTime);
		if (!Exporter::CURExporter::ImageSupported(img))
		{
			return IO::FileExporter::SupportType::NotSupported;
		}
	}
	return IO::FileExporter::SupportType::NormalStream;
}

Bool Exporter::ICOExporter::GetOutputName(UOSInt index, UTF8Char *nameBuff, UTF8Char *fileNameBuff)
{
	if (index == 0)
	{
		Text::StrConcatC(nameBuff, UTF8STRC("Icon File"));
		Text::StrConcatC(fileNameBuff, UTF8STRC("*.ico"));
		return true;
	}
	return false;
}

void Exporter::ICOExporter::SetCodePage(UInt32 codePage)
{
}

Bool Exporter::ICOExporter::ExportFile(IO::SeekableStream *stm, const UTF8Char *fileName, IO::ParsedObject *pobj, void *param)
{
	if (pobj->GetParserType() != IO::ParserType::ImageList)
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
