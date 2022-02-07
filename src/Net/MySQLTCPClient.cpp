#include "Stdafx.h"
#include "Crypto/Hash/SHA1.h"
#include "Data/ByteTool.h"
#include "Data/DateTime.h"
#include "DB/ColDef.h"
#include "DB/DBUtil.h"
#include "IO/OS.h"
#include "Net/MySQLTCPClient.h"
#include "Net/MySQLUtil.h"
#include "Net/SocketUtil.h"
#include "Sync/MutexUsage.h"
#include "Sync/Thread.h"
#include "Text/MyString.h"
#include "Text/MyStringFloat.h"
#include "Text/MyStringW.h"

#define CLIVERSION "1.0.0"
#define BUFFSIZE 65536

#include <stdio.h>
//#define VERBOSE
#if defined(VERBOSE)
#include "Text/StringBuilderUTF8.h"
#include <stdio.h>

#ifdef __WIN32
#include <windows.h>
#define printf(...) { Char dbgbuff[8192]; sprintf(dbgbuff, __VA_ARGS__ ); OutputDebugStringA(dbgbuff);}
#endif

#endif

class MySQLTCPReader : public DB::DBReader
{
private:
	typedef struct
	{
		Text::String *name;
		Text::String *defValues;
		UInt32 colLen;
		UInt16 charSet;
		UInt16 flags;
		Net::MySQLUtil::MySQLType colType;
		DB::DBUtil::ColType dbColType;
		UInt8 decimals;
	} ColumnDef;

	Data::ArrayList<ColumnDef*> *cols;
	UOSInt colCount;
	OSInt rowChanged;
	Text::String **currRow;
	Text::String **nextRow;
	Bool nextRowReady;
	Sync::Event *rowEvt;
	Sync::Event *nextRowEvt;
	Sync::MutexUsage *mutUsage;
public:
	MySQLTCPReader(Sync::MutexUsage *mutUsage)
	{
		this->mutUsage = mutUsage;
		NEW_CLASS(this->cols, Data::ArrayList<ColumnDef*>());
		this->rowChanged = -1;
		this->currRow = 0;
		this->nextRow = 0;
		this->colCount = 0;
		this->nextRowReady = false;
		NEW_CLASS(this->nextRowEvt, Sync::Event(true));
		NEW_CLASS(this->rowEvt, Sync::Event(true));
	}

	virtual ~MySQLTCPReader()
	{
		ColumnDef *col;
		while (this->ReadNext())
		{

		}
		UOSInt i = this->cols->GetCount();
		while (i-- > 0)
		{
			col = this->cols->GetItem(i);
			col->name->Release();
			SDEL_STRING(col->defValues);
			MemFree(col); 
		}
		DEL_CLASS(this->cols);
		DEL_CLASS(this->rowEvt);
		DEL_CLASS(this->nextRowEvt);
		DEL_CLASS(this->mutUsage);
	}

	virtual Bool ReadNext()
	{
		UOSInt i;
		if (this->currRow)
		{
			i = this->colCount;
			while (i-- > 0)
			{
				SDEL_STRING(this->currRow[i]);
			}
			MemFree(this->currRow);
			this->currRow = 0;
		}
		while (!this->nextRowReady)
		{
			this->rowEvt->Wait(1000);
		}
		if (this->nextRow)
		{
			this->currRow = this->nextRow;
			this->nextRowReady = false;
			this->nextRowEvt->Set();
			return true;
		}
		else
		{
			return false;
		}
	}

	virtual UOSInt ColCount()
	{
		return this->colCount;
	}

	virtual OSInt GetRowChanged()
	{
		return this->rowChanged;
	}

	virtual Int32 GetInt32(UOSInt colIndex)
	{
		if (this->currRow == 0)
		{
			return 0;
		}
		if (colIndex >= this->colCount)
		{
			return 0;
		}
		if (this->currRow[colIndex] == 0)
		{
			return 0;
		}
		return Text::StrToInt32(this->currRow[colIndex]->v);
	}

	virtual Int64 GetInt64(UOSInt colIndex)
	{
		if (this->currRow == 0)
		{
			return 0;
		}
		if (colIndex >= this->colCount)
		{
			return 0;
		}
		if (this->currRow[colIndex] == 0)
		{
			return 0;
		}
		return Text::StrToInt64(this->currRow[colIndex]->v);
	}

	virtual WChar *GetStr(UOSInt colIndex, WChar *buff)
	{
		if (this->currRow == 0)
		{
			return 0;
		}
		if (colIndex >= this->colCount)
		{
			return 0;
		}
		if (this->currRow[colIndex] == 0)
		{
			return 0;
		}
		return Text::StrUTF8_WChar(buff, this->currRow[colIndex]->v, 0);
	}

	virtual Bool GetStr(UOSInt colIndex, Text::StringBuilderUTF8 *sb)
	{
		if (this->currRow == 0)
		{
			return false;
		}
		if (colIndex >= this->colCount)
		{
			return false;
		}
		if (this->currRow[colIndex] == 0)
		{
			return false;
		}
		sb->AppendC(this->currRow[colIndex]->v, this->currRow[colIndex]->leng);
		return true;
	}

	virtual Text::String *GetNewStr(UOSInt colIndex)
	{
		if (this->currRow == 0)
		{
			return 0;
		}
		if (colIndex >= this->colCount)
		{
			return 0;
		}
		if (this->currRow[colIndex] == 0)
		{
			return 0;
		}
		return this->currRow[colIndex]->Clone();
	}

	virtual UTF8Char *GetStr(UOSInt colIndex, UTF8Char *buff, UOSInt buffSize)
	{
		if (this->currRow == 0)
		{
			return 0;
		}
		if (colIndex >= this->colCount)
		{
			return 0;
		}
		if (this->currRow[colIndex] == 0)
		{
			return 0;
		}
		return this->currRow[colIndex]->ConcatToS(buff, buffSize);
	}

	virtual DateErrType GetDate(UOSInt colIndex, Data::DateTime *outVal)
	{
		if (this->currRow == 0)
		{
			return DET_NULL;
		}
		if (colIndex >= this->colCount)
		{
			return DET_NULL;
		}
		if (this->currRow[colIndex] == 0)
		{
			return DET_NULL;
		}
		if (outVal->SetValue(this->currRow[colIndex]->v, this->currRow[colIndex]->leng))
		{
			return DET_OK;
		}
		else
		{
			return DET_ERROR;
		}
	}

	virtual Double GetDbl(UOSInt colIndex)
	{
		if (this->currRow == 0)
		{
			return 0;
		}
		if (colIndex >= this->colCount)
		{
			return 0;
		}
		if (this->currRow[colIndex] == 0)
		{
			return 0;
		}
		return Text::StrToDouble(this->currRow[colIndex]->v);
	}

	virtual Bool GetBool(UOSInt colIndex)
	{
		if (this->currRow == 0)
		{
			return false;
		}
		if (colIndex >= this->colCount)
		{
			return false;
		}
		if (this->currRow[colIndex] == 0)
		{
			return false;
		}
		return Text::StrToInt32(this->currRow[colIndex]->v) != 0;
	}

	virtual UOSInt GetBinarySize(UOSInt colIndex)
	{
		/////////////////////////////
		return 0;
	}

	virtual UOSInt GetBinary(UOSInt UOSInt, UInt8 *buff)
	{
		/////////////////////////////
		return 0;
	}

	virtual Math::Vector2D *GetVector(UOSInt colIndex)
	{
		/////////////////////////////
		return 0;
	}

	virtual Bool GetUUID(UOSInt colIndex, Data::UUID *uuid)
	{
		/////////////////////////////
		return false;
	}

	virtual Bool IsNull(UOSInt colIndex)
	{
		if (this->currRow == 0)
		{
			return true;
		}
		if (colIndex >= this->colCount)
		{
			return true;
		}
		return this->currRow[colIndex] == 0;
	}

	virtual UTF8Char *GetName(UOSInt colIndex, UTF8Char *buff)
	{
		ColumnDef *col = this->cols->GetItem(colIndex);
		if (col)
		{
			return Text::StrConcat(buff, col->name->v);
		}
		return 0;
	}

	virtual DB::DBUtil::ColType GetColType(UOSInt colIndex, UOSInt *colSize)
	{
		ColumnDef *col = this->cols->GetItem(colIndex);
		if (col)
		{
			*colSize = col->colLen;
			return col->dbColType;
		}
		return DB::DBUtil::CT_Unknown;
	}

	virtual Bool GetColDef(UOSInt colIndex, DB::ColDef *colDef)
	{
		ColumnDef *col = this->cols->GetItem(colIndex);
		if (col)
		{
			colDef->SetColName(col->name);
			colDef->SetColDP(col->decimals);
			colDef->SetColSize(col->colLen);
			colDef->SetColType(Net::MySQLUtil::MySQLType2ColType(col->colType));
			return true;
		}
		return false;
	}

	void SetRowChanged(Int64 val)
	{
		this->rowChanged = (OSInt)val;
		this->nextRowReady = true;
	}

