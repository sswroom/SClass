#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/ByteTool_C.h"
#include "Data/DateTime.h"
#include "DB/ColDef.h"
#include "DB/DBFFile.h"
#include "DB/TableDef.h"
#include "IO/StmData/BufferedStreamData.h"
#include "Sync/Event.h"
#include "Sync/Mutex.h"
#include "Text/Encoding.h"
#include "Text/MyString.h"
#include "Text/MyStringFloat.h"
#include "Text/MyStringW.h"

DB::DBFFile::DBFFile(NN<IO::StreamData> stmData, UInt32 codePage) : DB::ReadingDB(stmData->GetFullName()), enc(codePage)
{
	UTF8Char sbuff[256];
	UnsafeArray<UTF8Char> sptr;
	UInt8 buff[32];
	UOSInt currColOfst;
	UOSInt i;
	UInt64 currOfst;
	NEW_CLASSNN(this->stmData, IO::StmData::BufferedStreamData(stmData->GetPartialData(0, stmData->GetDataSize())));

	this->colCnt = 0;
	this->stmData->GetRealData(0, 32, BYTEARR(buff));
	refPos = ReadUInt16(&buff[8]);
	rowSize = ReadUInt32(&buff[10]);
	colCnt = (UOSInt)(ReadUInt16(&buff[8]) >> 5) - 1;
	rowCnt = ReadUInt32(&buff[4]);
	this->cols = MemAllocArr(DBFCol, colCnt);
	
	currOfst = 32;
	currColOfst = 1;
	i = 0;
	while (i < colCnt)
	{
		this->stmData->GetRealData(currOfst, 32, BYTEARR(buff));
		currOfst += 32;
		cols[i].name = Text::String::New(enc.CountUTF8Chars(buff, 11));
		*enc.UTF8FromBytes(cols[i].name->v, buff, 11, 0) = 0;
		cols[i].name->RTrim();
		cols[i].type = *(Int32*)&buff[11];
		cols[i].colSize = buff[16];
		cols[i].colDP = buff[17];
		cols[i].colOfst = currColOfst;
		currColOfst += cols[i].colSize;
		i += 1;
	}

	sptr = this->stmData->GetShortName().OrEmpty().ConcatTo(sbuff);
	i = Text::StrLastIndexOfCharC(sbuff, (UOSInt)(sptr - sbuff), '.');
	if (i != INVALID_INDEX)
	{
		sbuff[i] = 0;
		sptr = &sbuff[i];
	}
	this->name = Text::String::NewP(sbuff, sptr);
}

DB::DBFFile::~DBFFile()
{
	UOSInt i = colCnt;
	while (i-- > 0)
	{
		this->cols[i].name->Release();
	}
	MemFreeArr(this->cols);
	this->name->Release();
	this->stmData.Delete();
}

UOSInt DB::DBFFile::QueryTableNames(Text::CString schemaName, NN<Data::ArrayListStringNN> names)
{
	if (schemaName.leng != 0)
		return 0;
	names->Add(this->name->Clone());
	return 1;
}

Optional<DB::DBReader> DB::DBFFile::QueryTableData(Text::CString schemaName, Text::CStringNN tableName, Optional<Data::ArrayListStringNN> columnNames, UOSInt ofst, UOSInt maxCnt, Text::CString ordering, Optional<Data::QueryConditions> condition)
{
	if (this->colCnt > 0)
	{
		NN<DB::DBFReader> r;
		NEW_CLASSNN(r, DB::DBFReader(*this, this->colCnt, this->cols, this->rowSize, this->enc));
		return r;
	}
	else
	{
		return 0;
	}
}

Optional<DB::TableDef> DB::DBFFile::GetTableDef(Text::CString schemaName, Text::CStringNN tableName)
{
	NN<DB::ColDef> col;
	DB::TableDef *tab;
	UOSInt i;
	UOSInt j;
	NEW_CLASS(tab, DB::TableDef(schemaName, tableName));
	i = 0;
	j = this->GetColCount();
	while (i < j)
	{
		NEW_CLASSNN(col, DB::ColDef(Text::String::NewEmpty()));
		this->GetColumnDef(i, col);
		tab->AddCol(col);
		i++;
	}
	return tab;
}

