#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/DateTime.h"
#include "DB/ColDef.h"
#include "DB/CSVFile.h"
#include "DB/DBConn.h"
#include "DB/TableDef.h"
#include "IO/FileStream.h"
#include "IO/Path.h"
#include "IO/StreamDataStream.h"
#include "IO/StreamReader.h"
#include "Math/MSGeography.h"
#include "Text/MyString.h"
#include "Text/MyStringFloat.h"
#include "Text/MyStringW.h"
#include "Text/UTF8Reader.h"

void DB::CSVFile::InitReader(NN<CSVReader> r)
{
	r->SetIndexCol(this->indexCol);
	UOSInt i = this->timeCols.GetCount();
	while (i-- > 0)
	{
		r->AddTimeCol(this->timeCols.GetItem(i));
	}
}

DB::CSVFile::CSVFile(NN<Text::String> fileName, UInt32 codePage) : DB::ReadingDB(fileName), timeCols(4)
{
	this->fileName = fileName->Clone();
	this->stm = 0;
	this->fd = 0;
	this->codePage = codePage;
	this->noHeader = false;
	this->nullIfEmpty = false;
	this->indexCol = INVALID_INDEX;
}

DB::CSVFile::CSVFile(Text::CStringNN fileName, UInt32 codePage) : DB::ReadingDB(fileName), timeCols(4)
{
	this->fileName = Text::String::New(fileName);
	this->stm = 0;
	this->fd = 0;
	this->codePage = codePage;
	this->noHeader = false;
	this->nullIfEmpty = false;
	this->indexCol = INVALID_INDEX;
}

DB::CSVFile::CSVFile(NN<IO::SeekableStream> stm, UInt32 codePage) : DB::ReadingDB(stm->GetSourceNameObj()), timeCols(4)
{
	this->fileName = stm->GetSourceNameObj()->Clone();
	this->stm = stm;
	this->fd = 0;
	this->codePage = codePage;
	this->noHeader = false;
	this->nullIfEmpty = false;
	this->indexCol = INVALID_INDEX;
}

DB::CSVFile::CSVFile(NN<IO::StreamData> fd, UInt32 codePage) : DB::ReadingDB(fd->GetFullName()), timeCols(4)
{
	this->fileName = fd->GetFullName()->Clone();
	this->fd = fd->GetPartialData(0, fd->GetDataSize());
	this->stm = 0;
	this->codePage = codePage;
	this->noHeader = false;
	this->nullIfEmpty = false;
	this->indexCol = INVALID_INDEX;
}

DB::CSVFile::~CSVFile()
{
	this->fileName->Release();
	this->fd.Delete();
}

UOSInt DB::CSVFile::QueryTableNames(Text::CString schemaName, NN<Data::ArrayListStringNN> names)
{
	if (schemaName.leng != 0)
		return 0;
	names->Add(Text::String::New(UTF8STRC("CSVFile")));
	return 1;
}

Optional<DB::DBReader> DB::CSVFile::QueryTableData(Text::CString schemaName, Text::CStringNN tableName, Optional<Data::ArrayListStringNN> columnName, UOSInt ofst, UOSInt maxCnt, Text::CString ordering, Optional<Data::QueryConditions> condition)
{
	NN<IO::StreamData> fd;
	NN<IO::SeekableStream> stm;
	NN<IO::Reader> rdr;
	NN<DB::CSVReader> r;
	if (this->fd.SetTo(fd))
	{
		NEW_CLASSNN(stm, IO::StreamDataStream(fd));
		if (codePage == 65001)
		{
			NEW_CLASSNN(rdr, Text::UTF8Reader(stm));
		}
		else
		{
			NEW_CLASSNN(rdr, IO::StreamReader(stm, codePage));
		}
		NEW_CLASSNN(r, DB::CSVReader(stm, rdr, this->noHeader, this->nullIfEmpty, condition));
		this->InitReader(r);
		return r;
	}
	if (this->stm.SetTo(stm))
	{
		stm->SeekFromBeginning(0);
		if (codePage == 65001)
		{
			NEW_CLASSNN(rdr, Text::UTF8Reader(stm));
		}
		else
		{
			NEW_CLASSNN(rdr, IO::StreamReader(stm, codePage));
		}
		NEW_CLASSNN(r, DB::CSVReader(0, rdr, this->noHeader, this->nullIfEmpty, condition));
		this->InitReader(r);
		return r;
	}
	NN<IO::FileStream> fs;
	NEW_CLASSNN(fs, IO::FileStream(this->fileName, IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Sequential));
	if (!fs->IsError())
	{
		if (codePage == 65001)
		{
			NEW_CLASSNN(rdr, Text::UTF8Reader(fs));
		}
		else
		{
			NEW_CLASSNN(rdr, IO::StreamReader(fs, codePage));
		}
		NEW_CLASSNN(r, DB::CSVReader(fs, rdr, this->noHeader, this->nullIfEmpty, condition));
		this->InitReader(r);
		return r;
	}
	else
	{
		fs.Delete();
		return 0;
	}
}

