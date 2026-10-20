#include "Stdafx.h"
#include "Core/ByteTool_C.h"
#include "DB/MDBUtil.h"
#include "DB/MDBFileInt.h"
#include "Text/MyStringW.h"

Bool DB::MDBUtil::ParseRecord(const DB::MDBFile::TableInfo &table, UnsafeArray<const UInt8> record, UIntOS recordSize, UnsafeArray<Cell> cells)
{
	UnsafeArray<DB::MDBFile::ColumnInfo> columns;
	if (!table.columns.SetTo(columns))
		return false;
	if (recordSize < 4)
		return false;
	UIntOS numCols = ReadLUInt16(record.Ptr());
	UIntOS nullSize = (numCols + 7) >> 3;
	if (numCols > 512 || nullSize + 4 > recordSize)
		return false;
	UIntOS varCount = ReadLUInt16(&record[recordSize - nullSize - 2]);
	if (varCount > table.numVarCols || (varCount * 2) + nullSize + 4 > recordSize)
		return false;
	UIntOS varTableStart = recordSize - nullSize - 2 - varCount * 2;
	if (varTableStart < 4)
		return false;
	UIntOS eod = ReadLUInt16(&record[varTableStart - 2]);
	if (eod > recordSize || eod < 2)
		return false;

	for (UIntOS i = 0; i < table.columnCount; ++i)
	{
		DB::MDBFile::ColumnInfo col = columns[i];
		Cell cell;
		cell.data = nullptr;
		cell.size = 0;
		cell.value = 0;
		cell.isNull = true;
		if (col.colNum >= numCols)
		{
			cell.isNull = true;
			cells[i] = cell;
			continue;
		}
		Bool notNull = (record[recordSize - nullSize + (col.colNum >> 3)] & (1U << (col.colNum & 7))) != 0;
		cell.isNull = !notNull;
		if (col.type == 1)
		{
			cell.isNull = false;
			cell.value = notNull ? 1 : 0;
			cells[i] = cell;
			continue;
		}
		if (cell.isNull)
		{
			cells[i] = cell;
			continue;
		}

		UIntOS start;
		UIntOS end;
		if (col.flags & 0x01)
		{
			start = 2 + col.fixedOffset;
			end = start + col.size;
		}
		else
		{
			if (col.varNum >= varCount)
			{
				cell.isNull = true;
				cells[i] = cell;
				continue;
			}
			UIntOS slot = varTableStart + (varCount - col.varNum - 1) * 2;
			start = ReadLUInt16(&record[slot]);
			if ((UIntOS)col.varNum + 1 < varCount)
			{
				UIntOS nextSlot = varTableStart + (varCount - col.varNum - 2) * 2;
				end = ReadLUInt16(&record[nextSlot]);
			}
			else
			{
				end = eod;
			}
		}
		if (start > end || end > recordSize)
			return false;
		cell.data = &record[start];
		cell.size = end - start;
		cells[i] = cell;
	}
	return true;
}

UInt32 DB::MDBUtil::GetCellUInt32(UnsafeArray<const Cell> cells, UIntOS cellCount, IntOS index)
{
	if (index < 0 || (UIntOS)index >= cellCount)
		return 0;
	const Cell &v = cells[(UIntOS)index];
	if (v.isNull)
		return 0;
	if (!v.size)
		return v.value;
	UnsafeArray<const UInt8> data;
	if (!v.data.SetTo(data))
		return 0;
	if (v.size >= 4)
		return ReadLUInt32(data.Ptr());
	if (v.size >= 2)
		return ReadLUInt16(data.Ptr());
	return v.size ? data[0] : 0;
}

DB::DBUtil::ColType DB::MDBUtil::GetMDBColType(UInt8 type)
{
	switch (type)
	{
	case 1: return DB::DBUtil::CT_Bool;
	case 2: return DB::DBUtil::CT_Byte;
	case 3: return DB::DBUtil::CT_Int16;
	case 4: return DB::DBUtil::CT_Int32;
	case 5: return DB::DBUtil::CT_Decimal;
	case 6: return DB::DBUtil::CT_Float;
	case 7: return DB::DBUtil::CT_Double;
	case 8: return DB::DBUtil::CT_DateTime;
	case 9: return DB::DBUtil::CT_Binary;
	case 10: return DB::DBUtil::CT_VarUTF16Char;
	case 11: return DB::DBUtil::CT_Binary;
	case 12: return DB::DBUtil::CT_VarUTF16Char;
	case 15: return DB::DBUtil::CT_UUID;
	case 16: return DB::DBUtil::CT_Decimal;
	default: return DB::DBUtil::CT_Unknown;
	}
}

void DB::MDBUtil::CellToString(const DB::MDBFile::ColumnInfo &col, const Cell &cell, NN<Text::StringBuilderUTF8> out)
{
	if (cell.isNull)
		return;
	const UInt8 *data = cell.data.Ptr();
	UIntOS size = cell.size;
	switch (col.type)
	{
	case 1:
		if (cell.value)
			out->AppendC(UTF8STRC("True"));
		else
			out->AppendC(UTF8STRC("False"));
		return;
	case 2:
		if (size)
			out->AppendU32(data[0]);
		return;
	case 3:
		if (size >= 2)
			out->AppendI32(ReadLInt16(data));
		return;
	case 4:
		if (size >= 4)
			out->AppendI32(ReadLInt32(data));
		return;
	case 5:
		if (size >= 8)
			out->AppendDouble((Double)ReadLInt64(data) / 10000.0);
		return;
	case 6:
		if (size >= 4)
			out->AppendDouble(ReadLFloat(data));
		return;
	case 7:
	case 8:
		if (size >= 8)
			out->AppendDouble(ReadLDouble(data));
		return;
	case 10:
	case 12:
		if (col.miscFlags & 0x01)
			out->AppendS(UnsafeArray<const UTF8Char>(data), size);
		else
		{
			UTF16Char utf16[2048];
			UIntOS utf16Size = 0;
			for (UIntOS i = 0; i + 1 < size && utf16Size < 2048; i += 2)
			{
				UTF16Char c = ReadLUInt16(&data[i]);
				if (c == 0)
					break;
				utf16[utf16Size++] = c;
			}
			out->AppendUTF16(utf16, utf16Size);
		}
		return;
	default:
		return;
	}
}
