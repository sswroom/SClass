#ifndef _SM_MEDIA_MONITORSURFACE
#define _SM_MEDIA_MONITORSURFACE
#include "Media/Image.h"

namespace Media
{
	class MonitorSurface : public Media::Image
	{
	public:
		MonitorSurface() : Media::Image(0, 0) {};
		virtual ~MonitorSurface() {};

		virtual void WaitForVBlank() = 0;
		virtual void *GetHandle() = 0;

		virtual Bool DrawFromBuff() = 0;
		virtual Bool DrawFromSurface(Media::MonitorSurface *surface, Bool waitForVBlank) = 0;
	};
}
#endif
