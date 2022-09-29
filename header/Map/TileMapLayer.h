#ifndef _SM_MAP_TILEMAPLAYER
#define _SM_MAP_TILEMAPLAYER
#include "Data/SyncLinkedList.h"
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
			Math::Coord2DDbl tl;
			Math::Coord2DDbl br;
			Int64 imgId;
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

		UOSInt lastLevel;
		Sync::Mutex lastMut;
		Data::ArrayListInt64 lastIds;
		Data::ArrayList<CachedImage *> lastImgs;
		Sync::Mutex idleMut;
		Data::ArrayList<CachedImage *> idleImgs;

		Data::SyncLinkedList taskQueued;
		Sync::Event taskEvt;
		Sync::Mutex taskMut;
		Sync::Mutex updMut;
		Data::ArrayList<UpdatedHandler> updHdlrs;
		Data::ArrayList<void *> updObjs;

		static UInt32 __stdcall TaskThread(void *userObj);
		void AddTask(CachedImage *cimg);
		void CheckCache(Data::ArrayListInt64 *currIDs);
	public:
		TileMapLayer(Map::TileMap *tileMap, Parser::ParserList *parsers);
		virtual ~TileMapLayer();

		virtual void SetCurrScale(Double scale);
		virtual Map::MapView *CreateMapView(Math::Size2D<Double> scnSize);

		virtual DrawLayerType GetLayerType();
		virtual UOSInt GetAllObjectIds(Data::ArrayListInt64 *outArr, void **nameArr);
		virtual UOSInt GetObjectIds(Data::ArrayListInt64 *outArr, void **nameArr, Double mapRate, Math::RectArea<Int32> rect, Bool keepEmpty);
		virtual UOSInt GetObjectIdsMapXY(Data::ArrayListInt64 *outArr, void **nameArr, Math::RectAreaDbl rect, Bool keepEmpty);
		virtual Int64 GetObjectIdMax();
		virtual void ReleaseNameArr(void *nameArr);
		virtual UTF8Char *GetString(UTF8Char *buff, UOSInt buffSize, void *nameArr, Int64 id, UOSInt strIndex);
		virtual UOSInt GetColumnCnt();
		virtual UTF8Char *GetColumnName(UTF8Char *buff, UOSInt colIndex);
		virtual DB::DBUtil::ColType GetColumnType(UOSInt colIndex, UOSInt *colSize);
		virtual Bool GetColumnDef(UOSInt colIndex, DB::ColDef *colDef);
		virtual UInt32 GetCodePage();
		virtual Bool GetBounds(Math::RectAreaDbl *bounds);

		virtual void *BeginGetObject();
		virtual void EndGetObject(void *session);
		virtual Math::Geometry::Vector2D *GetNewVectorById(void *session, Int64 id);

		virtual ObjectClass GetObjectClass();

		virtual Bool CanQuery();
		virtual Math::Geometry::Vector2D *QueryInfo(Math::Coord2DDbl coord, Data::ArrayList<Text::String*> *nameList, Data::ArrayList<Text::String*> *valueList);

		virtual void AddUpdatedHandler(UpdatedHandler hdlr, void *obj);
		virtual void RemoveUpdatedHandler(UpdatedHandler hdlr, void *obj);
		Bool IsCaching(UOSInt level, Int64 imgId);
		void WaitCache();
		Map::TileMap *GetTileMap();
	};
}
#endif
