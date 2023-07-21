#include "Stdafx.h"
#include "MyMemory.h"
#include "Media/DrawEngineFactory.h"
#include "Media/GDIEngine.h"

NotNullPtr<Media::DrawEngine> Media::DrawEngineFactory::CreateDrawEngine()
{
	NotNullPtr<Media::GDIEngine> deng;
	NEW_CLASSNN(deng, Media::GDIEngine());
	return deng;
}
