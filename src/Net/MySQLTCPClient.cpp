#include "Stdafx.h"
#include "Data/ByteTool.h"
#include "Data/DateTime.h"
#include "DB/ColDef.h"
#include "DB/DBUtil.h"
#include "IO/OS.h"
#include "Math/WKBReader.h"
#include "Net/MySQLTCPClient.h"
#include "Net/MySQLUtil.h"
#include "Net/SocketUtil.h"
#include "Sync/MutexUsage.h"
#include "Sync/SimpleThread.h"
#include "Sync/ThreadUtil.h"
#include "Text/MyString.h"
#include "Text/MyStringFloat.h"
#include "Text/MyStringW.h"

#define CLIVERSION "1.0.0"
#define INITBUFFSIZE 65536
#define ROWBUFFCNT 4

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

namespace Net
{
	class MySQLTCPReader : public DB::DBReader
	{
	private:
		typedef struct
		{
			NotNullPtr<Text::String> name;
			Text::String *defValues;
			UInt32 colLen;
			UInt16 charSet;
			UInt16 flags;
			Net::MySQLUtil::MySQLType colType;
			DB::DBUtil::ColType dbColType;
			UInt8 decimals;
		} ColumnDef;

		Data::ArrayList<ColumnDef*> cols;
		UOSInt colCount;
		OSInt rowChanged;
		Text::String **currRow;
		Text::String **nextRow;
		Bool nextRowReady;
		Sync::Event rowEvt;
		Sync::Event nextRowEvt;
		Sync::MutexUsage mutUsage;
	public:
		MySQLTCPReader(NotNullPtr<Sync::Mutex> mut)
		{
			this->mutUsage.ReplaceMutex(mut);
			this->rowChanged = -1;
			this->currRow = 0;
			this->nextRow = 0;
			this->colCount = 0;
			this->nextRowReady = false;
		}

