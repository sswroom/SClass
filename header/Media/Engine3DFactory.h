#ifndef _SM_MEDIA_ENGINE3DFACTORY
#define _SM_MEDIA_ENGINE3DFACTORY
#include "Media/Engine3D.h"

namespace Media
{
	class Engine3DFactory
	{
	public:
		static NN<Media::Engine3D> CreateEngine3D();
	};
}
#endif