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
		Data::ArrayList<Data::VariObject*> objList;
		Data::ArrayListNN<DB::ColDef> cols;

		Data::VariItem *GetItem(UOSInt colIndex);
	public:
		SortableDBReader(DB::ReadingDB *db, Text::CString schemaName, Text::CString tableName, Data::ArrayListNN<Text::String> *colNames, UOSInt dataOfst, UOSInt maxCnt, Text::CString ordering, Data::QueryConditions *condition);
		virtual ~SortableDBReader();

		virtual Bool ReadNext();
		virtual UOSInt ColCount();
		virtual OSInt GetRowChanged(); //-1 = error
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
		virtual Bool GetVariItem(UOSInt colIndex, NotNullPtr<Data::VariItem> item);

		virtual Bool IsNull(UOSInt colIndex);
		virtual UTF8Char *GetName(UOSInt colIndex, UTF8Char *buff);
		virtual DB::DBUtil::ColType GetColType(UOSInt colIndex, OptOut<UOSInt> colSize);
		virtual Bool GetColDef(UOSInt colIndex, NotNullPtr<DB::ColDef> colDef);
	};
}
#endif