Optional<DB::TableDef> DB::CSVFile::GetTableDef(Text::CString schemaName, Text::CStringNN tableName)
{
	NN<DB::DBReader> r;
	if (this->QueryTableData(schemaName, tableName, 0, 0, 0, CSTR_NULL, 0).SetTo(r))
	{
		DB::TableDef *tab;
		NN<DB::ColDef> col;
		NEW_CLASS(tab, DB::TableDef(schemaName, tableName));
		UOSInt i = 0;
		UOSInt j = r->ColCount();
		while (i < j)
		{
			NEW_CLASSNN(col, DB::ColDef(Text::String::NewEmpty()));
			r->GetColDef(i, col);
			tab->AddCol(col);
			i++;
		}
		this->CloseReader(r);
		return tab;
	}
	return 0;
}

void DB::CSVFile::CloseReader(NN<DBReader> r)
{
	DB::CSVReader *rdr = (DB::CSVReader *)r.Ptr();
	DEL_CLASS(rdr);
}

void DB::CSVFile::GetLastErrorMsg(NN<Text::StringBuilderUTF8> str)
{
}

void DB::CSVFile::Reconnect()
{
}

void DB::CSVFile::SetNoHeader(Bool noHeader)
{
	this->noHeader = noHeader;
}

void DB::CSVFile::SetNullIfEmpty(Bool nullIfEmpty)
{
	this->nullIfEmpty = nullIfEmpty;
}

void DB::CSVFile::SetIndexCol(UOSInt indexCol)
{
	this->indexCol = indexCol;
}

void DB::CSVFile::SetTimeCols(Data::DataArray<UOSInt> timeCols)
{
	UOSInt i = 0;
	UOSInt j = timeCols.GetCount();
	this->timeCols.Clear();
	while (i < j)
	{
		this->timeCols.Add(timeCols[i]);
		i++;
	}
}

Optional<Data::TableData> DB::CSVFile::LoadAsTableData(Text::CStringNN fileName, UInt32 codePage, UOSInt indexCol, Data::DataArray<UOSInt> timeCols)
{
	if (IO::Path::GetPathType(fileName) != IO::Path::PathType::File)
		return 0;
	NN<DB::CSVFile> csv;
	NEW_CLASSNN(csv, DB::CSVFile(fileName, codePage));
	csv->SetIndexCol(indexCol);
	csv->SetTimeCols(timeCols);
	NN<Data::TableData> data;
	NEW_CLASSNN(data, Data::TableData(csv, true, 0, CSTR("")));
	return data;
}

