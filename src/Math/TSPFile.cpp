#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ByteTool.h"
#include "DB/ColDef.h"
#include "DB/TableDef.h"
#include "Math/Math.h"
#include "Math/TSPFile.h"
#include "Math/Geometry/PointZ.h"
#include "Text/MyString.h"
#include "Text/MyStringFloat.h"
#include "Text/MyStringW.h"

Math::TSPFile::TSPFile(NN<IO::StreamData> fd) : DB::ReadingDB(fd->GetFullName())
{
	UInt8 hdr[8];
	fd->GetRealData(0, 8, BYTEARR(hdr));

	if (*(Int64*)hdr == *(Int64*)"SmTS____")
	{
		this->rowCnt = (UOSInt)((fd->GetDataSize() - 8) / 64);
		this->ptBuff.ChangeSizeAndClear(this->rowCnt * 64);
		this->rowSize = 64;
		this->fileType = 1;
		fd->GetRealData(8, this->rowCnt * 64, this->ptBuff);
	}
	else if (*(Int64*)hdr == *(Int64*)"SmTS___A")
	{
		this->rowCnt = (UOSInt)((fd->GetDataSize() - 72) / 64);
		this->ptBuff.ChangeSizeAndClear(this->rowCnt * 64);
		this->rowSize = 64;
		this->fileType = 2;
		this->hdrBuff.ChangeSizeAndClear(64);
		fd->GetRealData(8, 64, this->hdrBuff);
		fd->GetRealData(72, this->rowCnt * 64, this->ptBuff);
	}
	else if (*(Int64*)hdr == *(Int64*)"SmTS___B")
	{
		this->rowCnt = (UOSInt)((fd->GetDataSize() - 72) / 128);
		this->ptBuff.ChangeSizeAndClear(this->rowCnt * 128);
		this->rowSize = 128;
		this->fileType = 3;
		this->hdrBuff.ChangeSizeAndClear(64);
		fd->GetRealData(8, 64, this->hdrBuff);
		fd->GetRealData(72, this->rowCnt * 128, this->ptBuff);
	}
	else
	{
		this->rowCnt = 0;
		this->rowSize = 0;
		this->fileType = 0;
		return;
	}
}

Math::TSPFile::~TSPFile()
{
}

UOSInt Math::TSPFile::QueryTableNames(Text::CString schemaName, NN<Data::ArrayListStringNN> names)
{
	if (schemaName.leng != 0)
		return 0;
	if (this->rowCnt > 0)
	{
		names->Add(Text::String::New(UTF8STRC("Points")));
		if (fileType == 2 || fileType == 3)
		{
			names->Add(Text::String::New(UTF8STRC("StationSetup")));
			return 2;
		}
		else
		{
			return 1;
		}
	}
	else
	{
		return 0;
	}
}

Optional<DB::DBReader> Math::TSPFile::QueryTableData(Text::CString schemaName, Text::CStringNN tableName, Optional<Data::ArrayListStringNN> columnNames, UOSInt ofst, UOSInt maxCnt, Text::CString ordering, Optional<Data::QueryConditions> condition)
{
	NN<DB::DBReader> reader;
	if (tableName.Equals(UTF8STRC("StationSetup")))
	{
		NEW_CLASSNN(reader, Math::TSPHReader(this));
	}
	else
	{
		NEW_CLASSNN(reader, Math::TSPReader(this));
	}
	return reader;
}

Optional<DB::TableDef> Math::TSPFile::GetTableDef(Text::CString schemaName, Text::CStringNN tableName)
{
	UOSInt i;
	UOSInt j;
	DB::TableDef *tab;
	NN<DB::ColDef> col;
	NEW_CLASS(tab, DB::TableDef(schemaName, tableName));
	if (tableName.Equals(UTF8STRC("StationSetup")))
	{
		i = 0;
		j = 8;
		while (i < j)
		{
			NEW_CLASSNN(col, DB::ColDef(Text::String::NewEmpty()));
			TSPHReader::GetColDefV(i, col);
			tab->AddCol(col);
			i++;
		}
	}
	else
	{
		i = 0;
		if (this->rowSize >= 128)
		{
			j = 12;
		}
		else
		{
			j = 9;
		}
		while (i < j)
		{
			NEW_CLASSNN(col, DB::ColDef(Text::String::NewEmpty()));
			TSPReader::GetColDefV(i, col, this->rowSize);
			tab->AddCol(col);
			i++;
		}
	}
	return tab;
}

