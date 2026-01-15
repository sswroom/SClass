#ifndef _SM_DB_DBFWRITE
#define _SM_DB_DBFWRITE

#include "Data/ArrayList.hpp"
#include "DB/DBUtil.h"
#include "IO/Stream.h"
#include "Text/Encoding.h"

namespace DB
{
	class DBFWrite
	{
	private:
		Data::ArrayList<Char**> values;
		Int32 colCnt;
		IntOS *strSize;
		const UTF8Char **cols;
		Text::Encoding enc;
		DB::DBUtil::ColType *colTypes;
		Int32 *colSizes;

	public:
		DBFWrite(Int32 nCol, const UTF8Char **colNmes, Int32 outputCodePage, const DB::DBUtil::ColType *colTypes, const Int32 *colSizes);
		~DBFWrite();
		void AddRecord(const UTF8Char **rowValues);
		void Save(IO::Stream *stm);
	};
}
#endif