DB::CSVReader::CSVReader(Optional<IO::Stream> stm, NN<IO::Reader> rdr, Bool noHeader, Bool nullIfEmpty, Optional<Data::QueryConditions> condition)
{
	this->stm = stm;
	this->rdr = rdr;
	this->noHeader = noHeader;
	this->nullIfEmpty = nullIfEmpty;
	this->nCol = 0;
	this->rowBuffSize = 16384;
	this->indexCol = INVALID_INDEX;
	this->row = MemAllocArr(UTF8Char, this->rowBuffSize);
	this->cols = MemAllocArr(CSVColumn, 128);
	this->hdrs = MemAllocArr(Text::PString, 128);
	this->condition = condition;

	UnsafeArray<UTF8Char> sptr;
	UnsafeArray<UTF8Char> currPtr;
	WChar c;
	Bool colStart = true;
	Bool quote = false;
	currPtr = this->row;
	if (this->rdr->ReadLine(this->row, this->rowBuffSize - 1).SetTo(sptr))
	{
		while (true)
		{
			c = *currPtr++;
			if (c == 0)
			{
				currPtr--;
				if (quote)
				{
					sptr = this->rdr->GetLastLineBreak(currPtr);
					currPtr = sptr;
					if (!this->rdr->ReadLine(sptr, this->rowBuffSize - (UOSInt)(sptr - this->row) - 1).SetTo(sptr))
						break;
					if (!this->rdr->IsLineBreak() && (UOSInt)(sptr - this->row) > this->rowBuffSize - 6)
					{
						UnsafeArray<UTF8Char> newRow = MemAllocArr(UTF8Char, this->rowBuffSize << 1);
						MemCopyNO(newRow.Ptr(), this->row.Ptr(), this->rowBuffSize);
						this->rowBuffSize <<= 1;
						sptr = &newRow[(sptr - this->row)];
						currPtr = &newRow[(currPtr - this->row)];
						MemFreeArr(this->row);
						this->row = newRow;
					}
				}
				else
				{
					break;
				}
			}
			else if (c == '"')
			{
				if (colStart && !quote)
				{
					quote = true;
					colStart = false;
				}
				else if (quote)
				{
					if (*currPtr == '"')
					{
						currPtr++;
					}
					else
					{
						quote = false;
					}
				}
				else
				{
				}
			}
			else if ((c == ',') && (quote == 0))
			{
				colStart = true;
			}
			else
			{
				colStart = false;
			}
		}
	}

	this->hdr = MemAllocArr(UTF8Char, (UOSInt)(currPtr - this->row + 1));
	MemCopyNO(this->hdr.Ptr(), this->row.Ptr(), sizeof(UTF8Char) * (UOSInt)(currPtr - this->row + 1));
	this->nHdr = Text::StrCSVSplitP(this->hdrs, 128, this->hdr);
	this->nCol = this->nHdr;
	UOSInt i = this->nCol;
	while (i-- > 0)
	{
		this->cols[i].colSize = this->hdrs[i].leng;
	}
	i = 128;
	while (i-- > 0)
	{
		this->cols[i].colType = DB::DBUtil::ColType::CT_VarUTF8Char;
	}
}

DB::CSVReader::~CSVReader()
{
	this->rdr.Delete();
	this->stm.Delete();
	MemFreeArr(this->row);
	MemFreeArr(this->cols);
	MemFreeArr(this->hdr);
	MemFreeArr(this->hdrs);
}

Bool DB::CSVReader::ReadNext()
{
	UnsafeArray<UTF8Char> sptr;
	UnsafeArray<UTF8Char> currPtr;
	WChar c;
	Bool colStart;
	Int32 quote = 0;

	UOSInt nCol;
	if (this->noHeader)
	{
		this->noHeader = false;
		nCol = this->nHdr;
		this->nCol = nCol;
		while (nCol-- > 0)
		{
			this->cols[nCol].value = this->hdrs[nCol].v;
		}
		return true;
	}
	while (true)
	{
		while (true)
		{
			if (!this->rdr->ReadLine(this->row, this->rowBuffSize - 1).SetTo(sptr))
			{
				this->nCol = 0;
				return false;
			}
			else if (sptr != this->row)
			{
				break;
			}
		}

		nCol = 1;
		cols[0].value = this->row;

		currPtr = this->row;
		colStart = true;
		while (true)
		{
			c = *currPtr;
			if (c == 0)
			{
				if (quote)
				{
					sptr = this->rdr->GetLastLineBreak(currPtr);
					if (!this->rdr->ReadLine(sptr, this->rowBuffSize - (UOSInt)(sptr - this->row) - 1).SetTo(sptr))
						break;
					if (!this->rdr->IsLineBreak() && (UOSInt)(sptr - this->row) > this->rowBuffSize - 6)
					{
						UnsafeArray<UTF8Char> newRow = MemAllocArr(UTF8Char, this->rowBuffSize << 1);
						MemCopyNO(newRow.Ptr(), this->row.Ptr(), this->rowBuffSize);
						this->rowBuffSize <<= 1;
						sptr = &newRow[(sptr - this->row)];
						currPtr = &newRow[(currPtr - this->row)];
						UOSInt i = nCol;
						while (i-- > 0)
						{
							cols[i].value = &newRow[(cols[i].value - &this->row[0])];
						}
						MemFreeArr(this->row);
						this->row = newRow;
					}
				}
				else
				{
					this->cols[nCol - 1].colSize = (UOSInt)(currPtr - this->cols[nCol - 1].value);
					break;
				}
			}
			else if (c == '"')
			{
				if (quote)
				{
					if (currPtr[1] == '"')
					{
						currPtr += 2;
					}
					else
					{
						quote = false;
						currPtr++;
					}
				}
				else if (colStart)
				{
					quote = true;
					colStart = false;
					currPtr++;
				}
				else
				{
					currPtr++;
				}
			}
			else if ((c == ',') && (quote == 0))
			{
				this->cols[nCol - 1].colSize = (UOSInt)(currPtr - this->cols[nCol - 1].value);
				*currPtr++ = 0;
				this->cols[nCol++].value = currPtr;
				colStart = true;
			}
			else
			{
				colStart = false;
				currPtr++;
			}
		}
		NN<Data::QueryConditions> nncondition;
		Bool valid;
		if (!this->condition.SetTo(nncondition) || !nncondition->IsValid(*this, valid) || valid)
		{
			this->nCol = nCol;
			return true;
		}
	}
}