	void AddColumnDef41(const UInt8 *colDef, UOSInt buffSize)
	{
		UInt64 v;
		ColumnDef *col = MemAlloc(ColumnDef, 1);
		const UInt8 *colEnd = colDef + buffSize;
		colDef = Net::MySQLUtil::ReadLenencInt(colDef, &v); //catalog
		colDef += v;
		colDef = Net::MySQLUtil::ReadLenencInt(colDef, &v); //schema
		colDef += v;
		colDef = Net::MySQLUtil::ReadLenencInt(colDef, &v); //table
		colDef += v;
		colDef = Net::MySQLUtil::ReadLenencInt(colDef, &v); //org_table
		colDef += v;
		colDef = Net::MySQLUtil::ReadLenencInt(colDef, &v); //name
		col->name = Text::String::New(colDef, (UOSInt)v);
		colDef += v;
		colDef = Net::MySQLUtil::ReadLenencInt(colDef, &v); //org_name
		colDef += v;
		if (colDef[0] != 12)
		{
			col->name->Release();
			MemFree(col);
			return;
		}
		colDef += 1;
		col->charSet = ReadUInt16(&colDef[0]);
		col->colLen = ReadUInt32(&colDef[2]);
		col->colType = (Net::MySQLUtil::MySQLType)colDef[6];
		col->dbColType = Net::MySQLUtil::MySQLType2ColType(col->colType);
		col->flags = ReadUInt16(&colDef[7]);
		col->decimals = colDef[9];
		colDef += 12;
		col->defValues = 0;
		if (colDef < colEnd)
		{
			colDef = Net::MySQLUtil::ReadLenencInt(colDef, &v); //catalog
			if ((colDef + v) <= colEnd)
			{
				col->defValues = Text::String::New(colDef, (UOSInt)v);
			}
		}
		this->cols->Add(col);
		this->colCount++;
	}

	void AddRowData(const UInt8 *rowData, UOSInt dataSize)
	{
		Text::String **row = MemAlloc(Text::String *, this->colCount);
		UOSInt i = 0;
		UOSInt j = this->colCount;
		UInt64 v;
		while (i < j)
		{
			if (rowData[0] == 0xfb)
			{
				row[i] = 0;
				rowData++;
			}
			else
			{
				rowData = Net::MySQLUtil::ReadLenencInt(rowData, &v);
				row[i] = Text::String::New(rowData, (UOSInt)v);
				rowData += v;
			}
			i++;
		}
		while (this->nextRowReady)
		{
			this->nextRowEvt->Wait(1000);
		}
		this->nextRow = row;
		this->nextRowReady = true;
		this->rowEvt->Set();
	}

	void EndData()
	{
		if (this->nextRow == 0 && this->nextRowReady)
		{
			return;
		}
		while (this->nextRowReady)
		{
			this->nextRowEvt->Wait(1000);
		}
		this->nextRow = 0;
		this->nextRowReady = true;
		this->rowEvt->Set();
	}
};


class MySQLTCPBinaryReader : public DB::DBReader
{
private:
	typedef struct
	{
		Text::String *name;
		Text::String *defValues;
		UInt32 colLen;
		UInt16 charSet;
		UInt16 flags;
		Net::MySQLUtil::MySQLType colType;
		DB::DBUtil::ColType dbColType;
		UInt8 decimals;
	} ColumnDef;

	typedef struct
	{
		UOSInt len;
		UOSInt ofst;
		Bool isNull;
	} RowColumn;

	typedef struct
	{
		UInt8 *rowBuff;
		UOSInt rowBuffCapacity;
		RowColumn *cols;
	} RowData;	

	Data::ArrayList<ColumnDef*> *cols;
	Net::MySQLUtil::MySQLType *colTypes;
	UOSInt colCount;
	OSInt rowChanged;
	RowData *currRow;
	RowData *nextRow;
	Bool nextRowReady;
	Sync::Event *rowEvt;
	Sync::Event *nextRowEvt;
	Sync::MutexUsage *mutUsage;
	UInt32 stmtId;
public:
	MySQLTCPBinaryReader(Sync::MutexUsage *mutUsage)
	{
		this->mutUsage = mutUsage;
		NEW_CLASS(this->cols, Data::ArrayList<ColumnDef*>());
		this->colTypes = 0;
		this->rowChanged = -1;
		this->currRow = 0;
		this->nextRow = 0;
		this->colCount = 0;
		this->nextRowReady = false;
		this->stmtId = 0;
		NEW_CLASS(this->nextRowEvt, Sync::Event(true));
		NEW_CLASS(this->rowEvt, Sync::Event(true));
	}

	virtual ~MySQLTCPBinaryReader()
	{
		ColumnDef *col;
		while (this->ReadNext())
		{

		}
		UOSInt i = this->cols->GetCount();
		while (i-- > 0)
		{
			col = this->cols->GetItem(i);
			col->name->Release();
			SDEL_STRING(col->defValues);
			MemFree(col); 
		}
		DEL_CLASS(this->cols);
		if (this->nextRow)
		{
			MemFree(this->nextRow->cols);
			MemFree(this->nextRow->rowBuff);
			MemFree(this->nextRow);
			this->nextRow = 0;
		}
		if (this->currRow)
		{
			MemFree(this->currRow->cols);
			MemFree(this->currRow->rowBuff);
			MemFree(this->currRow);
			this->currRow = 0;
		}
		DEL_CLASS(this->rowEvt);
		DEL_CLASS(this->nextRowEvt);
		DEL_CLASS(this->mutUsage);
		if (this->colTypes)
		{
			MemFree(this->colTypes);
			this->colTypes = 0;
		}
	}

	virtual Bool ReadNext()
	{
		while (!this->nextRowReady)
		{
			this->rowEvt->Wait(1000);
		}
		if (this->nextRow)
		{
			RowData *row = this->currRow;
			this->currRow = this->nextRow;
			this->nextRow = row;
			this->nextRowReady = false;
			this->nextRowEvt->Set();
			return true;
		}
		else
		{
			return false;
		}
	}

	virtual UOSInt ColCount()
	{
		return this->colCount;
	}

	virtual OSInt GetRowChanged()
	{
		return this->rowChanged;
	}

	virtual Int32 GetInt32(UOSInt colIndex)
	{
		Data::VariItem item;
		if (!this->GetVariItem(colIndex, &item))
		{
			return 0;
		}
		return item.GetAsI32();
	}

	virtual Int64 GetInt64(UOSInt colIndex)
	{
		Data::VariItem item;
		if (!this->GetVariItem(colIndex, &item))
		{
			return 0;
		}
		return item.GetAsI64();
	}

	virtual WChar *GetStr(UOSInt colIndex, WChar *buff)
	{
		Data::VariItem item;
		if (!this->GetVariItem(colIndex, &item))
		{
			return 0;
		}
		Data::VariItem::ItemType itemType = item.GetItemType();
		if (itemType == Data::VariItem::ItemType::Null)
		{
			return 0;
		}
		Text::StringBuilderUTF8 sb;
		item.GetAsString(&sb);
		return Text::StrUTF8_WChar(buff, sb.ToString(), 0);
	}

	virtual Bool GetStr(UOSInt colIndex, Text::StringBuilderUTF8 *sb)
	{
		Data::VariItem item;
		if (!this->GetVariItem(colIndex, &item))
		{
			return false;
		}
		Data::VariItem::ItemType itemType = item.GetItemType();
		if (itemType == Data::VariItem::ItemType::Null)
		{
			return false;
		}
		item.GetAsString(sb);
		return true;
	}

	virtual Text::String *GetNewStr(UOSInt colIndex)
	{
		Data::VariItem item;
		if (!this->GetVariItem(colIndex, &item))
		{
			return 0;
		}
		Data::VariItem::ItemType itemType = item.GetItemType();
		if (itemType == Data::VariItem::ItemType::Str)
		{
			return item.GetItemValue().str->Clone();
		}
		else if (itemType == Data::VariItem::ItemType::Null)
		{
			return 0;
		}
		else
		{
			Text::StringBuilderUTF8 sb;
			item.GetAsString(&sb);
			return Text::String::New(sb.ToString(), sb.GetLength());
		}
	}

	virtual UTF8Char *GetStr(UOSInt colIndex, UTF8Char *buff, UOSInt buffSize)
	{
		Data::VariItem item;
		if (!this->GetVariItem(colIndex, &item))
		{
			return 0;
		}
		if (item.GetItemType() == Data::VariItem::ItemType::Null)
		{
			return 0;
		}
		return item.GetAsStringS(buff, buffSize);
	}

	virtual DateErrType GetDate(UOSInt colIndex, Data::DateTime *outVal)
	{
		if (this->IsNull(colIndex))
		{
			return DET_NULL;
		}
		Data::VariItem item;
		if (!this->GetVariItem(colIndex, &item))
		{
			return DET_ERROR;
		}
		else
		{
			if (item.GetItemType() == Data::VariItem::ItemType::Date)
			{
				outVal->SetValue(item.GetItemValue().date);
				return DET_OK;
			}
			else
			{
				return DET_ERROR;
			}
		}
	}

	virtual Double GetDbl(UOSInt colIndex)
	{
		Data::VariItem item;
		if (!this->GetVariItem(colIndex, &item))
		{
			return 0;
		}
		return item.GetAsF64();
	}

	virtual Bool GetBool(UOSInt colIndex)
	{
		Data::VariItem item;
		if (!this->GetVariItem(colIndex, &item))
		{
			return 0;
		}
		return item.GetAsBool();
	}

	virtual UOSInt GetBinarySize(UOSInt colIndex)
	{
		if (this->currRow == 0)
		{
			return 0;
		}
		if (colIndex >= this->colCount)
		{
			return 0;
		}
		return this->currRow->cols[colIndex].len;
	}

	virtual UOSInt GetBinary(UOSInt colIndex, UInt8 *buff)
	{
		if (this->currRow == 0)
		{
			return 0;
		}
		if (colIndex >= this->colCount)
		{
			return 0;
		}
		UOSInt len = this->currRow->cols[colIndex].len;
		MemCopyNO(buff, &this->currRow->rowBuff[this->currRow->cols[colIndex].ofst], len);
		return len;
	}

