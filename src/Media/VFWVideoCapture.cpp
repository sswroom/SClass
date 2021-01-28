#include "Stdafx.h"
#include "MyMemory.h"
#include "Media/VFWVideoCapture.h"
#include "Text/MyString.h"
#include <windows.h>
#include <vfw.h>

Media::VFWVideoCapture *Media::VFWVideoCapture::capObj;

Int32 __stdcall Media::VFWVideoCapture::OnFrame(void *hWnd, void *lpVHdr)
{
	VIDEOHDR *hdr = (VIDEOHDR*)lpVHdr;
	if (capObj->frameCB)
	{
		capObj->frameCB(hdr->lpData, hdr->dwBytesUsed);
	}
	return 0;
}

Int32 Media::VFWVideoCapture::GetDeviceCount()
{
	WChar buff1[2];
	WChar buff2[2];
	Int32 i = 0;
	while (true)
	{
		if (capGetDriverDescriptionW(i, buff1, 2, buff2, 2) == 0)
			return i;
		i++;
	}
	return 0;
}

WChar *Media::VFWVideoCapture::GetDeviceName(WChar *buff, Int32 devNo)
{
	WChar buff2[2];
	if (capGetDriverDescriptionW(devNo, buff, 512, buff2, 2))
	{
		return &buff[Text::StrCharCnt(buff)];
	}
	else
	{
		return 0;
	}
}

Media::VFWVideoCapture::VFWVideoCapture(Int32 devNo)
{
	this->frameCB = 0;

	hWnd = capCreateCaptureWindowW(L"Capture Window", WS_CHILD, 0, 0, 160, 120, GetDesktopWindow(), 100);
	if (hWnd == 0)
		return;
	BOOL fOk = capDriverConnect((HWND)hWnd, devNo);
	fOk = capSetCallbackOnVideoStream((HWND)hWnd, OnFrame);
	fOk = capSetUserData((HWND)hWnd, this);
}

Media::VFWVideoCapture::~VFWVideoCapture()
{
	if (hWnd)
	{
		capDriverDisconnect((HWND)hWnd);
		CloseWindow((HWND)hWnd);
	}
}

Bool Media::VFWVideoCapture::ShowDlgVideoSource()
{
	CAPDRIVERCAPS CapDrvCaps;
	capDriverGetCaps((HWND)hWnd, &CapDrvCaps, sizeof(CAPDRIVERCAPS)); 
	if (CapDrvCaps.fHasDlgVideoSource)
	{
		capDlgVideoSource((HWND)hWnd); 
		return true;
	}
	else
	{
		return false;
	}
}

Bool Media::VFWVideoCapture::ShowDlgVideoFormat()
{
	CAPDRIVERCAPS CapDrvCaps;
	capDriverGetCaps((HWND)hWnd, &CapDrvCaps, sizeof (CAPDRIVERCAPS)); 
	if (CapDrvCaps.fHasDlgVideoFormat)
	{
		capDlgVideoFormat((HWND)hWnd); 
		return true;
	}
	else
	{
		return false;
	}
}

void Media::VFWVideoCapture::GetCaptureFormat(Media::FrameInfo *info)
{
	DWORD dwSize = capGetVideoFormatSize((HWND)hWnd);
	if (dwSize == 40)
	{
		BITMAPINFOHEADER bmih;
		capGetVideoFormat((HWND)hWnd, &bmih, 40); 
		info->width = bmih.biWidth;
		info->height = bmih.biHeight;
		info->bpp = bmih.biBitCount;
		info->fourcc = bmih.biCompression;
		info->byteSize = bmih.biSizeImage;
		info->ftype = Media::FT_NON_INTERLACE;
	}
	else
	{
		info->width = 0;
		info->height = 0;
		info->fourcc = 0;
		info->byteSize = 0;
		info->bpp = 0;
		info->ftype = Media::FT_NON_INTERLACE;
	}
}

void Media::VFWVideoCapture::SetFrameRate(Int32 rate, Int32 scale)
{
	CAPTUREPARMS CaptureParms;

	capCaptureGetSetup((HWND)hWnd, &CaptureParms, sizeof(CAPTUREPARMS));

	CaptureParms.dwRequestMicroSecPerFrame = (DWORD) MulDiv(1000000, scale, rate);
	capCaptureSetSetup((HWND)hWnd, &CaptureParms, sizeof (CAPTUREPARMS)); 
}

void Media::VFWVideoCapture::StartCapture(FrameCallback cb)
{
	this->frameCB = cb;
	capObj = this;
	capCaptureSequenceNoFile((HWND)hWnd);
}

void Media::VFWVideoCapture::StopCapture()
{
	capCaptureStop((HWND)hWnd);
}