UOSInt DB::CSVReader::ColCount()
{
	if (this->nHdr > this->nCol)
		return this->nHdr;
	return this->nCol;
}

OSInt DB::CSVReader::GetRowChanged()
{
	return -1;
}

Int32 DB::CSVReader::GetInt32(UOSInt colIndex)
{
	UTF8Char buff[60];
	this->GetStr(colIndex, buff, sizeof(buff));
	Text::StrTrim(buff);
	return Text::StrToInt32(buff);
}

Int64 DB::CSVReader::GetInt64(UOSInt colIndex)
{
	UTF8Char buff[60];
	this->GetStr(colIndex, buff, sizeof(buff));
	Text::StrTrim(buff);
	return Text::StrToInt64(buff);
}

UnsafeArrayOpt<WChar> DB::CSVReader::GetStr(UOSInt colIndex, UnsafeArray<WChar> buff)
{
	if (colIndex >= nCol)
		return 0;

	if (nullIfEmpty)
	{
		if (cols[colIndex].value[0] == 0 || cols[colIndex].value[0] == ',' || Text::StrStartsWith(cols[colIndex].value, (const UTF8Char*)"\"\",") || Text::StrEquals(cols[colIndex].value, (const UTF8Char*)"\"\""))
		{
			return 0;
		}
	}
	UnsafeArray<const WChar> csptr = Text::StrToWCharNew(cols[colIndex].value);
	UnsafeArray<const WChar> ptr = csptr;
	WChar c;
	Int32 quote = 0;
	c = *ptr;
	if (c == '"')
	{
		while (true)
		{
			c = *ptr++;
			if (c == 0)
				break;
			if (c == '"')
			{
				if (quote)
				{
					if (*ptr == '"')
					{
						*buff++ = '"';
						ptr++;
					}
					else
					{
						quote = 0;
					}
				}
				else
				{
					quote = 1;
				}
			}
			else if ((c == ',') && (quote == 0))
			{
				break;
			}
			else
			{
				*buff++ = c;
			}
		}
		*buff = 0;
		Text::StrDelNew(csptr);
		return buff;
	}
	else
	{
		while (true)
		{
			c = *ptr++;
			if (c == 0 || c == ',')
				break;
			*buff++ = c;
		}
		*buff = 0;
		Text::StrDelNew(csptr);
		return buff;
	}
}

