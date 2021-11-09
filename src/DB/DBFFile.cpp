#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ByteTool.h"
#include "Data/DateTime.h"
#include "DB/ColDef.h"
#include "DB/DBFFile.h"
#include "Sync/Event.h"
#include "Sync/Mutex.h"
#include "Text/Encoding.h"
#include "Text/MyString.h"
#include "Text/MyStringFloat.h"
#include "Text/MyStringW.h"

DB::DBFFile::DBFFile(IO::IStreamData *stmData, UInt32 codePage) : DB::ReadingDB(stmData->GetFullName())
{
	UTF8Char u8buff[256];
	UInt8 buff[32];
	UOSInt currColOfst;
	UOSInt i;
	UInt64 currOfst;
	this->stmData = stmData->GetPartialData(0, stmData->GetDataSize());
//	NEW_CLASS(mut, Sync::Mutex());

	this->cols = 0;
	this->colCnt = 0;
	this->stmData->GetRealData(0, 32, buff);
	NEW_CLASS(this->enc, Text::Encoding(codePage));
	refPos = ReadUInt16(&buff[8]);
	rowSize = ReadUInt32(&buff[10]);
	colCnt = (UOSInt)(ReadUInt16(&buff[8]) >> 5) - 1;
	rowCnt = ReadUInt32(&buff[4]);
	cols = MemAlloc(DBFCol, colCnt);
	
	currOfst = 32;
	currColOfst = 1;
	i = 0;
	while (i < colCnt)
	{
		this->stmData->GetRealData(currOfst, 32, buff);
		currOfst += 32;
		cols[i].name = MemAlloc(UTF8Char, enc->CountUTF8Chars(buff, 11) + 1);
		*enc->UTF8FromBytes(cols[i].name, buff, 11, 0) = 0;
		Text::StrRTrim(cols[i].name);
		cols[i].type = *(Int32*)&buff[11];
		cols[i].colSize = buff[16];
		cols[i].colDP = buff[17];
		cols[i].colOfst = currColOfst;
		currColOfst += cols[i].colSize;
		i += 1;
	}

	this->name = 0;
	Text::StrConcat(u8buff, this->stmData->GetShortName());
	i = Text::StrLastIndexOf(u8buff, '.');
	if (i != INVALID_INDEX)
	{
		u8buff[i] = 0;
	}
	this->name = Text::StrCopyNew(u8buff);
}

DB::DBFFile::~DBFFile()
{
//	DEL_CLASS(mut);
	if (cols)
	{
		UOSInt i = colCnt;
		while (i-- > 0)
		{
			MemFree(this->cols[i].name);
		}
		MemFree(cols);
		this->cols = 0;
	}
	if (enc)
	{
		DEL_CLASS(enc);
		enc = 0;
	}
	if (this->name)
	{
		Text::StrDelNew(this->name);
		this->name = 0;
	}
	if (this->stmData)
	{
		DEL_CLASS(this->stmData);
		this->stmData = 0;
	}
}

UOSInt DB::DBFFile::GetTableNames(Data::ArrayList<const UTF8Char*> *names)
{
	if (this->name)
	{
		names->Add(this->name);
	}
	else
	{
		names->Add((const UTF8Char*)"DBF");
	}
	return 1;
}

DB::DBReader *DB::DBFFile::GetTableData(const UTF8Char *tableName, Data::ArrayList<const UTF8Char*> *columnNames, UOSInt ofst, UOSInt maxCnt, const UTF8Char *ordering, DB::QueryConditions *condition)
{
	if (cols)
	{
		DB::DBFReader *r;
		NEW_CLASS(r, DB::DBFReader(this, this->colCnt, this->cols, this->rowSize, this->enc));
		return r;
	}
	else
	{
		return 0;
	}
}

void DB::DBFFile::CloseReader(DBReader *r)
{
	DB::DBFReader *rdr = (DB::DBFReader*)r;
	DEL_CLASS(rdr);
}

void DB::DBFFile::GetErrorMsg(Text::StringBuilderUTF *str)
{
}

void DB::DBFFile::Reconnect()
{
}

Bool DB::DBFFile::IsError()
{
	return this->cols == 0;
}

UInt32 DB::DBFFile::GetCodePage()
{
	return this->enc->GetEncCodePage();
}

UOSInt DB::DBFFile::GetColSize(UOSInt colIndex)
{
	if (colIndex >= this->colCnt)
		return 0;
	return this->cols[colIndex].colSize;
}

