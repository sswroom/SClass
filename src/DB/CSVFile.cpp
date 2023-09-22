#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/DateTime.h"
#include "DB/ColDef.h"
#include "DB/CSVFile.h"
#include "DB/DBConn.h"
#include "DB/TableDef.h"
#include "IO/FileStream.h"
#include "IO/StreamDataStream.h"
#include "IO/StreamReader.h"
#include "Math/MSGeography.h"
#include "Text/MyString.h"
#include "Text/MyStringFloat.h"
#include "Text/MyStringW.h"
#include "Text/UTF8Reader.h"

DB::CSVFile::CSVFile(NotNullPtr<Text::String> fileName, UInt32 codePage) : DB::ReadingDB(fileName)
{
	this->fileName = fileName->Clone();
	this->stm = 0;
	this->releaseStm = false;
	this->codePage = codePage;
	this->noHeader = false;
	this->nullIfEmpty = false;
}

DB::CSVFile::CSVFile(Text::CStringNN fileName, UInt32 codePage) : DB::ReadingDB(fileName)
{
	this->fileName = Text::String::New(fileName);
	this->stm = 0;
	this->releaseStm = false;
	this->codePage = codePage;
	this->noHeader = false;
	this->nullIfEmpty = false;
}

DB::CSVFile::CSVFile(NotNullPtr<IO::SeekableStream> stm, UInt32 codePage) : DB::ReadingDB(stm->GetSourceNameObj())
{
	this->fileName = stm->GetSourceNameObj()->Clone();
	this->stm = stm.Ptr();
	this->releaseStm = false;
	this->codePage = codePage;
	this->noHeader = false;
	this->nullIfEmpty = false;
}

DB::CSVFile::CSVFile(NotNullPtr<IO::StreamData> fd, UInt32 codePage) : DB::ReadingDB(fd->GetFullName())
{
	this->fileName = fd->GetFullName()->Clone();
	NEW_CLASS(this->stm, IO::StreamDataStream(fd));
	this->releaseStm = true;
	this->codePage = codePage;
	this->noHeader = false;
	this->nullIfEmpty = false;
}

DB::CSVFile::~CSVFile()
{
	this->fileName->Release();
	if (this->stm && this->releaseStm)
	{
		DEL_CLASS(this->stm);
	}
}

UOSInt DB::CSVFile::QueryTableNames(Text::CString schemaName, NotNullPtr<Data::ArrayListNN<Text::String>> names)
{
	if (schemaName.leng != 0)
		return 0;
	names->Add(Text::String::New(UTF8STRC("CSVFile")));
	return 1;
}

DB::DBReader *DB::CSVFile::QueryTableData(Text::CString schemaName, Text::CString tableName, Data::ArrayListNN<Text::String> *columnName, UOSInt ofst, UOSInt maxCnt, Text::CString ordering, Data::QueryConditions *condition)
{
	NotNullPtr<IO::Stream> stm;
	if (stm.Set(this->stm))
	{
		IO::Reader *rdr;
		DB::CSVReader *r;
		this->stm->SeekFromBeginning(0);
		if (codePage == 65001)
		{
			NEW_CLASS(rdr, Text::UTF8Reader(stm));
		}
		else
		{
			NEW_CLASS(rdr, IO::StreamReader(stm, codePage));
		}
		NEW_CLASS(r, DB::CSVReader(0, rdr, this->noHeader, this->nullIfEmpty));
		return r;
	}
	IO::Reader *rdr;
	DB::CSVReader *r;
	NotNullPtr<IO::FileStream> fs;
	NEW_CLASSNN(fs, IO::FileStream(this->fileName, IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Sequential));
	if (!fs->IsError())
	{
		if (codePage == 65001)
		{
			NEW_CLASS(rdr, Text::UTF8Reader(fs));
		}
		else
		{
			NEW_CLASS(rdr, IO::StreamReader(fs, codePage));
		}
		NEW_CLASS(r, DB::CSVReader(fs.Ptr(), rdr, this->noHeader, this->nullIfEmpty));
		return r;
	}
	else
	{
		fs.Delete();
		return 0;
	}
}