Bool DB::CSVReader::GetStr(UOSInt colIndex, NN<Text::StringBuilderUTF8> sb)
{
	if (colIndex >= nCol)
		return false;
	if (nullIfEmpty)
	{
		if (cols[colIndex].value[0] == 0 || cols[colIndex].value[0] == ',' || Text::StrStartsWith(cols[colIndex].value, (const UTF8Char*)"\"\",") || Text::StrEquals(cols[colIndex].value, (const UTF8Char*)"\"\""))
		{
			return 0;
		}
	}
	UnsafeArray<const WChar> csptr = Text::StrToWCharNew(cols[colIndex].value);
	UnsafeArray<const WChar> ptr = csptr;
	WChar c;
	Int32 quote = 0;
	c = *ptr;
	if (c == '"')
	{
		while (true)
		{
			c = *ptr++;
			if (c == 0)
				break;
			if (c == '"')
			{
				if (quote)
				{
					if (*ptr == '"')
					{
						sb->AppendUTF8Char('"');
						ptr++;
					}
					else
					{
						quote = 0;
					}
				}
				else
				{
					quote = 1;
				}
			}
			else if ((c == ',') && (quote == 0))
			{
				break;
			}
			else
			{
				sb->AppendChar(c, 1);
			}
		}
		Text::StrDelNew(csptr);
		return true;
	}
	else
	{
		while (true)
		{
			c = *ptr++;
			if (c == 0 || c == ',')
				break;
			sb->AppendChar(c, 1);
		}
		Text::StrDelNew(csptr);
		return true;
	}
}

Optional<Text::String> DB::CSVReader::GetNewStr(UOSInt colIndex)
{
	if (colIndex >= nCol)
		return 0;
	if (nullIfEmpty)
	{
		if (cols[colIndex].value[0] == 0 || cols[colIndex].value[0] == ',' || Text::StrStartsWith(cols[colIndex].value, (const UTF8Char*)"\"\",") || Text::StrEquals(cols[colIndex].value, (const UTF8Char*)"\"\""))
		{
			return 0;
		}
	}
	NN<Text::String> s;
	UOSInt len = 0;
	UnsafeArray<const UTF8Char> csptr = cols[colIndex].value;
	UnsafeArray<const UTF8Char> ptr = csptr;
	UTF8Char c;
	Int32 quote = 0;
	c = *ptr;
	if (c == '"')
	{
		while (true)
		{
			c = *ptr++;
			if (c == 0)
				break;
			if (c == '"')
			{
				if (quote)
				{
					if (*ptr == '"')
					{
						len++;
						ptr++;
					}
					else
					{
						quote = 0;
					}
				}
				else
				{
					quote = 1;
				}
			}
			else if ((c == ',') && (quote == 0))
			{
				break;
			}
			else
			{
				len++;
			}
		}
		s = Text::String::New(len);
		UnsafeArray<UTF8Char> buff = s->v;
		ptr = csptr;
		quote = 0;
		while (true)
		{
			c = *ptr++;
			if (c == 0)
				break;
			if (c == '"')
			{
				if (quote)
				{
					if (*ptr == '"')
					{
						*buff++ = '"';
						ptr++;
					}
					else
					{
						quote = 0;
					}
				}
				else
				{
					quote = 1;
				}
			}
			else if ((c == ',') && (quote == 0))
			{
				break;
			}
			else
			{
				*buff++ = c;
			}
		}

		*buff = 0;
		return s;
	}
	else
	{
		while (true)
		{
			c = *ptr++;
			if (c == 0 || c == ',')
				break;
			len++;
		}
		s = Text::String::New(len);
		UnsafeArray<UTF8Char> buff = s->v;
		ptr = csptr;
		while (true)
		{
			c = *ptr++;
			if (c == 0 || c == ',')
				break;
			*buff++ = c;
		}

		*buff = 0;
		return s;
	}
}

UnsafeArrayOpt<UTF8Char> DB::CSVReader::GetStr(UOSInt colIndex, UnsafeArray<UTF8Char> buff, UOSInt buffSize)
{
	if (colIndex >= nCol)
		return 0;
	if (nullIfEmpty)
	{
		if (cols[colIndex].value[0] == 0 || cols[colIndex].value[0] == ',' || Text::StrStartsWith(cols[colIndex].value, (const UTF8Char*)"\"\",") || Text::StrEquals(cols[colIndex].value, (const UTF8Char*)"\"\""))
		{
			return 0;
		}
	}
	UnsafeArray<const UTF8Char> ptr = cols[colIndex].value;
	UTF8Char c;
	Int32 quote = 0;
	c = *ptr;
	if (c == '"')
	{
		while (true)
		{
			if (buffSize <= 1)
			{
				break;
			}
			c = *ptr++;
			if (c == 0)
				break;
			if (c == '"')
			{
				if (quote)
				{
					if (*ptr == '"')
					{
						*buff++ = c;
						buffSize--;
						ptr++;
					}
					else
					{
						quote = 0;
					}
				}
				else
				{
					quote = 1;
				}
			}
			else if ((c == ',') && (quote == 0))
			{
				break;
			}
			else
			{
				*buff++ = c;
				buffSize--;
			}
		}
		*buff = 0;
		return buff;
	}
	else
	{
		while (true)
		{
			if (buffSize <= 1)
			{
				break;
			}
			c = *ptr++;
			if (c == 0 || c == ',')
				break;
			*buff++ = c;
			buffSize--;
		}
		*buff = 0;
		return buff;
	}
}

