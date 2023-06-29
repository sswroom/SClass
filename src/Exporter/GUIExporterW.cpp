#include "Stdafx.h"
#include "Exporter/GUIExporter.h"
#include "Media/ImageList.h"
#include "Text/MyString.h"
#include "Text/MyStringW.h"

#if defined(WIN32_LEAN_AND_MEAN)
#undef WIN32_LEAN_AND_MEAN
#endif

#include <windows.h>

#ifdef _WIN32_WCE
Exporter::GUIExporter::GUIExporter() : IO::FileExporter()
{
}

Exporter::GUIExporter::~GUIExporter()
{
}

IO::FileExporter::SupportType Exporter::GUIExporter::IsObjectSupported(IO::ParsedObject *pobj)
{
	return IO::FileExporter::SupportType::NotSupported;
}

void *Exporter::GUIExporter::ToImage(IO::ParsedObject *pobj, UInt8 **relBuff)
{
	return 0;
}
#else
#include <gdiplus.h>

typedef struct
{
	UInt32 gdiplusToken;
	Gdiplus::GdiplusStartupInput *gdiplusStartupInput;
} ClassData;

Exporter::GUIExporter::GUIExporter() : IO::FileExporter()
{
	ClassData *data = MemAlloc(ClassData, 1);
	this->clsData = data;
	NEW_CLASS(data->gdiplusStartupInput, Gdiplus::GdiplusStartupInput());
	Gdiplus::GdiplusStartup((ULONG_PTR*)&data->gdiplusToken, data->gdiplusStartupInput, NULL);
}

