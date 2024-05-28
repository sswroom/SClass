#include "Stdafx.h"
#include <windows.h>
#include "MyMemory.h"
#include "Media/FrameInfo.h"
#include "Media/IImgResizer.h"
#include "Media/RasterImage.h"
#include "Media/StaticImage.h"
#include "Media/ImageWin.h"

Media::ImageWin::ImageWin()
{
}

Media::ImageWin::~ImageWin()
{
	
}

Optional<Media::RasterImage> Media::ImageWin::CreateImage(HBITMAP hbmp)
{
	HDC hdc;
	BITMAPINFO bmi;
	Media::StaticImage *img;
	Char *imgBuff;

	Char *imgPtr;
	Char *outPtr;
	Int32 i;
	Int32 lineDiff;

	ZeroMemory(&bmi.bmiHeader, sizeof(bmi.bmiHeader));
	bmi.bmiHeader.biSize = sizeof(bmi.bmiHeader);

	hdc = GetDC(0);
	GetDIBits(hdc, hbmp, 0, 0, 0, &bmi, 0);
	if (bmi.bmiHeader.biBitCount == 24)
	{
		imgBuff = MemAlloc(Char, bmi.bmiHeader.biWidth * bmi.bmiHeader.biHeight * 3);
		GetDIBits(hdc, hbmp, 0, bmi.bmiHeader.biHeight, imgBuff, &bmi, 0);

		NEW_CLASS(img, Media::StaticImage(bmi.bmiHeader.biWidth, bmi.bmiHeader.biHeight, 0, 24, 0, Media::CS::TRANodeType::Unknown, Media::IColorHandler::YUVT_UNKNOWN, 2.2, Media::AT_NO_ALPHA));

		i = bmi.bmiHeader.biHeight;
		lineDiff = bmi.bmiHeader.biWidth * 3;
		imgPtr = imgBuff + lineDiff * i;;
		outPtr = (Char*)img->data;
		while (i-- > 0)
		{
			imgPtr = imgPtr - lineDiff;
			MemCopy(outPtr, imgPtr, lineDiff);

			outPtr = outPtr + lineDiff;
		}

		ReleaseDC(0, hdc);
		MemFree(imgBuff);
		return img;
	}
	else if (bmi.bmiHeader.biBitCount == 32)
	{
		imgBuff = MemAlloc(Char, bmi.bmiHeader.biWidth * bmi.bmiHeader.biHeight * 3);
		GetDIBits(hdc, hbmp, 0, bmi.bmiHeader.biHeight, imgBuff, &bmi, 0);

		NEW_CLASS(img, Media::StaticImage(bmi.bmiHeader.biWidth, bmi.bmiHeader.biHeight, 0, 24, 0, Media::CS::TRANodeType::Unknown, Media::IColorHandler::YUVT_UNKNOWN, 2.2, Media::AT_ALPHA));

		i = bmi.bmiHeader.biHeight;
		lineDiff = bmi.bmiHeader.biWidth * 3;
		imgPtr = imgBuff + lineDiff * i;;
		outPtr = (Char*)img->data;
		while (i-- > 0)
		{
			imgPtr = imgPtr - lineDiff;
			MemCopy(outPtr, imgPtr, lineDiff);

			outPtr = outPtr + lineDiff;
		}

		ReleaseDC(0, hdc);
		MemFree(imgBuff);
		return img;
	}
	else
	{
		ReleaseDC(0, hdc);
		return 0;
	}
}

HBITMAP Media::ImageWin::CreateHBITMAP(NN<Media::StaticImage> img)
{
	Char *imgPtr;
	Char *outPtr;
	Int32 lineDiff;
	BITMAPINFO bmi;
	HDC hdc;
	Int32 i;
	if (img->info->fourcc == 0)
	{
		if (img->info->bpp == 24)
		{
			hdc = GetDC(0);
			ZeroMemory(&bmi.bmiHeader, sizeof(bmi.bmiHeader));
			bmi.bmiHeader.biSize = sizeof(bmi.bmiHeader);
			bmi.bmiHeader.biWidth = img->info->width;
			bmi.bmiHeader.biHeight = img->info->height;
			bmi.bmiHeader.biBitCount = 24;
			bmi.bmiHeader.biPlanes = 1;
			bmi.bmiHeader.biXPelsPerMeter = 72;
			bmi.bmiHeader.biYPelsPerMeter = 72;

			HBITMAP hbmp = CreateDIBSection(hdc, &bmi, 0, (void**)&imgPtr, 0, 0);
			lineDiff = bmi.bmiHeader.biWidth * 3;
			if (hbmp)
			{
				outPtr = ((Char*)img->data) + bmi.bmiHeader.biHeight * lineDiff;
				i = bmi.bmiHeader.biHeight;
				while (i-- > 0)
				{
					outPtr = outPtr - lineDiff;
					MemCopy(imgPtr, outPtr, lineDiff);
					imgPtr = imgPtr + lineDiff;
				}
				
			}
			ReleaseDC(0, hdc);
			return hbmp;
		}
		else if (img->info->bpp == 32)
		{
			hdc = GetDC(0);
			ZeroMemory(&bmi.bmiHeader, sizeof(bmi.bmiHeader));
			bmi.bmiHeader.biSize = sizeof(bmi.bmiHeader);
			bmi.bmiHeader.biWidth = img->info->width;
			bmi.bmiHeader.biHeight = img->info->height;
			bmi.bmiHeader.biBitCount = 32;
			bmi.bmiHeader.biPlanes = 1;
			bmi.bmiHeader.biXPelsPerMeter = 72;
			bmi.bmiHeader.biYPelsPerMeter = 72;

			HBITMAP hbmp = CreateDIBSection(hdc, &bmi, 0, (void**)&imgPtr, 0, 0);
			lineDiff = bmi.bmiHeader.biWidth << 2;
			if (hbmp)
			{
				outPtr = ((Char*)img->data) + bmi.bmiHeader.biHeight * lineDiff;
				i = bmi.bmiHeader.biHeight;
				while (i-- > 0)
				{
					outPtr = outPtr - lineDiff;
					MemCopy(imgPtr, outPtr, lineDiff);
					imgPtr = imgPtr + lineDiff;
				}
			}
			ReleaseDC(0, hdc);
			return hbmp;
		}
		else
		{
			return 0;
		}
	}
	else
	{
		return 0;
	}
}
