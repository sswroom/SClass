#include "Stdafx.h"
#include "Core/ByteTool_C.h"
#include "DB/ColDef.h"
#include "DB/MDBFile.h"
#include "DB/MDBUtil.h"
#include "DB/SortableDBReader.h"
#include "DB/TableDef.h"
#include "Data/Timestamp.h"
#include "Text/MyString.h"
#include "Text/MyStringW.h"
#include "DB/MDBFileInt.h"

class DB::MDBFileReader : public DB::DBReader
{
private:
	NN<DB::MDBFile> db;
	DB::MDBFile::TableInfo *table;
	DB::MDBFile::ColumnInfo columns[512];
	UInt32 colIndexes[512];
	DB::MDBUtil::Cell cells[512];
	UIntOS columnCount;
	UIntOS pageIndex;
	UIntOS rowIndex;
	UIntOS rowCount;
	UIntOS skipRows;
	UIntOS maxRows;
	UIntOS returnedRows;
	UnsafeArray<UInt8> pageBuff;
	Bool hasRow;

public:
	MDBFileReader(NN<DB::MDBFile> db, DB::MDBFile::TableInfo &table, Optional<Data::ArrayListStringNN> columnNames, UIntOS dataOfst, UIntOS maxCnt) :
		db(db), table(&table)
	{
		this->pageBuff = MemAllocArr(UInt8, 4096);
		this->columnCount = 0;
		this->pageIndex = 0;
		this->rowIndex = 0;
		this->rowCount = 0;
		this->skipRows = dataOfst;
		this->maxRows = maxCnt;
		this->returnedRows = 0;
		this->hasRow = false;
		UnsafeArray<DB::MDBFile::ColumnInfo> tableColumns;
		Bool hasColumns = table.columns.SetTo(tableColumns);
		NN<Data::ArrayListStringNN> names;
		if (columnNames.SetTo(names) && hasColumns)
		{
			UIntOS i = 0;
			UIntOS j = names->GetCount();
			while (i < j)
			{
				NN<Text::String> name;
				if (names->GetItem(i).SetTo(name))
				{
					UIntOS k = 0;
					while (k < table.columnCount)
					{
						DB::MDBFile::ColumnInfo col = tableColumns[k];
						if (Text::StrEqualsICase(col.name, name->v))
						{
							this->columns[this->columnCount] = col;
							this->colIndexes[this->columnCount] = (UInt32)k;
							this->columnCount++;
							break;
						}
						k++;
					}
				}
				i++;
			}
		}
		else if (hasColumns)
		{
			for (UIntOS i = 0; i < table.columnCount; ++i)
			{
				this->columns[this->columnCount] = tableColumns[i];
				this->colIndexes[this->columnCount] = (UInt32)i;
				this->columnCount++;
			}
		}
	}

	virtual ~MDBFileReader()
	{
		MemFreeArr(this->pageBuff);
	}

	virtual Bool ReadNext()
	{
		if (this->maxRows && this->returnedRows >= this->maxRows)
		{
			this->hasRow = false;
			return false;
		}
		while (this->pageIndex < this->table->dataPages.GetCount())
		{
			if (this->rowIndex >= this->rowCount)
			{
				UInt32 pageNo = this->table->dataPages.GetItem(this->pageIndex++);
				if (!this->db->ReadPage(pageNo, this->pageBuff))
					continue;
				this->rowCount = ReadLUInt16(&this->pageBuff[12]);
				if (this->rowCount > (4096 - 14) / 2)
					this->rowCount = 0;
				this->rowIndex = 0;
			}
			while (this->rowIndex < this->rowCount)
			{
				UIntOS idx = this->rowIndex++;
				UInt16 rowOfst = ReadLUInt16(&this->pageBuff[14 + idx * 2]);
				if (rowOfst & 0x8000 || rowOfst & 0x4000)
					continue;
				UIntOS start = rowOfst & 0x1fff;
				UIntOS end = idx == 0 ? 4096 : (ReadLUInt16(&this->pageBuff[14 + (idx - 1) * 2]) & 0x1fff);
				if (start >= end || end > 4096)
					continue;
				DB::MDBUtil::Cell rec[512];
				if (!MDBUtil::ParseRecord(*this->table, &this->pageBuff[start], end - start, rec))
					continue;
				if (this->skipRows)
				{
					this->skipRows--;
					continue;
				}
				for (UIntOS i = 0; i < this->columnCount; ++i)
				{
					UInt32 colIndex = this->colIndexes[i];
					if (colIndex < this->table->columnCount)
						this->cells[i] = rec[colIndex];
					else
					{
						this->cells[i].data = nullptr;
						this->cells[i].size = 0;
						this->cells[i].value = 0;
						this->cells[i].isNull = true;
					}
				}
				this->returnedRows++;
				this->hasRow = true;
				return true;
			}
		}
		this->hasRow = false;
		return false;
	}

