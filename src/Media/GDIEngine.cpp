//require gdiplus.lib msimg32.lib
#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ByteTool.h"
#include "IO/Stream.h"
#include "IO/FileStream.h"
#include "Manage/HiResClock.h"
#include "Math/Math.h"
#include "Math/GeometryTool.h"
#include "Media/DrawEngine.h"
#include "Media/GDIEngine.h"
#include "Media/ImageCopyC.h"
#include "Media/ImageUtil.h"
#include "Media/StaticImage.h"
#include "Media/ImageTo8Bit.h"
#include "Sync/Event.h"
#include "Sync/MutexUsage.h"
#include "Sync/SimpleThread.h"
#include "Text/Encoding.h"
#include "Text/MyString.h"
#include "Text/MyStringW.h"

#if defined(WIN32_LEAN_AND_MEAN)
#undef WIN32_LEAN_AND_MEAN
#endif

#include <stdio.h>
#include "WinDebug.h"

#include <windows.h>

#if (defined(_MSC_VER) || defined(__CYGWIN__) || defined(__MINGW32__)) && !defined(_WIN32_WCE)
#include <gdiplus.h>
#if !defined(__CYGWIN__)
#include <share.h>
#endif
#include "Win32/COMStream.h"
#define HAS_GDIPLUS
#endif
#ifdef _WIN32_WCE
#define TextOutW(hdc, x, y, s, len) ExtTextOut(hdc, x, y, 0, 0, s, len, 0)
#endif

#undef LoadImage

//#define PI 3.1415926535898

#ifdef HAS_GDIPLUS
int GetEncoderClsid(const WCHAR* format, CLSID* pClsid)
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
		if( Text::StrEquals(pImageCodecInfo[j].MimeType, format) )
		{
			*pClsid = pImageCodecInfo[j].Clsid;
			MemFree(pImageCodecInfo);
			return (INT)j;  // Success
		}
	}

	MemFree(pImageCodecInfo);
	return -1;  // Failure
}
#endif

COLORREF GDIEGetCol(UInt32 col)
{
	return ((col & 0xff) << 16) | (col & 0xff00) | ((col >> 16) & 0xff);
}

Media::GDIEngine::GDIEngine() : iab(0, true)
{
#ifdef HAS_GDIPLUS
	NEW_CLASS(gdiplusStartupInput, Gdiplus::GdiplusStartupInput());
	Gdiplus::GdiplusStartup((ULONG_PTR*)&gdiplusToken, (Gdiplus::GdiplusStartupInput*)gdiplusStartupInput, NULL);
#endif
	this->hdc = GetDC(0);
	this->hdcScreen = CreateDC(TEXT("DISPLAY"), NULL, NULL, NULL); 
	this->hpenBlack = CreatePen(PS_SOLID, 1, 0);
	this->hbrushWhite = CreateSolidBrush(0xffffff);
}

Media::GDIEngine::~GDIEngine()
{
	DeleteDC((HDC)this->hdcScreen);
	ReleaseDC(0, (HDC)this->hdc);
	DeleteObject(this->hpenBlack);
	DeleteObject(this->hbrushWhite);

#ifdef HAS_GDIPLUS
	Gdiplus::GdiplusShutdown(gdiplusToken);
	DEL_CLASS((Gdiplus::GdiplusStartupInput*)gdiplusStartupInput);
#endif
}

Optional<Media::DrawImage> Media::GDIEngine::CreateImage32(Math::Size2D<UOSInt> size, Media::AlphaType atype)
{
	BITMAPINFO bInfo;
	void *bmpBits;
	HBITMAP hBmp;
	ZeroMemory(&bInfo, sizeof(bInfo));
	bInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bInfo.bmiHeader.biWidth = (LONG)size.x;
	bInfo.bmiHeader.biHeight = (LONG)size.y;
	bInfo.bmiHeader.biBitCount = 32;
	bInfo.bmiHeader.biPlanes = 1;
	bInfo.bmiHeader.biXPelsPerMeter = 96;
	bInfo.bmiHeader.biYPelsPerMeter = 96;

	hBmp = CreateDIBSection((HDC)this->hdc, &bInfo, 0, &bmpBits, 0, 0);
	if (hBmp)
	{
		GDIImage *img;
		HDC hdcBmp;
		Int32 i = 10;
		Sync::MutexUsage mutUsage(this->gdiMut);
		while ((hdcBmp = CreateCompatibleDC((HDC)this->hdcScreen)) == 0)
		{
			if (i-- <= 0)
				break;
			mutUsage.EndUse();
			Sync::SimpleThread::Sleep(10);
			mutUsage.BeginUse();
		}
		mutUsage.EndUse();
		if (hdcBmp)
		{
			SelectObject(hdcBmp, hBmp);
			NEW_CLASS(img, GDIImage(this, Math::Coord2D<OSInt>(0, 0), size, 32, (void *)hBmp, bmpBits, (void *)hdcBmp, atype));
		}
		else
		{
			img = 0;
		}
		return img;
	}
	return 0;
}

Optional<Media::GDIImage> Media::GDIEngine::CreateImage24(Math::Size2D<UOSInt> size)
{
	BITMAPINFO bInfo;
	void *bmpBits;
	HBITMAP hBmp;
	ZeroMemory(&bInfo, sizeof(bInfo));
	bInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bInfo.bmiHeader.biWidth = (LONG)size.x;
	bInfo.bmiHeader.biHeight = (LONG)size.y;
	bInfo.bmiHeader.biBitCount = 24;
	bInfo.bmiHeader.biPlanes = 1;
	bInfo.bmiHeader.biXPelsPerMeter = 96;
	bInfo.bmiHeader.biYPelsPerMeter = 96;


	hBmp = CreateDIBSection((HDC)this->hdc, &bInfo, 0, &bmpBits, 0, 0);
	if (hBmp)
	{
		GDIImage *img;
		HDC hdcBmp;
		Int32 i = 10;
		while ((hdcBmp = CreateCompatibleDC((HDC)this->hdcScreen)) == 0)
		{
			if (i-- <= 0)
				break;
			Sync::SimpleThread::Sleep(10);
		}
		if (hdcBmp)
		{
			SelectObject(hdcBmp, hBmp);
			NEW_CLASS(img, GDIImage(this, Math::Coord2D<OSInt>(0, 0), size, 24, (void *)hBmp, bmpBits, (void *)hdcBmp, Media::AT_NO_ALPHA));
		}
		else
		{
			img = 0;
		}
		return img;
	}
	return 0;
}

NN<Media::DrawImage> Media::GDIEngine::CreateImageScn(void *hdc, OSInt left, OSInt top, OSInt right, OSInt bottom)
{
	NN<GDIImage> img;
	NEW_CLASSNN(img, GDIImage(this, Math::Coord2D<OSInt>(left, top), Math::Size2D<UOSInt>((UOSInt)(right - left), (UOSInt)(bottom - top)), 32, 0, 0, hdc, Media::AT_NO_ALPHA));
	return img;
}

Optional<Media::DrawImage> Media::GDIEngine::LoadImage(Text::CStringNN fileName)
{
	NN<IO::FileStream> fstm;

	NEW_CLASSNN(fstm, IO::FileStream(fileName, IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal));
	if (fstm->IsError())
	{
		fstm.Delete();
		return 0;
	}
	Optional<DrawImage> img = LoadImageStream(fstm);

	fstm.Delete();
	return img;
}


Optional<Media::DrawImage> Media::GDIEngine::LoadImageStream(NN<IO::SeekableStream> fstm)
{
	UInt8 hdr[54];
	UInt8 pal[1024];
	BITMAPINFO bmi;
	DrawImage *img = 0;

	fstm->Read(BYTEARR(hdr));
	if (*(Int16*)hdr != *(Int16*)"BM")
	{
#ifdef HAS_GDIPLUS
		Gdiplus::Bitmap *gimg;
		Win32::COMStream *comStm;
		fstm->SeekFromBeginning(0);
		NEW_CLASS(comStm, Win32::COMStream(fstm));
		gimg = Gdiplus::Bitmap::FromStream(comStm, false);

		if (gimg == 0)
		{
			DEL_CLASS(comStm);
			return 0;
		}
		Gdiplus::BitmapData bmpd;
		Gdiplus::Status stat;
		Gdiplus::Rect rect(0, 0, (INT)gimg->GetWidth(), (INT)gimg->GetHeight());
		if ((stat = gimg->LockBits(&rect, Gdiplus::ImageLockModeRead, PixelFormat32bppARGB, &bmpd)) == Gdiplus::Ok)
		{
			void *pBits;
			HBITMAP hBmp;
			HDC hdcBmp;
			bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
			bmi.bmiHeader.biWidth = (LONG)bmpd.Width;
			bmi.bmiHeader.biHeight = (LONG)bmpd.Height;
			bmi.bmiHeader.biPlanes = 1;
			bmi.bmiHeader.biBitCount = 32;
			bmi.bmiHeader.biCompression = BI_RGB;
			bmi.bmiHeader.biSizeImage = 0;
			bmi.bmiHeader.biXPelsPerMeter = 96;
			bmi.bmiHeader.biYPelsPerMeter = 96;
			bmi.bmiHeader.biClrUsed = 0;
			bmi.bmiHeader.biClrImportant = 0;
			if ((hBmp = CreateDIBSection((HDC)this->hdc, &bmi, 0, &pBits, 0, 0)) != 0)
			{
				UInt8 *imgSrc = (UInt8*)bmpd.Scan0;
				UInt8 *imgDest = (UInt8*)pBits;
				UInt32 dbpl = bmpd.Width << 2;
				Int32 sbpl = bmpd.Stride;
				imgDest += bmpd.Height * dbpl;
				UInt32 i = bmpd.Height;
				while (i-- > 0)
				{
					imgDest -= dbpl;
					MemCopyNO(imgDest, imgSrc, dbpl);
					imgSrc += sbpl;
				}
				
				Sync::MutexUsage mutUsage(this->gdiMut);
				hdcBmp = CreateCompatibleDC((HDC)this->hdcScreen);
				mutUsage.EndUse();
				if (hdcBmp)
				{
					SelectObject(hdcBmp, hBmp);
					NEW_CLASS(img, GDIImage(this, Math::Coord2D<OSInt>(0, 0), Math::Size2D<UOSInt>((UOSInt)bmi.bmiHeader.biWidth, (UOSInt)bmi.bmiHeader.biHeight), 32, hBmp, pBits, (void*)hdcBmp, Media::AT_ALPHA));
				}
				else
				{
				}
			}
			gimg->UnlockBits(&bmpd);
		}
		delete gimg;
		DEL_CLASS(comStm);
		return img;
#else
		return 0;
#endif
	}
	bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	Int32 bpp;
	if (*(Int32*)&hdr[14] == 40)
	{
		bmi.bmiHeader.biWidth = *(Int32*)&hdr[18];
		bmi.bmiHeader.biHeight = *(Int32*)&hdr[22];
		bpp = *(Int16*)&hdr[28];
	}
	else if (*(Int32*)&hdr[14] == 12)
	{
		bmi.bmiHeader.biWidth = *(Int16*)&hdr[18];
		bmi.bmiHeader.biHeight = *(Int16*)&hdr[20];
		bpp = *(Int16*)&hdr[24];
		fstm->SeekFromBeginning(26);
	}
	else
	{
		bpp = 0;
	}
	void *pBits;
	HBITMAP hBmp;
	HDC hdcBmp;
	bmi.bmiHeader.biPlanes = 1;
	bmi.bmiHeader.biBitCount = 32;
	bmi.bmiHeader.biCompression = BI_RGB;
	bmi.bmiHeader.biSizeImage = 0;
	bmi.bmiHeader.biXPelsPerMeter = 96;
	bmi.bmiHeader.biYPelsPerMeter = 96;
	bmi.bmiHeader.biClrUsed = 0;
	bmi.bmiHeader.biClrImportant = 0;
	if ((hBmp = CreateDIBSection((HDC)this->hdc, &bmi, 0, &pBits, 0, 0)) != 0)
	{
		UInt8 *buff;
		UInt32 buffSize;
		Int32 lineW;
		Int32 i;
		Int32 j;
		UInt8 *psrc;
		UInt8 *psrc2;
		UInt32 *pdest;

		switch (bpp)
		{
		case 8:
			{
				fstm->Read(BYTEARR(pal));
				fstm->SeekFromBeginning(ReadUInt32(&hdr[10]));
				lineW = bmi.bmiHeader.biWidth;
				if (lineW & 3)
				{
					lineW = lineW + 4 - (lineW & 3);
				}
				buff = MemAlloc(UInt8, buffSize = (UInt32)(lineW * bmi.bmiHeader.biHeight));
				fstm->Read(Data::ByteArray(buff, buffSize));
				psrc = (UInt8*)pBits;
				pdest = (UInt32*)buff;
				j = bmi.bmiHeader.biHeight;
				while (j-- > 0)
				{
					psrc2 = psrc;
					i = bmi.bmiHeader.biWidth;
					while (i-- > 0)
					{
						*pdest++ = ((UInt32*)pal)[*psrc2++] | 0xff000000;
					}
					psrc += lineW;
				}
				MemFree(buff);

				Sync::MutexUsage mutUsage(this->gdiMut);
				hdcBmp = CreateCompatibleDC((HDC)this->hdcScreen);
				mutUsage.EndUse();
				if (hdcBmp)
				{
					SelectObject(hdcBmp, hBmp);
					NEW_CLASS(img, GDIImage(this, Math::Coord2D<OSInt>(0, 0), Math::Size2D<UOSInt>(bmi.bmiHeader.biWidth, bmi.bmiHeader.biHeight), 32, hBmp, pBits, (void*)hdcBmp, Media::AT_NO_ALPHA));
				}
				else
				{
				}
			}
			break;
		case 16:
			{
				fstm->SeekFromBeginning(ReadUInt32(&hdr[10]));
				buff = MemAlloc(UInt8, buffSize = ((bmi.bmiHeader.biWidth * bmi.bmiHeader.biHeight) << 1));
				fstm->Read(Data::ByteArray(buff, buffSize));
				//////////////////////////////////////////////////////////////////////
				MemFree(buff);
				Sync::MutexUsage mutUsage(this->gdiMut);
				hdcBmp = CreateCompatibleDC((HDC)this->hdcScreen);
				mutUsage.EndUse();
				if (hdcBmp)
				{
					SelectObject(hdcBmp, hBmp);
					NEW_CLASS(img, GDIImage(this, Math::Coord2D<OSInt>(0, 0), Math::Size2D<UOSInt>(bmi.bmiHeader.biWidth, bmi.bmiHeader.biHeight), 32, hBmp, pBits, (void*)hdcBmp, Media::AT_NO_ALPHA));
				}
				else
				{
				}
			}
			break;
		case 24:
			{
				fstm->SeekFromBeginning(ReadUInt32(&hdr[10]));
				lineW = bmi.bmiHeader.biWidth * 3;
				if (lineW & 3)
				{
					lineW = lineW + 4 - (lineW & 3);
				}
				buff = MemAlloc(UInt8, buffSize = (lineW * bmi.bmiHeader.biHeight));
				fstm->Read(Data::ByteArray(buff, buffSize));
				psrc = (UInt8*)pBits;
				pdest = (UInt32*)buff;
				j = bmi.bmiHeader.biHeight;
				while (j-- > 0)
				{
					psrc2 = psrc;
					i = bmi.bmiHeader.biWidth;
					while (i-- > 0)
					{
						*pdest++ = psrc2[0] | ((UInt32)psrc2[1] << 8) | ((UInt32)psrc2[2] << 16) | 0xff000000;
					}
					psrc += lineW;
				}
				MemFree(buff);

				Sync::MutexUsage mutUsage(this->gdiMut);
				hdcBmp = CreateCompatibleDC((HDC)this->hdcScreen);
				mutUsage.EndUse();
				if (hdcBmp)
				{
					SelectObject(hdcBmp, hBmp);
					NEW_CLASS(img, GDIImage(this, Math::Coord2D<OSInt>(0, 0), Math::Size2D<UOSInt>((ULONG)bmi.bmiHeader.biWidth, (ULONG)bmi.bmiHeader.biHeight), 32, hBmp, pBits, (void*)hdcBmp, Media::AT_NO_ALPHA));
				}
				else
				{
				}
			}
			break;
		case 32:
			{
				fstm->SeekFromBeginning(ReadUInt32(&hdr[10]));
				fstm->Read(Data::ByteArray((UInt8*)pBits, (bmi.bmiHeader.biWidth * bmi.bmiHeader.biHeight) << 2));

				Sync::MutexUsage mutUsage(this->gdiMut);
				hdcBmp = CreateCompatibleDC((HDC)this->hdcScreen);
				mutUsage.EndUse();
				if (hdcBmp)
				{
					SelectObject(hdcBmp, hBmp);
					NEW_CLASS(img, GDIImage(this, Math::Coord2D<OSInt>(0, 0), Math::Size2D<UOSInt>(bmi.bmiHeader.biWidth, bmi.bmiHeader.biHeight), 32, hBmp, pBits, (void*)hdcBmp, Media::AT_ALPHA));
				}
				else
				{
				}
			}
			break;
		default:
			DeleteObject(hBmp);
			img = 0;
			break;
		}
	}
	else
	{
		img = 0;
	}
	return img;
}

