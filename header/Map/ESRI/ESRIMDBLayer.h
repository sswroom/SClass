#ifndef _SM_MAP_ESRI_ESRIMDBLAYER
#define _SM_MAP_ESRI_ESRIMDBLAYER
#include "Data/ArrayListStringNN.h"
#include "Data/FastMap.h"
#include "DB/DBReader.h"
#include "DB/SharedDBConn.h"
#include "Map/MapDrawLayer.h"

namespace Map
{
	namespace ESRI
	{
		class ESRIMDBLayer : public Map::MapDrawLayer
		{
		private:
			DB::SharedDBConn *conn;
			Data::FastMap<Int32, Math::Geometry::Vector2D*> objects;
			DB::DBConn *currDB;
			DB::DBConn *lastDB;
			Map::DrawLayerType layerType;
			Data::ArrayListStringNN colNames;
			Math::Coord2DDbl min;
			Math::Coord2DDbl max;
			UOSInt objIdCol;
			UOSInt shapeCol;
			NotNullPtr<Text::String> tableName;

		private:
			Data::FastMap<Int32, const UTF8Char **> *ReadNameArr();

			void Init(DB::SharedDBConn *conn, UInt32 srid, Text::CString tableName);
		public:
			ESRIMDBLayer(DB::SharedDBConn *conn, UInt32 srid, NotNullPtr<Text::String> sourceName, Text::CString tableName);
			ESRIMDBLayer(DB::SharedDBConn *conn, UInt32 srid, Text::CStringNN sourceName, Text::CString tableName);
			virtual ~ESRIMDBLayer();

			virtual DrawLayerType GetLayerType() const;
			virtual UOSInt GetAllObjectIds(NotNullPtr<Data::ArrayListInt64> outArr, NameArray **nameArr);
			virtual UOSInt GetObjectIds(NotNullPtr<Data::ArrayListInt64> outArr, NameArray **nameArr, Double mapRate, Math::RectArea<Int32> rect, Bool keepEmpty);
			virtual UOSInt GetObjectIdsMapXY(NotNullPtr<Data::ArrayListInt64> outArr, NameArray **nameArr, Math::RectAreaDbl rect, Bool keepEmpty);
			virtual Int64 GetObjectIdMax() const;
			virtual void ReleaseNameArr(NameArray *nameArr);
			virtual UTF8Char *GetString(UTF8Char *buff, UOSInt buffSize, NameArray *nameArr, Int64 id, UOSInt strIndex);
			virtual UOSInt GetColumnCnt() const;
			virtual UTF8Char *GetColumnName(UTF8Char *buff, UOSInt colIndex);
			virtual DB::DBUtil::ColType GetColumnType(UOSInt colIndex, OptOut<UOSInt> colSize);
			virtual Bool GetColumnDef(UOSInt colIndex, NotNullPtr<DB::ColDef> colDef);
			virtual UInt32 GetCodePage() const;
			virtual Bool GetBounds(OutParam<Math::RectAreaDbl> rect) const;

			virtual GetObjectSess *BeginGetObject();
			virtual void EndGetObject(GetObjectSess *session);
			virtual Math::Geometry::Vector2D *GetNewVectorById(GetObjectSess *session, Int64 id);
			virtual void AddUpdatedHandler(UpdatedHandler hdlr, void *obj);
			virtual void RemoveUpdatedHandler(UpdatedHandler hdlr, void *obj);

			virtual UOSInt QueryTableNames(Text::CString schemaName, Data::ArrayListNN<Text::String> *names);
			virtual DB::DBReader *QueryTableData(Text::CString schemaName, Text::CString tableName, Data::ArrayListNN<Text::String> *columnNames, UOSInt ofst, UOSInt maxCnt, Text::CString ordering, Data::QueryConditions *condition);
			virtual DB::TableDef *GetTableDef(Text::CString schemaName, Text::CString tableName);
			virtual void CloseReader(DB::DBReader *r);
			virtual void GetLastErrorMsg(NotNullPtr<Text::StringBuilderUTF8> str);
			virtual void Reconnect();

			virtual ObjectClass GetObjectClass() const;
		};

		class ESRIMDBReader : public DB::DBReader
		{
		private:
			NotNullPtr<DB::DBConn> conn;
			NotNullPtr<DB::DBReader> r;
			NotNullPtr<Sync::MutexUsage> mutUsage;
		public:
			ESRIMDBReader(NotNullPtr<DB::DBConn> conn, NotNullPtr<DB::DBReader> r, NotNullPtr<Sync::MutexUsage> mutUsage);
			virtual ~ESRIMDBReader();

			virtual Bool ReadNext();
			virtual UOSInt ColCount();
			virtual OSInt GetRowChanged();

			virtual Int32 GetInt32(UOSInt colIndex);
			virtual Int64 GetInt64(UOSInt colIndex);
			virtual WChar *GetStr(UOSInt colIndex, WChar *buff);
			virtual Bool GetStr(UOSInt colIndex, NotNullPtr<Text::StringBuilderUTF8> sb);
			virtual Text::String *GetNewStr(UOSInt colIndex);
			virtual UTF8Char *GetStr(UOSInt colIndex, UTF8Char *buff, UOSInt buffSize);
			virtual Data::Timestamp GetTimestamp(UOSInt colIndex);
			virtual Double GetDbl(UOSInt colIndex);
			virtual Bool GetBool(UOSInt colIndex);
			virtual UOSInt GetBinarySize(UOSInt colIndex);
			virtual UOSInt GetBinary(UOSInt colIndex, UInt8 *buff);
			virtual Math::Geometry::Vector2D *GetVector(UOSInt colIndex);
			virtual Bool GetUUID(UOSInt colIndex, NotNullPtr<Data::UUID> uuid);

			virtual Bool IsNull(UOSInt colIndex);
			virtual UTF8Char *GetName(UOSInt colIndex, UTF8Char *buff);
			virtual DB::DBUtil::ColType GetColType(UOSInt colIndex, OptOut<UOSInt> colSize);
			virtual Bool GetColDef(UOSInt colIndex, NotNullPtr<DB::ColDef> colDef);
		};
	}
}
#endif
