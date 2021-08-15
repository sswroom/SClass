#ifndef _SM_MEDIA_DEFAULTMONITORSURFACEMGR
#define _SM_MEDIA_DEFAULTMONITORSURFACEMGR
#include "Media/ColorManager.h"
#include "Media/MonitorMgr.h"
#include "Media/MonitorSurfaceMgr.h"

namespace Media
{
	class DefaultMonitorSurfaceMgr
	{
	public:
		static MonitorSurfaceMgr *Create(UI::GUICore *ui, Media::ColorManagerSess *colorSess);
		static MonitorSurfaceMgr *Create(Media::MonitorMgr *monMgr, Media::ColorManager *colorMgr);
	};
}
#endif