	virtual UIntOS ColCount()
	{
		return this->columnCount;
	}

	virtual IntOS GetRowChanged()
	{
		return -1;
	}

	virtual Int32 GetInt32(UIntOS colIndex)
	{
		if (!this->hasRow || colIndex >= this->columnCount || this->cells[colIndex].isNull)
			return 0;
		DB::MDBUtil::Cell cell = this->cells[colIndex];
		DB::MDBFile::ColumnInfo col = this->columns[colIndex];
		const UInt8 *p = cell.data.Ptr();
		UIntOS size = cell.size;
		switch (col.type)
		{
		case 1: return cell.value ? 1 : 0;
		case 2: return size ? p[0] : 0;
		case 3: return size >= 2 ? ReadLInt16(p) : 0;
		case 4: return size >= 4 ? ReadLInt32(p) : 0;
		default:
		{
			Text::StringBuilderUTF8 sb;
			MDBUtil::CellToString(col, cell, NN<Text::StringBuilderUTF8>::FromPtr(&sb));
			return Text::StrToInt32(sb.ToString());
		}
		}
	}

	virtual Int64 GetInt64(UIntOS colIndex)
	{
		if (!this->hasRow || colIndex >= this->columnCount || this->cells[colIndex].isNull)
			return 0;
		DB::MDBUtil::Cell cell = this->cells[colIndex];
		const UInt8 *p = cell.data.Ptr();
		UIntOS size = cell.size;
		if (size >= 8)
		{
			return ReadLInt64(p);
		}
		return (Int64)GetInt32(colIndex);
	}

	virtual UnsafeArrayOpt<WChar> GetStr(UIntOS colIndex, UnsafeArray<WChar> buff)
	{
		if (!this->hasRow || colIndex >= this->columnCount || this->cells[colIndex].isNull)
			return nullptr;
		Text::StringBuilderUTF8 sb;
		MDBUtil::CellToString(this->columns[colIndex], this->cells[colIndex], NN<Text::StringBuilderUTF8>::FromPtr(&sb));
		return Text::StrUTF8_WCharC(buff, sb.ToString(), sb.GetLength(), 0);
	}

	virtual Bool GetStr(UIntOS colIndex, NN<Text::StringBuilderUTF8> sb)
	{
		if (!this->hasRow || colIndex >= this->columnCount || this->cells[colIndex].isNull)
			return false;
		Text::StringBuilderUTF8 tmp;
		MDBUtil::CellToString(this->columns[colIndex], this->cells[colIndex], NN<Text::StringBuilderUTF8>::FromPtr(&tmp));
		sb->AppendC(tmp.ToString(), tmp.GetLength());
		return true;
	}

	virtual Optional<Text::String> GetNewStr(UIntOS colIndex)
	{
		if (!this->hasRow || colIndex >= this->columnCount || this->cells[colIndex].isNull)
			return nullptr;
		Text::StringBuilderUTF8 sb;
		MDBUtil::CellToString(this->columns[colIndex], this->cells[colIndex], NN<Text::StringBuilderUTF8>::FromPtr(&sb));
		return Text::String::New(sb.ToString(), sb.GetLength());
	}

