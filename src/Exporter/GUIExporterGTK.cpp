#include "Stdafx.h"
#include "Exporter/GUIExporter.h"
#include "Media/ImageList.h"
#include "Media/ImageUtil.h"
#include "Media/StaticImage.h"
#include "Text/MyString.h"
#include <gdk-pixbuf/gdk-pixbuf.h>

void GUIExporter_SetDPI(GdkPixbuf *pixbuf, Double hdpi, Double vdpi)
{
	UTF8Char sbuff[64];
	Text::StrInt32(sbuff, Double2Int32(hdpi));
	gdk_pixbuf_set_option(pixbuf, "x-dpi", (const Char*)sbuff);
	Text::StrInt32(sbuff, Double2Int32(vdpi));
	gdk_pixbuf_set_option(pixbuf, "y-dpi", (const Char*)sbuff);
}

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
		return IO::FileExporter::SupportType::NotSupported;
	if (pobj->GetParserType() != IO::ParserType::ImageList)
		return IO::FileExporter::SupportType::NotSupported;
	imgList = (Media::ImageList*)pobj;
	if (imgList->GetCount() != 1)
		return IO::FileExporter::SupportType::NotSupported;
	Media::Image *img = imgList->GetImage(0, 0);
	if (img->info.fourcc != 0)
		return IO::FileExporter::SupportType::NotSupported;
	switch (img->info.pf)
	{
	case Media::PF_B8G8R8A8:
		return IO::FileExporter::SupportType::NormalStream;
	case Media::PF_B8G8R8:
		return IO::FileExporter::SupportType::NormalStream;
	case Media::PF_R8G8B8:
		return IO::FileExporter::SupportType::NormalStream;
	case Media::PF_R8G8B8A8:
		return IO::FileExporter::SupportType::NormalStream;
	case Media::PF_PAL_1:
	case Media::PF_PAL_2:
	case Media::PF_PAL_4:
	case Media::PF_PAL_8:
	case Media::PF_PAL_W1:
	case Media::PF_PAL_W2:
	case Media::PF_PAL_W4:
	case Media::PF_PAL_W8:
	case Media::PF_LE_R5G5B5:
	case Media::PF_LE_R5G6B5:
	case Media::PF_LE_B16G16R16A16:
	case Media::PF_LE_B16G16R16:
	case Media::PF_LE_W16:
	case Media::PF_LE_A2B10G10R10:
	case Media::PF_W8A8:
	case Media::PF_LE_W16A16:
	case Media::PF_LE_FB32G32R32A32:
	case Media::PF_LE_FB32G32R32:
	case Media::PF_LE_FW32A32:
	case Media::PF_LE_FW32:
	case Media::PF_PAL_1_A1:
	case Media::PF_PAL_2_A1:
	case Media::PF_PAL_4_A1:
	case Media::PF_PAL_8_A1:
	case Media::PF_B8G8R8A1:
	case Media::PF_UNKNOWN:
	default:
		return IO::FileExporter::SupportType::NotSupported;
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
	if (pobj->GetParserType() != IO::ParserType::ImageList)
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
	if (img == 0 || img->info.fourcc != 0)
	{
		return 0;
	}

	UInt8 *tmpBuff;
	GdkPixbuf *pixBuf = 0;
	switch (img->info.pf)
	{
	case Media::PF_B8G8R8A8:
		tmpBuff = MemAllocA(UInt8, img->info.dispHeight * img->info.storeWidth * 4);
		MemCopyANC(tmpBuff, img->data, img->info.dispHeight * img->info.storeWidth * 4);
		if (img->info.atype == Media::AT_NO_ALPHA)
		{
			ImageUtil_ConvR8G8B8N8_ARGB32(tmpBuff, tmpBuff, img->info.dispWidth, img->info.dispHeight, (OSInt)img->info.storeWidth * 4, (OSInt)img->info.storeWidth * 4);
		}
		else
		{
			ImageUtil_SwapRGB(tmpBuff, img->info.dispHeight * img->info.storeWidth, 32);
		}
		pixBuf = gdk_pixbuf_new_from_data(tmpBuff, GDK_COLORSPACE_RGB, true, 8, (int)img->info.dispWidth, (int)img->info.dispHeight, (int)img->info.storeWidth << 2, 0, 0);
		GUIExporter_SetDPI(pixBuf, img->info.hdpi, img->info.vdpi);
		*relBuff = tmpBuff;
		return pixBuf;
	case Media::PF_B8G8R8:
		tmpBuff = MemAllocA(UInt8, img->info.dispHeight * img->info.storeWidth * 3);
		MemCopyANC(tmpBuff, img->data, img->info.dispHeight * img->info.storeWidth * 3);
		ImageUtil_SwapRGB(tmpBuff, img->info.dispHeight * img->info.storeWidth, 24);
		pixBuf = gdk_pixbuf_new_from_data(tmpBuff, GDK_COLORSPACE_RGB, false, 8, (int)img->info.dispWidth, (int)img->info.dispHeight, (int)img->info.storeWidth * 3, 0, 0);
		GUIExporter_SetDPI(pixBuf, img->info.hdpi, img->info.vdpi);
		*relBuff = tmpBuff;
		return pixBuf;
	case Media::PF_R8G8B8:
		tmpBuff = MemAllocA(UInt8, img->info.dispHeight * img->info.storeWidth * 3);
		MemCopyANC(tmpBuff, img->data, img->info.dispHeight * img->info.storeWidth * 3);
		pixBuf = gdk_pixbuf_new_from_data(tmpBuff, GDK_COLORSPACE_RGB, false, 8, (int)img->info.dispWidth, (int)img->info.dispHeight, (int)img->info.storeWidth * 3, 0, 0);
		GUIExporter_SetDPI(pixBuf, img->info.hdpi, img->info.vdpi);
		*relBuff = tmpBuff;
		return pixBuf;
	case Media::PF_R8G8B8A8:
		tmpBuff = MemAllocA(UInt8, img->info.dispHeight * img->info.storeWidth * 4);
		MemCopyANC(tmpBuff, img->data, img->info.dispHeight * img->info.storeWidth * 4);
		pixBuf = gdk_pixbuf_new_from_data(tmpBuff, GDK_COLORSPACE_RGB, true, 8, (int)img->info.dispWidth, (int)img->info.dispHeight, (int)img->info.storeWidth << 2, 0, 0);
		GUIExporter_SetDPI(pixBuf, img->info.hdpi, img->info.vdpi);
		*relBuff = tmpBuff;
		return pixBuf;
	case Media::PF_PAL_1:
	case Media::PF_PAL_2:
	case Media::PF_PAL_4:
	case Media::PF_PAL_8:
	case Media::PF_PAL_W1:
	case Media::PF_PAL_W2:
	case Media::PF_PAL_W4:
	case Media::PF_PAL_W8:
	case Media::PF_LE_R5G5B5:
	case Media::PF_LE_R5G6B5:
	case Media::PF_LE_B16G16R16A16:
	case Media::PF_LE_B16G16R16:
	case Media::PF_LE_W16:
	case Media::PF_LE_A2B10G10R10:
	case Media::PF_W8A8:
	case Media::PF_LE_W16A16:
	case Media::PF_LE_FB32G32R32A32:
	case Media::PF_LE_FB32G32R32:
	case Media::PF_LE_FW32A32:
	case Media::PF_LE_FW32:
	case Media::PF_PAL_1_A1:
	case Media::PF_PAL_2_A1:
	case Media::PF_PAL_4_A1:
	case Media::PF_PAL_8_A1:
	case Media::PF_B8G8R8A1:
	case Media::PF_UNKNOWN:
	default:
		return 0;
	}
}

Int32 Exporter::GUIExporter::GetEncoderClsid(const WChar *format, void *clsid)
{
	return -1;  // Failure
}

