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

		ClassData *clsData;

	public:
		DRMSurface(Int32 fd, MonitorHandle *hMon, Media::ColorProfile *color, Double dpi);
		virtual ~DRMSurface();

		Bool IsError();
		
		virtual Media::Image *Clone() const;
		virtual Media::Image::ImageType GetImageType() const;
		virtual void GetImageData(UInt8 *destBuff, IntOS left, IntOS top, UIntOS width, UIntOS height, UIntOS destBpl, Bool upsideDown, Media::RotateType destRotate) const;

		virtual void WaitForVBlank();
		virtual void *GetHandle();

		virtual Bool DrawFromBuff();
		virtual Bool DrawFromSurface(Media::MonitorSurface *surface, IntOS destX, IntOS destY, UIntOS buffW, UIntOS buffH, Bool clearScn, Bool waitForVBlank);
		virtual UInt8 *LockSurface(IntOS *lineAdd);
		virtual void UnlockSurface();

		void SetBuffSurface(Media::MonitorSurface *buffSurface);
	};
}
#endif
