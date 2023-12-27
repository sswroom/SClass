#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ByteTool.h"
#include "DB/ColDef.h"
#include "DB/DBReader.h"
#include "DB/TableDef.h"
#include "Math/Math.h"
#include "Media/HTRecFile.h"
#include "Text/Encoding.h"
#include "Text/MyString.h"
#include "Text/MyStringFloat.h"
#include "Text/MyStringW.h"

Media::HTRecFile::HTRecReader::HTRecReader(Media::HTRecFile *file, Bool setting)
{
	this->file = file;
	this->setting = setting;
	this->nextRow = 0;
	this->recCount = this->file->GetRecCount();
}

Media::HTRecFile::HTRecReader::~HTRecReader()
{
}

Bool Media::HTRecFile::HTRecReader::ReadNext()
{
	if (this->setting)
	{
		if (this->nextRow > 18)
			return false;
		this->nextRow++;
		if (this->nextRow > 18)
			return false;
		return true;
	}
	else
	{
		if (this->nextRow >= this->recCount)
			return false;
		this->nextRow++;
		if (this->nextRow >= this->recCount)
			return false;
		return true;
	}
}

UOSInt Media::HTRecFile::HTRecReader::ColCount()
{
	if (this->setting)
	{
		return 2;
	}
	else
	{
		return 4;
	}
}

OSInt Media::HTRecFile::HTRecReader::GetRowChanged()
{
	return -1;
}

Int32 Media::HTRecFile::HTRecReader::GetInt32(UOSInt colIndex)
{
	UOSInt currRow = this->nextRow - 1;
	if (this->setting)
	{
		if (colIndex == 1)
		{
			if (currRow == 1)
			{
				return this->file->GetAddress();
			}
			else if (currRow == 5)
			{
				return (Int32)this->file->GetTotalRec();
			}
			else if (currRow == 6)
			{
				return (Int32)this->file->GetRecInterval();
			}
			else if (currRow == 9)
			{
				return Double2Int32(this->file->GetTempAlarmL());
			}
			else if (currRow == 10)
			{
				return Double2Int32(this->file->GetTempAlarmH());
			}
			else if (currRow == 11)
			{
				return Double2Int32(this->file->GetHumiAlarmL());
			}
			else if (currRow == 12)
			{
				return Double2Int32(this->file->GetHumiAlarmH());
			}
			else if (currRow == 15)
			{
				return (Int32)this->file->GetRecCount();
			}

			return 0;
		}
		else
		{
			return 0;
		}
	}
	else
	{
		if (nextRow > this->recCount)
			return 0;
		if (colIndex == 0)
		{
			return (Int32)nextRow;
		}
		else if (colIndex == 1)
		{
			return 0;
		}
		else if (colIndex == 2)
		{
			return Double2Int32(GetDbl(colIndex));
		}
		else if (colIndex == 3)
		{
			return Double2Int32(GetDbl(colIndex));
		}
		else
		{
			return 0;
		}
	}
}

Int64 Media::HTRecFile::HTRecReader::GetInt64(UOSInt colIndex)
{
	return GetInt32(colIndex);
}

WChar *Media::HTRecFile::HTRecReader::GetStr(UOSInt colIndex, WChar *buff)
{
	UTF8Char sbuff[40];
	if (GetStr(colIndex, sbuff, sizeof(sbuff)) == 0)
		return 0;
	return Text::StrUTF8_WChar(buff, sbuff, 0);
}

Bool Media::HTRecFile::HTRecReader::GetStr(UOSInt colIndex, NotNullPtr<Text::StringBuilderUTF8> sb)
{
	UTF8Char sbuff[40];
	UTF8Char *sptr;
	if ((sptr = GetStr(colIndex, sbuff, sizeof(sbuff))) == 0)
		return false;
	sb->AppendC(sbuff, (UOSInt)(sptr - sbuff));
	return true;
}

Optional<Text::String> Media::HTRecFile::HTRecReader::GetNewStr(UOSInt colIndex)
{
	UTF8Char sbuff[64];
	UTF8Char *sptr;
	if ((sptr = GetStr(colIndex, sbuff, sizeof(sbuff))) == 0)
		return 0;
	return Text::String::New(sbuff, (UOSInt)(sptr - sbuff));
}

