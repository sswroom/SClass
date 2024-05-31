#ifndef _SM_MAP_DBMAPLAYER
#define _SM_MAP_DBMAPLAYER
#include "Data/FastMap.h"
#include "DB/DBTool.h"
#include "Map/MapDrawLayer.h"

namespace Map
{
	class DBMapLayer : public Map::MapDrawLayer
	{
	private:
		Bool releaseDB;
		DB::ReadingDB *db;
		Optional<Text::String> schema;
		NN<Text::String> table;
		Math::Coord2DDbl min;
		Math::Coord2DDbl max;
		UOSInt idCol;
		UOSInt vecCol;
		UOSInt xCol;
		UOSInt yCol;
		UOSInt zCol;
		Optional<DB::TableDef> tabDef;
		Data::FastMap<Int64, Math::Geometry::Vector2D*> vecMap;
		MixedData mixedData;

		void ClearDB();
		void *InitNameArr();
	public:
		DBMapLayer(NN<Text::String> layerName);
		DBMapLayer(Text::CStringNN layerName);
		virtual ~DBMapLayer();

		virtual DrawLayerType GetLayerType() const;
		virtual void SetMixedData(MixedData mixedData);
		virtual UOSInt GetAllObjectIds(NN<Data::ArrayListInt64> outArr, NameArray **nameArr);
		virtual UOSInt GetObjectIds(NN<Data::ArrayListInt64> outArr, NameArray **nameArr, Double mapRate, Math::RectArea<Int32> rect, Bool keepEmpty);
		virtual UOSInt GetObjectIdsMapXY(NN<Data::ArrayListInt64> outArr, NameArray **nameArr, Math::RectAreaDbl rect, Bool keepEmpty);
		virtual Int64 GetObjectIdMax() const;
		virtual void ReleaseNameArr(NameArray *nameArr);
		virtual Bool GetString(NN<Text::StringBuilderUTF8> sb, NameArray *nameArr, Int64 id, UOSInt strIndex);
		virtual UOSInt GetColumnCnt() const;
		virtual UnsafeArrayOpt<UTF8Char> GetColumnName(UnsafeArray<UTF8Char> buff, UOSInt colIndex);
		virtual DB::DBUtil::ColType GetColumnType(UOSInt colIndex, OptOut<UOSInt> colSize);
		virtual Bool GetColumnDef(UOSInt colIndex, NN<DB::ColDef> colDef);
		virtual UInt32 GetCodePage() const;
		virtual Bool GetBounds(OutParam<Math::RectAreaDbl> rect) const;

		virtual GetObjectSess *BeginGetObject();
		virtual void EndGetObject(GetObjectSess *session);
		virtual Math::Geometry::Vector2D *GetNewVectorById(GetObjectSess *session, Int64 id);
		
		virtual UOSInt QueryTableNames(Text::CString schemaName, NN<Data::ArrayListStringNN> names);
		virtual Optional<DB::DBReader> QueryTableData(Text::CString schemaName, Text::CStringNN tableName, Data::ArrayListStringNN *columnNames, UOSInt ofst, UOSInt maxCnt, Text::CString ordering, Data::QueryConditions *condition);
		virtual Optional<DB::TableDef> GetTableDef(Text::CString schemaName, Text::CStringNN tableName);
		virtual void CloseReader(NN<DB::DBReader> r);
		virtual void GetLastErrorMsg(NN<Text::StringBuilderUTF8> str);
		virtual void Reconnect();

		virtual ObjectClass GetObjectClass() const;

		Bool SetDatabase(NN<DB::ReadingDB> db, Text::CString schemaName, Text::CStringNN tableName, Bool releaseDB);
	};
}
#endif