void Math::TSPFile::CloseReader(NN<DB::DBReader> r)
{
	Math::TSPReader *reader = (Math::TSPReader*)r.Ptr();
	DEL_CLASS(reader);
}

void Math::TSPFile::GetLastErrorMsg(NN<Text::StringBuilderUTF8> str)
{
}

void Math::TSPFile::Reconnect()
{
}

UInt8 *Math::TSPFile::GetRowPtr(UOSInt row) const
{
	if (row >= this->rowCnt)
		return 0;
	return &this->ptBuff[this->rowSize * row];
}

UnsafeArray<UInt8> Math::TSPFile::GetHdrPtr() const
{
	return this->hdrBuff.Arr();
}

UOSInt Math::TSPFile::GetRowCnt() const
{
	return this->rowCnt;
}

UOSInt Math::TSPFile::GetRowSize() const
{
	return this->rowSize;
}

Math::TSPReader::TSPReader(Math::TSPFile *file)
{
	this->file = file;
	this->currRowPtr = 0;
	this->currRow = -1;
	this->rowCnt = file->GetRowCnt();
	this->rowSize = file->GetRowSize();
}
Math::TSPReader::~TSPReader()
{
}

Bool Math::TSPReader::ReadNext()
{
	if (this->currRow >= (OSInt)this->rowCnt - 1)
	{
		this->currRow = (OSInt)this->rowCnt;
		this->currRowPtr = 0;
	}
	else
	{
		this->currRow++;
		this->currRowPtr = this->file->GetRowPtr((UOSInt)this->currRow);
	}
	return this->currRowPtr != 0;
}

UOSInt Math::TSPReader::ColCount()
{
	if (this->rowSize >= 128)
	{
		return 12;
	}
	else
	{
		return 9;
	}
}

OSInt Math::TSPReader::GetRowChanged()
{
	return -1;
}

Int32 Math::TSPReader::GetInt32(UOSInt colIndex)
{
	if (this->currRowPtr == 0)
		return 0;
	if (colIndex == 0)
	{
		return ReadInt32(&this->currRowPtr[56]);
	}
	return 0;
}

Int64 Math::TSPReader::GetInt64(UOSInt colIndex)
{
	if (this->currRowPtr == 0)
		return 0;
	if (colIndex == 0)
	{
		return ReadInt32(&this->currRowPtr[56]);
	}
	return 0;
}

