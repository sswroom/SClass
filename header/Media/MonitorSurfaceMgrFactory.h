#ifndef _SM_MEDIA_MONITORSURFACEMGRFACTORY
#define _SM_MEDIA_MONITORSURFACEMGRFACTORY
#include "Media/ColorManager.h"
#include "Media/MonitorMgr.h"
#include "Media/MonitorSurfaceMgr.h"

namespace Media
{
	class MonitorSurfaceMgrFactory
	{
	public:
		static NN<MonitorSurfaceMgr> Create(Media::MonitorMgr *monMgr, Media::ColorManagerSess *colorSess);
		static NN<MonitorSurfaceMgr> Create(Media::MonitorMgr *monMgr, Media::ColorManager *colorMgr);
	};
}
#endif