Optional<Media::DrawImage> Media::GDIEngine::ConvImage(NN<Media::RasterImage> img)
{
	if (img->info.fourcc != 0)
	{
		return 0; 
	}
	NN<Media::GDIImage> gimg;
	if (!Optional<Media::GDIImage>::ConvertFrom(CreateImage32(img->info.dispSize, img->info.atype)).SetTo(gimg))
		return 0;
	gimg->SetHDPI(img->info.hdpi);
	gimg->SetVDPI(img->info.vdpi);
	if (img->GetImageType() == Media::RasterImage::ImageType::Static)
	{
		NN<Media::StaticImage> simg = NN<Media::StaticImage>::ConvertFrom(img);
		if (simg->ToB8G8R8A8())
		{
			UnsafeArray<UInt8> sptr = simg->data;
			UInt8 *dptr = (UInt8*)gimg->bmpBits;
			OSInt sbpl = (OSInt)simg->info.storeSize.x << 2;
			OSInt dbpl = (OSInt)simg->info.dispSize.x << 2;
			ImageCopy_ImgCopy(sptr.Ptr(), dptr + ((UOSInt)dbpl * (simg->info.dispSize.y - 1)), simg->info.dispSize.x << 2, simg->info.dispSize.y, sbpl, -dbpl);
		}
	}
	else
	{
		NN<Media::StaticImage> simg = img->CreateStaticImage();
		if (simg->ToB8G8R8A8())
		{
			UnsafeArray<UInt8> sptr = simg->data;
			UInt8 *dptr = (UInt8*)gimg->bmpBits;
			OSInt sbpl = (OSInt)simg->info.storeSize.x << 2;
			OSInt dbpl = (OSInt)simg->info.dispSize.x << 2;
			ImageCopy_ImgCopy(sptr.Ptr(), dptr + ((UOSInt)dbpl * (simg->info.dispSize.y - 1)), simg->info.dispSize.x << 2, simg->info.dispSize.y, sbpl, -dbpl);
		}
		simg.Delete();
	}
	return gimg;
}

Optional<Media::DrawImage> Media::GDIEngine::CloneImage(NN<Media::DrawImage> img)
{
	NN<Media::GDIImage> newImg;
	if (Optional<Media::GDIImage>::ConvertFrom(this->CreateImage32(img->GetSize(), img->GetAlphaType())).SetTo(newImg))
	{
		newImg->info.Set(((Media::GDIImage*)img.Ptr())->info);
		if (img->GetBitCount() == 32)
		{
			MemCopyNO(newImg->bmpBits, ((Media::GDIImage*)img.Ptr())->bmpBits, newImg->GetWidth() * newImg->GetHeight() * 4);
		}
		else
		{
			newImg->DrawImagePt(img, Math::Coord2DDbl(0, 0));
		}
		return newImg;
	}
	return 0;
}

Bool Media::GDIEngine::DeleteImage(NN<DrawImage> img)
{
	GDIImage *image = (GDIImage *)img.Ptr();
	if (image->hBmp)
	{
		DeleteDC((HDC)image->hdcBmp);
		DeleteObject((HBITMAP)image->hBmp);
		image->bmpBits = 0;
	}
	DEL_CLASS(image);
	return true;
}

void *Media::GDIEngine::GetBlackPen()
{
	return this->hpenBlack;
}

void *Media::GDIEngine::GetWhiteBrush()
{
	return this->hbrushWhite;
}

Media::GDIBrush::GDIBrush(void *hbrush, UInt32 oriColor, DrawImage *img)
{
	this->hbrush = hbrush;
	this->oriColor = oriColor;
	this->color = GDIEGetCol(oriColor);
	this->img = img;
}

Media::GDIBrush::~GDIBrush()
{
}

Media::GDIPen::GDIPen(void *hpen, UInt32 *pattern, UOSInt nPattern, DrawImage *img, Double thick, UInt32 oriColor)
{
	this->hpen = hpen;
	this->pattern = pattern;
	this->nPattern = nPattern;
	this->img = img;
	this->thick = thick;
	this->oriColor = oriColor;
}

Media::GDIPen::~GDIPen()
{
}

Double Media::GDIPen::GetThick()
{
	return this->thick;
}

/*int __stdcall FontFunc(const LOGFONTW FAR* lpelf, const TEXTMETRICW FAR* lpntm, DWORD FontType, LPARAM lParam)
{
	Media::GDIFont *me = (Media::GDIFont *)lParam;
	return 0;
}*/

Media::GDIFont::GDIFont(void *hdc, UnsafeArray<const Char> fontName, Double ptSize, Media::DrawEngine::DrawFontStyle style, NN<DrawImage> img, Int32 codePage)
{
	this->fontName = Text::StrToWCharNew(UnsafeArray<const UTF8Char>::ConvertFrom(fontName));
	this->hdc = hdc;
	this->ptSize = ptSize;
	this->style = style;
	this->img = img;
	this->codePage = codePage;
	LOGFONTW lf;
	ZeroMemory(&lf, sizeof(LOGFONT));
	//this->pxSize = MulDiv(size, GetDeviceCaps((HDC)hdc, LOGPIXELSY), 72);
	this->pxSize = Double2Int32(ptSize * img->GetHDPI() / 72.0);
	lf.lfHeight = -this->pxSize;
	if (style & Media::DrawEngine::DFS_BOLD)
	{
		lf.lfWeight = FW_BOLD;
	}
	else
	{
		lf.lfWeight = FW_NORMAL;
	}
	if (style & Media::DrawEngine::DFS_ITALIC)
	{
		lf.lfItalic = TRUE;
	}
	if (style & Media::DrawEngine::DFS_ANTIALIAS)
	{
		lf.lfQuality = ANTIALIASED_QUALITY;
	}
	switch (codePage)
	{
	case 950:
        lf.lfCharSet = CHINESEBIG5_CHARSET;
		break;
	case 932:
        lf.lfCharSet = GB2312_CHARSET;
		break;
	case 936:
		lf.lfCharSet = SHIFTJIS_CHARSET;
		break;
	default:
		lf.lfCharSet = ANSI_CHARSET;
		break;
	}
	Text::StrConcat(lf.lfFaceName, this->fontName);
	this->hfont = CreateFontIndirectW(&lf);
}

Media::GDIFont::GDIFont(void *hdc, UnsafeArray<const WChar> fontName, Double ptSize, Media::DrawEngine::DrawFontStyle style, NN<DrawImage> img, Int32 codePage)
{
	this->hdc = hdc;
	this->ptSize = ptSize;
	this->style = style;
	this->img = img;
	this->codePage = codePage;
	this->fontName = Text::StrCopyNew(fontName);
	LOGFONTW lf;
//	EnumFontsW((HDC)hdc, 0, FontFunc, (LPARAM)this);
	ZeroMemory(&lf, sizeof(LOGFONT));
	//this->pxSize = MulDiv(size, GetDeviceCaps((HDC)hdc, LOGPIXELSY), 72);
	this->pxSize = Double2Int32(ptSize * img->GetHDPI() / 72.0);
	lf.lfHeight = -this->pxSize;
	if (style & Media::DrawEngine::DFS_BOLD)
	{
		lf.lfWeight = FW_BOLD;
	}
	else
	{
		lf.lfWeight = FW_NORMAL;
	}
	if (style & Media::DrawEngine::DFS_ITALIC)
	{
		lf.lfItalic = TRUE;
	}
	if (style & Media::DrawEngine::DFS_ANTIALIAS)
	{
		lf.lfQuality = ANTIALIASED_QUALITY;
	}
	switch (codePage)
	{
	case 950:
        lf.lfCharSet = CHINESEBIG5_CHARSET;
		break;
	case 932:
        lf.lfCharSet = GB2312_CHARSET;
		break;
	case 936:
		lf.lfCharSet = SHIFTJIS_CHARSET;
		break;
	default:
		lf.lfCharSet = ANSI_CHARSET;
		break;
	}

	Text::StrConcat(lf.lfFaceName, fontName);
	this->hfont = CreateFontIndirectW(&lf);
}

Media::GDIFont::~GDIFont()
{
	Text::StrDelNew(this->fontName);
	DeleteObject((HFONT)this->hfont);
}

UnsafeArray<const WChar> Media::GDIFont::GetNameW()
{
	return this->fontName;
}

Double Media::GDIFont::GetPointSize()
{
	return this->ptSize;
}

Media::DrawEngine::DrawFontStyle Media::GDIFont::GetFontStyle()
{
	return this->style;
}

Int32 Media::GDIFont::GetCodePage()
{
	return this->codePage;
}

Media::GDIImage::GDIImage(GDIEngine *eng, Math::Coord2D<OSInt> tl, Math::Size2D<UOSInt> size, UInt32 bitCount, void *hBmp, void *bmpBits, void *hdcBmp, Media::AlphaType atype) : Media::RasterImage(size, Math::Size2D<UOSInt>(0, 0), 0, bitCount, Media::PixelFormatGetDef(0, bitCount), 0, Media::ColorProfile(), Media::ColorProfile::YUVT_BT601, atype, Media::YCOFST_C_CENTER_LEFT)
{
	this->eng = eng;
	this->tl = tl;
	this->size = size;
	this->bitCount = bitCount;
	this->hBmp = hBmp;
	this->bmpBits = bmpBits;
	this->hdcBmp = hdcBmp;
	this->strAlign = Media::DrawEngine::DRAW_POS_TOPLEFT;
	this->currBrush = 0;
	this->currFont = 0;
	this->currPen = 0;
	SetBkMode((HDC)this->hdcBmp, TRANSPARENT);
}

Media::GDIImage::~GDIImage()
{
}

UOSInt Media::GDIImage::GetWidth() const
{
	return this->size.x;
}

UOSInt Media::GDIImage::GetHeight() const
{
	return this->size.y;
}

Math::Size2D<UOSInt> Media::GDIImage::GetSize() const
{
	return this->size;
}

UInt32 Media::GDIImage::GetBitCount() const
{
	return this->bitCount;	
}

NN<const Media::ColorProfile> Media::GDIImage::GetColorProfile() const
{
	return this->info.color;
}

void Media::GDIImage::SetColorProfile(NN<const Media::ColorProfile> color)
{
	return this->info.color.Set(color);
}

Media::AlphaType Media::GDIImage::GetAlphaType() const
{
	return this->info.atype;
}

void Media::GDIImage::SetAlphaType(Media::AlphaType atype)
{
	this->info.atype = atype;
}

Double Media::GDIImage::GetHDPI() const
{
	return this->info.hdpi;
}

Double Media::GDIImage::GetVDPI() const
{
	return this->info.vdpi;
}

void Media::GDIImage::SetHDPI(Double dpi)
{
	if (dpi > 0)
	{
		this->info.hdpi = dpi;
	}
}

void Media::GDIImage::SetVDPI(Double dpi)
{
	if (dpi > 0)
	{
		this->info.vdpi = dpi;
	}
}

UnsafeArrayOpt<UInt8> Media::GDIImage::GetImgBits(OutParam<Bool> revOrder)
{
	revOrder.Set(true);
	return (UInt8*)this->bmpBits;
}

void Media::GDIImage::GetImgBitsEnd(Bool modified)
{
}

UOSInt Media::GDIImage::GetImgBpl() const
{
	return this->GetDataBpl();
}

Optional<Media::EXIFData> Media::GDIImage::GetEXIF() const
{
	return this->exif;
}

Media::PixelFormat Media::GDIImage::GetPixelFormat() const
{
	return this->info.pf;
}

Bool Media::GDIImage::DrawLine(Double x1, Double y1, Double x2, Double y2, NN<DrawPen> p)
{
	if (this->currPen != p.Ptr())
	{
		GDIPen *pen = (GDIPen*)(this->currPen = p.Ptr());
		SelectObject((HDC)this->hdcBmp, (HPEN)pen->hpen);
	}
	if ((((GDIPen*)p.Ptr())->oriColor & 0xff000000))
	{
		MoveToEx((HDC)this->hdcBmp, Double2Int32(x1), Double2Int32(y1), 0);
		LineTo((HDC)this->hdcBmp, Double2Int32(x2), Double2Int32(y2));
	}
	return true;
}

Bool Media::GDIImage::DrawPolylineI(UnsafeArray<const Int32> points, UOSInt nPoints, NN<DrawPen> p)
{
	if (this->currPen != p.Ptr())
	{
		GDIPen *pen = (GDIPen*)(this->currPen = p.Ptr());
		SelectObject((HDC)this->hdcBmp, (HPEN)pen->hpen);
	}
	if ((((GDIPen*)p.Ptr())->oriColor & 0xff000000))
	{
		Polyline((HDC)this->hdcBmp, (POINT*)points.Ptr(), (int)nPoints);
	}
	return true;
}