	virtual Math::Vector2D *GetVector(UOSInt colIndex)
	{
		Data::VariItem item;
		if (!this->GetVariItem(colIndex, &item))
		{
			return 0;
		}
		return item.GetAndRemoveVector();
	}

	virtual Bool GetUUID(UOSInt colIndex, Data::UUID *uuid)
	{
		Data::VariItem item;
		if (!this->GetVariItem(colIndex, &item))
		{
			return 0;
		}
		return item.GetAndRemoveUUID();
	}

	virtual Bool GetVariItem(UOSInt colIndex, Data::VariItem *item)
	{
		if (this->currRow == 0)
		{
			return false;
		}
		if (colIndex >= this->colCount)
		{
			return false;
		}
		RowColumn *col = &this->currRow->cols[colIndex];
		if (col->isNull)
		{
			item->SetNull();
			return true;
		}
		switch (this->colTypes[colIndex])
		{
		case Net::MySQLUtil::MYSQL_TYPE_STRING:
		case Net::MySQLUtil::MYSQL_TYPE_VARCHAR:
		case Net::MySQLUtil::MYSQL_TYPE_VAR_STRING:
		case Net::MySQLUtil::MYSQL_TYPE_LONG_BLOB:
		case Net::MySQLUtil::MYSQL_TYPE_MEDIUM_BLOB:
		case Net::MySQLUtil::MYSQL_TYPE_BLOB:
		case Net::MySQLUtil::MYSQL_TYPE_TINY_BLOB:
			item->SetStr(&this->currRow->rowBuff[col->ofst], col->len);
			return true;

		case Net::MySQLUtil::MYSQL_TYPE_LONGLONG:
			item->SetI64(ReadInt64(&this->currRow->rowBuff[col->ofst]));
			return true;

		case Net::MySQLUtil::MYSQL_TYPE_LONG:
		case Net::MySQLUtil::MYSQL_TYPE_INT24:
			item->SetI32(ReadInt32(&this->currRow->rowBuff[col->ofst]));
			return true;

		case Net::MySQLUtil::MYSQL_TYPE_SHORT:
		case Net::MySQLUtil::MYSQL_TYPE_YEAR:
			item->SetI16(ReadInt16(&this->currRow->rowBuff[col->ofst]));
			return true;

		case Net::MySQLUtil::MYSQL_TYPE_TINY:
			item->SetI8((Int8)this->currRow->rowBuff[col->ofst]);
			return true;

		case Net::MySQLUtil::MYSQL_TYPE_DOUBLE:
			item->SetF64(ReadDouble(&this->currRow->rowBuff[col->ofst]));
			return true;

		case Net::MySQLUtil::MYSQL_TYPE_FLOAT:
			item->SetF32(ReadFloat(&this->currRow->rowBuff[col->ofst]));
			return true;

		case Net::MySQLUtil::MYSQL_TYPE_DATE:
		case Net::MySQLUtil::MYSQL_TYPE_DATETIME:
		case Net::MySQLUtil::MYSQL_TYPE_TIMESTAMP:
		{
			Data::DateTime *dt;
			switch (col->len)
			{
			case 0:
				NEW_CLASS(dt, Data::DateTime());
				dt->SetTicks(0);
				item->SetDateDirect(dt);
				return true;
			case 4:
				NEW_CLASS(dt, Data::DateTime());
				dt->SetValueNoFix(ReadUInt16(&this->currRow->rowBuff[col->ofst]), this->currRow->rowBuff[col->ofst + 2], this->currRow->rowBuff[col->ofst + 3], 0, 0, 0, 0, 0);
				item->SetDateDirect(dt);
				return true;
			case 7:
				NEW_CLASS(dt, Data::DateTime());
				dt->SetValueNoFix(ReadUInt16(&this->currRow->rowBuff[col->ofst]), this->currRow->rowBuff[col->ofst + 2], this->currRow->rowBuff[col->ofst + 3],
					this->currRow->rowBuff[col->ofst + 4], this->currRow->rowBuff[col->ofst + 5], this->currRow->rowBuff[col->ofst + 6], 0, 0);
				item->SetDateDirect(dt);
				return true;
			case 11:
				NEW_CLASS(dt, Data::DateTime());
				dt->SetValueNoFix(ReadUInt16(&this->currRow->rowBuff[col->ofst]), this->currRow->rowBuff[col->ofst + 2], this->currRow->rowBuff[col->ofst + 3],
					this->currRow->rowBuff[col->ofst + 4], this->currRow->rowBuff[col->ofst + 5], this->currRow->rowBuff[col->ofst + 6], (UInt16)(ReadUInt32(&this->currRow->rowBuff[col->ofst + 7]) / 1000), 0);
				item->SetDateDirect(dt);
				return true;
			default:
				//////////////////////////////////////
				printf("Unknown binary date format\r\n");
				item->SetNull();
				return true;
			}
		}
		case Net::MySQLUtil::MYSQL_TYPE_TIME:
		{
			Double v;
			switch (col->len)
			{
			case 0:
				item->SetF64(0);
				return true;
			case 8:
				v = ReadUInt32(&this->currRow->rowBuff[col->ofst + 1]) * 86400 + (UInt32)this->currRow->rowBuff[col->ofst + 5] * 3600 + (UInt32)this->currRow->rowBuff[col->ofst + 6] * 60 + this->currRow->rowBuff[col->ofst + 7];
				if (this->currRow->rowBuff[col->ofst])
				{
					item->SetF64(-v);
				}
				else
				{
					item->SetF64(v);
				}
				return true;
			case 12:
				v = ReadUInt32(&this->currRow->rowBuff[col->ofst + 1]) * 86400 + (UInt32)this->currRow->rowBuff[col->ofst + 5] * 3600 + (UInt32)this->currRow->rowBuff[col->ofst + 6] * 60 + this->currRow->rowBuff[col->ofst + 7] + (ReadUInt32(&this->currRow->rowBuff[col->ofst + 1]) / 1000000);
				if (this->currRow->rowBuff[col->ofst])
				{
					item->SetF64(-v);
				}
				else
				{
					item->SetF64(v);
				}
				return true;
			default:
				//////////////////////////////////////
				printf("Unknown binary time format\r\n");
				item->SetNull();
				return true;
			}
		}

		case Net::MySQLUtil::MYSQL_TYPE_NEWDATE:
		case Net::MySQLUtil::MYSQL_TYPE_TIMESTAMP2:
		case Net::MySQLUtil::MYSQL_TYPE_DATETIME2:
		case Net::MySQLUtil::MYSQL_TYPE_TIME2:
			//////////////////////////////////////
			printf("Unknown binary date2 format\r\n");
			item->SetNull();
			return true;

		case Net::MySQLUtil::MYSQL_TYPE_NULL:
			item->SetNull();
			return true;

		case Net::MySQLUtil::MYSQL_TYPE_ENUM:
		case Net::MySQLUtil::MYSQL_TYPE_SET:
		case Net::MySQLUtil::MYSQL_TYPE_GEOMETRY:
		case Net::MySQLUtil::MYSQL_TYPE_BIT:
		case Net::MySQLUtil::MYSQL_TYPE_DECIMAL:
		case Net::MySQLUtil::MYSQL_TYPE_NEWDECIMAL:
		default:
			////////////////////////////////////
			printf("Unknown binary other format\r\n");
			item->SetNull();
			return true;
		}
	}

	virtual Bool IsNull(UOSInt colIndex)
	{
		if (this->currRow == 0)
		{
			return true;
		}
		if (colIndex >= this->colCount)
		{
			return true;
		}
		return this->currRow->cols[colIndex].isNull;
	}

	virtual UTF8Char *GetName(UOSInt colIndex, UTF8Char *buff)
	{
		ColumnDef *col = this->cols->GetItem(colIndex);
		if (col)
		{
			return Text::StrConcat(buff, col->name->v);
		}
		return 0;
	}

	virtual DB::DBUtil::ColType GetColType(UOSInt colIndex, UOSInt *colSize)
	{
		ColumnDef *col = this->cols->GetItem(colIndex);
		if (col)
		{
			*colSize = col->colLen;
			return col->dbColType;
		}
		return DB::DBUtil::CT_Unknown;
	}

	virtual Bool GetColDef(UOSInt colIndex, DB::ColDef *colDef)
	{
		ColumnDef *col = this->cols->GetItem(colIndex);
		if (col)
		{
			colDef->SetColName(col->name);
			colDef->SetColDP(col->decimals);
			colDef->SetColSize(col->colLen);
			colDef->SetColType(Net::MySQLUtil::MySQLType2ColType(col->colType));
			return true;
		}
		return false;
	}

	void SetRowChanged(Int64 val)
	{
		this->rowChanged = (OSInt)val;
		this->nextRowReady = true;
	}

