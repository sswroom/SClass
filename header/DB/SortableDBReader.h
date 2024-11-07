#ifndef _SM_DB_SORTABLEDBREADER
#define _SM_DB_SORTABLEDBREADER
#include "Data/ArrayListNN.h"
#include "DB/DBReader.h"
#include "DB/ReadingDB.h"
#include "Text/String.h"

namespace DB
{
	class SortableDBReader : public DB::DBReader
	{
	private:
		UOSInt currIndex;
		Data::ArrayListNN<Data::VariObject> objList;
		Data::ArrayListNN<DB::ColDef> cols;

		Data::VariItem *GetItem(UOSInt colIndex);
	public:
		SortableDBReader(NN<DB::ReadingDB> db, Text::CString schemaName, Text::CStringNN tableName, Optional<Data::ArrayListStringNN> colNames, UOSInt dataOfst, UOSInt maxCnt, Text::CString ordering, Optional<Data::QueryConditions> condition);
		virtual ~SortableDBReader();

		virtual Bool ReadNext();
		virtual UOSInt ColCount();
		virtual OSInt GetRowChanged(); //-1 = error
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
		virtual Bool GetVariItem(UOSInt colIndex, NN<Data::VariItem> item);

		virtual Bool IsNull(UOSInt colIndex);
		virtual UnsafeArrayOpt<UTF8Char> GetName(UOSInt colIndex, UnsafeArray<UTF8Char> buff);
		virtual DB::DBUtil::ColType GetColType(UOSInt colIndex, OptOut<UOSInt> colSize);
		virtual Bool GetColDef(UOSInt colIndex, NN<DB::ColDef> colDef);
	};
}
#endif
