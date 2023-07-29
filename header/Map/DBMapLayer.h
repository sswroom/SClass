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
		Text::String *schema;
		Text::String *table;
		Math::Coord2DDbl min;
		Math::Coord2DDbl max;
		UOSInt idCol;
		UOSInt vecCol;
		UOSInt xCol;
		UOSInt yCol;
		UOSInt zCol;
		DB::TableDef *tabDef;
		Data::FastMap<Int64, Math::Geometry::Vector2D*> vecMap;
		MixedData mixedData;

		void ClearDB();
		void *InitNameArr();
	public:
		DBMapLayer(NotNullPtr<Text::String> layerName);
		DBMapLayer(Text::CString layerName);
		virtual ~DBMapLayer();

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
		virtual Bool GetColumnDef(UOSInt colIndex, DB::ColDef *colDef);
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

		Bool SetDatabase(DB::ReadingDB *db, Text::CString schemaName, Text::CString tableName, Bool releaseDB);
	};
}
#endif
