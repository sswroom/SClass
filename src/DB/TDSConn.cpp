#include "Stdafx.h"
#include "DB/ColDef.h"
#include "DB/DBReader.h"
#include "DB/SQLBuilder.h"
#include "DB/TDSConn.h"
#include "Math/MSGeography.h"
#include "Math/WKTWriter.h"
#include "Math/Geometry/Vector2D.h"
#include <sybfront.h>
#include <sybdb.h>

#include <stdio.h>
//#define VERBOSE

#define SYBGEOMETRY 240
#define SYBUUID 36

Bool DB::TDSConn::inited = false;

class TDSConnReader : public DB::DBReader
{
private:
	struct ColInfo
	{
		Char *name;
		UInt8 *buff;
		Int32 type;
		Int32 size;
		UInt32 buffSize;
		Int32 status;
	};
private:
	DBPROCESS* dbproc;
	Int8 tzQhr;
	Int32 rowsAffected;
	UOSInt nCols;
	ColInfo *cols;
public:
	TDSConnReader(DBPROCESS* dbproc, Int8 tzQhr)
	{
		this->dbproc = dbproc;
		this->tzQhr = tzQhr;
		this->cols = 0;
		this->nCols = 0;
		this->rowsAffected = dbcount(dbproc);
		this->NextResult();
	}

	virtual ~TDSConnReader()
	{
		while (true)
		{
			while (this->ReadNext());
			if (!this->NextResult())
				break;
		}
	}

