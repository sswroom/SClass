#include "Stdafx.h"
#include "Exporter/GUIExporter.h"
#include "Media/ImageList.h"
#include "Media/ImageUtil.h"
#include "Media/StaticImage.h"
#include "Text/MyString.h"

Exporter::GUIExporter::GUIExporter() : IO::FileExporter()
{
}

Exporter::GUIExporter::~GUIExporter()
{
}

IO::FileExporter::SupportType Exporter::GUIExporter::IsObjectSupported(NN<IO::ParsedObject> pobj)
{
	NN<Media::ImageList> imgList;
	if (pobj->GetParserType() != IO::ParserType::ImageList)
		return IO::FileExporter::SupportType::NotSupported;
	imgList = NN<Media::ImageList>::ConvertFrom(pobj);
	if (imgList->GetCount() != 1)
		return IO::FileExporter::SupportType::NotSupported;
	NN<Media::RasterImage> img;
	if (!imgList->GetImage(0, 0).SetTo(img) || img->info.fourcc != 0)
		return IO::FileExporter::SupportType::NotSupported;
	return IO::FileExporter::SupportType::NormalStream;
}

void *Exporter::GUIExporter::ToImage(NN<IO::ParsedObject> pobj, UInt8 **relBuff)
{
	return 0;
}

Int32 Exporter::GUIExporter::GetEncoderClsid(const WChar *format, void *clsid)
{
	return -1;  // Failure
}