Data::Timestamp DB::CSVReader::GetTimestamp(UOSInt colIndex)
{
	UTF8Char buff[60];
	UnsafeArray<UTF8Char> sptr;
	if (this->GetStr(colIndex, buff, sizeof(buff)).SetTo(sptr))
		return Data::Timestamp(CSTRP(buff, sptr), Data::DateTimeUtil::GetLocalTzQhr());
	return 0;
}

Double DB::CSVReader::GetDblOrNAN(UOSInt colIndex)
{
	UTF8Char buff[60];
	buff[0] = 0;
	this->GetStr(colIndex, buff, sizeof(buff));
	Text::StrTrim(buff);
	return Text::StrToDoubleOrNAN(buff);
}

Bool DB::CSVReader::GetBool(UOSInt colIndex)
{
	UTF8Char buff[20];
	UnsafeArray<UTF8Char> sptr;
	if (!this->GetStr(colIndex, buff, sizeof(buff)).SetTo(sptr))
		return false;
	if (Text::StrEqualsICaseC(buff, (UOSInt)(sptr - buff), UTF8STRC("TRUE")))
		return true;
	return Text::StrToInt32(buff) != 0;
}

UOSInt DB::CSVReader::GetBinarySize(UOSInt colIndex)
{
	return 0;
}

UOSInt DB::CSVReader::GetBinary(UOSInt colIndex, UnsafeArray<UInt8> buff)
{
	return 0;
}

Optional<Math::Geometry::Vector2D> DB::CSVReader::GetVector(UOSInt colIndex)
{
	if (colIndex >= nCol)
		return 0;
	if (nullIfEmpty)
	{
		if (cols[colIndex].value[0] == 0 || cols[colIndex].value[0] == ',' || Text::StrStartsWith(cols[colIndex].value, (const UTF8Char*)"\"\",") || Text::StrEquals(cols[colIndex].value, (const UTF8Char*)"\"\""))
		{
			return 0;
		}
	}
	UnsafeArray<const UTF8Char> ptr = cols[colIndex].value;
	if (Text::StrStartsWith(ptr, (const UTF8Char*)"0x"))
	{
		Text::StringBuilderUTF8 sb;
		UInt8 v = 0;
		UTF8Char c;
		ptr += 2;
		while (true)
		{
			c = ptr[0];
			if (c >= '0' && c <= '9')
			{
				v = (UInt8)(c - 0x30);
			}
			else if (c >= 'A' && c <= 'F')
			{
				v = (UInt8)(c - 0x37);
			}
			else if (c >= 'a' && c <= 'f')
			{
				v = (UInt8)(c - 0x57);
			}
			else
			{
				break;
			}
			c = ptr[1];
			if (c >= '0' && c <= '9')
			{
				v = (UInt8)((v << 4) + c - 0x30);
			}
			else if (c >= 'A' && c <= 'F')
			{
				v = (UInt8)((v << 4) + c - 0x37);
			}
			else if (c >= 'a' && c <= 'f')
			{
				v = (UInt8)((v << 4) + c - 0x57);
			}
			else
			{
				return 0;
			}
			sb.AppendUTF8Char(v);
			ptr += 2;
		}
		return Math::MSGeography::ParseBinary(sb.ToPtr(), sb.GetLength(), 0);
	}
	return 0;
}

Bool DB::CSVReader::GetUUID(UOSInt colIndex, NN<Data::UUID> uuid)
{
	return false;
}

