#ifndef _SM_MAP_MAPRENDERER
#define _SM_MAP_MAPRENDERER
#include "AnyType.h"
#include "Media/DrawEngine.h"
#include "Map/MapView.h"

namespace Map
{
	class MapRenderer
	{
	public:
		typedef void (CALLBACKFUNC UpdatedHandler)(AnyType userObj);

		virtual ~MapRenderer(){};
		virtual Bool DrawMap(NN<Media::DrawImage> img, NN<Map::MapView> view, OptOut<UInt32> imgDurMS) = 0; // true if some layers are loading
		virtual void SetUpdatedHandler(UpdatedHandler updHdlr, AnyType userObj) = 0;
	};
}
#endif