UnsafeArrayOpt<WChar> Math::TSPReader::GetStr(UOSInt colIndex, UnsafeArray<WChar> buff)
{
	if (this->currRowPtr == 0)
		return 0;
	if (colIndex == 0)
	{
		return Text::StrInt32(buff, ReadInt32(&this->currRowPtr[56]));
	}
	else if (colIndex == 1)
	{
		return buff;
	}
	else if (colIndex == 2)
	{
		Char sbuff[32];
		Data::DateTime dt;
		dt.SetTicks(ReadInt64(&this->currRowPtr[48]));
		dt.ToLocalTime();
		dt.ToString(sbuff, "yyyy-MM-dd HH:mm:ss.fff");
		return Text::StrConcatASCII(buff, sbuff);
	}
	else if (colIndex == 3)
	{
		return Text::StrDoubleW(buff, ReadDouble(&this->currRowPtr[0]));
	}
	else if (colIndex == 4)
	{
		return Text::StrDoubleW(buff, ReadDouble(&this->currRowPtr[8]));
	}
	else if (colIndex == 5)
	{
		return Text::StrDoubleW(buff, ReadDouble(&this->currRowPtr[16]));
	}
	else if (colIndex == 6)
	{
		return Text::StrDoubleW(buff, ReadDouble(&this->currRowPtr[24]));
	}
	else if (colIndex == 7)
	{
		return Text::StrDoubleW(buff, ReadDouble(&this->currRowPtr[32]));
	}
	else if (colIndex == 8)
	{
		return Text::StrDoubleW(buff, ReadDouble(&this->currRowPtr[40]));
	}
	else if (this->rowSize >= 128)
	{
		if (colIndex == 9)
		{
			return Text::StrDoubleW(buff, ReadDouble(&this->currRowPtr[64]));
		}
		else if (colIndex == 10)
		{
			return Text::StrDoubleW(buff, ReadDouble(&this->currRowPtr[72]));
		}
		else if (colIndex == 11)
		{
			return Text::StrDoubleW(buff, ReadDouble(&this->currRowPtr[80]));
		}
	}
	return 0;
}

Bool Math::TSPReader::GetStr(UOSInt colIndex, NN<Text::StringBuilderUTF8> sb)
{
	UTF8Char sbuff[64];
	UnsafeArray<UTF8Char> sptr;
	if (GetStr(colIndex, sbuff, sizeof(sbuff)).SetTo(sptr))
	{
		sb->AppendC(sbuff, (UOSInt)(sptr - sbuff));
		return true;
	}
	return false;
}

Optional<Text::String> Math::TSPReader::GetNewStr(UOSInt colIndex)
{
	UTF8Char sbuff[64];
	UnsafeArray<UTF8Char> sptr;
	if (GetStr(colIndex, sbuff, sizeof(sbuff)).SetTo(sptr))
	{
		return Text::String::NewP(sbuff, sptr);
	}
	return 0;
}
UnsafeArrayOpt<UTF8Char> Math::TSPReader::GetStr(UOSInt colIndex, UnsafeArray<UTF8Char> buff, UOSInt buffSize)
{
	if (this->currRowPtr == 0)
		return 0;
	if (colIndex == 0)
	{
		return Text::StrInt32(buff, ReadInt32(&this->currRowPtr[56]));
	}
	else if (colIndex == 1)
	{
		return buff;
	}
	else if (colIndex == 2)
	{
		Data::DateTime dt;
		dt.SetTicks(ReadInt64(&this->currRowPtr[48]));
		dt.ToLocalTime();
		return dt.ToString(buff, "yyyy-MM-dd HH:mm:ss.fff");
	}
	else if (colIndex == 3)
	{
		return Text::StrDouble(buff, ReadDouble(&this->currRowPtr[0]));
	}
	else if (colIndex == 4)
	{
		return Text::StrDouble(buff, ReadDouble(&this->currRowPtr[8]));
	}
	else if (colIndex == 5)
	{
		return Text::StrDouble(buff, ReadDouble(&this->currRowPtr[16]));
	}
	else if (colIndex == 6)
	{
		return Text::StrDouble(buff, ReadDouble(&this->currRowPtr[24]));
	}
	else if (colIndex == 7)
	{
		return Text::StrDouble(buff, ReadDouble(&this->currRowPtr[32]));
	}
	else if (colIndex == 8)
	{
		return Text::StrDouble(buff, ReadDouble(&this->currRowPtr[40]));
	}
	else if (this->rowSize >= 128)
	{
		if (colIndex == 9)
		{
			return Text::StrDouble(buff, ReadDouble(&this->currRowPtr[64]));
		}
		else if (colIndex == 10)
		{
			return Text::StrDouble(buff, ReadDouble(&this->currRowPtr[72]));
		}
		else if (colIndex == 11)
		{
			return Text::StrDouble(buff, ReadDouble(&this->currRowPtr[80]));
		}
	}
	return 0;
}

