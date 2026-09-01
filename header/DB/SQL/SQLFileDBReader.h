#ifndef _SM_DB_SQLFILEDBREADER
#define _SM_DB_SQLFILEDBREADER
#include "Data/ArrayListUInt32.h"
#include "DB/DBReader.h"
#include "DB/SQL/SQLFileDB.h"

namespace DB
{
	namespace SQL
	{
		class SQLFileDBReader : public DB::DBReader
		{
		private:
			NN<SQLFileDB::TableInfo> tableInfo;
			Data::ArrayListUInt32 colIndexMap;
			Optional<Data::ArrayListNN<SQLValue>> currRow;
			Optional<SQLInsertCommand> firstInsertCmd;
			Optional<Data::QueryConditions> condition;
			UIntOS nextCmdIndex;
			UIntOS nextRowIndex;
			UIntOS nextDataOfst;
			UIntOS rowReadCnt;
			UIntOS maxCnt;
			Int8 tzQhr;

			Optional<SQLInsertCommand> GetFirstInsertCmd() const;
			Int32 GetSourceColIndex(NN<SQLInsertCommand> insertCmd, Text::CStringNN colName) const;
			Optional<SQLValue> GetSQLValue(UIntOS colIndex) const;
		public:
			SQLFileDBReader(NN<SQLFileDB::TableInfo> tableInfo, Optional<Data::ArrayListStringNN> colNames, UIntOS dataOfst, UIntOS maxCnt, Optional<Data::QueryConditions> condition, Text::CString ordering, Int8 tzQhr);
			virtual ~SQLFileDBReader();

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
}
#endif
