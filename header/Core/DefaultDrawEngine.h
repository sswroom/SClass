#ifndef _SM_CORE_DEFAULTDRAWENGINE
#define _SM_CORE_DEFAULTDRAWENGINE
#include "Media/DrawEngine.h"

namespace Core
{
	class DefaultDrawEngine
	{
	public:
		static Media::DrawEngine *CreateDrawEngine();
	};
}
#endif
