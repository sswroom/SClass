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
		static NN<MonitorSurfaceMgr> Create(Optional<Media::MonitorMgr> monMgr, NN<Media::ColorManagerSess> colorSess);
		static NN<MonitorSurfaceMgr> Create(NN<Media::MonitorMgr> monMgr, NN<Media::ColorManager> colorMgr);
	};
}
#endif