UTF8Char *Media::HTRecFile::HTRecReader::GetStr(UOSInt colIndex, UTF8Char *buff, UOSInt buffSize)
{
	if (this->setting)
	{
		if (colIndex == 0)
		{
			switch (this->nextRow - 1)
			{
			case 0:
				return Text::StrConcatCS(buff, UTF8STRC("Download Time"), buffSize);
			case 1:
				return Text::StrConcatCS(buff, UTF8STRC("Address"), buffSize);
			case 2:
				return Text::StrConcatCS(buff, UTF8STRC("Serial Number"), buffSize);
			case 3:
				return Text::StrConcatCS(buff, UTF8STRC("Test Name"), buffSize);
			case 4:
				return Text::StrConcatCS(buff, UTF8STRC("Setting Time"), buffSize);
			case 5:
				return Text::StrConcatCS(buff, UTF8STRC("Total Records"), buffSize);
			case 6:
				return Text::StrConcatCS(buff, UTF8STRC("Record Interval"), buffSize);
			case 7:
				return Text::StrConcatCS(buff, UTF8STRC("Work LED"), buffSize);
			case 8:
				return Text::StrConcatCS(buff, UTF8STRC("Immediately/Manual"), buffSize);
			case 9:
				return Text::StrConcatCS(buff, UTF8STRC("Temperature Alarm L"), buffSize);
			case 10:
				return Text::StrConcatCS(buff, UTF8STRC("Temperature Alarm H"), buffSize);
			case 11:
				return Text::StrConcatCS(buff, UTF8STRC("Humidity Alarm L"), buffSize);
			case 12:
				return Text::StrConcatCS(buff, UTF8STRC("Humidity Alarm H"), buffSize);
			case 13:
				return Text::StrConcatCS(buff, UTF8STRC("Alarm Mode"), buffSize);
			case 14:
				return Text::StrConcatCS(buff, UTF8STRC("Start Time"), buffSize);
			case 15:
				return Text::StrConcatCS(buff, UTF8STRC("Test Records"), buffSize);
			case 16:
				return Text::StrConcatCS(buff, UTF8STRC("Is TEMP Alarm"), buffSize);
			case 17:
				return Text::StrConcatCS(buff, UTF8STRC("Is HUMI Alarm"), buffSize);
			}
		}
		else if (colIndex == 1)
		{
			switch (nextRow - 1)
			{
			case 0:
				return this->file->GetDownloadTime().ToString(buff, "yyyy-MM-dd HH:mm:ss");
			case 1:
				return Text::StrInt32(buff, this->file->GetAddress());
			case 2:

				return this->file->GetSerialNo(buff);
			case 3:
				return this->file->GetTestName(buff);
			case 4:
				return this->file->GetSettingTime().ToString(buff, "yyyy-MM-dd HH:mm:ss");
			case 5:
				return Text::StrUOSInt(buff, this->file->GetTotalRec());
			case 6:
				return Text::StrUInt32(buff, this->file->GetRecInterval());
			case 7:
				*buff = 0;
				return buff;
//				return Text::StrConcat(buff, L"Work LED");
			case 8:
				*buff = 0;
				return buff;
//				return Text::StrConcat(buff, L"Immediately/Manual");
			case 9:
				return Text::StrDoubleFmt(buff, this->file->GetTempAlarmL(), "0.0");
			case 10:
				return Text::StrDoubleFmt(buff, this->file->GetTempAlarmH(), "0.0");
			case 11:
				return Text::StrDoubleFmt(buff, this->file->GetHumiAlarmL(), "0.0");
			case 12:
				return Text::StrDoubleFmt(buff, this->file->GetHumiAlarmH(), "0.0");
			case 13:
				*buff = 0;
				return buff;
				//return Text::StrConcat(buff, L"Alarm Mode");
			case 14:
				return this->file->GetStartTime().ToString(buff, "yyyy-MM-dd HH:mm:ss");
			case 15:
				return Text::StrUOSInt(buff, this->file->GetRecCount());
			case 16:
				*buff = 0;
				return buff;
//				return Text::StrConcat(buff, L"Is TEMP Alarm");
			case 17:
				*buff = 0;
				return buff;
//				return Text::StrConcat(buff, L"Is HUMI Alarm");
			}
		}
		return 0;
	}
	else
	{
		if (nextRow > this->recCount)
			return 0;
		if (colIndex == 0)
		{
			return Text::StrUOSInt(buff, this->nextRow);
		}
		else if (colIndex == 1)
		{
			return GetTimestamp(colIndex).ToString(buff, "yyyy-MM-dd HH:mm:ss");
		}
		else if (colIndex == 2)
		{
			return Text::StrDoubleFmt(buff, GetDbl(colIndex), "0.0");
		}
		else if (colIndex == 3)
		{
			return Text::StrDoubleFmt(buff, GetDbl(colIndex), "0.0");
		}
		else
		{
			return 0;
		}
	}
}