OSInt DB::DBFFile::GetColIndex(const UTF8Char *name)
{
	UOSInt i = this->colCnt;
	while (i-- > 0)
	{
		if (Text::StrCompareICase(name, this->cols[i].name) == 0)
			return (OSInt)i;
	}
	return -1;
}

WChar *DB::DBFFile::GetRecord(WChar *buff, UOSInt row, UOSInt col)
{
	if (col >= this->colCnt)
		return 0;

	UInt8 *cbuff = MemAlloc(UInt8, this->cols[col].colSize);
	this->stmData->GetRealData(this->refPos + this->rowSize * row + this->cols[col].colOfst, this->cols[col].colSize, cbuff);
	WChar *ret = this->enc->WFromBytes(buff, cbuff, this->cols[col].colSize, 0);
	MemFree(cbuff);
	return ret;
}

UTF8Char *DB::DBFFile::GetRecord(UTF8Char *buff, UOSInt row, UOSInt col)
{
	if (col >= this->colCnt)
		return 0;

	UInt8 *cbuff = MemAlloc(UInt8, this->cols[col].colSize);
	this->stmData->GetRealData(this->refPos + this->rowSize * row + this->cols[col].colOfst, this->cols[col].colSize, cbuff);
	UTF8Char *ret = this->enc->UTF8FromBytes(buff, cbuff, this->cols[col].colSize, 0);
	MemFree(cbuff);
	return ret;
}

UOSInt DB::DBFFile::GetColCount()
{
	return this->colCnt;
}

UOSInt DB::DBFFile::GetRowCnt()
{
	return this->rowCnt;
}

UTF8Char *DB::DBFFile::GetColumnName(UOSInt colIndex, UTF8Char *buff)
{
	if (colIndex >= this->colCnt)
		return 0;
	return Text::StrConcat(buff, this->cols[colIndex].name);
}

DB::DBUtil::ColType DB::DBFFile::GetColumnType(UOSInt colIndex, UOSInt *colSize)
{
	if (colIndex >= this->colCnt)
	{
		if (colSize)
		{
			*colSize = 0;
		}
		return DB::DBUtil::CT_VarChar;
	}
	if (colSize)
	{
		*colSize = cols[colIndex].colSize;
	}
	if (cols[colIndex].type == 'D')
	{
		return DB::DBUtil::CT_DateTime;
	}
	else if (cols[colIndex].type == 'T')
	{
		return DB::DBUtil::CT_DateTime;
	}
	else if (cols[colIndex].type == 'N')
	{
		if (cols[colIndex].colSize == 6)
		{
			return DB::DBUtil::CT_Int16;
		}
		else if (cols[colIndex].colSize == 11)
		{
			return DB::DBUtil::CT_Int32;
		}
		else if (cols[colIndex].colSize == 21)
		{
			return DB::DBUtil::CT_Int64;
		}
		else if (cols[colIndex].colSize == 3)
		{
			return DB::DBUtil::CT_Byte;
		}
		else if (cols[colIndex].colSize == 10)
		{
			return DB::DBUtil::CT_UInt32;
		}
		else
		{
			return DB::DBUtil::CT_Double;
		}
	}
	else if (cols[colIndex].type == 'L')
	{
		return DB::DBUtil::CT_Bool;
	}
	else if (cols[colIndex].type == 'M')
	{
		return DB::DBUtil::CT_VarChar;
	}
	else if (cols[colIndex].type == 'C')
	{
		return DB::DBUtil::CT_Char;
	}
	else if (cols[colIndex].type == 'F')
	{
		return DB::DBUtil::CT_Float;
	}
	else if (cols[colIndex].type == 'B')
	{
		return DB::DBUtil::CT_Double;
	}
	else if (cols[colIndex].type == 'I')
	{
		return DB::DBUtil::CT_Int32;
	}
	return DB::DBUtil::CT_VarChar;
}

Bool DB::DBFFile::GetColumnDef(UOSInt colIndex, DB::ColDef *colDef)
{
	if (colIndex >= this->colCnt)
	{
		return false;
	}
	colDef->SetColName(this->cols[colIndex].name);
	colDef->SetColType(this->GetColumnType(colIndex, 0));
	colDef->SetColSize(this->cols[colIndex].colSize);
	colDef->SetColDP(this->cols[colIndex].colDP);
	colDef->SetNotNull(false);
	colDef->SetPK(false);
	colDef->SetAutoInc(false);
	colDef->SetDefVal(0);
	colDef->SetAttr(0);
	return true;
}

