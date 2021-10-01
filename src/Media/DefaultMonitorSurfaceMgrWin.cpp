#include "Stdafx.h"
#include "Media/DDrawManager.h"
#include "Media/DefaultMonitorSurfaceMgr.h"

Media::MonitorSurfaceMgr *Media::DefaultMonitorSurfaceMgr::Create(Media::MonitorMgr *monMgr, Media::ColorManagerSess *colorSess)
{
	Media::MonitorSurfaceMgr *surfaceMgr;
	NEW_CLASS(surfaceMgr, Media::DDrawManager(monMgr, colorSess));
	return surfaceMgr;
}

Media::MonitorSurfaceMgr *Media::DefaultMonitorSurfaceMgr::Create(Media::MonitorMgr *monMgr, Media::ColorManager *colorMgr)
{
	Media::MonitorSurfaceMgr *surfaceMgr;
	NEW_CLASS(surfaceMgr, Media::DDrawManager(monMgr, colorMgr));
	return surfaceMgr;
}