	virtual UnsafeArrayOpt<UTF8Char> GetStr(UIntOS colIndex, UnsafeArray<UTF8Char> buff, UIntOS buffSize)
	{
		if (!this->hasRow || colIndex >= this->columnCount || this->cells[colIndex].isNull)
			return nullptr;
		Text::StringBuilderUTF8 sb;
		MDBUtil::CellToString(this->columns[colIndex], this->cells[colIndex], NN<Text::StringBuilderUTF8>::FromPtr(&sb));
		if (sb.GetLength() + 1 > buffSize)
			return nullptr;
		MemCopyNO(buff.Ptr(), sb.ToString().Ptr(), sb.GetLength() + 1);
		return buff + sb.GetLength();
	}

	virtual Data::Timestamp GetTimestamp(UIntOS colIndex)
	{
		if (!this->hasRow || colIndex >= this->columnCount || this->cells[colIndex].isNull ||
			this->columns[colIndex].type != 8 || this->cells[colIndex].size < 8)
			return Data::Timestamp(nullptr);
		Double d = ReadLDouble(this->cells[colIndex].data.Ptr());
		if (Math::IsNAN(d) || Math::IsInfinity(d))
			return Data::Timestamp(nullptr);
		Double secs = (d - 25569.0) * 86400.0;
		Int64 whole = (Int64)secs;
		UInt32 ns = (UInt32)((secs - (Double)whole) * 1000000000.0);
		if (ns > 999999999)
			ns = 999999999;
		return Data::Timestamp(Data::TimeInstant(whole, ns), this->db->GetTzQhr());
	}

	virtual Double GetDblOrNAN(UIntOS colIndex)
	{
		if (!this->hasRow || colIndex >= this->columnCount || this->cells[colIndex].isNull)
			return NAN;
		DB::MDBUtil::Cell cell = this->cells[colIndex];
		DB::MDBFile::ColumnInfo col = this->columns[colIndex];
		const UInt8 *p = cell.data.Ptr();
		UIntOS size = cell.size;
		switch (col.type)
		{
		case 5:
			if (size >= 8)
			{
				Int64 v = ReadLInt64(p);
				return (Double)v / 10000.0;
			}
			break;
		case 6:
			if (size >= 4)
				return ReadLFloat(p);
			break;
		case 7:
		case 8:
			if (size >= 8)
				return ReadLDouble(p);
			break;
		default:
			return (Double)GetInt64(colIndex);
		}
		return NAN;
	}

	virtual Bool GetBool(UIntOS colIndex)
	{
		return GetInt32(colIndex) != 0;
	}

	virtual UIntOS GetBinarySize(UIntOS colIndex)
	{
		if (!this->hasRow || colIndex >= this->columnCount || this->cells[colIndex].isNull)
			return 0;
		if (this->columns[colIndex].type == 1)
			return 1;
		return this->cells[colIndex].size;
	}

	virtual UIntOS GetBinary(UIntOS colIndex, UnsafeArray<UInt8> buff)
	{
		UIntOS size = GetBinarySize(colIndex);
		if (size && this->columns[colIndex].type == 1)
		{
			buff[0] = this->cells[colIndex].value;
		}
		else if (size)
			MemCopyNO(buff.Ptr(), this->cells[colIndex].data.Ptr(), size);
		return size;
	}

	virtual Optional<Math::Geometry::Vector2D> GetVector(UIntOS colIndex)
	{
		return nullptr;
	}

	virtual Bool GetUUID(UIntOS colIndex, NN<Data::UUID> uuid)
	{
		if (!this->hasRow || colIndex >= this->columnCount || this->cells[colIndex].isNull ||
			this->columns[colIndex].type != 15 || this->cells[colIndex].size != 16)
			return false;
		uuid->SetValue(UnsafeArray<const UInt8>(this->cells[colIndex].data.Ptr()));
		return true;
	}