Bool Media::GDIImage::DrawPolygonI(UnsafeArray<const Int32> points, UOSInt nPoints, Optional<DrawPen> p, Optional<DrawBrush> b)
{
	NN<DrawPen> nnp;
	if (p.SetTo(nnp))
	{
		if (this->currPen != nnp.Ptr())
		{
			GDIPen *pen = (GDIPen*)(this->currPen = nnp.Ptr());
			SelectObject((HDC)this->hdcBmp, (HPEN)pen->hpen);
		}
	}

	NN<DrawBrush> nnb;
	if (!b.SetTo(nnb))
	{
#ifdef _WIN32_WCE
		if (p)
		{
			Int32 line2[4];
			line2[0] = points[0];
			line2[1] = points[1];
			line2[2] = points[(nPoints << 1) - 2];
			line2[3] = points[(nPoints << 1) - 1];
			PolylineAccel(this->hdcBmp, points.Ptr(), nPoints, left, top, width, height);
			PolylineAccel(this->hdcBmp, line2, 2, left, top, width, height);
		}
#else
		if (p.SetTo(nnp))
		{
			MoveToEx((HDC)this->hdcBmp, points[(nPoints << 1) - 2], points[(nPoints << 1) - 1], 0);
			PolylineTo((HDC)this->hdcBmp, (POINT*)points.Ptr(), (DWORD)nPoints);
		}
#endif
	}
	else
	{
		if (this->currBrush != nnb.Ptr())
		{
			this->currBrush = nnb.Ptr();
			NN<GDIBrush> brush = NN<GDIBrush>::ConvertFrom(nnb);
			SelectObject((HDC)this->hdcBmp, (HBRUSH)brush->hbrush);
		}
		
		Polygon((HDC)this->hdcBmp, (POINT*)points.Ptr(), (int)nPoints);
	}

	return true;
}

Bool Media::GDIImage::DrawPolyPolygonI(UnsafeArray<const Int32> points, UnsafeArray<const UInt32> pointCnt, UOSInt nPointCnt, Optional<DrawPen> p, Optional<DrawBrush> b)
{
	UOSInt i;
	UInt32 j;
	Double penWidth = 1;
	j = 0;
	i = nPointCnt;
	while (i-- > 0)
	{
		j += pointCnt[i];
	}

//	if (j == 1709)
//		return true;

	NN<DrawPen> nnp;
	if (p.SetTo(nnp))
	{
		if (this->currPen != nnp.Ptr())
		{
			GDIPen *pen = (GDIPen*)(this->currPen = nnp.Ptr());
			SelectObject((HDC)this->hdcBmp, (HPEN)pen->hpen);
		}
		if ((penWidth = ((Media::GDIPen*)nnp.Ptr())->thick) < 1)
			penWidth = 1;
	}

	NN<DrawBrush> nnb;
	if (!b.SetTo(nnb))
	{
		UnsafeArray<const Int32> pts = points;
		if (p.SetTo(nnp))
		{
			i = 0;
			while (i < nPointCnt)
			{
				j = pointCnt[i];

#ifdef _WIN32_WCE
				Int32 line2[4];
				line2[0] = points[0];
				line2[1] = points[1];
				line2[2] = points[(j << 1) - 2];
				line2[3] = points[(j << 1) - 1];
				PolylineAccel(this->hdcBmp, points.Ptr(), j, left, top, width, height);
				PolylineAccel(this->hdcBmp, line2, 2, left, top, width, height);
#else
				MoveToEx((HDC)this->hdcBmp, points[(j << 1) - 2], points[(j << 1) - 1], 0);
				PolylineTo((HDC)this->hdcBmp, (POINT*)points.Ptr(), j);
#endif

				points += j << 1;
				i++;
			}
		}
		points = pts;
	}
	else
	{
		if (this->info.atype == Media::AT_NO_ALPHA && (NN<GDIBrush>::ConvertFrom(nnb)->oriColor & 0xff000000) == 0xff000000)
		{
			if (this->currBrush != nnb.Ptr())
			{
				this->currBrush = nnb.Ptr();
				NN<GDIBrush> brush = NN<GDIBrush>::ConvertFrom(nnb);
				SelectObject((HDC)this->hdcBmp, (HBRUSH)brush->hbrush);
			}
#ifdef _WIN32_WCE
			i = 0;
			while (i < nPointCnt)
			{
				j = pointCnt[i];

				PolygonAccel(this->hdcBmp, points.Ptr(), j, this->tl.x, this->tl.y, this->size.x, this->size.y, Double2Int32(penWidth));

				points += j << 1;
				i++;
			}
#else
			PolyPolygonAccel((HDC)this->hdcBmp, points.Ptr(), pointCnt.Ptr(), nPointCnt, this->tl.x, this->tl.y, (OSInt)this->size.x, (OSInt)this->size.y, Double2Int32(penWidth));
#endif
		}
		else if (this->bmpBits && (NN<GDIBrush>::ConvertFrom(nnb)->oriColor & 0xff000000) == 0xff000000)
		{
			UInt32 bCol = NN<GDIBrush>::ConvertFrom(nnb)->oriColor;
			BITMAPINFOHEADER bmih;
			bmih.biSize = sizeof(bmih);
			bmih.biWidth = (LONG)this->info.dispSize.x;
			bmih.biHeight = (LONG)this->info.dispSize.y;
			bmih.biPlanes = 1;
			bmih.biBitCount = 32;
			bmih.biCompression = BI_RGB;
			bmih.biSizeImage = 0;
			bmih.biXPelsPerMeter = 300;
			bmih.biYPelsPerMeter = 300;
			bmih.biClrUsed = 0;
			bmih.biClrImportant = 0;
			void *pbits;
			HDC hdcBmp = CreateCompatibleDC((HDC)this->hdcBmp);
			HBITMAP hBmp = CreateDIBSection(hdcBmp, (BITMAPINFO*)&bmih, 0, &pbits, 0, 0);
			if (hBmp)
			{
				MemClear(pbits, this->info.dispSize.CalcArea() * 4);
				SelectObject(hdcBmp, hBmp);
				HBRUSH hbr = CreateSolidBrush(0xffffff);
				SelectObject(hdcBmp, hbr);
				PolyPolygon(hdcBmp, (POINT*)points.Ptr(), (const Int32*)pointCnt.Ptr(), (Int32)nPointCnt);
				DeleteObject(hbr);

				UInt32 *sptr = (UInt32*)pbits;
				UInt32 *dptr = (UInt32*)this->bmpBits;
				OSInt cnt = this->info.dispSize.CalcArea();
				while (cnt-- > 0)
				{
					if (*sptr)
					{
						*dptr = bCol;
					}
					sptr++;
					dptr++;
				}
				if (p.SetTo(nnp))
				{
					PolyPolyline((HDC)this->hdcBmp, (POINT*)points.Ptr(), (const DWORD*)pointCnt.Ptr(), (Int32)nPointCnt);
				}

				DeleteObject(hBmp);
			}
			DeleteDC(hdcBmp);
		}
		else
		{
			UInt32 bCol = NN<GDIBrush>::ConvertFrom(nnb)->oriColor;
			BITMAPINFOHEADER bmih;
			bmih.biSize = sizeof(bmih);
			bmih.biWidth = (LONG)this->info.dispSize.x;
			bmih.biHeight = (LONG)this->info.dispSize.y;
			bmih.biPlanes = 1;
			bmih.biBitCount = 32;
			bmih.biCompression = BI_RGB;
			bmih.biSizeImage = 0;
			bmih.biXPelsPerMeter = 300;
			bmih.biYPelsPerMeter = 300;
			bmih.biClrUsed = 0;
			bmih.biClrImportant = 0;
			void *pbits;
			HDC hdcBmp = CreateCompatibleDC((HDC)this->hdcBmp);
			HBITMAP hBmp = CreateDIBSection(hdcBmp, (BITMAPINFO*)&bmih, 0, &pbits, 0, 0);
			if (hBmp)
			{
				MemClear(pbits, this->info.dispSize.CalcArea() * 4);
				SelectObject(hdcBmp, hBmp);
				HBRUSH hbr = CreateSolidBrush(0xffffff);
				SelectObject(hdcBmp, hbr);
				PolyPolygon(hdcBmp, (POINT*)points.Ptr(), (const Int32*)pointCnt.Ptr(), (Int32)nPointCnt);
				DeleteObject(hbr);


				UInt32 *ptr = (UInt32*)pbits;
				OSInt cnt = this->info.dispSize.CalcArea();
				while (cnt-- > 0)
				{
					if (*ptr)
					{
						*ptr = bCol;
					}
					ptr++;
				}
				BLENDFUNCTION bf;
				bf.BlendOp = 0;
				bf.BlendFlags = 0;
				bf.SourceConstantAlpha = 255;
				bf.AlphaFormat = AC_SRC_ALPHA;
				AlphaBlend((HDC)this->hdcBmp, 0, 0, (int)this->info.dispSize.x, (int)this->info.dispSize.y, hdcBmp, 0, 0, (int)this->info.dispSize.x, (int)this->info.dispSize.y, bf);

				if (p.SetTo(nnp))
				{
					PolyPolyline((HDC)this->hdcBmp, (POINT*)points.Ptr(), (const DWORD*)pointCnt.Ptr(), (Int32)nPointCnt);
				}

				DeleteObject(hBmp);
			}
			DeleteDC(hdcBmp);
		}
	}

	return true;
}

Bool Media::GDIImage::DrawPolyline(UnsafeArray<const Math::Coord2DDbl> points, UOSInt nPoints, NN<DrawPen> p)
{
	UOSInt i = nPoints;
	Bool ret;
	Int32 *ipts = MemAlloc(Int32, i << 1);
	while (i-- > 0)
	{
		ipts[i << 1] = Double2Int32(points[i].x);
		ipts[(i << 1) + 1] = Double2Int32(points[i].y);
	}
	ret = DrawPolylineI(ipts, nPoints, p);
	MemFree(ipts);
	return ret;
}

Bool Media::GDIImage::DrawPolygon(UnsafeArray<const Math::Coord2DDbl> points, UOSInt nPoints, Optional<DrawPen> p, Optional<DrawBrush> b)
{
	UOSInt i = nPoints;
	Bool ret;
	Int32 *ipts = MemAlloc(Int32, i << 1);
	while (i-- > 0)
	{
		ipts[i << 1] = Double2Int32(points[i].x);
		ipts[(i << 1) + 1] = Double2Int32(points[i].y);
	}
	ret = DrawPolygonI(ipts, nPoints, p, b);
	MemFree(ipts);
	return ret;
}

Bool Media::GDIImage::DrawPolyPolygon(UnsafeArray<const Math::Coord2DDbl> points, UnsafeArray<const UInt32> pointCnt, UOSInt nPointCnt, Optional<DrawPen> p, Optional<DrawBrush> b)
{
	UOSInt i = 0;
	UOSInt j = nPointCnt;
	Bool ret;
	while (j-- > 0)
	{
		i += pointCnt[j];
	}
	Int32 *ipts = MemAlloc(Int32, i << 1);
	while (i-- > 0)
	{
		ipts[i << 1] = Double2Int32(points[i].x);
		ipts[(i << 1) + 1] = Double2Int32(points[i].y);
	}
	ret = DrawPolyPolygonI(ipts, pointCnt, nPointCnt, p, b);
	MemFree(ipts);
	return ret;
}

Bool Media::GDIImage::DrawRect(Math::Coord2DDbl tl, Math::Size2DDbl size, Optional<DrawPen> p, Optional<DrawBrush> b)
{
	NN<DrawBrush> nnb;
	if (b.SetTo(nnb))
	{
		NN<GDIBrush> brush = NN<GDIBrush>::ConvertFrom(nnb);
		if (this->bmpBits)
		{
			OSInt ix = Double2Int32(tl.x);
			OSInt iy = Double2Int32(tl.y);
			OSInt iw = Double2Int32(size.x);
			OSInt ih = Double2Int32(size.y);
			if (ix < 0)
			{
				iw += ix;
				ix = 0;
			}
			if (iy < 0)
			{
				ih += iy;
				iy = 0;
			}
			if ((ix + iw) > (OSInt)this->size.x)
			{
				iw = this->size.x - ix;
			}
			if ((iy + ih) > (OSInt)this->size.y)
			{
				ih = this->size.y - iy;
			}
			if (iw > 0 && ih > 0)
			{
				if ((brush->oriColor & 0xff000000) == 0xff000000)
				{
					ImageUtil_ImageColorFill32((this->size.y - iy - ih) * this->size.x * 4 + ix * 4 + (UInt8*)this->bmpBits, iw, ih, this->size.x << 2, brush->oriColor);
				}
				else
				{
					ImageUtil_ImageColorBlend32((this->size.y - iy - ih) * this->size.x * 4 + ix * 4 + (UInt8*)this->bmpBits, iw, ih, this->size.x << 2, brush->oriColor);
				}
			}
		}
		else
		{
			RECT rc;
			rc.left = Double2Int32(tl.x);
			rc.top = Double2Int32(tl.y);
			rc.right = Double2Int32(tl.x + size.x);
			rc.bottom = Double2Int32(tl.y + size.y);
			Int32 i = 10;
			
			while (FillRect((HDC)this->hdcBmp, &rc, (HBRUSH)brush->hbrush) == 0)
			{
				if (i-- <= 0)
				{
					break;
				}
				else
				{
					Sync::SimpleThread::Sleep(10);
				}
			}
		}
	}

	NN<DrawPen> nnp;
	if (p.SetTo(nnp))
	{
		GDIPen *pen = (GDIPen*)nnp.Ptr();
		if (this->bmpBits && (pen->oriColor & 0xff000000) == 0xff000000)
		{
			OSInt ix = Double2Int32(tl.x);
			OSInt iy = Double2Int32(tl.y);
			OSInt iw = Double2Int32(size.x);
			OSInt ih = Double2Int32(size.y);
			if (ix >= 0 && iy >= 0 && iw + ix < (OSInt)this->size.x && ih + iy < (OSInt)this->size.y)
			{
				ImageUtil_DrawRectNA32((this->size.y - iy - ih - 1) * this->size.x * 4 + ix * 4 + (UInt8*)this->bmpBits, iw + 1, ih + 1, this->size.x << 2, pen->oriColor);
			}
			else
			{
				POINT pts[5];
				if (this->currPen != nnp.Ptr())
				{
					GDIPen *pen = (GDIPen*)(this->currPen = nnp.Ptr());
					SelectObject((HDC)this->hdcBmp, (HPEN)pen->hpen);
				}
				pts[4].x = pts[1].x = pts[0].x = Double2Int32(tl.x);
				pts[4].y = pts[3].y = pts[0].y = Double2Int32(tl.y);
				pts[3].x = pts[2].x = Double2Int32(tl.x + size.x);
				pts[2].y = pts[1].y = Double2Int32(tl.y + size.y);
				Polyline((HDC)this->hdcBmp, pts, 5);
			}
		}
		else
		{
			POINT pts[5];
			if (this->currPen != nnp.Ptr())
			{
				GDIPen *pen = (GDIPen*)(this->currPen = nnp.Ptr());
				SelectObject((HDC)this->hdcBmp, (HPEN)pen->hpen);
			}
			pts[4].x = pts[1].x = pts[0].x = Double2Int32(tl.x);
			pts[4].y = pts[3].y = pts[0].y = Double2Int32(tl.y);
			pts[3].x = pts[2].x = Double2Int32(tl.x + size.x);
			pts[2].y = pts[1].y = Double2Int32(tl.y + size.y);
			Polyline((HDC)this->hdcBmp, pts, 5);
		}
	}

	return true;
}

