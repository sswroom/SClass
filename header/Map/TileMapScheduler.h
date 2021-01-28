#ifndef _SM_MAP_TILEMAPSCHEDULER
#define _SM_MAP_TILEMAPSCHEDULER
#include "Map/MapScheduler.h"

namespace Map
{
	class TileMapScheduler : public Map::MapScheduler
	{
	protected:
		virtual void DrawPoints(Map::DrawObject *dobj);
	public:
		TileMapScheduler();
		virtual ~TileMapScheduler();
	};
}
#endif
