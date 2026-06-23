#ifndef _SM_DB_DBFWRITE
#define _SM_DB_DBFWRITE

#include "Data/ArrayListArr.hpp"
#include "DB/DBUtil.h"
#include "IO/Stream.h"
#include "Text/Encoding.h"

namespace DB
{
	class DBFWrite
	{
	private:
		Data::ArrayListArr<UnsafeArray<Char>> values;
		Int32 colCnt;
		UnsafeArray<IntOS> strSize;
		UnsafeArray<UnsafeArray<const UTF8Char>> cols;
		Text::Encoding enc;
		UnsafeArray<DB::DBUtil::ColType> colTypes;
		UnsafeArray<Int32> colSizes;

	public:
		DBFWrite(Int32 nCol, UnsafeArray<UnsafeArray<const UTF8Char>> colNmes, Int32 outputCodePage, UnsafeArray<const DB::DBUtil::ColType> colTypes, UnsafeArray<const Int32> colSizes);
		~DBFWrite();
		void AddRecord(UnsafeArray<UnsafeArray<const UTF8Char>> rowValues);
		void Save(NN<IO::Stream> stm);
	};
}
#endif