Bool Media::GDIImage::DrawEllipse(Math::Coord2DDbl tl, Math::Size2DDbl size, Optional<DrawPen> p, Optional<DrawBrush> b)
{
	NN<DrawPen> nnp;
	if (this->info.atype == Media::AT_NO_ALPHA)
	{
		NN<DrawBrush> nnb;
		if (!b.SetTo(nnb))
		{
			if (p.SetTo(nnp))
			{
				if (this->currPen != nnp.Ptr())
				{
					GDIPen *pen = (GDIPen*)nnp.Ptr();
					SelectObject((HDC)this->hdcBmp, pen->hpen);
				}
				SelectObject((HDC)this->hdcBmp, GetStockObject(NULL_BRUSH));
				this->currBrush = 0;

				Ellipse((HDC)this->hdcBmp, Double2Int32(tl.x), Double2Int32(tl.y), Double2Int32(tl.x + size.x), Double2Int32(tl.y + size.y));
			}
		}
		else
		{
			if (p.SetTo(nnp))
			{
				if (this->currPen != nnp.Ptr())
				{
					GDIPen *pen = (GDIPen*)nnp.Ptr();
					SelectObject((HDC)this->hdcBmp, pen->hpen);
				}
			}
			else
			{
				SelectObject((HDC)this->hdcBmp, GetStockObject(NULL_PEN));
				this->currPen = 0;
			}
			if (this->currBrush != nnb.Ptr())
			{
				NN<GDIBrush> brush = NN<GDIBrush>::ConvertFrom(nnb);
				SelectObject((HDC)this->hdcBmp, brush->hbrush);
			}
			Ellipse((HDC)this->hdcBmp, Double2Int32(tl.x), Double2Int32(tl.y), Double2Int32(tl.x + size.x), Double2Int32(tl.y + size.y));
		}
	}
	else
	{
		NN<Media::GDIImage> tmpImg;
		if (!Optional<Media::GDIImage>::ConvertFrom(eng->CreateImage32(this->size, Media::AT_NO_ALPHA)).SetTo(tmpImg))
			return false;
		UInt8 *imgPtr = (UInt8*)tmpImg->bmpBits;
		UInt8 *imgPtr2 = (UInt8*)this->bmpBits;
		UOSInt imgW = this->size.x;
		UOSInt imgH = this->size.y;
		Int32 c1 = 0;
		Int32 c2 = 0;
		ImageUtil_ColorFill32(imgPtr, imgW * imgH, 0xff000000);
		if (p.SetTo(nnp))
		{
			SelectObject((HDC)tmpImg->hdcBmp, (HPEN)eng->GetBlackPen());
			c1 = ((Media::GDIPen*)nnp.Ptr())->oriColor;
		}
		else
		{
			SelectObject((HDC)tmpImg->hdcBmp, GetStockObject(NULL_PEN));
		}
		NN<DrawBrush> nnb;
		if (b.SetTo(nnb))
		{
			SelectObject((HDC)tmpImg->hdcBmp, (HBRUSH)eng->GetWhiteBrush());
			c2 = NN<Media::GDIBrush>::ConvertFrom(nnb)->oriColor;
		}
		else
		{
			SelectObject((HDC)tmpImg->hdcBmp, GetStockObject(NULL_BRUSH));
		}
		Ellipse((HDC)tmpImg->hdcBmp, Double2Int32(tl.x), Double2Int32(tl.y), Double2Int32(tl.x + size.x), Double2Int32(tl.y + size.y));
		ImageUtil_ImageMask2ABlend32(imgPtr, imgPtr2, imgW, imgH, imgW << 2, imgW << 2, c1, c2);
		
		eng->DeleteImage(tmpImg);
	}
	return true;
}

Bool Media::GDIImage::DrawString(Math::Coord2DDbl tl, NN<Text::String> str, NN<DrawFont> f, NN<DrawBrush> b)
{
	UnsafeArray<const WChar> wptr = Text::StrToWCharNew(str->v);
	Bool ret = this->DrawStringW(tl, wptr, f, b);
	Text::StrDelNew(wptr);
	return ret;
}

Bool Media::GDIImage::DrawString(Math::Coord2DDbl tl, Text::CStringNN str, NN<DrawFont> f, NN<DrawBrush> b)
{
	UnsafeArray<const WChar> wptr = Text::StrToWCharNew(str.v);
	Bool ret = this->DrawStringW(tl, wptr, f, b);
	Text::StrDelNew(wptr);
	return ret;
}

Bool Media::GDIImage::DrawStringW(Math::Coord2DDbl tl, UnsafeArray<const WChar> str, NN<DrawFont> f, NN<DrawBrush> b)
{
	NN<GDIBrush> brush = NN<GDIBrush>::ConvertFrom(b);
	UnsafeArray<const WChar> src = str;
	while (*src++);
	if (this->bmpBits == 0 || (this->info.atype == Media::AT_NO_ALPHA && (brush->oriColor & 0xff000000) == 0xff000000))
	{
		SetTextColor((HDC)this->hdcBmp, brush->color);
		if (this->currFont != f.Ptr())
		{
			this->currFont = f.Ptr();
			NN<GDIFont> font = NN<GDIFont>::ConvertFrom(f);
			SelectObject((HDC)this->hdcBmp, (HFONT)font->hfont);
		}
		Math::Size2DDbl sz;
		if (this->strAlign == Media::DrawEngine::DRAW_POS_CENTER || this->strAlign == Media::DrawEngine::DRAW_POS_CENTERLEFT || this->strAlign == Media::DrawEngine::DRAW_POS_CENTERRIGHT)
		{
			sz = GetTextSize(f, str, src - str - 1);
		}
		else
		{
			sz.y = 0;
		}
#ifdef _WIN32_WCE
		ExtTextOut((HDC)this->hdcBmp, Double2Int32(tl.x), Double2Int32(tl.y - (sz.y * 0.5)), 0, 0, str.Ptr(), (Int32)(src - str - 1), 0);
#else
		TextOutW((HDC)this->hdcBmp, Double2Int32(tl.x), Double2Int32(tl.y - (sz.y * 0.5)), str.Ptr(), (Int32)(src - str - 1));
#endif
	}
	else
	{
		Math::Size2DDbl sz = GetTextSize(f, str, src - str - 1);
		NN<Media::GDIImage> tmpImg;
		if (Optional<Media::GDIImage>::ConvertFrom(this->eng->CreateImage32(Math::Size2D<UOSInt>((UOSInt)sz.x + 1, (UOSInt)sz.y + 1), Media::AT_NO_ALPHA)).SetTo(tmpImg))
		{
			NN<Media::DrawBrush> b2 = tmpImg->NewBrushARGB(0xffffffff);
			tmpImg->DrawStringW(Math::Coord2DDbl(0, 0), str, f, b2);
			tmpImg->DelBrush(b2);
			Media::ImageUtil::ColorReplaceAlpha32((UInt8*)tmpImg->bmpBits, tmpImg->info.storeSize.x, tmpImg->info.storeSize.y, NN<GDIBrush>::ConvertFrom(b)->oriColor);
			tmpImg->info.atype = Media::AT_ALPHA;
			Double x;
			Double y;
			if (this->strAlign == Media::DrawEngine::DRAW_POS_TOPLEFT || this->strAlign == Media::DrawEngine::DRAW_POS_CENTERLEFT || this->strAlign == Media::DrawEngine::DRAW_POS_BOTTOMLEFT)
			{
				x = tl.x;
			}
			else if (this->strAlign == Media::DrawEngine::DRAW_POS_TOPRIGHT || this->strAlign == Media::DrawEngine::DRAW_POS_CENTERRIGHT || this->strAlign == Media::DrawEngine::DRAW_POS_BOTTOMRIGHT)
			{
				x = tl.x - sz.x;
			}
			else
			{
				x = tl.x - sz.x * 0.5;
			}
			if (this->strAlign == Media::DrawEngine::DRAW_POS_TOPLEFT || this->strAlign == Media::DrawEngine::DRAW_POS_TOPCENTER || this->strAlign == Media::DrawEngine::DRAW_POS_TOPRIGHT)
			{
				y = tl.y;
			}
			else if (this->strAlign == Media::DrawEngine::DRAW_POS_BOTTOMLEFT || this->strAlign == Media::DrawEngine::DRAW_POS_BOTTOMCENTER || this->strAlign == Media::DrawEngine::DRAW_POS_BOTTOMRIGHT)
			{
				y = tl.y - sz.y;
			}
			else
			{
				y = tl.y - sz.y * 0.5;
			}
			this->DrawImagePt(tmpImg, Math::Coord2DDbl(x, y));
			this->eng->DeleteImage(tmpImg);
		}
	}
	return true;
}

Bool Media::GDIImage::DrawStringRot(Math::Coord2DDbl center, NN<Text::String> str, NN<DrawFont> f, NN<DrawBrush> b, Double angleDegree)
{
	UnsafeArray<const WChar> wptr = Text::StrToWCharNew(str->v);
	Bool ret = this->DrawStringRotW(center, wptr, f, b, angleDegree);
	Text::StrDelNew(wptr);
	return ret;
}

Bool Media::GDIImage::DrawStringRot(Math::Coord2DDbl center, Text::CStringNN str, NN<DrawFont> f, NN<DrawBrush> b, Double angleDegree)
{
	UnsafeArray<const WChar> wptr = Text::StrToWCharNew(str.v);
	Bool ret = this->DrawStringRotW(center, wptr, f, b, angleDegree);
	Text::StrDelNew(wptr);
	return ret;
}

Bool Media::GDIImage::DrawStringRotW(Math::Coord2DDbl center, UnsafeArray<const WChar> str, NN<DrawFont> f, NN<DrawBrush> b, Double angleDegree)
{
	NN<GDIBrush> brush = NN<GDIBrush>::ConvertFrom(b);
	NN<GDIFont> font = NN<GDIFont>::ConvertFrom(f);
	UnsafeArray<const WChar> src = str;
	while (*src++);

	SetTextColor((HDC)this->hdcBmp, brush->color);
	
	HFONT hfont;
	LOGFONTW lf;

	ZeroMemory(&lf, sizeof(lf));
	lf.lfHeight = -font->pxSize;
	if (font->GetFontStyle() & Media::DrawEngine::DFS_BOLD)
	{
		lf.lfWeight = FW_BOLD;
	}
	else
	{
		lf.lfWeight = FW_NORMAL;
	}
	if (font->GetFontStyle() & Media::DrawEngine::DFS_ITALIC)
	{
		lf.lfItalic = TRUE;
	}
	if (font->GetFontStyle() & Media::DrawEngine::DFS_ANTIALIAS)
	{
		lf.lfQuality = ANTIALIASED_QUALITY;
	}
	Text::StrConcat(lf.lfFaceName, font->GetNameW());
	lf.lfEscapement = Double2Int32(angleDegree * 10.0);
	hfont = CreateFontIndirectW(&lf);

	Int32 bnds[8];
	OSInt px;
	OSInt py;
	GetStringBoundRotW(bnds, Double2Int32(center.x), Double2Int32(center.y), str, f, angleDegree, px, py);
	HGDIOBJ ofont = SelectObject((HDC)this->hdcBmp, (HFONT)hfont);
	TextOutW((HDC)this->hdcBmp, (int)px, (int)py, str.Ptr(), (Int32)(src - str - 1));
	SelectObject((HDC)this->hdcBmp, ofont);
	DeleteObject(hfont);
	return true;
}

Bool Media::GDIImage::DrawStringB(Math::Coord2DDbl tl, NN<Text::String> str1, NN<DrawFont> f, NN<DrawBrush> b, UOSInt buffSize)
{
	UnsafeArray<const WChar> wptr = Text::StrToWCharNew(str1->v);
	Bool ret = DrawStringBW(tl, wptr, f, b, buffSize);
	Text::StrDelNew(wptr);
	return ret;
}

Bool Media::GDIImage::DrawStringB(Math::Coord2DDbl tl, Text::CStringNN str1, NN<DrawFont> f, NN<DrawBrush> b, UOSInt buffSize)
{
	UnsafeArray<const WChar> wptr = Text::StrToWCharNew(str1.v);
	Bool ret = DrawStringBW(tl, wptr, f, b, buffSize);
	Text::StrDelNew(wptr);
	return ret;
}

