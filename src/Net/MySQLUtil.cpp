#include "Stdafx.h"
#include "Data/ByteTool.h"
#include "Net/MySQLUtil.h"
#include "Text/MyString.h"

const UInt8 *Net::MySQLUtil::ReadLenencInt(const UInt8 *buff, UInt64 *val)
{
	if (buff[0] < 251)
	{
		*val = buff[0];
		buff++;
	}
	else if (buff[0] == 251)
	{
		*val = 0; //NULL
		buff++;
	}
	else if (buff[0] == 252)
	{
		*val = ReadUInt16(&buff[1]);
		buff += 3;
	}
	else if (buff[0] == 253)
	{
		*val = ReadUInt24(&buff[1]);
		buff += 4;
	}
	else if (buff[0] == 254)
	{
		*val = ReadUInt64(&buff[1]);
		buff += 9;
	}
	else
	{
		*val = 0; // ERR_Packet
		buff += 1;
	}
	return buff;
}

UInt8 *Net::MySQLUtil::AppendLenencInt(UInt8 *buff, UInt64 val)
{
	if (val < 251)
	{
		buff[0] = (UInt8)val;
		return buff + 1;
	}
	else if (val < 0x10000)
	{
		buff[0] = 0xfc;
		WriteUInt16(&buff[1], (UInt16)val);
		return buff + 3;
	}
	else if (val < 0x1000000)
	{
		buff[0] = 0xfd;
		WriteInt24(&buff[1], (UInt32)val);
		return buff + 4;
	}
	else
	{
		buff[0] = 0xfe;
		WriteUInt64(&buff[1], val);
		return buff + 9;
	}
}

UInt8 *Net::MySQLUtil::AppendLenencStrC(UInt8 *buff, const UTF8Char *s, UOSInt len)
{
	if (s == 0)
	{
		buff[0] = 0;
		return buff + 1;
	}
	else
	{
		buff = AppendLenencInt(buff, len);
		MemCopyNO(buff, s, len);
		buff += len;
		return buff;
	}
}

Net::MySQLUtil::MySQLType Net::MySQLUtil::ColType2MySQLType(DB::DBUtil::ColType colType)
{
	switch (colType)
	{
		case DB::DBUtil::CT_UInt32:
			return Net::MySQLUtil::MYSQL_TYPE_LONG;
		case DB::DBUtil::CT_Int32:
			return Net::MySQLUtil::MYSQL_TYPE_LONG;
		case DB::DBUtil::CT_VarChar:
			return Net::MySQLUtil::MYSQL_TYPE_VARCHAR;
		case DB::DBUtil::CT_Char:
			return Net::MySQLUtil::MYSQL_TYPE_VARCHAR;
		case DB::DBUtil::CT_DateTime:
			return Net::MySQLUtil::MYSQL_TYPE_DATETIME;
		case DB::DBUtil::CT_DateTime2:
			return Net::MySQLUtil::MYSQL_TYPE_DATETIME2;
		case DB::DBUtil::CT_Double:
			return Net::MySQLUtil::MYSQL_TYPE_DOUBLE;
		case DB::DBUtil::CT_Float:
			return Net::MySQLUtil::MYSQL_TYPE_FLOAT;
		case DB::DBUtil::CT_Bool:
			return Net::MySQLUtil::MYSQL_TYPE_BIT;
		case DB::DBUtil::CT_Byte:
			return Net::MySQLUtil::MYSQL_TYPE_TINY;
		case DB::DBUtil::CT_Int16:
			return Net::MySQLUtil::MYSQL_TYPE_SHORT;
		case DB::DBUtil::CT_Int64:
			return Net::MySQLUtil::MYSQL_TYPE_LONGLONG;
		case DB::DBUtil::CT_UInt16:
			return Net::MySQLUtil::MYSQL_TYPE_SHORT;
		case DB::DBUtil::CT_UInt64:
			return Net::MySQLUtil::MYSQL_TYPE_LONGLONG;
		case DB::DBUtil::CT_Binary:
			return Net::MySQLUtil::MYSQL_TYPE_BLOB;
		case DB::DBUtil::CT_Vector:
			return Net::MySQLUtil::MYSQL_TYPE_GEOMETRY;
		case DB::DBUtil::CT_NVarChar:
			return Net::MySQLUtil::MYSQL_TYPE_VARCHAR;
		case DB::DBUtil::CT_UUID:
			return Net::MySQLUtil::MYSQL_TYPE_VARCHAR;
		case DB::DBUtil::CT_NChar:
			return Net::MySQLUtil::MYSQL_TYPE_VARCHAR;
		case DB::DBUtil::CT_Unknown:
			return Net::MySQLUtil::MYSQL_TYPE_VARCHAR;
		default:
			return Net::MySQLUtil::MYSQL_TYPE_NULL;
	}
}