void DB::DBFFile::CloseReader(NN<DBReader> r)
{
	DB::DBFReader *rdr = (DB::DBFReader*)r.Ptr();
	DEL_CLASS(rdr);
}

void DB::DBFFile::GetLastErrorMsg(NN<Text::StringBuilderUTF8> str)
{
}

void DB::DBFFile::Reconnect()
{
}

Bool DB::DBFFile::IsError()
{
	return this->colCnt == 0;
}

UInt32 DB::DBFFile::GetCodePage()
{
	return this->enc.GetEncCodePage();
}

UOSInt DB::DBFFile::GetColSize(UOSInt colIndex)
{
	if (colIndex >= this->colCnt)
		return 0;
	return this->cols[colIndex].colSize;
}

OSInt DB::DBFFile::GetColIndex(UnsafeArray<const UTF8Char> name)
{
	UOSInt i = this->colCnt;
	while (i-- > 0)
	{
		if (Text::StrCompareICase(name, this->cols[i].name->v) == 0)
			return (OSInt)i;
	}
	return -1;
}

UnsafeArrayOpt<WChar> DB::DBFFile::GetRecord(UnsafeArray<WChar> buff, UOSInt row, UOSInt col)
{
	if (col >= this->colCnt)
		return 0;

	UInt8 *cbuff = MemAlloc(UInt8, this->cols[col].colSize);
	this->stmData->GetRealData(this->refPos + this->rowSize * row + this->cols[col].colOfst, this->cols[col].colSize, Data::ByteArray(cbuff, this->cols[col].colSize));
	UnsafeArray<WChar> ret = this->enc.WFromBytes(buff, cbuff, this->cols[col].colSize, 0);
	MemFree(cbuff);
	return ret;
}

UnsafeArrayOpt<UTF8Char> DB::DBFFile::GetRecord(UnsafeArray<UTF8Char> buff, UOSInt row, UOSInt col)
{
	if (col >= this->colCnt)
		return 0;

	if (this->enc.IsUTF8())
	{
		buff += this->stmData->GetRealData(this->refPos + this->rowSize * row + this->cols[col].colOfst, this->cols[col].colSize, Data::ByteArray(buff, this->cols[col].colSize));
		*buff = 0;
		return buff;
	}
	else
	{
		UInt8 *cbuff = MemAlloc(UInt8, this->cols[col].colSize);
		this->stmData->GetRealData(this->refPos + this->rowSize * row + this->cols[col].colOfst, this->cols[col].colSize, Data::ByteArray(cbuff, this->cols[col].colSize));
		UnsafeArray<UTF8Char> ret = this->enc.UTF8FromBytes(buff, cbuff, this->cols[col].colSize, 0);
		MemFree(cbuff);
		return ret;
	}
}

Bool DB::DBFFile::GetRecord(NN<Text::StringBuilderUTF8> sb, UOSInt row, UOSInt col)
{
	if (col >= this->colCnt)
		return false;

	if (this->enc.IsUTF8())
	{
		sb->AllocLeng(this->cols[col].colSize);
		UOSInt readSize = this->stmData->GetRealData(this->refPos + this->rowSize * row + this->cols[col].colOfst, this->cols[col].colSize, Data::ByteArray(sb->GetEndPtr(), this->cols[col].colSize));
		sb->SetEndPtr(sb->GetEndPtr() + readSize);
		sb->GetEndPtr()[0] = 0;
		return true;
	}
	else
	{
		UInt8 *cbuff = MemAlloc(UInt8, this->cols[col].colSize);
		this->stmData->GetRealData(this->refPos + this->rowSize * row + this->cols[col].colOfst, this->cols[col].colSize, Data::ByteArray(cbuff, this->cols[col].colSize));
		UOSInt size = this->enc.CountUTF8Chars(cbuff, this->cols[col].colSize);
		sb->AllocLeng(size);
		UnsafeArray<UTF8Char> ret = this->enc.UTF8FromBytes(sb->GetEndPtr(), cbuff, this->cols[col].colSize, 0);
		MemFree(cbuff);
		sb->SetEndPtr(ret);
		return true;
	}
}

