#ifndef _SM_MEDIA_DRAWENGINEFACTORY
#define _SM_MEDIA_DRAWENGINEFACTORY
#include "Media/DrawEngine.h"

namespace Media
{
	class DrawEngineFactory
	{
	public:
		static Media::DrawEngine *CreateDrawEngine();
	};
}
#endif