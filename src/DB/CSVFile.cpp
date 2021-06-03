#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/DateTime.h"
#include "DB/ColDef.h"
#include "DB/CSVFile.h"
#include "DB/DBConn.h"
#include "IO/FileStream.h"
#include "IO/StreamReader.h"
#include "Text/MyString.h"
#include "Text/MyStringFloat.h"
#include "Text/MyStringW.h"

DB::CSVFile::CSVFile(const UTF8Char *fileName, UInt32 codePage) : DB::ReadingDB(fileName)
{
	this->fileName = Text::StrCopyNew(fileName);
	this->stm = 0;
	this->codePage = codePage;
	this->noHeader = false;
}

DB::CSVFile::CSVFile(IO::SeekableStream *stm, UInt32 codePage) : DB::ReadingDB(stm->GetSourceNameObj())
{
	this->fileName = Text::StrCopyNew(stm->GetSourceNameObj());
	this->stm = stm;
	this->codePage = codePage;
	this->noHeader = false;
}

DB::CSVFile::~CSVFile()
{
	Text::StrDelNew(this->fileName);
}

UOSInt DB::CSVFile::GetTableNames(Data::ArrayList<const UTF8Char*> *names)
{
	names->Add((const UTF8Char*)"CSVFile");
	return 1;
}

DB::DBReader *DB::CSVFile::GetTableData(const UTF8Char *name, UOSInt maxCnt, void *ordering, void *condition)
{
	if (this->stm)
	{
		IO::StreamReader *rdr;
		DB::CSVReader *r;
		this->stm->Seek(IO::SeekableStream::ST_BEGIN, 0);
		NEW_CLASS(rdr, IO::StreamReader(this->stm, codePage));
		NEW_CLASS(r, DB::CSVReader(0, rdr, this->noHeader));
		return r;
	}
	else if (this->fileName)
	{
		IO::StreamReader *rdr;
		DB::CSVReader *r;
		IO::FileStream *fs;
		NEW_CLASS(fs, IO::FileStream(this->fileName, IO::FileStream::FILE_MODE_READONLY, IO::FileStream::FILE_SHARE_DENY_NONE, IO::FileStream::BT_SEQUENTIAL));
		if (!fs->IsError())
		{
			NEW_CLASS(rdr, IO::StreamReader(fs, codePage));
			NEW_CLASS(r, DB::CSVReader(fs, rdr, this->noHeader));
			return r;
		}
		else
		{
			DEL_CLASS(fs);
			return 0;
		}
	}
	else
	{
		return 0;
	}
}

void DB::CSVFile::CloseReader(DBReader *r)
{
	DB::CSVReader *rdr = (DB::CSVReader *)r;
	DEL_CLASS(rdr);
}

void DB::CSVFile::GetErrorMsg(Text::StringBuilderUTF *str)
{
}

void DB::CSVFile::Reconnect()
{
}

void DB::CSVFile::SetNoHeader(Bool noHeader)
{
	this->noHeader = noHeader;
}

DB::CSVReader::CSVReader(IO::Stream *stm, IO::StreamReader *rdr, Bool noHeader)
{
	this->stm = stm;
	this->rdr = rdr;
	this->noHeader = noHeader;
	this->nCol = 0;
	this->row = MemAlloc(UTF8Char, 16384);
	this->cols = MemAlloc(UTF8Char*, 128);
	this->hdrs = MemAlloc(UTF8Char*, 128);

	UTF8Char *sptr;
	UTF8Char *currPtr;
	WChar c;
	Bool colStart = true;
	Bool quote = false;
	sptr = this->rdr->ReadLine(this->row, 16383);
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
				sptr = this->rdr->ReadLine(sptr, 16384);
				if (sptr == 0)
					break;
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
	this->nHdr = Text::StrCSVSplit(this->hdrs, 128, this->hdr);
	this->nCol = this->nHdr;
}

DB::CSVReader::~CSVReader()
{
	DEL_CLASS(this->rdr);
	SDEL_CLASS(this->stm);
	MemFree(this->row);
	MemFree(this->cols);
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
			this->cols[nCol] = this->hdrs[nCol];
		}
		return true;
	}

	sptr = this->rdr->ReadLine(this->row, 16384);
	if (sptr == 0)
	{
		this->nCol = 0;
		return false;
	}

	nCol = 1;
	cols[0] = this->row;

	currPtr = this->row;
	colStart = true;
	while (true)
	{
		c = *currPtr++;
		if (c == 0)
		{
			if (quote)
			{
				currPtr--;
				sptr = this->rdr->GetLastLineBreak(currPtr);
				sptr = this->rdr->ReadLine(sptr, 16384);
				if (sptr == 0)
					break;
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
			cols[nCol++] = currPtr;
			colStart = true;
		}
		else
		{
			colStart = false;
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

Bool DB::CSVReader::GetStr(UOSInt colIndex, Text::StringBuilderUTF *sb)
{
	if (colIndex >= nCol)
		return false;
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
						sb->AppendChar('"', 1);
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

const UTF8Char *DB::CSVReader::GetNewStr(UOSInt colIndex)
{
	if (colIndex >= nCol)
		return 0;
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
		UTF8Char *newStr = MemAlloc(UTF8Char, len + 1);
		UTF8Char *buff = newStr;
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
		Text::StrDelNew(csptr);
		return newStr;
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
		UTF8Char *newStr = MemAlloc(UTF8Char, len + 1);
		UTF8Char *buff = newStr;
		ptr = csptr;
		while (true)
		{
			c = *ptr++;
			if (c == 0 || c == ',')
				break;
			*buff++ = c;
		}

		*buff = 0;
		Text::StrDelNew(csptr);
		return newStr;
	}
}

UTF8Char *DB::CSVReader::GetStr(UOSInt colIndex, UTF8Char *buff, UOSInt buffSize)
{
	if (colIndex >= nCol)
		return 0;
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

DB::DBReader::DateErrType DB::CSVReader::GetDate(UOSInt colIndex, Data::DateTime *outVal)
{
	UTF8Char buff[60];
	this->GetStr(colIndex, buff, sizeof(buff));
	outVal->SetValue(buff);
	return DB::DBReader::DET_OK;
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
	this->GetStr(colIndex, buff, sizeof(buff));
	if (Text::StrEqualsICase(buff, (const UTF8Char*)"TRUE"))
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

Math::Vector2D *DB::CSVReader::GetVector(UOSInt colIndex)
{
	return 0;
}

UTF8Char *DB::CSVReader::GetName(UOSInt colIndex, UTF8Char *buff)
{
	if (colIndex >= this->nHdr)
		return 0;
	return Text::StrConcat(buff, this->hdrs[colIndex]);
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
	return DB::DBUtil::CT_VarChar;
}

Bool DB::CSVReader::GetColDef(UOSInt colIndex, DB::ColDef *colDef)
{
	if (colIndex >= nHdr)
		return false;
	colDef->SetColName(this->hdrs[colIndex]);
	colDef->SetColType(DB::DBUtil::CT_VarChar);
	colDef->SetColSize(256);
	colDef->SetColDP(0);
	colDef->SetNotNull(true);
	colDef->SetPK(false);
	colDef->SetAutoInc(false);
	colDef->SetDefVal(0);
	colDef->SetAttr(0);
	return true;
}

void DB::CSVReader::DelNewStr(const UTF8Char *s)
{
	MemFree((void*)s);
}
