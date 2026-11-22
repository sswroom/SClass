#ifndef _SM_DB_CSVMONTHGROUPEDREADER
#define _SM_DB_CSVMONTHGROUPEDREADER
#include "Data/ArrayListUInt32.h"
#include "Data/QueryConditions.h"
#include "DB/CSVFile.h"
#include "DB/DBReader.h"
#include "Text/String.h"

namespace DB
{
	class CSVMonthGroupedReader : public DB::DBReader
	{
	private:
		NN<Text::String> filePart1;
		NN<Text::String> filePart2;
		NN<Text::String> filePart3;
		Data::Date currDate;
		Data::DataArray<Text::CStringNN> tableColNames;
		UIntOS dateCol;
		UIntOS dataOfst;
		UIntOS countLeft;
		Optional<Data::QueryConditions> condition;
		Data::ArrayListNative<UIntOS> colIndex;
		Bool noHeader;
		Optional<DB::CSVReader> currReader;
		Data::ArrayListUInt32 monthList;

	public:
		CSVMonthGroupedReader(Text::CStringNN filePart1, Text::CStringNN filePart2, Text::CStringNN filePart3, Data::DataArray<Text::CStringNN> tableColNames,
			UIntOS dateCol, UIntOS dataOfst, UIntOS maxCount, Optional<Data::QueryConditions> condition, NN<Data::ArrayListNative<UIntOS>> colIndex, Bool noHeader);
		virtual ~CSVMonthGroupedReader();

		virtual Text::CStringNN GetClassName() const;
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
		virtual Bool GetVariItem(UIntOS colIndex, NN<Data::VariItem> item);

		virtual UnsafeArrayOpt<UTF8Char> GetName(UIntOS colIndex, UnsafeArray<UTF8Char> buff);
		virtual Bool IsNull(UIntOS colIndex);
		virtual DB::DBUtil::ColType GetColType(UIntOS colIndex, OptOut<UIntOS> colSize);
		virtual Bool GetColDef(UIntOS colIndex, NN<DB::ColDef> colDef);

		virtual NN<Data::VariItem> GetNewItem(Text::CStringNN name);
	};
}
#endif