Data::Timestamp Math::TSPReader::GetTimestamp(UOSInt colIndex)
{
	if (this->currRowPtr == 0)
		return Data::Timestamp(0);
	if (colIndex == 2)
	{
		return Data::Timestamp(ReadInt64(&this->currRowPtr[48]), Data::DateTimeUtil::GetLocalTzQhr());
	}
	return Data::Timestamp(0);
}

Double Math::TSPReader::GetDblOrNAN(UOSInt colIndex)
{
	if (this->currRowPtr == 0)
		return NAN;
	if (colIndex == 0)
	{
		return ReadInt32(&this->currRowPtr[56]);
	}
	else if (colIndex == 1)
	{
		return NAN;
	}
	else if (colIndex == 2)
	{
		return NAN;
	}
	else if (colIndex == 3)
	{
		return ReadDouble(&this->currRowPtr[0]);
	}
	else if (colIndex == 4)
	{
		return ReadDouble(&this->currRowPtr[8]);
	}
	else if (colIndex == 5)
	{
		return ReadDouble(&this->currRowPtr[16]);
	}
	else if (colIndex == 6)
	{
		return ReadDouble(&this->currRowPtr[24]);
	}
	else if (colIndex == 7)
	{
		return ReadDouble(&this->currRowPtr[32]);
	}
	else if (colIndex == 8)
	{
		return ReadDouble(&this->currRowPtr[40]);
	}
	else if (this->rowSize >= 128)
	{
		if (colIndex == 9)
		{
			return ReadDouble(&this->currRowPtr[64]);
		}
		else if (colIndex == 10)
		{
			return ReadDouble(&this->currRowPtr[72]);
		}
		else if (colIndex == 11)
		{
			return ReadDouble(&this->currRowPtr[80]);
		}
	}
	return NAN;
}

Bool Math::TSPReader::GetBool(UOSInt colIndex)
{
	Double v = GetDblOrNAN(colIndex);
	return !Math::IsNAN(v) && v != 0;
}

UOSInt Math::TSPReader::GetBinarySize(UOSInt colIndex)
{
	return 0;
}

UOSInt Math::TSPReader::GetBinary(UOSInt colIndex, UnsafeArray<UInt8> buff)
{
	return 0;
}

Optional<Math::Geometry::Vector2D> Math::TSPReader::GetVector(UOSInt colIndex)
{
	if (this->currRowPtr == 0)
		return 0;
	if (colIndex != 1)
		return 0;
	Math::Geometry::PointZ *pt;
	NEW_CLASS(pt, Math::Geometry::PointZ(4326, ReadDouble(&this->currRowPtr[0]), ReadDouble(&this->currRowPtr[8]), ReadDouble(&this->currRowPtr[16])));
	return pt;
}

Bool Math::TSPReader::GetUUID(UOSInt colIndex, NN<Data::UUID> uuid)
{
	return false;
}

UnsafeArrayOpt<UTF8Char> Math::TSPReader::GetName(UOSInt colIndex, UnsafeArray<UTF8Char> buff)
{
	Text::CStringNN cstr;
	if (this->GetName(colIndex, this->rowSize).SetTo(cstr))
		return cstr.ConcatTo(buff);
	return 0;
}

Bool Math::TSPReader::IsNull(UOSInt colIndex)
{
	return false;
}

DB::DBUtil::ColType Math::TSPReader::GetColType(UOSInt colIndex, OptOut<UOSInt> colSize)
{
	switch (colIndex)
	{
	case 0:
		colSize.Set(11);
		return DB::DBUtil::CT_Int32;
	case 1:
		colSize.Set(0x7fffffff);
		return DB::DBUtil::CT_Vector;
	case 2:
		colSize.Set(18);
		return DB::DBUtil::CT_DateTime;
	case 3:
	case 4:
	case 5:
	case 6:
	case 7:
	case 8:
	case 9:
	case 10:
	case 11:
		colSize.Set(33);
		return DB::DBUtil::CT_Double;
	default:
		return DB::DBUtil::CT_Unknown;
	}
}

