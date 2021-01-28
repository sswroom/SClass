#include "Stdafx.h"
#include "MyMemory.h"
#include "Media/GDIEngine.h"
#include "UI/GUIIcon.h"
#include <windows.h>

UI::GUIIcon::GUIIcon()
{
	this->hand = 0;
}

UI::GUIIcon::GUIIcon(Media::DrawImage *img)
{
	this->hand = 0;
	if ((img->GetWidth() & 7) == 0 && (img->GetHeight() & 7) == 0)
	{
		void *pBits;
		HDC hdcIcons = CreateCompatibleDC(GetDC(0));
		HBITMAP hbmp;
		BITMAPINFOHEADER bmpInfo;
		bmpInfo.biSize = sizeof(BITMAPINFOHEADER);
		bmpInfo.biWidth = (LONG)img->GetWidth();
		bmpInfo.biHeight = (LONG)img->GetHeight();
		bmpInfo.biPlanes = 1;
		bmpInfo.biXPelsPerMeter = 96;
		bmpInfo.biYPelsPerMeter = 96;
		bmpInfo.biSizeImage = 0;
		bmpInfo.biBitCount = 1;
		bmpInfo.biClrImportant = 0;
		bmpInfo.biClrUsed = 0;
		bmpInfo.biCompression = BI_RGB;
		hbmp = CreateDIBSection(hdcIcons, (BITMAPINFO*)&bmpInfo, 0, &pBits, 0, 0);
		////////////////////////////////////
		ICONINFO icoInfo;
		icoInfo.fIcon = TRUE;
		icoInfo.xHotspot = 0;
		icoInfo.yHotspot = 0;
		icoInfo.hbmMask = hbmp;
		icoInfo.hbmColor = (HBITMAP)((Media::GDIImage*)img)->hBmp;
		this->hand = CreateIconIndirect(&icoInfo);
		DeleteObject(hbmp);
		DeleteDC(hdcIcons);
	}
}

UI::GUIIcon::~GUIIcon()
{
	if (this->hand)
	{
		DeleteObject((HICON)this->hand);
		this->hand = 0;
	}
}

void *UI::GUIIcon::GetHandle()
{
	return this->hand;
}
