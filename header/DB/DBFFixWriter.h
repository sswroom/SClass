#ifndef _SM_DB_DBFIXWRITER
#define _SM_DB_DBFIXWRITER
#include "DB/DBUtil.h"
#include "IO/SeekableStream.h"
#include "Text/Encoding.h"
#include "Text/String.h"

namespace DB
{
	class DBFFixWriter
	{
	private:
		typedef struct
		{
			DB::DBUtil::ColType colType;
			UIntOS colSize;
			UIntOS decimalPoint;
			UIntOS colOfst;
		} DBFColumn;

	private:
		NN<IO::SeekableStream> stm;
		UIntOS colCnt;
		UnsafeArray<DBFColumn> columns;
		UIntOS rowCnt;
		UInt64 refPos;
		NN<Text::Encoding> enc;
		UnsafeArray<UInt8> rec;
		UIntOS recSize;

	public:
		DBFFixWriter(NN<IO::SeekableStream> stm, UIntOS nCol, UnsafeArray<NN<Text::String>> colNames, UnsafeArray<const UIntOS> colSize, UnsafeArray<const UIntOS> dp, UnsafeArray<DB::DBUtil::ColType> colTypes, UInt32 codePage);
		~DBFFixWriter();
		void AddRecord(UnsafeArray<UnsafeArray<const UTF8Char>> rowValues);

		Bool SetColumnDT(UIntOS index, NN<Data::DateTime> val);
		Bool SetColumnTS(UIntOS index, const Data::Timestamp &val);
		Bool SetColumnF64(UIntOS index, Double val);
		Bool SetColumnI16(UIntOS index, Int16 val);
		Bool SetColumnI32(UIntOS index, Int32 val);
		Bool SetColumnI64(UIntOS index, Int64 val);
		Bool SetColumnU32(UIntOS index, UInt32 val);
		Bool SetColumnBool(UIntOS index, Bool val);
		Bool SetColumnStr(UIntOS index, Text::CStringNN val);
		void WriteRecord();
	};
}
#endif
