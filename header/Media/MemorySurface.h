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
		MemorySurface(Math::Size2D<UIntOS> size, UIntOS bitPerPixel, Optional<const Media::ColorProfile> color, Double dpi);
		virtual ~MemorySurface();

		Bool IsError() const;
		
		virtual NN<Media::RasterImage> Clone() const;
		virtual Media::RasterImage::ImageClass GetImageClass() const;
		virtual void GetRasterData(UnsafeArray<UInt8> destBuff, IntOS left, IntOS top, UIntOS width, UIntOS height, UIntOS destBpl, Bool upsideDown, Media::RotateType destRotate) const;

		virtual void WaitForVBlank();
		virtual void *GetHandle();

		virtual Bool DrawFromBuff();
		virtual Bool DrawFromSurface(NN<Media::MonitorSurface> surface, Math::Coord2D<IntOS> destTL, Math::Size2D<UIntOS> buffSize, Bool clearScn, Bool waitForVBlank);
		virtual UnsafeArrayOpt<UInt8> LockSurface(OutParam<IntOS> lineAdd);
		virtual void UnlockSurface();
		virtual void SetSurfaceBugMode(Bool surfaceBugMode);
	};
}
#endif