Bool Media::GDIImage::DrawStringBW(Math::Coord2DDbl tl, UnsafeArray<const WChar> str1, NN<DrawFont> f, NN<DrawBrush> b, UOSInt buffSize)
{
	OSInt px = Double2Int32(tl.x);
	OSInt py = Double2Int32(tl.y);
	NN<GDIBrush> brush = NN<GDIBrush>::ConvertFrom(b);
	UnsafeArray<const WChar> src = str1;
	while (*src++);

	OSInt drawX;
	OSInt drawY;
	Int32 sz[2];
	Int32 bnds[8];
	NN<Media::GDIImage> gimg;
	OSInt swidth;
	OSInt sheight;
	OSInt sx;
	OSInt sy;
	OSInt dwidth;
	OSInt dheight;
	Int32 maxV;
	Int32 minV;

	GetStringBoundW(bnds, px, py, str1, f, px, py);
	minV = maxV = bnds[0];
	if (bnds[2] > maxV)
		maxV = bnds[2];
	if (bnds[4] > maxV)
		maxV = bnds[4];
	if (bnds[6] > maxV)
		maxV = bnds[6];
	if (bnds[2] < minV)
		minV = bnds[2];
	if (bnds[4] < minV)
		minV = bnds[4];
	if (bnds[6] < minV)
		minV = bnds[6];
	sz[0] = maxV - minV;
	drawX = px - minV;
	px = minV;
	minV = maxV = bnds[1];
	if (bnds[3] > maxV)
		maxV = bnds[3];
	if (bnds[5] > maxV)
		maxV = bnds[5];
	if (bnds[7] > maxV)
		maxV = bnds[7];
	if (bnds[3] < minV)
		minV = bnds[3];
	if (bnds[5] < minV)
		minV = bnds[5];
	if (bnds[7] < minV)
		minV = bnds[7];
	sz[1] = maxV - minV;
	drawY = py - minV;
	py = minV;
	dwidth = this->size.x - px;
	dheight = this->size.y - py;

	if (dwidth < 0)
	{
	}
	else if (dheight < 0)
	{
	}
	else
	{
		if (!Optional<Media::GDIImage>::ConvertFrom(eng->CreateImage32(Math::Size2D<UOSInt>(swidth = sz[0] + (buffSize << 1), sheight = sz[1] + (buffSize << 1)), Media::AT_ALPHA)).SetTo(gimg))
		{
			return false;
		}

		if (px < (OSInt)buffSize)
		{
			sx = -px + buffSize;
			swidth += px;
			px = 0;
		}
		else
		{
			sx = 0;
			px -= buffSize;
		}
		if (py < (OSInt)buffSize)
		{
			sy = -py + buffSize;
			sheight += py;
			py = 0;
		}
		else
		{
			sy = 0;
			py -= buffSize;
		}
		if ((OSInt)gimg->GetHeight() - sheight < sy)
		{
			sheight = gimg->GetHeight() - sy;
		}
		if ((OSInt)gimg->GetWidth() - swidth < sx)
		{
			swidth = gimg->GetWidth() - sx;
		}
		if (dwidth + (OSInt)buffSize < swidth)
		{
			swidth = dwidth + buffSize;
		}
		if (dheight + (OSInt)buffSize < sheight)
		{
			sheight = dheight + buffSize;
		}
		if (swidth <= 0 || sheight <= 0 || sz[0] <= 0 || sz[1] <= 0)
		{

		}
		else
		{
			SetTextColor((HDC)gimg->hdcBmp, 0xffffff);
			gimg->SetTextAlign(this->strAlign);
			if (gimg->currFont != f.Ptr())
			{
				gimg->currFont = f.Ptr();
				NN<GDIFont> font = NN<GDIFont>::ConvertFrom(f);
				SelectObject((HDC)gimg->hdcBmp, (HFONT)font->hfont);
			}
#ifdef _WIN32_WCE
			ExtTextOut((HDC)gimg->hdcBmp, drawX + buffSize, drawY + buffSize, 0, 0, str1.Ptr(), (Int32)(src - str1 - 1), 0);
#else
			TextOutW((HDC)gimg->hdcBmp, (int)(drawX + buffSize), (int)(drawY + buffSize), str1.Ptr(), (Int32)(src - str1 - 1));
#endif

			OSInt bpl = (sz[0] + (buffSize << 1)) << 2;
			OSInt dbpl = this->size.x << 2;
			Int32 color = brush->oriColor;
			UInt8 *pbits = (UInt8*)gimg->bmpBits;
			UInt8 *dbits = (UInt8*)this->bmpBits;
			ImageUtil_ImageColorBuffer32(pbits + bpl * buffSize + buffSize * 4, sz[0], sz[1], bpl, buffSize);
			if (py + sheight > (OSInt)this->size.x)
			{
				sheight = this->size.x - py;
			}
			if (px + swidth > (OSInt)this->size.x)
			{
				swidth = this->size.x - px;
			}
			if (swidth > 0 && sheight > 0)
			{
				if (this->bmpBits)
				{
					pbits = ((UInt8*)pbits) + ((gimg->size.y - sy - sheight) * bpl) + (sx << 2);
					dbits = ((UInt8*)dbits) + ((this->size.y - py - sheight) * dbpl) + (px << 2);

					if ((color & 0xff000000) == 0xff000000)
					{
						ImageUtil_ImageColorReplace32((UInt8*)pbits, (UInt8*)dbits, swidth, sheight, bpl, dbpl, color);
					}
					else
					{
						ImageUtil_ImageMaskABlend32((UInt8*)pbits, (UInt8*)dbits, swidth, sheight, bpl, dbpl, color);
					}
				}
				else
				{
					pbits = ((UInt8*)pbits) + ((gimg->size.y - sy - sheight) * bpl) + (sx << 2);
					dbits = ((UInt8*)dbits) + ((this->size.y - py - sheight) * dbpl) + (px << 2);

					ImageUtil_ImageColorReplace32((UInt8*)pbits, (UInt8*)pbits, swidth, sheight, bpl, bpl, color);
					gimg->SetHDPI(this->GetHDPI());
					gimg->SetVDPI(this->GetVDPI());
					this->DrawImagePt(gimg, Math::Coord2DDbl(OSInt2Double(px - sx), OSInt2Double(py - sy)));
				}
			}
		}
		eng->DeleteImage(gimg);
	}

	return true;
}

Bool Media::GDIImage::DrawStringRotB(Math::Coord2DDbl center, NN<Text::String> str1, NN<DrawFont> f, NN<DrawBrush> b, Double angleDegree, UOSInt buffSize)
{
	UnsafeArray<const WChar> wptr = Text::StrToWCharNew(str1->v);
	Bool ret = this->DrawStringRotBW(center, wptr, f, b, angleDegree, buffSize);
	Text::StrDelNew(wptr);
	return ret;
}

Bool Media::GDIImage::DrawStringRotB(Math::Coord2DDbl center, Text::CStringNN str1, NN<DrawFont> f, NN<DrawBrush> b, Double angleDegree, UOSInt buffSize)
{
	UnsafeArray<const WChar> wptr = Text::StrToWCharNew(str1.v);
	Bool ret = this->DrawStringRotBW(center, wptr, f, b, angleDegree, buffSize);
	Text::StrDelNew(wptr);
	return ret;
}

Bool Media::GDIImage::DrawStringRotBW(Math::Coord2DDbl center, UnsafeArray<const WChar> str1, NN<DrawFont> f, NN<DrawBrush> b, Double angleDegree, UOSInt buffSize)
{
	OSInt px = Double2Int32(center.x);
	OSInt py = Double2Int32(center.y);
	NN<GDIBrush> brush = NN<GDIBrush>::ConvertFrom(b);
	UnsafeArray<const WChar> src = str1;
	while (*src++);

	OSInt drawX;
	OSInt drawY;
	Int32 sz[2];
	Int32 bnds[8];
	NN<Media::GDIImage> gimg;
	OSInt swidth;
	OSInt sheight;
	OSInt sx;
	OSInt sy;
	OSInt dwidth;
	OSInt dheight;
	Int32 maxV;
	Int32 minV;
	GetStringBoundRotW(bnds, center.x, center.y, str1, f, angleDegree, px, py);
	minV = maxV = bnds[0];
	if (bnds[2] > maxV)
		maxV = bnds[2];
	if (bnds[4] > maxV)
		maxV = bnds[4];
	if (bnds[6] > maxV)
		maxV = bnds[6];
	if (bnds[2] < minV)
		minV = bnds[2];
	if (bnds[4] < minV)
		minV = bnds[4];
	if (bnds[6] < minV)
		minV = bnds[6];
	sz[0] = maxV - minV;
	drawX = px - minV;
	px = minV;
	minV = maxV = bnds[1];
	if (bnds[3] > maxV)
		maxV = bnds[3];
	if (bnds[5] > maxV)
		maxV = bnds[5];
	if (bnds[7] > maxV)
		maxV = bnds[7];
	if (bnds[3] < minV)
		minV = bnds[3];
	if (bnds[5] < minV)
		minV = bnds[5];
	if (bnds[7] < minV)
		minV = bnds[7];
	sz[1] = maxV - minV;
	drawY = py - minV;
	py = minV;
	dwidth = (OSInt)this->size.x - px;
	dheight = (OSInt)this->size.y - py;

	if (dwidth < 0)
	{
	}
	else if (dheight < 0)
	{
	}
	else
	{
		if (!Optional<Media::GDIImage>::ConvertFrom(eng->CreateImage32(Math::Size2D<UOSInt>(swidth = sz[0] + (buffSize << 1), sheight = sz[1] + (buffSize << 1)), Media::AT_NO_ALPHA)).SetTo(gimg))
			return false;

		if (px < (OSInt)buffSize)
		{
			sx = -px + buffSize;
			swidth += px;
			px = 0;
		}
		else
		{
			sx = 0;
			px -= buffSize;
		}
		if (py < (OSInt)buffSize)
		{
			sy = -py + buffSize;
			sheight += py;
			py = 0;
		}
		else
		{
			sy = 0;
			py -= buffSize;
		}
		if ((OSInt)gimg->GetHeight() - sheight < sy)
		{
			sheight = (OSInt)gimg->GetHeight() - sy;
		}
		if ((OSInt)gimg->GetWidth() - swidth < sx)
		{
			swidth = (OSInt)gimg->GetWidth() - sx;
		}
		if (dwidth + (OSInt)buffSize < swidth)
		{
			swidth = dwidth + buffSize;
		}
		if (dheight + (OSInt)buffSize < sheight)
		{
			sheight = dheight + buffSize;
		}
		if (swidth <= 0 || sheight <= 0 || sz[0] <= 0 || sz[1] <= 0)
		{

		}
		else
		{
			SetTextColor((HDC)gimg->hdcBmp, 0xffffff);
			HFONT hfont;
			LOGFONTW lf;
			NN<GDIFont> font = NN<GDIFont>::ConvertFrom(f);
			gimg->SetTextAlign(this->strAlign);

			ZeroMemory(&lf, sizeof(lf));
			lf.lfHeight = -font->pxSize;
			if (font->GetFontStyle() & Media::DrawEngine::DFS_BOLD)
			{
				lf.lfWeight = FW_BOLD;
			}
			else
			{
				lf.lfWeight = FW_NORMAL;
			}
			if (font->GetFontStyle() & Media::DrawEngine::DFS_ITALIC)
			{
				lf.lfItalic = TRUE;
			}
			if (font->GetFontStyle() & Media::DrawEngine::DFS_ANTIALIAS)
			{
				lf.lfQuality = ANTIALIASED_QUALITY;
			}
			Text::StrConcat(lf.lfFaceName, font->GetNameW());
			lf.lfEscapement = Double2Int32(angleDegree * 10);
			hfont = CreateFontIndirectW(&lf);

			HGDIOBJ ofont = SelectObject((HDC)gimg->hdcBmp, (HFONT)hfont);
			TextOutW((HDC)gimg->hdcBmp, (int)(drawX + buffSize), (int)(drawY + buffSize), str1.Ptr(), (Int32)(src - str1 - 1));
			SelectObject((HDC)gimg->hdcBmp, ofont);
			DeleteObject(hfont);


/*			HGDIOBJ ofont = SelectObject((HDC)gimg->hdcBmp, (HFONT)hfont);
			TextOutW((HDC)gimg->hdcBmp, drawX, drawY, str1, (Int32)(src - str1 - 1));
			SelectObject((HDC)gimg->hdcBmp, ofont);
			DeleteObject(hfont);*/

			OSInt bpl = (sz[0] + (buffSize << 1)) << 2;
			OSInt dbpl = this->size.x << 2;
			Int32 color = brush->oriColor;
			UInt8 *pbits = (UInt8*)gimg->bmpBits;
			UInt8 *dbits = (UInt8*)this->bmpBits;
			ImageUtil_ImageColorBuffer32(pbits + bpl * buffSize + buffSize * 4, sz[0], sz[1], bpl, buffSize);

			pbits = ((UInt8*)pbits) + ((gimg->size.y - sy - sheight) * bpl) + (sx << 2);
			if (py + sheight > (OSInt)this->size.y)
			{
				sheight = this->size.y - py;
			}
			if (px + swidth > (OSInt)this->size.x)
			{
				swidth = this->size.x - px;
			}
			dbits = ((UInt8*)dbits) + ((this->size.y - py - sheight) * dbpl) + (px << 2);
			ImageUtil_ImageColorReplace32(pbits, dbits, swidth, sheight, bpl, dbpl, color);
		}
		eng->DeleteImage(gimg);
	}

	return true;
}

Bool Media::GDIImage::DrawImagePt(NN<DrawImage> img, Math::Coord2DDbl tl)
{
	GDIImage *image = (GDIImage *)img.Ptr();
	if (this->hBmp == 0)
	{
		return this->DrawImageRect(img, Double2Int32(tl.x), Double2Int32(tl.y), Double2Int32(tl.x + image->GetWidth() * this->info.hdpi / image->GetHDPI()), Double2Int32(tl.y + image->GetHeight() * this->info.vdpi / image->GetVDPI()));
	}
	if (image->info.atype == Media::AT_NO_ALPHA)
	{
		if (this->IsOffScreen())
		{
			Int32 x = Double2Int32(tl.x);
			Int32 y = Double2Int32(tl.y);
			Int32 sx = 0;
			Int32 sy = 0;
			OSInt w = image->size.x;
			OSInt h = image->size.y;
			OSInt bpl = this->size.x << 2;
			if (x < 0)
			{
				w += x;
				sx = -x;
				x = 0;
			}
			if (y < 0)
			{
				h += y;
				sy = -y;
				y = 0;
			}
			if (x + w > (OSInt)this->size.x)
			{
				w = this->size.x - x;
			}
			if (y + h > (OSInt)this->size.y)
			{
				h = this->size.y - y;
			}
			if (w > 0 && h > 0)
			{
				img->CopyBits(sx, sy, ((UInt8*)this->bmpBits) + (this->size.y - h - y) * bpl + (x << 2), bpl, w, h, true);
			}
		}
		else
		{
			BitBlt((HDC)this->hdcBmp, Double2Int32(tl.x), Double2Int32(tl.y), (int)image->size.x, (int)image->size.y, (HDC)image->hdcBmp, 0, 0, SRCCOPY);
		}
	}
	else
	{
#if !defined(_WIN32_WCE)
		if (this->IsOffScreen())
		{
			OSInt w = image->size.x;
			OSInt h = image->size.y;
			UInt8 *dbits = (UInt8*)this->bmpBits;
			UInt8 *sbits = (UInt8*)image->bmpBits;
			OSInt dbpl = this->size.x << 2;
			OSInt sbpl = image->size.x << 2;

			if (tl.x < 0)
			{
				w += Double2Int32(tl.x);
				sbits -= Double2Int32(tl.x) << 2;
				tl.x = 0;
			}
			if (tl.y < 0)
			{
				h += Double2Int32(tl.y);
				tl.y = 0;
			}

			if (tl.x + w > this->size.x)
			{
				w = this->size.x - Double2Int32(tl.x);
			}
			if (tl.y + h > this->size.y)
			{
				sbits += (h - (this->size.y - Double2Int32(tl.y))) * sbpl;
				h = this->size.y - Double2Int32(tl.y);
			}
			if (w > 0 && h > 0)
			{
				this->eng->iab.SetSourceProfile(image->info.color);
				this->eng->iab.SetDestProfile(this->info.color);
				this->eng->iab.SetOutputProfile(this->info.color);
				this->eng->iab.Blend(dbits + (this->size.y - (Int32)tl.y - h) * dbpl + (((Int32)tl.x) * 4), dbpl, sbits, sbpl, w, h, image->info.atype);
			}
		}
		else
		{
			if (image->info.atype == Media::AT_PREMUL_ALPHA)
			{
				BLENDFUNCTION bf;
				bf.SourceConstantAlpha = 255;
				bf.AlphaFormat = AC_SRC_ALPHA;
				bf.BlendOp = AC_SRC_OVER;
				bf.BlendFlags = 0;
				AlphaBlend((HDC)this->hdcBmp, Double2Int32(tl.x), Double2Int32(tl.y), (int)image->size.x, (int)image->size.y, (HDC)image->hdcBmp, 0, 0, (int)image->size.x, (int)image->size.y, bf);
			}
			else
			{
				BLENDFUNCTION bf;
				bf.SourceConstantAlpha = 255;
				bf.AlphaFormat = AC_SRC_ALPHA;
				bf.BlendOp = AC_SRC_OVER;
				bf.BlendFlags = 0;
				AlphaBlend((HDC)this->hdcBmp, Double2Int32(tl.x), Double2Int32(tl.y), (int)image->size.x, (int)image->size.y, (HDC)image->hdcBmp, 0, 0, (int)image->size.x, (int)image->size.y, bf);
			}
		}
#elif (!defined(_WIN32_WCE) || (_WIN32_WCE >= 0x0500))
		if (image->info.atype == Media::AT_PREMUL_ALPHA)
		{
			BLENDFUNCTION bf;
			bf.SourceConstantAlpha = 255;
			bf.AlphaFormat = AC_SRC_ALPHA;
			bf.BlendOp = AC_SRC_OVER;
			bf.BlendFlags = 0;
			AlphaBlend((HDC)this->hdcBmp, Double2Int32(tlx), Double2Int32(tly), (int)image->size.x, (int)image->size.y, (HDC)image->hdcBmp, 0, 0, (int)image->width, (int)image->height, bf);
		}
		else
		{
			BLENDFUNCTION bf;
			bf.SourceConstantAlpha = 255;
			bf.AlphaFormat = AC_SRC_ALPHA;
			bf.BlendOp = AC_SRC_OVER;
			bf.BlendFlags = 0;
			AlphaBlend((HDC)this->hdcBmp, Double2Int32(tlx), Double2Int32(tly), (int)image->width, (int)image->height, (HDC)image->hdcBmp, 0, 0, (int)image->width, (int)image->height, bf);
		}
#else
		BitBlt((HDC)this->hdcBmp, Double2Int32(tlx), Double2Int32(tly), (int)image->width, (int)image->height, (HDC)image->hdcBmp, 0, 0, SRCCOPY);
#endif
	}
	return true;
}

