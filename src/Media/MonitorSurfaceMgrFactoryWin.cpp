#include "Stdafx.h"
#include "Media/DDrawManager.h"
#include "Media/MonitorSurfaceMgrFactory.h"

Media::MonitorSurfaceMgr *Media::MonitorSurfaceMgrFactory::Create(Media::MonitorMgr *monMgr, Media::ColorManagerSess *colorSess)
{
	Media::MonitorSurfaceMgr *surfaceMgr;
	NEW_CLASS(surfaceMgr, Media::DDrawManager(monMgr, colorSess));
	return surfaceMgr;
}

Media::MonitorSurfaceMgr *Media::MonitorSurfaceMgrFactory::Create(Media::MonitorMgr *monMgr, Media::ColorManager *colorMgr)
{
	Media::MonitorSurfaceMgr *surfaceMgr;
	NEW_CLASS(surfaceMgr, Media::DDrawManager(monMgr, colorMgr));
	return surfaceMgr;
}