Data::Timestamp Media::HTRecFile::HTRecReader::GetTimestamp(UOSInt colIndex)
{
	UOSInt currRow = this->nextRow - 1;
	if (this->setting)
	{
		if (colIndex == 1)
		{
			if (currRow == 0)
			{
				return this->file->GetDownloadTime();
			}
			else if (currRow == 4)
			{
				return this->file->GetSettingTime();
			}
			else if (currRow == 14)
			{
				return this->file->GetStartTime();
			}
		}
		return Data::Timestamp(0);
	}
	else
	{
		if (currRow >= this->recCount)
			return Data::Timestamp(0);
		if (colIndex == 1)
		{
			Data::Timestamp ts = this->file->GetAdjStartTime().AddMS((OSInt)currRow * this->file->GetAdjRecInterval());
			return ts;
		}
		else
		{
			return Data::Timestamp(0);
		}
	}
}

Double Media::HTRecFile::HTRecReader::GetDbl(UOSInt colIndex)
{
	UOSInt currRow = this->nextRow - 1;
	if (this->setting)
	{
		if (colIndex != 1)
			return 0;
		if (currRow == 1)
		{
			return this->file->GetAddress();
		}
		else if (currRow == 5)
		{
			return UOSInt2Double(this->file->GetTotalRec());
		}
		else if (currRow == 6)
		{
			return this->file->GetRecInterval();
		}
		else if (currRow == 9)
		{
			return this->file->GetTempAlarmL();
		}
		else if (currRow == 10)
		{
			return this->file->GetTempAlarmH();
		}
		else if (currRow == 11)
		{
			return this->file->GetHumiAlarmL();
		}
		else if (currRow == 12)
		{
			return this->file->GetHumiAlarmH();
		}
		else if (currRow == 15)
		{
			return UOSInt2Double(this->file->GetRecCount());
		}

		return 0;
	}
	else
	{
		if (currRow >= this->recCount)
			return 0;
		if (colIndex == 0)
		{
			return UOSInt2Double(currRow + 1);
		}
		else if (colIndex == 1)
		{
			return 0;
		}
		else if (colIndex == 2)
		{
			const UInt8 *recBuff = this->file->GetRecBuff();
			UOSInt i = currRow * 3;
			return (recBuff[i] | ((recBuff[i + 2] & 0xf) << 8)) * 0.1 - 40.0;
		}
		else if (colIndex == 3)
		{
			const UInt8 *recBuff = this->file->GetRecBuff();
			UOSInt i = currRow * 3;
			return (recBuff[i + 1] | ((recBuff[i + 2] & 0xf0) << 4)) * 0.1;
		}
		else
		{
			return 0;
		}
	}
}

Bool Media::HTRecFile::HTRecReader::GetBool(UOSInt colIndex)
{
	return false;
}

UOSInt Media::HTRecFile::HTRecReader::GetBinarySize(UOSInt colIndex)
{
	return 0;
}

UOSInt Media::HTRecFile::HTRecReader::GetBinary(UOSInt colIndex, UInt8 *buff)
{
	return 0;
}