		virtual ~MySQLTCPReader()
		{
			ColumnDef *col;
			while (this->ReadNext())
			{

			}
			UOSInt i = this->cols.GetCount();
			while (i-- > 0)
			{
				col = this->cols.GetItem(i);
				col->name->Release();
				SDEL_STRING(col->defValues);
				MemFree(col); 
			}
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
				this->rowEvt.Wait(1000);
			}
			if (this->nextRow)
			{
				this->currRow = this->nextRow;
				this->nextRowReady = false;
				this->nextRowEvt.Set();
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

		virtual Bool GetStr(UOSInt colIndex, NotNullPtr<Text::StringBuilderUTF8> sb)
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

		virtual Optional<Text::String> GetNewStr(UOSInt colIndex)
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

		virtual Data::Timestamp GetTimestamp(UOSInt colIndex)
		{
			if (this->currRow == 0)
			{
				return Data::Timestamp(0);
			}
			if (colIndex >= this->colCount)
			{
				return Data::Timestamp(0);
			}
			if (this->currRow[colIndex] == 0)
			{
				return Data::Timestamp(0);
			}
			return Data::Timestamp(this->currRow[colIndex]->ToCString(), 0);
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

		virtual Optional<Math::Geometry::Vector2D> GetVector(UOSInt colIndex)
		{
			/////////////////////////////
			return 0;
		}

		virtual Bool GetUUID(UOSInt colIndex, NotNullPtr<Data::UUID> uuid)
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
			ColumnDef *col = this->cols.GetItem(colIndex);
			if (col)
			{
				return col->name->ConcatTo(buff);
			}
			return 0;
		}

		virtual DB::DBUtil::ColType GetColType(UOSInt colIndex, OptOut<UOSInt> colSize)
		{
			ColumnDef *col = this->cols.GetItem(colIndex);
			if (col)
			{
				colSize.Set(col->colLen);
				return col->dbColType;
			}
			return DB::DBUtil::CT_Unknown;
		}

		virtual Bool GetColDef(UOSInt colIndex, NotNullPtr<DB::ColDef> colDef)
		{
			ColumnDef *col = this->cols.GetItem(colIndex);
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
					col->defValues = Text::String::New(colDef, (UOSInt)v).Ptr();
				}
			}
			this->cols.Add(col);
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
					row[i] = Text::String::New(rowData, (UOSInt)v).Ptr();
					rowData += v;
				}
				i++;
			}
			while (this->nextRowReady)
			{
				this->nextRowEvt.Wait(1000);
			}
			this->nextRow = row;
			this->nextRowReady = true;
			this->rowEvt.Set();
		}

		void EndData()
		{
			if (this->nextRow == 0 && this->nextRowReady)
			{
				return;
			}
			while (this->nextRowReady)
			{
				this->nextRowEvt.Wait(1000);
			}
			this->nextRow = 0;
			this->nextRowReady = true;
			this->rowEvt.Set();
		}
	};


	class MySQLTCPBinaryReader : public DB::DBReader
	{
	private:
		typedef struct
		{
			NotNullPtr<Text::String> name;
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
			UOSInt rowNum;
		} RowData;	

		Data::ArrayList<ColumnDef*> cols;
		Net::MySQLUtil::MySQLType *colTypes;
		UOSInt colCount;
		OSInt rowChanged;
		RowData *currRow;
		RowData *nextRow[ROWBUFFCNT];
		Bool nextRowReady[ROWBUFFCNT];
		RowData *preparingRow;
		Sync::Event rowEvt;
		Sync::Event nextRowEvt;
		Sync::MutexUsage mutUsage;
		UOSInt rowNum;
		UInt32 stmtId;
	public:
		MySQLTCPBinaryReader(NotNullPtr<Sync::Mutex> mut)
		{
			this->mutUsage.ReplaceMutex(mut);
			this->colTypes = 0;
			this->rowChanged = -1;
			this->currRow = 0;
			this->rowNum = 0;
			UOSInt i = ROWBUFFCNT;
			while (i-- > 0)
			{
				this->nextRow[i] = 0;
				this->nextRowReady[i] = false;
			}
			this->preparingRow = 0;
			this->colCount = 0;
			this->stmtId = 0;
		}

		virtual ~MySQLTCPBinaryReader()
		{
			ColumnDef *col;
			this->WaitForDataEnd();
			UOSInt i = this->cols.GetCount();
			while (i-- > 0)
			{
				col = this->cols.GetItem(i);
				col->name->Release();
				SDEL_STRING(col->defValues);
				MemFree(col); 
			}
			i = ROWBUFFCNT;
			while (i-- > 0)
			{
				if (this->nextRow[i])
				{
					MemFree(this->nextRow[i]->cols);
					MemFree(this->nextRow[i]->rowBuff);
					MemFree(this->nextRow[i]);
					this->nextRow[i] = 0;
				}
			}
			if (this->currRow)
			{
				MemFree(this->currRow->cols);
				MemFree(this->currRow->rowBuff);
				MemFree(this->currRow);
				this->currRow = 0;
			}
			if (this->preparingRow)
			{
				MemFree(this->preparingRow->cols);
				MemFree(this->preparingRow->rowBuff);
				MemFree(this->preparingRow);
				this->preparingRow = 0;
			}
			if (this->colTypes)
			{
				MemFree(this->colTypes);
				this->colTypes = 0;
			}
		}

		void WaitForDataEnd()
		{
			while (this->ReadNext())
			{

			}
		}

		virtual Bool ReadNext()
		{
			UOSInt rowIndex = INVALID_INDEX;
			UOSInt i;
			UOSInt minRowNum = INVALID_INDEX;
			while (true)
			{
				i = ROWBUFFCNT;
				while (i-- > 0)
				{
					if (this->nextRowReady[i])
					{
						if (this->nextRow[i] == 0)
						{
							rowIndex = i;
							break;
						}
						else if (this->nextRow[i]->rowNum < minRowNum)
						{
							rowIndex = i;
							minRowNum = this->nextRow[i]->rowNum;
						}
					}
				}
				if (rowIndex != INVALID_INDEX)
				{
					break;
				}
				this->rowEvt.Wait(1000);
			}
			if (this->nextRow[rowIndex])
			{
				RowData *row = this->currRow;
				this->currRow = this->nextRow[rowIndex];
				this->nextRow[rowIndex] = row;
				this->nextRowReady[rowIndex] = false;
				this->nextRowEvt.Set();
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
			if (!this->GetVariItem(colIndex, item))
			{
				return 0;
			}
			return item.GetAsI32();
		}

		virtual Int64 GetInt64(UOSInt colIndex)
		{
			Data::VariItem item;
			if (!this->GetVariItem(colIndex, item))
			{
				return 0;
			}
			return item.GetAsI64();
		}

		virtual WChar *GetStr(UOSInt colIndex, WChar *buff)
		{
			Data::VariItem item;
			if (!this->GetVariItem(colIndex, item))
			{
				return 0;
			}
			Data::VariItem::ItemType itemType = item.GetItemType();
			if (itemType == Data::VariItem::ItemType::Null)
			{
				return 0;
			}
			Text::StringBuilderUTF8 sb;
			item.GetAsString(sb);
			return Text::StrUTF8_WChar(buff, sb.ToString(), 0);
		}

		virtual Bool GetStr(UOSInt colIndex, NotNullPtr<Text::StringBuilderUTF8> sb)
		{
			Data::VariItem item;
			if (!this->GetVariItem(colIndex, item))
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

		virtual Optional<Text::String> GetNewStr(UOSInt colIndex)
		{
			Data::VariItem item;
			if (!this->GetVariItem(colIndex, item))
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
				item.GetAsString(sb);
				return Text::String::New(sb.ToString(), sb.GetLength());
			}
		}

		virtual UTF8Char *GetStr(UOSInt colIndex, UTF8Char *buff, UOSInt buffSize)
		{
			Data::VariItem item;
			if (!this->GetVariItem(colIndex, item))
			{
				return 0;
			}
			if (item.GetItemType() == Data::VariItem::ItemType::Null)
			{
				return 0;
			}
			return item.GetAsStringS(buff, buffSize);
		}

		virtual Data::Timestamp GetTimestamp(UOSInt colIndex)
		{
			if (this->currRow == 0)
			{
				return Data::Timestamp(0);
			}
			if (colIndex >= this->colCount)
			{
				return Data::Timestamp(0);
			}
			RowColumn *col = &this->currRow->cols[colIndex];
			if (col->isNull)
			{
				return Data::Timestamp(0);
			}
			if (this->colTypes[colIndex] == Net::MySQLUtil::MYSQL_TYPE_DATE ||
				this->colTypes[colIndex] == Net::MySQLUtil::MYSQL_TYPE_DATETIME ||
				this->colTypes[colIndex] == Net::MySQLUtil::MYSQL_TYPE_TIMESTAMP)
			{
				Data::DateTimeUtil::TimeValue tval;
				UInt32 microsec;
				switch (col->len)
				{
				case 0:
					return Data::Timestamp(0);
				case 4:
					tval.year = ReadUInt16(&this->currRow->rowBuff[col->ofst]);
					tval.month = this->currRow->rowBuff[col->ofst + 2];
					tval.day = this->currRow->rowBuff[col->ofst + 3];
					tval.hour = 0;
					tval.minute = 0;
					tval.second = 0;
					return Data::Timestamp(Data::TimeInstant(Data::DateTimeUtil::TimeValue2Secs(tval, 0), 0), 0);
				case 7:
					tval.year = ReadUInt16(&this->currRow->rowBuff[col->ofst]);
					tval.month = this->currRow->rowBuff[col->ofst + 2];
					tval.day = this->currRow->rowBuff[col->ofst + 3];
					tval.hour = this->currRow->rowBuff[col->ofst + 4];
					tval.minute = this->currRow->rowBuff[col->ofst + 5];
					tval.second = this->currRow->rowBuff[col->ofst + 6];
					return Data::Timestamp(Data::TimeInstant(Data::DateTimeUtil::TimeValue2Secs(tval, 0), 0), 0);
				case 11:
					tval.year = ReadUInt16(&this->currRow->rowBuff[col->ofst]);
					tval.month = this->currRow->rowBuff[col->ofst + 2];
					tval.day = this->currRow->rowBuff[col->ofst + 3];
					tval.hour = this->currRow->rowBuff[col->ofst + 4];
					tval.minute = this->currRow->rowBuff[col->ofst + 5];
					tval.second = this->currRow->rowBuff[col->ofst + 6];
					microsec = ReadUInt32(&this->currRow->rowBuff[col->ofst + 7]);
					return Data::Timestamp(Data::TimeInstant(Data::DateTimeUtil::TimeValue2Secs(tval, 0), microsec * 1000), 0);
				default:
					//////////////////////////////////////
					printf("Unknown binary date format\r\n");
				return Data::Timestamp(0);
				}
			}
			else
			{
				return Data::Timestamp(0);
			}
		}

		virtual Double GetDbl(UOSInt colIndex)
		{
			Data::VariItem item;
			if (!this->GetVariItem(colIndex, item))
			{
				return 0;
			}
			return item.GetAsF64();
		}

		virtual Bool GetBool(UOSInt colIndex)
		{
			Data::VariItem item;
			if (!this->GetVariItem(colIndex, item))
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

		virtual Optional<Math::Geometry::Vector2D> GetVector(UOSInt colIndex)
		{
			Data::VariItem item;
			if (!this->GetVariItem(colIndex, item))
			{
				return 0;
			}
			return item.GetAndRemoveVector();
		}

		virtual Bool GetUUID(UOSInt colIndex, NotNullPtr<Data::UUID> uuid)
		{
			Data::VariItem item;
			if (!this->GetVariItem(colIndex, item))
			{
				return false;
			}
			NotNullPtr<Data::UUID> itemUUID;
			if (!itemUUID.Set(item.GetAndRemoveUUID()))
			{
				return false;
			}
			uuid->SetValue(itemUUID);
			itemUUID.Delete();
			return true;
		}

		virtual Bool GetVariItem(UOSInt colIndex, NotNullPtr<Data::VariItem> item)
		{
			if (this->currRow == 0 || colIndex >= this->colCount)
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
				switch (col->len)
				{
				case 0:

					item->SetDate(Data::Timestamp(0));
					return true;
				case 4:
					{
						Data::DateTimeUtil::TimeValue tval;
						tval.year = ReadUInt16(&this->currRow->rowBuff[col->ofst]);
						tval.month = this->currRow->rowBuff[col->ofst + 2];
						tval.day = this->currRow->rowBuff[col->ofst + 3];
						tval.hour = 0;
						tval.minute = 0;
						tval.second = 0;
						item->SetDate(Data::Timestamp::FromTimeValue(tval, 0, 0));
					}
					return true;
				case 7:
					{
						Data::DateTimeUtil::TimeValue tval;
						tval.year = ReadUInt16(&this->currRow->rowBuff[col->ofst]);
						tval.month = this->currRow->rowBuff[col->ofst + 2];
						tval.day = this->currRow->rowBuff[col->ofst + 3];
						tval.hour = this->currRow->rowBuff[col->ofst + 4];
						tval.minute = this->currRow->rowBuff[col->ofst + 5];
						tval.second = this->currRow->rowBuff[col->ofst + 6];
						item->SetDate(Data::Timestamp::FromTimeValue(tval, 0, 0));
					}
					return true;
				case 11:
					{
						UInt32 us;
						Data::DateTimeUtil::TimeValue tval;
						tval.year = ReadUInt16(&this->currRow->rowBuff[col->ofst]);
						tval.month = this->currRow->rowBuff[col->ofst + 2];
						tval.day = this->currRow->rowBuff[col->ofst + 3];
						tval.hour = this->currRow->rowBuff[col->ofst + 4];
						tval.minute = this->currRow->rowBuff[col->ofst + 5];
						tval.second = this->currRow->rowBuff[col->ofst + 6];
						us = ReadUInt32(&this->currRow->rowBuff[col->ofst + 7]);
						item->SetDate(Data::Timestamp::FromTimeValue(tval, us * 1000, 0));
					}
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

			case Net::MySQLUtil::MYSQL_TYPE_GEOMETRY:
				if (col->len > 4)
				{
					Math::WKBReader wkb(ReadUInt32(&this->currRow->rowBuff[col->ofst]));
					NotNullPtr<Math::Geometry::Vector2D> vec;
					if (wkb.ParseWKB(&this->currRow->rowBuff[col->ofst + 4], col->len - 4, 0).SetTo(vec))
					{
						item->SetVectorDirect(vec);
						return true;
					}
					else
					{
						printf("Unknown binary geometry format\r\n");
						item->SetNull();
						return true;
					}
				}
				else
				{
					printf("Unknown binary geometry format\r\n");
					item->SetNull();
					return true;
				}
			case Net::MySQLUtil::MYSQL_TYPE_NEWDECIMAL:
				item->SetStrCopy(&this->currRow->rowBuff[col->ofst], col->len);
				return true;
			case Net::MySQLUtil::MYSQL_TYPE_ENUM:
			case Net::MySQLUtil::MYSQL_TYPE_SET:
			case Net::MySQLUtil::MYSQL_TYPE_BIT:
			case Net::MySQLUtil::MYSQL_TYPE_DECIMAL:
			default:
				////////////////////////////////////
				{
					Text::StringBuilderUTF8 sb;
					sb.AppendHexBuff(&this->currRow->rowBuff[col->ofst], col->len, ' ', Text::LineBreakType::None);
					printf("Unknown binary other format %d: %s\r\n", this->colTypes[colIndex], sb.ToString());
				}
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
			ColumnDef *col = this->cols.GetItem(colIndex);
			if (col)
			{
				return Text::StrConcat(buff, col->name->v);
			}
			return 0;
		}

		virtual DB::DBUtil::ColType GetColType(UOSInt colIndex, OptOut<UOSInt> colSize)
		{
			ColumnDef *col = this->cols.GetItem(colIndex);
			if (col)
			{
				colSize.Set(col->colLen);
				return col->dbColType;
			}
			return DB::DBUtil::CT_Unknown;
		}

		virtual Bool GetColDef(UOSInt colIndex, NotNullPtr<DB::ColDef> colDef)
		{
			ColumnDef *col = this->cols.GetItem(colIndex);
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
			this->nextRowReady[0] = true;
		}

		Bool IsDataEnd()
		{
			return this->nextRow[0] == 0 && this->nextRowReady[0];
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
					col->defValues = Text::String::New(colDef, (UOSInt)v).Ptr();
				}
			}
			this->cols.Add(col);
			this->colCount++;
		}

		void AddRowData(const UInt8 *rowData, UOSInt dataSize)
		{
			RowData *row;
			if (this->IsDataEnd())
				return;
			if (this->preparingRow == 0)
			{
				row = this->preparingRow = MemAlloc(RowData, 1);
				row->cols = MemAlloc(RowColumn, this->colCount);
				row->rowBuffCapacity = dataSize * 2;
				row->rowBuff = MemAlloc(UInt8, dataSize * 2);
			}
			else
			{
				row = this->preparingRow;
				if (row->rowBuffCapacity < dataSize)
				{
					row->rowBuffCapacity = dataSize;
					MemFree(row->rowBuff);
					row->rowBuff = MemAlloc(UInt8, dataSize);
				}
			}
			MemCopyNO(row->rowBuff, rowData, dataSize);
			row->rowNum = this->rowNum++;
			UOSInt i = 0;
			UOSInt j = this->colCount;
			UInt64 v;
			UOSInt nullOfst = 1;
			UOSInt nullBitOfst = 2;
			UOSInt tableOfst = 1 + ((j + 2 + 7) >> 3);
			RowColumn *col;
			while (i < j)
			{
				col = &row->cols[i];
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
			while (true)
			{
				i = ROWBUFFCNT;
				while (i-- > 0)
				{
					if (!this->nextRowReady[i])
						break;
				}
				if (i == INVALID_INDEX)
				{
					this->nextRowEvt.Wait(1000);
				}
				else
				{
					break;
				}
			}
			row = this->nextRow[i];
			this->nextRow[i] = this->preparingRow;
			this->preparingRow = row;
			this->nextRowReady[i] = true;
			this->rowEvt.Set();
		}

		void EndCols()
		{
			if (this->colTypes == 0)
			{
				this->colTypes = MemAlloc(Net::MySQLUtil::MySQLType, this->colCount);
				UOSInt i = this->colCount;
				while (i-- > 0)
				{
					this->colTypes[i] = this->cols.GetItem(i)->colType;
				}
			}
		}

		void EndData()
		{
			if (this->IsDataEnd())
			{
				return;
			}
			UOSInt i;
			while (true)
			{
				i = ROWBUFFCNT;
				while (i-- > 0)
				{
					if (this->nextRowReady[i])
						break;
				}
				if (i == INVALID_INDEX)
					break;
				this->nextRowEvt.Wait(1000);
			}
			i = ROWBUFFCNT;
			while (i-- > 0)
			{
				RowData *row = this->nextRow[i];
				this->nextRow[i] = 0;
				this->nextRowReady[i] = true;

				if (row)
				{
					MemFree(row->cols);
					MemFree(row->rowBuff);
					MemFree(row);
				}
			}
			this->rowEvt.Set();
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
}

UInt32 __stdcall Net::MySQLTCPClient::RecvThread(void *userObj)
{
	Net::MySQLTCPClient *me = (Net::MySQLTCPClient*)userObj;
	UInt8 *buff;
	UOSInt buffCapacity = INITBUFFSIZE;
	UOSInt buffSize;
	UOSInt readSize;
	UTF8Char sbuff[512];
	UTF8Char *sptr;
	UTF8Char sbuff2[128];
	UTF8Char *sptr2;
	UInt8 *ptrCurr;
	UInt8 *ptrEnd;
	{
		me->recvStarted = true;
		me->recvRunning = true;
		buffSize = 0;
	#if defined(VERBOSE)
		Text::StringBuilderUTF8 sb;
	#endif
		buff = MemAlloc(UInt8, buffCapacity);
		while (true)
		{
			readSize = me->cli->Read(Data::ByteArray(&buff[buffSize], buffCapacity - buffSize));
			if (readSize <= 0)
			{
	#if defined(VERBOSE)
				printf("MySQLTCP %d Disconnected, buffSize = %d\r\n", me->cli->GetLocalPort(), (UInt32)buffSize);
	#endif
				break;
			}
	#if defined(VERBOSE)
			sb.ClearStr();
			sb.AppendHexBuff(&buff[buffSize], readSize, ' ', Text::LineBreakType::CRLF);
			printf("MySQLTCP %d Received Buff:\r\n%s\r\n", me->cli->GetLocalPort(), sb.ToString());
	#endif
			buffSize += readSize;

			if (me->mode == ClientMode::Handshake)
			{
				readSize = buffSize;
				if (buffSize >= 10)
				{
					if (buff[3] != 0)
					{
	#if defined(VERBOSE)
						printf("MySQLTCP %d Seq Id Invalid\r\n", me->cli->GetLocalPort());
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
							printf("MySQLTCP %d packet size Invalid\r\n", me->cli->GetLocalPort());
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
									printf("MySQLTCP %d protocol ver 9 invalid\r\n", me->cli->GetLocalPort());
	#endif
									me->cli->Close();
									readSize = 0;
								}
								else
								{
									me->svrVer = Text::String::NewNotNullSlow(&buff[5]).Ptr();
									me->connId = ReadUInt32(&buff[packetSize - 9]);
									MemCopyNO(me->authPluginData, &buff[packetSize - 5], 8);
									me->authPluginDataSize = 8;
									me->mode = ClientMode::Authen;
									me->axisAware = Net::MySQLUtil::IsAxisAware(me->svrVer->ToCString());
									////////////////////////////////
	#if defined(VERBOSE)
									printf("MySQLTCP %d Server ver = %s\r\n", me->cli->GetLocalPort(), me->svrVer->v);
									printf("MySQLTCP %d Conn Id = %d\r\n", me->cli->GetLocalPort(), me->connId);
									sb.ClearStr();
									sb.AppendHexBuff(me->authPluginData, me->authPluginDataSize, ' ', Text::LineBreakType::None);
									printf("MySQLTCP %d Auth Plugin Data = %s\r\n", me->cli->GetLocalPort(), sb.ToString());
									printf("MySQLTCP %d Axis-Aware = %d\r\n", me->cli->GetLocalPort(), me->axisAware?1:0);
									printf("MySQLTCP %d Axis-Aware = %d\r\n", me->cli->GetLocalPort(), me->axisAware?1:0);
	#endif
									readSize = 0;
								}
							}
							else if (buff[4] == 10)
							{
								ptrEnd = &buff[packetSize + 4];
								sptr = Text::StrConcatS(sbuff, &buff[5], packetSize - 1);
								ptrCurr = &buff[6] + (sptr - sbuff);
								me->svrVer = Text::String::New(sbuff, (UOSInt)(sptr - sbuff)).Ptr();
								me->axisAware = Net::MySQLUtil::IsAxisAware(me->svrVer->ToCString());
	#if defined(VERBOSE)
								printf("MySQLTCP %d Server ver = %s\r\n", me->cli->GetLocalPort(), me->svrVer->v);
								printf("MySQLTCP %d Axis-Aware = %d\r\n", me->cli->GetLocalPort(), me->axisAware?1:0);
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
											sptr = Text::StrConcatS(sbuff, ptrCurr, (UOSInt)(ptrEnd - ptrCurr));
											me->authenType = Net::MySQLUtil::AuthenTypeParse(CSTRP(sbuff, sptr));
										}
										else
										{
											sbuff[0] = 0;
										}
	#if defined(VERBOSE)
										printf("MySQLTCP %d Conn Id = %d\r\n", me->cli->GetLocalPort(), me->connId);
										printf("MySQLTCP %d Server Cap = 0x%x\r\n", me->cli->GetLocalPort(), me->svrCap);
										printf("MySQLTCP %d character set = %d\r\n", me->cli->GetLocalPort(), me->svrCS);
										printf("MySQLTCP %d status = 0x%x\r\n", me->cli->GetLocalPort(), me->connStatus);
										sb.ClearStr();
										sb.AppendHexBuff(me->authPluginData, me->authPluginDataSize, ' ', Text::LineBreakType::None);
										printf("MySQLTCP %d auth plugin data = %s\r\n", me->cli->GetLocalPort(), sb.ToString());
										printf("MySQLTCP %d auth plugin name = %s\r\n", me->cli->GetLocalPort(), sbuff);
	#endif
									}
									else
									{
	#if defined(VERBOSE)
										printf("MySQLTCP %d Conn Id = %d\r\n", me->cli->GetLocalPort(), me->connId);
										printf("MySQLTCP %d Server Cap = 0x%x\r\n", me->cli->GetLocalPort(), me->svrCap);
										sb.ClearStr();
										sb.AppendHexBuff(me->authPluginData, me->authPluginDataSize, ' ', Text::LineBreakType::None);
										printf("MySQLTCP %d auth plugin data = %s\r\n", me->cli->GetLocalPort(), sb.ToString());
	#endif
									}
									me->mode = ClientMode::Authen;

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
										if (!me->database.IsNull())
										{
											cliCap |= Net::MySQLUtil::CLIENT_CONNECT_WITH_DB;
										}
										WriteUInt32(&buff[4], cliCap);
										WriteInt32(&buff[8], 16777215);
										buff[12] = 45;
										MemClear(&buff[13], 23);
										ptrCurr = me->userName->ConcatTo(&buff[36]) + 1;
										ptrCurr[0] = (UInt8)Net::MySQLUtil::BuildAuthen(ptrCurr + 1, me->authenType, me->authPluginData, 20, me->password->ToCString());
										ptrCurr += ptrCurr[0] + 1;
										NotNullPtr<Text::String> s;
										if (me->database.SetTo(s))
										{
											ptrCurr = s->ConcatTo(ptrCurr) + 1;
										}
										if (cliCap & Net::MySQLUtil::CLIENT_PLUGIN_AUTH)
										{
											ptrCurr = Net::MySQLUtil::AuthenTypeGetName(me->authenType).ConcatTo(ptrCurr) + 1;
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
										me->cmdSeqNum = 1;
										me->cli->Write(buff, (UOSInt)(ptrCurr - buff));
	#if defined(VERBOSE)
										printf("MySQLTCP %d handshake response sent\r\n", me->cli->GetLocalPort());
	#endif
									}

									readSize = 0;
								}
								else
								{
	#if defined(VERBOSE)
									printf("MySQLTCP %d protocol version 10 invalid\r\n", me->cli->GetLocalPort());
	#endif
									me->mode = ClientMode::Authen;
									////////////////////////////////
									readSize = 0;
									me->cli->Close();
								}
							}
							else
							{
	#if defined(VERBOSE)
								printf("MySQLTCP %d protocol version unsupported (%d)\r\n", me->cli->GetLocalPort(), buff[4]);
	#endif
								readSize = 0;
								me->cli->Close();
							}
						}
					}
				}
			}
			else if (me->mode == ClientMode::Authen)
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
							me->SetLastError({&buff[7], readSize - 3});
							me->cli->Close();
						}
						else if (buff[3] != me->cmdSeqNum + 1)
						{
							me->SetLastError(CSTR("Invalid login reply"));
							me->cli->Close();
						}
						else if (buff[4] == 0)
						{
							me->mode = ClientMode::Data;
	#if defined(VERBOSE)
							printf("MySQLTCP %d login success\r\n", me->cli->GetLocalPort());
	#endif
						}
						else if (buff[4] == 0xFE)
						{
							UOSInt nameLen = Text::StrCharCnt(&buff[5]);
	#if defined(VERBOSE)
							printf("MySQLTCP %d AuthSwitchRequest: plugin name = %s\r\n", me->cli->GetLocalPort(), &buff[5]);
							Text::StringBuilderUTF8 sb;
							sb.AppendHexBuff(&buff[6 + nameLen], readSize - 3 - nameLen, ' ', Text::LineBreakType::None);
							printf("MySQLTCP %d AuthSwitchRequest: plugin data = %s\r\n", me->cli->GetLocalPort(), sb.ToString());
	#endif
							me->cmdSeqNum += 2;
							me->authenType = Net::MySQLUtil::AuthenTypeParse(Text::CString(&buff[5], nameLen));
							UInt8 packetBuff[64];
							UOSInt authSize = Net::MySQLUtil::BuildAuthen(&packetBuff[4], me->authenType, &buff[6 + nameLen], readSize - 3 - nameLen, me->password->ToCString());
							WriteUInt32(packetBuff, (UInt32)authSize);
							packetBuff[3] = (UInt8)me->cmdSeqNum;
							me->cli->Write(packetBuff, authSize + 4);
						}
						else
						{
							me->SetLastError(CSTR("Invalid reply on login"));
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
			else if (me->mode == ClientMode::Data)
			{
				UInt32 packetSize;
				UInt64 val;
				readSize = 0;
				while (readSize + 4 <= buffSize)
				{
					packetSize = ReadUInt24(&buff[readSize]);
					if (4 + packetSize > buffCapacity)
					{
						while (4 + packetSize > buffCapacity)
						{
							buffCapacity <<= 1;
						}
						UInt8 *newBuff = MemAlloc(UInt8, buffCapacity);
						MemCopyNO(newBuff, buff, buffSize);
						MemFree(buff);
						buff = newBuff;
					}
					if (readSize + 4 + packetSize > buffSize)
					{
						break;
					}
					if (buff[readSize + 4] == 0xFF || (buff[readSize + 3] == (me->cmdSeqNum & 0xff) && (me->cmdTCPReader || me->cmdBinReader)))
					{
						if (me->cmdSeqNum == 1)
						{
							me->cmdSeqNum++;
							if (me->cmdResultType == CmdResultType::ProcessingBinary)
							{
								if (buff[readSize + 4] == 0) //OK
								{
									UInt32 stmtId = ReadUInt32(&buff[readSize + 5]);
									me->cmdBinReader->SetStmtId(stmtId);
									UInt16 numColumns = ReadUInt16(&buff[readSize + 9]);
#if defined(VERBOSE)
									UInt16 numParams = ReadUInt16(&buff[readSize + 11]);
									printf("MySQLTCP %d COM_STMT_PREPARE OK, stmt id = %d, num_columns = %d, num_params = %d\r\n", me->cli->GetLocalPort(), stmtId, numColumns, numParams);
#endif
									if (numColumns == 0)
									{
										me->SendExecuteStmt(stmtId);
#if defined(VERBOSE)
										printf("MySQLTCP %d No columns found, execute statment id %d\r\n", me->cli->GetLocalPort(), stmtId);
#endif
										me->cmdResultType = CmdResultType::BinaryExecuting;
										me->cmdEvt.Set();

									}
								}
								else if (buff[readSize + 4] == 0xFF) //ERR
								{
									me->SetLastError({&buff[readSize + 7], packetSize - 3});
									if (me->cmdBinReader)
									{
										me->cmdResultType = CmdResultType::Error;
										me->cmdEvt.Set();
										me->cmdBinReader->EndData();
									}
									else
									{
										me->cmdResultType = CmdResultType::Error;
										me->cmdEvt.Set();
									}
	#if defined(VERBOSE)
									printf("MySQLTCP %d COM_STMT_PREPARE Error\r\n", me->cli->GetLocalPort());
	#endif
								}
								else
								{
									me->cmdResultType = CmdResultType::Error;
									me->cmdEvt.Set();
									if (me->cmdBinReader)
									{
										me->cmdBinReader->EndData();
									}
	#if defined(VERBOSE)
									printf("MySQLTCP %d COM_STMT_PREPARE Error\r\n", me->cli->GetLocalPort());
	#endif
								}
							}
							else if (me->cmdResultType == CmdResultType::BinaryExecuting)
							{
								if (buff[readSize + 4] == 0xFF) //ERR
								{
									me->SetLastError({&buff[readSize + 7], packetSize - 3});
									if (me->cmdBinReader)
									{
										me->cmdResultType = CmdResultType::Error;
										me->cmdEvt.Set();
										me->cmdBinReader->EndData();
									}
									else
									{
										me->cmdResultType = CmdResultType::Error;
										me->cmdEvt.Set();
									}
	#if defined(VERBOSE)
									printf("MySQLTCP %d COM_STMT_EXECUTE Error\r\n", me->cli->GetLocalPort());
	#endif
								}
								else
								{
									Net::MySQLUtil::ReadLenencInt(&buff[readSize + 4], &val);
	#if defined(VERBOSE)
									printf("MySQLTCP %d COM_STMT_EXECUTE executed, column cnt = %lld\r\n", me->cli->GetLocalPort(), val);
	#endif
									if (val == 0)
									{
										me->cmdBinReader->SetRowChanged(-1);
										me->cmdBinReader->EndCols();
										me->SendStmtClose(me->cmdBinReader->GetStmtId());
										me->cmdResultType = CmdResultType::ResultEnd;
										me->cmdBinReader->EndData();
										me->cmdEvt.Set();
									}
								}
							}
							else
							{
								if (buff[readSize + 4] == 0) //OK
								{
									Net::MySQLUtil::ReadLenencInt(&buff[readSize + 5], &val);
									me->cmdTCPReader->SetRowChanged((Int64)val);
									me->cmdResultType = CmdResultType::ResultEnd;
									me->cmdEvt.Set();
	#if defined(VERBOSE)
									printf("MySQLTCP %d Command OK, row changed = %lld\r\n", me->cli->GetLocalPort(), val);
	#endif
								}
								else if (buff[readSize + 4] == 0xFF) //ERR
								{
									me->SetLastError({&buff[readSize + 7], packetSize - 3});
									if (me->cmdTCPReader)
									{
										if (me->cmdResultType == CmdResultType::Processing)
										{
											me->cmdResultType = CmdResultType::Error;
											me->cmdEvt.Set();
										}
										me->cmdTCPReader->EndData();
									}
									else
									{
										me->cmdResultType = CmdResultType::Error;
										me->cmdEvt.Set();
									}
		//							me->cli->Close();
								}
								else
								{
									Net::MySQLUtil::ReadLenencInt(&buff[readSize + 4], &val);
	#if defined(VERBOSE)
									printf("MySQLTCP %d Command executed, column cnt = %lld\r\n", me->cli->GetLocalPort(), val);
	#endif
								}
							}
						}
						else if (buff[readSize + 4] == 0xFE) //EOF
						{
							me->cmdSeqNum++;
	#if defined(VERBOSE)
							printf("MySQLTCP %d EOF found, curr result type = %d\r\n", me->cli->GetLocalPort(), (int)me->cmdResultType);
	#endif
							switch (me->cmdResultType)
							{
							case CmdResultType::Processing:
								me->cmdResultType = CmdResultType::ResultReady;
								me->cmdEvt.Set();
								break;
							case CmdResultType::ProcessingBinary:
							{
								UInt32 stmtId = me->cmdBinReader->GetStmtId();
								me->SendExecuteStmt(stmtId);
	#if defined(VERBOSE)
								printf("MySQLTCP %d EOF found, execute statment id %d\r\n", me->cli->GetLocalPort(), stmtId);
	#endif
								me->cmdResultType = CmdResultType::BinaryExecuting;
								me->cmdEvt.Set();
								break;
							}
							case CmdResultType::BinaryExecuting:
								me->cmdResultType = CmdResultType::BinaryResultReady;
								me->cmdBinReader->EndCols();
								me->cmdEvt.Set();
								break;
							case CmdResultType::BinaryResultReady:
								if (me->cmdBinReader)
								{
									me->SendStmtClose(me->cmdBinReader->GetStmtId());
									me->cmdBinReader->EndData();
									me->cmdResultType = CmdResultType::ResultEnd;
									me->cmdEvt.Set();
								}
								else
								{
									me->cmdResultType = CmdResultType::ResultEnd;
								}
								break;
							case CmdResultType::ResultEnd:
							case CmdResultType::ResultReady:
							case CmdResultType::Error:
							default:
								me->cmdResultType = CmdResultType::ResultEnd;
								if (me->cmdTCPReader)
									me->cmdTCPReader->EndData();
								else if (me->cmdBinReader)
									me->cmdBinReader->EndData();
								me->cmdEvt.Set();
								break;
							}
						}
						else
						{
							me->cmdSeqNum++;
							switch (me->cmdResultType)
							{
							case CmdResultType::Processing: //ColumnDef
	#if defined(VERBOSE)
								printf("MySQLTCP %d Seq %d Column found\r\n", me->cli->GetLocalPort(), buff[readSize + 3]);
	#endif
								if (me->cmdTCPReader)
									me->cmdTCPReader->AddColumnDef41(&buff[readSize + 4], packetSize);
								break;
							case CmdResultType::ProcessingBinary: //ColumnDefinition
	#if defined(VERBOSE)
								printf("MySQLTCP %d Seq %d Statement Column found\r\n", me->cli->GetLocalPort(), buff[readSize + 3]);
	#endif
								break;
							case CmdResultType::BinaryExecuting:
	#if defined(VERBOSE)
								printf("MySQLTCP %d Seq %d Binary Column found\r\n", me->cli->GetLocalPort(), buff[readSize + 3]);
	#endif
								if (me->cmdBinReader)
									me->cmdBinReader->AddColumnDef41(&buff[readSize + 4], packetSize);
								break;
							case CmdResultType::BinaryResultReady:
	#if defined(VERBOSE)
								printf("MySQLTCP %d Seq %d Binary Row found\r\n", me->cli->GetLocalPort(), buff[readSize + 3]);
	#endif
								if (me->cmdBinReader)
									me->cmdBinReader->AddRowData(&buff[readSize + 4], packetSize);
								break;
							case CmdResultType::ResultEnd:
							case CmdResultType::Error:
							case CmdResultType::ResultReady:
							default:
	#if defined(VERBOSE)
								printf("MySQLTCP %d Seq %d Text Row found\r\n", me->cli->GetLocalPort(), buff[readSize + 3]);
	#endif
								if (me->cmdTCPReader)
									me->cmdTCPReader->AddRowData(&buff[readSize + 4], packetSize);
								break;
							}
						}
					}
					else
					{
	#if defined(VERBOSE)
						printf("MySQLTCP %d Seq num mismatch: %d != %d\r\n", me->cli->GetLocalPort(), buff[readSize + 3], (int)me->cmdSeqNum);
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

		if (me->cmdTCPReader)
		{
			if (me->cmdResultType == CmdResultType::Processing)
			{
	#if defined(VERBOSE)	
				printf("MySQLTCP %d End Conn: signal waiting\r\n", me->cli->GetLocalPort());
	#endif
				me->cmdResultType = CmdResultType::Error;
				me->cmdEvt.Set();
			}
			else
			{
	#if defined(VERBOSE)	
				printf("MySQLTCP %d End Conn: end data %d\r\n", me->cli->GetLocalPort(), (Int32)me->cmdResultType);
	#endif
				me->cmdTCPReader->EndData();
			}
		}
		if (me->cmdBinReader)
		{
			if (me->cmdResultType == CmdResultType::Processing)
			{
	#if defined(VERBOSE)	
				printf("MySQLTCP %d End Conn: signal waiting\r\n", me->cli->GetLocalPort());
	#endif
				me->cmdResultType = CmdResultType::Error;
				me->cmdEvt.Set();
			}
			else
			{
	#if defined(VERBOSE)	
				printf("MySQLTCP %d End Conn: end data %d\r\n", me->cli->GetLocalPort(), (Int32)me->cmdResultType);
	#endif
				me->cmdBinReader->EndData();
			}
		}
	#if defined(VERBOSE)	
		printf("MySQLTCP %d End connection\r\n", me->cli->GetLocalPort());
	#endif
		MemFree(buff);
	}
	me->recvRunning = false;
	return 0;
}

void Net::MySQLTCPClient::SetLastError(Text::CString errMsg)
{
	SDEL_STRING(this->lastError);
	this->lastError = Text::String::New(errMsg).Ptr();
#if defined(VERBOSE)
	Text::StringBuilderUTF8 sb;
	this->GetLastErrorMsg(sb);
	UInt32 port;
	if (this->cli)
	{
		port = this->cli->GetLocalPort();
	}
	else
	{
		port = 0;
	}
	printf("MySQLTCP %d Error: %s\r\n", port, sb.ToString());
#endif
}

void Net::MySQLTCPClient::SendExecuteStmt(UInt32 stmtId)
{
	UInt8 sbuff[14];
	this->cmdSeqNum = 1;
	WriteUInt32(&sbuff[0], 10);
	sbuff[3] = 0;
	sbuff[4] = 0x17;
	WriteUInt32(&sbuff[5], stmtId);
	sbuff[9] = 0;
	WriteUInt32(&sbuff[10], 1);
	this->cli->Write(sbuff, 14);
}

void Net::MySQLTCPClient::SendStmtClose(UInt32 stmtId)
{
	UInt8 sbuff[9];
	WriteUInt32(&sbuff[0], 5);
	sbuff[3] = 0;
	sbuff[4] = 0x19; //COM_STMT_CLOSE
	WriteUInt32(&sbuff[5], stmtId);
	this->cli->Write(sbuff, 9);
}

Net::MySQLTCPClient::MySQLTCPClient(NotNullPtr<Net::SocketFactory> sockf, NotNullPtr<const Net::SocketUtil::AddressInfo> addr, UInt16 port, NotNullPtr<Text::String> userName, NotNullPtr<Text::String> password, Optional<Text::String> database) : DB::DBConn(CSTR("MySQLTCPClient"))
{
	this->sockf = sockf;
	this->recvRunning = false;
	this->recvStarted = false;
	this->addr = *addr.Ptr();
	this->port = port;
	this->mode = ClientMode::Handshake;
	this->authenType = Net::MySQLUtil::AuthenType::MySQLNativePassword;
	this->svrVer = 0;
	this->connId = 0;
	this->authPluginDataSize = 0;
	this->svrCap = 0;
	this->lastError = 0;
	this->userName = userName->Clone();
	this->password = password->Clone();
	this->database = Text::String::CopyOrNull(database);
	this->cmdSeqNum = 0;
	this->cmdTCPReader = 0;
	this->cmdBinReader = 0;
	this->cmdResultType = CmdResultType::Processing;
	this->cli = 0;
	this->Reconnect();
}

Net::MySQLTCPClient::MySQLTCPClient(NotNullPtr<Net::SocketFactory> sockf, NotNullPtr<const Net::SocketUtil::AddressInfo> addr, UInt16 port, Text::CString userName, Text::CString password, Text::CString database) : DB::DBConn(CSTR("MySQLTCPClient"))
{
	this->sockf = sockf;
	this->recvRunning = false;
	this->recvStarted = false;
	this->addr = *addr.Ptr();
	this->port = port;
	this->mode = ClientMode::Handshake;
	this->svrVer = 0;
	this->axisAware = false;
	this->connId = 0;
	this->authPluginDataSize = 0;
	this->svrCap = 0;
	this->lastError = 0;
	this->userName = Text::String::New(userName);
	this->password = Text::String::New(password);
	this->database = Text::String::NewOrNull(database);
	this->cmdSeqNum = 0;
	this->cmdTCPReader = 0;
	this->cmdBinReader = 0;
	this->cmdResultType = CmdResultType::Processing;
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
			Sync::SimpleThread::Sleep(1);
		}
		DEL_CLASS(this->cli);
		this->cli = 0;
	}
	this->userName->Release();
	this->password->Release();
	OPTSTR_DEL(this->database);
	SDEL_STRING(this->svrVer);
	SDEL_STRING(this->lastError);
}

