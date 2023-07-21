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
			Media::DrawImage *img;
		} CachedImage;
	private:
		NotNullPtr<Media::DrawEngine> eng;
		Map::TileMap *map;
		Parser::ParserList *parsers;

		OSInt lastLevel;
		Data::ArrayListInt64 *lastIds;
		Data::ArrayList<CachedImage *> *lastImgs;

	public:
		TileMapRenderer(NotNullPtr<Media::DrawEngine> eng, Map::TileMap *map, Parser::ParserList *parsers);
		virtual ~TileMapRenderer();

		virtual void DrawMap(Media::DrawImage *img, Map::MapView *view);
		virtual void SetUpdatedHandler(UpdatedHandler updHdlr, void *userObj);
	};
};
#endif