Math::Geometry::Vector2D *Media::HTRecFile::HTRecReader::GetVector(UOSInt colIndex)
{
	return 0;
}

Bool Media::HTRecFile::HTRecReader::GetUUID(UOSInt colIndex, NotNullPtr<Data::UUID> uuid)
{
	return false;
}

UTF8Char *Media::HTRecFile::HTRecReader::GetName(UOSInt colIndex, UTF8Char *buff)
{
	Text::CString s = GetName(colIndex, this->setting);
	if (s.v == 0)
		return 0;
	return s.ConcatTo(buff);
}

Bool Media::HTRecFile::HTRecReader::IsNull(UOSInt colIndex)
{
	return false;
}

DB::DBUtil::ColType Media::HTRecFile::HTRecReader::GetColType(UOSInt colIndex, OptOut<UOSInt> colSize)
{
	if (this->setting)
	{
		switch (colIndex)
		{
		case 0:
			colSize.Set(32);
			return DB::DBUtil::CT_VarUTF8Char;
		case 1:
			colSize.Set(32);
			return DB::DBUtil::CT_VarUTF8Char;
		}
	}
	else
	{
		switch (colIndex)
		{
		case 0:
			colSize.Set(11);
			return DB::DBUtil::CT_Int32;
		case 1:
			colSize.Set(0);
			return DB::DBUtil::CT_DateTime;
		case 2:
			colSize.Set(32);
			return DB::DBUtil::CT_Double;
		case 3:
			colSize.Set(32);
			return DB::DBUtil::CT_Double;
		}
	}
	return DB::DBUtil::CT_Unknown;
}

Bool Media::HTRecFile::HTRecReader::GetColDef(UOSInt colIndex, NotNullPtr<DB::ColDef> colDef)
{
	return GetColDefV(colIndex, colDef, this->setting);
}

Text::CString Media::HTRecFile::HTRecReader::GetName(UOSInt colIndex, Bool setting)
{
	if (setting)
	{
		switch (colIndex)
		{
		case 0:
			return CSTR("name");
		case 1:
			return CSTR("value");
		}
	}
	else
	{
		switch (colIndex)
		{
		case 0:
			return CSTR("recno");
		case 1:
			return CSTR("time");
		case 2:
			return CSTR("temperature");
		case 3:
			return CSTR("rh");
		}
	}
	return CSTR_NULL;
}

Bool Media::HTRecFile::HTRecReader::GetColDefV(UOSInt colIndex, NotNullPtr<DB::ColDef> colDef, Bool setting)
{
	colDef->SetNotNull(true);
	colDef->SetPK(colIndex == 0);
	colDef->SetAutoIncNone();
	colDef->SetDefVal(CSTR_NULL);
	colDef->SetAttr(CSTR_NULL);
	if (setting)
	{
		switch (colIndex)
		{
		case 0:
			colDef->SetColName(GetName(colIndex, setting));
			colDef->SetColType(DB::DBUtil::CT_VarUTF8Char);
			colDef->SetColSize(32);
			colDef->SetColDP(0);
			return true;
		case 1:
			colDef->SetColName(GetName(colIndex, setting));
			colDef->SetColType(DB::DBUtil::CT_VarUTF8Char);
			colDef->SetColSize(32);
			colDef->SetColDP(0);
			return true;
		}
	}
	else
	{
		switch (colIndex)
		{
		case 0:
			colDef->SetColName(GetName(colIndex, setting));
			colDef->SetColType(DB::DBUtil::CT_Int32);
			colDef->SetColSize(11);
			colDef->SetColDP(0);
			colDef->SetAutoInc(DB::ColDef::AutoIncType::Default, 1, 1);
			return true;
		case 1:
			colDef->SetColName(GetName(colIndex, setting));
			colDef->SetColType(DB::DBUtil::CT_DateTime);
			colDef->SetColSize(0);
			colDef->SetColDP(0);
			return true;
		case 2:
			colDef->SetColName(GetName(colIndex, setting));
			colDef->SetColType(DB::DBUtil::CT_Double);
			colDef->SetColSize(32);
			colDef->SetColDP(1);
			return true;
		case 3:
			colDef->SetColName(GetName(colIndex, setting));
			colDef->SetColType(DB::DBUtil::CT_Double);
			colDef->SetColSize(32);
			colDef->SetColDP(1);
			return true;
		}
	}
	return false;
}

