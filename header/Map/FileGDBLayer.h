#ifndef _SM_MAP_FILEGDBLAYER
#define _SM_MAP_FILEGDBLAYER
#include "Data/ArrayListStrUTF8.h"
#include "Data/FastMapNN.hpp"
#include "Data/FastMapObj.hpp"
#include "DB/SharedReadingDB.h"
#include "Map/MapDrawLayer.h"
#include "Math/ArcGISPRJParser.h"
#include "Text/String.h"

namespace Map
{
	class FileGDBLayer : public Map::MapDrawLayer
	{
	private:
		NN<DB::SharedReadingDB> conn;
		Data::FastMapNN<Int32, Math::Geometry::Vector2D> objects;
		Optional<DB::ReadingDB> lastDB;
		Map::DrawLayerType layerType;
		Data::ArrayListStringNN colNames;
		Math::Coord2DDbl minPos;
		Math::Coord2DDbl maxPos;
		UIntOS objIdCol;
		UIntOS shapeCol;
		NN<Text::String> tableName;
		Optional<DB::TableDef> tabDef;

	private:
		Optional<Data::FastMapObj<Int32, UnsafeArrayOpt<UnsafeArrayOpt<const UTF8Char>>>> ReadNameArr();
	public:
		FileGDBLayer(NN<DB::SharedReadingDB> conn, Text::CStringNN sourceName, Text::CStringNN tableName, NN<Math::ArcGISPRJParser> prjParser);
		virtual ~FileGDBLayer();

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

		virtual UIntOS QueryTableNames(Text::CString schemaName, NN<Data::ArrayListStringNN> names);
		virtual Optional<DB::DBReader> QueryTableData(Text::CString schemaName, Text::CStringNN tableName, Optional<Data::ArrayListStringNN> columnName, UIntOS ofst, UIntOS maxCnt, Text::CString ordering, Optional<Data::QueryConditions> condition);
		virtual Optional<DB::TableDef> GetTableDef(Text::CString schemaName, Text::CStringNN tableName);
		virtual void CloseReader(NN<DB::DBReader> r);
		virtual void GetLastErrorMsg(NN<Text::StringBuilderUTF8> str);
		virtual void Reconnect();
		virtual UIntOS GetGeomCol() const;

		virtual ObjectClass GetObjectClass() const;
	};

	class FileGDBLReader : public DB::DBReader
	{
	private:
		NN<DB::ReadingDB> conn;
		NN<DB::DBReader> r;
		NN<Sync::MutexUsage> mutUsage;
	public:
		FileGDBLReader(NN<DB::ReadingDB> conn, NN<DB::DBReader> r, NN<Sync::MutexUsage> mutUsage);
		virtual ~FileGDBLReader();

		virtual Bool ReadNext();
		virtual UIntOS ColCount();
		virtual IntOS GetRowChanged();

		virtual Int32 GetInt32(UIntOS colIndex);
		virtual Int64 GetInt64(UIntOS colIndex);
		virtual UnsafeArrayOpt<WChar> GetStr(UIntOS colIndex, UnsafeArray<WChar> buff);
		virtual Bool GetStr(UIntOS colIndex, NN<Text::StringBuilderUTF8> sb);
		virtual Optional<Text::String> GetNewStr(UIntOS colIndex);
		virtual UnsafeArrayOpt<UTF8Char> GetStr(UIntOS colIndex, UnsafeArray<UTF8Char> buff, UIntOS buffSize);
		virtual Data::Timestamp GetTimestamp(UIntOS colIndex);
		virtual Double GetDblOrNAN(UIntOS colIndex);
		virtual Bool GetBool(UIntOS colIndex);
		virtual UIntOS GetBinarySize(UIntOS colIndex);
		virtual UIntOS GetBinary(UIntOS colIndex, UnsafeArray<UInt8> buff);
		virtual Optional<Math::Geometry::Vector2D> GetVector(UIntOS colIndex);
		virtual Bool GetUUID(UIntOS colIndex, NN<Data::UUID> uuid);

		virtual Bool IsNull(UIntOS colIndex);
		virtual UnsafeArrayOpt<UTF8Char> GetName(UIntOS colIndex, UnsafeArray<UTF8Char> buff);
		virtual DB::DBUtil::ColType GetColType(UIntOS colIndex, OptOut<UIntOS> colSize);
		virtual Bool GetColDef(UIntOS colIndex, NN<DB::ColDef> colDef);
	};
}
#endif
