#include "Stdafx.h"
#include "Media/FBMonitorSurfaceMgr.h"
#include "Media/MonitorSurfaceMgrFactory.h"

NotNullPtr<Media::MonitorSurfaceMgr> Media::MonitorSurfaceMgrFactory::Create(Media::MonitorMgr *monMgr, Media::ColorManagerSess *colorSess)
{
	NotNullPtr<Media::MonitorSurfaceMgr> surfaceMgr;
	NEW_CLASSNN(surfaceMgr, Media::FBMonitorSurfaceMgr(monMgr, colorSess));
	return surfaceMgr;
}

NotNullPtr<Media::MonitorSurfaceMgr> Media::MonitorSurfaceMgrFactory::Create(Media::MonitorMgr *monMgr, Media::ColorManager *colorMgr)
{
	NotNullPtr<Media::MonitorSurfaceMgr> surfaceMgr;
	NEW_CLASSNN(surfaceMgr, Media::FBMonitorSurfaceMgr(monMgr, colorMgr));
	return surfaceMgr;
}
