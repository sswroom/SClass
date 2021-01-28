#ifndef _SM_MAP_IMAPCONFIG
#define _SM_MAP_IMAPCONFIG
#include "Media/DrawEngine.h"
#include "Map/MapView.h"
#include "Map/MapScheduler.h"

namespace Map
{
	class IMapConfig
	{
	public:
		virtual ~IMapConfig() {};

		virtual Bool IsError() = 0;
		virtual void DrawMap(Media::DrawImage *img, Map::MapView *view, Bool *isLayerEmpty, Map::MapScheduler *mapSch) = 0;
	};
};
#endif
