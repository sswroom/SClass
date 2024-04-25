#include "Stdafx.h"
#include "MyMemory.h"
#include "Media/DrawEngineFactory.h"
#include "Media/StaticEngine.h"

NN<Media::DrawEngine> Media::DrawEngineFactory::CreateDrawEngine()
{
	NN<Media::StaticEngine> deng;
	NEW_CLASSNN(deng, Media::StaticEngine(0));
	return deng;
}