Bool Media::GDIImage::DrawImagePt2(NN<Media::StaticImage> img, Math::Coord2DDbl tl)
{
	if (this->IsOffScreen() && img->GetImageType() == Media::RasterImage::ImageType::Static)
	{
		Media::StaticImage *simg = img.Ptr();
		simg->ToB8G8R8A8();
		if (simg->info.atype == Media::AT_NO_ALPHA)
		{
			Int32 x = Double2Int32(tl.x);
			Int32 y = Double2Int32(tl.y);
			Int32 sx = 0;
			Int32 sy = 0;
			OSInt w = simg->info.dispSize.x;
			OSInt h = simg->info.dispSize.y;
			OSInt bpl = this->size.x << 2;
			if (x < 0)
			{
				w += x;
				sx = -x;
				x = 0;
			}
			if (y < 0)
			{
				h += y;
				sy = -y;
				y = 0;
			}
			if (x + w > (OSInt)this->size.x)
			{
				w = this->size.x - x;
			}
			if (y + h > (OSInt)this->size.y)
			{
				h = this->size.y - y;
			}
			if (w > 0 && h > 0)
			{
				ImageCopy_ImgCopy(simg->data.Ptr() + (sy * simg->info.storeSize.x << 2) + (sx << 2), ((UInt8*)this->bmpBits) + (this->size.y - y - 1) * bpl + (x << 2), w << 2, h, simg->info.storeSize.x << 2, -(OSInt)this->size.x << 2);
			}
		}
		else
		{
			OSInt w = (OSInt)simg->info.dispSize.x;
			OSInt h = (OSInt)simg->info.dispSize.y;
			UInt8 *dbits = (UInt8*)this->bmpBits;
			UnsafeArray<UInt8> sbits = simg->data;
			UOSInt dbpl = this->size.x << 2;
			UOSInt sbpl = simg->info.storeSize.x << 2;

			if (tl.x < 0)
			{
				w += Double2Int32(tl.x);
				sbits -= Double2Int32(tl.x) << 2;
				tl.x = 0;
			}
			if (tl.y < 0)
			{
				h += Double2Int32(tl.y);
				sbits -= Double2Int32(tl.y) * sbpl;
				tl.y = 0;
			}

			if (tl.x + w > this->size.x)
			{
				w = this->size.x - Double2Int32(tl.x);
			}
			if (tl.y + h > this->size.y)
			{
				h = this->size.y - Double2Int32(tl.y);
			}
			if (w > 0 && h > 0)
			{
				this->eng->iab.SetSourceProfile(simg->info.color);
				this->eng->iab.SetDestProfile(this->info.color);
				this->eng->iab.SetOutputProfile(this->info.color);
				this->eng->iab.Blend(dbits + (this->size.y - Double2Int32(tl.y) - 1) * dbpl + (Double2Int32(tl.x) * 4), -(OSInt)dbpl, sbits, sbpl, w, h, simg->info.atype);
			}
		}
		return true;
	}
	else
	{
		NN<Media::DrawImage> dimg;
		if (this->eng->ConvImage(img).SetTo(dimg))
		{
			DrawImagePt(dimg, tl);
			this->eng->DeleteImage(dimg);
			return true;
		}
		return false;
	}
}

Bool Media::GDIImage::DrawImagePt3(NN<DrawImage> img, Math::Coord2DDbl destTL, Math::Coord2DDbl srcTL, Math::Size2DDbl srcSize)
{
	GDIImage *image = (GDIImage *)img.Ptr();
	if (this->hBmp == 0)
	{
		return this->DrawImageRect(img, Double2OSInt(destTL.x), Double2OSInt(destTL.y), Double2OSInt(destTL.x + srcSize.x * this->info.hdpi / image->GetHDPI()), Double2OSInt(destTL.y + srcSize.y * this->info.vdpi / image->GetVDPI()));
	}
	if (image->info.atype == Media::AT_NO_ALPHA)
	{
		if (this->IsOffScreen())
		{
			Int32 x = Double2Int32(destTL.x);
			Int32 y = Double2Int32(destTL.y);
			Int32 sx = Double2Int32(srcTL.x);
			Int32 sy = Double2Int32(srcTL.y);
			OSInt w = Double2Int32(srcSize.x);
			OSInt h = Double2Int32(srcSize.y);
			OSInt bpl = this->size.x << 2;
			if (x < 0)
			{
				w += x;
				sx -= x;
				x = 0;
			}
			if (y < 0)
			{
				h += y;
				sy -= y;
				y = 0;
			}
			if (x + w > (OSInt)this->size.x)
			{
				w = this->size.x - x;
			}
			if (y + h > (OSInt)this->size.y)
			{
				h = this->size.y - y;
			}
			if (w > 0 && h > 0)
			{
				img->CopyBits(sx, sy, ((UInt8*)this->bmpBits) + (this->size.y - h - y) * bpl + (x << 2), bpl, w, h, true);
			}
		}
		else
		{
			BitBlt((HDC)this->hdcBmp, Double2Int32(destTL.x), Double2Int32(destTL.y), Double2Int32(srcSize.x), Double2Int32(srcSize.y), (HDC)image->hdcBmp, Double2Int32(srcTL.x), Double2Int32(srcTL.y), SRCCOPY);
		}
	}
	else
	{
#if !defined(_WIN32_WCE)
		if (this->IsOffScreen())
		{
			Int32 x = Double2Int32(destTL.x);
			Int32 y = Double2Int32(destTL.y);
			Int32 sx = Double2Int32(srcTL.x);
			Int32 sy = Double2Int32(srcTL.y);
			OSInt w = Double2Int32(srcSize.x);
			OSInt h = Double2Int32(srcSize.y);
			UInt8 *dbits = (UInt8*)this->bmpBits;
			UInt8 *sbits = (UInt8*)image->bmpBits;
			OSInt dbpl = this->size.x << 2;
			OSInt sbpl = image->size.x << 2;
			OSInt sh = image->size.y;

			if (x < 0)
			{
				w += x;
				sx -= x;
				x = 0;
			}
			if (y < 0)
			{
				h += y;
				sy -= y;
				y = 0;
			}
			if (x + w > (OSInt)this->size.x)
			{
				w = this->size.x - x;
			}
			if (y + h > (OSInt)this->size.y)
			{
				h = this->size.y - y;
			}
			if (w > 0 && h > 0)
			{
				this->eng->iab.SetSourceProfile(image->info.color);
				this->eng->iab.SetDestProfile(this->info.color);
				this->eng->iab.SetOutputProfile(this->info.color);
				this->eng->iab.Blend(dbits + (this->size.y - y - h) * dbpl + (x * 4), dbpl, sbits + (sh - sy - h) * sbpl + (sx << 2), sbpl, w, h, image->info.atype);
			}
		}
		else
		{
			if (image->info.atype == Media::AT_PREMUL_ALPHA)
			{
				BLENDFUNCTION bf;
				bf.SourceConstantAlpha = 255;
				bf.AlphaFormat = AC_SRC_ALPHA;
				bf.BlendOp = AC_SRC_OVER;
				bf.BlendFlags = 0;
				AlphaBlend((HDC)this->hdcBmp, Double2Int32(destTL.x), Double2Int32(destTL.y), Double2Int32(srcSize.x), Double2Int32(srcSize.y), (HDC)image->hdcBmp, Double2Int32(srcTL.x), Double2Int32(srcTL.y), Double2Int32(srcSize.x), Double2Int32(srcSize.y), bf);
			}
			else
			{
				BLENDFUNCTION bf;
				bf.SourceConstantAlpha = 255;
				bf.AlphaFormat = AC_SRC_ALPHA;
				bf.BlendOp = AC_SRC_OVER;
				bf.BlendFlags = 0;
				AlphaBlend((HDC)this->hdcBmp, Double2Int32(destTL.x), Double2Int32(destTL.y), Double2Int32(srcSize.x), Double2Int32(srcSize.y), (HDC)image->hdcBmp, Double2Int32(srcTL.x), Double2Int32(srcTL.y), Double2Int32(srcSize.x), Double2Int32(srcSize.y), bf);
			}
		}
#elif (!defined(_WIN32_WCE) || (_WIN32_WCE >= 0x0500))
		if (image->info.atype == Media::AT_PREMUL_ALPHA)
		{
			BLENDFUNCTION bf;
			bf.SourceConstantAlpha = 255;
			bf.AlphaFormat = AC_SRC_ALPHA;
			bf.BlendOp = AC_SRC_OVER;
			bf.BlendFlags = 0;
			AlphaBlend((HDC)this->hdcBmp, Double2Int32(destTL.x), Double2Int32(destTL.y), Double2Int32(srcSize.x), Double2Int32(srcSize.y), (HDC)image->hdcBmp, Double2Int32(srcTL.x), Double2Int32(srcTL.y), Double2Int32(srcSize.x), Double2Int32(srcSize.y), bf);
		}
		else
		{
			BLENDFUNCTION bf;
			bf.SourceConstantAlpha = 255;
			bf.AlphaFormat = AC_SRC_ALPHA;
			bf.BlendOp = AC_SRC_OVER;
			bf.BlendFlags = 0;
			AlphaBlend((HDC)this->hdcBmp, Double2Int32(destTL.x), Double2Int32(destY), Double2Int32(srcSize.x), Double2Int32(srcSize.y), (HDC)image->hdcBmp, Double2Int32(srcTL.x), Double2Int32(srcTL.y), Double2Int32(srcSize.x), Double2Int32(srcSize.y), bf);
		}
#else
		BitBlt((HDC)this->hdcBmp, Double2Int32(destX), Double2Int32(destY), Double2Int32(srcW), Double2Int32(srcH), (HDC)image->hdcBmp, Double2Int32(srcX), Double2Int32(srcY), SRCCOPY);
#endif
	}
	return true;
}

Bool Media::GDIImage::DrawImageRect(NN<DrawImage> img, OSInt tlx, OSInt tly, OSInt brx, OSInt bry)
{
	GDIImage *image = (GDIImage *)img.Ptr();
	StretchBlt((HDC)this->hdcBmp, (int)tlx, (int)tly, (int)(brx - tlx), (int)(bry - tly), (HDC)image->hdcBmp, 0, 0, (int)img->GetWidth(), (int)img->GetHeight(), SRCCOPY);
	return true;
}

NN<Media::DrawPen> Media::GDIImage::NewPenARGB(UInt32 color, Double thick, UnsafeArrayOpt<UInt8> pattern, UOSInt nPattern)
{
	if (thick < 1)
		thick = 1;
	HPEN hpen;
	DWORD *dwPattern = 0;
	UnsafeArray<UInt8> nnpattern;
	if (nPattern == 0 || !pattern.SetTo(nnpattern))
	{
		hpen = CreatePen(PS_SOLID, Double2Int32(thick), GDIEGetCol(color));
	}
	else
	{
#ifdef _WIN32_WCE
		LOGPEN lp;
		lp.lopnStyle = PS_DASH;
		lp.lopnWidth.x = Double2Int32(thick);
		lp.lopnWidth.y = lp.lopnWidth.x;
		lp.lopnColor = GDIEGetCol(color);
		hpen = CreatePenIndirect(&lp);
#else
		LOGBRUSH lb;
		lb.lbColor = GDIEGetCol(color);
		lb.lbStyle = BS_SOLID;
		lb.lbHatch = 0;
		dwPattern = MemAlloc(DWORD, nPattern);
		OSInt i = nPattern;
		while (i-- > 0)
		{
			dwPattern[i] = nnpattern[i];
		}
		hpen = ExtCreatePen(PS_GEOMETRIC | PS_USERSTYLE | PS_ENDCAP_ROUND, Double2Int32(thick), &lb, (UInt32)nPattern, dwPattern);
#endif
	}
	NN<GDIPen> pen;
	NEW_CLASSNN(pen, GDIPen(hpen, (UInt32*)dwPattern, nPattern, this, thick, color));
	return pen;
}

NN<Media::DrawBrush> Media::GDIImage::NewBrushARGB(UInt32 color)
{
	HBRUSH hbrush = CreateSolidBrush(GDIEGetCol(color));
	NN<GDIBrush> brush;
	NEW_CLASSNN(brush, GDIBrush(hbrush, color, this));
	return brush;
}

NN<Media::DrawFont> Media::GDIImage::NewFontPt(Text::CStringNN name, Double ptSize, Media::DrawEngine::DrawFontStyle fontStyle, UInt32 codePage)
{
	NN<GDIFont> f;
	UnsafeArray<const WChar> wptr = Text::StrToWCharNew(name.v);
	NEW_CLASSNN(f, GDIFont(this->hdcBmp, wptr, ptSize, fontStyle, *this, codePage));
	Text::StrDelNew(wptr);
	return f;
}