	void AddColumnDef41(const UInt8 *colDef, UOSInt buffSize)
	{
		UInt64 v;
		ColumnDef *col = MemAlloc(ColumnDef, 1);
		const UInt8 *colEnd = colDef + buffSize;
		colDef = Net::MySQLUtil::ReadLenencInt(colDef, &v); //catalog
		colDef += v;
		colDef = Net::MySQLUtil::ReadLenencInt(colDef, &v); //schema
		colDef += v;
		colDef = Net::MySQLUtil::ReadLenencInt(colDef, &v); //table
		colDef += v;
		colDef = Net::MySQLUtil::ReadLenencInt(colDef, &v); //org_table
		colDef += v;
		colDef = Net::MySQLUtil::ReadLenencInt(colDef, &v); //name
		col->name = Text::String::New(colDef, (UOSInt)v);
		colDef += v;
		colDef = Net::MySQLUtil::ReadLenencInt(colDef, &v); //org_name
		colDef += v;
		if (colDef[0] != 12)
		{
			col->name->Release();
			MemFree(col);
			return;
		}
		colDef += 1;
		col->charSet = ReadUInt16(&colDef[0]);
		col->colLen = ReadUInt32(&colDef[2]);
		col->colType = (Net::MySQLUtil::MySQLType)colDef[6];
		col->dbColType = Net::MySQLUtil::MySQLType2ColType(col->colType);
		col->flags = ReadUInt16(&colDef[7]);
		col->decimals = colDef[9];
		colDef += 12;
		col->defValues = 0;
		if (colDef < colEnd)
		{
			colDef = Net::MySQLUtil::ReadLenencInt(colDef, &v); //catalog
			if ((colDef + v) <= colEnd)
			{
				col->defValues = Text::String::New(colDef, (UOSInt)v);
			}
		}
		this->cols->Add(col);
		this->colCount++;
	}

	void AddRowData(const UInt8 *rowData, UOSInt dataSize)
	{
		while (this->nextRowReady)
		{
			this->nextRowEvt->Wait(1000);
		}
		if (this->nextRow == 0)
		{
			this->nextRow = MemAlloc(RowData, 1);
			this->nextRow->cols = MemAlloc(RowColumn, this->colCount);
			this->nextRow->rowBuffCapacity = dataSize * 2;
			this->nextRow->rowBuff = MemAlloc(UInt8, dataSize * 2);
		}
		else if (this->nextRow->rowBuffCapacity < dataSize)
		{
			this->nextRow->rowBuffCapacity = dataSize;
			MemFree(this->nextRow->rowBuff);
			this->nextRow->rowBuff = MemAlloc(UInt8, dataSize);
		}
		MemCopyNO(this->nextRow->rowBuff, rowData, dataSize);
		UOSInt i = 0;
		UOSInt j = this->colCount;
		UInt64 v;
		UOSInt nullOfst = 1;
		UOSInt nullBitOfst = 2;
		UOSInt tableOfst = 1 + ((j + 2 + 7) >> 3);
		RowColumn *col;
		while (i < j)
		{
			col = &this->nextRow->cols[i];
			if (rowData[nullOfst] & (1 << nullBitOfst))
			{
				col->isNull = true;
				col->ofst = 0;
				col->len = 0;
			}
			else
			{
				col->isNull = false;
				col->ofst = tableOfst;
				switch (this->colTypes[i])
				{
				case Net::MySQLUtil::MYSQL_TYPE_STRING:
				case Net::MySQLUtil::MYSQL_TYPE_VARCHAR:
				case Net::MySQLUtil::MYSQL_TYPE_VAR_STRING:
				case Net::MySQLUtil::MYSQL_TYPE_ENUM:
				case Net::MySQLUtil::MYSQL_TYPE_SET:
				case Net::MySQLUtil::MYSQL_TYPE_LONG_BLOB:
				case Net::MySQLUtil::MYSQL_TYPE_MEDIUM_BLOB:
				case Net::MySQLUtil::MYSQL_TYPE_BLOB:
				case Net::MySQLUtil::MYSQL_TYPE_TINY_BLOB:
				case Net::MySQLUtil::MYSQL_TYPE_GEOMETRY:
				case Net::MySQLUtil::MYSQL_TYPE_BIT:
				case Net::MySQLUtil::MYSQL_TYPE_DECIMAL:
				case Net::MySQLUtil::MYSQL_TYPE_NEWDECIMAL:
					col->ofst = (UOSInt)(Net::MySQLUtil::ReadLenencInt(&rowData[tableOfst], &v) - rowData);
					col->len = (UOSInt)v;
					tableOfst = col->ofst + (UOSInt)v;
					break;

				case Net::MySQLUtil::MYSQL_TYPE_LONGLONG:
					tableOfst += 8;
					col->len = 8;
					break;

				case Net::MySQLUtil::MYSQL_TYPE_LONG:
				case Net::MySQLUtil::MYSQL_TYPE_INT24:
					tableOfst += 4;
					col->len = 4;
					break;

				case Net::MySQLUtil::MYSQL_TYPE_SHORT:
				case Net::MySQLUtil::MYSQL_TYPE_YEAR:
					tableOfst += 2;
					col->len = 2;
					break;

				case Net::MySQLUtil::MYSQL_TYPE_TINY:
					tableOfst += 1;
					col->len = 1;
					break;

				case Net::MySQLUtil::MYSQL_TYPE_DOUBLE:
					tableOfst += 8;
					col->len = 8;
					break;

				case Net::MySQLUtil::MYSQL_TYPE_FLOAT:
					tableOfst += 4;
					col->len = 4;
					break;

				case Net::MySQLUtil::MYSQL_TYPE_DATE:
				case Net::MySQLUtil::MYSQL_TYPE_DATETIME:
				case Net::MySQLUtil::MYSQL_TYPE_TIMESTAMP:
					col->len = rowData[tableOfst];
					tableOfst += (UOSInt)col->len + 1;
					col->ofst += 1;
					break;

				case Net::MySQLUtil::MYSQL_TYPE_TIME:
					col->len = rowData[tableOfst];
					tableOfst += (UOSInt)col->len + 1;
					col->ofst += 1;
					break;

				case Net::MySQLUtil::MYSQL_TYPE_NEWDATE:
				case Net::MySQLUtil::MYSQL_TYPE_TIMESTAMP2:
				case Net::MySQLUtil::MYSQL_TYPE_DATETIME2:
				case Net::MySQLUtil::MYSQL_TYPE_TIME2:
					col->len = rowData[tableOfst];
					tableOfst += (UOSInt)col->len + 1;
					col->ofst += 1;
					break;

				case Net::MySQLUtil::MYSQL_TYPE_NULL:
				default:
					col->len = 0;
					col->isNull = true;
					break;
				}
			}
			if (++nullBitOfst >= 8)
			{
				nullBitOfst -= 8;
				nullOfst++;
			}
			i++;
		}
		this->nextRowReady = true;
		this->rowEvt->Set();
	}

	void EndCols()
	{
		if (this->colTypes == 0)
		{
			this->colTypes = MemAlloc(Net::MySQLUtil::MySQLType, this->colCount);
			UOSInt i = this->colCount;
			while (i-- > 0)
			{
				this->colTypes[i] = this->cols->GetItem(i)->colType;
			}
		}
	}

	void EndData()
	{
		if (this->nextRow == 0 && this->nextRowReady)
		{
			return;
		}
		while (this->nextRowReady)
		{
			this->nextRowEvt->Wait(1000);
		}
		RowData *row = this->nextRow;
		this->nextRow = 0;
		this->nextRowReady = true;
		this->rowEvt->Set();

		if (row)
		{
			MemFree(row->cols);
			MemFree(row->rowBuff);
			MemFree(row);
		}
	}

	void SetStmtId(UInt32 stmtId)
	{
		this->stmtId = stmtId;
	}

	UInt32 GetStmtId()
	{
		return this->stmtId;
	}
};