	virtual Bool IsNull(UIntOS colIndex)
	{
		return !this->hasRow || colIndex >= this->columnCount || this->cells[colIndex].isNull;
	}

	virtual UnsafeArrayOpt<UTF8Char> GetName(UIntOS colIndex, UnsafeArray<UTF8Char> buff)
	{
		if (colIndex >= this->columnCount)
			return nullptr;
		return Text::StrConcat(buff, this->columns[colIndex].name);
	}

	virtual DB::DBUtil::ColType GetColType(UIntOS colIndex, OptOut<UIntOS> colSize)
	{
		if (colIndex >= this->columnCount)
		{
			colSize.Set(0);
			return DB::DBUtil::CT_Unknown;
		}
		DB::MDBFile::ColumnInfo col = this->columns[colIndex];
		colSize.Set(col.size);
		return MDBUtil::GetMDBColType(col.type);
	}

	virtual Bool GetColDef(UIntOS colIndex, NN<DB::ColDef> colDef)
	{
		if (colIndex >= this->columnCount)
			return false;
		DB::MDBFile::ColumnInfo col = this->columns[colIndex];
		colDef->SetColName(col.name);
		colDef->SetColType(MDBUtil::GetMDBColType(col.type));
		colDef->SetColSize(col.size);
		colDef->SetNotNull(false);
		colDef->SetPK(false);
		colDef->SetAutoIncNone();
		colDef->SetDefVal(Text::CString(nullptr));
		colDef->SetAttr(Text::CString(nullptr));
		return true;
	}
};

DB::MDBFile::MDBFile(NN<IO::StreamData> stmData) : DB::ReadingDB(stmData->GetFullName())
{
	this->stmData = stmData->GetPartialData(0, stmData->GetDataSize());
	this->pageSize = 4096;
	this->tzQhr = Data::DateTimeUtil::GetLocalTzQhr();
	this->error = true;
	this->lastError[0] = 0;
	UInt8 hdr[24];
	if (stmData->GetDataSize() < sizeof(hdr) || stmData->GetRealData(0, sizeof(hdr), BYTEARR(hdr)) != sizeof(hdr))
	{
		Text::StrConcat(this->lastError, (const UTF8Char*)"MDB header is truncated");
		return;
	}
	static const UInt8 jetSignature[] = "Standard Jet DB";
	Bool signatureMatch = true;
	for (UIntOS i = 0; i < sizeof(jetSignature) - 1; ++i)
	{
		if (hdr[4 + i] != jetSignature[i])
		{
			signatureMatch = false;
			break;
		}
	}
	if (ReadLUInt32(hdr) != 0x100 || !signatureMatch)
	{
		Text::StrConcat(this->lastError, (const UTF8Char*)"Unsupported MDB file header");
		return;
	}
	if (ReadLUInt32(&hdr[20]) != 1)
	{
		Text::StrConcat(this->lastError, (const UTF8Char*)"Only Jet 4 MDB files are supported by this build");
		return;
	}
	if (this->stmData->GetDataSize() % this->pageSize != 0)
	{
		Text::StrConcat(this->lastError, (const UTF8Char*)"MDB file size is not aligned to its page size");
		return;
	}
	if (!this->ReadCatalog())
	{
		Text::StrConcat(this->lastError, (const UTF8Char*)"Unable to read MDB table catalog");
		return;
	}
	this->error = false;
	this->lastError[0] = 0;
}

DB::MDBFile::~MDBFile()
{
	this->tables.DeleteAll();
	this->stmData.Delete();
}

Bool DB::MDBFile::ReadPage(UInt32 pageNo, UnsafeArray<UInt8> buff) const
{
	UInt64 ofst = (UInt64)pageNo * this->pageSize;
	if (ofst > this->stmData->GetDataSize() || this->pageSize > this->stmData->GetDataSize() - ofst)
		return false;
	return this->stmData->GetRealData(ofst, this->pageSize, Data::ByteArray(buff, this->pageSize)) == this->pageSize;
}