Exporter::GUIExporter::~GUIExporter()
{
	ClassData *data = (ClassData*)this->clsData;
	Gdiplus::GdiplusShutdown(data->gdiplusToken);
	DEL_CLASS((Gdiplus::GdiplusStartupInput*)data->gdiplusStartupInput);
	MemFree(data);
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
	case Media::PF_LE_R5G6B5:
		return IO::FileExporter::SupportType::NormalStream;
	case Media::PF_PAL_8:
		return IO::FileExporter::SupportType::NormalStream;
	case Media::PF_PAL_4:
		return IO::FileExporter::SupportType::NormalStream;
	case Media::PF_PAL_1:
		return IO::FileExporter::SupportType::NormalStream;
	case Media::PF_LE_R5G5B5:
	case Media::PF_LE_B16G16R16A16:
	case Media::PF_LE_B16G16R16:
	case Media::PF_LE_W16:
	case Media::PF_LE_A2B10G10R10:
	case Media::PF_PAL_2:
	case Media::PF_PAL_W1:
	case Media::PF_PAL_W2:
	case Media::PF_PAL_W4:
	case Media::PF_PAL_W8:
	case Media::PF_W8A8:
	case Media::PF_LE_W16A16:
	case Media::PF_LE_FB32G32R32A32:
	case Media::PF_LE_FB32G32R32:
	case Media::PF_LE_FW32A32:
	case Media::PF_LE_FW32:
	case Media::PF_R8G8B8A8:
	case Media::PF_R8G8B8:
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
	*relBuff = 0;
	Media::ImageList *imgList;
	if (pobj == 0)
		return 0;
	if (pobj->GetParserType() != IO::ParserType::ImageList)
		return 0;
	imgList = (Media::ImageList*)pobj;
	if (imgList->GetCount() != 1)
		return 0;
	Media::Image *img = imgList->GetImage(0, 0);
	if (img->info.fourcc != 0)
		return 0;
	Gdiplus::Bitmap *gimg;
	Gdiplus::Rect rc(0, 0, (INT)img->info.dispSize.x, (INT)img->info.dispSize.y);
	Gdiplus::BitmapData bd;
	Gdiplus::ColorPalette *pal;
	switch (img->info.pf)
	{
	case Media::PF_B8G8R8A8:
		NEW_CLASS(gimg, Gdiplus::Bitmap((INT)img->info.dispSize.x, (INT)img->info.dispSize.y, PixelFormat32bppARGB));
		gimg->SetResolution((Gdiplus::REAL)img->info.hdpi, (Gdiplus::REAL)img->info.vdpi);
		if (img->info.atype == Media::AT_NO_ALPHA)
		{
			if (gimg->LockBits(&rc, Gdiplus::ImageLockModeWrite, PixelFormat32bppRGB, &bd) == Gdiplus::Ok)
			{
				img->GetImageData((UInt8*)bd.Scan0, 0, 0, img->info.dispSize.x, img->info.dispSize.y, (UOSInt)(OSInt)bd.Stride, false, Media::RotateType::None);
				gimg->UnlockBits(&bd);
			}
		}
		else
		{
			if (gimg->LockBits(&rc, Gdiplus::ImageLockModeWrite, PixelFormat32bppARGB, &bd) == Gdiplus::Ok)
			{
				img->GetImageData((UInt8*)bd.Scan0, 0, 0, img->info.dispSize.x, img->info.dispSize.y, (UOSInt)(OSInt)bd.Stride, false, Media::RotateType::None);
				gimg->UnlockBits(&bd);
			}
		}
		return gimg;
	case Media::PF_B8G8R8:
		NEW_CLASS(gimg, Gdiplus::Bitmap((INT)img->info.dispSize.x, (INT)img->info.dispSize.y, PixelFormat24bppRGB));
		gimg->SetResolution((Gdiplus::REAL)img->info.hdpi, (Gdiplus::REAL)img->info.vdpi);
		if (gimg->LockBits(&rc, Gdiplus::ImageLockModeWrite, PixelFormat24bppRGB, &bd) == Gdiplus::Ok)
		{
			img->GetImageData((UInt8*)bd.Scan0, 0, 0, img->info.dispSize.x, img->info.dispSize.y, (UOSInt)(OSInt)bd.Stride, false, Media::RotateType::None);
			gimg->UnlockBits(&bd);
		}
		return gimg;
	case Media::PF_LE_R5G6B5:
		NEW_CLASS(gimg, Gdiplus::Bitmap((INT)img->info.dispSize.x, (INT)img->info.dispSize.y, PixelFormat16bppRGB565));
		gimg->SetResolution((Gdiplus::REAL)img->info.hdpi, (Gdiplus::REAL)img->info.vdpi);
		if (gimg->LockBits(&rc, Gdiplus::ImageLockModeWrite, PixelFormat16bppRGB565, &bd) == Gdiplus::Ok)
		{
			img->GetImageData((UInt8*)bd.Scan0, 0, 0, img->info.dispSize.x, img->info.dispSize.y, (UOSInt)(OSInt)bd.Stride, false, Media::RotateType::None);
			gimg->UnlockBits(&bd);
		}
		return gimg;
	case Media::PF_PAL_8:
		NEW_CLASS(gimg, Gdiplus::Bitmap((INT)img->info.dispSize.x, (INT)img->info.dispSize.y, PixelFormat8bppIndexed));
		gimg->SetResolution((Gdiplus::REAL)img->info.hdpi, (Gdiplus::REAL)img->info.vdpi);
		if (gimg->LockBits(&rc, Gdiplus::ImageLockModeWrite, PixelFormat8bppIndexed, &bd) == Gdiplus::Ok)
		{
			img->GetImageData((UInt8*)bd.Scan0, 0, 0, img->info.dispSize.x, img->info.dispSize.y, (UOSInt)(OSInt)bd.Stride, false, Media::RotateType::None);
			gimg->UnlockBits(&bd);
		}

		pal = (Gdiplus::ColorPalette*)MAlloc(1032);
		pal->Flags = Gdiplus::PaletteFlagsHasAlpha;
		pal->Count = 256;
		MemCopyNO(pal->Entries, img->pal, 1024);
		gimg->SetPalette(pal);
		MemFree(pal);

		return gimg;
	case Media::PF_PAL_4:
		NEW_CLASS(gimg, Gdiplus::Bitmap((INT)img->info.dispSize.x, (INT)img->info.dispSize.y, PixelFormat4bppIndexed));
		gimg->SetResolution((Gdiplus::REAL)img->info.hdpi, (Gdiplus::REAL)img->info.vdpi);

		if (gimg->LockBits(&rc, Gdiplus::ImageLockModeWrite, PixelFormat4bppIndexed, &bd) == Gdiplus::Ok)
		{
			img->GetImageData((UInt8*)bd.Scan0, 0, 0, img->info.dispSize.x, img->info.dispSize.y, (UOSInt)(OSInt)bd.Stride, false, Media::RotateType::None);
			gimg->UnlockBits(&bd);
		}

		pal = (Gdiplus::ColorPalette*)MAlloc(72);
		pal->Flags = Gdiplus::PaletteFlagsHasAlpha;
		pal->Count = 16;
		MemCopyNO(pal->Entries, img->pal, 64);
		gimg->SetPalette(pal);
		MemFree(pal);

		return gimg;
	case Media::PF_PAL_1:
		NEW_CLASS(gimg, Gdiplus::Bitmap((INT)img->info.dispSize.x, (INT)img->info.dispSize.y, PixelFormat1bppIndexed));
		gimg->SetResolution((Gdiplus::REAL)img->info.hdpi, (Gdiplus::REAL)img->info.vdpi);

		if (gimg->LockBits(&rc, Gdiplus::ImageLockModeWrite, PixelFormat1bppIndexed, &bd) == Gdiplus::Ok)
		{
			img->GetImageData((UInt8*)bd.Scan0, 0, 0, img->info.dispSize.x, img->info.dispSize.y, (UOSInt)(OSInt)bd.Stride, false, Media::RotateType::None);
			gimg->UnlockBits(&bd);
		}

		pal = (Gdiplus::ColorPalette*)MAlloc(16);
		pal->Flags = Gdiplus::PaletteFlagsHasAlpha;
		pal->Count = 2;
		MemCopyNO(pal->Entries, img->pal, 8);
		gimg->SetPalette(pal);
		MemFree(pal);

		return gimg;
	case Media::PF_LE_R5G5B5:
	case Media::PF_LE_B16G16R16A16:
	case Media::PF_LE_B16G16R16:
	case Media::PF_LE_W16:
	case Media::PF_LE_A2B10G10R10:
	case Media::PF_PAL_2:
	case Media::PF_PAL_W1:
	case Media::PF_PAL_W2:
	case Media::PF_PAL_W4:
	case Media::PF_PAL_W8:
	case Media::PF_W8A8:
	case Media::PF_LE_W16A16:
	case Media::PF_LE_FB32G32R32A32:
	case Media::PF_LE_FB32G32R32:
	case Media::PF_LE_FW32A32:
	case Media::PF_LE_FW32:
	case Media::PF_R8G8B8A8:
	case Media::PF_R8G8B8:
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
	UINT  num = 0;          // number of image encoders
	UINT  size = 0;         // size of the image encoder array in bytes

	Gdiplus::ImageCodecInfo* pImageCodecInfo = NULL;

	Gdiplus::GetImageEncodersSize(&num, &size);
	if(size == 0)
		return -1;  // Failure

	pImageCodecInfo = (Gdiplus::ImageCodecInfo*)(MAlloc(size));
	if(pImageCodecInfo == NULL)
		return -1;  // Failure

	GetImageEncoders(num, size, pImageCodecInfo);

	for(UINT j = 0; j < num; ++j)
	{
		if(Text::StrEquals(pImageCodecInfo[j].MimeType, format))
		{
			*(CLSID*)clsid = pImageCodecInfo[j].Clsid;
			MemFree(pImageCodecInfo);
			return (Int32)j;  // Success
		}
	}

	MemFree(pImageCodecInfo);
	return -1;  // Failure
}

#endif