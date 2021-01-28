#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ByteTool.h"
#include "DB/ColDef.h"
#include "Math/Math.h"
#include "Math/Point3D.h"
#include "Math/TSPFile.h"
#include "Text/MyString.h"
#include "Text/MyStringFloat.h"
#include "Text/MyStringW.h"

Math::TSPFile::TSPFile(IO::IStreamData *fd) : DB::ReadingDB(fd->GetFullName())
{
	UInt8 hdr[8];
	fd->GetRealData(0, 8, hdr);

	if (*(Int64*)hdr == *(Int64*)"SmTS____")
	{
		this->rowCnt = (OSInt)((fd->GetDataSize() - 8) / 64);
		this->ptBuff = MemAlloc(UInt8, this->rowCnt * 64);
		this->rowSize = 64;
		this->fileType = 1;
		this->hdrBuff = 0;
		fd->GetRealData(8, this->rowCnt * 64, this->ptBuff);
	}
	else if (*(Int64*)hdr == *(Int64*)"SmTS___A")
	{
		this->rowCnt = (OSInt)((fd->GetDataSize() - 72) / 64);
		this->ptBuff = MemAlloc(UInt8, this->rowCnt * 64);
		this->rowSize = 64;
		this->fileType = 2;
		this->hdrBuff = MemAlloc(UInt8, 64);
		fd->GetRealData(8, 64, this->hdrBuff);
		fd->GetRealData(72, this->rowCnt * 64, this->ptBuff);
	}
	else if (*(Int64*)hdr == *(Int64*)"SmTS___B")
	{
		this->rowCnt = (OSInt)((fd->GetDataSize() - 72) / 128);
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

UOSInt Math::TSPFile::GetTableNames(Data::ArrayList<const UTF8Char*> *names)
{
	if (this->rowCnt > 0)
	{
		names->Add((const UTF8Char*)"Points");
		if (fileType == 2 || fileType == 3)
		{
			names->Add((const UTF8Char*)"StationSetup");
		}
		return 1;
	}
	else
	{
		return 0;
	}
}

DB::DBReader *Math::TSPFile::GetTableData(const UTF8Char *name, UOSInt maxCnt, void *ordering, void *condition)
{
	DB::DBReader *reader;
	if (name != 0 && Text::StrEquals(name, (const UTF8Char*)"StationSetup"))
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

void Math::TSPFile::GetErrorMsg(Text::StringBuilderUTF *str)
{
}

void Math::TSPFile::Reconnect()
{
}

UInt8 *Math::TSPFile::GetRowPtr(UOSInt row)
{
	if (row >= this->rowCnt)
		return 0;
	return &this->ptBuff[this->rowSize * row];
}

UInt8 *Math::TSPFile::GetHdrPtr()
{
	return this->hdrBuff;
}

UOSInt Math::TSPFile::GetRowCnt()
{
	return this->rowCnt;
}

UOSInt Math::TSPFile::GetRowSize()
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
		this->currRow = this->rowCnt;
		this->currRowPtr = 0;
	}
	else
	{
		this->currRow++;
		this->currRowPtr = this->file->GetRowPtr(this->currRow);
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

Bool Math::TSPReader::GetStr(UOSInt colIndex, Text::StringBuilderUTF *sb)
{
	UTF8Char sbuff[64];
	if (GetStr(colIndex, sbuff, sizeof(sbuff)))
	{
		sb->Append(sbuff);
		return true;
	}
	return false;
}

const WChar *Math::TSPReader::GetNewStr(UOSInt colIndex)
{
	WChar sbuff[64];
	if (GetStr(colIndex, sbuff))
	{
		return Text::StrCopyNew(sbuff);
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

DB::DBReader::DateErrType Math::TSPReader::GetDate(UOSInt colIndex, Data::DateTime *outVal)
{
	if (this->currRowPtr == 0)
		return DB::DBReader::DET_ERROR;
	if (colIndex == 2)
	{
		outVal->SetTicks(ReadInt64(&this->currRowPtr[48]));
		return DB::DBReader::DET_OK;
	}
	return DB::DBReader::DET_ERROR;
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

Math::Vector2D *Math::TSPReader::GetVector(UOSInt colIndex)
{
	if (this->currRowPtr == 0)
		return 0;
	if (colIndex != 1)
		return 0;
	Math::Point3D *pt;
	NEW_CLASS(pt, Math::Point3D(4326, ReadDouble(&this->currRowPtr[0]), ReadDouble(&this->currRowPtr[8]), ReadDouble(&this->currRowPtr[16])));
	return pt;
}

UTF8Char *Math::TSPReader::GetName(UOSInt colIndex, UTF8Char *buff)
{
	const UTF8Char *sptr = this->GetName(colIndex);
	if (sptr)
		return Text::StrConcat(buff, sptr);
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
		colDef->SetIsNotNull(true);
		colDef->SetColType(DB::DBUtil::CT_Int32);
		colDef->SetColSize(11);
		return true;
	case 1:
		colDef->SetColName(this->GetName(colIndex));
		colDef->SetIsNotNull(true);
		colDef->SetColType(DB::DBUtil::CT_Vector);
		colDef->SetColSize(0x7fffffff);
		return true;
	case 2:
		colDef->SetColName(this->GetName(colIndex));
		colDef->SetIsNotNull(true);
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
		colDef->SetIsNotNull(true);
		colDef->SetColType(DB::DBUtil::CT_Double);
		colDef->SetColSize(33);
		return true;
	default:
		return false;
	}
}

void Math::TSPReader::DelNewStr(const WChar *s)
{
	Text::StrDelNew(s);
}

const UTF8Char *Math::TSPReader::GetName(UOSInt colIndex)
{
	switch (colIndex)
	{
	case 0:
		return (const UTF8Char*)"AreaId";
	case 1:
		return (const UTF8Char*)"Shape";
	case 2:
		return (const UTF8Char*)"ScanTime";
	case 3:
		return (const UTF8Char*)"ResultX";
	case 4:
		return (const UTF8Char*)"ResultY";
	case 5:
		return (const UTF8Char*)"ResultZ";
	case 6:
		return (const UTF8Char*)"H-Angle";
	case 7:
		return (const UTF8Char*)"V-Angle";
	case 8:
		return (const UTF8Char*)"SlopeDistance";
	case 9:
		if (this->rowSize >= 128)
			return (const UTF8Char*)"RequestX";
		else
			return 0;
	case 10:
		if (this->rowSize >= 128)
			return (const UTF8Char*)"RequestY";
		else
			return 0;
	case 11:
		if (this->rowSize >= 128)
			return (const UTF8Char*)"RequestZ";
		else
			return 0;
	default:
		return 0;
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
	return Math::Double2Int32(GetDbl(colIndex));
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

Bool Math::TSPHReader::GetStr(UOSInt colIndex, Text::StringBuilderUTF *sb)
{
	if (this->currRow != 0)
		return false;
	Text::SBAppendF64(sb, GetDbl(colIndex));
	return true;
}

const WChar *Math::TSPHReader::GetNewStr(UOSInt colIndex)
{
	WChar sbuff[64];
	if (GetStr(colIndex, sbuff))
	{
		return Text::StrCopyNew(sbuff);
	}
	return 0;
}
UTF8Char *Math::TSPHReader::GetStr(UOSInt colIndex, UTF8Char *buff, UOSInt buffSize)
{
	if (this->currRow != 0)
		return 0;
	return Text::StrDouble(buff, GetDbl(colIndex));
}

DB::DBReader::DateErrType Math::TSPHReader::GetDate(UOSInt colIndex, Data::DateTime *outVal)
{
	return DB::DBReader::DET_ERROR;
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

Math::Vector2D *Math::TSPHReader::GetVector(UOSInt colIndex)
{
	return 0;
}

UTF8Char *Math::TSPHReader::GetName(UOSInt colIndex, UTF8Char *buff)
{
	const UTF8Char *sptr = this->GetName(colIndex);
	if (sptr)
		return Text::StrConcat(buff, sptr);
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
		colDef->SetIsNotNull(true);
		colDef->SetColType(DB::DBUtil::CT_Double);
		colDef->SetColSize(33);
		return true;
	default:
		return false;
	}
}

void Math::TSPHReader::DelNewStr(const WChar *s)
{
	Text::StrDelNew(s);
}

const UTF8Char *Math::TSPHReader::GetName(UOSInt colIndex)
{
	switch (colIndex)
	{
	case 0:
		return (const UTF8Char*)"InstX";
	case 1:
		return (const UTF8Char*)"InstY";
	case 2:
		return (const UTF8Char*)"InstZ";
	case 3:
		return (const UTF8Char*)"FromAngle";
	case 4:
		return (const UTF8Char*)"ToAngle";
	case 5:
		return (const UTF8Char*)"Radius";
	case 6:
		return (const UTF8Char*)"HAngle";
	case 7:
		return (const UTF8Char*)"Azimuth";
	default:
		return 0;
	}
}
