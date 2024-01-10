#ifndef _SM_MEDIA_FBSURFACE
#define _SM_MEDIA_FBSURFACE
#include "Handles.h"
#include "Media/MonitorSurface.h"

namespace Media
{
	class FBSurface : public Media::MonitorSurface
	{
	private:
		struct ClassData;

		ClassData *clsData;

		Bool UpdateToScreen(Bool waitForVBlank);
	public:
		FBSurface(MonitorHandle *hMon, const Media::ColorProfile *color, Double dpi, Media::RotateType rotateType);
		virtual ~FBSurface();

		Bool IsError() const;
		
		virtual NotNullPtr<Media::RasterImage> Clone() const;
		virtual Media::RasterImage::ImageType GetImageType() const;
		virtual void GetRasterData(UInt8 *destBuff, OSInt left, OSInt top, UOSInt width, UOSInt height, UOSInt destBpl, Bool upsideDown, Media::RotateType destRotate) const;

		virtual void WaitForVBlank();
		virtual void *GetHandle();

		virtual Bool DrawFromBuff();
		virtual Bool DrawFromSurface(NotNullPtr<Media::MonitorSurface> surface, Math::Coord2D<OSInt> destTL, Math::Size2D<UOSInt> buffSize, Bool clearScn, Bool waitForVBlank);
		virtual UInt8 *LockSurface(OSInt *lineAdd);
		virtual void UnlockSurface();
		virtual void SetSurfaceBugMode(Bool surfaceBugMode);

		void SetBuffSurface(Media::MonitorSurface *buffSurface);
	};
}
#endif
