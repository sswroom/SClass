#ifndef _SM_MAP_ESRI_ESRIMDBLAYER
#define _SM_MAP_ESRI_ESRIMDBLAYER
#include "Data/ArrayListStrUTF8.h"
#include "Data/Int32Map.h"
#include "DB/DBReader.h"
#include "DB/SharedDBConn.h"
#include "Map/IMapDrawLayer.h"

namespace Map
{
	namespace ESRI
	{
		class ESRIMDBLayer : public Map::IMapDrawLayer
		{
		private:
			DB::SharedDBConn *conn;
			Data::Int32Map<Math::Vector2D*> *objects;
			DB::DBConn *currDB;
			DB::DBConn *lastDB;
			Map::DrawLayerType layerType;
			Data::ArrayListString *colNames;
			Math::Coord2DDbl min;
			Math::Coord2DDbl max;
			UOSInt objIdCol;
			UOSInt shapeCol;
			Text::String *tableName;

		private:
			Data::Int32Map<const UTF8Char **> *ReadNameArr();

			void Init(DB::SharedDBConn *conn, UInt32 srid, Text::CString tableName);
		public:
			ESRIMDBLayer(DB::SharedDBConn *conn, UInt32 srid, Text::String *sourceName, Text::CString tableName);
			ESRIMDBLayer(DB::SharedDBConn *conn, UInt32 srid, Text::CString sourceName, Text::CString tableName);
			virtual ~ESRIMDBLayer();

			virtual DrawLayerType GetLayerType();
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
			virtual DrawObjectL *GetNewObjectById(void *session, Int64 id);
			virtual Math::Vector2D *GetNewVectorById(void *session, Int64 id);
			virtual void ReleaseObject(void *session, DrawObjectL *obj);
			virtual void AddUpdatedHandler(UpdatedHandler hdlr, void *obj);
			virtual void RemoveUpdatedHandler(UpdatedHandler hdlr, void *obj);

			virtual UOSInt QueryTableNames(Text::CString schemaName, Data::ArrayList<Text::String*> *names);
			virtual DB::DBReader *QueryTableData(Text::CString schemaName, Text::CString tableName, Data::ArrayList<Text::String*> *columnNames, UOSInt ofst, UOSInt maxCnt, Text::CString ordering, Data::QueryConditions *condition);
			virtual void CloseReader(DB::DBReader *r);
			virtual void GetErrorMsg(Text::StringBuilderUTF8 *str);
			virtual void Reconnect();

			virtual ObjectClass GetObjectClass();
		};

		class ESRIMDBReader : public DB::DBReader
		{
		private:
			DB::DBConn *conn;
			DB::DBReader *r;
			Sync::MutexUsage *mutUsage;
		public:
			ESRIMDBReader(DB::DBConn *conn, DB::DBReader *r, Sync::MutexUsage *mutUsage);
			virtual ~ESRIMDBReader();

			virtual Bool ReadNext();
			virtual UOSInt ColCount();
			virtual OSInt GetRowChanged();

			virtual Int32 GetInt32(UOSInt colIndex);
			virtual Int64 GetInt64(UOSInt colIndex);
			virtual WChar *GetStr(UOSInt colIndex, WChar *buff);
			virtual Bool GetStr(UOSInt colIndex, Text::StringBuilderUTF8 *sb);
			virtual Text::String *GetNewStr(UOSInt colIndex);
			virtual UTF8Char *GetStr(UOSInt colIndex, UTF8Char *buff, UOSInt buffSize);
			virtual DateErrType GetDate(UOSInt colIndex, Data::DateTime *outVal); //return 0 = ok, 2 = Error
			virtual Double GetDbl(UOSInt colIndex);
			virtual Bool GetBool(UOSInt colIndex);
			virtual UOSInt GetBinarySize(UOSInt colIndex);
			virtual UOSInt GetBinary(UOSInt colIndex, UInt8 *buff);
			virtual Math::Vector2D *GetVector(UOSInt colIndex);
			virtual Bool GetUUID(UOSInt colIndex, Data::UUID *uuid);

			virtual Bool IsNull(UOSInt colIndex);
			virtual UTF8Char *GetName(UOSInt colIndex, UTF8Char *buff);
			virtual DB::DBUtil::ColType GetColType(UOSInt colIndex, UOSInt *colSize);
			virtual Bool GetColDef(UOSInt colIndex, DB::ColDef *colDef);
		};
	}
}
#endif
