#include "Stdafx.h"
#include "MyMemory.h"
#include "Media/DrawEngineFactory.h"
#include "Media/StaticEngine.h"

Media::DrawEngine *Media::DrawEngineFactory::CreateDrawEngine()
{
	Media::StaticEngine *deng;
	NEW_CLASS(deng, Media::StaticEngine(0));
	return deng;
}