DB::TableDef *DB::CSVFile::GetTableDef(Text::CString schemaName, Text::CString tableName)
{
	DB::DBReader *r = this->QueryTableData(schemaName, tableName, 0, 0, 0, CSTR_NULL, 0);
	if (r)
	{
		DB::TableDef *tab;
		NotNullPtr<DB::ColDef> col;
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
		return tab;
	}
	return 0;
}

void DB::CSVFile::CloseReader(NotNullPtr<DBReader> r)
{
	DB::CSVReader *rdr = (DB::CSVReader *)r.Ptr();
	DEL_CLASS(rdr);
}

void DB::CSVFile::GetLastErrorMsg(NotNullPtr<Text::StringBuilderUTF8> str)
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

DB::CSVReader::CSVReader(IO::Stream *stm, IO::Reader *rdr, Bool noHeader, Bool nullIfEmpty)
{
	this->stm = stm;
	this->rdr = rdr;
	this->noHeader = noHeader;
	this->nullIfEmpty = nullIfEmpty;
	this->nCol = 0;
	this->rowBuffSize = 16384;
	this->row = MemAlloc(UTF8Char, this->rowBuffSize);
	this->cols = MemAlloc(UTF8Char*, 128);
	this->colSize = MemAlloc(UOSInt, 128);
	this->hdrs = MemAlloc(Text::PString, 128);

	UTF8Char *sptr;
	UTF8Char *currPtr;
	WChar c;
	Bool colStart = true;
	Bool quote = false;
	sptr = this->rdr->ReadLine(this->row, this->rowBuffSize - 1);
	currPtr = this->row;
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
				sptr = this->rdr->ReadLine(sptr, this->rowBuffSize - (UOSInt)(sptr - this->row) - 1);
				if (sptr == 0)
					break;
				if (!this->rdr->IsLineBreak() && (UOSInt)(sptr - this->row) > this->rowBuffSize - 6)
				{
					UTF8Char *newRow = MemAlloc(UTF8Char, this->rowBuffSize << 1);
					MemCopyNO(newRow, this->row, this->rowBuffSize);
					this->rowBuffSize <<= 1;
					sptr = &newRow[(sptr - this->row)];
					currPtr = &newRow[(currPtr - this->row)];
					MemFree(this->row);
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

	this->hdr = MemAlloc(UTF8Char, (UOSInt)(currPtr - this->row + 1));
	MemCopyNO(this->hdr, this->row, sizeof(UTF8Char) * (UOSInt)(currPtr - this->row + 1));
	this->nHdr = Text::StrCSVSplitP(this->hdrs, 128, this->hdr);
	this->nCol = this->nHdr;
	UOSInt i = this->nCol;
	while (i-- > 0)
	{
		this->colSize[i] = this->hdrs[i].leng;
	}
}

DB::CSVReader::~CSVReader()
{
	DEL_CLASS(this->rdr);
	SDEL_CLASS(this->stm);
	MemFree(this->row);
	MemFree(this->cols);
	MemFree(this->colSize);
	MemFree(this->hdr);
	MemFree(this->hdrs);
}

Bool DB::CSVReader::ReadNext()
{
	UTF8Char *sptr;
	UTF8Char *currPtr;
	WChar c;
	Bool colStart;
	Int32 quote = 0;

	UOSInt nCol;
	if (this->rdr == 0)
	{
		this->nCol = 0;
		return false;
	}
	if (this->noHeader)
	{
		this->noHeader = false;
		nCol = this->nHdr;
		this->nCol = nCol;
		while (nCol-- > 0)
		{
			this->cols[nCol] = this->hdrs[nCol].v;
		}
		return true;
	}

	while (true)
	{
		sptr = this->rdr->ReadLine(this->row, this->rowBuffSize - 1);
		if (sptr == 0)
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
	cols[0] = this->row;

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
				sptr = this->rdr->ReadLine(sptr, this->rowBuffSize - (UOSInt)(sptr - this->row) - 1);
				if (sptr == 0)
					break;
				if (!this->rdr->IsLineBreak() && (UOSInt)(sptr - this->row) > this->rowBuffSize - 6)
				{
					UTF8Char *newRow = MemAlloc(UTF8Char, this->rowBuffSize << 1);
					MemCopyNO(newRow, this->row, this->rowBuffSize);
					this->rowBuffSize <<= 1;
					sptr = &newRow[(sptr - this->row)];
					currPtr = &newRow[(currPtr - this->row)];
					UOSInt i = nCol;
					while (i-- > 0)
					{
						cols[i] = &newRow[(cols[i] - this->row)];
					}
					MemFree(this->row);
					this->row = newRow;
				}
			}
			else
			{
				this->colSize[nCol - 1] = (UOSInt)(currPtr - cols[nCol - 1]);
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
			this->colSize[nCol - 1] = (UOSInt)(currPtr - cols[nCol - 1]);
			*currPtr++ = 0;
			cols[nCol++] = currPtr;
			colStart = true;
		}
		else
		{
			colStart = false;
			currPtr++;
		}
	}
	this->nCol = nCol;
	return true;
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

WChar *DB::CSVReader::GetStr(UOSInt colIndex, WChar *buff)
{
	if (colIndex >= nCol)
		return 0;

	if (nullIfEmpty)
	{
		if (cols[colIndex][0] == 0 || cols[colIndex][0] == ',' || Text::StrStartsWith(cols[colIndex], (const UTF8Char*)"\"\",") || Text::StrEquals(cols[colIndex], (const UTF8Char*)"\"\""))
		{
			return 0;
		}
	}
	const WChar *csptr = Text::StrToWCharNew(cols[colIndex]);
	const WChar *ptr = csptr;
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

Bool DB::CSVReader::GetStr(UOSInt colIndex, NotNullPtr<Text::StringBuilderUTF8> sb)
{
	if (colIndex >= nCol)
		return false;
	if (nullIfEmpty)
	{
		if (cols[colIndex][0] == 0 || cols[colIndex][0] == ',' || Text::StrStartsWith(cols[colIndex], (const UTF8Char*)"\"\",") || Text::StrEquals(cols[colIndex], (const UTF8Char*)"\"\""))
		{
			return 0;
		}
	}
	const WChar *csptr = Text::StrToWCharNew(cols[colIndex]);
	const WChar *ptr = csptr;
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

Text::String *DB::CSVReader::GetNewStr(UOSInt colIndex)
{
	if (colIndex >= nCol)
		return 0;
	if (nullIfEmpty)
	{
		if (cols[colIndex][0] == 0 || cols[colIndex][0] == ',' || Text::StrStartsWith(cols[colIndex], (const UTF8Char*)"\"\",") || Text::StrEquals(cols[colIndex], (const UTF8Char*)"\"\""))
		{
			return 0;
		}
	}
	NotNullPtr<Text::String> s;
	UOSInt len = 0;
	const UTF8Char *csptr = cols[colIndex];
	const UTF8Char *ptr = csptr;
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
		UTF8Char *buff = s->v;
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
		return s.Ptr();
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
		UTF8Char *buff = s->v;
		ptr = csptr;
		while (true)
		{
			c = *ptr++;
			if (c == 0 || c == ',')
				break;
			*buff++ = c;
		}

		*buff = 0;
		return s.Ptr();
	}
}

UTF8Char *DB::CSVReader::GetStr(UOSInt colIndex, UTF8Char *buff, UOSInt buffSize)
{
	if (colIndex >= nCol)
		return 0;
	if (nullIfEmpty)
	{
		if (cols[colIndex][0] == 0 || cols[colIndex][0] == ',' || Text::StrStartsWith(cols[colIndex], (const UTF8Char*)"\"\",") || Text::StrEquals(cols[colIndex], (const UTF8Char*)"\"\""))
		{
			return 0;
		}
	}
	const UTF8Char *ptr = cols[colIndex];
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
	UTF8Char *sptr;
	sptr = this->GetStr(colIndex, buff, sizeof(buff));
	return Data::Timestamp(CSTRP(buff, sptr), Data::DateTimeUtil::GetLocalTzQhr());
}

Double DB::CSVReader::GetDbl(UOSInt colIndex)
{
	UTF8Char buff[60];
	this->GetStr(colIndex, buff, sizeof(buff));
	Text::StrTrim(buff);
	return Text::StrToDouble(buff);
}

Bool DB::CSVReader::GetBool(UOSInt colIndex)
{
	UTF8Char buff[20];
	UTF8Char *sptr;
	sptr = this->GetStr(colIndex, buff, sizeof(buff));
	if (Text::StrEqualsICaseC(buff, (UOSInt)(sptr - buff), UTF8STRC("TRUE")))
		return true;
	return Text::StrToInt32(buff) != 0;
}

UOSInt DB::CSVReader::GetBinarySize(UOSInt colIndex)
{
	return 0;
}

UOSInt DB::CSVReader::GetBinary(UOSInt colIndex, UInt8 *buff)
{
	return 0;
}

Math::Geometry::Vector2D *DB::CSVReader::GetVector(UOSInt colIndex)
{
	if (colIndex >= nCol)
		return 0;
	if (nullIfEmpty)
	{
		if (cols[colIndex][0] == 0 || cols[colIndex][0] == ',' || Text::StrStartsWith(cols[colIndex], (const UTF8Char*)"\"\",") || Text::StrEquals(cols[colIndex], (const UTF8Char*)"\"\""))
		{
			return 0;
		}
	}
	const UTF8Char *ptr = cols[colIndex];
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
		Math::Geometry::Vector2D *vec = Math::MSGeography::ParseBinary(sb.ToString(), sb.GetLength(), 0);
		return vec;
	}
	return 0;
}

Bool DB::CSVReader::GetUUID(UOSInt colIndex, Data::UUID *uuid)
{
	return false;
}

Bool DB::CSVReader::GetVariItem(UOSInt colIndex, Data::VariItem *item)
{
	if (colIndex >= nCol)
	{
		item->SetNull();
		return false;
	}
	if (nullIfEmpty)
	{
		if (cols[colIndex][0] == 0 || (*(UInt16*)cols[colIndex] == 0x2222 && cols[colIndex][2] == 0))
		{
			item->SetNull();
			return true;
		}
	}
	const UTF8Char *ptr = cols[colIndex];
	UTF8Char c;
	c = *ptr;
	if (c == '"')
	{
		NotNullPtr<Text::String> s = Text::String::New(this->colSize[colIndex]);
		UTF8Char *buff = s->v;
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
		item->SetStr(s);
		s->Release();
		return true;
	}
	else
	{
		item->SetStrSlow(ptr);
		return true;
	}	
}

UTF8Char *DB::CSVReader::GetName(UOSInt colIndex, UTF8Char *buff)
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

DB::DBUtil::ColType DB::CSVReader::GetColType(UOSInt colIndex, UOSInt *colSize)
{
	if (colIndex >= nHdr)
		return DB::DBUtil::CT_Unknown;
	return DB::DBUtil::CT_VarUTF8Char;
}

Bool DB::CSVReader::GetColDef(UOSInt colIndex, NotNullPtr<DB::ColDef> colDef)
{
	if (colIndex >= nHdr)
		return false;
	colDef->SetColName(this->hdrs[colIndex].ToCString());
	colDef->SetColType(DB::DBUtil::CT_VarUTF8Char);
	colDef->SetColSize(256);
	colDef->SetColDP(0);
	colDef->SetNotNull(true);
	colDef->SetPK(false);
	colDef->SetAutoInc(DB::ColDef::AutoIncType::None, 1, 1);
	colDef->SetDefVal(CSTR_NULL);
	colDef->SetAttr(CSTR_NULL);
	return true;
}