UInt32 __stdcall Net::MySQLTCPClient::RecvThread(void *userObj)
{
	Net::MySQLTCPClient *me = (Net::MySQLTCPClient*)userObj;
	UInt8 *buff;
	UOSInt buffSize;
	UOSInt readSize;
	UTF8Char sbuff[512];
	UTF8Char *sptr;
	UTF8Char sbuff2[128];
	UTF8Char *sptr2;
	UInt8 *ptrCurr;
	UInt8 *ptrEnd;
	OSInt i;
#if defined(VERBOSE)
	Text::StringBuilderUTF8 *sb;
#endif
	me->recvStarted = true;
	me->recvRunning = true;
	buffSize = 0;
#if defined(VERBOSE)
	NEW_CLASS(sb, Text::StringBuilderUTF8());
#endif
	buff = MemAlloc(UInt8, BUFFSIZE);
	while (true)
	{
		readSize = me->cli->Read(&buff[buffSize], BUFFSIZE - buffSize);
		if (readSize <= 0)
			break;
#if defined(VERBOSE)
		sb->ClearStr();
		sb->AppendHexBuff(&buff[buffSize], readSize, ' ', Text::LineBreakType::CRLF);
		printf("MySQLTCP Received Buff:\r\n%s\r\n", sb->ToString());
#endif
		buffSize += readSize;

		if (me->mode == 0)
		{
			readSize = buffSize;
			if (buffSize >= 10)
			{
				if (buff[3] != 0)
				{
#if defined(VERBOSE)
					printf("MySQLTCP Seq Id Invalid\r\n");
#endif
					me->cli->Close();
					readSize = 0;
				}
				else
				{
					UOSInt packetSize = ReadUInt32(buff);
					if (packetSize < 10 || packetSize > 1024)
					{
#if defined(VERBOSE)
						printf("MySQLTCP packet size Invalid\r\n");
#endif
						me->cli->Close();
						readSize = 0;
					}
					else if (packetSize + 4 <= buffSize)
					{
						if (buff[4] == 9)
						{
							if (buff[packetSize + 3] != 0 || buff[packetSize - 10] != 0)
							{
#if defined(VERBOSE)
								printf("MySQLTCP protocol ver 9 invalid\r\n");
#endif
								me->cli->Close();
								readSize = 0;
							}
							else
							{
								me->svrVer = Text::String::NewNotNull(&buff[5]);
								me->connId = ReadUInt32(&buff[packetSize - 9]);
								MemCopyNO(me->authPluginData, &buff[packetSize - 5], 8);
								me->authPluginDataSize = 8;
								me->mode = 1;
								////////////////////////////////
#if defined(VERBOSE)
								printf("MySQLTCP Server ver = %s\r\n", me->svrVer->v);
								printf("MySQLTCP Conn Id = %d\r\n", me->connId);
								sb->ClearStr();
								sb->AppendHexBuff(me->authPluginData, me->authPluginDataSize, ' ', Text::LineBreakType::None);
								printf("MySQLTCP Auth Plugin Data = %s\r\n", sb->ToString());
#endif
								readSize = 0;
							}
						}
						else if (buff[4] == 10)
						{
							ptrEnd = &buff[packetSize + 4];
							sptr = Text::StrConcatS(sbuff, &buff[5], packetSize - 1);
							ptrCurr = &buff[6] + (sptr - sbuff);
							me->svrVer = Text::String::New(sbuff, (UOSInt)(sptr - sbuff));
#if defined(VERBOSE)
							printf("MySQLTCP Server ver = %s\r\n", me->svrVer->v);
#endif
							if (ptrEnd - ptrCurr >= 15)
							{
								me->connId = ReadUInt32(&ptrCurr[0]);
								MemCopyNO(me->authPluginData, &ptrCurr[4], 8);
								me->authPluginDataSize = 8;
								me->svrCap = ReadUInt16(&ptrCurr[13]);
								ptrCurr += 15;
								if (ptrEnd - ptrCurr >= 16)
								{
									me->svrCS = ptrCurr[0];
									me->connStatus = ReadUInt16(&ptrCurr[1]);
									me->svrCap |= ((UInt32)ReadUInt16(&ptrCurr[3])) << 16;
									UInt8 len = ptrCurr[5];
									ptrCurr += 16;
									if (me->svrCap & Net::MySQLUtil::CLIENT_SECURE_CONNECTION)
									{
										if (len == 21 && (ptrEnd - ptrCurr) >= 13)
										{
											MemCopyNO(&me->authPluginData[8], ptrCurr, 12);
											me->authPluginDataSize = 20;
											ptrCurr += 13;
										}
										else if (len > 21)
										{
											ptrCurr += 13;
										}
										else
										{
											ptrCurr += len - 8;
										}
									}
									if (me->svrCap & Net::MySQLUtil::CLIENT_PLUGIN_AUTH)
									{
										Text::StrConcatS(sbuff, ptrCurr, (UOSInt)(ptrEnd - ptrCurr));
									}
									else
									{
										sbuff[0] = 0;
									}
#if defined(VERBOSE)
									printf("MySQLTCP Conn Id = %d\r\n", me->connId);
									printf("MySQLTCP Server Cap = 0x%x\r\n", me->svrCap);
									printf("MySQLTCP character set = %d\r\n", me->svrCS);
									printf("MySQLTCP status = 0x%x\r\n", me->connStatus);
									sb->ClearStr();
									sb->AppendHexBuff(me->authPluginData, me->authPluginDataSize, ' ', Text::LineBreakType::None);
									printf("MySQLTCP auth plugin data = %s\r\n", sb->ToString());
									printf("MySQLTCP auth plugin name = %s\r\n", sbuff);
#endif
								}
								else
								{
#if defined(VERBOSE)
									printf("MySQLTCP Conn Id = %d\r\n", me->connId);
									printf("MySQLTCP Server Cap = 0x%x\r\n", me->svrCap);
									sb->ClearStr();
									sb->AppendHexBuff(me->authPluginData, me->authPluginDataSize, ' ', Text::LineBreakType::None);
									printf("MySQLTCP auth plugin data = %s\r\n", sb->ToString());
#endif
								}
								me->mode = 1;

								if (me->authPluginDataSize == 20)
								{
									UInt32 cliCap = Net::MySQLUtil::CLIENT_LONG_PASSWORD | 
										Net::MySQLUtil::CLIENT_FOUND_ROWS |
										Net::MySQLUtil::CLIENT_LONG_FLAG |
										Net::MySQLUtil::CLIENT_PROTOCOL_41 |
										Net::MySQLUtil::CLIENT_TRANSACTIONS |
										Net::MySQLUtil::CLIENT_SECURE_CONNECTION |
										Net::MySQLUtil::CLIENT_MULTI_RESULTS |
										Net::MySQLUtil::CLIENT_PLUGIN_AUTH |
										Net::MySQLUtil::CLIENT_CONNECT_ATTRS |
										Net::MySQLUtil::CLIENT_PLUGIN_AUTH_LENENC_CLIENT_DATA;
									if (me->database)
									{
										cliCap |= Net::MySQLUtil::CLIENT_CONNECT_WITH_DB;
									}
									WriteUInt32(&buff[4], cliCap);
									WriteInt32(&buff[8], 16777215);
									buff[12] = 45;
									MemClear(&buff[13], 23);
									ptrCurr = me->userName->ConcatTo(&buff[36]) + 1;
									ptrCurr[0] = 20;
									Crypto::Hash::SHA1 sha1;
									sha1.Calc(me->password->v, me->password->leng);
									sha1.GetValue(sbuff);
									sha1.Clear();
									sha1.Calc(sbuff, 20);
									sha1.GetValue(&ptrCurr[1]);
									sha1.Clear();
									sha1.Calc(me->authPluginData, 20);
									sha1.Calc(&ptrCurr[1], 20);
									sha1.GetValue(&ptrCurr[1]);
									i = 0;
									while (i < 20)
									{
										ptrCurr[i + 1] ^= sbuff[i];
										i++;
									}
									ptrCurr += 21;
									if (me->database)
									{
										ptrCurr = me->database->ConcatTo(ptrCurr) + 1;
									}
									if (cliCap & Net::MySQLUtil::CLIENT_PLUGIN_AUTH)
									{
										ptrCurr = Text::StrConcatC(ptrCurr, UTF8STRC("mysql_native_password")) + 1;
									}

									if (cliCap & Net::MySQLUtil::CLIENT_CONNECT_ATTRS)
									{
										sptr = sbuff;
										sptr = Net::MySQLUtil::AppendLenencStrC(sptr, UTF8STRC("_client_name"));
										sptr = Net::MySQLUtil::AppendLenencStrC(sptr, UTF8STRC("MySQL TCP Client/SSWR"));
										sptr = Net::MySQLUtil::AppendLenencStrC(sptr, UTF8STRC("_client_version"));
										sptr = Net::MySQLUtil::AppendLenencStrC(sptr, UTF8STRC(CLIVERSION));
										sptr = Net::MySQLUtil::AppendLenencStrC(sptr, UTF8STRC("_os"));
										sptr2 = IO::OS::GetDistro(sbuff2);
										sptr = Net::MySQLUtil::AppendLenencStrC(sptr, sbuff2, (UOSInt)(sptr2 - sbuff2));
										sptr = Net::MySQLUtil::AppendLenencStrC(sptr, UTF8STRC("_os_version"));
										sptr2 = IO::OS::GetVersion(sbuff2);
										sptr = Net::MySQLUtil::AppendLenencStrC(sptr, sbuff2, (UOSInt)(sptr2 - sbuff2));
										ptrCurr = Net::MySQLUtil::AppendLenencInt(ptrCurr, (UOSInt)(sptr - sbuff));
										MemCopyNO(ptrCurr, sbuff, (UOSInt)(sptr - sbuff));
										ptrCurr += sptr - sbuff;
									}
									WriteInt24(buff, (ptrCurr - buff - 4));
									buff[3] = 1;
									me->cli->Write(buff, (UOSInt)(ptrCurr - buff));
#if defined(VERBOSE)
									printf("MySQLTCP handshake response sent\r\n");
#endif
								}

								readSize = 0;
							}
							else
							{
#if defined(VERBOSE)
								printf("MySQLTCP protocol version 10 invalid\r\n");
#endif
								me->mode = 1;
								////////////////////////////////
								readSize = 0;
								me->cli->Close();
							}
						}
						else
						{
#if defined(VERBOSE)
							printf("MySQLTCP protocol version unsupported (%d)\r\n", buff[4]);
#endif
							readSize = 0;
							me->cli->Close();
						}
					}
				}
			}
		}
		else if (me->mode == 1)
		{
			if (buffSize < 4)
			{
				readSize = buffSize;
			}
			else
			{
				readSize = ReadUInt24(&buff[buffSize - readSize]);
				if (readSize + 4 <= buffSize)
				{
					if (buff[4] == 0xff)
					{
						me->SetLastError(&buff[7], readSize - 3);
						me->cli->Close();
					}
					else if (buff[3] != 2)
					{
						me->SetLastError(UTF8STRC("Invalid login reply"));
						me->cli->Close();
					}
					else if (buff[4] == 0)
					{
						me->mode = 2;
#if defined(VERBOSE)
						printf("MySQLTCP login success\r\n");
#endif
					}
					else if (buff[4] == 0xFE)
					{
						Text::StringBuilderUTF8 sb;
						sb.AppendC(UTF8STRC("AuthSwitchRequest received: plugin name = "));
						sb.AppendSlow(&buff[5]);
						me->SetLastError(sb.ToString());
						me->cli->Close();
					}
					else
					{
						me->SetLastError(UTF8STRC("Invalid reply on login"));
						me->cli->Close();
					}
					readSize = 0;
				}
				else
				{
					readSize = buffSize;
				}
			}
		}
		else if (me->mode == 2)
		{
			UInt32 packetSize;
			UInt64 val;
			readSize = 0;
			while (readSize + 4 <= buffSize)
			{
				packetSize = ReadUInt24(&buff[readSize]);
				if (readSize + 4 + packetSize > buffSize)
				{
					break;
				}
				if (buff[readSize + 4] == 0xFF || (buff[readSize + 3] == (me->cmdSeqNum & 0xff) && me->cmdReader))
				{
					if (me->cmdSeqNum == 1)
					{
						if (me->cmdResultType == CmdResultType::ProcessingBinary)
						{
							if (buff[readSize + 4] == 0) //OK
							{
								UInt32 stmtId = ReadUInt32(&buff[readSize + 5]);
								((MySQLTCPBinaryReader*)me->cmdReader)->SetStmtId(stmtId);
#if defined(VERBOSE)
								UInt16 numColumns = ReadUInt16(&buff[readSize + 9]);
								UInt16 numParams = ReadUInt16(&buff[readSize + 11]);
								printf("MySQLTCP COM_STMT_PREPARE OK, stmt id = %d, num_columns = %d, num_params = %d\r\n", stmtId, numColumns, numParams);
#endif
							}
							else if (buff[readSize + 4] == 0xFF) //ERR
							{
								me->SetLastError(&buff[readSize + 7], packetSize - 3);
								if (me->cmdReader)
								{
									me->cmdResultType = CmdResultType::Error;
									me->cmdEvt->Set();
									((MySQLTCPBinaryReader*)me->cmdReader)->EndData();
								}
								else
								{
									me->cmdResultType = CmdResultType::Error;
									me->cmdEvt->Set();
								}
#if defined(VERBOSE)
								printf("MySQLTCP COM_STMT_PREPARE Error\r\n");
#endif
							}
							else
							{
								me->cmdResultType = CmdResultType::Error;
								me->cmdEvt->Set();
								if (me->cmdReader)
								{
									((MySQLTCPBinaryReader*)me->cmdReader)->EndData();
								}
#if defined(VERBOSE)
								printf("MySQLTCP COM_STMT_PREPARE Error\r\n");
#endif
							}
						}
						else if (me->cmdResultType == CmdResultType::BinaryExecuting)
						{
							if (buff[readSize + 4] == 0xFF) //ERR
							{
								me->SetLastError(&buff[readSize + 7], packetSize - 3);
								if (me->cmdReader)
								{
									me->cmdResultType = CmdResultType::Error;
									me->cmdEvt->Set();
									((MySQLTCPBinaryReader*)me->cmdReader)->EndData();
								}
								else
								{
									me->cmdResultType = CmdResultType::Error;
									me->cmdEvt->Set();
								}
#if defined(VERBOSE)
								printf("MySQLTCP COM_STMT_EXECUTE Error\r\n");
#endif
							}
							else
							{
								Net::MySQLUtil::ReadLenencInt(&buff[readSize + 4], &val);
#if defined(VERBOSE)
								printf("MySQLTCP COM_STMT_EXECUTE executed, column cnt = %lld\r\n", val);
#endif
							}
						}
						else
						{
							if (buff[readSize + 4] == 0) //OK
							{
								Net::MySQLUtil::ReadLenencInt(&buff[readSize + 5], &val);
								((MySQLTCPReader*)me->cmdReader)->SetRowChanged((Int64)val);
								me->cmdResultType = CmdResultType::ResultEnd;
								me->cmdEvt->Set();
#if defined(VERBOSE)
								printf("MySQLTCP Command OK, row changed = %lld\r\n", val);
#endif
							}
							else if (buff[readSize + 4] == 0xFF) //ERR
							{
								me->SetLastError(&buff[readSize + 7], packetSize - 3);
								if (me->cmdReader)
								{
									if (me->cmdResultType == CmdResultType::Processing)
									{
										me->cmdResultType = CmdResultType::Error;
										me->cmdEvt->Set();
									}
									((MySQLTCPReader*)me->cmdReader)->EndData();
								}
								else
								{
									me->cmdResultType = CmdResultType::Error;
									me->cmdEvt->Set();
								}
	//							me->cli->Close();
							}
							else
							{
								Net::MySQLUtil::ReadLenencInt(&buff[readSize + 4], &val);
#if defined(VERBOSE)
								printf("MySQLTCP Command executed, column cnt = %lld\r\n", val);
#endif
							}
						}
					}
					else if (buff[readSize + 4] == 0xFE) //EOF
					{
#if defined(VERBOSE)
						printf("MySQLTCP EOF found, curr result type = %d\r\n", (int)me->cmdResultType);
#endif
						switch (me->cmdResultType)
						{
						case CmdResultType::Processing:
							me->cmdResultType = CmdResultType::ResultReady;
							me->cmdEvt->Set();
							break;
						case CmdResultType::ProcessingBinary:
						{
							UInt32 stmtId = ((MySQLTCPBinaryReader*)me->cmdReader)->GetStmtId();
							me->cmdSeqNum = 0;
							WriteUInt32(&sbuff[0], 10);
							sbuff[3] = 0;
							sbuff[4] = 0x17;
							WriteUInt32(&sbuff[5], stmtId);
							sbuff[9] = 0;
							WriteUInt32(&sbuff[10], 1);
							me->cli->Write(sbuff, 14);
#if defined(VERBOSE)
							printf("MySQLTCP EOF found, execute statment id %d\r\n", stmtId);
#endif
							me->cmdResultType = CmdResultType::BinaryExecuting;
							me->cmdEvt->Set();
							break;
						}
						case CmdResultType::BinaryExecuting:
							me->cmdResultType = CmdResultType::BinaryResultReady;
							((MySQLTCPBinaryReader*)me->cmdReader)->EndCols();
							me->cmdEvt->Set();
							break;
						case CmdResultType::BinaryResultReady:
							WriteUInt32(&sbuff[0], 5);
							sbuff[3] = 0;
							sbuff[4] = 0x19; //COM_STMT_CLOSE
							WriteUInt32(&sbuff[5], ((MySQLTCPBinaryReader*)me->cmdReader)->GetStmtId());
							me->cli->Write(sbuff, 9);

							me->cmdResultType = CmdResultType::ResultEnd;
							((MySQLTCPBinaryReader*)me->cmdReader)->EndData();
							me->cmdEvt->Set();

							break;
						case CmdResultType::ResultEnd:
						case CmdResultType::ResultReady:
						case CmdResultType::Error:
						default:
							me->cmdResultType = CmdResultType::ResultEnd;
							((MySQLTCPReader*)me->cmdReader)->EndData();
							me->cmdEvt->Set();
							break;
						}
					}
					else
					{
						switch (me->cmdResultType)
						{
						case CmdResultType::Processing: //ColumnDef
#if defined(VERBOSE)
							printf("MySQLTCP Seq %d Column found\r\n", buff[readSize + 3]);
#endif
							((MySQLTCPReader*)me->cmdReader)->AddColumnDef41(&buff[readSize + 4], packetSize);
							break;
						case CmdResultType::ProcessingBinary: //ColumnDefinition
#if defined(VERBOSE)
							printf("MySQLTCP Seq %d Statement Column found\r\n", buff[readSize + 3]);
#endif
							break;
						case CmdResultType::BinaryExecuting:
#if defined(VERBOSE)
							printf("MySQLTCP Seq %d Binary Column found\r\n", buff[readSize + 3]);
#endif
							((MySQLTCPBinaryReader*)me->cmdReader)->AddColumnDef41(&buff[readSize + 4], packetSize);
							break;
						case CmdResultType::BinaryResultReady:
#if defined(VERBOSE)
							printf("MySQLTCP Seq %d Binary Row found\r\n", buff[readSize + 3]);
#endif
							((MySQLTCPBinaryReader*)me->cmdReader)->AddRowData(&buff[readSize + 4], packetSize);
							break;
						case CmdResultType::ResultEnd:
						case CmdResultType::Error:
						case CmdResultType::ResultReady:
						default:
#if defined(VERBOSE)
							printf("MySQLTCP Seq %d Text Row found\r\n", buff[readSize + 3]);
#endif
							((MySQLTCPReader*)me->cmdReader)->AddRowData(&buff[readSize + 4], packetSize);
							break;
						}
					}
					me->cmdSeqNum++;
				}
				else
				{
#if defined(VERBOSE)
					printf("MySQLTCP Seq num mismatch: %d != %d\r\n", buff[readSize + 3], (int)me->cmdSeqNum);
#endif
				}
				readSize += packetSize + 4;
			}
			readSize = buffSize - readSize;
		}

		if (readSize == 0)
		{
			buffSize = 0;
		}
		else if (readSize < buffSize)
		{
			MemCopyO(buff, &buff[buffSize - readSize], readSize);
			buffSize = readSize;
		}
	}

	if (me->cmdReader)
	{
		if (me->cmdResultType == CmdResultType::Processing)
		{
#if defined(VERBOSE)	
			printf("MySQLTCP End Conn: signal waiting\r\n");
#endif
			me->cmdResultType = CmdResultType::Error;
			me->cmdEvt->Set();
		}
		else
		{
#if defined(VERBOSE)	
			printf("MySQLTCP End Conn: end data\r\n");
#endif
			((MySQLTCPReader*)me->cmdReader)->EndData();
		}
	}
#if defined(VERBOSE)	
	printf("MySQLTCP End connection\r\n");
	DEL_CLASS(sb);
#endif
	MemFree(buff);
	me->recvRunning = false;
	return 0;
}

