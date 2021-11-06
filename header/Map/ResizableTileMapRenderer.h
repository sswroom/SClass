#ifndef _SM_MAP_RESIZABLETILEMAPRENDERER
#define _SM_MAP_RESIZABLETILEMAPRENDERER
#include "Sync/Event.h"
#include "Map/MapRenderer.h"
#include "Media/DrawEngine.h"
#include "Media/StaticImage.h"
#include "Media/IImgResizer.h"
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
			Int64 imgId;
			Double tlx;
			Double tly;
			Double brx;
			Double bry;
			OSInt level;
			Bool isFinish;
			Bool isCancel;
			Media::StaticImage *img;
		} CachedImage;

		typedef struct
		{
			Bool toStop;
			Bool running;
			ResizableTileMapRenderer *me;
			Sync::Event *evt;
		} ThreadStat;
	private:
		Media::DrawEngine *eng;
		Map::TileMap *map;
		Parser::ParserList *parsers;
		Media::IImgResizer *resizer;
		Media::ColorManager *colorMgr;
		Media::CS::TransferType outputRGBType;
		Double outputGamma;

		OSInt threadCnt;
		ThreadStat *threads;
		OSInt threadNext;

		OSInt lastLevel;
		Data::ArrayListInt64 *lastIds;
		Data::ArrayList<CachedImage *> *lastImgs;
		Data::ArrayList<CachedImage *> *idleImgs;

		Data::SyncLinkedList *taskQueued;
		Sync::Event *taskEvt;
		Sync::Mutex *taskMut;
		UpdatedHandler updHdlr;
		void *updObj;

		static UInt32 __stdcall TaskThread(void *userObj);
		void AddTask(CachedImage *cimg);
		void DrawImage(Map::MapView *view, Media::DrawImage *img, CachedImage *cimg);
	public:
		ResizableTileMapRenderer(Media::DrawEngine *eng, Map::TileMap *map, Parser::ParserList *parsers, Media::ColorManager *colorMgr, Media::CS::TransferType outputRGBType, Double outputGamma);
		virtual ~ResizableTileMapRenderer();

		virtual void DrawMap(Media::DrawImage *img, Map::MapView *view);
		virtual void SetUpdatedHandler(UpdatedHandler hdlr, void *userObj);
	};
};
#endif