Bool DB::CSVReader::GetVariItem(UOSInt colIndex, NN<Data::VariItem> item)
{
	if (colIndex >= nCol)
	{
		item->SetNull();
		return false;
	}
	if (nullIfEmpty)
	{
		if (cols[colIndex].value[0] == 0 || (*(UInt16*)cols[colIndex].value.Ptr() == 0x2222 && cols[colIndex].value[2] == 0))
		{
			item->SetNull();
			return true;
		}
	}
	UnsafeArray<const UTF8Char> ptr = cols[colIndex].value;
	UTF8Char c;
	c = *ptr;
	if (c == '"')
	{
		NN<Text::String> s = Text::String::New(this->cols[colIndex].colSize);
		UnsafeArray<UTF8Char> buff = s->v;
		Int32 quote = 0;
		while (true)
		{
			c = *ptr++;
			if (c == 0)
				break;
			if (c == '"')
			{
				if (quote)
				{
					if (*ptr == '"')
					{
						*buff++ = c;
						ptr++;
					}
					else
					{
						quote = 0;
					}
				}
				else
				{
					quote = 1;
				}
			}
			else
			{
				*buff++ = c;
			}
		}
		*buff = 0;
		s->leng = (UOSInt)(buff - s->v);
		if (cols[colIndex].colType == DB::DBUtil::CT_DateTimeTZ)
		{
			item->SetDate(Data::Timestamp::FromStr(s->ToCString(), Data::DateTimeUtil::GetLocalTzQhr()));
		}
		else
		{
			item->SetStr(s);
		}
		s->Release();
		return true;
	}
	else
	{
		if (cols[colIndex].colType == DB::DBUtil::CT_DateTimeTZ)
		{
			item->SetDate(Data::Timestamp::FromStr(Text::CStringNN::FromPtr(ptr), Data::DateTimeUtil::GetLocalTzQhr()));
		}
		else
		{
			item->SetStrSlow(ptr);
		}
		return true;
	}	
}

UnsafeArrayOpt<UTF8Char> DB::CSVReader::GetName(UOSInt colIndex, UnsafeArray<UTF8Char> buff)
{
	if (colIndex >= this->nHdr)
		return 0;
	return this->hdrs[colIndex].ConcatTo(buff);
}

Bool DB::CSVReader::IsNull(UOSInt colIndex)
{
	if (colIndex >= nCol)
		return true;
	return false;
}

DB::DBUtil::ColType DB::CSVReader::GetColType(UOSInt colIndex, OptOut<UOSInt> colSize)
{
	if (colIndex >= nHdr)
		return DB::DBUtil::CT_Unknown;
	colSize.Set(0xffffffff);
	return cols[colIndex].colType;
}

Bool DB::CSVReader::GetColDef(UOSInt colIndex, NN<DB::ColDef> colDef)
{
	if (colIndex >= nHdr)
		return false;
	colDef->SetColName(this->hdrs[colIndex].ToCString());
	colDef->SetColType(cols[colIndex].colType);
	colDef->SetColSize(256);
	colDef->SetColDP(0);
	colDef->SetNotNull(true);
	colDef->SetPK(colIndex == this->indexCol);
	colDef->SetAutoInc(DB::ColDef::AutoIncType::None, 1, 1);
	colDef->SetDefVal(CSTR_NULL);
	colDef->SetAttr(CSTR_NULL);
	return true;
}

NN<Data::VariItem> DB::CSVReader::GetNewItem(Text::CStringNN name)
{
	UOSInt i = this->nHdr;
	while (i-- > 0)
	{
		if (this->hdrs[i].Equals(name))
		{
			if (i >= this->nCol)
			{
				return Data::VariItem::NewUnknown();
			}
			else
			{
				return Data::VariItem::NewStrSlow(UnsafeArray<const UTF8Char>(this->cols[i].value));
			}
		}
	}
	return Data::VariItem::NewUnknown();
}

void DB::CSVReader::SetIndexCol(UOSInt indexCol)
{
	this->indexCol = indexCol;
}

void DB::CSVReader::AddTimeCol(UOSInt timeCol)
{
	if (timeCol < 128)
	{
		this->cols[timeCol].colType = DB::DBUtil::CT_DateTimeTZ;
	}
}