NN<Media::DrawFont> Media::GDIImage::NewFontPtW(UnsafeArray<const WChar> name, Double ptSize, Media::DrawEngine::DrawFontStyle fontStyle, UInt32 codePage)
{
	NN<GDIFont> f;
	NEW_CLASSNN(f, GDIFont(this->hdcBmp, name, ptSize, fontStyle, *this, codePage));
	return f;
}

NN<Media::DrawFont> Media::GDIImage::NewFontPx(Text::CStringNN name, Double pxSize, Media::DrawEngine::DrawFontStyle fontStyle, UInt32 codePage)
{
	NN<GDIFont> f;
	UnsafeArray<const WChar> wptr = Text::StrToWCharNew(name.v);
	NEW_CLASSNN(f, GDIFont(this->hdcBmp, wptr, pxSize * 72.0 / this->info.hdpi, fontStyle, *this, codePage));
	Text::StrDelNew(wptr);
	return f;
}

NN<Media::DrawFont> Media::GDIImage::NewFontPxW(UnsafeArray<const WChar> name, Double pxSize, Media::DrawEngine::DrawFontStyle fontStyle, UInt32 codePage)
{
	NN<GDIFont> f;
	NEW_CLASSNN(f, GDIFont(this->hdcBmp, name, pxSize * 72.0 / this->info.hdpi, fontStyle, *this, codePage));
	return f;
}

NN<Media::DrawFont> Media::GDIImage::CloneFont(NN<Media::DrawFont> f)
{
	NN<GDIFont> oldF = NN<Media::GDIFont>::ConvertFrom(f);
	NEW_CLASSNN(f, GDIFont(this->hdcBmp, oldF->GetNameW(), oldF->GetPointSize(), oldF->GetFontStyle(), *this, oldF->GetCodePage()));
	return f;
}

void Media::GDIImage::DelPen(NN<DrawPen> p)
{
	if (this->currPen == p.Ptr())
		this->currPen = 0;
	GDIPen *pen = (GDIPen *)p.Ptr();
	if (pen->pattern)
		MemFree(pen->pattern);
	DeleteObject((HPEN)pen->hpen);
	DEL_CLASS(pen);
}

void Media::GDIImage::DelBrush(NN<DrawBrush> b)
{
	if (this->currBrush == b.Ptr())
		this->currBrush = 0;
	NN<GDIBrush> brush = NN<GDIBrush>::ConvertFrom(b);
	DeleteObject((HBRUSH)brush->hbrush);
	brush.Delete();
}

void Media::GDIImage::DelFont(NN<DrawFont> f)
{
	if (this->currFont == f.Ptr())
		this->currFont = 0;
	NN<GDIFont> font = NN<GDIFont>::ConvertFrom(f);
	font.Delete();
}

Math::Size2DDbl Media::GDIImage::GetTextSize(NN<DrawFont> fnt, Text::CStringNN txt)
{
	UOSInt strLen;
	strLen = Text::StrUTF8_WCharCntC(txt.v, txt.leng);
	WChar *wptr = MemAlloc(WChar, strLen + 1);
	Text::StrUTF8_WCharC(wptr, txt.v, txt.leng, 0);
	Math::Size2DDbl ret = GetTextSize(fnt, wptr, strLen);
	MemFree(wptr);
	return ret;
}

Math::Size2DDbl Media::GDIImage::GetTextSize(NN<DrawFont> fnt, UnsafeArray<const WChar> txt, OSInt txtLen)
{
	Bool isCJK = true;
	if (txtLen == -1)
		txtLen = (OSInt)Text::StrCharCnt(txt);

	OSInt i = txtLen;
	while (i-- > 0)
	{
		if (txt[i] < 0x4E00 || txt[i] > 0x9FFF)
		{
			isCJK = false;
			break;
		}
	}

	if (isCJK)
	{
		return Math::Size2DDbl(OSInt2Double((NN<GDIFont>::ConvertFrom(fnt)->pxSize + 1) * txtLen), NN<GDIFont>::ConvertFrom(fnt)->pxSize + 2);
	}
	else
	{
		SIZE size;
		Int32 i = 3;
		if (this->currFont != fnt.Ptr())
		{
			this->currFont = fnt.Ptr();
			NN<GDIFont> f = NN<GDIFont>::ConvertFrom(fnt);
			SelectObject((HDC)this->hdcBmp, (HFONT)f->hfont);
		}
		while (GetTextExtentExPointW((HDC)this->hdcBmp, txt.Ptr(), (int)txtLen, (int)this->size.x, 0, 0, &size) == 0)
		{
			if (i-- <= 0)
			{
				this->currFont = fnt.Ptr();
				
				//wprintf(L"Error in get text size: %s, font = %s, %X\n", txt, f->GetNameW(), f->hfont);
				size.cx = 0;
				size.cy = 0;
				break;
			}
			else
			{
				this->currFont = fnt.Ptr();
				NN<GDIFont> f = NN<GDIFont>::ConvertFrom(fnt);
				SelectObject((HDC)this->hdcBmp, (HFONT)f->hfont);
				Sync::SimpleThread::Sleep(10);
			}
		}
		return Math::Size2DDbl(size.cx, size.cy);
	}
}

void Media::GDIImage::SetTextAlign(Media::DrawEngine::DrawPos pos)
{
	static UInt32 textAlign[] = {TA_TOP | TA_LEFT, TA_TOP | TA_CENTER, TA_TOP | TA_RIGHT, TA_TOP | TA_LEFT, TA_TOP | TA_CENTER, TA_TOP | TA_RIGHT, TA_BOTTOM | TA_LEFT, TA_BOTTOM | TA_CENTER, TA_BOTTOM | TA_RIGHT};
	this->strAlign = pos;
	::SetTextAlign((HDC)this->hdcBmp, textAlign[pos]);
}

void Media::GDIImage::GetStringBound(UnsafeArray<Int32> pos, OSInt centX, OSInt centY, UnsafeArray<const UTF8Char> str, NN<DrawFont> f, OutParam<OSInt> drawX, OutParam<OSInt> drawY)
{
	UnsafeArray<const WChar> wptr = Text::StrToWCharNew(str);
	GetStringBoundW(pos, centX, centY, wptr, f, drawX, drawY);
	Text::StrDelNew(wptr);
}

void Media::GDIImage::GetStringBoundW(UnsafeArray<Int32> pos, OSInt centX, OSInt centY, UnsafeArray<const WChar> str, NN<DrawFont> f, OutParam<OSInt> drawX, OutParam<OSInt> drawY)
{
	Math::Size2DDbl sz = GetTextSize(f, str, (OSInt)Text::StrCharCnt(str));
	Bool isCenter = false;
	if (strAlign == Media::DrawEngine::DRAW_POS_TOPLEFT)
	{
		pos[0] = (Int32)centX;
		pos[1] = (Int32)centY;
	}
	else if (strAlign == Media::DrawEngine::DRAW_POS_TOPCENTER)
	{
		pos[0] = (Int32)centX - Double2Int32(sz.x * 0.5);
		pos[1] = (Int32)centY;
	}
	else if (strAlign == Media::DrawEngine::DRAW_POS_TOPRIGHT)
	{
		pos[0] = (Int32)centX - Double2Int32(sz.x);
		pos[1] = (Int32)centY;
	}
	else if (strAlign == Media::DrawEngine::DRAW_POS_CENTERLEFT)
	{
		pos[0] = (Int32)centX;
		pos[1] = (Int32)centY - Double2Int32(sz.y * 0.5);
		isCenter = true;
	}
	else if (strAlign == Media::DrawEngine::DRAW_POS_CENTER)
	{
		pos[0] = (Int32)centX - Double2Int32(sz.x * 0.5);
		pos[1] = (Int32)centY - Double2Int32(sz.y * 0.5);
		isCenter = true;
	}
	else if (strAlign == Media::DrawEngine::DRAW_POS_CENTERRIGHT)
	{
		pos[0] = (Int32)centX - Double2Int32(sz.x);
		pos[1] = (Int32)centY - Double2Int32(sz.y * 0.5);
		isCenter = true;
	}
	else if (strAlign == Media::DrawEngine::DRAW_POS_BOTTOMLEFT)
	{
		pos[0] = (Int32)centX;
		pos[1] = (Int32)centY - Double2Int32(sz.y);
	}
	else if (strAlign == Media::DrawEngine::DRAW_POS_BOTTOMCENTER)
	{
		pos[0] = (Int32)centX - Double2Int32(sz.x * 0.5);
		pos[1] = (Int32)centY - Double2Int32(sz.y);
	}
	else if (strAlign == Media::DrawEngine::DRAW_POS_BOTTOMRIGHT)
	{
		pos[0] = (Int32)centX - Double2Int32(sz.x);
		pos[1] = (Int32)centY - Double2Int32(sz.y);
	}
	pos[2] = pos[0] + Double2Int32(sz.x);
	pos[3] = pos[1];
	pos[4] = pos[0] + Double2Int32(sz.x);
	pos[5] = pos[1] + Double2Int32(sz.y);
	pos[6] = pos[0];
	pos[7] = pos[1] + Double2Int32(sz.y);
	if (isCenter)
	{
		drawX.Set(centX);
		drawY.Set(centY - Double2Int32(sz.y * 0.5));
	}
	else
	{
		drawX.Set(centX);
		drawY.Set(centY);
	}
}

void Media::GDIImage::GetStringBoundRot(UnsafeArray<Int32> pos, Double centX, Double centY, UnsafeArray<const UTF8Char> str, NN<DrawFont> f, Double angleDegree, OutParam<OSInt> drawX, OutParam<OSInt> drawY)
{
	UnsafeArray<const WChar> wptr = Text::StrToWCharNew(str);
	this->GetStringBoundRotW(pos, centX, centY, wptr, f, angleDegree, drawX, drawY);
	Text::StrDelNew(wptr);
}

void Media::GDIImage::GetStringBoundRotW(UnsafeArray<Int32> pos, Double centX, Double centY, UnsafeArray<const WChar> str, NN<DrawFont> f, Double angleDegree, OutParam<OSInt> drawX, OutParam<OSInt> drawY)
{
	Math::Size2DDbl sz = GetTextSize(f, str, (OSInt)Text::StrCharCnt(str));
	Double pts[10];
	Bool isCenter = false;
	if (strAlign == Media::DrawEngine::DRAW_POS_TOPLEFT)
	{
		pts[0] = centX;
		pts[1] = centY;
	}
	else if (strAlign == Media::DrawEngine::DRAW_POS_TOPCENTER)
	{
		pts[0] = centX - sz.x * 0.5;
		pts[1] = centY;
	}
	else if (strAlign == Media::DrawEngine::DRAW_POS_TOPRIGHT)
	{
		pts[0] = centX - sz.x;
		pts[1] = centY;
	}
	else if (strAlign == Media::DrawEngine::DRAW_POS_CENTERLEFT)
	{
		pts[0] = centX;
		pts[1] = centY - sz.y * 0.5;
		isCenter = true;
	}
	else if (strAlign == Media::DrawEngine::DRAW_POS_CENTER)
	{
		pts[0] = centX - sz.x * 0.5;
		pts[1] = centY - sz.y * 0.5;
		isCenter = true;
	}
	else if (strAlign == Media::DrawEngine::DRAW_POS_CENTERRIGHT)
	{
		pts[0] = centX - sz.x;
		pts[1] = centY - sz.y * 0.5;
		isCenter = true;
	}
	else if (strAlign == Media::DrawEngine::DRAW_POS_BOTTOMLEFT)
	{
		pts[0] = centX;
		pts[1] = centY - sz.y;
	}
	else if (strAlign == Media::DrawEngine::DRAW_POS_BOTTOMCENTER)
	{
		pts[0] = centX - sz.x * 0.5;
		pts[1] = centY - sz.y;
	}
	else if (strAlign == Media::DrawEngine::DRAW_POS_BOTTOMRIGHT)
	{
		pts[0] = centX - sz.x;
		pts[1] = centY - sz.y;
	}
	pts[2] = pts[0] + sz.x;
	pts[3] = pts[1];
	pts[4] = pts[0] + sz.x;
	pts[5] = pts[1] + sz.y;
	pts[6] = pts[0];
	pts[7] = pts[1] + sz.y;
	if (isCenter)
	{
		pts[8] = centX;
		pts[9] = centY - sz.y * 0.5;
		Math::GeometryTool::RotateACW(pts, pts, 5, centX, centY, angleDegree * Math::PI / 180.0);
		pos[0] = Double2Int32(pts[0]);
		pos[1] = Double2Int32(pts[1]);
		pos[2] = Double2Int32(pts[2]);
		pos[3] = Double2Int32(pts[3]);
		pos[4] = Double2Int32(pts[4]);
		pos[5] = Double2Int32(pts[5]);
		pos[6] = Double2Int32(pts[6]);
		pos[7] = Double2Int32(pts[7]);
		drawX.Set(Double2Int32(pts[8]));
		drawY.Set(Double2Int32(pts[9]));
	}
	else
	{
		Math::GeometryTool::RotateACW(pts, pts, 4, centX, centY, angleDegree * Math::PI / 180.0);
		pos[0] = Double2Int32(pts[0]);
		pos[1] = Double2Int32(pts[1]);
		pos[2] = Double2Int32(pts[2]);
		pos[3] = Double2Int32(pts[3]);
		pos[4] = Double2Int32(pts[4]);
		pos[5] = Double2Int32(pts[5]);
		pos[6] = Double2Int32(pts[6]);
		pos[7] = Double2Int32(pts[7]);
		drawX.Set(Double2Int32(centX));
		drawY.Set(Double2Int32(centY));
	}
}

void Media::GDIImage::CopyBits(OSInt x, OSInt y, UnsafeArray<UInt8> imgPtr, UOSInt dbpl, UOSInt width, UOSInt height, Bool upsideDown) const
{
	UnsafeArray<UInt8> iptr = imgPtr;
	UInt8 *sptr = (UInt8*)this->bmpBits;
	UOSInt sbpl = this->size.x << 2;
	OSInt dAdd = (OSInt)dbpl;
	if (upsideDown)
	{
		iptr += (height - 1) * dbpl;
		dAdd = -dAdd;
	}
	UOSInt sheight = this->size.y;
	if (x < 0)
	{
		iptr = iptr - (x << 2);
		width += x;
		x = 0;
	}
	if (y < 0)
	{
		iptr = iptr - dAdd * y;
		height += y;
		y = 0;
	}
	if (x + (OSInt)width > (OSInt)this->size.x)
	{
		width = this->size.x - x;
	}
	if (y + (OSInt)height > (OSInt)this->size.y)
	{
		height = this->size.y - y;
	}

	if ((width << 2) < 1024)
	{
		sptr += (sheight - y) * sbpl + (x << 2);
		while (height-- > 0)
		{
			sptr -= sbpl;
			MemCopyNO(iptr.Ptr(), sptr, width << 2);
			iptr += dAdd;
		}
	}
	else
	{
		sptr += (sheight - y) * sbpl + (x << 2);
		while (height-- > 0)
		{
			sptr -= sbpl;
			MemCopyNANC(iptr.Ptr(), sptr, width << 2);
			iptr += dAdd;
		}
	}
}

