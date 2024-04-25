#include "Stdafx.h"
#include "MyMemory.h"
//#include "Media/DRMMonitorSurfaceMgr.h"
#include "Media/ImageUtil.h"
#include "Media/ScreenCapturer.h"
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xmd.h> 
#include <X11/Xatom.h>

#include <stdio.h>

#undef None
/*#include "Sync/ThreadUtil.h"

Media::ScreenCapturer::ScreenCapturer(Media::MonitorMgr *monMgr, Media::ColorManager *colorMgr)
{
	this->monMgr = monMgr;
	this->colorMgr = colorMgr;
	NEW_CLASS(this->surfaceMgr, Media::DRMMonitorSurfaceMgr(0, monMgr, colorMgr));
}

Media::ScreenCapturer::~ScreenCapturer()
{
	DEL_CLASS(this->surfaceMgr);
}

Media::StaticImage *Media::ScreenCapturer::CaptureScreen(MonitorHandle *hMon)
{
	Media::MonitorSurface *surface = this->surfaceMgr->CreatePrimarySurface(hMon, 0);
	if (surface == 0) return 0;
	Media::StaticImage *retImg = surface->CreateStaticImage();
	OSInt lineAdd;
	UInt8 *dataPtr = surface->LockSurface(&lineAdd);
	UOSInt i = 10;
	while (i-- > 0)
	{
		ImageUtil_ColorFill32(dataPtr, surface->info->storeWidth * surface->info->storeHeight, 0xff800000);
		Sync::SimpleThread::Sleep(1000);
	}
	DEL_CLASS(surface);
	return retImg;
}*/

Media::ScreenCapturer::ScreenCapturer(Media::MonitorMgr *monMgr, Media::ColorManager *colorMgr)
{
	this->monMgr = monMgr;
	this->colorMgr = colorMgr;
}

Media::ScreenCapturer::~ScreenCapturer()
{
}

Media::StaticImage *Media::ScreenCapturer::CaptureScreen(MonitorHandle *hMon)
{
    Display *dis = XOpenDisplay((char *)0);
	printf("nScreen = %d\r\n", ScreenCount(dis));
    Screen *scr = ScreenOfDisplay(dis, -1 + (int)(OSInt)hMon);
    Drawable drawable = XRootWindowOfScreen(scr);

	Media::StaticImage *retImg = 0;
    XImage *image = XGetImage(dis, drawable, 0, 0, (UInt32)scr->width, (UInt32)scr->height, AllPlanes, ZPixmap);
	if (image)
	{
		NN<Media::MonitorColorManager> monColor = this->colorMgr->GetMonColorManager(hMon);
		Bool valid = true;
		Media::FrameInfo info;
		info.fourcc = 0;
		info.ftype = Media::FT_NON_INTERLACE;
		info.atype = Media::AT_NO_ALPHA;
		info.ycOfst = Media::YCOFST_C_CENTER_LEFT;
		info.yuvType = Media::ColorProfile::YUVT_UNKNOWN;
		info.storeBPP = (UInt32)image->bits_per_pixel;
		info.pf = Media::PixelFormatGetDef(0, info.storeBPP);
		if (info.storeBPP == 32 && monColor->Get10BitColor())
		{
			info.pf = Media::PF_LE_A2B10G10R10;
		}
		info.dispSize = Math::Size2D<UOSInt>((UInt32)image->width, (UInt32)image->height);
		info.storeSize = Math::Size2D<UOSInt>((UInt32)image->bytes_per_line / (info.storeBPP >> 3), (UInt32)image->height);
		info.byteSize = info.storeSize.CalcArea() * (info.storeBPP >> 3);
		info.par2 = 1.0;
		info.hdpi = this->monMgr->GetMonitorHDPI(hMon);;
		info.vdpi = info.hdpi;
		NN<const Media::IColorHandler::RGBPARAM2> params = monColor->GetRGBParam();
		info.color.Set(params->monProfile);
		info.rotateType = Media::RotateType::None;
		
		if (valid)
		{
			NEW_CLASS(retImg, Media::StaticImage(info));
			MemCopyNANC(retImg->data, image->data, info.byteSize);
		}
	    XDestroyImage(image);
	}	

    XCloseDisplay(dis); 
	return retImg;
}