	Bool NextResult()
	{
		UOSInt i;
		if (this->cols)
		{
			i = this->nCols;
			while (i-- > 0)
			{
				if (this->cols[i].buff)
					MemFree(this->cols[i].buff);
			}
			MemFree(this->cols);
		}
		RETCODE ret = dbresults(this->dbproc);
		if (ret == FAIL)
		{
			this->nCols = 0;
			return false;
		}
		else if (ret != NO_MORE_RESULTS)
		{
			this->nCols = (UOSInt)dbnumcols(this->dbproc);
			this->cols = MemAlloc(ColInfo, this->nCols);
			Int32 c;
			i = 0;
			while (i < this->nCols)
			{
				c = (Int32)i + 1;
				this->cols[i].name = dbcolname(this->dbproc, c);
				this->cols[i].type = dbcoltype(this->dbproc, c);
				this->cols[i].size = dbcollen(this->dbproc, c);
				switch (this->cols[i].type)
				{
				case SYBCHAR:
				case SYBTEXT:
					this->cols[i].buffSize = (UInt32)this->cols[i].size;
					this->cols[i].buff = 0;
//					if (dbbind(this->dbproc, c, NTBSTRINGBIND, (Int32)this->cols[i].buffSize + 1, this->cols[i].buff) == FAIL)
//					{
//						printf("TDS: Error in binding char of column %d\r\n", (UInt32)i);
//					}
					break;
				case SYBBIT:
					this->cols[i].buffSize = 1;
					this->cols[i].buff = MemAlloc(UInt8, this->cols[i].buffSize);
					if (dbbind(this->dbproc, c, BITBIND, (Int32)this->cols[i].buffSize, this->cols[i].buff) == FAIL)
					{
						printf("TDS: Error in binding bit of column %d\r\n", (UInt32)i);
					}
					break;
				case SYBINT1:
					this->cols[i].buffSize = 1;
					this->cols[i].buff = MemAlloc(UInt8, this->cols[i].buffSize);
					if (dbbind(this->dbproc, c, TINYBIND, (Int32)this->cols[i].buffSize, this->cols[i].buff) == FAIL)
					{
						printf("TDS: Error in binding int1 of column %d\r\n", (UInt32)i);
					}
					break;
				case SYBINT2:
					this->cols[i].buffSize = 2;
					this->cols[i].buff = MemAlloc(UInt8, this->cols[i].buffSize);
					if (dbbind(this->dbproc, c, SMALLBIND, (Int32)this->cols[i].buffSize, this->cols[i].buff) == FAIL)
					{
						printf("TDS: Error in binding int2 of column %d\r\n", (UInt32)i);
					}
					break;
				case SYBINT4:
					this->cols[i].buffSize = 4;
					this->cols[i].buff = MemAlloc(UInt8, this->cols[i].buffSize);
					if (dbbind(this->dbproc, c, INTBIND, (Int32)this->cols[i].buffSize, this->cols[i].buff) == FAIL)
					{
						printf("TDS: Error in binding int4 of column %d\r\n", (UInt32)i);
					}
					break;
				case SYBINT8:
					this->cols[i].buffSize = 8;
					this->cols[i].buff = MemAlloc(UInt8, this->cols[i].buffSize);
					if (dbbind(this->dbproc, c, BIGINTBIND, (Int32)this->cols[i].buffSize, this->cols[i].buff) == FAIL)
					{
						printf("TDS: Error in binding int8 of column %d\r\n", (UInt32)i);
					}
					break;
				case SYBREAL:
					this->cols[i].buffSize = 4;
					this->cols[i].buff = MemAlloc(UInt8, this->cols[i].buffSize);
					if (dbbind(this->dbproc, c, REALBIND, (Int32)this->cols[i].buffSize, this->cols[i].buff) == FAIL)
					{
						printf("TDS: Error in binding real of column %d\r\n", (UInt32)i);
					}
					break;
				case SYBFLT8:
					this->cols[i].buffSize = 8;
					this->cols[i].buff = MemAlloc(UInt8, this->cols[i].buffSize);
					if (dbbind(this->dbproc, c, FLT8BIND, (Int32)this->cols[i].buffSize, this->cols[i].buff) == FAIL)
					{
						printf("TDS: Error in binding float8 of column %d\r\n", (UInt32)i);
					}
					break;
				case SYBDECIMAL:
				case SYBNUMERIC:
					this->cols[i].buffSize = 8;
					this->cols[i].buff = MemAlloc(UInt8, this->cols[i].buffSize);
					if (dbbind(this->dbproc, c, FLT8BIND, (Int32)this->cols[i].buffSize, this->cols[i].buff) == FAIL)
					{
						printf("TDS: Error in binding numeric of column %d\r\n", (UInt32)i);
					}
					break;
				case SYBDATETIME:
					this->cols[i].buffSize = 8;
					this->cols[i].buff = MemAlloc(UInt8, this->cols[i].buffSize);
					if (dbbind(this->dbproc, c, DATETIMEBIND, (Int32)this->cols[i].buffSize, this->cols[i].buff) == FAIL)
					{
						printf("TDS: Error in binding datetime of column %d\r\n", (UInt32)i);
					}
					break;
				case SYBMSDATE:
				case SYBMSTIME:
				case SYBMSDATETIME2:
					this->cols[i].buffSize = 16;
					this->cols[i].buff = MemAlloc(UInt8, this->cols[i].buffSize);
					if (dbbind(this->dbproc, c, DATETIME2BIND, (Int32)this->cols[i].buffSize, this->cols[i].buff) == FAIL)
					{
						printf("TDS: Error in binding msdatetime2 of column %d\r\n", (UInt32)i);
					}
					break;
				case SYBUUID:
					this->cols[i].buffSize = 0;
					this->cols[i].buff = 0;
					break;
				case SYBGEOMETRY:
					this->cols[i].buffSize = 0;
					this->cols[i].buff = 0;
					break;
				case SYBBINARY:
					this->cols[i].buffSize = 0;
					this->cols[i].buff = 0;
					break;
				default:
					this->cols[i].buffSize = (UInt32)dbprcollen(this->dbproc, c);;
					printf("TDS: Unsupported type %d, use print size %d (%d)\r\n", this->cols[i].type, this->cols[i].buffSize, this->cols[i].size);
					this->cols[i].buff = MemAlloc(UInt8, this->cols[i].buffSize + 1);
					if (dbbind(this->dbproc, c, NTBSTRINGBIND, (Int32)this->cols[i].buffSize + 1, this->cols[i].buff) == FAIL)
					{
						printf("TDS: Error in binding print string of column %d\r\n", (UInt32)i);
					}
					break;
				}
				if (dbnullbind(this->dbproc, c, &this->cols[i].status) == FAIL)
				{
					printf("TDS: Error in binding null of column %d\r\n", (UInt32)i);
				}
				i++;
			}
			return true;
		}
		else
		{
			this->nCols = 0;
			return false;
		}
	}

	virtual Bool ReadNext()
	{
		while (true)
		{
			RETCODE ret = dbnextrow(this->dbproc);
			switch (ret)
			{
			case REG_ROW:
				return true;
			case BUF_FULL:
				printf("TDS: nextrow return BUF_FULL\r\n");
				break;
			case NO_MORE_ROWS:
				return false;
			case FAIL:
				printf("TDS: nextrow return FAIL\r\n");
				return false;
			default:
				printf("TDS: nextrow return unknown row code: %d\r\n", ret);
				break;
			}
		}
	}

	virtual UOSInt ColCount()
	{
		return this->nCols;
	}

	virtual OSInt GetRowChanged()
	{
		return this->rowsAffected;
	}

	virtual Int32 GetInt32(UOSInt colIndex)
	{
		Data::VariItem item;
		if (this->GetVariItem(colIndex, &item))
			return item.GetAsI32();
		return 0;
	}

	virtual Int64 GetInt64(UOSInt colIndex)
	{
		Data::VariItem item;
		if (this->GetVariItem(colIndex, &item))
			return item.GetAsI64();
		return 0;
	}

	virtual WChar *GetStr(UOSInt colIndex, WChar *buff)
	{
		return 0;
	}

