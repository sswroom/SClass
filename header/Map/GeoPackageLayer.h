#ifndef _SM_MAP_GEOPACKAGELAYER
#define _SM_MAP_GEOPACKAGELAYER
#include "Map/GeoPackage.h"
#include "Map/MapDrawLayer.h"

namespace Map
{
	class GeoPackageLayer : public Map::MapDrawLayer
	{
	private:
		struct StringSession
		{
			Optional<DB::DBReader> r;
			UIntOS thisId;
		};

		NN<Map::GeoPackage> gpkg;
		NN<Map::GeoPackage::ContentInfo> layerContent;
		Optional<DB::TableDef> tabDef;
		Data::ArrayListObj<Optional<Math::Geometry::Vector2D>> vecList;
		UIntOS geomCol;
		MixedData mixedData;

		NN<StringSession> StringSessCreate();
		Bool StringSessGoRow(NN<StringSession> sess, UIntOS index);
	public:
		GeoPackageLayer(NN<Map::GeoPackage> gpkg, NN<Map::GeoPackage::ContentInfo> layerContent);
		virtual ~GeoPackageLayer();

		virtual DrawLayerType GetLayerType() const;
		virtual void SetMixedData(MixedData mixedData);
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

		virtual UIntOS QueryTableNames(Text::CString schemaName, NN<Data::ArrayListStringNN> names);
		virtual Optional<DB::DBReader> QueryTableData(Text::CString schemaName, Text::CStringNN tableName, Optional<Data::ArrayListStringNN> columnNames, UIntOS ofst, UIntOS maxCnt, Text::CString ordering, Optional<Data::QueryConditions> condition);
		virtual Optional<DB::TableDef> GetTableDef(Text::CString schemaName, Text::CStringNN tableName);
		virtual void CloseReader(NN<DB::DBReader> r);
		virtual void GetLastErrorMsg(NN<Text::StringBuilderUTF8> str);
		virtual void Reconnect();
		virtual UIntOS GetGeomCol() const;

		virtual ObjectClass GetObjectClass() const;

		void MultiplyCoordinates(Double v);
	};
}
#endif
