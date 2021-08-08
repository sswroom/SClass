#include "Stdafx.h"
#include "MyMemory.h"
#include "Media/ScreenCapturer.h"
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xmd.h> 
#include <X11/Xatom.h>

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
    Screen *scr = ScreenOfDisplay(dis, -1 + (int)(OSInt)hMon);
    Drawable drawable = XRootWindow(dis, -1 + (int)(OSInt)hMon);

	Media::StaticImage *retImg = 0;
    XImage *image = XGetImage(dis, drawable, 0, 0, (UInt32)scr->width, (UInt32)scr->height, AllPlanes, ZPixmap);
	if (image)
	{
		Media::MonitorColorManager *monColor = this->colorMgr->GetMonColorManager(hMon);
		Bool valid = true;
		Media::FrameInfo info;
		info.fourcc = 0;
		info.ftype = Media::FT_NON_INTERLACE;
		info.atype = Media::AT_NO_ALPHA;
		info.ycOfst = Media::YCOFST_C_CENTER_LEFT;
		info.yuvType = Media::ColorProfile::YUVT_UNKNOWN;
		info.storeBPP = (UInt32)image->bits_per_pixel;
		info.pf = Media::FrameInfo::GetDefPixelFormat(0, info.storeBPP);
		if (info.storeBPP == 32 && monColor->Get10BitColor())
		{
			info.pf = Media::PF_LE_A2B10G10R10;
		}
		info.dispWidth = (UInt32)image->width;
		info.dispHeight = (UInt32)image->height;
		info.storeWidth = (UInt32)image->bytes_per_line / (info.storeBPP >> 3);
		info.storeHeight = (UInt32)image->height;
		info.byteSize = info.storeWidth * info.storeHeight * (info.storeBPP >> 3);
		info.par2 = 1.0;
		info.hdpi = this->monMgr->GetMonitorHDPI(hMon);;
		info.vdpi = info.hdpi;
		const Media::IColorHandler::RGBPARAM2 *params = monColor->GetRGBParam();
		info.color->Set(params->monProfile);
		
		if (valid)
		{
			NEW_CLASS(retImg, Media::StaticImage(&info));
			MemCopyNANC(retImg->data, image->data, info.byteSize);
		}
	    XDestroyImage(image);
	}	

    XCloseDisplay(dis); 
	return retImg;
}
