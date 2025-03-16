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
			UOSInt colSize;
			UOSInt decimalPoint;
			UOSInt colOfst;
		} DBFColumn;

	private:
		NN<IO::SeekableStream> stm;
		UOSInt colCnt;
		DBFColumn *columns;
		UOSInt rowCnt;
		UInt64 refPos;
		Text::Encoding *enc;
		UInt8 *rec;
		UOSInt recSize;

	public:
		DBFFixWriter(NN<IO::SeekableStream> stm, UOSInt nCol, Text::String **colNames, const UOSInt *colSize, const UOSInt *dp, DB::DBUtil::ColType *colTypes, UInt32 codePage);
		~DBFFixWriter();
		void AddRecord(const UTF8Char **rowValues);

		Bool SetColumnDT(UOSInt index, NN<Data::DateTime> val);
		Bool SetColumnTS(UOSInt index, const Data::Timestamp &val);
		Bool SetColumnF64(UOSInt index, Double val);
		Bool SetColumnI16(UOSInt index, Int16 val);
		Bool SetColumnI32(UOSInt index, Int32 val);
		Bool SetColumnI64(UOSInt index, Int64 val);
		Bool SetColumnU32(UOSInt index, UInt32 val);
		Bool SetColumnBool(UOSInt index, Bool val);
		Bool SetColumnStr(UOSInt index, Text::CStringNN val);
		void WriteRecord();
	};
}
#endif