void Net::MySQLTCPClient::SetLastError(const UTF8Char *errMsg, UOSInt msgLen)
{
	SDEL_STRING(this->lastError);
	this->lastError = Text::String::New(errMsg, msgLen);
#if defined(VERBOSE)
	Text::StringBuilderUTF8 sb;
	this->GetErrorMsg(&sb);
	printf("MySQLTCP Error: %s\r\n", sb.ToString());
#endif
}

void Net::MySQLTCPClient::SetLastError(const UTF8Char *errMsg)
{
	SDEL_STRING(this->lastError);
	this->lastError = Text::String::NewOrNull(errMsg);
#if defined(VERBOSE)
	Text::StringBuilderUTF8 sb;
	this->GetErrorMsg(&sb);
	printf("MySQLTCP Error: %s\r\n", sb.ToString());
#endif
}

Net::MySQLTCPClient::MySQLTCPClient(Net::SocketFactory *sockf, const Net::SocketUtil::AddressInfo *addr, UInt16 port, Text::String *userName, Text::String *password, Text::String *database) : DB::DBConn(CSTR("MySQLTCPClient"))
{
	this->sockf = sockf;
	this->recvRunning = false;
	this->recvStarted = false;
	this->addr = *addr;
	this->port = port;
	this->mode = 0;
	this->svrVer = 0;
	this->connId = 0;
	this->authPluginDataSize = 0;
	this->svrCap = 0;
	this->tableNames = 0;
	this->lastError = 0;
	this->userName = userName->Clone();
	this->password = password->Clone();
	this->database = SCOPY_STRING(database);
	NEW_CLASS(this->cmdMut, Sync::Mutex());
	NEW_CLASS(this->cmdEvt, Sync::Event(true));
	this->cmdSeqNum = 0;
	this->cmdReader = 0;
	this->cmdResultType = CmdResultType::Processing;
	NEW_CLASS(this->cliMut, Sync::Mutex());
	this->cli = 0;
	this->Reconnect();
}

