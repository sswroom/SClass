#ifndef _SM_MAP_TILEMAPLAYER
#define _SM_MAP_TILEMAPLAYER
#include "Data/LinkedList.h"
#include "Media/SharedImage.h"
#include "Map/MapRenderer.h"
#include "Map/IMapDrawLayer.h"
#include "Map/TileMap.h"

namespace Map
{
	class TileMapLayer : public Map::IMapDrawLayer
	{
	private:
		typedef struct
		{
			Int64 imgId;
			Double tlx;
			Double tly;
			Double brx;
			Double bry;
			UOSInt level;
			Bool isFinish;
			Bool isCancel;
			Media::SharedImage *img;
		} CachedImage;

		typedef struct
		{
			Bool toStop;
			Bool running;
			Bool isIdle;
			TileMapLayer *me;
			Sync::Event *evt;
		} ThreadStat;

	private:
		Map::TileMap *tileMap;
		Double scale;
		Parser::ParserList *parsers;

		UOSInt threadCnt;
		ThreadStat *threads;
		UOSInt threadNext;

		OSInt lastLevel;
		Sync::Mutex *lastMut;
		Data::ArrayListInt64 *lastIds;
		Data::ArrayList<CachedImage *> *lastImgs;
		Sync::Mutex *idleMut;
		Data::ArrayList<CachedImage *> *idleImgs;

		Data::LinkedList *taskQueued;
		Sync::Event *taskEvt;
		Sync::Mutex *taskMut;
		Sync::Mutex *updMut;
		Data::ArrayList<UpdatedHandler> *updHdlrs;
		Data::ArrayList<void *> *updObjs;

		static UInt32 __stdcall TaskThread(void *userObj);
		void AddTask(CachedImage *cimg);
		void CheckCache(Data::ArrayListInt64 *currIDs);
	public:
		TileMapLayer(Map::TileMap *tileMap, Parser::ParserList *parsers);
		virtual ~TileMapLayer();

		virtual void SetCurrScale(Double scale);
		virtual Map::MapView *CreateMapView(UOSInt width, UOSInt height);

		virtual DrawLayerType GetLayerType();
		virtual UOSInt GetAllObjectIds(Data::ArrayListInt64 *outArr, void **nameArr);
		virtual UOSInt GetObjectIds(Data::ArrayListInt64 *outArr, void **nameArr, Double mapRate, Int32 x1, Int32 y1, Int32 x2, Int32 y2, Bool keepEmpty);
		virtual UOSInt GetObjectIdsMapXY(Data::ArrayListInt64 *outArr, void **nameArr, Double x1, Double y1, Double x2, Double y2, Bool keepEmpty);
		virtual Int64 GetObjectIdMax();
		virtual void ReleaseNameArr(void *nameArr);
		virtual UTF8Char *GetString(UTF8Char *buff, UOSInt buffSize, void *nameArr, Int64 id, UOSInt strIndex);
		virtual UOSInt GetColumnCnt();
		virtual UTF8Char *GetColumnName(UTF8Char *buff, UOSInt colIndex);
		virtual DB::DBUtil::ColType GetColumnType(UOSInt colIndex, UOSInt *colSize);
		virtual Bool GetColumnDef(UOSInt colIndex, DB::ColDef *colDef);
		virtual Int32 GetCodePage();
		virtual Bool GetBoundsDbl(Double *minX, Double *minY, Double *maxX, Double *maxY);

		virtual void *BeginGetObject();
		virtual void EndGetObject(void *session);
		virtual DrawObjectL *GetObjectByIdD(void *session, Int64 id);
		virtual Math::Vector2D *GetVectorById(void *session, Int64 id);
		virtual void ReleaseObject(void *session, DrawObjectL *obj);

		virtual ObjectClass GetObjectClass();

		virtual void AddUpdatedHandler(UpdatedHandler hdlr, void *obj);
		virtual void RemoveUpdatedHandler(UpdatedHandler hdlr, void *obj);
		Bool IsCaching(UOSInt level, Int64 imgId);
		void WaitCache();
		Map::TileMap *GetTileMap();
	};
}
#endif