	virtual Bool GetStr(UOSInt colIndex, NotNullPtr<Text::StringBuilderUTF8> sb)
	{
		if (colIndex > this->nCols)
			return false;
		if (this->cols[colIndex].status == -1)
			return false;
		switch (this->cols[colIndex].type)
		{
		case SYBTEXT:
		case SYBCHAR:
		{
			UOSInt len = (UInt32)dbdatlen(this->dbproc, (int)colIndex + 1);
			UInt8 *data = dbdata(this->dbproc, (int)colIndex + 1);
			sb->AppendC(data, len);
			return true;
		}
		case SYBBIT:
			sb->AppendU16((this->cols[colIndex].buff[0] != 0)?1:0);
			return true;
		case SYBINT1:
			sb->AppendI16((Int8)this->cols[colIndex].buff[0]);
			return true;
		case SYBINT2:
			sb->AppendI16(ReadNInt16(&this->cols[colIndex].buff[0]));
			return true;
		case SYBINT4:
			sb->AppendI32(ReadNInt32(&this->cols[colIndex].buff[0]));
			return true;
		case SYBINT8:
			sb->AppendI64(ReadNInt64(&this->cols[colIndex].buff[0]));
			return true;
		case SYBREAL:
			sb->AppendDouble(ReadFloat(&this->cols[colIndex].buff[0]));
			return true;
		case SYBDECIMAL:
		case SYBNUMERIC:
		case SYBFLT8:
			sb->AppendDouble(ReadDouble(&this->cols[colIndex].buff[0]));
			return true;
		case SYBDATETIME:
		case SYBMSDATE:
		case SYBMSTIME:
		case SYBMSDATETIME2:
			sb->AppendTS(this->GetTimestamp(colIndex));
			return true;
		case SYBGEOMETRY:
			{
				NotNullPtr<Math::Geometry::Vector2D> vec;
				if (vec.Set(this->GetVector(colIndex)))
				{
					Math::WKTWriter wkt;
					wkt.ToText(sb, vec);
					vec.Delete();
				}
			}
			return true;
		case SYBUUID:
		{
			Data::UUID uuid;
			this->GetUUID(colIndex, &uuid);
			uuid.ToString(sb);
			return true;
		}
		case SYBBINARY:
			return false;
		default:
			printf("TDS: Unsupported type %d to str(sb)\r\n", this->cols[colIndex].type);
			return false;
		}
	}

	virtual Text::String *GetNewStr(UOSInt colIndex)
	{
		UTF8Char sbuff[64];
		UTF8Char *sptr;
		if (colIndex > this->nCols)
			return 0;
		if (this->cols[colIndex].status == -1)
			return 0;
		switch (this->cols[colIndex].type)
		{
		case SYBTEXT:
		case SYBCHAR:
		{
			UOSInt len = (UInt32)dbdatlen(this->dbproc, (int)colIndex + 1);
			UInt8 *data = dbdata(this->dbproc, (int)colIndex + 1);
			return Text::String::New(data, len).Ptr();
		}
		case SYBBIT:
			return Text::String::New((this->cols[colIndex].buff[0] != 0)?CSTR("1"):CSTR("0")).Ptr();
		case SYBINT1:
			sptr = Text::StrInt16(sbuff, (Int8)this->cols[colIndex].buff[0]);
			return Text::String::NewP(sbuff, sptr).Ptr();
		case SYBINT2:
			sptr = Text::StrInt16(sbuff, ReadNInt16(&this->cols[colIndex].buff[0]));
			return Text::String::NewP(sbuff, sptr).Ptr();
		case SYBINT4:
			sptr = Text::StrInt32(sbuff, ReadNInt32(&this->cols[colIndex].buff[0]));
			return Text::String::NewP(sbuff, sptr).Ptr();
		case SYBINT8:
			sptr = Text::StrInt64(sbuff, ReadNInt64(&this->cols[colIndex].buff[0]));
			return Text::String::NewP(sbuff, sptr).Ptr();
		case SYBREAL:
			sptr = Text::StrDouble(sbuff, ReadFloat(&this->cols[colIndex].buff[0]));
			return Text::String::NewP(sbuff, sptr).Ptr();
		case SYBDECIMAL:
		case SYBNUMERIC:
		case SYBFLT8:
			sptr = Text::StrDouble(sbuff, ReadDouble(&this->cols[colIndex].buff[0]));
			return Text::String::NewP(sbuff, sptr).Ptr();
		case SYBDATETIME:
		case SYBMSDATE:
		case SYBMSTIME:
		case SYBMSDATETIME2:
			sptr = this->GetTimestamp(colIndex).ToString(sbuff);
			return Text::String::NewP(sbuff, sptr).Ptr();
		case SYBGEOMETRY:
			{
				NotNullPtr<Math::Geometry::Vector2D> vec;
				if (vec.Set(this->GetVector(colIndex)))
				{
					Text::StringBuilderUTF8 sb;
					Math::WKTWriter wkt;
					wkt.ToText(sb, vec);
					vec.Delete();
					return Text::String::New(sb.ToString(), sb.GetLength()).Ptr();
				}
			}
			return 0;
		case SYBUUID:
		{
			Data::UUID uuid;
			this->GetUUID(colIndex, &uuid);
			sptr = uuid.ToString(sbuff);
			return Text::String::NewP(sbuff, sptr).Ptr();
		}
		case SYBBINARY:
			return 0;
		default:
			printf("TDS: Unsupported type %d to new str\r\n", this->cols[colIndex].type);
			return 0;
		}
	}

