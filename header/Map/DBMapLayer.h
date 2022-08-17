#ifndef _SM_MAP_DBMAPLAYER
#define _SM_MAP_DBMAPLAYER
#include "Data/Int64Map.h"
#include "DB/DBTool.h"
#include "Map/IMapDrawLayer.h"

namespace Map
{
	class DBMapLayer : public Map::IMapDrawLayer
	{
	private:
		DB::DBTool *db;
		Text::String *schema;
		Text::String *table;
		Math::Coord2DDbl min;
		Math::Coord2DDbl max;
		UOSInt idCol;
		UOSInt vecCol;
		DB::TableDef *tabDef;
		Data::Int64Map<Math::Geometry::Vector2D*> vecMap;
		Math::Geometry::Vector2D::VectorType mixedType;
		Bool pointType;

		void ClearDB();
	public:
		DBMapLayer(Text::String *layerName);
		DBMapLayer(Text::CString layerName);
		virtual ~DBMapLayer();

		virtual DrawLayerType GetLayerType();
		virtual void SetMixedType(Bool pointType);
		virtual UOSInt GetAllObjectIds(Data::ArrayListInt64 *outArr, void **nameArr);
		virtual UOSInt GetObjectIds(Data::ArrayListInt64 *outArr, void **nameArr, Double mapRate, Math::RectArea<Int32> rect, Bool keepEmpty);
		virtual UOSInt GetObjectIdsMapXY(Data::ArrayListInt64 *outArr, void **nameArr, Math::RectAreaDbl rect, Bool keepEmpty);
		virtual Int64 GetObjectIdMax();
		virtual void ReleaseNameArr(void *nameArr);
		virtual UTF8Char *GetString(UTF8Char *buff, UOSInt buffSize, void *nameArr, Int64 id, UOSInt strIndex);
		virtual UOSInt GetColumnCnt();
		virtual UTF8Char *GetColumnName(UTF8Char *buff, UOSInt colIndex);
		virtual DB::DBUtil::ColType GetColumnType(UOSInt colIndex, UOSInt *colSize);
		virtual Bool GetColumnDef(UOSInt colIndex, DB::ColDef *colDef);
		virtual UInt32 GetCodePage();
		virtual Bool GetBounds(Math::RectAreaDbl *rect);

		virtual void *BeginGetObject();
		virtual void EndGetObject(void *session);
		virtual Math::Geometry::Vector2D *GetNewVectorById(void *session, Int64 id);
		
		virtual UOSInt QueryTableNames(Text::CString schemaName, Data::ArrayList<Text::String*> *names);
		virtual DB::DBReader *QueryTableData(Text::CString schemaName, Text::CString tableName, Data::ArrayList<Text::String*> *columnNames, UOSInt ofst, UOSInt maxCnt, Text::CString ordering, Data::QueryConditions *condition);
		virtual void CloseReader(DB::DBReader *r);
		virtual void GetErrorMsg(Text::StringBuilderUTF8 *str);
		virtual void Reconnect();

		virtual ObjectClass GetObjectClass();

		Bool SetDatabase(DB::DBTool *db, Text::CString schemaName, Text::CString tableName);
	};
}
#endif
