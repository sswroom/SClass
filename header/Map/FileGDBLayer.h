#ifndef _SM_MAP_FILEGDBLAYER
#define _SM_MAP_FILEGDBLAYER
#include "Data/ArrayListStrUTF8.h"
#include "Data/FastMap.h"
#include "DB/SharedReadingDB.h"
#include "Map/MapDrawLayer.h"
#include "Math/ArcGISPRJParser.h"
#include "Text/String.h"

namespace Map
{
	class FileGDBLayer : public Map::MapDrawLayer
	{
	private:
		DB::SharedReadingDB *conn;
		Data::FastMap<Int32, Math::Geometry::Vector2D*> objects;
		DB::ReadingDB *currDB;
		DB::ReadingDB *lastDB;
		Map::DrawLayerType layerType;
		Data::ArrayListStringNN colNames;
		Math::Coord2DDbl minPos;
		Math::Coord2DDbl maxPos;
		UOSInt objIdCol;
		UOSInt shapeCol;
		NN<Text::String> tableName;

	private:
		Data::FastMap<Int32, const UTF8Char **> *ReadNameArr();
	public:
		FileGDBLayer(DB::SharedReadingDB *conn, Text::CStringNN sourceName, Text::CStringNN tableName, Math::ArcGISPRJParser *prjParser);
		virtual ~FileGDBLayer();

		virtual DrawLayerType GetLayerType() const;
		virtual UOSInt GetAllObjectIds(NN<Data::ArrayListInt64> outArr, NameArray **nameArr);
		virtual UOSInt GetObjectIds(NN<Data::ArrayListInt64> outArr, NameArray **nameArr, Double mapRate, Math::RectArea<Int32> rect, Bool keepEmpty);
		virtual UOSInt GetObjectIdsMapXY(NN<Data::ArrayListInt64> outArr, NameArray **nameArr, Math::RectAreaDbl rect, Bool keepEmpty);
		virtual Int64 GetObjectIdMax() const;
		virtual void ReleaseNameArr(NameArray *nameArr);
		virtual Bool GetString(NN<Text::StringBuilderUTF8> sb, NameArray *nameArr, Int64 id, UOSInt strIndex);
		virtual UOSInt GetColumnCnt() const;
		virtual UTF8Char *GetColumnName(UTF8Char *buff, UOSInt colIndex);
		virtual DB::DBUtil::ColType GetColumnType(UOSInt colIndex, OptOut<UOSInt> colSize);
		virtual Bool GetColumnDef(UOSInt colIndex, NN<DB::ColDef> colDef);
		virtual UInt32 GetCodePage() const;
		virtual Bool GetBounds(OutParam<Math::RectAreaDbl> rect) const;

		virtual GetObjectSess *BeginGetObject();
		virtual void EndGetObject(GetObjectSess *session);
		virtual Math::Geometry::Vector2D *GetNewVectorById(GetObjectSess *session, Int64 id);
		virtual void AddUpdatedHandler(UpdatedHandler hdlr, AnyType obj);
		virtual void RemoveUpdatedHandler(UpdatedHandler hdlr, AnyType obj);

		virtual UOSInt QueryTableNames(Text::CString schemaName, NN<Data::ArrayListStringNN> names);
		virtual Optional<DB::DBReader> QueryTableData(Text::CString schemaName, Text::CString tableName, Data::ArrayListStringNN *columnName, UOSInt ofst, UOSInt maxCnt, Text::CString ordering, Data::QueryConditions *condition);
		virtual DB::TableDef *GetTableDef(Text::CString schemaName, Text::CString tableName);
		virtual void CloseReader(NN<DB::DBReader> r);
		virtual void GetLastErrorMsg(NN<Text::StringBuilderUTF8> str);
		virtual void Reconnect();

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
		virtual UOSInt ColCount();
		virtual OSInt GetRowChanged();

		virtual Int32 GetInt32(UOSInt colIndex);
		virtual Int64 GetInt64(UOSInt colIndex);
		virtual WChar *GetStr(UOSInt colIndex, WChar *buff);
		virtual Bool GetStr(UOSInt colIndex, NN<Text::StringBuilderUTF8> sb);
		virtual Optional<Text::String> GetNewStr(UOSInt colIndex);
		virtual UTF8Char *GetStr(UOSInt colIndex, UTF8Char *buff, UOSInt buffSize);
		virtual Data::Timestamp GetTimestamp(UOSInt colIndex);
		virtual Double GetDbl(UOSInt colIndex);
		virtual Bool GetBool(UOSInt colIndex);
		virtual UOSInt GetBinarySize(UOSInt colIndex);
		virtual UOSInt GetBinary(UOSInt colIndex, UInt8 *buff);
		virtual Optional<Math::Geometry::Vector2D> GetVector(UOSInt colIndex);
		virtual Bool GetUUID(UOSInt colIndex, NN<Data::UUID> uuid);

		virtual Bool IsNull(UOSInt colIndex);
		virtual UTF8Char *GetName(UOSInt colIndex, UTF8Char *buff);
		virtual DB::DBUtil::ColType GetColType(UOSInt colIndex, OptOut<UOSInt> colSize);
		virtual Bool GetColDef(UOSInt colIndex, NN<DB::ColDef> colDef);
	};
}
#endif
