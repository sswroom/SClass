#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/DefaultDrawEngine.h"
#include "Media/GDIEngineC.h"

Media::DrawEngine *Core::DefaultDrawEngine::CreateDrawEngine()
{
	Media::GDIEngine *deng;
	NEW_CLASS(deng, Media::GDIEngineC());
	return deng;
}
