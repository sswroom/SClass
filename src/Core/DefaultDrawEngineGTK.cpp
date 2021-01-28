#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/DefaultDrawEngine.h"
#include "Media/GTKDrawEngine.h"

Media::DrawEngine *Core::DefaultDrawEngine::CreateDrawEngine()
{
	Media::GTKDrawEngine *deng;
	NEW_CLASS(deng, Media::GTKDrawEngine());
	return deng;
}
