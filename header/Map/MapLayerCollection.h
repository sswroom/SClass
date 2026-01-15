#ifndef _SM_MAP_MAPLAYERCOLLECTION
#define _SM_MAP_MAPLAYERCOLLECTION
#include "Data/ArrayListObj.hpp"
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
		Data::ArrayListObj<Data::CallbackStorage<Map::MapDrawLayer::UpdatedHandler>> updHdlrs;

		static void __stdcall InnerUpdated(AnyType userObj);
	public:
		MapLayerCollection(NN<Text::String> sourceName, Text::String *layerName);
		MapLayerCollection(Text::CStringNN sourceName, Text::CString layerName);
		virtual ~MapLayerCollection();

		virtual UIntOS Add(NN<Map::MapDrawLayer> val);
		virtual Optional<Map::MapDrawLayer> RemoveAt(UIntOS index);
		virtual void Clear();
		virtual UIntOS GetCount() const;
		virtual Optional<Map::MapDrawLayer> GetItem(UIntOS index) const;
		virtual void SetItem(UIntOS index, NN<Map::MapDrawLayer> val);
		Data::ArrayIterator<NN<Map::MapDrawLayer>> Iterator(NN<Sync::RWMutexUsage> mutUsage) const;

		virtual void SetCurrScale(Double scale);
		virtual void SetCurrTimeTS(Int64 timeStamp);
		virtual Int64 GetTimeStartTS() const;
		virtual Int64 GetTimeEndTS() const;

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
		virtual Bool GetBounds(OutParam<Math::RectAreaDbl> rect) const;

		virtual NN<GetObjectSess> BeginGetObject();
		virtual void EndGetObject(NN<GetObjectSess> session);
		virtual Optional<Math::Geometry::Vector2D> GetNewVectorById(NN<GetObjectSess> session, Int64 id);
		virtual void AddUpdatedHandler(UpdatedHandler hdlr, AnyType obj);
		virtual void RemoveUpdatedHandler(UpdatedHandler hdlr, AnyType obj);
		virtual UIntOS GetGeomCol() const;

		virtual ObjectClass GetObjectClass() const;
		virtual NN<Math::CoordinateSystem> GetCoordinateSystem() const;
		virtual void SetCoordinateSystem(NN<Math::CoordinateSystem> csys);

		void ReleaseAll();
		void ReorderLayers();
		UIntOS GetUpdatedHandlerCnt() const;
		UpdatedHandler GetUpdatedHandler(UIntOS index) const;
		AnyType GetUpdatedObject(UIntOS index) const;
	};
}
#endif
