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
			NN<Text::String> tableName;

		private:
			Data::FastMap<Int32, const UTF8Char **> *ReadNameArr();

			void Init(DB::SharedDBConn *conn, UInt32 srid, Text::CStringNN tableName);
		public:
			ESRIMDBLayer(DB::SharedDBConn *conn, UInt32 srid, NN<Text::String> sourceName, Text::CStringNN tableName);
			ESRIMDBLayer(DB::SharedDBConn *conn, UInt32 srid, Text::CStringNN sourceName, Text::CStringNN tableName);
			virtual ~ESRIMDBLayer();

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

			virtual UOSInt QueryTableNames(Text::CString schemaName, NN<Data::ArrayListStringNN> names);
			virtual Optional<DB::DBReader> QueryTableData(Text::CString schemaName, Text::CStringNN tableName, Data::ArrayListStringNN *columnNames, UOSInt ofst, UOSInt maxCnt, Text::CString ordering, Data::QueryConditions *condition);
			virtual Optional<DB::TableDef> GetTableDef(Text::CString schemaName, Text::CStringNN tableName);
			virtual void CloseReader(DB::DBReader *r);
			virtual void GetLastErrorMsg(NN<Text::StringBuilderUTF8> str);
			virtual void Reconnect();
			virtual UOSInt GetGeomCol() const;

			virtual ObjectClass GetObjectClass() const;
		};

		class ESRIMDBReader : public DB::DBReader
		{
		private:
			NN<DB::DBConn> conn;
			NN<DB::DBReader> r;
			NN<Sync::MutexUsage> mutUsage;
		public:
			ESRIMDBReader(NN<DB::DBConn> conn, NN<DB::DBReader> r, NN<Sync::MutexUsage> mutUsage);
			virtual ~ESRIMDBReader();

			virtual Bool ReadNext();
			virtual UOSInt ColCount();
			virtual OSInt GetRowChanged();

			virtual Int32 GetInt32(UOSInt colIndex);
			virtual Int64 GetInt64(UOSInt colIndex);
			virtual UnsafeArrayOpt<WChar> GetStr(UOSInt colIndex, UnsafeArray<WChar> buff);
			virtual Bool GetStr(UOSInt colIndex, NN<Text::StringBuilderUTF8> sb);
			virtual Optional<Text::String> GetNewStr(UOSInt colIndex);
			virtual UnsafeArrayOpt<UTF8Char> GetStr(UOSInt colIndex, UnsafeArray<UTF8Char> buff, UOSInt buffSize);
			virtual Data::Timestamp GetTimestamp(UOSInt colIndex);
			virtual Double GetDblOrNAN(UOSInt colIndex);
			virtual Bool GetBool(UOSInt colIndex);
			virtual UOSInt GetBinarySize(UOSInt colIndex);
			virtual UOSInt GetBinary(UOSInt colIndex, UnsafeArray<UInt8> buff);
			virtual Optional<Math::Geometry::Vector2D> GetVector(UOSInt colIndex);
			virtual Bool GetUUID(UOSInt colIndex, NN<Data::UUID> uuid);

			virtual Bool IsNull(UOSInt colIndex);
			virtual UnsafeArrayOpt<UTF8Char> GetName(UOSInt colIndex, UnsafeArray<UTF8Char> buff);
			virtual DB::DBUtil::ColType GetColType(UOSInt colIndex, OptOut<UOSInt> colSize);
			virtual Bool GetColDef(UOSInt colIndex, NN<DB::ColDef> colDef);
		};
	}
}
#endif