Bool Math::TSPReader::GetColDef(UOSInt colIndex, NN<DB::ColDef> colDef)
{
	return GetColDefV(colIndex, colDef, this->rowSize);
}

Text::CString Math::TSPReader::GetName(UOSInt colIndex, UOSInt rowSize)
{
	switch (colIndex)
	{
	case 0:
		return CSTR("AreaId");
	case 1:
		return CSTR("Shape");
	case 2:
		return CSTR("ScanTime");
	case 3:
		return CSTR("ResultX");
	case 4:
		return CSTR("ResultY");
	case 5:
		return CSTR("ResultZ");
	case 6:
		return CSTR("H-Angle");
	case 7:
		return CSTR("V-Angle");
	case 8:
		return CSTR("SlopeDistance");
	case 9:
		if (rowSize >= 128)
			return CSTR("RequestX");
		else
			return nullptr;
	case 10:
		if (rowSize >= 128)
			return CSTR("RequestY");
		else
			return nullptr;
	case 11:
		if (rowSize >= 128)
			return CSTR("RequestZ");
		else
			return nullptr;
	default:
		return nullptr;
	}
}

Bool Math::TSPReader::GetColDefV(UOSInt colIndex, NN<DB::ColDef> colDef, UOSInt rowSize)
{
	switch (colIndex)
	{
	case 0:
		colDef->SetColName(GetName(colIndex, rowSize).OrEmpty());
		colDef->SetNotNull(true);
		colDef->SetColType(DB::DBUtil::CT_Int32);
		colDef->SetColSize(11);
		return true;
	case 1:
		colDef->SetColName(GetName(colIndex, rowSize).OrEmpty());
		colDef->SetNotNull(true);
		colDef->SetColType(DB::DBUtil::CT_Vector);
		colDef->SetColSize(0x7fffffff);
		return true;
	case 2:
		colDef->SetColName(GetName(colIndex, rowSize).OrEmpty());
		colDef->SetNotNull(true);
		colDef->SetColType(DB::DBUtil::CT_DateTime);
		colDef->SetColSize(3);
		return true;
	case 3:
	case 4:
	case 5:
	case 6:
	case 7:
	case 8:
	case 9:
	case 10:
	case 11:
		colDef->SetColName(GetName(colIndex, rowSize).OrEmpty());
		colDef->SetNotNull(true);
		colDef->SetColType(DB::DBUtil::CT_Double);
		colDef->SetColSize(33);
		return true;
	default:
		return false;
	}
}

Math::TSPHReader::TSPHReader(Math::TSPFile *file)
{
	this->file = file;
	this->currRow = -1;
}
Math::TSPHReader::~TSPHReader()
{
}

Bool Math::TSPHReader::ReadNext()
{
	if (this->currRow >= 1)
	{
		return false;
	}
	else
	{
		this->currRow++;
		return true;
	}
}

UOSInt Math::TSPHReader::ColCount()
{
	return 8;
}

OSInt Math::TSPHReader::GetRowChanged()
{
	return -1;
}

Int32 Math::TSPHReader::GetInt32(UOSInt colIndex)
{
	return Double2Int32(GetDblOrNAN(colIndex));
}

Int64 Math::TSPHReader::GetInt64(UOSInt colIndex)
{
	return (Int64)GetDblOrNAN(colIndex);
}

UnsafeArrayOpt<WChar> Math::TSPHReader::GetStr(UOSInt colIndex, UnsafeArray<WChar> buff)
{
	if (this->currRow != 0)
		return 0;
	return Text::StrDoubleW(buff, GetDblOrNAN(colIndex));
}

Bool Math::TSPHReader::GetStr(UOSInt colIndex, NN<Text::StringBuilderUTF8> sb)
{
	if (this->currRow != 0)
		return false;
	Text::SBAppendF64(sb, GetDblOrNAN(colIndex));
	return true;
}

