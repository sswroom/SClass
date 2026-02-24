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

		NN<ClassData> clsData;

	public:
		DDrawSurface(NN<DDrawManager> mgr, void *lpDD, void *surface, Optional<MonitorHandle> hMon, Bool needRelease, Media::RotateType rotateType);
		virtual ~DDrawSurface();

		virtual NN<Media::RasterImage> Clone() const;
		virtual Media::RasterImage::ImageClass GetImageClass() const;
		virtual void GetRasterData(UnsafeArray<UInt8> destBuff, IntOS left, IntOS top, UIntOS width, UIntOS height, UIntOS destBpl, Bool upsideDown, Media::RotateType destRotate) const;

		virtual void WaitForVBlank();
		virtual void *GetHandle();

		virtual Bool DrawFromBuff();
		virtual Bool DrawFromSurface(NN<Media::MonitorSurface> surface, Math::Coord2D<IntOS> tl, Math::Size2D<UIntOS> drawSize, Bool clearScn, Bool waitForVBlank);
		virtual UnsafeArrayOpt<UInt8> LockSurface(OutParam<IntOS> lineAdd);
		virtual void UnlockSurface();
		virtual void SetSurfaceBugMode(Bool surfaceBugMode);

		void SetClipWindow(Optional<ControlHandle> clipWindow);
		void SetBuffSurface(NN<Media::DDrawSurface> buffSurface);
	};
}
#endif
