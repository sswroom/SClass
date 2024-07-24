#ifndef _SM_MEDIA_MEMORYSURFACE
#define _SM_MEDIA_MEMORYSURFACE
#include "Media/MonitorSurface.h"

namespace Media
{
	class MemorySurface : public Media::MonitorSurface
	{
	private:
		UnsafeArray<UInt8> buffPtr;

	public:
		MemorySurface(Math::Size2D<UOSInt> size, UOSInt bitPerPixel, const Media::ColorProfile *color, Double dpi);
		virtual ~MemorySurface();

		Bool IsError() const;
		
		virtual NN<Media::RasterImage> Clone() const;
		virtual Media::RasterImage::ImageType GetImageType() const;
		virtual void GetRasterData(UnsafeArray<UInt8> destBuff, OSInt left, OSInt top, UOSInt width, UOSInt height, UOSInt destBpl, Bool upsideDown, Media::RotateType destRotate) const;

		virtual void WaitForVBlank();
		virtual void *GetHandle();

		virtual Bool DrawFromBuff();
		virtual Bool DrawFromSurface(NN<Media::MonitorSurface> surface, Math::Coord2D<OSInt> destTL, Math::Size2D<UOSInt> buffSize, Bool clearScn, Bool waitForVBlank);
		virtual UnsafeArrayOpt<UInt8> LockSurface(OutParam<OSInt> lineAdd);
		virtual void UnlockSurface();
		virtual void SetSurfaceBugMode(Bool surfaceBugMode);
	};
}
#endif
