#include "Stdafx.h"
#include "Media/MonitorSurfaceMgrFactory.h"

Media::MonitorSurfaceMgr *Media::MonitorSurfaceMgrFactory::Create(Media::MonitorMgr *monMgr, Media::ColorManagerSess *colorSess)
{
	return 0;
}

Media::MonitorSurfaceMgr *Media::MonitorSurfaceMgrFactory::Create(Media::MonitorMgr *monMgr, Media::ColorManager *colorMgr)
{
	return 0;
}
