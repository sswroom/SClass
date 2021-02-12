//require gdiplus.lib msimg32.lib
#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/Stream.h"
#include "IO/FileStream.h"
#include "Manage/HiResClock.h"
#include "Math/Math.h"
#include "Math/Geometry.h"
#include "Media/DrawEngine.h"
#include "Media/GDIEngine.h"
#include "Media/ImageCopyC.h"
#include "Media/ImageUtil.h"
#include "Media/StaticImage.h"
#include "Media/ImageTo8Bit.h"
#include "Media/ABlend/AlphaBlend8_8.h"
#include "Media/ABlend/AlphaBlend8_C8.h"
#include "Sync/Event.h"
#include "Sync/MutexUsage.h"
#include "Sync/Thread.h"
#include "Text/Encoding.h"
#include "Text/MyString.h"
#include "Text/MyStringW.h"

#if defined(WIN32_LEAN_AND_MEAN)
#undef WIN32_LEAN_AND_MEAN
#endif

//#include <stdio.h>
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
			return j;  // Success
		}
	}

	MemFree(pImageCodecInfo);
	return -1;  // Failure
}
#endif

COLORREF GDIEGetCol(Int32 col)
{
	return ((col & 0xff) << 16) | (col & 0xff00) | ((col >> 16) & 0xff);
}

Media::GDIEngine::GDIEngine()
{
#ifdef HAS_GDIPLUS
	NEW_CLASS(gdiplusStartupInput, Gdiplus::GdiplusStartupInput());
	Gdiplus::GdiplusStartup((ULONG_PTR*)&gdiplusToken, (Gdiplus::GdiplusStartupInput*)gdiplusStartupInput, NULL);
#endif
	this->hdc = GetDC(0);
	this->hdcScreen = CreateDC(TEXT("DISPLAY"), NULL, NULL, NULL); 
	this->hpenBlack = CreatePen(PS_SOLID, 1, 0);
	this->hbrushWhite = CreateSolidBrush(0xffffff);
	NEW_CLASS(this->gdiMut, Sync::Mutex());
	NEW_CLASS(this->iab, Media::ABlend::AlphaBlend8_8());
}

Media::GDIEngine::~GDIEngine()
{
	DeleteDC((HDC)this->hdcScreen);
	ReleaseDC(0, (HDC)this->hdc);
	DeleteObject(this->hpenBlack);
	DeleteObject(this->hbrushWhite);
	DEL_CLASS(this->iab);
	DEL_CLASS(this->gdiMut);

#ifdef HAS_GDIPLUS
	Gdiplus::GdiplusShutdown(gdiplusToken);
	DEL_CLASS((Gdiplus::GdiplusStartupInput*)gdiplusStartupInput);
#endif
}

Media::DrawImage *Media::GDIEngine::CreateImage32(UOSInt width, UOSInt height, Media::AlphaType atype)
{
	BITMAPINFO bInfo;
	void *bmpBits;
	HBITMAP hBmp;
	ZeroMemory(&bInfo, sizeof(bInfo));
	bInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bInfo.bmiHeader.biWidth = (LONG)width;
	bInfo.bmiHeader.biHeight = (LONG)height;
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
			Sync::Thread::Sleep(10);
			mutUsage.BeginUse();
		}
		mutUsage.EndUse();
		if (hdcBmp)
		{
			SelectObject(hdcBmp, hBmp);
			NEW_CLASS(img, GDIImage(this, 0, 0, width, height, 32, (void *)hBmp, bmpBits, (void *)hdcBmp, atype));
		}
		else
		{
			img = 0;
		}
		return img;
	}
	return 0;
}

Media::GDIImage *Media::GDIEngine::CreateImage24(UOSInt width, UOSInt height)
{
	BITMAPINFO bInfo;
	void *bmpBits;
	HBITMAP hBmp;
	ZeroMemory(&bInfo, sizeof(bInfo));
	bInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bInfo.bmiHeader.biWidth = (LONG)width;
	bInfo.bmiHeader.biHeight = (LONG)height;
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
			Sync::Thread::Sleep(10);
		}
		if (hdcBmp)
		{
			SelectObject(hdcBmp, hBmp);
			NEW_CLASS(img, GDIImage(this, 0, 0, width, height, 24, (void *)hBmp, bmpBits, (void *)hdcBmp, Media::AT_NO_ALPHA));
		}
		else
		{
			img = 0;
		}
		return img;
	}
	return 0;
}

Media::DrawImage *Media::GDIEngine::CreateImageScn(void *hdc, OSInt left, OSInt top, OSInt right, OSInt bottom)
{
	GDIImage *img;
	NEW_CLASS(img, GDIImage(this, left, top, right - left, bottom - top, 32, 0, 0, hdc, Media::AT_NO_ALPHA));
	return img;
}

Media::DrawImage *Media::GDIEngine::LoadImage(const UTF8Char *fileName)
{
	IO::FileStream *fstm;

	NEW_CLASS(fstm, IO::FileStream(fileName, IO::FileStream::FILE_MODE_READONLY, IO::FileStream::FILE_SHARE_DENY_NONE, IO::FileStream::BT_NORMAL));
	if (fstm->IsError())
	{
		DEL_CLASS(fstm);
		return 0;
	}
	DrawImage *img = LoadImageStream(fstm);

	DEL_CLASS(fstm);
	return img;
}


