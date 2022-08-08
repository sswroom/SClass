#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ByteTool.h"
#include "DB/ColDef.h"
#include "Math/Math.h"
#include "Math/TSPFile.h"
#include "Math/Geometry/PointZ.h"
#include "Text/MyString.h"
#include "Text/MyStringFloat.h"
#include "Text/MyStringW.h"

Math::TSPFile::TSPFile(IO::IStreamData *fd) : DB::ReadingDB(fd->GetFullName())
{
	UInt8 hdr[8];
	fd->GetRealData(0, 8, hdr);

	if (*(Int64*)hdr == *(Int64*)"SmTS____")
	{
		this->rowCnt = (UOSInt)((fd->GetDataSize() - 8) / 64);
		this->ptBuff = MemAlloc(UInt8, this->rowCnt * 64);
		this->rowSize = 64;
		this->fileType = 1;
		this->hdrBuff = 0;
		fd->GetRealData(8, this->rowCnt * 64, this->ptBuff);
	}
	else if (*(Int64*)hdr == *(Int64*)"SmTS___A")
	{
		this->rowCnt = (UOSInt)((fd->GetDataSize() - 72) / 64);
		this->ptBuff = MemAlloc(UInt8, this->rowCnt * 64);
		this->rowSize = 64;
		this->fileType = 2;
		this->hdrBuff = MemAlloc(UInt8, 64);
		fd->GetRealData(8, 64, this->hdrBuff);
		fd->GetRealData(72, this->rowCnt * 64, this->ptBuff);
	}
	else if (*(Int64*)hdr == *(Int64*)"SmTS___B")
	{
		this->rowCnt = (UOSInt)((fd->GetDataSize() - 72) / 128);
		this->ptBuff = MemAlloc(UInt8, this->rowCnt * 128);
		this->rowSize = 128;
		this->fileType = 3;
		this->hdrBuff = MemAlloc(UInt8, 64);
		fd->GetRealData(8, 64, this->hdrBuff);
		fd->GetRealData(72, this->rowCnt * 128, this->ptBuff);
	}
	else
	{
		this->rowCnt = 0;
		this->ptBuff = 0;
		this->rowSize = 0;
		this->fileType = 0;
		this->hdrBuff = 0;
		return;
	}
}

Math::TSPFile::~TSPFile()
{
	if (this->ptBuff)
	{
		MemFree(this->ptBuff);
		this->ptBuff = 0;
	}
	if (this->hdrBuff)
	{
		MemFree(this->hdrBuff);
		this->hdrBuff = 0;
	}
}

UOSInt Math::TSPFile::QueryTableNames(Text::CString schemaName, Data::ArrayList<Text::String*> *names)
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

DB::DBReader *Math::TSPFile::QueryTableData(Text::CString schemaName, Text::CString tableName, Data::ArrayList<Text::String*> *columnNames, UOSInt ofst, UOSInt maxCnt, Text::CString ordering, Data::QueryConditions *condition)
{
	DB::DBReader *reader;
	if (tableName.v != 0 && tableName.Equals(UTF8STRC("StationSetup")))
	{
		NEW_CLASS(reader, Math::TSPHReader(this));
	}
	else
	{
		NEW_CLASS(reader, Math::TSPReader(this));
	}
	return reader;
}

void Math::TSPFile::CloseReader(DB::DBReader *r)
{
	Math::TSPReader *reader = (Math::TSPReader*)r;
	DEL_CLASS(reader);
}

void Math::TSPFile::GetErrorMsg(Text::StringBuilderUTF8 *str)
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