Optional<Text::String> Math::TSPHReader::GetNewStr(UOSInt colIndex)
{
	UTF8Char sbuff[64];
	UnsafeArray<UTF8Char> sptr;
	if (GetStr(colIndex, sbuff, sizeof(sbuff)).SetTo(sptr))
	{
		return Text::String::NewP(sbuff, sptr);
	}
	return 0;
}
UnsafeArrayOpt<UTF8Char> Math::TSPHReader::GetStr(UOSInt colIndex, UnsafeArray<UTF8Char> buff, UOSInt buffSize)
{
	if (this->currRow != 0)
		return 0;
	return Text::StrDouble(buff, GetDblOrNAN(colIndex));
}

Data::Timestamp Math::TSPHReader::GetTimestamp(UOSInt colIndex)
{
	return Data::Timestamp(0);
}

Double Math::TSPHReader::GetDblOrNAN(UOSInt colIndex)
{
	if (this->currRow != 0)
		return NAN;
	if (colIndex >= 8)
	{
		return NAN;
	}
	else
	{
		return ReadDouble(&this->file->GetHdrPtr()[colIndex * 8]);;
	}
}

Bool Math::TSPHReader::GetBool(UOSInt colIndex)
{
	Double v = GetDblOrNAN(colIndex);
	return !Math::IsNAN(v) && v != 0;
}

UOSInt Math::TSPHReader::GetBinarySize(UOSInt colIndex)
{
	return 0;
}

UOSInt Math::TSPHReader::GetBinary(UOSInt colIndex, UnsafeArray<UInt8> buff)
{
	return 0;
}

Optional<Math::Geometry::Vector2D> Math::TSPHReader::GetVector(UOSInt colIndex)
{
	return 0;
}

Bool Math::TSPHReader::GetUUID(UOSInt colIndex, NN<Data::UUID> uuid)
{
	return false;
}

UnsafeArrayOpt<UTF8Char> Math::TSPHReader::GetName(UOSInt colIndex, UnsafeArray<UTF8Char> buff)
{
	Text::CStringNN cstr;
	if (GetName(colIndex).SetTo(cstr))
		return cstr.ConcatTo(buff);
	return 0;
}

Bool Math::TSPHReader::IsNull(UOSInt colIndex)
{
	return false;
}

DB::DBUtil::ColType Math::TSPHReader::GetColType(UOSInt colIndex, OptOut<UOSInt> colSize)
{
	switch (colIndex)
	{
	case 0:
	case 1:
	case 2:
	case 3:
	case 4:
	case 5:
	case 6:
	case 7:
		colSize.Set(33);
		return DB::DBUtil::CT_Double;
	default:
		return DB::DBUtil::CT_Unknown;
	}
}

Bool Math::TSPHReader::GetColDef(UOSInt colIndex, NN<DB::ColDef> colDef)
{
	return GetColDefV(colIndex, colDef);
}

Text::CString Math::TSPHReader::GetName(UOSInt colIndex)
{
	switch (colIndex)
	{
	case 0:
		return CSTR("InstX");
	case 1:
		return CSTR("InstY");
	case 2:
		return CSTR("InstZ");
	case 3:
		return CSTR("FromAngle");
	case 4:
		return CSTR("ToAngle");
	case 5:
		return CSTR("Radius");
	case 6:
		return CSTR("HAngle");
	case 7:
		return CSTR("Azimuth");
	default:
		return nullptr;
	}
}

Bool Math::TSPHReader::GetColDefV(UOSInt colIndex, NN<DB::ColDef> colDef)
{
	switch (colIndex)
	{
	case 0:
	case 1:
	case 2:
	case 3:
	case 4:
	case 5:
	case 6:
	case 7:
		colDef->SetColName(GetName(colIndex).OrEmpty());
		colDef->SetNotNull(true);
		colDef->SetColType(DB::DBUtil::CT_Double);
		colDef->SetColSize(33);
		return true;
	default:
		return false;
	}
}