	virtual UTF8Char *GetStr(UOSInt colIndex, UTF8Char *buff, UOSInt buffSize)
	{
		if (colIndex > this->nCols)
			return 0;
		if (this->cols[colIndex].status == -1)
			return 0;
		switch (this->cols[colIndex].type)
		{
		case SYBTEXT:
		case SYBCHAR:
		{
			UOSInt len = (UInt32)dbdatlen(this->dbproc, (int)colIndex + 1);
			UInt8 *data = dbdata(this->dbproc, (int)colIndex + 1);
			return Text::StrConcatCS(buff, data, len, buffSize);
		}
		case SYBBIT:
			buff[0] = (this->cols[colIndex].buff[0] != 0)?'1':'0';
			buff[1] = 0;
			return buff + 1;
		case SYBINT1:
			return Text::StrInt16(buff, (Int8)this->cols[colIndex].buff[0]);
		case SYBINT2:
			return Text::StrInt16(buff, ReadNInt16(&this->cols[colIndex].buff[0]));
		case SYBINT4:
			return Text::StrInt32(buff, ReadNInt32(&this->cols[colIndex].buff[0]));
		case SYBINT8:
			return Text::StrInt64(buff, ReadNInt64(&this->cols[colIndex].buff[0]));
		case SYBREAL:
			return Text::StrDouble(buff, ReadFloat(&this->cols[colIndex].buff[0]));
		case SYBDECIMAL:
		case SYBNUMERIC:
		case SYBFLT8:
			return Text::StrDouble(buff, ReadDouble(&this->cols[colIndex].buff[0]));
		case SYBDATETIME:
		case SYBMSDATE:
		case SYBMSTIME:
		case SYBMSDATETIME2:
			return this->GetTimestamp(colIndex).ToString(buff);
		case SYBUUID:
		{
			Data::UUID uuid;
			this->GetUUID(colIndex, &uuid);
			return uuid.ToString(buff);
		}
		case SYBGEOMETRY:
			printf("TDS: Geometry to string not supported\r\n");
			return 0;
		case SYBBINARY:
			return 0;
		default:
			printf("TDS: Unsupported type %d to str(s)\r\n", this->cols[colIndex].type);
			return 0;
		}
	}

	virtual Data::Timestamp GetTimestamp(UOSInt colIndex)
	{
		if (colIndex >= this->nCols)
			return 0;
		if (this->cols[colIndex].status == -1)
			return 0;
		if (this->cols[colIndex].type == SYBDATETIME)
		{
			Int64 secs = (-25567 + ReadInt32(this->cols[colIndex].buff)) * 86400;
			UInt32 t = ReadUInt32(&this->cols[colIndex].buff[4]);
			secs += t / 300;
			t = (t % 300) * 10000000 / 3;
			return Data::Timestamp(Data::TimeInstant(secs - this->tzQhr * 900, t), this->tzQhr);
		}
		else if (this->cols[colIndex].type == SYBMSDATETIME2 || this->cols[colIndex].type == SYBMSDATE || this->cols[colIndex].type == SYBMSTIME || this->cols[colIndex].type == SYBMSDATETIMEOFFSET)
		{
			DBDATETIMEALL *t = (DBDATETIMEALL *)this->cols[colIndex].buff;
			Int64 secs = (-25567 + t->date) * 86400;
			UInt32 ns = (UInt32)(t->time % 10000000) * 100;
			secs += (Int64)(t->time / 10000000);
			Int8 tzQhr;
			if (t->has_offset)
				tzQhr = (Int8)(t->offset / 15);
			else
				tzQhr = this->tzQhr;
			return Data::Timestamp(Data::TimeInstant(secs - tzQhr * 900, ns), tzQhr);
		}
		else
		{

			return 0;
		}
	}

	virtual Double GetDbl(UOSInt colIndex)
	{
		Data::VariItem item;
		if (this->GetVariItem(colIndex, &item))
			return item.GetAsF64();
		return 0;
	}

	virtual Bool GetBool(UOSInt colIndex)
	{
		Data::VariItem item;
		if (this->GetVariItem(colIndex, &item))
			return item.GetAsBool();
		return 0;
	}