Optional<Media::StaticImage> Media::GDIImage::ToStaticImage() const
{
	return CreateStaticImage();
}

UOSInt Media::GDIImage::SavePng(NN<IO::SeekableStream> stm)
{
#ifdef HAS_GDIPLUS
	CLSID   encoderClsid;
	Gdiplus::Status  stat;
	Gdiplus::Image*   image;

	image = (Gdiplus::Image*)CreateGDIImage();
	if (image == 0)
	{
		return 2;
	}

	// Get the CLSID of the PNG encoder.
	if (GetEncoderClsid(L"image/png", &encoderClsid) < 0)
	{
		delete image;
		return 3;
	}

	Win32::COMStream *cstm;
	NEW_CLASS(cstm, Win32::COMStream(stm));

	stat = image->Save(cstm, &encoderClsid, NULL);

	DEL_CLASS(cstm);
	delete image;

	if(stat == Gdiplus::Ok)
		return 0;
	else
		return ((UOSInt)stat) | 0x10000; 
#else
	return 0x10000;
#endif
}

UOSInt Media::GDIImage::SaveGIF(NN<IO::SeekableStream> stm)
{
#ifdef HAS_GDIPLUS
	CLSID   encoderClsid;
	Gdiplus::Status  stat;
	Gdiplus::Bitmap*   image;
	Gdiplus::Bitmap*  newBmp;
	Gdiplus::Rect *rc;
	Gdiplus::BitmapData *bds;
	Gdiplus::BitmapData *bdd;
	Gdiplus::ColorPalette *pal;
	Bool succ;

	image = (Gdiplus::Bitmap*)CreateGDIImage();
	if (image == 0)
	{
		return 2;
	}
	
	newBmp = new Gdiplus::Bitmap((INT)this->size.x, (INT)this->size.y, PixelFormat8bppIndexed);
	bdd = new Gdiplus::BitmapData();
	bds = new Gdiplus::BitmapData();
	rc = new Gdiplus::Rect(0, 0, (INT)this->size.x, (INT)this->size.y);
	succ = false;
	if (newBmp->LockBits(rc, Gdiplus::ImageLockModeWrite, PixelFormat8bppIndexed, bdd) == 0)
	{
		if (image->LockBits(rc, Gdiplus::ImageLockModeRead | Gdiplus::ImageLockModeWrite, PixelFormat32bppARGB, bds) == 0)
		{
			pal = (Gdiplus::ColorPalette *)MAlloc(8 + 1024);
			pal->Flags = 0;
			pal->Count = 256;
			Media::ImageTo8Bit::From32bpp((UInt8*)bds->Scan0, (UInt8*)bdd->Scan0, (UInt8*)pal->Entries, this->size.x, this->size.y, bds->Stride, bdd->Stride);
			image->UnlockBits(bds);
			newBmp->UnlockBits(bdd);
			newBmp->SetPalette(pal);
			MemFree(pal);
			succ = true;
		}
		else
		{
			newBmp->UnlockBits(bdd);
		}
	}
	DEL_CLASS(rc);
	DEL_CLASS(bdd);
	DEL_CLASS(bds);
	if (!succ)
	{
		DEL_CLASS(newBmp);
		DEL_CLASS(image);
		return 4;
	}

	// Get the CLSID of the PNG encoder.
	if (GetEncoderClsid(L"image/gif", &encoderClsid) < 0)
	{
		DEL_CLASS(newBmp);
		DEL_CLASS(image);
		return 3;
	}

	Win32::COMStream *cstm;
	NEW_CLASS(cstm, Win32::COMStream(stm));

	stat = newBmp->Save(cstm, &encoderClsid, NULL);

	DEL_CLASS(cstm);
	DEL_CLASS(newBmp);
	DEL_CLASS(image);

	if(stat == Gdiplus::Ok)
		return 0;
	else
		return ((UOSInt)stat) | 0x10000; 
#else
	return 0x10000;
#endif
}

UOSInt Media::GDIImage::SaveJPG(NN<IO::SeekableStream> stm)
{
#ifdef HAS_GDIPLUS
	CLSID   encoderClsid;
	Gdiplus::Status  stat;
	Gdiplus::Image*   image;

	image = (Gdiplus::Image*)CreateGDIImage();
	if (image == 0)
	{
		return 2;
	}

	// Get the CLSID of the PNG encoder.
	if (GetEncoderClsid(L"image/jpeg", &encoderClsid) < 0)
	{
		DEL_CLASS(image);
		return 3;
	}

	Win32::COMStream *cstm;
	NEW_CLASS(cstm, Win32::COMStream(stm));

	stat = image->Save(cstm, &encoderClsid, NULL);

	DEL_CLASS(cstm);
	DEL_CLASS(image);


	if(stat == Gdiplus::Ok)
		return 0;
	else
		return ((UOSInt)stat) | 0x10000; 
#else
	return 0x10000;
#endif
}

NN<Media::RasterImage> Media::GDIImage::Clone() const
{
	return CreateStaticImage();
}

Media::RasterImage::ImageType Media::GDIImage::GetImageType() const
{
	return Media::RasterImage::ImageType::GUIImage;
}

void Media::GDIImage::GetRasterData(UnsafeArray<UInt8> destBuff, OSInt left, OSInt top, UOSInt width, UOSInt height, UOSInt destBpl, Bool upsideDown, Media::RotateType destRotate) const
{
	CopyBits(left, top, destBuff.Ptr(), destBpl, width, height, upsideDown);
}

void Media::GDIImage::PolylineAccel(void *hdc, const Int32 *points, UOSInt nPoints, OSInt ofstX, OSInt ofstY, OSInt width, OSInt height)
{
	Bool thisInScn;
	Bool lastInScn;
	const Int32 *ptStart = 0;
	UInt32 i;
	lastInScn = (points[0] >= 0) && (points[0] < width) && (points[1] >= 0) && (points[1] < height);
	if (lastInScn)
	{
		ptStart = points;
	}
	points += 2;

	i = 1;
	while (i < nPoints)
	{
		thisInScn = (points[0] >= 0) && (points[0] < width) && (points[1] >= 0) && (points[1] < height);
		if (thisInScn != lastInScn)
		{
			if (lastInScn)
			{
				Polyline((HDC)hdc, (POINT*)ptStart, (Int32)(1 + ((points - ptStart) >> 1)));
			}
			else
			{
				ptStart = points - 2;
			}
			lastInScn = thisInScn;
		}
		points += 2;
		i++;
	}
}

void Media::GDIImage::PolygonAccel(void *hdc, const Int32 *points, UOSInt nPoints, OSInt ofstX, OSInt ofstY, OSInt width, OSInt height, Int32 penWidth)
{
	//////////////////////////////////////////
	Int32 *pointsTmp = MemAlloc(Int32, nPoints << 2);
	Int32 *pointsTmp2;
	UOSInt ptCnt;
	UOSInt ptCnt2;

	if (pointsTmp == 0)
	{
		return;
	}

	ptCnt = Math::GeometryTool::BoundPolygonY(points, nPoints, pointsTmp, -penWidth, height + penWidth, 0, 0);
	if (ptCnt > 0)
	{
		pointsTmp2 = MemAlloc(Int32, ptCnt << 2);
//		Polygon((HDC)hdc, (POINT*)pointsTmp, ptCnt);

		ptCnt2 = Math::GeometryTool::BoundPolygonX(pointsTmp, ptCnt, pointsTmp2, -penWidth, width + penWidth, ofstX, ofstY);

		if (ptCnt2 > 0)
		{
#if defined(_WIN32_WCE) && defined(NDEBUG)
			WChar debug[16];
			Text::StrInt(debug, nPoints);
#endif
		//	MessageBox(0, debug, L"Debug", MB_OK);
			Polygon((HDC)hdc, (POINT*)pointsTmp2, (int)ptCnt2);
		}
		MemFree(pointsTmp2);
	}

	MemFree(pointsTmp);
}

void Media::GDIImage::PolyPolygonAccel(void *hdc, const Int32 *points, const UInt32 *pointCnt, UOSInt nPointCnt, OSInt ofstX, OSInt ofstY, OSInt width, OSInt height, Int32 penWidth)
{
	Int32 *pointsTmp;
	Int32 *pointsTmp2;
	Int32 *pointsTmp3;

	Int32 *currPoints;
	UInt32 nParts;
	UOSInt ptCnt;
	UOSInt ptCnt2;

	UInt32 pointsTmp3Size;
	UInt32 totalPoints = 0;
	UInt32 nPoints;
	UOSInt i = nPointCnt;
	while (i-- > 0)
	{
		totalPoints += pointCnt[i];
	}
	pointsTmp = MemAlloc(Int32, (UOSInt)totalPoints << 2);
	pointsTmp2 = MemAlloc(Int32, nPointCnt);
	pointsTmp3 = 0;
	pointsTmp3Size = 0;
	nParts = 0;
	currPoints = pointsTmp;

	i = 0;
	while (i < nPointCnt)
	{
		nPoints = pointCnt[i];
		if (pointsTmp3Size < nPoints)
		{
			if (pointsTmp3)
			{
				MemFree(pointsTmp3);
			}
			pointsTmp3 = MemAlloc(Int32, (pointsTmp3Size = nPoints) << 2);
		}

		ptCnt = Math::GeometryTool::BoundPolygonY(points, nPoints, pointsTmp3, -penWidth, height + penWidth, 0, 0);
		if (ptCnt > 0)
		{
			ptCnt2 = Math::GeometryTool::BoundPolygonX(pointsTmp3, ptCnt, currPoints, -penWidth, width + penWidth, ofstX, ofstY);
			if (ptCnt2 > 0)
			{
				pointsTmp2[nParts++] = (Int32)ptCnt2;
				currPoints += ptCnt2 << 1;
			}
		}

		points += nPoints << 1;
		i++;
	}

	if (nParts > 0)
	{
#ifdef _WIN32_WCE
		/////////////////////////////
#else
		PolyPolygon((HDC)hdc, (POINT*)pointsTmp, pointsTmp2, nParts);
#endif
	}

	if (pointsTmp3)
	{
		MemFree(pointsTmp3);
	}
	MemFree(pointsTmp2);
	MemFree(pointsTmp);
}

Bool Media::GDIImage::IsOffScreen()
{
	return this->hBmp != 0;
}

Bool Media::GDIImage::DrawRectN(OSInt oriX, OSInt oriY, OSInt oriW, OSInt oriH, DrawPen *p, Optional<DrawBrush> b)
{
	NN<DrawBrush> nnb;
	NN<Media::GDIBrush> gb;

	if (!b.SetTo(nnb))
	{
		if (p != 0)
		{
			DrawRect(Math::Coord2DDbl(OSInt2Double(oriX), OSInt2Double(oriY)), Math::Size2DDbl(OSInt2Double(oriW), OSInt2Double(oriH)), p, 0);
		}
		return true;
	}
	gb = NN<GDIBrush>::ConvertFrom(nnb);

	OSInt x = oriX;
	OSInt y = oriY;
	OSInt w = oriW;
	OSInt h = oriH;

	if (x < 0)
	{
		w += x;
		x = 0;
	}
	if (y < 0)
	{
		h += y;
		y = 0;
	}
	if (x + w > (OSInt)this->size.x)
	{
		w = this->size.x - x;
	}
	if (y + h > (OSInt)this->size.y)
	{
		h = this->size.y - y;
	}
	if (w <= 0)
		return false;
	if (h <= 0)
		return false;
	
	UOSInt bpl = this->size.x << 2;
	UInt8 *initOfst = ((UInt8*) this->bmpBits) + (OSInt)bpl * y + (x << 2);
	UInt32 c = gb->oriColor;

	if ((c & 0xff000000) == 0xff000000)
	{
		ImageUtil_ImageColorFill32(initOfst, (UOSInt)w, (UOSInt)h, bpl, c);
	}
	else
	{
		ImageUtil_ImageColorBlend32(initOfst, (UOSInt)w, (UOSInt)h, bpl, c);
	}
	if (p)
	{
		DrawRect(Math::Coord2DDbl(OSInt2Double(oriX), OSInt2Double(oriY)), Math::Size2DDbl(OSInt2Double(oriW), OSInt2Double(oriH)), p, 0);
	}
	return true;
}

void *Media::GDIImage::GetHDC()
{
	return this->hdcBmp;
}

void Media::GDIImage::FillAlphaRect(OSInt left, OSInt top, OSInt width, OSInt height, UInt8 alpha)
{
	if (left < 0)
	{
		width = width + left;
		left = 0;
	}
	if (top < 0)
	{
		height = height + top;
		top = 0;
	}
	if (left >= (OSInt)this->size.x)
		return;
	if (top >= (OSInt)this->size.y)
		return;
	if (left + width > (OSInt)this->size.x)
	{
		width = (OSInt)this->size.x - left;
	}
	if (top + height > (OSInt)this->size.y)
	{
		height = (OSInt)this->size.y - top;
	}
	if (width <= 0)
		return;
	if (height <= 0)
		return;
	if (this->info.storeBPP == 32 && this->bmpBits && this->info.pf == Media::PF_B8G8R8A8)
	{
		UInt8 *pbits = (UInt8*)this->bmpBits;
		UOSInt bpl = this->size.x * 4;
		pbits = pbits + ((OSInt)this->size.y - top - height) * (OSInt)bpl + left * 4;
		ImageUtil_ImageFillAlpha32(pbits, (UOSInt)width, (UOSInt)height, bpl, alpha);
	}
}

void *Media::GDIImage::CreateGDIImage()
{
#ifdef _WIN32_WCE
	return 0;
#else
	if (this->bitCount == 32 && this->bmpBits)
	{
		UInt8 *dptr;
		UInt8 *sptr;
		UOSInt i;
		UOSInt sbpl;
		UOSInt dbpl;

		Gdiplus::Rect rect(0, 0, (INT)this->size.x, (INT)this->size.y);
		Gdiplus::BitmapData *bitmapData = new Gdiplus::BitmapData();
		Gdiplus::Bitmap *bmp = new Gdiplus::Bitmap((INT)this->size.x, (INT)this->size.y, PixelFormat32bppARGB);
		bmp->LockBits(&rect, Gdiplus::ImageLockModeWrite, PixelFormat32bppARGB, bitmapData);
		i = this->size.y;
		sbpl = this->GetDataBpl();
		dbpl = (UINT)bitmapData->Stride;
		sptr = i * sbpl + (UInt8*)this->bmpBits;
		dptr = (UInt8*)bitmapData->Scan0;

		while (i-- > 0)
		{
			sptr -= sbpl;
			MemCopyNO(dptr, sptr, sbpl);
			dptr += dbpl;
		}

		bmp->UnlockBits(bitmapData);
		delete bitmapData;
		return bmp;
	}
	else
	{
		return Gdiplus::Bitmap::FromHBITMAP((HBITMAP)this->hBmp, 0);
	}
#endif
}
