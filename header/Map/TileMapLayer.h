#ifndef _SM_MAP_TILEMAPLAYER
#define _SM_MAP_TILEMAPLAYER
#include "Data/SyncLinkedList.h"
#include "Media/SharedImage.h"
#include "Map/MapRenderer.h"
#include "Map/MapDrawLayer.h"
#include "Map/TileMap.h"

namespace Map
{
	class TileMapLayer : public Map::MapDrawLayer
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
			UOSInt index;
		} ThreadStat;

	private:
		NotNullPtr<Map::TileMap> tileMap;
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
		static Math::Coord2D<Int32> IdToCoord(Int64 id);
		static Int64 CoordToId(Math::Coord2D<Int32> tileId);
		void AddTask(CachedImage *cimg);
		void CheckCache(NotNullPtr<Data::ArrayListInt64> currIDs);
	public:
		TileMapLayer(NotNullPtr<Map::TileMap> tileMap, Parser::ParserList *parsers);
		virtual ~TileMapLayer();

		virtual void SetCurrScale(Double scale);
		virtual NotNullPtr<Map::MapView> CreateMapView(Math::Size2DDbl scnSize);

		virtual DrawLayerType GetLayerType() const;
		virtual UOSInt GetAllObjectIds(NotNullPtr<Data::ArrayListInt64> outArr, NameArray **nameArr);
		virtual UOSInt GetObjectIds(NotNullPtr<Data::ArrayListInt64> outArr, NameArray **nameArr, Double mapRate, Math::RectArea<Int32> rect, Bool keepEmpty);
		virtual UOSInt GetObjectIdsMapXY(NotNullPtr<Data::ArrayListInt64> outArr, NameArray **nameArr, Math::RectAreaDbl rect, Bool keepEmpty);
		virtual Int64 GetObjectIdMax() const;
		virtual void ReleaseNameArr(NameArray *nameArr);
		virtual UTF8Char *GetString(UTF8Char *buff, UOSInt buffSize, NameArray *nameArr, Int64 id, UOSInt strIndex);
		virtual UOSInt GetColumnCnt() const;
		virtual UTF8Char *GetColumnName(UTF8Char *buff, UOSInt colIndex);
		virtual DB::DBUtil::ColType GetColumnType(UOSInt colIndex, OptOut<UOSInt> colSize);
		virtual Bool GetColumnDef(UOSInt colIndex, NotNullPtr<DB::ColDef> colDef);
		virtual UInt32 GetCodePage() const;
		virtual Bool GetBounds(OutParam<Math::RectAreaDbl> bounds) const;

		virtual GetObjectSess *BeginGetObject();
		virtual void EndGetObject(GetObjectSess *session);
		virtual Math::Geometry::Vector2D *GetNewVectorById(GetObjectSess *session, Int64 id);

		virtual ObjectClass GetObjectClass() const;

		virtual Bool CanQuery();
		virtual Bool QueryInfos(Math::Coord2DDbl coord, Data::ArrayList<Math::Geometry::Vector2D*> *vecList, Data::ArrayList<UOSInt> *valueOfstList, Data::ArrayListNN<Text::String> *nameList, Data::ArrayList<Text::String*> *valueList);

		virtual void AddUpdatedHandler(UpdatedHandler hdlr, void *obj);
		virtual void RemoveUpdatedHandler(UpdatedHandler hdlr, void *obj);
		Bool IsCaching(UOSInt level, Int64 imgId);
		void WaitCache();
		NotNullPtr<Map::TileMap> GetTileMap();
	};
}
#endif
