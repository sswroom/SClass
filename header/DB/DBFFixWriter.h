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

		Bool SetColumn(UOSInt index, Data::DateTime *val);
		Bool SetColumn(UOSInt index, const Data::Timestamp &val);
		Bool SetColumn(UOSInt index, Double val);
		Bool SetColumn(UOSInt index, Int16 val);
		Bool SetColumn(UOSInt index, Int32 val);
		Bool SetColumn(UOSInt index, Int64 val);
		Bool SetColumn(UOSInt index, UInt32 val);
		Bool SetColumn(UOSInt index, Bool val);
		Bool SetColumn(UOSInt index, const UTF8Char *val);
		void WriteRecord();
	};
}
#endif