Net::MySQLTCPClient::MySQLTCPClient(Net::SocketFactory *sockf, const Net::SocketUtil::AddressInfo *addr, UInt16 port, Text::CString userName, Text::CString password, Text::CString database) : DB::DBConn(CSTR("MySQLTCPClient"))
{
	this->sockf = sockf;
	this->recvRunning = false;
	this->recvStarted = false;
	this->addr = *addr;
	this->port = port;
	this->mode = 0;
	this->svrVer = 0;
	this->connId = 0;
	this->authPluginDataSize = 0;
	this->svrCap = 0;
	this->tableNames = 0;
	this->lastError = 0;
	this->userName = Text::String::New(userName);
	this->password = Text::String::New(password);
	this->database = Text::String::NewOrNull(database);
	NEW_CLASS(this->cmdMut, Sync::Mutex());
	NEW_CLASS(this->cmdEvt, Sync::Event(true));
	this->cmdSeqNum = 0;
	this->cmdReader = 0;
	this->cmdResultType = CmdResultType::Processing;
	NEW_CLASS(this->cliMut, Sync::Mutex());
	this->cli = 0;
	this->Reconnect();
}

Net::MySQLTCPClient::~MySQLTCPClient()
{
	if (this->cli)
	{
		if (this->recvRunning)
		{
			this->cli->Close();
		}
		while (this->recvRunning)
		{
			Sync::Thread::Sleep(1);
		}
		DEL_CLASS(this->cli);
		this->cli = 0;
	}
	DEL_CLASS(this->cliMut);
	DEL_CLASS(this->cmdMut);
	DEL_CLASS(this->cmdEvt);
	this->userName->Release();
	this->password->Release();
	SDEL_STRING(this->database);
	SDEL_STRING(this->svrVer);
	SDEL_STRING(this->lastError);
	if (this->tableNames)
	{
		UOSInt i = this->tableNames->GetCount();
		while (i-- > 0)
		{
			Text::StrDelNew(this->tableNames->GetItem(i));
		}
		DEL_CLASS(this->tableNames);
	}
}

DB::DBUtil::ServerType Net::MySQLTCPClient::GetSvrType()
{
	return DB::DBUtil::ServerType::MySQL;
}

DB::DBConn::ConnType Net::MySQLTCPClient::GetConnType()
{
	return DB::DBConn::CT_MYSQLTCP;
}

Int8 Net::MySQLTCPClient::GetTzQhr()
{
	return 0;
}

void Net::MySQLTCPClient::ForceTz(Int8 tzQhr)
{

}

void Net::MySQLTCPClient::GetConnName(Text::StringBuilderUTF8 *sb)
{
	UTF8Char sbuff[64];
	UTF8Char *sptr;
	sb->AppendC(UTF8STRC("MySQLTCP:"));
	sptr = Net::SocketUtil::GetAddrName(sbuff, &this->addr, this->port);
	sb->AppendC(sbuff, (UOSInt)(sptr - sbuff));
	if (this->database)
	{
		sb->AppendChar('/', 1);
		sb->Append(this->database);
	}
}

void Net::MySQLTCPClient::Close()
{
	this->cli->Close();
}

void Net::MySQLTCPClient::Dispose()
{
	this->Close();
}

OSInt Net::MySQLTCPClient::ExecuteNonQuery(const UTF8Char *sql)
{
	return ExecuteNonQueryC(sql, Text::StrCharCnt(sql));
}

OSInt Net::MySQLTCPClient::ExecuteNonQueryC(const UTF8Char *sql, UOSInt sqlLen)
{
	DB::DBReader *reader = ExecuteReaderTextC(sql, sqlLen);
	if (reader == 0)
	{
		return -2;
	}
	else
	{
		OSInt cnt = reader->GetRowChanged();
		this->CloseReader(reader);
		return cnt;
	}
}

DB::DBReader *Net::MySQLTCPClient::ExecuteReader(const UTF8Char *sql)
{
	return ExecuteReaderC(sql, Text::StrCharCnt(sql));
}

DB::DBReader *Net::MySQLTCPClient::ExecuteReaderC(const UTF8Char *sql, UOSInt sqlLen)
{
	return ExecuteReaderBinaryC(sql, sqlLen);
}

DB::DBReader *Net::MySQLTCPClient::ExecuteReaderTextC(const UTF8Char *sql, UOSInt sqlLen)
{
	if (this->cli == 0 || !this->recvRunning)
	{
		this->lastDataError = DE_CONN_ERROR;
		return 0;
	}
	while (this->mode != 2)
	{
		if (this->cli == 0 || !this->recvRunning)
		{
			return 0;
		}
		Sync::Thread::Sleep(10);
	}
	MySQLTCPReader *reader;
	Sync::MutexUsage *mutUsage;
	NEW_CLASS(mutUsage, Sync::MutexUsage(this->cmdMut));
	this->cmdResultType = CmdResultType::Processing;
	this->cmdSeqNum = 1;
	NEW_CLASS(reader, MySQLTCPReader(mutUsage));
	this->cmdReader = reader;
	UInt8 *buff = MemAlloc(UInt8, sqlLen + 5);
	WriteInt32(buff, (Int32)(sqlLen + 1));
	buff[4] = 3;
	MemCopyNO(&buff[5], sql, sqlLen);
	if (this->cli->Write(buff, 5 + sqlLen) != 5 + sqlLen)
	{
		this->cmdReader = 0;
		DEL_CLASS(reader);
		MemFree(buff);
		this->lastDataError = DE_CONN_ERROR;
		return 0;
	}
	MemFree(buff);
#if defined(VERBOSE)
	printf("MySQLTCP Sent SQL: %s\r\n", sql);
#endif
//	dt.SetCurrTimeUTC();
//	startTime = dt.ToTicks();
	while (this->cmdResultType == CmdResultType::Processing)
	{
		this->cmdEvt->Wait(10000);
	}
	if (this->cmdResultType == CmdResultType::Error)
	{
		this->cmdReader = 0;
		reader->EndData();
		DEL_CLASS(reader);
		this->lastDataError = DE_EXEC_SQL_ERROR;
		return 0;
	}
	this->lastDataError = DE_NO_ERROR;
	return reader;
}

DB::DBReader *Net::MySQLTCPClient::ExecuteReaderBinaryC(const UTF8Char *sql, UOSInt sqlLen)
{
	if (this->cli == 0 || !this->recvRunning)
	{
		this->lastDataError = DE_CONN_ERROR;
		return 0;
	}
	while (this->mode != 2)
	{
		if (this->cli == 0 || !this->recvRunning)
		{
			return 0;
		}
		Sync::Thread::Sleep(10);
	}
	MySQLTCPBinaryReader *reader;
	Sync::MutexUsage *mutUsage;
	NEW_CLASS(mutUsage, Sync::MutexUsage(this->cmdMut));
	this->cmdResultType = CmdResultType::ProcessingBinary;
	this->cmdSeqNum = 1;
	NEW_CLASS(reader, MySQLTCPBinaryReader(mutUsage));
	this->cmdReader = reader;
	UInt8 *buff = MemAlloc(UInt8, sqlLen + 5);
	WriteInt32(buff, (Int32)(sqlLen + 1));
	buff[4] = 22;
	MemCopyNO(&buff[5], sql, sqlLen);
	if (this->cli->Write(buff, 5 + sqlLen) != 5 + sqlLen)
	{
		this->cmdReader = 0;
		DEL_CLASS(reader);
		MemFree(buff);
		this->lastDataError = DE_CONN_ERROR;
		return 0;
	}
	MemFree(buff);
#if defined(VERBOSE)
	printf("MySQLTCP Sent Prepare Stmt: %s\r\n", sql);
#endif
//	dt.SetCurrTimeUTC();
//	startTime = dt.ToTicks();
	while (this->cmdResultType == CmdResultType::ProcessingBinary || this->cmdResultType == CmdResultType::BinaryExecuting)
	{
		this->cmdEvt->Wait(10000);
	}
	if (this->cmdResultType == CmdResultType::Error)
	{
		this->cmdReader = 0;
		reader->EndData();
		DEL_CLASS(reader);
		this->lastDataError = DE_EXEC_SQL_ERROR;
		return 0;
	}
	this->lastDataError = DE_NO_ERROR;
	return reader;
}