Media::HTRecFile::HTRecFile(NotNullPtr<IO::StreamData> stmData) : DB::ReadingDB(stmData->GetFullName())
{
	UInt8 buff[96];
	this->time1TS = 0;
	this->time2TS = 0;
	this->time3TS = 0;
	this->serialNo = 0;
	this->testName = 0;
	if (stmData->GetRealData(0, 96, BYTEARR(buff)) != 96)
	{
		return;
	}
	if (*(Int32*)&buff[0] != *(Int32*)"$#\" ")
	{
		return;
	}
	UInt32 recCnt = ReadUInt16(&buff[83]);
	if (stmData->GetDataSize() != 96 + recCnt * 3)
		return;
	UTF8Char sbuff[37];
	const WChar *wptr;
	UTF8Char *dptr;
	this->time1TS = ReadUInt32(&buff[0x04]); //Server Recv Time
	this->address = buff[8];
	Text::StrConcatC(sbuff, &buff[9], 10);
	this->serialNo = Text::StrCopyNew(sbuff).Ptr();
	wptr = (const WChar*)&buff[19];
	dptr = sbuff;
	OSInt charLeft = 36;
	while (true)
	{
		if (charLeft-- <= 0)
		{
			*dptr = 0;
			break;
		}
		if ((*dptr++ = (UTF8Char)*wptr++) != 0)
		{
		}
		else
		{
			break;
		}
	}
	this->testName = Text::StrCopyNew(sbuff).Ptr();
	this->totalRecords = ReadUInt16(&buff[0x3b]);
	this->recInterval = ReadUInt16(&buff[0x3d]);
	this->tempAlarmL = ReadUInt16(&buff[0x41]) - 400;
	this->tempAlarmH = ReadUInt16(&buff[0x43]) - 400;
	this->rhAlarmL = ReadUInt16(&buff[0x45]);
	this->rhAlarmH = ReadUInt16(&buff[0x47]);
	this->time2TS = ReadUInt32(&buff[0x4a]); //Setting Time
	this->time3TS = ReadUInt32(&buff[0x4f]); //Start Time
	this->recCount = recCnt;
	
	if (this->time3TS + (Int64)(this->recCount * this->recInterval) > this->time1TS)
	{
		Int64 tick1 = this->time2TS * 1000;
		Int64 tick2 = this->time3TS + (Int64)(this->recCount * this->recInterval) * 1000 - tick1;
		tick1 = this->time1TS * 1000 - tick1;
		this->adjRecInterval = (UInt32)(recInterval * 1000 * (UInt64)tick1 / (UInt64)tick2); 
		this->adjStTimeTicks = this->time1TS * 1000 - (OSInt)this->recCount * this->adjRecInterval;
	}
	else
	{
		this->adjStTimeTicks = this->time3TS * 1000;
		this->adjRecInterval = this->recInterval * 1000;
	}
	this->recBuff.ChangeSize(recCnt * 3);
	stmData->GetRealData(96, recCnt * 3, this->recBuff);
}

Media::HTRecFile::~HTRecFile()
{
	SDEL_TEXT(this->serialNo);
	SDEL_TEXT(this->testName);
}

UOSInt Media::HTRecFile::QueryTableNames(Text::CString schemaName, NotNullPtr<Data::ArrayListStringNN> names)
{
	if (this->recBuff.IsNull() || schemaName.leng != 0)
	{
		return 0;
	}
	else
	{
		names->Add(Text::String::New(UTF8STRC("Setting")));
		names->Add(Text::String::New(UTF8STRC("Records")));
		return 2;
	}
}

