#ifndef _SM_MAP_TILEMAPRENDERER
#define _SM_MAP_TILEMAPRENDERER
#include "Map/MapRenderer.h"
#include "Media/DrawEngine.h"
#include "Map/TileMap.h"
#include "Map/MapView.h"
#include "Data/ArrayListInt64.h"
#include "Parser/ParserList.h"

namespace Map
{
	class TileMapRenderer : public MapRenderer
	{
	private:
		typedef struct
		{
			Double tlx;
			Double tly;
			NN<Media::DrawImage> img;
		} CachedImage;
	private:
		NN<Media::DrawEngine> eng;
		Map::TileMap *map;
		NN<Parser::ParserList> parsers;

		OSInt lastLevel;
		Data::ArrayListInt64 lastIds;
		Data::ArrayList<CachedImage *> lastImgs;

	public:
		TileMapRenderer(NN<Media::DrawEngine> eng, Map::TileMap *map, NN<Parser::ParserList> parsers);
		virtual ~TileMapRenderer();

		virtual void DrawMap(NN<Media::DrawImage> img, NN<Map::MapView> view, OptOut<UInt32> imgDurMS);
		virtual void SetUpdatedHandler(UpdatedHandler updHdlr, AnyType userObj);
	};
}
#endif
