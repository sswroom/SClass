#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/DefaultDrawEngine.h"
#include "Media/StaticEngine.h"

Media::DrawEngine *Core::DefaultDrawEngine::CreateDrawEngine()
{
	Media::StaticEngine *deng;
	NEW_CLASS(deng, Media::StaticEngine(0));
	return deng;
}
