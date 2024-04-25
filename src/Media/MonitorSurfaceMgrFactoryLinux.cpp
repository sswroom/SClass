#include "Stdafx.h"
#include "Media/FBMonitorSurfaceMgr.h"
#include "Media/MonitorSurfaceMgrFactory.h"

NN<Media::MonitorSurfaceMgr> Media::MonitorSurfaceMgrFactory::Create(Media::MonitorMgr *monMgr, Media::ColorManagerSess *colorSess)
{
	NN<Media::MonitorSurfaceMgr> surfaceMgr;
	NEW_CLASSNN(surfaceMgr, Media::FBMonitorSurfaceMgr(monMgr, colorSess));
	return surfaceMgr;
}

NN<Media::MonitorSurfaceMgr> Media::MonitorSurfaceMgrFactory::Create(Media::MonitorMgr *monMgr, Media::ColorManager *colorMgr)
{
	NN<Media::MonitorSurfaceMgr> surfaceMgr;
	NEW_CLASSNN(surfaceMgr, Media::FBMonitorSurfaceMgr(monMgr, colorMgr));
	return surfaceMgr;
}
