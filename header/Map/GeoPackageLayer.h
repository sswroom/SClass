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
			DB::DBReader *r;
			UOSInt thisId;
		};

		Map::GeoPackage *gpkg;
		NotNullPtr<Map::GeoPackage::ContentInfo> layerContent;
		DB::TableDef *tabDef;
		Data::ArrayList<Math::Geometry::Vector2D*> vecList;
		UOSInt geomCol;
		MixedData mixedData;

		StringSession *StringSessCreate();
		Bool StringSessGoRow(StringSession *sess, UOSInt index);
	public:
		GeoPackageLayer(Map::GeoPackage *gpkg, NotNullPtr<Map::GeoPackage::ContentInfo> layerContent);
		virtual ~GeoPackageLayer();

		virtual DrawLayerType GetLayerType() const;
		virtual void SetMixedData(MixedData mixedData);
		virtual UOSInt GetAllObjectIds(NotNullPtr<Data::ArrayListInt64> outArr, NameArray **nameArr);
		virtual UOSInt GetObjectIds(NotNullPtr<Data::ArrayListInt64> outArr, NameArray **nameArr, Double mapRate, Math::RectArea<Int32> rect, Bool keepEmpty);
		virtual UOSInt GetObjectIdsMapXY(NotNullPtr<Data::ArrayListInt64> outArr, NameArray **nameArr, Math::RectAreaDbl rect, Bool keepEmpty);
		virtual Int64 GetObjectIdMax() const;
		virtual void ReleaseNameArr(NameArray *nameArr);
		virtual Bool GetString(NotNullPtr<Text::StringBuilderUTF8> sb, NameArray *nameArr, Int64 id, UOSInt strIndex);
		virtual UOSInt GetColumnCnt() const;
		virtual UTF8Char *GetColumnName(UTF8Char *buff, UOSInt colIndex);
		virtual DB::DBUtil::ColType GetColumnType(UOSInt colIndex, OptOut<UOSInt> colSize);
		virtual Bool GetColumnDef(UOSInt colIndex, NotNullPtr<DB::ColDef> colDef);
		virtual UInt32 GetCodePage() const;
		virtual Bool GetBounds(OutParam<Math::RectAreaDbl> rect) const;

		virtual GetObjectSess *BeginGetObject();
		virtual void EndGetObject(GetObjectSess *session);
		virtual Math::Geometry::Vector2D *GetNewVectorById(GetObjectSess *session, Int64 id);

		virtual UOSInt QueryTableNames(Text::CString schemaName, NotNullPtr<Data::ArrayListNN<Text::String>> names);
		virtual DB::DBReader *QueryTableData(Text::CString schemaName, Text::CString tableName, Data::ArrayListNN<Text::String> *columnNames, UOSInt ofst, UOSInt maxCnt, Text::CString ordering, Data::QueryConditions *condition);
		virtual DB::TableDef *GetTableDef(Text::CString schemaName, Text::CString tableName);
		virtual void CloseReader(NotNullPtr<DB::DBReader> r);
		virtual void GetLastErrorMsg(NotNullPtr<Text::StringBuilderUTF8> str);
		virtual void Reconnect();

		virtual ObjectClass GetObjectClass() const;

		void MultiplyCoordinates(Double v);
	};
}
#endif