	virtual UOSInt GetBinarySize(UOSInt colIndex)
	{
		if (colIndex >= this->nCols)
			return 0;
		if (this->cols[colIndex].status == -1)
		{
			return 0;
		}
		if (this->cols[colIndex].type != SYBBINARY)
			return 0;
		return (UInt32)dbdatlen(this->dbproc, (int)colIndex + 1);
	}

	virtual UOSInt GetBinary(UOSInt colIndex, UInt8 *buff)
	{
		if (colIndex >= this->nCols)
			return 0;
		if (this->cols[colIndex].status == -1)
		{
			return 0;
		}
		if (this->cols[colIndex].type != SYBBINARY)
			return 0;
		UOSInt dataSize = (UInt32)dbdatlen(this->dbproc, (int)colIndex + 1);
		UInt8 *buffPtr = dbdata(this->dbproc, (int)colIndex + 1);
		MemCopyNO(buff, buffPtr, dataSize);
		return dataSize;
	}

	virtual Math::Geometry::Vector2D *GetVector(UOSInt colIndex)
	{
		if (colIndex >= this->nCols)
			return 0;
		if (this->cols[colIndex].status == -1)
		{
			return 0;
		}
		if (this->cols[colIndex].type != SYBGEOMETRY)
			return 0;
		UOSInt dataSize = (UInt32)dbdatlen(this->dbproc, (int)colIndex + 1);
		UInt8 *buffPtr = dbdata(this->dbproc, (int)colIndex + 1);
		UInt32 srId;
		return Math::MSGeography::ParseBinary(buffPtr, dataSize, &srId);
	}

	virtual Bool GetUUID(UOSInt colIndex, Data::UUID *uuid)
	{
		if (colIndex >= this->nCols)
			return false;
		if (this->cols[colIndex].status == -1)
		{
			return false;
		}
		if (this->cols[colIndex].type != SYBUUID)
			return false;
		uuid->SetValue(dbdata(this->dbproc, (int)colIndex + 1));
		return true;
	}

	virtual Bool GetVariItem(UOSInt colIndex, Data::VariItem *item)
	{
		if (colIndex >= this->nCols)
			return false;
		if (this->cols[colIndex].status == -1)
		{
			item->SetNull();
			return true;
		}
		switch (this->cols[colIndex].type)
		{
		case SYBBIT:
			item->SetBool(this->cols[colIndex].buff[0] != 0);
			return true;
		case SYBINT1:
			item->SetI8((Int8)this->cols[colIndex].buff[0]);
			return true;
		case SYBINT2:
			item->SetI16(ReadNInt16(&this->cols[colIndex].buff[0]));
			return true;
		case SYBINT4:
			item->SetI32(ReadNInt32(&this->cols[colIndex].buff[0]));
			return true;
		case SYBINT8:
			item->SetI64(ReadNInt64(&this->cols[colIndex].buff[0]));
			return true;
		case SYBREAL:
			item->SetF32(ReadFloat(&this->cols[colIndex].buff[0]));
			return true;
		case SYBDECIMAL:
		case SYBNUMERIC:
		case SYBFLT8:
			item->SetF64(ReadDouble(&this->cols[colIndex].buff[0]));
			return true;
		case SYBDATETIME:
		{
			Int64 secs = (-25567 + ReadInt32(this->cols[colIndex].buff)) * 86400;
			UInt32 t = ReadUInt32(&this->cols[colIndex].buff[4]);
			secs += t / 300;
			t = (t % 300) * 10000000 / 3;
			item->SetDate(Data::Timestamp(Data::TimeInstant(secs - this->tzQhr * 900, t), this->tzQhr));
			return true;
		}
		case SYBMSDATE:
		case SYBMSTIME:
		case SYBMSDATETIME2:
		{
			DBDATETIMEALL *t = (DBDATETIMEALL *)this->cols[colIndex].buff;
			Int64 secs = (-25567 + t->date) * 86400;
			UInt32 ns = (UInt32)(t->time % 10000000) * 100;
			secs += (Int64)(t->time / 10000000);
			Int8 tzQhr;
			if (t->has_offset)
				tzQhr = (Int8)(t->offset / 15);
			else
				tzQhr = this->tzQhr;
			item->SetDate(Data::Timestamp(Data::TimeInstant(secs - tzQhr * 900, ns), tzQhr));
			return true;
		}
		case SYBTEXT:
		case SYBCHAR:
		{
			UOSInt dataSize = (UInt32)dbdatlen(this->dbproc, (int)colIndex + 1);
			UInt8 *buffPtr = dbdata(this->dbproc, (int)colIndex + 1);
			item->SetStr(buffPtr, dataSize);
			return true;
		}
		case SYBGEOMETRY:
		{
			UOSInt dataSize = (UInt32)dbdatlen(this->dbproc, (int)colIndex + 1);
			UInt8 *buffPtr = dbdata(this->dbproc, (int)colIndex + 1);
			UInt32 srId;
			return Math::MSGeography::ParseBinary(buffPtr, dataSize, &srId);
		}
		case SYBBINARY:
		{
			UOSInt dataSize = (UInt32)dbdatlen(this->dbproc, (int)colIndex + 1);
			UInt8 *buffPtr = dbdata(this->dbproc, (int)colIndex + 1);
			item->SetByteArr(buffPtr, dataSize);
			return true;
		}
		case SYBUUID:
		{
			Data::UUID *uuid;
			NEW_CLASS(uuid, Data::UUID(dbdata(this->dbproc, (int)colIndex + 1)));
			item->SetUUIDDirect(uuid);
			return true;
		}
		default:
			item->SetNull();
			return true;
		}
	}

