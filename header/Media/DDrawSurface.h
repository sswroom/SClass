#ifndef _SM_MEDIA_DDRAWSURFACE
#define _SM_MEDIA_DDRAWSURFACE
#include "Handles.h"
#include "Media/MonitorSurface.h"
#include "Media/RasterImage.h"

namespace Media
{
	class DDrawManager;

	class DDrawSurface : public Media::MonitorSurface
	{
	private:
		struct ClassData;

		ClassData *clsData;

	public:
		DDrawSurface(DDrawManager *mgr, void *lpDD, void *surface, MonitorHandle *hMon, Bool needRelease, Media::RotateType rotateType);
		virtual ~DDrawSurface();

		virtual NN<Media::RasterImage> Clone() const;
		virtual Media::RasterImage::ImageType GetImageType() const;
		virtual void GetRasterData(UInt8 *destBuff, OSInt left, OSInt top, UOSInt width, UOSInt height, UOSInt destBpl, Bool upsideDown, Media::RotateType destRotate) const;

		virtual void WaitForVBlank();
		virtual void *GetHandle();

		virtual Bool DrawFromBuff();
		virtual Bool DrawFromSurface(NN<Media::MonitorSurface> surface, Math::Coord2D<OSInt> tl, Math::Size2D<UOSInt> drawSize, Bool clearScn, Bool waitForVBlank);
		virtual UInt8 *LockSurface(OutParam<OSInt> lineAdd);
		virtual void UnlockSurface();
		virtual void SetSurfaceBugMode(Bool surfaceBugMode);

		void SetClipWindow(ControlHandle *clipWindow);
		void SetBuffSurface(NN<Media::DDrawSurface> buffSurface);
	};
}
#endif
