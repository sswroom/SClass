#ifndef _SM_DB_MDBFILEINT
#define _SM_DB_MDBFILEINT
#include "DB/MDBFile.h"
#include "Data/ArrayListUInt32.h"
#include "UnsafeArrayOpt.h"

struct DB::MDBFile::ColumnInfo
{
	UTF8Char name[256];
	UInt8 type;
	UInt8 flags;
	UInt8 miscFlags;
	UInt16 colNum;
	UInt16 varNum;
	UInt16 fixedOffset;
	UInt16 size;
};

struct DB::MDBFile::TableInfo
{
	UTF8Char name[256];
	UInt32 pageNo;
	UInt32 rowCount;
	UInt16 numCols;
	UInt16 numVarCols;
	UnsafeArrayOpt<ColumnInfo> columns;
	UIntOS columnCount;
	Data::ArrayListUInt32 dataPages;

	TableInfo() : columns(nullptr), columnCount(0) {}
	~TableInfo()
	{
		UnsafeArray<ColumnInfo> columns;
		if (this->columns.SetTo(columns))
			MemFreeArr(columns);
	}
};

#endif
