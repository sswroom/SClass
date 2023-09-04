#ifndef _SM_MAP_MAPRENDERER
#define _SM_MAP_MAPRENDERER

#include "Media/DrawEngine.h"
#include "Map/MapView.h"

namespace Map
{
	class MapRenderer
	{
	public:
		typedef void (__stdcall *UpdatedHandler)(void *userObj);

		virtual ~MapRenderer(){};
		virtual void DrawMap(NotNullPtr<Media::DrawImage> img, Map::MapView *view, UInt32 *imgDurMS) = 0;
		virtual void SetUpdatedHandler(UpdatedHandler updHdlr, void *userObj) = 0;
	};
}
#endif
