#ifndef _SM_MAP_TILEMAPLAYER
#define _SM_MAP_TILEMAPLAYER
#include "Data/CallbackStorage.h"
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
			UIntOS level;
			Bool isFinish;
			Bool isCancel;
			Optional<Media::SharedImage> img;
		} CachedImage;

		typedef struct
		{
			Bool toStop;
			Bool running;
			Bool isIdle;
			TileMapLayer *me;
			Sync::Event *evt;
			UIntOS index;
		} ThreadStat;

	private:
		NN<Map::TileMap> tileMap;
		Double scale;
		NN<Parser::ParserList> parsers;

		UIntOS threadCnt;
		ThreadStat *threads;
		UIntOS threadNext;

		UIntOS lastLevel;
		Sync::Mutex lastMut;
		Data::ArrayListInt64 lastIds;
		Data::ArrayListNN<CachedImage> lastImgs;
		Sync::Mutex idleMut;
		Data::ArrayListNN<CachedImage> idleImgs;

		Data::SyncLinkedList taskQueued;
		Sync::Event taskEvt;
		Sync::Mutex taskMut;
		Sync::Mutex updMut;
		Data::ArrayListObj<Data::CallbackStorage<UpdatedHandler>> updHdlrs;
		
		static UInt32 __stdcall TaskThread(AnyType userObj);
		static Math::Coord2D<Int32> IdToCoord(Int64 id);
		static Int64 CoordToId(Math::Coord2D<Int32> tileId);
		void AddTask(NN<CachedImage> cimg);
		void CheckCache(NN<Data::ArrayListInt64> currIDs);
	public:
		TileMapLayer(NN<Map::TileMap> tileMap, NN<Parser::ParserList> parsers);
		virtual ~TileMapLayer();

		virtual void SetCurrScale(Double scale);
		virtual NN<Map::MapView> CreateMapView(Math::Size2DDbl scnSize);

		virtual DrawLayerType GetLayerType() const;
		virtual UIntOS GetAllObjectIds(NN<Data::ArrayListInt64> outArr, OptOut<Optional<NameArray>> nameArr);
		virtual UIntOS GetObjectIds(NN<Data::ArrayListInt64> outArr, OptOut<Optional<NameArray>> nameArr, Double mapRate, Math::RectArea<Int32> rect, Bool keepEmpty);
		virtual UIntOS GetObjectIdsMapXY(NN<Data::ArrayListInt64> outArr, OptOut<Optional<NameArray>> nameArr, Math::RectAreaDbl rect, Bool keepEmpty);
		virtual Int64 GetObjectIdMax() const;
		virtual UIntOS GetRecordCnt() const;
		virtual void ReleaseNameArr(Optional<NameArray> nameArr);
		virtual Bool GetString(NN<Text::StringBuilderUTF8> sb, Optional<NameArray> nameArr, Int64 id, UIntOS strIndex);
		virtual UIntOS GetColumnCnt() const;
		virtual UnsafeArrayOpt<UTF8Char> GetColumnName(UnsafeArray<UTF8Char> buff, UIntOS colIndex) const;
		virtual DB::DBUtil::ColType GetColumnType(UIntOS colIndex, OptOut<UIntOS> colSize) const;
		virtual Bool GetColumnDef(UIntOS colIndex, NN<DB::ColDef> colDef) const;
		virtual UInt32 GetCodePage() const;
		virtual Bool GetBounds(OutParam<Math::RectAreaDbl> bounds) const;

		virtual NN<GetObjectSess> BeginGetObject();
		virtual void EndGetObject(NN<GetObjectSess> session);
		virtual Optional<Math::Geometry::Vector2D> GetNewVectorById(NN<GetObjectSess> session, Int64 id);
		virtual UIntOS GetGeomCol() const;

		virtual ObjectClass GetObjectClass() const;

		virtual Bool CanQuery();
		virtual Bool QueryInfos(Math::Coord2DDbl coord, NN<Data::ArrayListNN<Math::Geometry::Vector2D>> vecList, NN<Data::ArrayListNative<UIntOS>> valueOfstList, NN<Data::ArrayListStringNN> nameList, NN<Data::ArrayListNN<Text::String>> valueList);

		virtual void AddUpdatedHandler(UpdatedHandler hdlr, AnyType obj);
		virtual void RemoveUpdatedHandler(UpdatedHandler hdlr, AnyType obj);
		Bool IsCaching(UIntOS level, Int64 imgId);
		void WaitCache();
		NN<Map::TileMap> GetTileMap();
	};
}
#endif