Bool DB::DBFFile::ReadRowData(UOSInt row, UInt8 *recordBuff)
{
	return this->rowSize == this->stmData->GetRealData(this->refPos + this->rowSize * row, this->rowSize, recordBuff);
}

Int32 DB::DBFFile::GetCodePage(UInt8 langDriver)
{
	switch (langDriver)
	{
	case 0x01:
		return 437;
	case 0x02:
		return 850;
	case 0x03:
		return 1252;
	case 0x04:
		return 10000;
	case 0x08:
		return 865;
	case 0x09:
		return 437;
	case 0x0a:
		return 850;
	case 0x0b:
		return 437;
	case 0x0d:
		return 437;
	case 0x0e:
		return 850;
	case 0x0f:
		return 437;
	case 0x10:
		return 850;
	case 0x11:
		return 437;
	case 0x12:
		return 850;
	case 0x13:
		return 437;
	case 0x14:
		return 850;
	case 0x15:
		return 437;
	case 0x16:
		return 850;
	case 0x17:
		return 865;
	case 0x18:
		return 437;
	case 0x19:
		return 437;
	case 0x1A:
		return 850;
	case 0x1B:
		return 437;
	case 0x1C:
		return 863;
	case 0x1D:
		return 850;
	case 0x1F:
		return 852;
	case 0x22:
		return 852;
	case 0x23:
		return 852;
	case 0x24:
		return 860;
	case 0x25:
		return 850;
	case 0x26:
		return 866;
	case 0x37:
		return 850;
	case 0x40:
		return 852;
	case 0x4d:
		return 936;
	case 0x4e:
		return 949;
	case 0x4f:
		return 950;
	case 0x50:
		return 874;
	case 0x57:
		return 0;
	case 0x58:
		return 1252;
	case 0x59:
		return 1252;
	case 0x64:
		return 852;
	case 0x65:
		return 866;
	case 0x66:
		return 865;
	case 0x67:
		return 961;
	case 0x68:
		return 895;
	case 0x69:
		return 620;
	case 0x6a:
		return 737;
	case 0x6b:
		return 857;
	case 0x6c:
		return 863;
	case 0x78:
		return 950;
	case 0x79:
		return 949;
	case 0x7a:
		return 936;
	case 0x7b:
		return 932;
	case 0x7c:
		return 874;
	case 0x86:
		return 737;
	case 0x87:
		return 852;
	case 0x88:
		return 857;
	case 0x96:
		return 10007;
	case 0x97:
		return 10029;
	case 0x98:
		return 10006;
	case 0xc8:
		return 1250;
	case 0xc9:
		return 1251;
	case 0xca:
		return 1254;
	case 0xcb:
		return 1253;
	case 0xcc:
		return 1257;
	default:
		return 0;
	}
}

UInt8 DB::DBFFile::GetLangDriver(UInt32 codePage)
{
	switch (codePage)
	{
	case 437:
		return 0x1b;
	case 620:
		return 0x69;
	case 737:
		return 0x6a;
	case 850:
		return 0x2;
	case 852:
		return 0x64;
	case 857:
		return 0x6b;
	case 860:
		return 0x24;
	case 861:
		return 0x67;
	case 863:
		return 0x1c;
	case 865:
		return 0x17;
	case 866:
		return 0x26;
	case 874:
		return 0x50;
	case 895:
		return 0x68;
	case 932:
		return 0x13;
	case 936:
		return 0x4d;
	case 949:
		return 0x4e;
	case 950:
		return 0x4f;
	case 1250:
		return 0xc8;
	case 1251:
		return 0xc9;
	case 1252:
		return 0x58;
	case 1253:
		return 0xcb;
	case 1254:
		return 0xca;
	case 1257:
		return 0xcc;
	case 10000:
		return 0x04;
	case 10006:
		return 0x98;
	case 10007:
		return 0x96;
	case 10029:
		return 0x97;
	default:
		return 0x57;
	}
}

DB::DBFReader::DBFReader(DB::DBFFile *dbf, UOSInt colCnt, DB::DBFFile::DBFCol *cols, UOSInt rowSize, Text::Encoding *enc)
{
	this->dbf = dbf;
	this->enc = enc;
	this->rowCnt = this->dbf->GetRowCnt();
	this->colCnt = colCnt;
	this->cols = cols;
	this->rowSize = rowSize;
	this->currIndex = -1;
	this->recordExist = false;
	this->recordData = MemAlloc(UInt8, this->rowSize);
}

DB::DBFReader::~DBFReader()
{
	MemFree(this->recordData);
}

