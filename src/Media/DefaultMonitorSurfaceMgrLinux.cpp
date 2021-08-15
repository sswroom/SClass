#include "Stdafx.h"
#include "Media/DefaultMonitorSurfaceMgr.h"
#include "Media/FBMonitorSurfaceMgr.h"

Media::MonitorSurfaceMgr *Media::DefaultMonitorSurfaceMgr::Create(UI::GUICore *ui, Media::ColorManagerSess *colorSess)
{
	Media::MonitorSurfaceMgr *surfaceMgr;
	NEW_CLASS(surfaceMgr, Media::FBMonitorSurfaceMgr(ui, colorSess));
	return surfaceMgr;
}

Media::MonitorSurfaceMgr *Media::DefaultMonitorSurfaceMgr::Create(Media::MonitorMgr *monMgr, Media::ColorManager *colorMgr)
{
	Media::MonitorSurfaceMgr *surfaceMgr;
	NEW_CLASS(surfaceMgr, Media::FBMonitorSurfaceMgr(monMgr, colorMgr));
	return surfaceMgr;
}