Bool DB::MDBFile::ReadTableDef(UInt32 pageNo, TableInfo &table)
{
	UnsafeArray<UInt8> page = MemAllocArr(UInt8, this->pageSize);
	if (!this->ReadPage(pageNo, page) || page[0] != 0x02)
	{
		MemFreeArr(page);
		return false;
	}
	table.pageNo = pageNo;
	table.rowCount = ReadLUInt32(&page[16]);
	table.numVarCols = ReadLUInt16(&page[43]);
	table.numCols = ReadLUInt16(&page[45]);
	table.columnCount = table.numCols;
	UInt32 numRealIdx = ReadLUInt32(&page[51]);
	if (table.numCols > 512 || numRealIdx > 1024)
	{
		MemFreeArr(page);
		return false;
	}
	UnsafeArray<ColumnInfo> tableColumns;
	if (table.columns.SetTo(tableColumns))
		MemFreeArr(tableColumns);
	table.columns = MemAllocArr(ColumnInfo, table.numCols);
	if (!table.columns.SetTo(tableColumns))
	{
		MemFreeArr(page);
		return false;
	}
	UIntOS colOfst = 63 + (UIntOS)numRealIdx * 12;
	if (colOfst > this->pageSize || table.numCols * 25 > this->pageSize - colOfst)
	{
		MemFreeArr(page);
		return false;
	}
	for (UIntOS i = 0; i < table.numCols; ++i)
	{
		const UInt8 *p = &page[colOfst + i * 25];
		ColumnInfo col = {};
		col.type = p[0];
		col.colNum = ReadLUInt16(&p[5]);
		col.varNum = ReadLUInt16(&p[7]);
		col.flags = p[15];
		col.miscFlags = p[16];
		col.fixedOffset = ReadLUInt16(&p[21]);
		col.size = ReadLUInt16(&p[23]);
		tableColumns[i] = col;
	}
	colOfst += (UIntOS)table.numCols * 25;
	for (UIntOS i = 0; i < table.numCols; ++i)
	{
		if (colOfst + 2 > this->pageSize)
		{
			MemFreeArr(page);
			return false;
		}
		UIntOS nameSize = ReadLUInt16(&page[colOfst]);
		colOfst += 2;
		if (nameSize > this->pageSize - colOfst)
		{
			MemFreeArr(page);
			return false;
		}
		ColumnInfo col = tableColumns[i];
		UTF16Char utf16[2048];
		UIntOS utf16Size = 0;
		for (UIntOS j = 0; j + 1 < nameSize && utf16Size < 2048; j += 2)
		{
			UTF16Char c = ReadLUInt16(&page[colOfst + j]);
			if (c == 0)
				break;
			utf16[utf16Size++] = c;
		}
		Text::StringBuilderUTF8 colName;
		colName.AppendUTF16(utf16, utf16Size);
		if (colName.GetLength() >= sizeof(col.name))
		{
			MemFreeArr(page);
			return false;
		}
		MemCopyNO(col.name, colName.ToString().Ptr(), colName.GetLength() + 1);
		tableColumns[i] = col;
		colOfst += nameSize;
	}
	MemFreeArr(page);
	return true;
}