	virtual Bool IsNull(UOSInt colIndex)
	{
		if (colIndex >= this->nCols)
			return true;
		return this->cols[colIndex].status == -1;
	}

	virtual UTF8Char *GetName(UOSInt colIndex, UTF8Char *buff)
	{
		if (colIndex >= this->nCols)
			return 0;
		return Text::StrConcat(buff, (const UTF8Char*)this->cols[colIndex].name);
	}

	virtual DB::DBUtil::ColType GetColType(UOSInt colIndex, UOSInt *colSize)
	{
		if (colIndex >= this->nCols)
			return DB::DBUtil::ColType::CT_Unknown;
		if (colSize)
			*colSize = (UInt32)this->cols[colIndex].size;
		switch (this->cols[colIndex].type)
		{
		case SYBBIT:
			return DB::DBUtil::ColType::CT_Bool;
		case SYBINT1:
			return DB::DBUtil::ColType::CT_Byte;
		case SYBINT2:
			return DB::DBUtil::ColType::CT_Int16;
		case SYBINT4:
			return DB::DBUtil::ColType::CT_Int32;
		case SYBINT8:
			return DB::DBUtil::ColType::CT_Int64;
		case SYBDATETIME:
			return DB::DBUtil::ColType::CT_DateTime;
		case SYBMSDATE:
			return DB::DBUtil::ColType::CT_Date;
		case SYBMSTIME:
		case SYBMSDATETIME2:
			return DB::DBUtil::ColType::CT_DateTimeTZ;
		case SYBREAL:
			return DB::DBUtil::ColType::CT_Float;
		case SYBFLT8:
			return DB::DBUtil::ColType::CT_Double;
		case SYBDECIMAL:
		case SYBNUMERIC:
			return DB::DBUtil::ColType::CT_Decimal;
		case SYBTEXT:
		case SYBCHAR:
			return DB::DBUtil::ColType::CT_VarUTF8Char;
		case SYBGEOMETRY:
			return DB::DBUtil::ColType::CT_Vector;
		case SYBBINARY:
			return DB::DBUtil::ColType::CT_Binary;
		case SYBUUID:
			return DB::DBUtil::ColType::CT_UUID;
		default:
			return DB::DBUtil::ColType::CT_Unknown;
		}
	}

	virtual Bool GetColDef(UOSInt colIndex, NotNullPtr<DB::ColDef> colDef)
	{
		if (colIndex >= this->nCols)
			return DB::DBUtil::ColType::CT_Unknown;
		NotNullPtr<const UTF8Char> colName;
		if (!colName.Set((const UTF8Char*)this->cols[colIndex].name)) colName.Set((const UTF8Char*)"");
		colDef->SetColName(colName);
		colDef->SetColType(this->GetColType(colIndex, 0));
		colDef->SetColSize((UInt32)this->cols[colIndex].size);
		return true;
	}
};

struct DB::TDSConn::ClassData
{
	DBPROCESS* dbproc;
	Int8 tzQhr;

	Text::StringBuilderUTF8 *errMsg;
	IO::LogTool *log;
	NotNullPtr<Text::String> host;
	NotNullPtr<Text::String> username;
	NotNullPtr<Text::String> password;
	Text::String *database;
};

int TDSConnErrHdlr(DBPROCESS * dbproc, int severity, int dberr, int oserr, char *dberrstr, char *oserrstr)
{
	printf("TDS: Error occurs, severity = %d, dberr = %d, oserr = %d, msg: %s\r\n", severity, dberr, oserr, dberrstr);
	return 0;
}

int TDSConnMsgHdlr(DBPROCESS * dbproc, DBINT msgno, int msgstate, int severity, char *msgtext, char *srvname, char *proc, int line)
{
#if defined(VERBOSE)
	printf("TDS: Messages, msgno = %d, msgstate = %d, severity = %d, server = %s, msg: %s\r\n", msgno, msgstate, severity, srvname, msgtext);
#endif
	return 0;
}

