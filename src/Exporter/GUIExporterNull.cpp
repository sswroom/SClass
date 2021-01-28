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

IO::FileExporter::SupportType Exporter::GUIExporter::IsObjectSupported(IO::ParsedObject *pobj)
{
	Media::ImageList *imgList;
	if (pobj == 0)
		return IO::FileExporter::ST_NOT_SUPPORTED;
	if (pobj->GetParserType() != IO::ParsedObject::PT_IMAGE_LIST_PARSER)
		return IO::FileExporter::ST_NOT_SUPPORTED;
	imgList = (Media::ImageList*)pobj;
	if (imgList->GetCount() != 1)
		return IO::FileExporter::ST_NOT_SUPPORTED;
	Media::Image *img = imgList->GetImage(0, 0);
	if (img->info->fourcc != 0)
		return IO::FileExporter::ST_NOT_SUPPORTED;
	return IO::FileExporter::ST_NORMAL_STREAM;
}

void *Exporter::GUIExporter::ToImage(IO::ParsedObject *pobj, UInt8 **relBuff)
{
	return 0;
}

Int32 Exporter::GUIExporter::GetEncoderClsid(const WChar *format, void *clsid)
{
	return -1;  // Failure
}