UOSInt DB::DBFFile::GetColCount()
{
	return this->colCnt;
}

UOSInt DB::DBFFile::GetRowCnt()
{
	return this->rowCnt;
}

UnsafeArrayOpt<UTF8Char> DB::DBFFile::GetColumnName(UOSInt colIndex, UnsafeArray<UTF8Char> buff)
{
	if (colIndex >= this->colCnt)
		return 0;
	return this->cols[colIndex].name->ConcatTo(buff);
}

DB::DBUtil::ColType DB::DBFFile::GetColumnType(UOSInt colIndex, OptOut<UOSInt> colSize)
{
	if (colIndex >= this->colCnt)
	{
		colSize.Set(0);
		return DB::DBUtil::CT_Unknown;
	}
	colSize.Set(cols[colIndex].colSize);
	if (cols[colIndex].type == 'D')
	{
		return DB::DBUtil::CT_Date;
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
		return DB::DBUtil::CT_VarUTF8Char;
	}
	else if (cols[colIndex].type == 'C')
	{
		return DB::DBUtil::CT_UTF8Char;
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
	return DB::DBUtil::CT_VarUTF8Char;
}

Bool DB::DBFFile::GetColumnDef(UOSInt colIndex, NN<DB::ColDef> colDef)
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
	colDef->SetAutoIncNone();
	colDef->SetDefVal(Text::CString(nullptr));
	colDef->SetAttr(Text::CString(nullptr));
	return true;
}