DB::SQLType Net::MySQLTCPClient::GetSQLType() const
{
	return DB::SQLType::MySQL;
}

Bool Net::MySQLTCPClient::IsAxisAware() const
{
	return this->axisAware;
}

DB::DBConn::ConnType Net::MySQLTCPClient::GetConnType() const
{
	return DB::DBConn::CT_MYSQLTCP;
}

Int8 Net::MySQLTCPClient::GetTzQhr() const
{
	return 0;
}

void Net::MySQLTCPClient::ForceTz(Int8 tzQhr)
{

}

void Net::MySQLTCPClient::GetConnName(NotNullPtr<Text::StringBuilderUTF8> sb)
{
	UTF8Char sbuff[64];
	UTF8Char *sptr;
	sb->AppendC(UTF8STRC("MySQLTCP:"));
	sptr = Net::SocketUtil::GetAddrName(sbuff, this->addr, this->port);
	sb->AppendC(sbuff, (UOSInt)(sptr - sbuff));
	NotNullPtr<Text::String> s;
	if (this->database.SetTo(s))
	{
		sb->AppendUTF8Char('/');
		sb->Append(s);
	}
}

void Net::MySQLTCPClient::Close()
{
#if defined(VERBOSE)
	printf("MySQLTCP %d User close\r\n", this->cli->GetLocalPort());
#endif
	this->cli->Close();
}

