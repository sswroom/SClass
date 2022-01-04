#include "Stdafx.h"
#include "MyMemory.h"
#include "Media/DrawEngineFactory.h"
#include "Media/GTKDrawEngine.h"

Media::DrawEngine *Media::DrawEngineFactory::CreateDrawEngine()
{
	Media::GTKDrawEngine *deng;
	NEW_CLASS(deng, Media::GTKDrawEngine());
	return deng;
}
