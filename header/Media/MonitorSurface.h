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
	};
}
#endif
