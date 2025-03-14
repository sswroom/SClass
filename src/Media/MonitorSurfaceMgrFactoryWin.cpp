#include "Stdafx.h"
#include "Media/DDrawManager.h"
#include "Media/MonitorSurfaceMgrFactory.h"

NN<Media::MonitorSurfaceMgr> Media::MonitorSurfaceMgrFactory::Create(Optional<Media::MonitorMgr> monMgr, NN<Media::ColorManagerSess> colorSess)
{
	NN<Media::MonitorSurfaceMgr> surfaceMgr;
	NEW_CLASSNN(surfaceMgr, Media::DDrawManager(monMgr, colorSess));
	return surfaceMgr;
}

NN<Media::MonitorSurfaceMgr> Media::MonitorSurfaceMgrFactory::Create(NN<Media::MonitorMgr> monMgr, NN<Media::ColorManager> colorMgr)
{
	NN<Media::MonitorSurfaceMgr> surfaceMgr;
	NEW_CLASSNN(surfaceMgr, Media::DDrawManager(monMgr, colorMgr));
	return surfaceMgr;
}
