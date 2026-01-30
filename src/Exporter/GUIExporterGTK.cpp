#include "Stdafx.h"
#include "Exporter/GUIExporter.h"
#include "Media/ImageList.h"
#include "Media/ImageUtil_C.h"
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

IO::FileExporter::SupportType Exporter::GUIExporter::IsObjectSupported(NN<IO::ParsedObject> pobj)
{
	NN<Media::ImageList> imgList;
	if (pobj->GetParserType() != IO::ParserType::ImageList)
		return IO::FileExporter::SupportType::NotSupported;
	imgList = NN<Media::ImageList>::ConvertFrom(pobj);
	if (imgList->GetCount() != 1)
		return IO::FileExporter::SupportType::NotSupported;
	NN<Media::RasterImage> img;
	if (!imgList->GetImage(0, 0).SetTo(img))
		return IO::FileExporter::SupportType::NotSupported;
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
	case Media::PF_LE_R16G16B16A16:
	case Media::PF_LE_B16G16R16:
	case Media::PF_LE_R16G16B16:
	case Media::PF_LE_W16:
	case Media::PF_LE_A2B10G10R10:
	case Media::PF_W8A8:
	case Media::PF_LE_W16A16:
	case Media::PF_LE_FB32G32R32A32:
	case Media::PF_LE_FR32G32B32A32:
	case Media::PF_LE_FB32G32R32:
	case Media::PF_LE_FR32G32B32:
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

AnyType Exporter::GUIExporter::ToImage(NN<IO::ParsedObject> pobj, OutParam<UInt8*> relBuff)
{
	NN<Media::ImageList> imgList;
	relBuff.Set(nullptr);
	if (pobj->GetParserType() != IO::ParserType::ImageList)
	{
		return 0;
	}
	imgList = NN<Media::ImageList>::ConvertFrom(pobj);
	if (imgList->GetCount() != 1)
	{
		return 0;
	}
	imgList->ToStaticImage(0);
	NN<Media::StaticImage> img;
	if (!Optional<Media::StaticImage>::ConvertFrom(imgList->GetImage(0, 0)).SetTo(img) || img->info.fourcc != 0)
	{
		return 0;
	}

	UInt8 *tmpBuff;
	GdkPixbuf *pixBuf = 0;
	switch (img->info.pf)
	{
	case Media::PF_B8G8R8A8:
		tmpBuff = MemAllocA(UInt8, img->info.dispSize.y * img->info.storeSize.x * 4);
		MemCopyANC(tmpBuff, img->data.Ptr(), img->info.dispSize.y * img->info.storeSize.x * 4);
		if (img->info.atype == Media::AT_ALPHA_ALL_FF || img->info.atype == Media::AT_IGNORE_ALPHA)
		{
			ImageUtil_ConvR8G8B8N8_B8G8R8A8(tmpBuff, tmpBuff, img->info.dispSize.x, img->info.dispSize.y, (IntOS)img->info.storeSize.x * 4, (IntOS)img->info.storeSize.x * 4);
		}
		else
		{
			ImageUtil_SwapRGB(tmpBuff, img->info.dispSize.y * img->info.storeSize.x, 32);
		}
		pixBuf = gdk_pixbuf_new_from_data(tmpBuff, GDK_COLORSPACE_RGB, true, 8, (int)img->info.dispSize.x, (int)img->info.dispSize.y, (int)img->info.storeSize.x << 2, 0, 0);
		GUIExporter_SetDPI(pixBuf, img->info.hdpi, img->info.vdpi);
		relBuff.Set(tmpBuff);
		return pixBuf;
	case Media::PF_B8G8R8:
		tmpBuff = MemAllocA(UInt8, img->info.dispSize.y * img->info.storeSize.x * 3);
		MemCopyANC(tmpBuff, img->data.Ptr(), img->info.dispSize.y * img->info.storeSize.x * 3);
		ImageUtil_SwapRGB(tmpBuff, img->info.dispSize.y * img->info.storeSize.x, 24);
		pixBuf = gdk_pixbuf_new_from_data(tmpBuff, GDK_COLORSPACE_RGB, false, 8, (int)img->info.dispSize.x, (int)img->info.dispSize.y, (int)img->info.storeSize.x * 3, 0, 0);
		GUIExporter_SetDPI(pixBuf, img->info.hdpi, img->info.vdpi);
		relBuff.Set(tmpBuff);
		return pixBuf;
	case Media::PF_R8G8B8:
		tmpBuff = MemAllocA(UInt8, img->info.dispSize.y * img->info.storeSize.x * 3);
		MemCopyANC(tmpBuff, img->data.Ptr(), img->info.dispSize.y * img->info.storeSize.x * 3);
		pixBuf = gdk_pixbuf_new_from_data(tmpBuff, GDK_COLORSPACE_RGB, false, 8, (int)img->info.dispSize.x, (int)img->info.dispSize.y, (int)img->info.storeSize.x * 3, 0, 0);
		GUIExporter_SetDPI(pixBuf, img->info.hdpi, img->info.vdpi);
		relBuff.Set(tmpBuff);
		return pixBuf;
	case Media::PF_R8G8B8A8:
		tmpBuff = MemAllocA(UInt8, img->info.dispSize.y * img->info.storeSize.x * 4);
		MemCopyANC(tmpBuff, img->data.Ptr(), img->info.dispSize.y * img->info.storeSize.x * 4);
		pixBuf = gdk_pixbuf_new_from_data(tmpBuff, GDK_COLORSPACE_RGB, true, 8, (int)img->info.dispSize.x, (int)img->info.dispSize.y, (int)img->info.storeSize.x << 2, 0, 0);
		GUIExporter_SetDPI(pixBuf, img->info.hdpi, img->info.vdpi);
		relBuff.Set(tmpBuff);
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
	case Media::PF_LE_R16G16B16A16:
	case Media::PF_LE_B16G16R16:
	case Media::PF_LE_R16G16B16:
	case Media::PF_LE_W16:
	case Media::PF_LE_A2B10G10R10:
	case Media::PF_W8A8:
	case Media::PF_LE_W16A16:
	case Media::PF_LE_FB32G32R32A32:
	case Media::PF_LE_FR32G32B32A32:
	case Media::PF_LE_FB32G32R32:
	case Media::PF_LE_FR32G32B32:
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

Int32 Exporter::GUIExporter::GetEncoderClsid(UnsafeArray<const WChar> format, void *clsid)
{
	return -1;  // Failure
}