Bool DB::DBFReader::ReadNext()
{
	if ((UOSInt)(this->currIndex + 1) < this->rowCnt)
	{
		this->currIndex++;
		this->recordExist = true;
		this->dbf->ReadRowData((UOSInt)this->currIndex, this->recordData);
		return true;
	}
	else
	{
		this->currIndex = (OSInt)this->rowCnt;
		this->recordExist = false;
		return false;
	}
}

UOSInt DB::DBFReader::ColCount()
{
	return this->colCnt;
}

OSInt DB::DBFReader::GetRowChanged()
{
	return -1;
}

Int32 DB::DBFReader::GetInt32(UOSInt colIndex)
{
	if (!this->recordExist)
		return 0;
	if (colIndex >= this->colCnt)
		return 0;
	Char buff[12];
	MemCopyNO(buff, &this->recordData[this->cols[colIndex].colOfst], this->cols[colIndex].colSize);
	buff[this->cols[colIndex].colSize] = 0;
	Text::StrRTrim(buff);
	return Text::StrToInt32(buff);
}

Int64 DB::DBFReader::GetInt64(UOSInt colIndex)
{
	if (!this->recordExist)
		return 0;
	if (colIndex >= this->colCnt)
		return 0;
	Char buff[25];
	MemCopyNO(buff, &this->recordData[this->cols[colIndex].colOfst], this->cols[colIndex].colSize);
	buff[this->cols[colIndex].colSize] = 0;
	Text::StrRTrim(buff);
	return Text::StrToInt64(buff);
}

WChar *DB::DBFReader::GetStr(UOSInt colIndex, WChar *buff)
{
	if (!this->recordExist)
		return 0;
	if (colIndex >= this->colCnt)
		return 0;
	this->enc->WFromBytes(buff, &this->recordData[this->cols[colIndex].colOfst], this->cols[colIndex].colSize, 0);
	if (cols[colIndex].type != 'C')
	{
		return Text::StrLTrim(buff);
	}
	else
	{
		return Text::StrRTrim(buff);
	}
}

Bool DB::DBFReader::GetStr(UOSInt colIndex, Text::StringBuilderUTF *sb)
{
	if (!this->recordExist)
		return false;
	if (colIndex >= this->colCnt)
		return false;
	UOSInt strLen = this->enc->CountUTF8Chars(&this->recordData[this->cols[colIndex].colOfst], this->cols[colIndex].colSize);
	UTF8Char *sbuff = MemAlloc(UTF8Char, strLen + 1);
	this->enc->UTF8FromBytes(sbuff, &this->recordData[this->cols[colIndex].colOfst], this->cols[colIndex].colSize, 0);
	sbuff[strLen] = 0;
	sb->Append(sbuff);
	MemFree(sbuff);
	return true;
}

const UTF8Char *DB::DBFReader::GetNewStr(UOSInt colIndex)
{
	if (!this->recordExist)
		return 0;
	if (colIndex >= this->colCnt)
		return 0;
	UOSInt strLen = this->enc->CountUTF8Chars(&this->recordData[this->cols[colIndex].colOfst], this->cols[colIndex].colSize);
	UTF8Char *buff;
	buff = MemAlloc(UTF8Char, strLen + 1);
	this->enc->UTF8FromBytes(buff, &this->recordData[this->cols[colIndex].colOfst], this->cols[colIndex].colSize, 0);
	return buff;
}

UTF8Char *DB::DBFReader::GetStr(UOSInt colIndex, UTF8Char *buff, UOSInt buffSize)
{
	if (!this->recordExist)
		return 0;
	if (colIndex >= this->colCnt)
		return 0;
	this->enc->UTF8FromBytes(buff, &this->recordData[this->cols[colIndex].colOfst], this->cols[colIndex].colSize, 0);
	if (cols[colIndex].type != 'C')
	{
		return Text::StrLTrim(buff);
	}
	else
	{
		return Text::StrRTrim(buff);
	}
}

