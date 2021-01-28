#ifndef _SM_DB_DBFWRITE
#define _SM_DB_DBFWRITE

#include "Data/ArrayList.h"
#include "Text/Encoding.h"
#include "IO/Stream.h"
#include "DB/DBConn.h"

namespace DB
{
	class DBFWrite
	{
	private:
		Data::ArrayList<Char**> *values;
		Int32 colCnt;
		OSInt *strSize;
		const WChar **cols;
		Text::Encoding *enc;
		DB::DBConn::ColType *colTypes;
		Int32 *colSizes;

	public:
		DBFWrite(Int32 nCol, const WChar **colNmes, Int32 outputCodePage, const DB::DBConn::ColType *colTypes, const Int32 *colSizes);
		~DBFWrite();
		void AddRecord(const WChar **rowValues);
		void Save(IO::Stream *stm);
	};
};
#endif
