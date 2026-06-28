#ifndef _SM_MAP_RESIZABLETILEMAPRENDERER
#define _SM_MAP_RESIZABLETILEMAPRENDERER
#include "Sync/Event.h"
#include "Map/MapRenderer.h"
#include "Media/DrawEngine.h"
#include "Media/StaticImage.h"
#include "Media/ImageResizer.h"
#include "Media/ColorManager.h"
#include "Map/TileMap.h"
#include "Map/MapView.h"
#include "Data/SyncLinkedList.h"
#include "Data/ArrayListInt64.h"
#include "Parser/ParserList.h"

namespace Map
{
	class ResizableTileMapRenderer : public MapRenderer
	{
	private:
		typedef struct
		{
			Math::Coord2D<Int32> imgId;
			Double tlx;
			Double tly;
			Double brx;
			Double bry;
			IntOS level;
			Bool isFinish;
			Bool isCancel;
			Optional<Media::StaticImage> img;
		} CachedImage;

		typedef struct
		{
			Bool toStop;
			Bool running;
			ResizableTileMapRenderer *me;
			NN<Sync::Event> evt;
		} ThreadStat;
	private:
		NN<Media::DrawEngine> eng;
		NN<Map::TileMap> map;
		NN<Parser::ParserList> parsers;
		NN<Media::ImageResizer> resizer;
		Media::ColorProfile srcColor;
		Media::ColorProfile outputColor;

		UIntOS threadCnt;
		UnsafeArray<ThreadStat> threads;
		UIntOS threadNext;

		IntOS lastLevel;
		Data::ArrayListT<Math::Coord2D<Int32>> lastIds;
		Data::ArrayListNN<CachedImage> lastImgs;
		Data::ArrayListNN<CachedImage> idleImgs;

		Data::SyncLinkedList taskQueued;
		Sync::Event taskEvt;
		Sync::Mutex taskMut;
		UpdatedHandler updHdlr;
		AnyType updObj;

		static UInt32 __stdcall TaskThread(AnyType userObj);
		void AddTask(NN<CachedImage> cimg);
		void DrawImage(NN<Map::MapView> view, NN<Media::DrawImage> img, NN<CachedImage> cimg);
	public:
		ResizableTileMapRenderer(NN<Media::DrawEngine> eng, NN<Map::TileMap> map, NN<Parser::ParserList> parsers, Optional<Media::ColorManagerSess> sess, NN<Media::ColorProfile> outputColor);
		virtual ~ResizableTileMapRenderer();

		virtual void DrawMap(NN<Media::DrawImage> img, NN<Map::MapView> view);
		virtual void SetUpdatedHandler(UpdatedHandler hdlr, AnyType userObj);
	};
}
#endif
