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
		FBSurface(Optional<MonitorHandle> hMon, Optional<const Media::ColorProfile> color, Double dpi, Media::RotateType rotateType);
		virtual ~FBSurface();

		Bool IsError() const;
		
		virtual NN<Media::RasterImage> Clone() const;
		virtual Media::RasterImage::ImageType GetImageType() const;
		virtual void GetRasterData(UnsafeArray<UInt8> destBuff, IntOS left, IntOS top, UIntOS width, UIntOS height, UIntOS destBpl, Bool upsideDown, Media::RotateType destRotate) const;

		virtual void WaitForVBlank();
		virtual void *GetHandle();

		virtual Bool DrawFromBuff();
		virtual Bool DrawFromSurface(NN<Media::MonitorSurface> surface, Math::Coord2D<IntOS> destTL, Math::Size2D<UIntOS> buffSize, Bool clearScn, Bool waitForVBlank);
		virtual UnsafeArrayOpt<UInt8> LockSurface(OutParam<IntOS> lineAdd);
		virtual void UnlockSurface();
		virtual void SetSurfaceBugMode(Bool surfaceBugMode);

		void SetBuffSurface(NN<Media::MonitorSurface> buffSurface);
	};
}
#endif
