#ifndef _SM_MAP_DRAWMAPSERVICELAYER
#define _SM_MAP_DRAWMAPSERVICELAYER
#include "AnyType.h"
#include "Data/CallbackStorage.h"
#include "Data/SyncLinkedList.h"
#include "Map/DrawMapService.h"
#include "Map/MapDrawLayer.h"
#include "Map/MapRenderer.h"
#include "Media/SharedImage.h"

namespace Map
{
	class DrawMapServiceLayer : public Map::MapDrawLayer
	{
	private:
		NN<Map::DrawMapService> mapService;

		Bool threadToStop;
		Bool threadRunning;
		Sync::Event *threadEvt;

		Sync::Mutex dispMut;
		Math::RectAreaDbl dispBounds;
		Math::Size2DDbl dispSize;
		Double dispDPI;
		Int64 dispId;
		Optional<Media::SharedImage> dispImage;
		Optional<Text::String> dispImageURL;
		Math::RectAreaDbl lastBounds;
		Math::Size2DDbl lastSize;
		Double lastDPI;
		Int64 lastId;
		Optional<Media::SharedImage> lastImage;
		Optional<Text::String> lastImageURL;

		Sync::Mutex updMut;
		Data::ArrayList<Data::CallbackStorage<UpdatedHandler>> updHdlrs;

		static UInt32 __stdcall TaskThread(AnyType userObj);
		void ClearDisp();
	public:
		DrawMapServiceLayer(NN<Map::DrawMapService> mapService);
		virtual ~DrawMapServiceLayer();

		virtual void SetCurrScale(Double scale);
		virtual NN<Map::MapView> CreateMapView(Math::Size2DDbl scnSize);

		virtual DrawLayerType GetLayerType() const;
		virtual UOSInt GetAllObjectIds(NN<Data::ArrayListInt64> outArr, OptOut<Optional<NameArray>> nameArr);
		virtual UOSInt GetObjectIds(NN<Data::ArrayListInt64> outArr, OptOut<Optional<NameArray>> nameArr, Double mapRate, Math::RectArea<Int32> rect, Bool keepEmpty);
		virtual UOSInt GetObjectIdsMapXY(NN<Data::ArrayListInt64> outArr, OptOut<Optional<NameArray>> nameArr, Math::RectAreaDbl rect, Bool keepEmpty);
		virtual Int64 GetObjectIdMax() const;
		virtual void ReleaseNameArr(Optional<NameArray> nameArr);
		virtual Bool GetString(NN<Text::StringBuilderUTF8> sb, Optional<NameArray> nameArr, Int64 id, UOSInt strIndex);
		virtual UOSInt GetColumnCnt() const;
		virtual UnsafeArrayOpt<UTF8Char> GetColumnName(UnsafeArray<UTF8Char> buff, UOSInt colIndex);
		virtual DB::DBUtil::ColType GetColumnType(UOSInt colIndex, OptOut<UOSInt> colSize);
		virtual Bool GetColumnDef(UOSInt colIndex, NN<DB::ColDef> colDef);
		virtual UInt32 GetCodePage() const;
		virtual Bool GetBounds(OutParam<Math::RectAreaDbl> bounds) const;
		virtual void SetDispSize(Math::Size2DDbl size, Double dpi);

		virtual NN<GetObjectSess> BeginGetObject();
		virtual void EndGetObject(NN<GetObjectSess> session);
		virtual Optional<Math::Geometry::Vector2D> GetNewVectorById(NN<GetObjectSess> session, Int64 id);

		virtual ObjectClass GetObjectClass() const;

		virtual Bool CanQuery();
		virtual Bool QueryInfos(Math::Coord2DDbl coord, NN<Data::ArrayListNN<Math::Geometry::Vector2D>> vecList, NN<Data::ArrayList<UOSInt>> valueOfstList, NN<Data::ArrayListStringNN> nameList, NN<Data::ArrayListNN<Text::String>> valueList);

		virtual void AddUpdatedHandler(UpdatedHandler hdlr, AnyType obj);
		virtual void RemoveUpdatedHandler(UpdatedHandler hdlr, AnyType obj);
		NN<Map::DrawMapService> GetDrawMapService();
	};
}
#endif