Media::DrawImage *Media::GDIEngine::LoadImageStream(IO::SeekableStream *fstm)
{
	UInt8 hdr[54];
	UInt8 pal[1024];
	BITMAPINFO bmi;
	DrawImage *img = 0;

	fstm->Read(hdr, 54);
	if (*(Int16*)hdr != *(Int16*)"BM")
	{
#ifdef HAS_GDIPLUS
		Gdiplus::Bitmap *gimg;
		Win32::COMStream *comStm;
		fstm->Seek(IO::SeekableStream::ST_BEGIN, 0);
		NEW_CLASS(comStm, Win32::COMStream(fstm));
		gimg = Gdiplus::Bitmap::FromStream(comStm, false);

		if (gimg == 0)
		{
			DEL_CLASS(comStm);
			return 0;
		}
		Gdiplus::BitmapData bmpd;
		Gdiplus::Status stat;
		Gdiplus::Rect rect(0, 0, gimg->GetWidth(), gimg->GetHeight());
		if ((stat = gimg->LockBits(&rect, Gdiplus::ImageLockModeRead, PixelFormat32bppARGB, &bmpd)) == Gdiplus::Ok)
		{
			void *pBits;
			HBITMAP hBmp;
			HDC hdcBmp;
			bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
			bmi.bmiHeader.biWidth = bmpd.Width;
			bmi.bmiHeader.biHeight = bmpd.Height;
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
				Int32 dbpl = bmpd.Width << 2;
				Int32 sbpl = bmpd.Stride;
				imgDest += bmpd.Height * dbpl;
				Int32 i = bmpd.Height;
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
					NEW_CLASS(img, GDIImage(this, 0, 0, bmi.bmiHeader.biWidth, bmi.bmiHeader.biHeight, 32, hBmp, pBits, (void*)hdcBmp, Media::AT_ALPHA));
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
		fstm->Seek(IO::SeekableStream::ST_BEGIN, 26);
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
		Int32 buffSize;
		Int32 lineW;
		Int32 i;
		Int32 j;
		UInt8 *psrc;
		UInt8 *psrc2;
		Int32 *pdest;

		switch (bpp)
		{
		case 8:
			{
				fstm->Read(pal, 1024);
				fstm->Seek(IO::SeekableStream::ST_BEGIN, *(Int32*)&hdr[10]);
				lineW = bmi.bmiHeader.biWidth;
				if (lineW & 3)
				{
					lineW = lineW + 4 - (lineW & 3);
				}
				buff = MemAlloc(UInt8, buffSize = (lineW * bmi.bmiHeader.biHeight));
				fstm->Read(buff, buffSize);
				psrc = (UInt8*)pBits;
				pdest = (Int32*)buff;
				j = bmi.bmiHeader.biHeight;
				while (j-- > 0)
				{
					psrc2 = psrc;
					i = bmi.bmiHeader.biWidth;
					while (i-- > 0)
					{
						*pdest++ = ((Int32*)pal)[*psrc2++] | 0xff000000;
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
					NEW_CLASS(img, GDIImage(this, 0, 0, bmi.bmiHeader.biWidth, bmi.bmiHeader.biHeight, 32, hBmp, pBits, (void*)hdcBmp, Media::AT_NO_ALPHA));
				}
				else
				{
				}
			}
			break;
		case 16:
			{
				fstm->Seek(IO::SeekableStream::ST_BEGIN, *(Int32*)&hdr[10]);
				buff = MemAlloc(UInt8, buffSize = ((bmi.bmiHeader.biWidth * bmi.bmiHeader.biHeight) << 1));
				fstm->Read(buff, buffSize);
				//////////////////////////////////////////////////////////////////////
				MemFree(buff);
				Sync::MutexUsage mutUsage(this->gdiMut);
				hdcBmp = CreateCompatibleDC((HDC)this->hdcScreen);
				mutUsage.EndUse();
				if (hdcBmp)
				{
					SelectObject(hdcBmp, hBmp);
					NEW_CLASS(img, GDIImage(this, 0, 0, bmi.bmiHeader.biWidth, bmi.bmiHeader.biHeight, 32, hBmp, pBits, (void*)hdcBmp, Media::AT_NO_ALPHA));
				}
				else
				{
				}
			}
			break;
		case 24:
			{
				fstm->Seek(IO::SeekableStream::ST_BEGIN, *(Int32*)&hdr[10]);
				lineW = bmi.bmiHeader.biWidth * 3;
				if (lineW & 3)
				{
					lineW = lineW + 4 - (lineW & 3);
				}
				buff = MemAlloc(UInt8, buffSize = (lineW * bmi.bmiHeader.biHeight));
				fstm->Read(buff, buffSize);
				psrc = (UInt8*)pBits;
				pdest = (Int32*)buff;
				j = bmi.bmiHeader.biHeight;
				while (j-- > 0)
				{
					psrc2 = psrc;
					i = bmi.bmiHeader.biWidth;
					while (i-- > 0)
					{
						*pdest++ = psrc2[0] | ((Int32)psrc2[1] << 8) | ((Int32)psrc2[2] << 16) | 0xff000000;
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
					NEW_CLASS(img, GDIImage(this, 0, 0, bmi.bmiHeader.biWidth, bmi.bmiHeader.biHeight, 32, hBmp, pBits, (void*)hdcBmp, Media::AT_NO_ALPHA));
				}
				else
				{
				}
			}
			break;
		case 32:
			{
				fstm->Seek(IO::SeekableStream::ST_BEGIN,  *(Int32*)&hdr[10]);
				fstm->Read((UInt8*)pBits, (bmi.bmiHeader.biWidth * bmi.bmiHeader.biHeight) << 2);

				Sync::MutexUsage mutUsage(this->gdiMut);
				hdcBmp = CreateCompatibleDC((HDC)this->hdcScreen);
				mutUsage.EndUse();
				if (hdcBmp)
				{
					SelectObject(hdcBmp, hBmp);
					NEW_CLASS(img, GDIImage(this, 0, 0, bmi.bmiHeader.biWidth, bmi.bmiHeader.biHeight, 32, hBmp, pBits, (void*)hdcBmp, Media::AT_ALPHA));
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

Media::DrawImage *Media::GDIEngine::ConvImage(Media::Image *img)
{
	if (img == 0)
	{
		return 0;
	}
	if (img->info->fourcc != 0)
	{
		return 0; 
	}
	Media::GDIImage *gimg = (Media::GDIImage*)CreateImage32(img->info->dispWidth, img->info->dispHeight, img->info->atype);
	if (gimg == 0)
		return 0;
	gimg->SetHDPI(img->info->hdpi);
	gimg->SetVDPI(img->info->vdpi);
	if (img->GetImageType() == Media::Image::IT_STATIC)
	{
		Media::StaticImage *simg = (Media::StaticImage*)img;
		if (simg->To32bpp())
		{
			UInt8 *sptr = (UInt8*)simg->data;
			UInt8 *dptr = (UInt8*)gimg->bmpBits;
			OSInt sbpl = simg->info->storeWidth << 2;
			OSInt dbpl = simg->info->dispWidth << 2;
			ImageCopy_ImgCopy(sptr, dptr + (dbpl * (simg->info->dispHeight - 1)), simg->info->dispWidth << 2, simg->info->dispHeight, sbpl, -dbpl);
		}
	}
	else
	{
		Media::StaticImage *simg = img->CreateStaticImage();
		if (simg->To32bpp())
		{
			UInt8 *sptr = (UInt8*)simg->data;
			UInt8 *dptr = (UInt8*)gimg->bmpBits;
			OSInt sbpl = simg->info->storeWidth << 2;
			OSInt dbpl = simg->info->dispWidth << 2;
			ImageCopy_ImgCopy(sptr, dptr + (dbpl * (simg->info->dispHeight - 1)), simg->info->dispWidth << 2, simg->info->dispHeight, sbpl, -dbpl);
		}
		DEL_CLASS(simg);
	}
	return gimg;
}

Media::DrawImage *Media::GDIEngine::CloneImage(Media::DrawImage *img)
{
	Media::GDIImage *newImg = (Media::GDIImage*)this->CreateImage32(img->GetWidth(), img->GetHeight(), img->GetAlphaType());
	newImg->info->Set(((Media::GDIImage*)img)->info);
	if (img->GetBitCount() == 32)
	{
		MemCopyNO(newImg->bmpBits, ((Media::GDIImage*)img)->bmpBits, newImg->GetWidth() * newImg->GetHeight() * 4);
	}
	else
	{
		newImg->DrawImagePt(img, 0, 0);
	}
	return newImg;
}

Bool Media::GDIEngine::DeleteImage(DrawImage *img)
{
	GDIImage *image = (GDIImage *)img;
	if (image->hBmp)
	{
		DeleteDC((HDC)image->hdcBmp);
		DeleteObject((HBITMAP)image->hBmp);
		image->bmpBits = 0;
	}
	DEL_CLASS((GDIImage*)img);
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

Media::GDIBrush::GDIBrush(void *hbrush, Int32 oriColor, DrawImage *img)
{
	this->hbrush = hbrush;
	this->oriColor = oriColor;
	this->color = GDIEGetCol(oriColor);
	this->img = img;
}

Media::GDIBrush::~GDIBrush()
{
}

Media::GDIPen::GDIPen(void *hpen, UInt32 *pattern, OSInt nPattern, DrawImage *img, Double thick, Int32 oriColor)
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

Media::GDIFont::GDIFont(void *hdc, const Char *fontName, Double size, Media::DrawEngine::DrawFontStyle style, DrawImage *img, Int32 codePage)
{
	this->fontName = Text::StrToWCharNew((const UTF8Char*)fontName);
	this->hdc = hdc;
	this->size = size;
	this->style = style;
	this->img = img;
	this->codePage = codePage;
	LOGFONTW lf;
	ZeroMemory(&lf, sizeof(LOGFONT));
	//this->pxSize = MulDiv(size, GetDeviceCaps((HDC)hdc, LOGPIXELSY), 72);
	this->pxSize = Math::Double2Int32(size * img->GetHDPI() / 72.0);
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

Media::GDIFont::GDIFont(void *hdc, const WChar *fontName, Double size, Media::DrawEngine::DrawFontStyle style, DrawImage *img, Int32 codePage)
{
	const WChar *src;
	WChar *dest;
	this->hdc = hdc;
	this->size = size;
	this->style = style;
	this->img = img;
	this->codePage = codePage;
	this->fontName = Text::StrCopyNew(fontName);
	LOGFONTW lf;
//	EnumFontsW((HDC)hdc, 0, FontFunc, (LPARAM)this);
	ZeroMemory(&lf, sizeof(LOGFONT));
	//this->pxSize = MulDiv(size, GetDeviceCaps((HDC)hdc, LOGPIXELSY), 72);
	this->pxSize = Math::Double2Int32(size * img->GetHDPI() / 72.0);
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

	dest = lf.lfFaceName;
	src = fontName;
	while ((*dest++ = *src++) != 0);
	this->hfont = CreateFontIndirectW(&lf);
}

Media::GDIFont::~GDIFont()
{
	Text::StrDelNew(this->fontName);
	DeleteObject(hfont);
}

const WChar *Media::GDIFont::GetNameW()
{
	return this->fontName;
}

Double Media::GDIFont::GetPointSize()
{
	return this->size;
}

Media::DrawEngine::DrawFontStyle Media::GDIFont::GetFontStyle()
{
	return this->style;
}

Int32 Media::GDIFont::GetCodePage()
{
	return this->codePage;
}

Media::GDIImage::GDIImage(GDIEngine *eng, OSInt left, OSInt top, UOSInt width, UOSInt height, UInt32 bitCount, void *hBmp, void *bmpBits, void *hdcBmp, Media::AlphaType atype) : Media::Image(width, height, 0, 0, 0, bitCount, Media::FrameInfo::GetDefPixelFormat(0, bitCount), 0, 0, Media::ColorProfile::YUVT_BT601, atype, Media::YCOFST_C_CENTER_LEFT)
{
	this->eng = eng;
	this->left = left;
	this->top = top;
	this->width = width;
	this->height = height;
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

UOSInt Media::GDIImage::GetWidth()
{
	return this->width;
}

UOSInt Media::GDIImage::GetHeight()
{
	return this->height;
}

UInt32 Media::GDIImage::GetBitCount()
{
	return this->bitCount;	
}

Media::ColorProfile *Media::GDIImage::GetColorProfile()
{
	return this->info->color;
}

void Media::GDIImage::SetColorProfile(const Media::ColorProfile *color)
{
	return this->info->color->Set(color);
}

Media::AlphaType Media::GDIImage::GetAlphaType()
{
	return this->info->atype;
}

void Media::GDIImage::SetAlphaType(Media::AlphaType atype)
{
	this->info->atype = atype;
}

Double Media::GDIImage::GetHDPI()
{
	return this->info->hdpi;
}

Double Media::GDIImage::GetVDPI()
{
	return this->info->vdpi;
}

void Media::GDIImage::SetHDPI(Double dpi)
{
	if (dpi > 0)
	{
		this->info->hdpi = dpi;
	}
}

void Media::GDIImage::SetVDPI(Double dpi)
{
	if (dpi > 0)
	{
		this->info->vdpi = dpi;
	}
}

UInt8 *Media::GDIImage::GetImgBits(Bool *revOrder)
{
	*revOrder = true;
	return (UInt8*)this->bmpBits;
}

void Media::GDIImage::GetImgBitsEnd(Bool modified)
{
}

OSInt Media::GDIImage::GetImgBpl()
{
	return this->GetDataBpl();
}

Media::EXIFData *Media::GDIImage::GetEXIF()
{
	return this->exif;
}

Media::PixelFormat Media::GDIImage::GetPixelFormat()
{
	return this->info->pf;
}

Bool Media::GDIImage::DrawLine(Double x1, Double y1, Double x2, Double y2, DrawPen *p)
{
	if (this->currPen != p)
	{
		GDIPen *pen = (GDIPen*)(this->currPen = p);
		SelectObject((HDC)this->hdcBmp, (HPEN)pen->hpen);
	}
	if (p && (((GDIPen*)p)->oriColor & 0xff000000))
	{
		MoveToEx((HDC)this->hdcBmp, Math::Double2Int32(x1), Math::Double2Int32(y1), 0);
		LineTo((HDC)this->hdcBmp, Math::Double2Int32(x2), Math::Double2Int32(y2));
	}
	return true;
}

Bool Media::GDIImage::DrawPolylineI(Int32 *points, UOSInt nPoints, DrawPen *p)
{
	if (this->currPen != p)
	{
		GDIPen *pen = (GDIPen*)(this->currPen = p);
		SelectObject((HDC)this->hdcBmp, (HPEN)pen->hpen);
	}
	if (p && (((GDIPen*)p)->oriColor & 0xff000000))
	{
		Polyline((HDC)this->hdcBmp, (POINT*)points, (int)nPoints);
	}
	return true;
}

Bool Media::GDIImage::DrawPolygonI(Int32 *points, UOSInt nPoints, DrawPen *p, DrawBrush *b)
{
	if (p)
	{
		if (this->currPen != p)
		{
			GDIPen *pen = (GDIPen*)(this->currPen = p);
			SelectObject((HDC)this->hdcBmp, (HPEN)pen->hpen);
		}
	}

	if (b == 0)
	{
#ifdef _WIN32_WCE
		if (p)
		{
			Int32 line2[4];
			line2[0] = points[0];
			line2[1] = points[1];
			line2[2] = points[(nPoints << 1) - 2];
			line2[3] = points[(nPoints << 1) - 1];
			PolylineAccel(this->hdcBmp, points, nPoints, left, top, width, height);
			PolylineAccel(this->hdcBmp, line2, 2, left, top, width, height);
		}
#else
		if (p)
		{
			MoveToEx((HDC)this->hdcBmp, points[(nPoints << 1) - 2], points[(nPoints << 1) - 1], 0);
			PolylineTo((HDC)this->hdcBmp, (POINT*)points, (DWORD)nPoints);
		}
#endif
	}
	else
	{
		if (this->currBrush != b)
		{
			GDIBrush *brush = (GDIBrush*)(this->currBrush = b);
			SelectObject((HDC)this->hdcBmp, (HBRUSH)brush->hbrush);
		}
		
		Polygon((HDC)this->hdcBmp, (POINT*)points, (int)nPoints);
	}

	return true;
}

Bool Media::GDIImage::DrawPolyPolygonI(Int32 *points, UInt32 *pointCnt, UOSInt nPointCnt, DrawPen *p, DrawBrush *b)
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

	if (p)
	{
		if (this->currPen != p)
		{
			GDIPen *pen = (GDIPen*)(this->currPen = p);
			SelectObject((HDC)this->hdcBmp, (HPEN)pen->hpen);
		}
		if ((penWidth = ((Media::GDIPen*)p)->thick) < 1)
			penWidth = 1;
	}

	if (b == 0)
	{
		Int32 *pts = points;
		if (p)
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
				PolylineAccel(this->hdcBmp, points, j, left, top, width, height);
				PolylineAccel(this->hdcBmp, line2, 2, left, top, width, height);
#else
				MoveToEx((HDC)this->hdcBmp, points[(j << 1) - 2], points[(j << 1) - 1], 0);
				PolylineTo((HDC)this->hdcBmp, (POINT*)points, j);
#endif

				points += j << 1;
				i++;
			}
		}
		points = pts;
	}
	else
	{
		if (this->info->atype == Media::AT_NO_ALPHA && (((GDIBrush*)b)->oriColor & 0xff000000) == 0xff000000)
		{
			if (this->currBrush != b)
			{
				GDIBrush *brush = (GDIBrush*)(this->currBrush = b);
				SelectObject((HDC)this->hdcBmp, (HBRUSH)brush->hbrush);
			}
#ifdef _WIN32_WCE
			i = 0;
			while (i < nPointCnt)
			{
				j = pointCnt[i];

				PolygonAccel(this->hdcBmp, points, j, left, top, width, height, Math::Double2Int32(penWidth));

				points += j << 1;
				i++;
			}
#else
			PolyPolygonAccel((HDC)this->hdcBmp, points, pointCnt, nPointCnt, left, top, width, height, Math::Double2Int32(penWidth));
#endif
		}
		else if (this->bmpBits && (((GDIBrush*)b)->oriColor & 0xff000000) == 0xff000000)
		{
			UInt32 bCol = ((GDIBrush*)b)->oriColor;
			BITMAPINFOHEADER bmih;
			bmih.biSize = sizeof(bmih);
			bmih.biWidth = (LONG)this->info->dispWidth;
			bmih.biHeight = (LONG)this->info->dispHeight;
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
				MemClear(pbits, this->info->dispWidth * this->info->dispHeight * 4);
				SelectObject(hdcBmp, hBmp);
				HBRUSH hbr = CreateSolidBrush(0xffffff);
				SelectObject(hdcBmp, hbr);
				PolyPolygon(hdcBmp, (POINT*)points, (const Int32*)pointCnt, (Int32)nPointCnt);
				DeleteObject(hbr);

				UInt32 *sptr = (UInt32*)pbits;
				UInt32 *dptr = (UInt32*)this->bmpBits;
				OSInt cnt = this->info->dispWidth * this->info->dispHeight;
				while (cnt-- > 0)
				{
					if (*sptr)
					{
						*dptr = bCol;
					}
					sptr++;
					dptr++;
				}
				if (p)
				{
					PolyPolyline((HDC)this->hdcBmp, (POINT*)points, (const DWORD*)pointCnt, (Int32)nPointCnt);
				}

				DeleteObject(hBmp);
			}
			DeleteDC(hdcBmp);
		}
		else
		{
			UInt32 bCol = ((GDIBrush*)b)->oriColor;
			BITMAPINFOHEADER bmih;
			bmih.biSize = sizeof(bmih);
			bmih.biWidth = (LONG)this->info->dispWidth;
			bmih.biHeight = (LONG)this->info->dispHeight;
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
				MemClear(pbits, this->info->dispWidth * this->info->dispHeight * 4);
				SelectObject(hdcBmp, hBmp);
				HBRUSH hbr = CreateSolidBrush(0xffffff);
				SelectObject(hdcBmp, hbr);
				PolyPolygon(hdcBmp, (POINT*)points, (const Int32*)pointCnt, (Int32)nPointCnt);
				DeleteObject(hbr);


				UInt32 *ptr = (UInt32*)pbits;
				OSInt cnt = this->info->dispWidth * this->info->dispHeight;
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
				AlphaBlend((HDC)this->hdcBmp, 0, 0, (int)this->info->dispWidth, (int)this->info->dispHeight, hdcBmp, 0, 0, (int)this->info->dispWidth, (int)this->info->dispHeight, bf);

				if (p)
				{
					PolyPolyline((HDC)this->hdcBmp, (POINT*)points, (const DWORD*)pointCnt, (Int32)nPointCnt);
				}

				DeleteObject(hBmp);
			}
			DeleteDC(hdcBmp);
		}
	}

	return true;
}

Bool Media::GDIImage::DrawPolyline(Double *points, UOSInt nPoints, DrawPen *p)
{
	UOSInt i = nPoints << 1;
	Bool ret;
	Int32 *ipts = MemAlloc(Int32, i);
	while (i-- > 0)
	{
		ipts[i] = Math::Double2Int32(points[i]);
	}
	ret = DrawPolylineI(ipts, nPoints, p);
	MemFree(ipts);
	return ret;
}

Bool Media::GDIImage::DrawPolygon(Double *points, UOSInt nPoints, DrawPen *p, DrawBrush *b)
{
	UOSInt i = nPoints << 1;
	Bool ret;
	Int32 *ipts = MemAlloc(Int32, i);
	while (i-- > 0)
	{
		ipts[i] = Math::Double2Int32(points[i]);
	}
	ret = DrawPolygonI(ipts, nPoints, p, b);
	MemFree(ipts);
	return ret;
}

Bool Media::GDIImage::DrawPolyPolygon(Double *points, UInt32 *pointCnt, UOSInt nPointCnt, DrawPen *p, DrawBrush *b)
{
	UOSInt i = 0;
	UOSInt j = nPointCnt;
	Bool ret;
	while (j-- > 0)
	{
		i += pointCnt[j];
	}
	i = i << 1;
	Int32 *ipts = MemAlloc(Int32, i);
	while (i-- > 0)
	{
		ipts[i] = Math::Double2Int32(points[i]);
	}
	ret = DrawPolyPolygonI(ipts, pointCnt, nPointCnt, p, b);
	MemFree(ipts);
	return ret;
}

Bool Media::GDIImage::DrawRect(Double x, Double y, Double w, Double h, DrawPen *p, DrawBrush *b)
{
	if (b)
	{
		GDIBrush *brush = (GDIBrush*)b;
		if (this->bmpBits)
		{
			OSInt ix = Math::Double2Int32(x);
			OSInt iy = Math::Double2Int32(y);
			OSInt iw = Math::Double2Int32(w);
			OSInt ih = Math::Double2Int32(h);
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
			if ((ix + iw) > (OSInt)this->width)
			{
				iw = this->width - ix;
			}
			if ((iy + ih) > (OSInt)this->height)
			{
				ih = this->height - iy;
			}
			if (iw > 0 && ih > 0)
			{
				if ((brush->oriColor & 0xff000000) == 0xff000000)
				{
					ImageUtil_ImageColorFill32((this->height - iy - ih) * this->width * 4 + ix * 4 + (UInt8*)this->bmpBits, iw, ih, this->width << 2, brush->oriColor);
				}
				else
				{
					ImageUtil_ImageColorBlend32((this->height - iy - ih) * this->width * 4 + ix * 4 + (UInt8*)this->bmpBits, iw, ih, this->width << 2, brush->oriColor);
				}
			}
		}
		else
		{
			RECT rc;
			rc.left = Math::Double2Int32(x);
			rc.top = Math::Double2Int32(y);
			rc.right = Math::Double2Int32(x + w);
			rc.bottom = Math::Double2Int32(y + h);
			Int32 i = 10;
			
			while (FillRect((HDC)this->hdcBmp, &rc, (HBRUSH)brush->hbrush) == 0)
			{
				if (i-- <= 0)
				{
					break;
				}
				else
				{
					Sync::Thread::Sleep(10);
				}
			}
		}
	}

	if (p)
	{
		GDIPen *pen = (GDIPen*)p;
		if (this->bmpBits && (pen->oriColor & 0xff000000) == 0xff000000)
		{
			OSInt ix = Math::Double2Int32(x);
			OSInt iy = Math::Double2Int32(y);
			OSInt iw = Math::Double2Int32(w);
			OSInt ih = Math::Double2Int32(h);
			if (ix >= 0 && iy >= 0 && iw + ix < (OSInt)this->width && ih + iy < (OSInt)this->height)
			{
				ImageUtil_DrawRectNA32((this->height - iy - ih - 1) * this->width * 4 + ix * 4 + (UInt8*)this->bmpBits, iw + 1, ih + 1, this->width << 2, pen->oriColor);
			}
			else
			{
				POINT pts[5];
				if (this->currPen != p)
				{
					GDIPen *pen = (GDIPen*)(this->currPen = p);
					SelectObject((HDC)this->hdcBmp, (HPEN)pen->hpen);
				}
				pts[4].x = pts[1].x = pts[0].x = Math::Double2Int32(x);
				pts[4].y = pts[3].y = pts[0].y = Math::Double2Int32(y);
				pts[3].x = pts[2].x = Math::Double2Int32(x + w);
				pts[2].y = pts[1].y = Math::Double2Int32(y + h);
				Polyline((HDC)this->hdcBmp, pts, 5);
			}
		}
		else
		{
			POINT pts[5];
			if (this->currPen != p)
			{
				GDIPen *pen = (GDIPen*)(this->currPen = p);
				SelectObject((HDC)this->hdcBmp, (HPEN)pen->hpen);
			}
			pts[4].x = pts[1].x = pts[0].x = Math::Double2Int32(x);
			pts[4].y = pts[3].y = pts[0].y = Math::Double2Int32(y);
			pts[3].x = pts[2].x = Math::Double2Int32(x + w);
			pts[2].y = pts[1].y = Math::Double2Int32(y + h);
			Polyline((HDC)this->hdcBmp, pts, 5);
		}
	}

	return true;
}

Bool Media::GDIImage::DrawEllipse(Double tlx, Double tly, Double w, Double h, DrawPen *p, DrawBrush *b)
{
	if (this->info->atype == Media::AT_NO_ALPHA)
	{
		if (b == 0)
		{
			if (p != 0)
			{
				if (this->currPen != p)
				{
					GDIPen *pen = (GDIPen*)p;
					SelectObject((HDC)this->hdcBmp, pen->hpen);
				}
				SelectObject((HDC)this->hdcBmp, GetStockObject(NULL_BRUSH));
				this->currBrush = 0;

				Ellipse((HDC)this->hdcBmp, Math::Double2Int32(tlx), Math::Double2Int32(tly), Math::Double2Int32(tlx + w), Math::Double2Int32(tly + h));
			}
		}
		else
		{
			if (p != 0)
			{
				if (this->currPen != p)
				{
					GDIPen *pen = (GDIPen*)p;
					SelectObject((HDC)this->hdcBmp, pen->hpen);
				}
			}
			else
			{
				SelectObject((HDC)this->hdcBmp, GetStockObject(NULL_PEN));
				this->currPen = 0;
			}
			if (this->currBrush != b)
			{
				GDIBrush *brush = (GDIBrush*)b;
				SelectObject((HDC)this->hdcBmp, brush->hbrush);
			}
			Ellipse((HDC)this->hdcBmp, Math::Double2Int32(tlx), Math::Double2Int32(tly), Math::Double2Int32(tlx + w), Math::Double2Int32(tly + h));
		}
	}
	else
	{
		Media::GDIImage *tmpImg = (Media::GDIImage*)eng->CreateImage32(this->width, this->height, Media::AT_NO_ALPHA);
		if (tmpImg == 0)
			return false;
		UInt8 *imgPtr = (UInt8*)tmpImg->bmpBits;
		UInt8 *imgPtr2 = (UInt8*)this->bmpBits;
		OSInt imgW = this->width;
		OSInt imgH = this->height;
		Int32 c1 = 0;
		Int32 c2 = 0;
		ImageUtil_ColorFill32(imgPtr, imgW * imgH, 0xff000000);
		if (p)
		{
			SelectObject((HDC)tmpImg->hdcBmp, (HPEN)eng->GetBlackPen());
			c1 = ((Media::GDIPen*)p)->oriColor;
		}
		else
		{
			SelectObject((HDC)tmpImg->hdcBmp, GetStockObject(NULL_PEN));
		}
		if (b)
		{
			SelectObject((HDC)tmpImg->hdcBmp, (HBRUSH)eng->GetWhiteBrush());
			c2 = ((Media::GDIBrush*)b)->oriColor;
		}
		else
		{
			SelectObject((HDC)tmpImg->hdcBmp, GetStockObject(NULL_BRUSH));
		}
		Ellipse((HDC)tmpImg->hdcBmp, Math::Double2Int32(tlx), Math::Double2Int32(tly), Math::Double2Int32(tlx + w), Math::Double2Int32(tly + h));
		ImageUtil_ImageMask2ABlend32(imgPtr, imgPtr2, imgW, imgH, imgW << 2, imgW << 2, c1, c2);
		
		eng->DeleteImage(tmpImg);
	}
	return true;
}

Bool Media::GDIImage::DrawStringUTF8(Double tlx, Double tly, const UTF8Char *str, DrawFont *f, DrawBrush *b)
{
	const WChar *wptr = Text::StrToWCharNew(str);
	Bool ret = DrawString(tlx, tly, wptr, f, b);
	Text::StrDelNew(wptr);
	return ret;
}

Bool Media::GDIImage::DrawString(Double tlx, Double tly, const WChar *str, DrawFont *f, DrawBrush *b)
{
	GDIBrush *brush = (GDIBrush*)b;
	const WChar *src = str;
	while (*src++);
	if (this->bmpBits == 0 || (this->info->atype == Media::AT_NO_ALPHA && (brush->oriColor & 0xff000000) == 0xff000000))
	{
		SetTextColor((HDC)this->hdcBmp, brush->color);
		if (this->currFont != f)
		{
			GDIFont *font = (GDIFont*)(this->currFont = f);
			SelectObject((HDC)this->hdcBmp, (HFONT)font->hfont);
		}
		Double sz[2];
		if (this->strAlign == Media::DrawEngine::DRAW_POS_CENTER || this->strAlign == Media::DrawEngine::DRAW_POS_CENTERLEFT || this->strAlign == Media::DrawEngine::DRAW_POS_CENTERRIGHT)
		{
			GetTextSize(f, str, src - str - 1, sz);
		}
		else
		{
			sz[1] = 0;
		}
#ifdef _WIN32_WCE
		ExtTextOut((HDC)this->hdcBmp, Math::Double2Int32(tlx), Math::Double2Int32(tly - (sz[1] * 0.5)), 0, 0, str, (Int32)(src - str - 1), 0);
#else
		TextOutW((HDC)this->hdcBmp, Math::Double2Int32(tlx), Math::Double2Int32(tly - (sz[1] * 0.5)), str, (Int32)(src - str - 1));
#endif
	}
	else
	{
		Double sz[2];
		GetTextSize(f, str, src - str - 1, sz);
		Media::GDIImage *tmpImg = (Media::GDIImage*)this->eng->CreateImage32((Int32)sz[0] + 1, (Int32)sz[1] + 1, Media::AT_NO_ALPHA);
		Media::DrawBrush *b2 = tmpImg->NewBrushARGB(0xffffffff);
		tmpImg->DrawString(0, 0, str, f, b2);
		tmpImg->DelBrush(b2);
		ImageUtil_ColorReplace32((UInt8*)tmpImg->bmpBits, tmpImg->info->storeWidth, tmpImg->info->storeHeight, ((GDIBrush*)b)->oriColor);
		tmpImg->info->atype = Media::AT_ALPHA;
		Double x;
		Double y;
		if (this->strAlign == Media::DrawEngine::DRAW_POS_TOPLEFT || this->strAlign == Media::DrawEngine::DRAW_POS_CENTERLEFT || this->strAlign == Media::DrawEngine::DRAW_POS_BOTTOMLEFT)
		{
			x = tlx;
		}
		else if (this->strAlign == Media::DrawEngine::DRAW_POS_TOPRIGHT || this->strAlign == Media::DrawEngine::DRAW_POS_CENTERRIGHT || this->strAlign == Media::DrawEngine::DRAW_POS_BOTTOMRIGHT)
		{
			x = tlx - sz[0];
		}
		else
		{
			x = tlx - sz[0] * 0.5;
		}
		if (this->strAlign == Media::DrawEngine::DRAW_POS_TOPLEFT || this->strAlign == Media::DrawEngine::DRAW_POS_TOPCENTER || this->strAlign == Media::DrawEngine::DRAW_POS_TOPRIGHT)
		{
			y = tly;
		}
		else if (this->strAlign == Media::DrawEngine::DRAW_POS_BOTTOMLEFT || this->strAlign == Media::DrawEngine::DRAW_POS_BOTTOMCENTER || this->strAlign == Media::DrawEngine::DRAW_POS_BOTTOMRIGHT)
		{
			y = tly - sz[1];
		}
		else
		{
			y = tly - sz[1] * 0.5;
		}
		this->DrawImagePt(tmpImg, Math::Double2Int32(x), Math::Double2Int32(y));
		this->eng->DeleteImage(tmpImg);
	}
	return true;
}

Bool Media::GDIImage::DrawStringRotUTF8(Double centX, Double centY, const UTF8Char *str, DrawFont *f, DrawBrush *b, Double angleDegree)
{
	const WChar *wptr = Text::StrToWCharNew(str);
	Bool ret = DrawStringRot(centX, centY, wptr, f, b, angleDegree);
	Text::StrDelNew(wptr);
	return ret;
}

Bool Media::GDIImage::DrawStringRot(Double centX, Double centY, const WChar *str, DrawFont *f, DrawBrush *b, Double angleDegree)
{
	GDIBrush *brush = (GDIBrush*)b;
	GDIFont *font = (GDIFont*)f;
	const WChar *src = str;
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
	lf.lfEscapement = Math::Double2Int32(angleDegree * 10.0);
	hfont = CreateFontIndirectW(&lf);

	Int32 bnds[8];
	OSInt px;
	OSInt py;
	GetStringBoundRotW(bnds, Math::Double2Int32(centX), Math::Double2Int32(centY), str, f, angleDegree, &px, &py);
	HGDIOBJ ofont = SelectObject((HDC)this->hdcBmp, (HFONT)hfont);
	TextOutW((HDC)this->hdcBmp, (int)px, (int)py, str, (Int32)(src - str - 1));
	SelectObject((HDC)this->hdcBmp, ofont);
	DeleteObject(hfont);
	return true;
}

Bool Media::GDIImage::DrawStringBUTF8(Double dx, Double dy, const UTF8Char *str1, DrawFont *f, DrawBrush *b, OSInt buffSize)
{
	const WChar *wptr = Text::StrToWCharNew(str1);
	Bool ret = DrawStringB(dx, dy, wptr, f, b, buffSize);
	Text::StrDelNew(wptr);
	return ret;
}

Bool Media::GDIImage::DrawStringB(Double dx, Double dy, const WChar *str1, DrawFont *f, DrawBrush *b, OSInt buffSize)
{
	OSInt px = Math::Double2Int32(dx);
	OSInt py = Math::Double2Int32(dy);
	GDIBrush *brush = (GDIBrush*)b;
	const WChar *src = str1;
	while (*src++);

	OSInt drawX;
	OSInt drawY;
	Int32 sz[2];
	Int32 bnds[8];
	Media::GDIImage *gimg;
	OSInt swidth;
	OSInt sheight;
	OSInt sx;
	OSInt sy;
	OSInt dwidth;
	OSInt dheight;
	Int32 maxV;
	Int32 minV;

	GetStringBoundW(bnds, px, py, str1, f, &px, &py);
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
	dwidth = this->width - px;
	dheight = this->height - py;

	if (dwidth < 0)
	{
	}
	else if (dheight < 0)
	{
	}
	else
	{
		gimg = (Media::GDIImage*)eng->CreateImage32(swidth = sz[0] + (buffSize << 1), sheight = sz[1] + (buffSize << 1), Media::AT_ALPHA);
		if (gimg == 0)
		{
			return false;
		}

		if (px < buffSize)
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
		if (py < buffSize)
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
		if (dwidth + buffSize < swidth)
		{
			swidth = dwidth + buffSize;
		}
		if (dheight + buffSize < sheight)
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
			if (gimg->currFont != f)
			{
				GDIFont *font = (GDIFont*)(gimg->currFont = f);
				SelectObject((HDC)gimg->hdcBmp, (HFONT)font->hfont);
			}
#ifdef _WIN32_WCE
			ExtTextOut((HDC)gimg->hdcBmp, drawX + buffSize, drawY + buffSize, 0, 0, str1, (Int32)(src - str1 - 1), 0);
#else
			TextOutW((HDC)gimg->hdcBmp, (int)(drawX + buffSize), (int)(drawY + buffSize), str1, (Int32)(src - str1 - 1));
#endif

			OSInt bpl = (sz[0] + (buffSize << 1)) << 2;
			OSInt dbpl = this->width << 2;
			Int32 color = brush->oriColor;
			UInt8 *pbits = (UInt8*)gimg->bmpBits;
			UInt8 *dbits = (UInt8*)this->bmpBits;
			ImageUtil_ImageColorBuffer32(pbits + bpl * buffSize + buffSize * 4, sz[0], sz[1], bpl, buffSize);
			if (py + sheight > (OSInt)this->height)
			{
				sheight = this->height - py;
			}
			if (px + swidth > (OSInt)this->width)
			{
				swidth = this->width - px;
			}
			if (swidth > 0 && sheight > 0)
			{
				if (this->bmpBits)
				{
					pbits = ((UInt8*)pbits) + ((gimg->height - sy - sheight) * bpl) + (sx << 2);
					dbits = ((UInt8*)dbits) + ((this->height - py - sheight) * dbpl) + (px << 2);

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
					pbits = ((UInt8*)pbits) + ((gimg->height - sy - sheight) * bpl) + (sx << 2);
					dbits = ((UInt8*)dbits) + ((this->height - py - sheight) * dbpl) + (px << 2);

					ImageUtil_ImageColorReplace32((UInt8*)pbits, (UInt8*)pbits, swidth, sheight, bpl, bpl, color);
					gimg->SetHDPI(this->GetHDPI());
					gimg->SetVDPI(this->GetVDPI());
					this->DrawImagePt(gimg, Math::OSInt2Double(px - sx), Math::OSInt2Double(py - sy));
				}
			}
		}
		eng->DeleteImage(gimg);
	}

	return true;
}

Bool Media::GDIImage::DrawStringRotBUTF8(Double dx, Double dy, const UTF8Char *str1, DrawFont *f, DrawBrush *b, Double angleDegree, OSInt buffSize)
{
	const WChar *wptr = Text::StrToWCharNew(str1);
	Bool ret = this->DrawStringRotB(dx, dy, wptr, f, b, angleDegree, buffSize);
	Text::StrDelNew(wptr);
	return ret;
}

Bool Media::GDIImage::DrawStringRotB(Double dx, Double dy, const WChar *str1, DrawFont *f, DrawBrush *b, Double angleDegree, OSInt buffSize)
{
	OSInt px = Math::Double2Int32(dx);
	OSInt py = Math::Double2Int32(dy);
	GDIBrush *brush = (GDIBrush*)b;
	const WChar *src = str1;
	while (*src++);

	OSInt drawX;
	OSInt drawY;
	Int32 sz[2];
	Int32 bnds[8];
	Media::GDIImage *gimg;
	OSInt swidth;
	OSInt sheight;
	OSInt sx;
	OSInt sy;
	OSInt dwidth;
	OSInt dheight;
	Int32 maxV;
	Int32 minV;
	GetStringBoundRotW(bnds, dx, dy, str1, f, angleDegree, &px, &py);
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
	dwidth = this->width - px;
	dheight = this->height - py;

	if (dwidth < 0)
	{
	}
	else if (dheight < 0)
	{
	}
	else
	{
		gimg = (Media::GDIImage*)eng->CreateImage32(swidth = sz[0] + (buffSize << 1), sheight = sz[1] + (buffSize << 1), Media::AT_NO_ALPHA);
		if (gimg == 0)
			return false;

		if (px < buffSize)
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
		if (py < buffSize)
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
		if (dwidth + buffSize < swidth)
		{
			swidth = dwidth + buffSize;
		}
		if (dheight + buffSize < sheight)
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
			GDIFont *font = (GDIFont*)f;
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
			lf.lfEscapement = Math::Double2Int32(angleDegree * 10);
			hfont = CreateFontIndirectW(&lf);

			HGDIOBJ ofont = SelectObject((HDC)gimg->hdcBmp, (HFONT)hfont);
			TextOutW((HDC)gimg->hdcBmp, (int)(drawX + buffSize), (int)(drawY + buffSize), str1, (Int32)(src - str1 - 1));
			SelectObject((HDC)gimg->hdcBmp, ofont);
			DeleteObject(hfont);


/*			HGDIOBJ ofont = SelectObject((HDC)gimg->hdcBmp, (HFONT)hfont);
			TextOutW((HDC)gimg->hdcBmp, drawX, drawY, str1, (Int32)(src - str1 - 1));
			SelectObject((HDC)gimg->hdcBmp, ofont);
			DeleteObject(hfont);*/

			OSInt bpl = (sz[0] + (buffSize << 1)) << 2;
			OSInt dbpl = this->width << 2;
			Int32 color = brush->oriColor;
			UInt8 *pbits = (UInt8*)gimg->bmpBits;
			UInt8 *dbits = (UInt8*)this->bmpBits;
			ImageUtil_ImageColorBuffer32(pbits + bpl * buffSize + buffSize * 4, sz[0], sz[1], bpl, buffSize);

			pbits = ((UInt8*)pbits) + ((gimg->height - sy - sheight) * bpl) + (sx << 2);
			if (py + sheight > (OSInt)this->height)
			{
				sheight = this->height - py;
			}
			if (px + swidth > (OSInt)this->width)
			{
				swidth = this->width - px;
			}
			dbits = ((UInt8*)dbits) + ((this->height - py - sheight) * dbpl) + (px << 2);
			ImageUtil_ImageColorReplace32(pbits, dbits, swidth, sheight, bpl, dbpl, color);
		}
		eng->DeleteImage(gimg);
	}

	return true;
}

Bool Media::GDIImage::DrawImagePt(DrawImage *img, Double tlx, Double tly)
{
	GDIImage *image = (GDIImage *)img;
	if (this->hBmp == 0)
	{
		return this->DrawImageRect(img, Math::Double2Int32(tlx), Math::Double2Int32(tly), Math::Double2Int32(tlx + image->GetWidth() * this->info->hdpi / image->GetHDPI()), Math::Double2Int32(tly + image->GetHeight() * this->info->vdpi / image->GetVDPI()));
	}
	if (image->info->atype == Media::AT_NO_ALPHA)
	{
		if (this->IsOffScreen())
		{
			Int32 x = Math::Double2Int32(tlx);
			Int32 y = Math::Double2Int32(tly);
			Int32 sx = 0;
			Int32 sy = 0;
			OSInt w = image->width;
			OSInt h = image->height;
			OSInt bpl = this->width << 2;
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
			if (x + w > (OSInt)this->width)
			{
				w = this->width - x;
			}
			if (y + h > (OSInt)this->height)
			{
				h = this->height - y;
			}
			if (w > 0 && h > 0)
			{
				img->CopyBits(sx, sy, ((UInt8*)this->bmpBits) + (this->height - y - 1) * bpl + (x << 2), -bpl, w, h);
			}
		}
		else
		{
			BitBlt((HDC)this->hdcBmp, Math::Double2Int32(tlx), Math::Double2Int32(tly), (int)image->width, (int)image->height, (HDC)image->hdcBmp, 0, 0, SRCCOPY);
		}
	}
	else
	{
#if !defined(_WIN32_WCE)
		if (this->IsOffScreen())
		{
			OSInt w = image->width;
			OSInt h = image->height;
			UInt8 *dbits = (UInt8*)this->bmpBits;
			UInt8 *sbits = (UInt8*)image->bmpBits;
			OSInt dbpl = this->width << 2;
			OSInt sbpl = image->width << 2;

			if (tlx < 0)
			{
				w += Math::Double2Int32(tlx);
				sbits -= Math::Double2Int32(tlx) << 2;
				tlx = 0;
			}
			if (tly < 0)
			{
				h += Math::Double2Int32(tly);
				tly = 0;
			}

			if (tlx + w > this->width)
			{
				w = this->width - Math::Double2Int32(tlx);
			}
			if (tly + h > this->height)
			{
				sbits += (h - (this->height - Math::Double2Int32(tly))) * sbpl;
				h = this->height - Math::Double2Int32(tly);
			}
			if (w > 0 && h > 0)
			{
				this->eng->iab->SetSourceProfile(image->info->color);
				this->eng->iab->SetDestProfile(this->info->color);
				this->eng->iab->SetOutputProfile(this->info->color);
				this->eng->iab->Blend(dbits + (this->height - (Int32)tly - h) * dbpl + (((Int32)tlx) * 4), dbpl, sbits, sbpl, w, h, image->info->atype);
			}
		}
		else
		{
			if (image->info->atype == Media::AT_PREMUL_ALPHA)
			{
				BLENDFUNCTION bf;
				bf.SourceConstantAlpha = 255;
				bf.AlphaFormat = AC_SRC_ALPHA;
				bf.BlendOp = AC_SRC_OVER;
				bf.BlendFlags = 0;
				AlphaBlend((HDC)this->hdcBmp, Math::Double2Int32(tlx), Math::Double2Int32(tly), (int)image->width, (int)image->height, (HDC)image->hdcBmp, 0, 0, (int)image->width, (int)image->height, bf);
			}
			else
			{
				BLENDFUNCTION bf;
				bf.SourceConstantAlpha = 255;
				bf.AlphaFormat = AC_SRC_ALPHA;
				bf.BlendOp = AC_SRC_OVER;
				bf.BlendFlags = 0;
				AlphaBlend((HDC)this->hdcBmp, Math::Double2Int32(tlx), Math::Double2Int32(tly), (int)image->width, (int)image->height, (HDC)image->hdcBmp, 0, 0, (int)image->width, (int)image->height, bf);
			}
		}
#elif (!defined(_WIN32_WCE) || (_WIN32_WCE >= 0x0500))
		if (image->info->atype == Media::AT_PREMUL_ALPHA)
		{
			BLENDFUNCTION bf;
			bf.SourceConstantAlpha = 255;
			bf.AlphaFormat = AC_SRC_ALPHA;
			bf.BlendOp = AC_SRC_OVER;
			bf.BlendFlags = 0;
			AlphaBlend((HDC)this->hdcBmp, Math::Double2Int32(tlx), Math::Double2Int32(tly), (int)image->width, (int)image->height, (HDC)image->hdcBmp, 0, 0, (int)image->width, (int)image->height, bf);
		}
		else
		{
			BLENDFUNCTION bf;
			bf.SourceConstantAlpha = 255;
			bf.AlphaFormat = AC_SRC_ALPHA;
			bf.BlendOp = AC_SRC_OVER;
			bf.BlendFlags = 0;
			AlphaBlend((HDC)this->hdcBmp, Math::Double2Int32(tlx), Math::Double2Int32(tly), (int)image->width, (int)image->height, (HDC)image->hdcBmp, 0, 0, (int)image->width, (int)image->height, bf);
		}
#else
		BitBlt((HDC)this->hdcBmp, Math::Double2Int32(tlx), Math::Double2Int32(tly), (int)image->width, (int)image->height, (HDC)image->hdcBmp, 0, 0, SRCCOPY);
#endif
	}
	return true;
}

Bool Media::GDIImage::DrawImagePt2(Media::StaticImage *img, Double tlx, Double tly)
{
	if (this->IsOffScreen() && img->GetImageType() == Media::Image::IT_STATIC)
	{
		Media::StaticImage *simg = img;
		simg->To32bpp();
		if (simg->info->atype == Media::AT_NO_ALPHA)
		{
			Int32 x = Math::Double2Int32(tlx);
			Int32 y = Math::Double2Int32(tly);
			Int32 sx = 0;
			Int32 sy = 0;
			OSInt w = simg->info->dispWidth;
			OSInt h = simg->info->dispHeight;
			OSInt bpl = this->width << 2;
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
			if (x + w > (OSInt)this->width)
			{
				w = this->width - x;
			}
			if (y + h > (OSInt)this->height)
			{
				h = this->height - y;
			}
			if (w > 0 && h > 0)
			{
				ImageCopy_ImgCopy(simg->data + (sy * simg->info->storeWidth << 2) + (sx << 2), ((UInt8*)this->bmpBits) + (this->height - y - 1) * bpl + (x << 2), w << 2, h, simg->info->storeWidth << 2, -(OSInt)this->width << 2);
			}
		}
		else
		{
			OSInt w = simg->info->dispWidth;
			OSInt h = simg->info->dispHeight;
			UInt8 *dbits = (UInt8*)this->bmpBits;
			UInt8 *sbits = simg->data;
			OSInt dbpl = this->width << 2;
			OSInt sbpl = simg->info->storeWidth << 2;

			if (tlx < 0)
			{
				w += Math::Double2Int32(tlx);
				sbits -= Math::Double2Int32(tlx) << 2;
				tlx = 0;
			}
			if (tly < 0)
			{
				h += Math::Double2Int32(tly);
				sbits -= Math::Double2Int32(tly) * sbpl;
				tly = 0;
			}

			if (tlx + w > this->width)
			{
				w = this->width - Math::Double2Int32(tlx);
			}
			if (tly + h > this->height)
			{
				h = this->height - Math::Double2Int32(tly);
			}
			if (w > 0 && h > 0)
			{
				this->eng->iab->SetSourceProfile(simg->info->color);
				this->eng->iab->SetDestProfile(this->info->color);
				this->eng->iab->SetOutputProfile(this->info->color);
				this->eng->iab->Blend(dbits + (this->height - Math::Double2Int32(tly) - 1) * dbpl + (Math::Double2Int32(tlx) * 4), -dbpl, sbits, sbpl, w, h, simg->info->atype);
			}
		}
		return true;
	}
	else
	{
		Media::DrawImage *dimg = this->eng->ConvImage(img);
		if (dimg)
		{
			DrawImagePt(dimg, tlx, tly);
			this->eng->DeleteImage(dimg);
			return true;
		}
		return false;
	}
}

Bool Media::GDIImage::DrawImagePt3(DrawImage *img, Double destX, Double destY, Double srcX, Double srcY, Double srcW, Double srcH)
{
	GDIImage *image = (GDIImage *)img;
	if (this->hBmp == 0)
	{
		return this->DrawImageRect(img, Math::Double2Int32(destX), Math::Double2Int32(destY), Math::Double2Int32(destX + srcW * this->info->hdpi / image->GetHDPI()), Math::Double2Int32(destY + srcH * this->info->vdpi / image->GetVDPI()));
	}
	if (image->info->atype == Media::AT_NO_ALPHA)
	{
		if (this->IsOffScreen())
		{
			Int32 x = Math::Double2Int32(destX);
			Int32 y = Math::Double2Int32(destY);
			Int32 sx = Math::Double2Int32(srcX);
			Int32 sy = Math::Double2Int32(srcY);
			OSInt w = Math::Double2Int32(srcW);
			OSInt h = Math::Double2Int32(srcH);
			OSInt bpl = this->width << 2;
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
			if (x + w > (OSInt)this->width)
			{
				w = this->width - x;
			}
			if (y + h > (OSInt)this->height)
			{
				h = this->height - y;
			}
			if (w > 0 && h > 0)
			{
				img->CopyBits(sx, sy, ((UInt8*)this->bmpBits) + (this->height - y - 1) * bpl + (x << 2), -bpl, w, h);
			}
		}
		else
		{
			BitBlt((HDC)this->hdcBmp, Math::Double2Int32(destX), Math::Double2Int32(destY), Math::Double2Int32(srcW), Math::Double2Int32(srcH), (HDC)image->hdcBmp, Math::Double2Int32(srcX), Math::Double2Int32(srcY), SRCCOPY);
		}
	}
	else
	{
#if !defined(_WIN32_WCE)
		if (this->IsOffScreen())
		{
			Int32 x = Math::Double2Int32(destX);
			Int32 y = Math::Double2Int32(destY);
			Int32 sx = Math::Double2Int32(srcX);
			Int32 sy = Math::Double2Int32(srcY);
			OSInt w = Math::Double2Int32(srcW);
			OSInt h = Math::Double2Int32(srcH);
			UInt8 *dbits = (UInt8*)this->bmpBits;
			UInt8 *sbits = (UInt8*)image->bmpBits;
			OSInt dbpl = this->width << 2;
			OSInt sbpl = image->width << 2;
			OSInt sh = image->height;

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
			if (x + w > (OSInt)this->width)
			{
				w = this->width - x;
			}
			if (y + h > (OSInt)this->height)
			{
				h = this->height - y;
			}
			if (w > 0 && h > 0)
			{
				this->eng->iab->SetSourceProfile(image->info->color);
				this->eng->iab->SetDestProfile(this->info->color);
				this->eng->iab->SetOutputProfile(this->info->color);
				this->eng->iab->Blend(dbits + (this->height - y - h) * dbpl + (x * 4), dbpl, sbits + (sh - sy - h) * sbpl + (sx << 2), sbpl, w, h, image->info->atype);
			}
		}
		else
		{
			if (image->info->atype == Media::AT_PREMUL_ALPHA)
			{
				BLENDFUNCTION bf;
				bf.SourceConstantAlpha = 255;
				bf.AlphaFormat = AC_SRC_ALPHA;
				bf.BlendOp = AC_SRC_OVER;
				bf.BlendFlags = 0;
				AlphaBlend((HDC)this->hdcBmp, Math::Double2Int32(destX), Math::Double2Int32(destY), Math::Double2Int32(srcW), Math::Double2Int32(srcH), (HDC)image->hdcBmp, Math::Double2Int32(srcX), Math::Double2Int32(srcY), Math::Double2Int32(srcW), Math::Double2Int32(srcH), bf);
			}
			else
			{
				BLENDFUNCTION bf;
				bf.SourceConstantAlpha = 255;
				bf.AlphaFormat = AC_SRC_ALPHA;
				bf.BlendOp = AC_SRC_OVER;
				bf.BlendFlags = 0;
				AlphaBlend((HDC)this->hdcBmp, Math::Double2Int32(destX), Math::Double2Int32(destY), Math::Double2Int32(srcW), Math::Double2Int32(srcH), (HDC)image->hdcBmp, Math::Double2Int32(srcX), Math::Double2Int32(srcY), Math::Double2Int32(srcW), Math::Double2Int32(srcH), bf);
			}
		}
#elif (!defined(_WIN32_WCE) || (_WIN32_WCE >= 0x0500))
		if (image->info->atype == Media::AT_PREMUL_ALPHA)
		{
			BLENDFUNCTION bf;
			bf.SourceConstantAlpha = 255;
			bf.AlphaFormat = AC_SRC_ALPHA;
			bf.BlendOp = AC_SRC_OVER;
			bf.BlendFlags = 0;
			AlphaBlend((HDC)this->hdcBmp, Math::Double2Int32(destX), Math::Double2Int32(destY), Math::Double2Int32(srcW), Math::Double2Int32(srcH), (HDC)image->hdcBmp, Math::Double2Int32(srcX), Math::Double2Int32(srcY), Math::Double2Int32(srcW), Math::Double2Int32(srcH), bf);
		}
		else
		{
			BLENDFUNCTION bf;
			bf.SourceConstantAlpha = 255;
			bf.AlphaFormat = AC_SRC_ALPHA;
			bf.BlendOp = AC_SRC_OVER;
			bf.BlendFlags = 0;
			AlphaBlend((HDC)this->hdcBmp, Math::Double2Int32(destX), Math::Double2Int32(destY), Math::Double2Int32(srcW), Math::Double2Int32(srcH), (HDC)image->hdcBmp, Math::Double2Int32(srcX), Math::Double2Int32(srcY), Math::Double2Int32(srcW), Math::Double2Int32(srcH), bf);
		}
#else
		BitBlt((HDC)this->hdcBmp, Math::Double2Int32(destX), Math::Double2Int32(destY), Math::Double2Int32(srcW), Math::Double2Int32(srcH), (HDC)image->hdcBmp, Math::Double2Int32(srcX), Math::Double2Int32(srcY), SRCCOPY);
#endif
	}
	return true;
}

Bool Media::GDIImage::DrawImageRect(DrawImage *img, OSInt tlx, OSInt tly, OSInt brx, OSInt bry)
{
	GDIImage *image = (GDIImage *)img;
	StretchBlt((HDC)this->hdcBmp, (int)tlx, (int)tly, (int)(brx - tlx), (int)(bry - tly), (HDC)image->hdcBmp, 0, 0, (int)img->GetWidth(), (int)img->GetHeight(), SRCCOPY);
	return true;
}

Media::DrawPen *Media::GDIImage::NewPenARGB(Int32 color, Double thick, UInt8 *pattern, OSInt nPattern)
{
	if (thick < 1)
		thick = 1;
	HPEN hpen;
	DWORD *dwPattern = 0;
	if (nPattern == 0)
	{
		hpen = CreatePen(PS_SOLID, Math::Double2Int32(thick), GDIEGetCol(color));
	}
	else
	{
#ifdef _WIN32_WCE
		LOGPEN lp;
		lp.lopnStyle = PS_DASH;
		lp.lopnWidth.x = Math::Double2Int32(thick);
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
			dwPattern[i] = pattern[i];
		}
		hpen = ExtCreatePen(PS_GEOMETRIC | PS_USERSTYLE | PS_ENDCAP_ROUND, Math::Double2Int32(thick), &lb, (UInt32)nPattern, dwPattern);
#endif
	}
	GDIPen *pen;
	NEW_CLASS(pen, GDIPen(hpen, (UInt32*)dwPattern, nPattern, this, thick, color));
	return pen;
}

Media::DrawBrush *Media::GDIImage::NewBrushARGB(Int32 color)
{
	HBRUSH hbrush = CreateSolidBrush(GDIEGetCol(color));
	GDIBrush *brush;
	NEW_CLASS(brush, GDIBrush(hbrush, color, this));
	return brush;
}

Media::DrawFont *Media::GDIImage::NewFontA(const Char *name, Int16 pxSize, Media::DrawEngine::DrawFontStyle fontStyle)
{
	GDIFont *f;
	NEW_CLASS(f, GDIFont(this->hdcBmp, name, pxSize * 96.0 / this->info->hdpi, fontStyle, this, 0));
	return f;
}

Media::DrawFont *Media::GDIImage::NewFontW(const WChar *name, Int16 pxSize, Media::DrawEngine::DrawFontStyle fontStyle)
{
	GDIFont *f;
	NEW_CLASS(f, GDIFont(this->hdcBmp, name, pxSize * 96.0 / this->info->hdpi, fontStyle, this, 0));
	return f;
}

Media::DrawFont *Media::GDIImage::NewFontHUTF8(const UTF8Char *name, Double height, Media::DrawEngine::DrawFontStyle fontStyle, Int32 codePage)
{
	GDIFont *f;
	const WChar *wptr = Text::StrToWCharNew(name);
	NEW_CLASS(f, GDIFont(this->hdcBmp, wptr, height * 72.0 / this->info->hdpi, fontStyle, this, codePage));
	Text::StrDelNew(wptr);
	return f;
}

Media::DrawFont *Media::GDIImage::NewFontH(const WChar *name, Double height, Media::DrawEngine::DrawFontStyle fontStyle, Int32 codePage)
{
	GDIFont *f;
	NEW_CLASS(f, GDIFont(this->hdcBmp, name, height * 72.0 / this->info->hdpi, fontStyle, this, codePage));
	return f;
}

Media::DrawFont *Media::GDIImage::CloneFont(Media::DrawFont *f)
{
	GDIFont *oldF = (Media::GDIFont*)f;
	NEW_CLASS(f, GDIFont(this->hdcBmp, oldF->GetNameW(), oldF->GetPointSize(), oldF->GetFontStyle(), this, oldF->GetCodePage()));
	return f;
}

void Media::GDIImage::DelPen(DrawPen *p)
{
	if (this->currPen == p)
		this->currPen = 0;
	GDIPen *pen = (GDIPen *)p;
	if (pen->pattern)
		MemFree(pen->pattern);
	DeleteObject((HPEN)pen->hpen);
	DEL_CLASS(pen);
}

void Media::GDIImage::DelBrush(DrawBrush *b)
{
	if (this->currBrush == b)
		this->currBrush = 0;
	GDIBrush *brush = (GDIBrush *)b;
	DeleteObject((HBRUSH)brush->hbrush);
	DEL_CLASS(brush);
}

void Media::GDIImage::DelFont(DrawFont *f)
{
	if (this->currFont == f)
		this->currFont = 0;
	GDIFont *font = (GDIFont*)f;
	DEL_CLASS(font);
}

Bool Media::GDIImage::GetTextSizeUTF8(DrawFont *fnt, const UTF8Char *txt, OSInt txtLen, Double *sz)
{
	OSInt strLen = Text::StrUTF8_WCharCnt(txt, txtLen);
	WChar *wptr = MemAlloc(WChar, strLen + 1);
	Text::StrUTF8_WChar(wptr, txt, txtLen, 0);
	Bool ret = GetTextSize(fnt, wptr, strLen, sz);
	MemFree(wptr);
	return ret;
}

Bool Media::GDIImage::GetTextSize(DrawFont *fnt, const WChar *txt, OSInt txtLen, Double *sz)
{
	Bool isCJK = true;
	if (txtLen == -1)
		txtLen = Text::StrCharCnt(txt);

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
		sz[0] = Math::OSInt2Double((((GDIFont*)fnt)->pxSize + 1) * txtLen);
		sz[1] = ((GDIFont*)fnt)->pxSize + 2;
	}
	else
	{
		SIZE size;
		Int32 i = 3;
		if (this->currFont != fnt)
		{
			GDIFont *f = (GDIFont*)(this->currFont = fnt);
			SelectObject((HDC)this->hdcBmp, (HFONT)f->hfont);
		}
		while (GetTextExtentExPointW((HDC)this->hdcBmp, txt, (int)txtLen, (int)width, 0, 0, &size) == 0)
		{
			if (i-- <= 0)
			{
				this->currFont = fnt;
				
				//wprintf(L"Error in get text size: %s, font = %s, %X\n", txt, f->GetNameW(), f->hfont);
				size.cx = 0;
				size.cy = 0;
				break;
			}
			else
			{
				GDIFont *f = (GDIFont*)(this->currFont = fnt);
				SelectObject((HDC)this->hdcBmp, (HFONT)f->hfont);
				Sync::Thread::Sleep(10);
			}
		}
		sz[0] = size.cx;
		sz[1] = size.cy;
	}
	return true;
}

void Media::GDIImage::SetTextAlign(Media::DrawEngine::DrawPos pos)
{
	static UInt32 textAlign[] = {TA_TOP | TA_LEFT, TA_TOP | TA_CENTER, TA_TOP | TA_RIGHT, TA_TOP | TA_LEFT, TA_TOP | TA_CENTER, TA_TOP | TA_RIGHT, TA_BOTTOM | TA_LEFT, TA_BOTTOM | TA_CENTER, TA_BOTTOM | TA_RIGHT};
	this->strAlign = pos;
	::SetTextAlign((HDC)this->hdcBmp, textAlign[pos]);
}

void Media::GDIImage::GetStringBoundW(Int32 *pos, OSInt centX, OSInt centY, const WChar *str, DrawFont *f, OSInt *drawX, OSInt *drawY)
{
	Double sz[2];
	GetTextSize(f, str, Text::StrCharCnt(str), sz);
	Bool isCenter = false;
	if (strAlign == Media::DrawEngine::DRAW_POS_TOPLEFT)
	{
		pos[0] = (Int32)centX;
		pos[1] = (Int32)centY;
	}
	else if (strAlign == Media::DrawEngine::DRAW_POS_TOPCENTER)
	{
		pos[0] = (Int32)centX - Math::Double2Int32(sz[0] * 0.5);
		pos[1] = (Int32)centY;
	}
	else if (strAlign == Media::DrawEngine::DRAW_POS_TOPRIGHT)
	{
		pos[0] = (Int32)centX - Math::Double2Int32(sz[0]);
		pos[1] = (Int32)centY;
	}
	else if (strAlign == Media::DrawEngine::DRAW_POS_CENTERLEFT)
	{
		pos[0] = (Int32)centX;
		pos[1] = (Int32)centY - Math::Double2Int32(sz[1] * 0.5);
		isCenter = true;
	}
	else if (strAlign == Media::DrawEngine::DRAW_POS_CENTER)
	{
		pos[0] = (Int32)centX - Math::Double2Int32(sz[0] * 0.5);
		pos[1] = (Int32)centY - Math::Double2Int32(sz[1] * 0.5);
		isCenter = true;
	}
	else if (strAlign == Media::DrawEngine::DRAW_POS_CENTERRIGHT)
	{
		pos[0] = (Int32)centX - Math::Double2Int32(sz[0]);
		pos[1] = (Int32)centY - Math::Double2Int32(sz[1] * 0.5);
		isCenter = true;
	}
	else if (strAlign == Media::DrawEngine::DRAW_POS_BOTTOMLEFT)
	{
		pos[0] = (Int32)centX;
		pos[1] = (Int32)centY - Math::Double2Int32(sz[1]);
	}
	else if (strAlign == Media::DrawEngine::DRAW_POS_BOTTOMCENTER)
	{
		pos[0] = (Int32)centX - Math::Double2Int32(sz[0] * 0.5);
		pos[1] = (Int32)centY - Math::Double2Int32(sz[1]);
	}
	else if (strAlign == Media::DrawEngine::DRAW_POS_BOTTOMRIGHT)
	{
		pos[0] = (Int32)centX - Math::Double2Int32(sz[0]);
		pos[1] = (Int32)centY - Math::Double2Int32(sz[1]);
	}
	pos[2] = pos[0] + Math::Double2Int32(sz[0]);
	pos[3] = pos[1];
	pos[4] = pos[0] + Math::Double2Int32(sz[0]);
	pos[5] = pos[1] + Math::Double2Int32(sz[1]);
	pos[6] = pos[0];
	pos[7] = pos[1] + Math::Double2Int32(sz[1]);
	if (isCenter)
	{
		*drawX = centX;
		*drawY = centY - Math::Double2Int32(sz[1] * 0.5);
	}
	else
	{
		*drawX = centX;
		*drawY = centY;
	}
}


void Media::GDIImage::GetStringBoundRotW(Int32 *pos, Double centX, Double centY, const WChar *str, DrawFont *f, Double angleDegree, OSInt *drawX, OSInt *drawY)
{
	Double sz[2];
	GetTextSize(f, str, Text::StrCharCnt(str), sz);
	Double pts[10];
	Bool isCenter = false;
	if (strAlign == Media::DrawEngine::DRAW_POS_TOPLEFT)
	{
		pts[0] = centX;
		pts[1] = centY;
	}
	else if (strAlign == Media::DrawEngine::DRAW_POS_TOPCENTER)
	{
		pts[0] = centX - sz[0] * 0.5;
		pts[1] = centY;
	}
	else if (strAlign == Media::DrawEngine::DRAW_POS_TOPRIGHT)
	{
		pts[0] = centX - sz[0];
		pts[1] = centY;
	}
	else if (strAlign == Media::DrawEngine::DRAW_POS_CENTERLEFT)
	{
		pts[0] = centX;
		pts[1] = centY - sz[1] * 0.5;
		isCenter = true;
	}
	else if (strAlign == Media::DrawEngine::DRAW_POS_CENTER)
	{
		pts[0] = centX - sz[0] * 0.5;
		pts[1] = centY - sz[1] * 0.5;
		isCenter = true;
	}
	else if (strAlign == Media::DrawEngine::DRAW_POS_CENTERRIGHT)
	{
		pts[0] = centX - sz[0];
		pts[1] = centY - sz[1] * 0.5;
		isCenter = true;
	}
	else if (strAlign == Media::DrawEngine::DRAW_POS_BOTTOMLEFT)
	{
		pts[0] = centX;
		pts[1] = centY - sz[1];
	}
	else if (strAlign == Media::DrawEngine::DRAW_POS_BOTTOMCENTER)
	{
		pts[0] = centX - sz[0] * 0.5;
		pts[1] = centY - sz[1];
	}
	else if (strAlign == Media::DrawEngine::DRAW_POS_BOTTOMRIGHT)
	{
		pts[0] = centX - sz[0];
		pts[1] = centY - sz[1];
	}
	pts[2] = pts[0] + sz[0];
	pts[3] = pts[1];
	pts[4] = pts[0] + sz[0];
	pts[5] = pts[1] + sz[1];
	pts[6] = pts[0];
	pts[7] = pts[1] + sz[1];
	if (isCenter)
	{
		pts[8] = centX;
		pts[9] = centY - sz[1] * 0.5;
		Math::Geometry::RotateACW(pts, pts, 5, centX, centY, angleDegree * Math::PI / 180.0);
		pos[0] = Math::Double2Int32(pts[0]);
		pos[1] = Math::Double2Int32(pts[1]);
		pos[2] = Math::Double2Int32(pts[2]);
		pos[3] = Math::Double2Int32(pts[3]);
		pos[4] = Math::Double2Int32(pts[4]);
		pos[5] = Math::Double2Int32(pts[5]);
		pos[6] = Math::Double2Int32(pts[6]);
		pos[7] = Math::Double2Int32(pts[7]);
		*drawX = Math::Double2Int32(pts[8]);
		*drawY = Math::Double2Int32(pts[9]);
	}
	else
	{
		Math::Geometry::RotateACW(pts, pts, 4, centX, centY, angleDegree * Math::PI / 180.0);
		pos[0] = Math::Double2Int32(pts[0]);
		pos[1] = Math::Double2Int32(pts[1]);
		pos[2] = Math::Double2Int32(pts[2]);
		pos[3] = Math::Double2Int32(pts[3]);
		pos[4] = Math::Double2Int32(pts[4]);
		pos[5] = Math::Double2Int32(pts[5]);
		pos[6] = Math::Double2Int32(pts[6]);
		pos[7] = Math::Double2Int32(pts[7]);
		*drawX = Math::Double2Int32(centX);
		*drawY = Math::Double2Int32(centY);
	}
}

void Media::GDIImage::CopyBits(OSInt x, OSInt y, void *imgPtr, OSInt bpl, OSInt width, OSInt height)
{
	UInt8 *iptr = (UInt8*)imgPtr;
	UInt8 *sptr = (UInt8*)this->bmpBits;
	OSInt sbpl = this->width << 2;
	OSInt sheight = this->height;
	if (x < 0)
	{
		iptr = iptr - (x << 2);
		width += x;
		x = 0;
	}
	if (y < 0)
	{
		iptr = iptr - bpl * y;
		height += y;
		y = 0;
	}
	if (x + width > (OSInt)this->width)
	{
		width = this->width - x;
	}
	if (y + height > (OSInt)this->height)
	{
		height = this->height - y;
	}

	if ((width << 2) < 1024)
	{
		sptr += (sheight - y) * sbpl + (x << 2);
		while (height-- > 0)
		{
			sptr -= sbpl;
			MemCopyNO(iptr, sptr, width << 2);
			iptr += bpl;
		}
	}
	else
	{
		sptr += (sheight - y) * sbpl + (x << 2);
		while (height-- > 0)
		{
			sptr -= sbpl;
			MemCopyNANC(iptr, sptr, width << 2);
			iptr += bpl;
		}
	}
}

Media::StaticImage *Media::GDIImage::ToStaticImage()
{
	return CreateStaticImage();
}

Int32 Media::GDIImage::SavePng(IO::SeekableStream *stm)
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
		return stat | 0x10000; 
#else
	return 0x10000;
#endif
}

Int32 Media::GDIImage::SaveGIF(IO::SeekableStream *stm)
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
	
	newBmp = new Gdiplus::Bitmap((INT)this->width, (INT)this->height, PixelFormat8bppIndexed);
	bdd = new Gdiplus::BitmapData();
	bds = new Gdiplus::BitmapData();
	rc = new Gdiplus::Rect(0, 0, (INT)this->width, (INT)this->height);
	succ = false;
	if (newBmp->LockBits(rc, Gdiplus::ImageLockModeWrite, PixelFormat8bppIndexed, bdd) == 0)
	{
		if (image->LockBits(rc, Gdiplus::ImageLockModeRead | Gdiplus::ImageLockModeWrite, PixelFormat32bppARGB, bds) == 0)
		{
			pal = (Gdiplus::ColorPalette *)MAlloc(8 + 1024);
			pal->Flags = 0;
			pal->Count = 256;
			Media::ImageTo8Bit::From32bpp((UInt8*)bds->Scan0, (UInt8*)bdd->Scan0, (UInt8*)pal->Entries, this->width, this->height, bds->Stride, bdd->Stride);
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
		return stat | 0x10000; 
#else
	return 0x10000;
#endif
}

Int32 Media::GDIImage::SaveJPG(IO::SeekableStream *stm)
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
		return stat | 0x10000; 
#else
	return 0x10000;
#endif
}

Media::Image *Media::GDIImage::Clone()
{
	return CreateStaticImage();
}

Media::Image::ImageType Media::GDIImage::GetImageType()
{
	return IT_GUIIMAGE;
}

void Media::GDIImage::GetImageData(UInt8 *destBuff, OSInt left, OSInt top, OSInt width, OSInt height, OSInt destBpl)
{
	if (left < 0)
	{
		width += left;
		left = 0;
	}
	if (top < 0)
	{
		height += top;
		top = 0;
	}
	if (left >= (OSInt)this->width || top >= (OSInt)this->height)
		return;
	if (left + width > (OSInt)this->width)
	{
		width = this->width - left;
	}
	if (top + height > (OSInt)this->height)
	{
		height = this->height - top;
	}
	OSInt lineSize = (width * this->bitCount) >> 3;
	OSInt srcBpl = (this->width * this->bitCount) >> 3;
	
	UInt8 *srcPtr = (UInt8*)this->bmpBits;
	srcPtr = srcPtr + (this->height - top) * srcBpl + ((left * this->bitCount) >> 3);
	OSInt i = height;
	while (i-- > 0)
	{
		srcPtr -= srcBpl;
		MemCopyNO(destBuff, srcPtr, lineSize);
		destBuff += destBpl;
	}
}

Int32 Media::GDIImage::GetPixel32(OSInt x, OSInt y)
{
	if (x < 0 || x >= (OSInt)this->width)
		return 0;
	if (y < 0 || y >= (OSInt)this->height)
		return 0;
	if (this->bitCount == 32)
	{
		return *(Int32*)(((y * this->width + x) * 4) + (UInt8*)this->bmpBits);
	}
	else if (this->bitCount == 24)
	{
		return 0xff000000 | *(Int32*)(((y * this->width + x) * 3) + (UInt8*)this->bmpBits);
	}
	else
	{
		return 0;
	}
}

void Media::GDIImage::PolylineAccel(void *hdc, Int32 *points, UOSInt nPoints, OSInt ofstX, OSInt ofstY, OSInt width, OSInt height)
{
	Bool thisInScn;
	Bool lastInScn;
	Int32 *ptStart = 0;
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

void Media::GDIImage::PolygonAccel(void *hdc, Int32 *points, UOSInt nPoints, OSInt ofstX, OSInt ofstY, OSInt width, OSInt height, Int32 penWidth)
{
	//////////////////////////////////////////
	Int32 *pointsTmp = MemAlloc(Int32, nPoints << 2);
	Int32 *pointsTmp2;
	OSInt ptCnt;
	OSInt ptCnt2;

	if (pointsTmp == 0)
	{
		return;
	}

	ptCnt = Math::Geometry::BoundPolygonY(points, nPoints, pointsTmp, -penWidth, height + penWidth, 0, 0);
	if (ptCnt > 0)
	{
		pointsTmp2 = MemAlloc(Int32, ptCnt << 2);
//		Polygon((HDC)hdc, (POINT*)pointsTmp, ptCnt);

		ptCnt2 = Math::Geometry::BoundPolygonX(pointsTmp, ptCnt, pointsTmp2, -penWidth, width + penWidth, ofstX, ofstY);

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

void Media::GDIImage::PolyPolygonAccel(void *hdc, Int32 *points, UInt32 *pointCnt, UOSInt nPointCnt, OSInt ofstX, OSInt ofstY, OSInt width, OSInt height, Int32 penWidth)
{
	Int32 *pointsTmp;
	Int32 *pointsTmp2;
	Int32 *pointsTmp3;

	Int32 *currPoints;
	Int32 nParts;
	OSInt ptCnt;
	OSInt ptCnt2;

	Int32 pointsTmp3Size;
	Int32 totalPoints = 0;
	Int32 nPoints;
	UOSInt i = nPointCnt;
	while (i-- > 0)
	{
		totalPoints += pointCnt[i];
	}
	pointsTmp = MemAlloc(Int32, totalPoints << 2);
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

		ptCnt = Math::Geometry::BoundPolygonY(points, nPoints, pointsTmp3, -penWidth, height + penWidth, 0, 0);
		if (ptCnt > 0)
		{
			ptCnt2 = Math::Geometry::BoundPolygonX(pointsTmp3, ptCnt, currPoints, -penWidth, width + penWidth, ofstX, ofstY);
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

Bool Media::GDIImage::DrawRectN(OSInt oriX, OSInt oriY, OSInt oriW, OSInt oriH, DrawPen *p, DrawBrush *b)
{
	Media::GDIBrush *gb = (Media::GDIBrush*)b;

	if (b == 0)
	{
		if (p != 0)
		{
			DrawRect(Math::OSInt2Double(oriX), Math::OSInt2Double(oriY), Math::OSInt2Double(oriW), Math::OSInt2Double(oriH), p, 0);
		}
		return true;
	}

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
	if (x + w > (OSInt)this->width)
	{
		w = this->width - x;
	}
	if (y + h > (OSInt)this->height)
	{
		h = this->height - y;
	}
	if (w <= 0)
		return false;
	if (h <= 0)
		return false;
	
	OSInt bpl = this->width << 2;
	UInt8 *initOfst = ((UInt8*) this->bmpBits) + bpl * y + (x << 2);
	Int32 c = gb->oriColor;

	if ((c & 0xff000000) == 0xff000000)
	{
		ImageUtil_ImageColorFill32(initOfst, w, h, bpl, c);
	}
	else
	{
		ImageUtil_ImageColorBlend32(initOfst, w, h, bpl, c);
	}
	if (p)
	{
		DrawRect(Math::OSInt2Double(oriX), Math::OSInt2Double(oriY), Math::OSInt2Double(oriW), Math::OSInt2Double(oriH), p, 0);
	}
	return true;
}

void *Media::GDIImage::GetHDC()
{
	return this->hdcBmp;
}

void Media::GDIImage::SetImageAlpha(UInt8 alpha)
{
	if (this->info->storeBPP == 32 && this->bmpBits && this->info->pf == Media::PF_B8G8R8A8)
	{
		ImageUtil_ImageFillAlpha32((UInt8*)this->bmpBits, this->width, this->height, this->width << 2, alpha);
	}
}

void Media::GDIImage::MulImageAlpha(Double val)
{
	if (val >= 1.0)
		return;
	if (val <= 0.0)
	{
		this->SetImageAlpha(0);
		return;
	}
	if (this->info->storeBPP == 32 && this->bmpBits && this->info->pf == Media::PF_B8G8R8A8)
	{
		ImageUtil_ImageAlphaMul32((UInt8*)this->bmpBits, this->width, this->height, this->width << 2, Math::Double2Int32(val * 65536));
	}
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
	if (left >= (OSInt)this->width)
		return;
	if (top >= (OSInt)this->height)
		return;
	if (left + width > (OSInt)this->width)
	{
		width = this->width - left;
	}
	if (top + height > (OSInt)this->height)
	{
		height = this->height - top;
	}
	if (width <= 0)
		return;
	if (height <= 0)
		return;
	if (this->info->storeBPP == 32 && this->bmpBits && this->info->pf == Media::PF_B8G8R8A8)
	{
		UInt8 *pbits = (UInt8*)this->bmpBits;
		OSInt bpl = this->width * 4;
		pbits = pbits + (this->height - top - height) * bpl + left * 4;
		ImageUtil_ImageFillAlpha32(pbits, width, height, bpl, alpha);
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
		OSInt i;
		OSInt sbpl;
		OSInt dbpl;

		Gdiplus::Rect rect(0, 0, (INT)this->width, (INT)this->height);
		Gdiplus::BitmapData *bitmapData = new Gdiplus::BitmapData();
		Gdiplus::Bitmap *bmp = new Gdiplus::Bitmap((INT)this->width, (INT)this->height, PixelFormat32bppARGB);
		bmp->LockBits(&rect, Gdiplus::ImageLockModeWrite, PixelFormat32bppARGB, bitmapData);
		i = this->height;
		sbpl = this->GetDataBpl();
		dbpl = bitmapData->Stride;
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