Bool DB::MDBFile::ReadCatalog()
{
	TableInfo catalog;
	if (!this->ReadTableDef(2, catalog))
		return false;
	UnsafeArray<ColumnInfo> catalogColumns;
	if (!catalog.columns.SetTo(catalogColumns))
		return false;
	Text::StrConcat(catalog.name, (const UTF8Char*)"MSysObjects");

	UnsafeArray<UInt8> page = MemAllocArr(UInt8, this->pageSize);
	UInt32 pageCount = (UInt32)(this->stmData->GetDataSize() / this->pageSize);
	for (UInt32 pageNo = 1; pageNo < pageCount; ++pageNo)
	{
		if (!this->ReadPage(pageNo, page) || page[0] != 0x01 || ReadLUInt32(&page[4]) != 2)
			continue;
		catalog.dataPages.Add(pageNo);
	}
	IntOS nameIndex = -1;
	IntOS typeIndex = -1;
	IntOS idIndex = -1;
	IntOS flagsIndex = -1;
	for (UIntOS i = 0; i < catalog.columnCount; ++i)
	{
		ColumnInfo col = catalogColumns[i];
		const UTF8Char *name = col.name;
		if (Text::StrEqualsICase(name, (const UTF8Char*)"Name"))
			nameIndex = (IntOS)i;
		else if (Text::StrEqualsICase(name, (const UTF8Char*)"Type"))
			typeIndex = (IntOS)i;
		else if (Text::StrEqualsICase(name, (const UTF8Char*)"Id"))
			idIndex = (IntOS)i;
		else if (Text::StrEqualsICase(name, (const UTF8Char*)"Flags"))
			flagsIndex = (IntOS)i;
	}
	if (nameIndex < 0 || typeIndex < 0 || idIndex < 0 || flagsIndex < 0)
	{
		MemFreeArr(page);
		return false;
	}

	for (UIntOS p = 0; p < catalog.dataPages.GetCount(); ++p)
	{
		if (!this->ReadPage(catalog.dataPages.GetItem(p), page))
			continue;
		UIntOS rows = ReadLUInt16(&page[12]);
		if (rows > (this->pageSize - 14) / 2)
			continue;
		for (UIntOS row = 0; row < rows; ++row)
		{
			UInt16 rawOfst = ReadLUInt16(&page[14 + row * 2]);
			if (rawOfst & 0xc000)
				continue;
			UIntOS start = rawOfst & 0x1fff;
			UIntOS end = row == 0 ? this->pageSize : (ReadLUInt16(&page[14 + (row - 1) * 2]) & 0x1fff);
			if (start >= end || end > this->pageSize)
				continue;
			DB::MDBUtil::Cell cells[512];
			if (!MDBUtil::ParseRecord(catalog, &page[start], end - start, cells) || (UIntOS)nameIndex >= catalog.columnCount)
				continue;
			UInt32 type = MDBUtil::GetCellUInt32(cells, catalog.columnCount, typeIndex);
			UInt32 flags = MDBUtil::GetCellUInt32(cells, catalog.columnCount, flagsIndex);
			UInt32 defPage = MDBUtil::GetCellUInt32(cells, catalog.columnCount, idIndex) & 0x00ffffff;
			if (type != 1 || (flags & 0x80000002) != 0 || defPage == 0)
				continue;
			UTF8Char name[256];
			Text::StringBuilderUTF8 nameSB;
			MDBUtil::CellToString(catalogColumns[(UIntOS)nameIndex], cells[(UIntOS)nameIndex], NN<Text::StringBuilderUTF8>::FromPtr(&nameSB));
			if (nameSB.GetLength() == 0 || nameSB.GetLength() >= sizeof(name))
				continue;
			MemCopyNO(name, nameSB.ToString().Ptr(), nameSB.GetLength() + 1);
			if (Text::StrEqualsICase(name, (const UTF8Char*)"MSysObjects"))
				continue;
			NN<TableInfo> table;
			NEW_CLASSNN(table, TableInfo());
			if (!this->ReadTableDef(defPage, *table.Ptr()))
			{
				DEL_CLASS(table.Ptr());
				continue;
			}
			Text::StrConcat(table->name, name);
			for (UInt32 dataPage = 1; dataPage < pageCount; ++dataPage)
			{
				if (this->ReadPage(dataPage, page) && page[0] == 0x01 && ReadLUInt32(&page[4]) == defPage)
					table->dataPages.Add(dataPage);
			}
			this->tables.Add(table);
		}
	}
	MemFreeArr(page);
	return true;
}