void Net::MySQLTCPClient::Dispose()
{
	this->Close();
}

OSInt Net::MySQLTCPClient::ExecuteNonQuery(Text::CStringNN sql)
{
	NotNullPtr<DB::DBReader> reader;
	if (!reader.Set(ExecuteReaderText(sql)))
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

DB::DBReader *Net::MySQLTCPClient::ExecuteReader(Text::CStringNN sql)
{
	if (sql.StartsWith(UTF8STRC("check table ")))
	{
		return ExecuteReaderText(sql);
	}
	else if (sql.StartsWith(UTF8STRC("use ")))
	{
		return ExecuteReaderText(sql);
	}
	else
	{
		return ExecuteReaderBinary(sql);
	}
}

DB::DBReader *Net::MySQLTCPClient::ExecuteReaderText(Text::CStringNN sql)
{
	if (this->cli == 0 || !this->recvRunning)
	{
		this->lastDataError = DE_CONN_ERROR;
		return 0;
	}
	while (this->mode != ClientMode::Data)
	{
		if (this->cli == 0 || !this->recvRunning)
		{
			return 0;
		}
		Sync::SimpleThread::Sleep(10);
	}
	MySQLTCPReader *reader;
	NEW_CLASS(reader, MySQLTCPReader(this->cmdMut));
	this->cmdResultType = CmdResultType::Processing;
	this->cmdSeqNum = 1;
	this->cmdTCPReader = reader;
	UInt8 *buff = MemAlloc(UInt8, sql.leng + 5);
	WriteInt32(buff, (Int32)(sql.leng + 1));
	buff[4] = 3;
	MemCopyNO(&buff[5], sql.v, sql.leng);
	if (this->cli->Write(buff, 5 + sql.leng) != 5 + sql.leng)
	{
		this->cmdTCPReader = 0;
		DEL_CLASS(reader);
		MemFree(buff);
		this->lastDataError = DE_CONN_ERROR;
		return 0;
	}
	MemFree(buff);
#if defined(VERBOSE)
	printf("MySQLTCP %d Sent SQL: %s\r\n", this->cli->GetLocalPort(), sql.v);
#endif
//	dt.SetCurrTimeUTC();
//	startTime = dt.ToTicks();
	while (this->cmdResultType == CmdResultType::Processing)
	{
		this->cmdEvt.Wait(10000);
	}
	if (this->cmdResultType == CmdResultType::Error)
	{
		this->cmdTCPReader = 0;
		reader->EndData();
		DEL_CLASS(reader);
		this->lastDataError = DE_EXEC_SQL_ERROR;
		return 0;
	}
	this->lastDataError = DE_NO_ERROR;
	return reader;
}

DB::DBReader *Net::MySQLTCPClient::ExecuteReaderBinary(Text::CStringNN sql)
{
	if (this->cli == 0 || !this->recvRunning)
	{
		this->lastDataError = DE_CONN_ERROR;
		return 0;
	}
	while (this->mode != ClientMode::Data)
	{
		if (this->cli == 0 || !this->recvRunning)
		{
			return 0;
		}
		Sync::SimpleThread::Sleep(10);
	}
	MySQLTCPBinaryReader *reader;
	NEW_CLASS(reader, MySQLTCPBinaryReader(this->cmdMut));
	this->cmdResultType = CmdResultType::ProcessingBinary;
	this->cmdSeqNum = 1;
	this->cmdBinReader = reader;
	UInt8 *buff = MemAlloc(UInt8, sql.leng + 5);
	WriteInt32(buff, (Int32)(sql.leng + 1));
	buff[4] = 22;
	MemCopyNO(&buff[5], sql.v, sql.leng);
	if (this->cli->Write(buff, 5 + sql.leng) != 5 + sql.leng)
	{
		this->cmdBinReader = 0;
		DEL_CLASS(reader);
		MemFree(buff);
		this->lastDataError = DE_CONN_ERROR;
		return 0;
	}
	MemFree(buff);
#if defined(VERBOSE)
	printf("MySQLTCP %d Sent Prepare Stmt: %s\r\n", this->cli->GetLocalPort(), sql.v);
#endif
//	dt.SetCurrTimeUTC();
//	startTime = dt.ToTicks();
	while (this->cmdResultType == CmdResultType::ProcessingBinary || this->cmdResultType == CmdResultType::BinaryExecuting)
	{
		this->cmdEvt.Wait(10000);
	}
	if (this->cmdResultType == CmdResultType::Error)
	{
		this->cmdBinReader = 0;
		reader->EndData();
		DEL_CLASS(reader);
		this->lastDataError = DE_EXEC_SQL_ERROR;
		return 0;
	}
	this->lastDataError = DE_NO_ERROR;
	return reader;
}

void Net::MySQLTCPClient::CloseReader(NotNullPtr<DB::DBReader> r)
{
	while (r->ReadNext())
	{
	}
	while (this->cmdResultType != CmdResultType::ResultEnd && this->cmdResultType != CmdResultType::Error)
	{
		this->cmdEvt.Wait(10000);
	}
	if (this->cmdBinReader == (MySQLTCPBinaryReader*)r.Ptr())
	{
		MySQLTCPBinaryReader *reader = (MySQLTCPBinaryReader*)r.Ptr();
		this->cmdBinReader = 0;
		DEL_CLASS(reader);
	}
	else if (this->cmdTCPReader == (MySQLTCPReader*)r.Ptr())
	{
		MySQLTCPReader *reader = (MySQLTCPReader*)r.Ptr();
		this->cmdTCPReader = 0;
		DEL_CLASS(reader);
	}
}

void Net::MySQLTCPClient::GetLastErrorMsg(NotNullPtr<Text::StringBuilderUTF8> str)
{
	if (this->lastError)
	{
		if (this->lastError->v[0] == '#')
		{
			str->AppendUTF8Char('[');
			str->AppendC(&this->lastError->v[1], 5);
			str->AppendUTF8Char(']');
			str->AppendUTF8Char(' ');
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
	this->mode = ClientMode::Handshake;
	SDEL_STRING(this->svrVer);
	this->axisAware = false;
	NEW_CLASS(this->cli, Net::TCPClient(this->sockf, this->addr, this->port, 15000));
	if (this->cli->IsConnectError())
	{
		DEL_CLASS(this->cli);
		this->cli = 0;
		this->SetLastError(CSTR("Cannot connect to server"));
	}
	else
	{
		this->cli->SetNoDelay(true);
		Sync::ThreadUtil::Create(RecvThread, this);
		while (!this->recvStarted)
		{
			Sync::SimpleThread::Sleep(1);
		}
	}
}

void *Net::MySQLTCPClient::BeginTransaction()
{
	if (this->ExecuteNonQuery(CSTR("START TRANSACTION")) != -2)
	{
		return (void*)-1;
	}
	return 0;
}
void Net::MySQLTCPClient::Commit(void *tran)
{
	this->ExecuteNonQuery(CSTR("COMMIT"));
}

void Net::MySQLTCPClient::Rollback(void *tran)
{
	this->ExecuteNonQuery(CSTR("ROLLBACK"));
}

UOSInt Net::MySQLTCPClient::QueryTableNames(Text::CString schemaName, NotNullPtr<Data::ArrayListStringNN> names)
{
	if (schemaName.leng != 0)
		return 0;
	UTF8Char sbuff[256];
	UTF8Char *sptr;
	UOSInt len;
	UOSInt initCnt = names->GetCount();
	NotNullPtr<DB::DBReader> rdr;
	if (rdr.Set(this->ExecuteReader(CSTR("show tables"))))
	{
		while (rdr->ReadNext())
		{
			sptr = rdr->GetStr(0, sbuff, sizeof(sbuff));
			len = (UOSInt)(sptr - sbuff);
			names->Add(Text::String::New(sbuff, len));
		}
		this->CloseReader(rdr);
	}
	return names->GetCount() - initCnt;
}

DB::DBReader *Net::MySQLTCPClient::QueryTableData(Text::CString schemaName, Text::CString tableName, Data::ArrayListStringNN *columnNames, UOSInt ofst, UOSInt maxCnt, Text::CString ordering, Data::QueryConditions *condition)
{
	UTF8Char sbuff[512];
	UTF8Char *sptr;
	Text::StringBuilderUTF8 sb;
	sb.AppendC(UTF8STRC("select "));
	if (columnNames == 0 || columnNames->GetCount() == 0)
	{
		sb.AppendC(UTF8STRC("*"));
	}
	else
	{
		Data::ArrayIterator<NotNullPtr<Text::String>> it = columnNames->Iterator();
		Bool found = false;
		while (it.HasNext())
		{
			if (found)
				sb.AppendUTF8Char(',');
			sptr = DB::DBUtil::SDBColUTF8(sbuff, it.Next()->v, DB::SQLType::MySQL);
			sb.AppendC(sbuff, (UOSInt)(sptr - sbuff));
			found = true;
		}
	}
	sb.AppendC(UTF8STRC(" from "));
	if (schemaName.leng > 0)
	{
		sptr = DB::DBUtil::SDBColUTF8(sbuff, schemaName.v, DB::SQLType::MySQL);
		sb.AppendP(sbuff, sptr);
		sb.AppendUTF8Char('.');
	}
	sptr = DB::DBUtil::SDBColUTF8(sbuff, tableName.v, DB::SQLType::MySQL);
	sb.AppendP(sbuff, sptr);
	if (condition)
	{
		Data::ArrayListNN<Data::QueryConditions::Condition> cliCond;
		sb.AppendC(UTF8STRC(" where "));
		condition->ToWhereClause(sb, DB::SQLType::MySQL, 0, 100, cliCond);
	}
	if (ordering.leng > 0)
	{
		sb.AppendC(UTF8STRC(" order by "));
		sb.Append(ordering);
	}
	if (maxCnt > 0)
	{
		sb.AppendC(UTF8STRC(" LIMIT "));
		sb.AppendUOSInt(maxCnt);
	}
	return this->ExecuteReader(sb.ToCString());
}

Bool Net::MySQLTCPClient::ChangeSchema(const UTF8Char *schemaName)
{
	UTF8Char sbuff[128];
	UTF8Char *sptr2;
	Text::StringBuilderUTF8 sb;
	sb.AppendC(UTF8STRC("use "));
	UOSInt colLen = DB::DBUtil::SDBColUTF8Leng(schemaName, DB::SQLType::MySQL);
	if (colLen > 127)
	{
		UTF8Char *sptr = MemAlloc(UTF8Char, colLen + 1);
		sptr2 = DB::DBUtil::SDBColUTF8(sptr, schemaName, DB::SQLType::MySQL);
		sb.AppendC(sptr, (UOSInt)(sptr2 - sptr));
		MemFree(sptr);
	}
	else
	{
		sptr2 = DB::DBUtil::SDBColUTF8(sbuff, schemaName, DB::SQLType::MySQL);
		sb.AppendC(sbuff, (UOSInt)(sptr2 - sbuff));
	}
	if (this->ExecuteNonQuery(sb.ToCString()) >= 0)
	{
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
	return this->mode >= ClientMode::Authen;
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

UInt32 Net::MySQLTCPClient::GetServerCap() const
{
	return this->svrCap;
}

UInt16 Net::MySQLTCPClient::GetServerCS() const
{
	return this->svrCS;
}

NotNullPtr<const Net::SocketUtil::AddressInfo> Net::MySQLTCPClient::GetConnAddr() const
{
	return this->addr;
}

UInt16 Net::MySQLTCPClient::GetConnPort() const
{
	return this->port;
}

Optional<Text::String> Net::MySQLTCPClient::GetConnDB() const
{
	return this->database;
}

NotNullPtr<Text::String> Net::MySQLTCPClient::GetConnUID() const
{
	return this->userName;
}

NotNullPtr<Text::String> Net::MySQLTCPClient::GetConnPWD() const
{
	return this->password;
}

UInt16 Net::MySQLTCPClient::GetDefaultPort()
{
	return 3306;
}

DB::DBTool *Net::MySQLTCPClient::CreateDBTool(NotNullPtr<Net::SocketFactory> sockf, NotNullPtr<Text::String> serverName, Optional<Text::String> dbName, NotNullPtr<Text::String> uid, NotNullPtr<Text::String> pwd, NotNullPtr<IO::LogTool> log, Text::CString logPrefix)
{
	NotNullPtr<Net::MySQLTCPClient> conn;
	DB::DBTool *db;
	Net::SocketUtil::AddressInfo addr;
	if (sockf->DNSResolveIP(serverName->ToCString(), addr))
	{
		NEW_CLASSNN(conn, Net::MySQLTCPClient(sockf, addr, 3306, uid, pwd, dbName));
		if (conn->IsError() == 0)
		{
			NEW_CLASS(db, DB::DBTool(conn, true, log, logPrefix));
			return db;
		}
		else
		{
			conn.Delete();
			return 0;
		}
	}
	else
	{
		return 0;
	}
}

DB::DBTool *Net::MySQLTCPClient::CreateDBTool(NotNullPtr<Net::SocketFactory> sockf, Text::CStringNN serverName, Text::CString dbName, Text::CString uid, Text::CString pwd, NotNullPtr<IO::LogTool> log, Text::CString logPrefix)
{
	NotNullPtr<Net::MySQLTCPClient> conn;
	DB::DBTool *db;
	Net::SocketUtil::AddressInfo addr;
	if (sockf->DNSResolveIP(serverName, addr))
	{
		NEW_CLASSNN(conn, Net::MySQLTCPClient(sockf, addr, 3306, uid, pwd, dbName));
		if (conn->IsError() == 0)
		{
			NEW_CLASS(db, DB::DBTool(conn, true, log, logPrefix));
			return db;
		}
		else
		{
			conn.Delete();
			return 0;
		}
	}
	else
	{
		return 0;
	}
}