DB::DBUtil::ColType Net::MySQLUtil::MySQLType2ColType(Net::MySQLUtil::MySQLType colType)
{
	switch (colType)
	{
		case Net::MySQLUtil::MYSQL_TYPE_DECIMAL:
			return DB::DBUtil::CT_Double;
		case Net::MySQLUtil::MYSQL_TYPE_TINY:
			return DB::DBUtil::CT_Byte;
		case Net::MySQLUtil::MYSQL_TYPE_SHORT:
			return DB::DBUtil::CT_Int16;
		case Net::MySQLUtil::MYSQL_TYPE_LONG:
			return DB::DBUtil::CT_Int32;
		case Net::MySQLUtil::MYSQL_TYPE_FLOAT:
			return DB::DBUtil::CT_Float;
		case Net::MySQLUtil::MYSQL_TYPE_DOUBLE:
			return DB::DBUtil::CT_Double;
		case Net::MySQLUtil::MYSQL_TYPE_NULL:
			return DB::DBUtil::CT_Unknown;
		case Net::MySQLUtil::MYSQL_TYPE_TIMESTAMP:
			return DB::DBUtil::CT_DateTime;
		case Net::MySQLUtil::MYSQL_TYPE_LONGLONG:
			return DB::DBUtil::CT_Int64;
		case Net::MySQLUtil::MYSQL_TYPE_INT24:
			return DB::DBUtil::CT_Int32;
		case Net::MySQLUtil::MYSQL_TYPE_DATE:
			return DB::DBUtil::CT_DateTime;
		case Net::MySQLUtil::MYSQL_TYPE_TIME:
			return DB::DBUtil::CT_DateTime;
		case Net::MySQLUtil::MYSQL_TYPE_DATETIME:
			return DB::DBUtil::CT_DateTime;
		case Net::MySQLUtil::MYSQL_TYPE_YEAR:
			return DB::DBUtil::CT_DateTime;
		case Net::MySQLUtil::MYSQL_TYPE_NEWDATE:
			return DB::DBUtil::CT_DateTime;
		case Net::MySQLUtil::MYSQL_TYPE_VARCHAR:
			return DB::DBUtil::CT_VarChar;
		case Net::MySQLUtil::MYSQL_TYPE_BIT:
			return DB::DBUtil::CT_Bool;
		case Net::MySQLUtil::MYSQL_TYPE_TIMESTAMP2:
			return DB::DBUtil::CT_DateTime;
		case Net::MySQLUtil::MYSQL_TYPE_DATETIME2:
			return DB::DBUtil::CT_DateTime;
		case Net::MySQLUtil::MYSQL_TYPE_TIME2:
			return DB::DBUtil::CT_DateTime;
		case Net::MySQLUtil::MYSQL_TYPE_NEWDECIMAL:
			return DB::DBUtil::CT_Double;
		case Net::MySQLUtil::MYSQL_TYPE_ENUM:
			return DB::DBUtil::CT_Int32;
		case Net::MySQLUtil::MYSQL_TYPE_SET:
			return DB::DBUtil::CT_Unknown;
		case Net::MySQLUtil::MYSQL_TYPE_TINY_BLOB:
			return DB::DBUtil::CT_Binary;
		case Net::MySQLUtil::MYSQL_TYPE_MEDIUM_BLOB:
			return DB::DBUtil::CT_Binary;
		case Net::MySQLUtil::MYSQL_TYPE_LONG_BLOB:
			return DB::DBUtil::CT_Binary;
		case Net::MySQLUtil::MYSQL_TYPE_BLOB:
			return DB::DBUtil::CT_Binary;
		case Net::MySQLUtil::MYSQL_TYPE_VAR_STRING:
			return DB::DBUtil::CT_VarChar;
		case Net::MySQLUtil::MYSQL_TYPE_STRING:
			return DB::DBUtil::CT_VarChar;
		case Net::MySQLUtil::MYSQL_TYPE_GEOMETRY:
			return DB::DBUtil::CT_Vector;
		default:
			return DB::DBUtil::CT_Unknown;
	}
}
