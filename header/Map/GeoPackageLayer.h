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
		Map::GeoPackage::ContentInfo *layerContent;
		DB::TableDef *tabDef;
		Data::ArrayList<Math::Geometry::Vector2D*> vecList;
		UOSInt geomCol;
		MixedData mixedData;

		StringSession *StringSessCreate();
		Bool StringSessGoRow(StringSession *sess, UOSInt index);
	public:
		GeoPackageLayer(Map::GeoPackage *gpkg, Map::GeoPackage::ContentInfo *layerContent);
		~GeoPackageLayer();

		virtual DrawLayerType GetLayerType();
		virtual void SetMixedData(MixedData mixedData);
		virtual UOSInt GetAllObjectIds(Data::ArrayListInt64 *outArr, NameArray **nameArr);
		virtual UOSInt GetObjectIds(Data::ArrayListInt64 *outArr, NameArray **nameArr, Double mapRate, Math::RectArea<Int32> rect, Bool keepEmpty);
		virtual UOSInt GetObjectIdsMapXY(Data::ArrayListInt64 *outArr, NameArray **nameArr, Math::RectAreaDbl rect, Bool keepEmpty);
		virtual Int64 GetObjectIdMax();
		virtual void ReleaseNameArr(NameArray *nameArr);
		virtual UTF8Char *GetString(UTF8Char *buff, UOSInt buffSize, NameArray *nameArr, Int64 id, UOSInt strIndex);
		virtual UOSInt GetColumnCnt();
		virtual UTF8Char *GetColumnName(UTF8Char *buff, UOSInt colIndex);
		virtual DB::DBUtil::ColType GetColumnType(UOSInt colIndex, UOSInt *colSize);
		virtual Bool GetColumnDef(UOSInt colIndex, NotNullPtr<DB::ColDef> colDef);
		virtual UInt32 GetCodePage();
		virtual Bool GetBounds(Math::RectAreaDbl *rect);

		virtual GetObjectSess *BeginGetObject();
		virtual void EndGetObject(GetObjectSess *session);
		virtual Math::Geometry::Vector2D *GetNewVectorById(GetObjectSess *session, Int64 id);

		virtual UOSInt QueryTableNames(Text::CString schemaName, Data::ArrayListNN<Text::String> *names);
		virtual DB::DBReader *QueryTableData(Text::CString schemaName, Text::CString tableName, Data::ArrayListNN<Text::String> *columnNames, UOSInt ofst, UOSInt maxCnt, Text::CString ordering, Data::QueryConditions *condition);
		virtual DB::TableDef *GetTableDef(Text::CString schemaName, Text::CString tableName);
		virtual void CloseReader(DB::DBReader *r);
		virtual void GetLastErrorMsg(NotNullPtr<Text::StringBuilderUTF8> str);
		virtual void Reconnect();

		virtual ObjectClass GetObjectClass();

		void MultiplyCoordinates(Double v);
	};
}
#endif
