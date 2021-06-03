#include "Stdafx.h"
#include "Exporter/GUIExporter.h"
#include "Media/ImageList.h"
#include "Media/ImageUtil.h"
#include "Media/StaticImage.h"
#include "Text/MyString.h"
#include <gdk-pixbuf/gdk-pixbuf.h>

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
	switch (img->info->pf)
	{
	case Media::PF_B8G8R8A8:
		return IO::FileExporter::ST_NORMAL_STREAM;
	case Media::PF_B8G8R8:
		return IO::FileExporter::ST_NORMAL_STREAM;
	default:
		return IO::FileExporter::ST_NOT_SUPPORTED;
	}
}

void *Exporter::GUIExporter::ToImage(IO::ParsedObject *pobj, UInt8 **relBuff)
{
	Media::ImageList *imgList;
	*relBuff = 0;
	if (pobj == 0)
	{
		return 0;
	}
	if (pobj->GetParserType() != IO::ParsedObject::PT_IMAGE_LIST_PARSER)
	{
		return 0;
	}
	imgList = (Media::ImageList*)pobj;
	if (imgList->GetCount() != 1)
	{
		return 0;
	}
	imgList->ToStaticImage(0);
	Media::StaticImage *img = (Media::StaticImage*)imgList->GetImage(0, 0);
	if (img == 0 || img->info->fourcc != 0)
	{
		return 0;
	}

	UInt8 *tmpBuff;
	GdkPixbuf *pixBuf = 0;
	switch (img->info->pf)
	{
	case Media::PF_B8G8R8A8:
		tmpBuff = MemAllocA(UInt8, img->info->dispHeight * img->info->storeWidth * 4);
		MemCopyANC(tmpBuff, img->data, img->info->dispHeight * img->info->storeWidth * 4);
		if (img->info->atype == Media::AT_NO_ALPHA)
		{
			ImageUtil_ImageFillAlpha32(tmpBuff, img->info->dispWidth, img->info->dispHeight, (OSInt)img->info->storeWidth * 4, 0xff);
		}
		ImageUtil_SwapRGB(tmpBuff, img->info->dispHeight * img->info->storeWidth, 32);
		pixBuf = gdk_pixbuf_new_from_data(tmpBuff, GDK_COLORSPACE_RGB, true, 8, (int)img->info->dispWidth, (int)img->info->dispHeight, (int)img->info->storeWidth << 2, 0, 0);
		*relBuff = tmpBuff;
		return pixBuf;
	case Media::PF_B8G8R8:
		tmpBuff = MemAllocA(UInt8, img->info->dispHeight * img->info->storeWidth * 4);
		MemCopyANC(tmpBuff, img->data, img->info->dispHeight * img->info->storeWidth * 4);
		ImageUtil_SwapRGB(tmpBuff, img->info->dispHeight * img->info->storeWidth, 32);
		pixBuf = gdk_pixbuf_new_from_data(tmpBuff, GDK_COLORSPACE_RGB, false, 8, (int)img->info->dispWidth, (int)img->info->dispHeight, (int)img->info->storeWidth * 3, 0, 0);
		*relBuff = tmpBuff;
		return pixBuf;
	default:
		return 0;
	}
}

Int32 Exporter::GUIExporter::GetEncoderClsid(const WChar *format, void *clsid)
{
	return -1;  // Failure
}