DB::TDSConn::TDSConn(Text::CStringNN serverHost, UInt16 port, Bool encrypt, Text::CString database, Text::CString userName, Text::CString password, IO::LogTool *log, Text::StringBuilderUTF8 *errMsg) : DBConn(serverHost)
{
	if (!inited)
	{
		dbinit();
		dberrhandle(TDSConnErrHdlr);
		dbmsghandle(TDSConnMsgHdlr);
		inited = true;
	}
	this->sqlType = SQLType::MSSQL;
	this->clsData = MemAlloc(ClassData, 1);
	this->clsData->dbproc = 0;
	this->clsData->tzQhr = Data::DateTimeUtil::GetLocalTzQhr();
	Text::StringBuilderUTF8 sb;
	sb.Append(serverHost);
	sb.AppendUTF8Char(':');
	sb.AppendU16(port);
	this->clsData->host = Text::String::New(sb.ToCString());
	this->clsData->username = Text::String::New(userName);
	this->clsData->password = Text::String::New(password);
	this->clsData->database = Text::String::NewOrNull(database);
	this->clsData->errMsg = errMsg;
	this->clsData->log = log;
	this->Reconnect();
	if (this->clsData->dbproc != 0)
	{
		DB::DBReader *r = this->ExecuteReader(CSTR("select SYSDATETIME(), GETUTCDATE()"));
		if (r)
		{
			if (r->ReadNext())
			{
				Data::Timestamp ts1 = r->GetTimestamp(0);
				Data::Timestamp ts2 = r->GetTimestamp(1);
				this->clsData->tzQhr = (Int8)((ts1.inst.sec + 1 - ts2.inst.sec) / 900);
			}
			this->CloseReader(r);
		}
	}
}

DB::TDSConn::~TDSConn()
{
	this->Close();
	this->clsData->host->Release();
	this->clsData->username->Release();
	this->clsData->password->Release();
	SDEL_STRING(this->clsData->database);
	MemFree(this->clsData);
}

Bool DB::TDSConn::IsConnected() const
{
	return this->clsData->dbproc != 0;
}

NotNullPtr<Text::String> DB::TDSConn::GetConnHost() const
{
	return this->clsData->host;
}

Text::String *DB::TDSConn::GetConnDB() const
{
	return this->clsData->database;
}

NotNullPtr<Text::String> DB::TDSConn::GetConnUID() const
{
	return this->clsData->username;
}

NotNullPtr<Text::String> DB::TDSConn::GetConnPWD() const
{
	return this->clsData->password;
}

DB::SQLType DB::TDSConn::GetSQLType() const
{
	return this->sqlType;
}

DB::DBConn::ConnType DB::TDSConn::GetConnType() const
{
	return CT_TDSCONN;
}

Int8 DB::TDSConn::GetTzQhr() const
{
	return this->clsData->tzQhr;
}

void DB::TDSConn::ForceTz(Int8 tzQhr)
{
	this->clsData->tzQhr = tzQhr;
}

void DB::TDSConn::GetConnName(NotNullPtr<Text::StringBuilderUTF8> sb)
{
	sb->AppendC(UTF8STRC("TDS:"));
	sb->Append(this->clsData->host);
	sb->AppendUTF8Char(',');
	sb->Append(this->clsData->username);
	if (this->clsData->database)
	{
		sb->AppendUTF8Char(',');
		sb->Append(this->clsData->database);
	}
}

void DB::TDSConn::Close()
{
	if (this->clsData->dbproc)
	{
		dbclose(this->clsData->dbproc);
		this->clsData->dbproc = 0;
	}
}

OSInt DB::TDSConn::ExecuteNonQuery(Text::CString sql)
{
	DBReader *r = this->ExecuteReader(sql);
	if (r == 0)
		return -2;
	OSInt rows = r->GetRowChanged();
	this->CloseReader(r);
	return rows;
}

DB::DBReader *DB::TDSConn::ExecuteReader(Text::CString sql)
{
	if (this->clsData->dbproc == 0)
		return 0;
	this->cmdMut.Lock();
#if defined(VERBOSE)
	printf("TDS: Execute SQL: %s\r\n", sql.v);
#endif
	dbcmd(this->clsData->dbproc, (const Char*)sql.v);
	RETCODE ret = dbsqlexec(this->clsData->dbproc);
	if (ret == FAIL)
	{
		this->cmdMut.Unlock();
		printf("TDS: Execute SQL Error\r\n");
		return 0;
	}
	TDSConnReader *r;
	NEW_CLASS(r, TDSConnReader(this->clsData->dbproc, this->clsData->tzQhr));
	return r;
}

void DB::TDSConn::CloseReader(DBReader *r)
{
	TDSConnReader *reader = (TDSConnReader*)r;
	DEL_CLASS(reader);
	this->cmdMut.Unlock();
}

void DB::TDSConn::GetLastErrorMsg(NotNullPtr<Text::StringBuilderUTF8> str)
{
	///////////////////////////////////////
}

Bool DB::TDSConn::IsLastDataError()
{
	///////////////////////////////////////
	return false;
}

