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
		virtual void GetImageData(UInt8 *destBuff, OSInt left, OSInt top, UOSInt width, UOSInt height, UOSInt destBpl, Bool upsideDown) const;

		virtual void WaitForVBlank();
		virtual void *GetHandle();

		virtual Bool DrawFromBuff();
		virtual Bool DrawFromSurface(Media::MonitorSurface *surface, Bool waitForVBlank);
		virtual Bool DrawFromMem(UInt8 *buff, OSInt lineAdd, OSInt destX, OSInt destY, UOSInt buffW, UOSInt buffH, Bool clearScn, Bool waitForVBlank);
		virtual UInt8 *LockSurface(OSInt *lineAdd);
		virtual void UnlockSurface();

		void SetBuffSurface(Media::MonitorSurface *buffSurface);
	};
}
#endif