void Net::MySQLTCPClient::CloseReader(DB::DBReader *r)
{
	while (r->ReadNext())
	{
	}
	while (this->cmdResultType != CmdResultType::ResultEnd && this->cmdResultType != CmdResultType::Error)
	{
		this->cmdEvt->Wait(10000);
	}
	this->cmdReader = 0;
	MySQLTCPReader *reader = (MySQLTCPReader*)r;
	DEL_CLASS(reader);
}

void Net::MySQLTCPClient::GetErrorMsg(Text::StringBuilderUTF8 *str)
{
	if (this->lastError)
	{
		if (this->lastError->v[0] == '#')
		{
			str->AppendChar('[', 1);
			str->AppendC(&this->lastError->v[1], 5);
			str->AppendChar(']', 1);
			str->AppendChar(' ', 1);
			str->AppendC(&this->lastError->v[6], this->lastError->leng - 6);
		}
		else
		{
			str->Append(this->lastError);
		}
	}
}

Bool Net::MySQLTCPClient::IsLastDataError()
{
	return this->lastDataError == DE_EXEC_SQL_ERROR;
}

void Net::MySQLTCPClient::Reconnect()
{
	if (this->cli && this->recvRunning)
	{
		return;
	}
	if (this->cli)
	{
		DEL_CLASS(this->cli);
		this->cli = 0;
	}
	this->recvRunning = false;
	this->recvStarted = false;
	this->mode = 0;
	SDEL_STRING(this->svrVer);
	NEW_CLASS(this->cli, Net::TCPClient(this->sockf, &this->addr, this->port));
	if (this->cli->IsConnectError())
	{
		DEL_CLASS(this->cli);
		this->cli = 0;
		this->SetLastError(UTF8STRC("Cannot connect to server"));
	}
	else
	{
		this->cli->SetNoDelay(true);
		Sync::Thread::Create(RecvThread, this);
		while (!this->recvStarted)
		{
			Sync::Thread::Sleep(1);
		}
	}
}

void *Net::MySQLTCPClient::BeginTransaction()
{
	if (this->ExecuteNonQueryC(UTF8STRC("START TRANSACTION")) != -2)
	{
		return (void*)-1;
	}
	return 0;
}
void Net::MySQLTCPClient::Commit(void *tran)
{
	this->ExecuteNonQueryC(UTF8STRC("COMMIT"));
}

void Net::MySQLTCPClient::Rollback(void *tran)
{
	this->ExecuteNonQueryC(UTF8STRC("ROLLBACK"));
}

UOSInt Net::MySQLTCPClient::GetTableNames(Data::ArrayList<const UTF8Char*> *names)
{
	if (this->tableNames)
	{
		names->AddAll(this->tableNames);
		return this->tableNames->GetCount();
	}
	else
	{
		UTF8Char sbuff[256];
		DB::DBReader *rdr = this->ExecuteReaderC(UTF8STRC("show tables"));
		NEW_CLASS(this->tableNames, Data::ArrayList<const UTF8Char*>());
		if (rdr)
		{
			while (rdr->ReadNext())
			{
				rdr->GetStr(0, sbuff, sizeof(sbuff));
				this->tableNames->Add(Text::StrCopyNew(sbuff));
			}
			this->CloseReader(rdr);
		}
		names->AddAll(this->tableNames);
		return this->tableNames->GetCount();
	}
}

DB::DBReader *Net::MySQLTCPClient::GetTableData(const UTF8Char *tableName, Data::ArrayList<Text::String*> *columnNames, UOSInt ofst, UOSInt maxCnt, const UTF8Char *ordering, Data::QueryConditions *condition)
{
	UTF8Char sbuff[512];
	UTF8Char *sptr;
	Text::StringBuilderUTF8 sb;
	UOSInt i;
	UOSInt j;
	sb.AppendC(UTF8STRC("select "));
	if (columnNames == 0 || columnNames->GetCount() == 0)
	{
		sb.AppendC(UTF8STRC("*"));
	}
	else
	{
		i = 0;
		j = columnNames->GetCount();
		while (i < j)
		{
			if (i > 0)
			{
				sb.AppendChar(',', 1);
			}
			sptr = DB::DBUtil::SDBColUTF8(sbuff, columnNames->GetItem(i)->v, DB::DBUtil::ServerType::MySQL);
			sb.AppendC(sbuff, (UOSInt)(sptr - sbuff));
			i++;
		}
	}
	sb.AppendC(UTF8STRC(" from "));
	sptr = DB::DBUtil::SDBColUTF8(sbuff, tableName, DB::DBUtil::ServerType::MySQL);
	sb.AppendC(sbuff, (UOSInt)(sptr - sbuff));
	if (maxCnt > 0)
	{
		sb.AppendC(UTF8STRC(" LIMIT "));
		sb.AppendUOSInt(maxCnt);
	}
	return this->ExecuteReaderC(sb.ToString(), sb.GetLength());
}

Bool Net::MySQLTCPClient::ChangeSchema(const UTF8Char *schemaName)
{
	UTF8Char sbuff[128];
	UTF8Char *sptr2;
	Text::StringBuilderUTF8 sb;
	sb.AppendC(UTF8STRC("use "));
	UOSInt colLen = DB::DBUtil::SDBColUTF8Leng(schemaName, DB::DBUtil::ServerType::MySQL);
	if (colLen > 127)
	{
		UTF8Char *sptr = MemAlloc(UTF8Char, colLen + 1);
		sptr2 = DB::DBUtil::SDBColUTF8(sptr, schemaName, DB::DBUtil::ServerType::MySQL);
		sb.AppendC(sptr, (UOSInt)(sptr2 - sptr));
		MemFree(sptr);
	}
	else
	{
		sptr2 = DB::DBUtil::SDBColUTF8(sbuff, schemaName, DB::DBUtil::ServerType::MySQL);
		sb.AppendC(sbuff, (UOSInt)(sptr2 - sbuff));
	}
	if (this->ExecuteNonQueryC(sb.ToString(), sb.GetLength()) >= 0)
	{
		if (this->tableNames)
		{
			LIST_FREE_FUNC(this->tableNames, Text::StrDelNew);
			DEL_CLASS(this->tableNames);
			this->tableNames = 0;
		}
		return true;
	}
	else
	{
		return false;
	}
}

Bool Net::MySQLTCPClient::IsError()
{
	return this->cli == 0 || !this->recvRunning;
}

Bool Net::MySQLTCPClient::ServerInfoRecv()
{
	return this->mode >= 1;
}

Text::String *Net::MySQLTCPClient::GetServerVer()
{
	return this->svrVer;
}

UInt32 Net::MySQLTCPClient::GetConnId()
{
	return this->connId;
}

UOSInt Net::MySQLTCPClient::GetAuthPluginData(UInt8 *buff)
{
	MemCopyNO(buff, this->authPluginData, this->authPluginDataSize);
	return this->authPluginDataSize;
}

UInt32 Net::MySQLTCPClient::GetServerCap()
{
	return this->svrCap;
}

UInt16 Net::MySQLTCPClient::GetServerCS()
{
	return this->svrCS;
}

const Net::SocketUtil::AddressInfo *Net::MySQLTCPClient::GetConnAddr()
{
	return &this->addr;
}

UInt16 Net::MySQLTCPClient::GetConnPort()
{
	return this->port;
}

Text::String *Net::MySQLTCPClient::GetConnDB()
{
	return this->database;
}

Text::String *Net::MySQLTCPClient::GetConnUID()
{
	return this->userName;
}

Text::String *Net::MySQLTCPClient::GetConnPWD()
{
	return this->password;
}

UInt16 Net::MySQLTCPClient::GetDefaultPort()
{
	return 3306;
}

DB::DBTool *Net::MySQLTCPClient::CreateDBTool(Net::SocketFactory *sockf, Text::String *serverName, Text::String *dbName, Text::String *uid, Text::String *pwd, IO::LogTool *log, Text::CString logPrefix)
{
	Net::MySQLTCPClient *conn;
	DB::DBTool *db;
	Net::SocketUtil::AddressInfo addr;
	if (sockf->DNSResolveIP(serverName->v, serverName->leng, &addr))
	{
		NEW_CLASS(conn, Net::MySQLTCPClient(sockf, &addr, 3306, uid, pwd, dbName));
		if (conn->IsError() == 0)
		{
			NEW_CLASS(db, DB::DBTool(conn, true, log, logPrefix));
			return db;
		}
		else
		{
			DEL_CLASS(conn);
			return 0;
		}
	}
	else
	{
		return 0;
	}
}

DB::DBTool *Net::MySQLTCPClient::CreateDBTool(Net::SocketFactory *sockf, Text::CString serverName, Text::CString dbName, Text::CString uid, Text::CString pwd, IO::LogTool *log, Text::CString logPrefix)
{
	Net::MySQLTCPClient *conn;
	DB::DBTool *db;
	Net::SocketUtil::AddressInfo addr;
	if (sockf->DNSResolveIP(serverName.v, serverName.leng, &addr))
	{
		NEW_CLASS(conn, Net::MySQLTCPClient(sockf, &addr, 3306, uid, pwd, dbName));
		if (conn->IsError() == 0)
		{
			NEW_CLASS(db, DB::DBTool(conn, true, log, logPrefix));
			return db;
		}
		else
		{
			DEL_CLASS(conn);
			return 0;
		}
	}
	else
	{
		return 0;
	}
}
