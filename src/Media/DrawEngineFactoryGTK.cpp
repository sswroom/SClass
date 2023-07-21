#include "Stdafx.h"
#include "MyMemory.h"
#include "Media/DrawEngineFactory.h"
#include "Media/GTKDrawEngine.h"

NotNullPtr<Media::DrawEngine> Media::DrawEngineFactory::CreateDrawEngine()
{
	NotNullPtr<Media::GTKDrawEngine> deng;
	NEW_CLASSNN(deng, Media::GTKDrawEngine());
	return deng;
}
