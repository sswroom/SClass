#ifndef _SM_MEDIA_DRMSURFACE
#define _SM_MEDIA_DRMSURFACE
#include "Handles.h"
#include "Media/MonitorSurface.h"

namespace Media
{
	class DRMSurface : public Media::MonitorSurface
	{
	private:
		struct ClassData;

		NN<ClassData> clsData;

	public:
		DRMSurface(Int32 fd, MonitorHandle *hMon, NN<const Media::ColorProfile> color, Double dpi);
		virtual ~DRMSurface();

		Bool IsError();
		
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
		void SetBuffSurface(Media::MonitorSurface *buffSurface);
	};
}
#endif
