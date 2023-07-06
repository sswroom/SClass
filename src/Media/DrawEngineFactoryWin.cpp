#include "Stdafx.h"
#include "MyMemory.h"
#include "Media/DrawEngineFactory.h"
#include "Media/GDIEngine.h"

Media::DrawEngine *Media::DrawEngineFactory::CreateDrawEngine()
{
	Media::GDIEngine *deng;
	NEW_CLASS(deng, Media::GDIEngine());
	return deng;
}
