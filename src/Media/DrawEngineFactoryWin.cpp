#include "Stdafx.h"
#include "MyMemory.h"
#include "Media/DrawEngineFactory.h"
#include "Media/GDIEngine.h"

NN<Media::DrawEngine> Media::DrawEngineFactory::CreateDrawEngine()
{
	NN<Media::GDIEngine> deng;
	NEW_CLASSNN(deng, Media::GDIEngine());
	return deng;
}