DB::DBReader::DateErrType DB::DBFReader::GetDate(UOSInt colIndex, Data::DateTime *outVal)
{
	if (!this->recordExist)
		return DB::DBReader::DET_ERROR;
	if (colIndex >= this->colCnt)
		return DB::DBReader::DET_ERROR;
	if (this->cols[colIndex].type != 'D')
	{
		return DB::DBReader::DET_ERROR;
	}
	Char buff[5];
	Char *currPtr = (Char*)&this->recordData[this->cols[colIndex].colOfst];
	UInt16 year;
	Int32 month;
	Int32 day;
	buff[0] = currPtr[0];
	buff[1] = currPtr[1];
	buff[2] = currPtr[2];
	buff[3] = currPtr[3];
	buff[4] = 0;
	Text::StrToUInt16S(buff, &year, 0);
	buff[0] = currPtr[4];
	buff[1] = currPtr[5];
	buff[2] = 0;
	month = Text::StrToInt32(buff);
	buff[0] = currPtr[6];
	buff[1] = currPtr[7];
	buff[2] = 0;
	day = Text::StrToInt32(buff);
	outVal->SetValue(year, month, day, 0, 0, 0, 0);
	return DB::DBReader::DET_OK;
}

Double DB::DBFReader::GetDbl(UOSInt colIndex)
{
	if (!this->recordExist)
		return 2;
	if (colIndex >= this->colCnt)
		return 0;
	Char buff[30];
	MemCopyNO(buff, &this->recordData[this->cols[colIndex].colOfst], this->cols[colIndex].colSize);
	buff[this->cols[colIndex].colSize] = 0;
	Text::StrRTrim(buff);
	return Text::StrToDouble(buff);
}

Bool DB::DBFReader::GetBool(UOSInt colIndex)
{
	if (!this->recordExist)
		return false;
	if (colIndex >= this->colCnt)
		return false;
	Char buff[30];
	MemCopyNO(buff, &this->recordData[this->cols[colIndex].colOfst], this->cols[colIndex].colSize);
	buff[this->cols[colIndex].colSize] = 0;
	Text::StrRTrim(buff);
	if (Text::StrCompareICase(buff, "false") == 0)
		return false;
	else if (Text::StrCompareICase(buff, "true") == 0)
		return true;
	else if (buff[0] == 'T' || buff[0] == 't' || buff[0] == 'Y' || buff[0] == 'y')
	{
		return true;
	}
	else if (buff[0] == 'F' || buff[0] == 'f' || buff[0] == 'N' || buff[0] == 'n' || buff[0] == '?')
	{
		return false;
	}
	else
		return Text::StrToInt32(buff) != 0;
}

UOSInt DB::DBFReader::GetBinarySize(UOSInt colIndex)
{
	if (!this->recordExist)
		return 0;
	if (colIndex >= this->colCnt)
		return 0;
	return this->cols[colIndex].colSize;
}

UOSInt DB::DBFReader::GetBinary(UOSInt colIndex, UInt8 *buff)
{
	if (!this->recordExist)
		return 0;
	if (colIndex >= this->colCnt)
		return 0;
	MemCopyNO(buff, &this->recordData[this->cols[colIndex].colOfst], this->cols[colIndex].colSize);
	return this->cols[colIndex].colSize;
}

Math::Vector2D *DB::DBFReader::GetVector(UOSInt colIndex)
{
	return 0;
}

Bool DB::DBFReader::GetUUID(UOSInt colIndex, Data::UUID *uuid)
{
	return false;
}

Bool DB::DBFReader::IsNull(UOSInt colIndex)
{
	if (!this->recordExist)
		return true;
	if (colIndex >= this->colCnt)
		return true;

	UInt8 *buff = &this->recordData[this->cols[colIndex].colOfst];
	UOSInt buffSize = this->cols[colIndex].colSize;
	Bool isEmpty = true;
	if (this->cols[colIndex].type == DB::DBUtil::CT_Bool && buff[0] == '?')
		return true;
	while (buffSize-- > 0)
	{
		if (*buff++ != ' ')
		{
			isEmpty = false;
			break;
		}
	}
	return isEmpty;
}

/*WChar *DB::DBFReader::GetName(OSInt colIndex)
{
	if (colIndex < 0 || colIndex >= this->colCnt)
		return 0;
	return this->cols[colIndex].name;
}*/

UTF8Char *DB::DBFReader::GetName(UOSInt colIndex, UTF8Char *buff)
{
	if (colIndex >= this->colCnt)
		return 0;
	return Text::StrConcat(buff, this->cols[colIndex].name);
}

DB::DBUtil::ColType DB::DBFReader::GetColType(UOSInt colIndex, UOSInt *colSize)
{
	return this->dbf->GetColumnType(colIndex, colSize);
}

Bool DB::DBFReader::GetColDef(UOSInt colIndex, DB::ColDef *colDef)
{
	return this->dbf->GetColumnDef(colIndex, colDef);
}

void DB::DBFReader::DelNewStr(const UTF8Char *s)
{
	MemFree((UTF8Char*)s);
}
