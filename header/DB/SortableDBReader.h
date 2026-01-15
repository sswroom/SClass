#ifndef _SM_DB_SORTABLEDBREADER
#define _SM_DB_SORTABLEDBREADER
#include "Data/ArrayListNN.hpp"
#include "DB/DBReader.h"
#include "DB/ReadingDB.h"
#include "Text/String.h"

namespace DB
{
	class SortableDBReader : public DB::DBReader
	{
	private:
		UIntOS currIndex;
		Data::ArrayListNN<Data::VariObject> objList;
		Data::ArrayListNN<DB::ColDef> cols;

		Optional<Data::VariItem> GetItem(UIntOS colIndex);
	public:
		SortableDBReader(NN<DB::ReadingDB> db, Text::CString schemaName, Text::CStringNN tableName, Optional<Data::ArrayListStringNN> colNames, UIntOS dataOfst, UIntOS maxCnt, Text::CString ordering, Optional<Data::QueryConditions> condition);
		virtual ~SortableDBReader();

		virtual Bool ReadNext();
		virtual UIntOS ColCount();
		virtual IntOS GetRowChanged(); //-1 = error
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
		virtual Bool GetVariItem(UIntOS colIndex, NN<Data::VariItem> item);

		virtual Bool IsNull(UIntOS colIndex);
		virtual UnsafeArrayOpt<UTF8Char> GetName(UIntOS colIndex, UnsafeArray<UTF8Char> buff);
		virtual DB::DBUtil::ColType GetColType(UIntOS colIndex, OptOut<UIntOS> colSize);
		virtual Bool GetColDef(UIntOS colIndex, NN<DB::ColDef> colDef);
	};
}
#endif