void DB::TDSConn::Reconnect()
{
	this->Close();
	LOGINREC *login = dblogin();
	if (login == 0)
	{
		if (this->clsData->errMsg)
			this->clsData->errMsg->AppendC(UTF8STRC("Error in allocating login structure"));
		if (this->clsData->log)
			this->clsData->log->LogMessage(CSTR("TDS: Error in allocating login structure"), IO::LogHandler::LogLevel::Error);
		return;
	}
	DBSETLUSER(login, (const Char*)this->clsData->username->v);
	DBSETLPWD(login, (const Char*)this->clsData->password->v);
	DBSETLAPP(login, "SSWRTDS");
	DBSETLCHARSET(login, "utf8");
	if (this->clsData->database)
		DBSETLDBNAME(login, (const Char*)this->clsData->database->v);

	this->clsData->dbproc = dbopen(login, (const Char*)this->clsData->host->v);
	dbloginfree(login);
	if (this->clsData->dbproc == 0)
	{
		if (this->clsData->errMsg)
		{
			this->clsData->errMsg->AppendC(UTF8STRC("Error in connecting to "));
			this->clsData->errMsg->Append(this->clsData->host);
		}
		if (this->clsData->log)
		{
			Text::StringBuilderUTF8 sb;
			sb.AppendC(UTF8STRC("TDS: Error in connecting to "));
			sb.Append(this->clsData->host);
			this->clsData->log->LogMessage(sb.ToCString(), IO::LogHandler::LogLevel::Error);
		}
		return;
	}
	dbsetuserdata(this->clsData->dbproc, (BYTE*)this);
}

void *DB::TDSConn::BeginTransaction()
{
	///////////////////////////////////////
	return 0;
}

void DB::TDSConn::Commit(void *tran)
{
	///////////////////////////////////////
}

void DB::TDSConn::Rollback(void *tran)
{
	///////////////////////////////////////
}

UOSInt DB::TDSConn::QueryTableNames(Text::CString schemaName, Data::ArrayListNN<Text::String> *names)
{
	if (this->sqlType == DB::SQLType::MSSQL)
	{
		UOSInt ret = 0;
		DB::SQLBuilder sql(DB::SQLType::MSSQL, false, this->GetTzQhr());
		sql.AppendCmdC(CSTR("select TABLE_SCHEMA, TABLE_NAME from INFORMATION_SCHEMA.TABLES where TABLE_TYPE='BASE TABLE' and TABLE_SCHEMA="));
		if (schemaName.leng == 0)
		{
			sql.AppendStrC(CSTR("dbo"));
		}
		else
		{
			sql.AppendStrC(schemaName);
		}
		DB::DBReader *r = this->ExecuteReader(sql.ToCString());
		if (r)
		{
			Text::StringBuilderUTF8 sb;
			while (r->ReadNext())
			{
				sb.ClearStr();
				if (r->GetStr(1, sb))
				{
					names->Add(Text::String::New(sb.ToCString()));
					ret++;
				}
			}
			this->CloseReader(r);
		}
		return ret;
	}
	return 0;
}

DB::DBReader *DB::TDSConn::QueryTableData(Text::CString schemaName, Text::CString tableName, Data::ArrayListNN<Text::String> *columnNames, UOSInt ofst, UOSInt maxCnt, Text::CString ordering, Data::QueryConditions *condition)
{
	UTF8Char sbuff[512];
	UTF8Char *sptr;
	UOSInt i;
	UOSInt j;
	Text::StringBuilderUTF8 sb;
	sb.AppendC(UTF8STRC("select "));
	if (this->sqlType == DB::SQLType::MSSQL || this->sqlType == DB::SQLType::Access)
	{
		if (maxCnt > 0)
		{
			sb.AppendC(UTF8STRC("TOP "));
			sb.AppendUOSInt(maxCnt);
			sb.AppendUTF8Char(' ');
		}
	}
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
				sb.AppendC(UTF8STRC(","));
			}
			sptr = DB::DBUtil::SDBColUTF8(sbuff, columnNames->GetItem(i)->v, this->sqlType);
			sb.AppendC(sbuff, (UOSInt)(sptr - sbuff));
			i++;
		}
	}
	sb.AppendC(UTF8STRC(" from "));
	if (schemaName.leng > 0 && DB::DBUtil::HasSchema(this->sqlType))
	{
		sptr = DB::DBUtil::SDBColUTF8(sbuff, schemaName.v, this->sqlType);
		sb.AppendP(sbuff, sptr);
		sb.AppendUTF8Char('.');
	}
	sptr = DB::DBUtil::SDBColUTF8(sbuff, tableName.v, this->sqlType);
	sb.AppendP(sbuff, sptr);
	if (this->sqlType == DB::SQLType::SQLite || this->sqlType == DB::SQLType::MySQL)
	{
		if (maxCnt > 0)
		{
			sb.AppendC(UTF8STRC(" LIMIT "));
			sb.AppendUOSInt(maxCnt);
		}
	}
	return this->ExecuteReader(sb.ToCString());
}