Optional<DB::MDBFile::TableInfo> DB::MDBFile::FindTable(Text::CStringNN tableName)
{
	for (UIntOS i = 0; i < this->tables.GetCount(); ++i)
	{
		NN<TableInfo> table = this->tables.GetItemNoCheck(i);
		if (Text::StrEqualsICase(table->name, tableName.v))
			return table;
	}
	return nullptr;
}

UIntOS DB::MDBFile::QueryTableNames(Text::CString schemaName, NN<Data::ArrayListStringNN> names)
{
	if (this->error || schemaName.leng != 0)
		return 0;
	for (UIntOS i = 0; i < this->tables.GetCount(); ++i)
	{
		UTF8Char *name = this->tables.GetItemNoCheck(i)->name;
		names->Add(Text::String::New(UnsafeArray<const UTF8Char>(name), Text::StrCharCnt(name)));
	}
	return this->tables.GetCount();
}

Optional<DB::DBReader> DB::MDBFile::QueryTableData(Text::CString schemaName, Text::CStringNN tableName, Optional<Data::ArrayListStringNN> columnNames, UIntOS dataOfst, UIntOS maxCnt, Text::CString ordering, Optional<Data::QueryConditions> condition)
{
	if (this->error || schemaName.leng != 0)
		return nullptr;
	NN<TableInfo> table;
	if (!this->FindTable(tableName).SetTo(table))
		return nullptr;
	NN<Data::QueryConditions> nnCondition;
	if (ordering.leng != 0 || condition.SetTo(nnCondition))
		return NEW_CLASS_D(DB::SortableDBReader(*this, schemaName, tableName, columnNames, dataOfst, maxCnt, ordering, condition));
	NN<DB::MDBFileReader> reader;
	NEW_CLASSNN(reader, DB::MDBFileReader(*this, *table.Ptr(), columnNames, dataOfst, maxCnt));
	return reader;
}

Optional<DB::TableDef> DB::MDBFile::GetTableDef(Text::CString schemaName, Text::CStringNN tableName)
{
	NN<TableInfo> table;
	if (!this->FindTable(tableName).SetTo(table))
		return nullptr;
	NN<DB::TableDef> tableDef;
	NEW_CLASSNN(tableDef, DB::TableDef(schemaName, tableName));
	for (UIntOS i = 0; i < table->columnCount; ++i)
	{
		NN<DB::ColDef> col;
		NEW_CLASSNN(col, DB::ColDef(CSTR("")));
		UnsafeArray<ColumnInfo> tableColumns;
		if (!table->columns.SetTo(tableColumns))
		{
			DEL_CLASS(col.Ptr());
			DEL_CLASS(tableDef.Ptr());
			return nullptr;
		}
		ColumnInfo colInfo = tableColumns[i];
		col->SetColName(colInfo.name);
		col->SetColType(MDBUtil::GetMDBColType(colInfo.type));
		col->SetColSize(colInfo.size);
		col->SetNotNull(false);
		col->SetPK(false);
		col->SetAutoIncNone();
		col->SetDefVal(Text::CString(nullptr));
		col->SetAttr(Text::CString(nullptr));
		tableDef->AddCol(col);
	}
	return tableDef;
}

void DB::MDBFile::CloseReader(NN<DB::DBReader> r)
{
	DB::MDBFileReader *reader = (DB::MDBFileReader*)r.Ptr();
	DEL_CLASS(reader);
}

void DB::MDBFile::GetLastErrorMsg(NN<Text::StringBuilderUTF8> str)
{
	str->AppendC(this->lastError, Text::StrCharCnt(this->lastError));
}

void DB::MDBFile::Reconnect()
{
}

Int8 DB::MDBFile::GetTzQhr() const
{
	return this->tzQhr;
}

void DB::MDBFile::ForceTzQhr(Int8 tzQhr)
{
	this->tzQhr = tzQhr;
}

Bool DB::MDBFile::IsError() const
{
	return this->error;
}