UInt8 *Math::TSPFile::GetHdrPtr() const
{
	return this->hdrBuff;
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

WChar *Math::TSPReader::GetStr(UOSInt colIndex, WChar *buff)
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

Bool Math::TSPReader::GetStr(UOSInt colIndex, Text::StringBuilderUTF8 *sb)
{
	UTF8Char sbuff[64];
	UTF8Char *sptr;
	if ((sptr = GetStr(colIndex, sbuff, sizeof(sbuff))) != 0)
	{
		sb->AppendC(sbuff, (UOSInt)(sptr - sbuff));
		return true;
	}
	return false;
}

Text::String *Math::TSPReader::GetNewStr(UOSInt colIndex)
{
	UTF8Char sbuff[64];
	UTF8Char *sptr;
	if ((sptr = GetStr(colIndex, sbuff, sizeof(sbuff))) != 0)
	{
		return Text::String::NewP(sbuff, sptr);
	}
	return 0;
}
UTF8Char *Math::TSPReader::GetStr(UOSInt colIndex, UTF8Char *buff, UOSInt buffSize)
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
		return Data::Timestamp(0, 0);
	if (colIndex == 2)
	{
		return Data::Timestamp(ReadInt64(&this->currRowPtr[48]), 0, Data::DateTimeUtil::GetLocalTzQhr());
	}
	return Data::Timestamp(0, 0);
}

Double Math::TSPReader::GetDbl(UOSInt colIndex)
{
	if (this->currRowPtr == 0)
		return 0;
	if (colIndex == 0)
	{
		return ReadInt32(&this->currRowPtr[56]);
	}
	else if (colIndex == 1)
	{
		return 0;
	}
	else if (colIndex == 2)
	{
		return 0;
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
	return 0;
}

Bool Math::TSPReader::GetBool(UOSInt colIndex)
{
	return GetDbl(colIndex) != 0;
}

UOSInt Math::TSPReader::GetBinarySize(UOSInt colIndex)
{
	return 0;
}

UOSInt Math::TSPReader::GetBinary(UOSInt colIndex, UInt8 *buff)
{
	return 0;
}

Math::Geometry::Vector2D *Math::TSPReader::GetVector(UOSInt colIndex)
{
	if (this->currRowPtr == 0)
		return 0;
	if (colIndex != 1)
		return 0;
	Math::Geometry::PointZ *pt;
	NEW_CLASS(pt, Math::Geometry::PointZ(4326, ReadDouble(&this->currRowPtr[0]), ReadDouble(&this->currRowPtr[8]), ReadDouble(&this->currRowPtr[16])));
	return pt;
}

Bool Math::TSPReader::GetUUID(UOSInt colIndex, Data::UUID *uuid)
{
	return false;
}

UTF8Char *Math::TSPReader::GetName(UOSInt colIndex, UTF8Char *buff)
{
	Text::CString cstr = this->GetName(colIndex);
	if (cstr.v)
		return cstr.ConcatTo(buff);
	return 0;
}

Bool Math::TSPReader::IsNull(UOSInt colIndex)
{
	return false;
}

DB::DBUtil::ColType Math::TSPReader::GetColType(UOSInt colIndex, UOSInt *colSize)
{
	switch (colIndex)
	{
	case 0:
		if (colSize)
			*colSize = 11;
		return DB::DBUtil::CT_Int32;
	case 1:
		if (colSize)
			*colSize = 0x7fffffff;
		return DB::DBUtil::CT_Vector;
	case 2:
		if (colSize)
			*colSize = 18;
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
		if (colSize)
		{
			*colSize = 33;
		}
		return DB::DBUtil::CT_Double;
	default:
		return DB::DBUtil::CT_Unknown;
	}
}

Bool Math::TSPReader::GetColDef(UOSInt colIndex, DB::ColDef *colDef)
{
	switch (colIndex)
	{
	case 0:
		colDef->SetColName(this->GetName(colIndex));
		colDef->SetNotNull(true);
		colDef->SetColType(DB::DBUtil::CT_Int32);
		colDef->SetColSize(11);
		return true;
	case 1:
		colDef->SetColName(this->GetName(colIndex));
		colDef->SetNotNull(true);
		colDef->SetColType(DB::DBUtil::CT_Vector);
		colDef->SetColSize(0x7fffffff);
		return true;
	case 2:
		colDef->SetColName(this->GetName(colIndex));
		colDef->SetNotNull(true);
		colDef->SetColType(DB::DBUtil::CT_DateTime);
		colDef->SetColSize(18);
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
		colDef->SetColName(this->GetName(colIndex));
		colDef->SetNotNull(true);
		colDef->SetColType(DB::DBUtil::CT_Double);
		colDef->SetColSize(33);
		return true;
	default:
		return false;
	}
}

Text::CString Math::TSPReader::GetName(UOSInt colIndex)
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
		if (this->rowSize >= 128)
			return CSTR("RequestX");
		else
			return CSTR_NULL;
	case 10:
		if (this->rowSize >= 128)
			return CSTR("RequestY");
		else
			return CSTR_NULL;
	case 11:
		if (this->rowSize >= 128)
			return CSTR("RequestZ");
		else
			return CSTR_NULL;
	default:
		return CSTR_NULL;
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
	return Double2Int32(GetDbl(colIndex));
}

