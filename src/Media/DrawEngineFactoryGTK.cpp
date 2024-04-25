#include "Stdafx.h"
#include "MyMemory.h"
#include "Media/DrawEngineFactory.h"
#include "Media/GTKDrawEngine.h"

NN<Media::DrawEngine> Media::DrawEngineFactory::CreateDrawEngine()
{
	NN<Media::GTKDrawEngine> deng;
	NEW_CLASSNN(deng, Media::GTKDrawEngine());
	return deng;
}
