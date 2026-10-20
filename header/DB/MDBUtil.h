#ifndef _SM_DB_MDBUTIL
#define _SM_DB_MDBUTIL
#include "DB/ColDef.h"
#include "DB/MDBFile.h"
#include "Text/StringBuilderUTF8.h"
#include "UnsafeArrayOpt.h"

namespace DB
{
	class MDBUtil
	{
	public:
		struct Cell
		{
			UnsafeArrayOpt<const UInt8> data;
			UIntOS size;
			UInt8 value;
			Bool isNull;
		};

		static Bool ParseRecord(const MDBFile::TableInfo &table, UnsafeArray<const UInt8> record, UIntOS recordSize, UnsafeArray<Cell> cells);
		static UInt32 GetCellUInt32(UnsafeArray<const Cell> cells, UIntOS cellCount, IntOS index);
		static DB::DBUtil::ColType GetMDBColType(UInt8 type);
		static void CellToString(const MDBFile::ColumnInfo &col, const Cell &cell, NN<Text::StringBuilderUTF8> out);
	};
}

#endif