Bool DB::DBFFile::ReadRowData(UOSInt row, UnsafeArray<UInt8> recordBuff)
{
	return this->rowSize == this->stmData->GetRealData(this->refPos + this->rowSize * row, this->rowSize, Data::ByteArray(recordBuff, this->rowSize));
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

DB::DBFReader::DBFReader(NN<DB::DBFFile> dbf, UOSInt colCnt, UnsafeArray<DB::DBFFile::DBFCol> cols, UOSInt rowSize, NN<Text::Encoding> enc)
{
	this->dbf = dbf;
	this->enc = enc;
	this->rowCnt = this->dbf->GetRowCnt();
	this->colCnt = colCnt;
	this->cols = cols;
	this->rowSize = rowSize;
	this->currIndex = -1;
	this->recordExist = false;
	this->recordData = MemAllocArr(UInt8, this->rowSize);
}

DB::DBFReader::~DBFReader()
{
	MemFreeArr(this->recordData);
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
	return Text::StrToInt32Ch(buff);
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
	return Text::StrToInt64Ch(buff);
}

UnsafeArrayOpt<WChar> DB::DBFReader::GetStr(UOSInt colIndex, UnsafeArray<WChar> buff)
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

Bool DB::DBFReader::GetStr(UOSInt colIndex, NN<Text::StringBuilderUTF8> sb)
{
	if (!this->recordExist)
		return false;
	if (colIndex >= this->colCnt)
		return false;
	UOSInt strLen = this->enc->CountUTF8Chars(&this->recordData[this->cols[colIndex].colOfst], this->cols[colIndex].colSize);
	UnsafeArray<UTF8Char> sbuff = MemAllocArr(UTF8Char, strLen + 1);
	this->enc->UTF8FromBytes(sbuff, &this->recordData[this->cols[colIndex].colOfst], this->cols[colIndex].colSize, 0);
	sbuff[strLen] = 0;
	sb->AppendC(sbuff, strLen);
	MemFreeArr(sbuff);
	return true;
}

Optional<Text::String> DB::DBFReader::GetNewStr(UOSInt colIndex)
{
	if (!this->recordExist)
		return 0;
	if (colIndex >= this->colCnt)
		return 0;
	UOSInt strLen = this->enc->CountUTF8Chars(&this->recordData[this->cols[colIndex].colOfst], this->cols[colIndex].colSize);
	NN<Text::String> s = Text::String::New(strLen);
	this->enc->UTF8FromBytes(s->v, &this->recordData[this->cols[colIndex].colOfst], this->cols[colIndex].colSize, 0);
	return s;
}

UnsafeArrayOpt<UTF8Char> DB::DBFReader::GetStr(UOSInt colIndex, UnsafeArray<UTF8Char> buff, UOSInt buffSize)
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

Data::Timestamp DB::DBFReader::GetTimestamp(UOSInt colIndex)
{
	if (!this->recordExist)
		return Data::Timestamp(0);
	if (colIndex >= this->colCnt)
		return Data::Timestamp(0);
	if (this->cols[colIndex].type != 'D')
	{
		return Data::Timestamp(0);
	}
	Char buff[5];
	Char *currPtr = (Char*)&this->recordData[this->cols[colIndex].colOfst];
	Data::DateTimeUtil::TimeValue tval;
	tval.hour = 0;
	tval.minute = 0;
	tval.second = 0;
	buff[0] = currPtr[0];
	buff[1] = currPtr[1];
	buff[2] = currPtr[2];
	buff[3] = currPtr[3];
	buff[4] = 0;
	tval.year = Data::DateTimeUtil::ParseYearStr(Text::CStringNN((const UTF8Char*)buff, 4));;
	buff[0] = currPtr[4];
	buff[1] = currPtr[5];
	buff[2] = 0;
	tval.month = Text::StrToUInt8Ch(buff);
	buff[0] = currPtr[6];
	buff[1] = currPtr[7];
	buff[2] = 0;
	tval.day = Text::StrToUInt8Ch(buff);
	return Data::Timestamp(Data::DateTimeUtil::TimeValue2Ticks(tval, 0, 0), 0);
}

Double DB::DBFReader::GetDblOrNAN(UOSInt colIndex)
{
	if (!this->recordExist)
		return NAN;
	if (colIndex >= this->colCnt)
		return NAN;
	Char buff[30];
	MemCopyNO(buff, &this->recordData[this->cols[colIndex].colOfst], this->cols[colIndex].colSize);
	buff[this->cols[colIndex].colSize] = 0;
	Text::StrRTrim(buff);
	return Text::StrToDoubleOrNANCh(buff);
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
	if (Text::StrCompareICaseCh(buff, "false") == 0)
		return false;
	else if (Text::StrCompareICaseCh(buff, "true") == 0)
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
		return Text::StrToInt32Ch(buff) != 0;
}

UOSInt DB::DBFReader::GetBinarySize(UOSInt colIndex)
{
	if (!this->recordExist)
		return 0;
	if (colIndex >= this->colCnt)
		return 0;
	return this->cols[colIndex].colSize;
}

UOSInt DB::DBFReader::GetBinary(UOSInt colIndex, UnsafeArray<UInt8> buff)
{
	if (!this->recordExist)
		return 0;
	if (colIndex >= this->colCnt)
		return 0;
	MemCopyNO(buff.Ptr(), &this->recordData[this->cols[colIndex].colOfst], this->cols[colIndex].colSize);
	return this->cols[colIndex].colSize;
}

Optional<Math::Geometry::Vector2D> DB::DBFReader::GetVector(UOSInt colIndex)
{
	return 0;
}

Bool DB::DBFReader::GetUUID(UOSInt colIndex, NN<Data::UUID> uuid)
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

UnsafeArrayOpt<UTF8Char> DB::DBFReader::GetName(UOSInt colIndex, UnsafeArray<UTF8Char> buff)
{
	if (colIndex >= this->colCnt)
		return 0;
	return this->cols[colIndex].name->ConcatTo(buff);
}

DB::DBUtil::ColType DB::DBFReader::GetColType(UOSInt colIndex, OptOut<UOSInt> colSize)
{
	return this->dbf->GetColumnType(colIndex, colSize);
}

Bool DB::DBFReader::GetColDef(UOSInt colIndex, NN<DB::ColDef> colDef)
{
	return this->dbf->GetColumnDef(colIndex, colDef);
}