Int64 Math::TSPHReader::GetInt64(UOSInt colIndex)
{
	return (Int64)GetDbl(colIndex);
}

WChar *Math::TSPHReader::GetStr(UOSInt colIndex, WChar *buff)
{
	if (this->currRow != 0)
		return 0;
	return Text::StrDouble(buff, GetDbl(colIndex));
}

Bool Math::TSPHReader::GetStr(UOSInt colIndex, Text::StringBuilderUTF8 *sb)
{
	if (this->currRow != 0)
		return false;
	Text::SBAppendF64(sb, GetDbl(colIndex));
	return true;
}

Text::String *Math::TSPHReader::GetNewStr(UOSInt colIndex)
{
	UTF8Char sbuff[64];
	UTF8Char *sptr;
	if ((sptr = GetStr(colIndex, sbuff, sizeof(sbuff))) != 0)
	{
		return Text::String::NewP(sbuff, sptr);
	}
	return 0;
}
UTF8Char *Math::TSPHReader::GetStr(UOSInt colIndex, UTF8Char *buff, UOSInt buffSize)
{
	if (this->currRow != 0)
		return 0;
	return Text::StrDouble(buff, GetDbl(colIndex));
}

Data::Timestamp Math::TSPHReader::GetTimestamp(UOSInt colIndex)
{
	return Data::Timestamp(0, 0);
}

Double Math::TSPHReader::GetDbl(UOSInt colIndex)
{
	if (this->currRow != 0)
		return 0;
	if (colIndex >= 8)
	{
		return 0;
	}
	else
	{
		return ReadDouble(&this->file->GetHdrPtr()[colIndex * 8]);;
	}
}

Bool Math::TSPHReader::GetBool(UOSInt colIndex)
{
	return GetDbl(colIndex) != 0;
}

UOSInt Math::TSPHReader::GetBinarySize(UOSInt colIndex)
{
	return 0;
}

UOSInt Math::TSPHReader::GetBinary(UOSInt colIndex, UInt8 *buff)
{
	return 0;
}

Math::Geometry::Vector2D *Math::TSPHReader::GetVector(UOSInt colIndex)
{
	return 0;
}

Bool Math::TSPHReader::GetUUID(UOSInt colIndex, Data::UUID *uuid)
{
	return false;
}

UTF8Char *Math::TSPHReader::GetName(UOSInt colIndex, UTF8Char *buff)
{
	Text::CString cstr = this->GetName(colIndex);
	if (cstr.v)
		return cstr.ConcatTo(buff);
	return 0;
}

Bool Math::TSPHReader::IsNull(UOSInt colIndex)
{
	return false;
}

DB::DBUtil::ColType Math::TSPHReader::GetColType(UOSInt colIndex, UOSInt *colSize)
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
		if (colSize)
		{
			*colSize = 33;
		}
		return DB::DBUtil::CT_Double;
	default:
		return DB::DBUtil::CT_Unknown;
	}
}

Bool Math::TSPHReader::GetColDef(UOSInt colIndex, DB::ColDef *colDef)
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
		colDef->SetColName(this->GetName(colIndex));
		colDef->SetNotNull(true);
		colDef->SetColType(DB::DBUtil::CT_Double);
		colDef->SetColSize(33);
		return true;
	default:
		return false;
	}
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
		return CSTR_NULL;
	}
}
