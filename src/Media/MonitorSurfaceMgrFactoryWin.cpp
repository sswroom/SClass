#include "Stdafx.h"
#include "Media/DDrawManager.h"
#include "Media/MonitorSurfaceMgrFactory.h"

NotNullPtr<Media::MonitorSurfaceMgr> Media::MonitorSurfaceMgrFactory::Create(Media::MonitorMgr *monMgr, Media::ColorManagerSess *colorSess)
{
	NotNullPtr<Media::MonitorSurfaceMgr> surfaceMgr;
	NEW_CLASSNN(surfaceMgr, Media::DDrawManager(monMgr, colorSess));
	return surfaceMgr;
}

NotNullPtr<Media::MonitorSurfaceMgr> Media::MonitorSurfaceMgrFactory::Create(Media::MonitorMgr *monMgr, Media::ColorManager *colorMgr)
{
	NotNullPtr<Media::MonitorSurfaceMgr> surfaceMgr;
	NEW_CLASSNN(surfaceMgr, Media::DDrawManager(monMgr, colorMgr));
	return surfaceMgr;
}