DB::DBReader *Media::HTRecFile::QueryTableData(Text::CString schemaName, Text::CString tableName, Data::ArrayListStringNN *columnNames, UOSInt ofst, UOSInt maxCnt, Text::CString ordering, Data::QueryConditions *condition)
{
	if (tableName.Equals(UTF8STRC("Setting")))
	{
		HTRecReader *reader;
		NEW_CLASS(reader, HTRecReader(this, true));
		return reader;
	}
	else if (tableName.Equals(UTF8STRC("Records")))
	{
		HTRecReader *reader;
		NEW_CLASS(reader, HTRecReader(this, false));
		return reader;
	}
	return 0;
}

DB::TableDef *Media::HTRecFile::GetTableDef(Text::CString schemaName, Text::CString tableName)
{
	UOSInt i = 0;
	UOSInt j;
	NotNullPtr<DB::ColDef> col;
	DB::TableDef *tab = 0;
	if (tableName.Equals(UTF8STRC("Setting")))
	{
		NEW_CLASS(tab, DB::TableDef(CSTR_NULL, tableName));
		j = 2;
		while (i < j)
		{
			NEW_CLASSNN(col, DB::ColDef(Text::String::NewEmpty()));
			Media::HTRecFile::HTRecReader::GetColDefV(i, col, true);
			tab->AddCol(col);
			i++;
		}
	}
	else if (tableName.Equals(UTF8STRC("Records")))
	{
		NEW_CLASS(tab, DB::TableDef(CSTR_NULL, tableName));
		j = 4;
		while (i < j)
		{
			NEW_CLASSNN(col, DB::ColDef(Text::String::NewEmpty()));
			Media::HTRecFile::HTRecReader::GetColDefV(i, col, false);
			tab->AddCol(col);
			i++;
		}
	}
	return tab;
}

void Media::HTRecFile::CloseReader(NotNullPtr<DB::DBReader> r)
{
	HTRecReader *reader = (HTRecReader*)r.Ptr();
	DEL_CLASS(reader);
}

void Media::HTRecFile::GetLastErrorMsg(NotNullPtr<Text::StringBuilderUTF8> str)
{
}

void Media::HTRecFile::Reconnect()
{
}

Data::Timestamp Media::HTRecFile::GetDownloadTime()
{
	return Data::Timestamp(this->time1TS * 1000, Data::DateTimeUtil::GetLocalTzQhr());
}

Int32 Media::HTRecFile::GetAddress()
{
	return this->address;
}

Data::Timestamp Media::HTRecFile::GetSettingTime()
{
	return Data::Timestamp(this->time2TS * 1000, Data::DateTimeUtil::GetLocalTzQhr());
}

UOSInt Media::HTRecFile::GetTotalRec()
{
	return this->totalRecords;
}

UInt32 Media::HTRecFile::GetRecInterval()
{
	return this->recInterval;
}

Double Media::HTRecFile::GetTempAlarmL()
{
	return this->tempAlarmL * 0.1;
}

Double Media::HTRecFile::GetTempAlarmH()
{
	return this->tempAlarmH * 0.1;
}

Double Media::HTRecFile::GetHumiAlarmL()
{
	return this->rhAlarmL * 0.1;
}

Double Media::HTRecFile::GetHumiAlarmH()
{
	return this->rhAlarmH * 0.1;
}

Data::Timestamp Media::HTRecFile::GetStartTime()
{
	return Data::Timestamp(this->time3TS * 1000, Data::DateTimeUtil::GetLocalTzQhr());
}

UOSInt Media::HTRecFile::GetRecCount()
{
	return this->recCount;
}

UTF8Char *Media::HTRecFile::GetSerialNo(UTF8Char *sbuff)
{
	return Text::StrConcat(sbuff, this->serialNo);
}

UTF8Char *Media::HTRecFile::GetTestName(UTF8Char *sbuff)
{
	return Text::StrConcat(sbuff, this->testName);
}

Data::Timestamp Media::HTRecFile::GetAdjStartTime()
{
	return Data::Timestamp(this->adjStTimeTicks, Data::DateTimeUtil::GetLocalTzQhr());
}

UInt32 Media::HTRecFile::GetAdjRecInterval()
{
	return this->adjRecInterval;
}

const UInt8 *Media::HTRecFile::GetRecBuff()
{
	return this->recBuff.Ptr();
}
