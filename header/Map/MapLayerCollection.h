#ifndef _SM_MAP_MAPLAYERCOLLECTION
#define _SM_MAP_MAPLAYERCOLLECTION
#include "Data/ArrayList.h"
#include "Data/CallbackStorage.h"
#include "Map/MapDrawLayer.h"
#include "Sync/RWMutex.h"
#include "Sync/RWMutexUsage.h"

namespace Map
{
	class MapLayerCollection : public Map::MapDrawLayer
	{
	private:
		Sync::RWMutex mut;
		Data::ArrayListNN<Map::MapDrawLayer> layerList;
		Data::ArrayList<Data::CallbackStorage<Map::MapDrawLayer::UpdatedHandler>> updHdlrs;

		static void __stdcall InnerUpdated(AnyType userObj);
	public:
		MapLayerCollection(NN<Text::String> sourceName, Text::String *layerName);
		MapLayerCollection(Text::CStringNN sourceName, Text::CString layerName);
		virtual ~MapLayerCollection();

		virtual UOSInt Add(NN<Map::MapDrawLayer> val);
		virtual Optional<Map::MapDrawLayer> RemoveAt(UOSInt index);
		virtual void Clear();
		virtual UOSInt GetCount() const;
		virtual Optional<Map::MapDrawLayer> GetItem(UOSInt index) const;
		virtual void SetItem(UOSInt index, NN<Map::MapDrawLayer> val);
		Data::ArrayIterator<NN<Map::MapDrawLayer>> Iterator(NN<Sync::RWMutexUsage> mutUsage) const;

		virtual void SetCurrScale(Double scale);
		virtual void SetCurrTimeTS(Int64 timeStamp);
		virtual Int64 GetTimeStartTS() const;
		virtual Int64 GetTimeEndTS() const;

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
		virtual Bool GetBounds(OutParam<Math::RectAreaDbl> rect) const;

		virtual NN<GetObjectSess> BeginGetObject();
		virtual void EndGetObject(NN<GetObjectSess> session);
		virtual Optional<Math::Geometry::Vector2D> GetNewVectorById(NN<GetObjectSess> session, Int64 id);
		virtual void AddUpdatedHandler(UpdatedHandler hdlr, AnyType obj);
		virtual void RemoveUpdatedHandler(UpdatedHandler hdlr, AnyType obj);

		virtual ObjectClass GetObjectClass() const;
		virtual NN<Math::CoordinateSystem> GetCoordinateSystem();
		virtual void SetCoordinateSystem(NN<Math::CoordinateSystem> csys);

		void ReleaseAll();
		UOSInt GetUpdatedHandlerCnt() const;
		UpdatedHandler GetUpdatedHandler(UOSInt index) const;
		AnyType GetUpdatedObject(UOSInt index) const;
	};
}
#endif
