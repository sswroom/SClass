#include "Stdafx.h"
#include "DB/DBReader.h"
#include "DB/SQLGenerator.h"


void DB::SQLGenerator::AppendColDef(DB::DBUtil::SQLType sqlType, DB::SQLBuilder *sql, DB::ColDef *col)
{
	sql->AppendCol(col->GetColName()->v);
	sql->AppendCmdC(CSTR(" "));
	AppendColType(sqlType, sql, col->GetColType(), col->GetColSize(), col->GetColDP(), col->IsAutoInc());
	if (col->IsNotNull())
	{
		sql->AppendCmdC(CSTR(" NOT NULL"));
	}
	if (sqlType == DB::DBUtil::SQLType::MSSQL)
	{
		if (col->IsAutoInc())
		{
			sql->AppendCmdC(CSTR(" IDENTITY(1,1)"));
		}
	}
	else if (sqlType == DB::DBUtil::SQLType::MySQL)
	{
		if (col->IsAutoInc())
		{
			sql->AppendCmdC(CSTR(" AUTO_INCREMENT"));
		}
	}
	else if (sqlType == DB::DBUtil::SQLType::SQLite)
	{
		if (col->IsAutoInc() && col->IsPK())
		{
			sql->AppendCmdC(CSTR(" PRIMARY KEY"));
		}
		if (col->IsAutoInc() && (col->GetColType() == DB::DBUtil::CT_Int32 || col->GetColType() == DB::DBUtil::CT_UInt32))
		{
			sql->AppendCmdC(CSTR(" AUTOINCREMENT"));
		}
	}
	else if (sqlType == DB::DBUtil::SQLType::Access)
	{
		if (col->IsPK())
		{
			sql->AppendCmdC(CSTR(" PRIMARY KEY"));
		}
	}

	if (col->GetDefVal())
	{
		sql->AppendCmdC(CSTR(" DEFAULT "));
		col->GetDefVal(sql);
		//sql->AppendStr(col->GetDefVal());
	}
}

void DB::SQLGenerator::AppendColType(DB::DBUtil::SQLType sqlType, DB::SQLBuilder *sql, DB::DBUtil::ColType colType, UOSInt colSize, UOSInt colDP, Bool autoInc)
{
	switch (sqlType)
	{
	case DB::DBUtil::SQLType::MySQL:
		switch (colType)
		{
		case DB::DBUtil::CT_UTF8Char:
			sql->AppendCmdC(CSTR("CHAR("));
			sql->AppendUInt32((UInt32)colSize);
			sql->AppendCmdC(CSTR(")"));
			break;
		case DB::DBUtil::CT_UTF16Char:
			sql->AppendCmdC(CSTR("CHAR("));
			sql->AppendUInt32((UInt32)colSize);
			sql->AppendCmdC(CSTR(")"));
			break;
		case DB::DBUtil::CT_UTF32Char:
			sql->AppendCmdC(CSTR("CHAR("));
			sql->AppendUInt32((UInt32)colSize);
			sql->AppendCmdC(CSTR(")"));
			break;
		case DB::DBUtil::CT_VarUTF8Char:
			if (colSize == (UOSInt)-1)
			{
				sql->AppendCmdC(CSTR("LONGTEXT"));
			}
			else if (colSize >= 16384)
			{
				sql->AppendCmdC(CSTR("TEXT"));
			}
			else
			{
				sql->AppendCmdC(CSTR("VARCHAR("));
				sql->AppendUInt32((UInt32)colSize);
				sql->AppendCmdC(CSTR(")"));
			}
			break;
		case DB::DBUtil::CT_VarUTF16Char:
			if (colSize == (UOSInt)-1)
			{
				sql->AppendCmdC(CSTR("LONGTEXT"));
			}
			else if (colSize >= 16384)
			{
				sql->AppendCmdC(CSTR("TEXT"));
			}
			else
			{
				sql->AppendCmdC(CSTR("VARCHAR("));
				sql->AppendUInt32((UInt32)colSize);
				sql->AppendCmdC(CSTR(")"));
			}
			break;
		case DB::DBUtil::CT_VarUTF32Char:
			if (colSize == (UOSInt)-1)
			{
				sql->AppendCmdC(CSTR("LONGTEXT"));
			}
			else if (colSize >= 16384)
			{
				sql->AppendCmdC(CSTR("TEXT"));
			}
			else
			{
				sql->AppendCmdC(CSTR("VARCHAR("));
				sql->AppendUInt32((UInt32)colSize);
				sql->AppendCmdC(CSTR(")"));
			}
			break;
		case DB::DBUtil::CT_DateTime:
		case DB::DBUtil::CT_DateTimeTZ:
			sql->AppendCmdC(CSTR("DATETIME("));
			if (colSize > 6)
			{
				sql->AppendUInt32(6);
			}
			else
			{
				sql->AppendUInt32((UInt32)colSize);
			}
			sql->AppendCmdC(CSTR(")"));
			break;
		case DB::DBUtil::CT_Date:
			sql->AppendCmdC(CSTR("DATE"));
			break;
		case DB::DBUtil::CT_Double:
			sql->AppendCmdC(CSTR("DOUBLE"));
			break;
		case DB::DBUtil::CT_Float:
			sql->AppendCmdC(CSTR("FLOAT"));
			break;
		case DB::DBUtil::CT_Decimal:
			sql->AppendCmdC(CSTR("DECIMAL("));
			sql->AppendUInt32((UInt32)colSize);
			sql->AppendCmdC(CSTR(","));
			sql->AppendUInt32((UInt32)colDP);
			sql->AppendCmdC(CSTR(")"));
			break;
		case DB::DBUtil::CT_Bool:
			sql->AppendCmdC(CSTR("TINYINT(1)"));
			break;
		case DB::DBUtil::CT_Byte:
			sql->AppendCmdC(CSTR("TINYINT(3)"));
			break;
		case DB::DBUtil::CT_Int16:
			sql->AppendCmdC(CSTR("SMALLINT"));
			break;
		case DB::DBUtil::CT_Int32:
			sql->AppendCmdC(CSTR("INTEGER"));
			break;
		case DB::DBUtil::CT_Int64:
			sql->AppendCmdC(CSTR("BIGINT"));
			break;
		case DB::DBUtil::CT_UInt16:
			sql->AppendCmdC(CSTR("SMALLINT UNSIGNED"));
			break;
		case DB::DBUtil::CT_UInt32:
			sql->AppendCmdC(CSTR("INTEGER UNSIGNED"));
			break;
		case DB::DBUtil::CT_UInt64:
			sql->AppendCmdC(CSTR("BIGINT UNSIGNED"));
			break;
		case DB::DBUtil::CT_Binary:
			if (colSize >= 16384)
			{
				sql->AppendCmdC(CSTR("BLOB"));
			}
			else
			{
				sql->AppendCmdC(CSTR("VARBINARY("));
				sql->AppendUInt32((UInt32)colSize);
				sql->AppendCmdC(CSTR(")"));
			}
			break;
		case DB::DBUtil::CT_Vector:
			sql->AppendCmdC(CSTR("GEOMETRY"));
			break;
		case DB::DBUtil::CT_UUID:
			sql->AppendCmdC(CSTR("VARCHAR(36)"));
			break;
		case DB::DBUtil::CT_Unknown:
		default:
			////////////////////////////////////////////////////////
			break;
		}
		break;
	case DB::DBUtil::SQLType::MSSQL:
		switch (colType)
		{
		case DB::DBUtil::CT_UTF8Char:
			sql->AppendCmdC(CSTR("CHAR("));
			sql->AppendUInt32((UInt32)colSize);
			sql->AppendCmdC(CSTR(")"));
			break;
		case DB::DBUtil::CT_UTF16Char:
			sql->AppendCmdC(CSTR("NCHAR("));
			sql->AppendUInt32((UInt32)colSize);
			sql->AppendCmdC(CSTR(")"));
			break;
		case DB::DBUtil::CT_UTF32Char:
			sql->AppendCmdC(CSTR("NCHAR("));
			sql->AppendUInt32((UInt32)(colSize * 2));
			sql->AppendCmdC(CSTR(")"));
			break;
		case DB::DBUtil::CT_VarUTF8Char:
			if (colSize == (UOSInt)-1)
			{
				sql->AppendCmdC(CSTR("VARCHAR(MAX)"));
			}
			else if (colSize > 8000)
			{
				sql->AppendCmdC(CSTR("VARCHAR(MAX)"));
			}
			else
			{
				sql->AppendCmdC(CSTR("VARCHAR("));
				sql->AppendUInt32((UInt32)colSize);
				sql->AppendCmdC(CSTR(")"));
			}
			break;
		case DB::DBUtil::CT_VarUTF16Char:
			if (colSize == (UOSInt)-1)
			{
				sql->AppendCmdC(CSTR("NVARCHAR(MAX)"));
			}
			else if (colSize > 4000)
			{
				sql->AppendCmdC(CSTR("NVARCHAR(MAX)"));
			}
			else
			{
				sql->AppendCmdC(CSTR("NVARCHAR("));
				sql->AppendUInt32((UInt32)colSize);
				sql->AppendCmdC(CSTR(")"));
			}
			break;
		case DB::DBUtil::CT_VarUTF32Char:
			if (colSize == (UOSInt)-1)
			{
				sql->AppendCmdC(CSTR("NVARCHAR(MAX)"));
			}
			else if (colSize > 2000)
			{
				sql->AppendCmdC(CSTR("NVARCHAR(MAX)"));
			}
			else
			{
				sql->AppendCmdC(CSTR("NVARCHAR("));
				sql->AppendUInt32((UInt32)(colSize * 2));
				sql->AppendCmdC(CSTR(")"));
			}
			break;
		case DB::DBUtil::CT_Date:
			sql->AppendCmdC(CSTR("DATE"));
			break;
		case DB::DBUtil::CT_DateTime:
			sql->AppendCmdC(CSTR("DATETIME2("));
			sql->AppendUInt32((UInt32)colSize);
			sql->AppendCmdC(CSTR(")"));
			break;
		case DB::DBUtil::CT_DateTimeTZ:
			sql->AppendCmdC(CSTR("DATETIMEOFFSET("));
			sql->AppendUInt32((UInt32)colSize);
			sql->AppendCmdC(CSTR(")"));
			break;
		case DB::DBUtil::CT_Double:
			sql->AppendCmdC(CSTR("BINARY_DOUBLE"));
			break;
		case DB::DBUtil::CT_Float:
			sql->AppendCmdC(CSTR("BINARY_FLOAT"));
			break;
		case DB::DBUtil::CT_Decimal:
			sql->AppendCmdC(CSTR("DECIMAL("));
			sql->AppendUInt32((UInt32)colSize);
			sql->AppendCmdC(CSTR(","));
			sql->AppendUInt32((UInt32)colDP);
			sql->AppendCmdC(CSTR(")"));
			break;
		case DB::DBUtil::CT_Bool:
			sql->AppendCmdC(CSTR("BOOL"));
			break;
		case DB::DBUtil::CT_Byte:
			sql->AppendCmdC(CSTR("BYTE"));
			break;
		case DB::DBUtil::CT_Int16:
			sql->AppendCmdC(CSTR("SMALLINT"));
			break;
		case DB::DBUtil::CT_Int32:
			sql->AppendCmdC(CSTR("INT"));
			break;
		case DB::DBUtil::CT_Int64:
			sql->AppendCmdC(CSTR("BIGINT"));
			break;
		case DB::DBUtil::CT_UInt16:
			sql->AppendCmdC(CSTR("SMALLINT"));
			break;
		case DB::DBUtil::CT_UInt32:
			sql->AppendCmdC(CSTR("INT"));
			break;
		case DB::DBUtil::CT_UInt64:
			sql->AppendCmdC(CSTR("BIGINT"));
			break;
		case DB::DBUtil::CT_Binary:
			if (colSize > 8000)
			{
				sql->AppendCmdC(CSTR("VARBINARY(MAX)"));
			}
			else
			{
				sql->AppendCmdC(CSTR("VARBINARY("));
				sql->AppendUInt32((UInt32)colSize);
				sql->AppendCmdC(CSTR(")"));
			}
			break;
		case DB::DBUtil::CT_Vector:
			sql->AppendCmdC(CSTR("GEOMETRY"));
			break;
		case DB::DBUtil::CT_UUID:
			sql->AppendCmdC(CSTR("UNIQUEIDENTIFIER"));
			break;
		case DB::DBUtil::CT_Unknown:
		default:
			////////////////////////////////////////
			break;
		}
		break;
	case DB::DBUtil::SQLType::MDBTools:
	case DB::DBUtil::SQLType::Access:
		switch (colType)
		{
		case DB::DBUtil::CT_UTF8Char:
			sql->AppendCmdC(CSTR("CHAR("));
			sql->AppendUInt32((UInt32)colSize);
			sql->AppendCmdC(CSTR(")"));
			break;
		case DB::DBUtil::CT_UTF16Char:
			sql->AppendCmdC(CSTR("CHAR("));
			sql->AppendUInt32((UInt32)(colSize * 3));
			sql->AppendCmdC(CSTR(")"));
			break;
		case DB::DBUtil::CT_UTF32Char:
			sql->AppendCmdC(CSTR("CHAR("));
			sql->AppendUInt32((UInt32)(colSize * 4));
			sql->AppendCmdC(CSTR(")"));
			break;
		case DB::DBUtil::CT_VarUTF8Char:
			if (colSize == (UOSInt)-1)
			{
				sql->AppendCmdC(CSTR("TEXT"));
			}
			else if (colSize > 255)
			{
				sql->AppendCmdC(CSTR("TEXT"));
			}
			else
			{
				sql->AppendCmdC(CSTR("VARCHAR("));
				sql->AppendUInt32((UInt32)colSize);
				sql->AppendCmdC(CSTR(")"));
			}
			break;
		case DB::DBUtil::CT_VarUTF16Char:
			if (colSize == (UOSInt)-1)
			{
				sql->AppendCmdC(CSTR("TEXT"));
			}
			else if (colSize > 255)
			{
				sql->AppendCmdC(CSTR("TEXT"));
			}
			else
			{
				sql->AppendCmdC(CSTR("VARCHAR("));
				sql->AppendUInt32((UInt32)(colSize * 3));
				sql->AppendCmdC(CSTR(")"));
			}
			break;
		case DB::DBUtil::CT_VarUTF32Char:
			if (colSize == (UOSInt)-1)
			{
				sql->AppendCmdC(CSTR("TEXT"));
			}
			else if (colSize > 255)
			{
				sql->AppendCmdC(CSTR("TEXT"));
			}
			else
			{
				sql->AppendCmdC(CSTR("VARCHAR("));
				sql->AppendUInt32((UInt32)(colSize * 4));
				sql->AppendCmdC(CSTR(")"));
			}
			break;
		case DB::DBUtil::CT_Date:
			sql->AppendCmdC(CSTR("DATE"));
			break;
		case DB::DBUtil::CT_DateTime:
			sql->AppendCmdC(CSTR("DATETIME"));
			break;
		case DB::DBUtil::CT_DateTimeTZ:
			sql->AppendCmdC(CSTR("DATETIME"));
			break;
		case DB::DBUtil::CT_Double:
			sql->AppendCmdC(CSTR("DOUBLE"));
			break;
		case DB::DBUtil::CT_Float:
			sql->AppendCmdC(CSTR("SINGLE"));
			break;
		case DB::DBUtil::CT_Decimal:
			sql->AppendCmdC(CSTR("DECIMAL("));
			sql->AppendUInt32((UInt32)colSize);
			sql->AppendCmdC(CSTR(","));
			sql->AppendUInt32((UInt32)colDP);
			sql->AppendCmdC(CSTR(")"));
			break;
		case DB::DBUtil::CT_Bool:
			sql->AppendCmdC(CSTR("BOOL"));
			break;
		case DB::DBUtil::CT_Byte:
			sql->AppendCmdC(CSTR("BYTE"));
			break;
		case DB::DBUtil::CT_Int16:
			sql->AppendCmdC(CSTR("SMALLINT"));
			break;
		case DB::DBUtil::CT_Int32:
			sql->AppendCmdC(CSTR("INT"));
			break;
		case DB::DBUtil::CT_Int64:
			sql->AppendCmdC(CSTR("BIGINT"));
			break;
		case DB::DBUtil::CT_UInt16:
			sql->AppendCmdC(CSTR("SMALLINT"));
			break;
		case DB::DBUtil::CT_UInt32:
			sql->AppendCmdC(CSTR("INT"));
			break;
		case DB::DBUtil::CT_UInt64:
			sql->AppendCmdC(CSTR("BIGINT"));
			break;
		case DB::DBUtil::CT_Binary:
		case DB::DBUtil::CT_Vector:
		case DB::DBUtil::CT_UUID:
		case DB::DBUtil::CT_Unknown:
		default:
			////////////////////////////////////////
			break;
		}
		break;
	case DB::DBUtil::SQLType::SQLite:
		switch (colType)
	 	{
		case DB::DBUtil::CT_UTF8Char:
			sql->AppendCmdC(CSTR("CHAR("));
			sql->AppendUInt32((UInt32)colSize);
			sql->AppendCmdC(CSTR(")"));
			break;
		case DB::DBUtil::CT_UTF16Char:
			sql->AppendCmdC(CSTR("CHAR("));
			sql->AppendUInt32((UInt32)(colSize * 3));
			sql->AppendCmdC(CSTR(")"));
			break;
		case DB::DBUtil::CT_UTF32Char:
			sql->AppendCmdC(CSTR("CHAR("));
			sql->AppendUInt32((UInt32)(colSize * 4));
			sql->AppendCmdC(CSTR(")"));
			break;
		case DB::DBUtil::CT_VarUTF8Char:
			if (colSize == (UOSInt)-1)
			{
				sql->AppendCmdC(CSTR("TEXT"));
			}
			else
			{
				sql->AppendCmdC(CSTR("VARCHAR("));
				sql->AppendUInt32((UInt32)colSize);
				sql->AppendCmdC(CSTR(")"));
			}
			break;
		case DB::DBUtil::CT_VarUTF16Char:
			if (colSize == (UOSInt)-1)
			{
				sql->AppendCmdC(CSTR("TEXT"));
			}
			else
			{
				sql->AppendCmdC(CSTR("VARCHAR("));
				sql->AppendUInt32((UInt32)(colSize * 3));
				sql->AppendCmdC(CSTR(")"));
			}
			break;
		case DB::DBUtil::CT_VarUTF32Char:
			if (colSize == (UOSInt)-1)
			{
				sql->AppendCmdC(CSTR("TEXT"));
			}
			else
			{
				sql->AppendCmdC(CSTR("VARCHAR("));
				sql->AppendUInt32((UInt32)(colSize * 4));
				sql->AppendCmdC(CSTR(")"));
			}
			break;
		case DB::DBUtil::CT_Date:
			sql->AppendCmdC(CSTR("DATE"));
			break;
		case DB::DBUtil::CT_DateTime:
			sql->AppendCmdC(CSTR("DATETIME"));
			break;
		case DB::DBUtil::CT_DateTimeTZ:
			sql->AppendCmdC(CSTR("DATETIME"));
			break;
		case DB::DBUtil::CT_Double:
			sql->AppendCmdC(CSTR("DOUBLE"));
			break;
		case DB::DBUtil::CT_Float:
			sql->AppendCmdC(CSTR("FLOAT"));
			break;
		case DB::DBUtil::CT_Decimal:
			sql->AppendCmdC(CSTR("DECIMAL("));
			sql->AppendUInt32((UInt32)colSize);
			sql->AppendCmdC(CSTR(","));
			sql->AppendUInt32((UInt32)colDP);
			sql->AppendCmdC(CSTR(")"));
			break;
		case DB::DBUtil::CT_Bool:
			sql->AppendCmdC(CSTR("BOOLEAN"));
			break;
		case DB::DBUtil::CT_Byte:
			sql->AppendCmdC(CSTR("TINYINT"));
			break;
		case DB::DBUtil::CT_Int16:
			sql->AppendCmdC(CSTR("SMALLINT"));
			break;
		case DB::DBUtil::CT_Int32:
			sql->AppendCmdC(CSTR("INTEGER"));
			break;
		case DB::DBUtil::CT_Int64:
			sql->AppendCmdC(CSTR("BIGINT"));
			break;
		case DB::DBUtil::CT_UInt16:
			sql->AppendCmdC(CSTR("SMALLINT"));
			break;
		case DB::DBUtil::CT_UInt32:
			sql->AppendCmdC(CSTR("INTEGER"));
			break;
		case DB::DBUtil::CT_UInt64:
			sql->AppendCmdC(CSTR("BIGINT"));
			break;
		case DB::DBUtil::CT_Binary:
			sql->AppendCmdC(CSTR("BLOB"));
			break;
		case DB::DBUtil::CT_Vector:
		case DB::DBUtil::CT_UUID:
		case DB::DBUtil::CT_Unknown:
		default:
			////////////////////////////////////////
			break;
		}
		break;
	case DB::DBUtil::SQLType::PostgreSQL:
		switch (colType)
	 	{
		case DB::DBUtil::CT_UTF8Char:
			sql->AppendCmdC(CSTR("char("));
			sql->AppendUInt32((UInt32)colSize);
			sql->AppendCmdC(CSTR(")"));
			break;
		case DB::DBUtil::CT_UTF16Char:
			sql->AppendCmdC(CSTR("char("));
			sql->AppendUInt32((UInt32)colSize);
			sql->AppendCmdC(CSTR(")"));
			break;
		case DB::DBUtil::CT_UTF32Char:
			sql->AppendCmdC(CSTR("char("));
			sql->AppendUInt32((UInt32)colSize);
			sql->AppendCmdC(CSTR(")"));
			break;
		case DB::DBUtil::CT_VarUTF8Char:
			if (colSize > 10485760)
			{
				sql->AppendCmdC(CSTR("text"));
			}
			else
			{
				sql->AppendCmdC(CSTR("varchar("));
				sql->AppendUInt32((UInt32)colSize);
				sql->AppendCmdC(CSTR(")"));
			}
			break;
		case DB::DBUtil::CT_VarUTF16Char:
			if (colSize > 10485760)
			{
				sql->AppendCmdC(CSTR("text"));
			}
			else
			{
				sql->AppendCmdC(CSTR("varchar("));
				sql->AppendUInt32((UInt32)colSize);
				sql->AppendCmdC(CSTR(")"));
			}
			break;
		case DB::DBUtil::CT_VarUTF32Char:
			if (colSize > 10485760)
			{
				sql->AppendCmdC(CSTR("text"));
			}
			else
			{
				sql->AppendCmdC(CSTR("varchar("));
				sql->AppendUInt32((UInt32)colSize);
				sql->AppendCmdC(CSTR(")"));
			}
			break;
		case DB::DBUtil::CT_Date:
			sql->AppendCmdC(CSTR("date"));
			break;
		case DB::DBUtil::CT_DateTime:
			sql->AppendCmdC(CSTR("timestamp("));
			if (colSize > 6)
			{
				sql->AppendUInt32(6);
			}
			else
			{
				sql->AppendUInt32((UInt32)colSize);
			}
			sql->AppendCmdC(CSTR(")"));
			break;
		case DB::DBUtil::CT_DateTimeTZ:
			sql->AppendCmdC(CSTR("timestamptz("));
			if (colSize > 6)
			{
				sql->AppendUInt32(6);
			}
			else
			{
				sql->AppendUInt32((UInt32)colSize);
			}
			sql->AppendCmdC(CSTR(")"));
			break;
		case DB::DBUtil::CT_Double:
			sql->AppendCmdC(CSTR("float8"));
			break;
		case DB::DBUtil::CT_Float:
			sql->AppendCmdC(CSTR("real"));
			break;
		case DB::DBUtil::CT_Decimal:
			sql->AppendCmdC(CSTR("numeric("));
			sql->AppendUInt32((UInt32)colSize);
			sql->AppendCmdC(CSTR(","));
			sql->AppendUInt32((UInt32)colDP);
			sql->AppendCmdC(CSTR(")"));
			break;
		case DB::DBUtil::CT_Bool:
			sql->AppendCmdC(CSTR("bool"));
			break;
		case DB::DBUtil::CT_Byte:
			sql->AppendCmdC(CSTR("smallint"));
			break;
		case DB::DBUtil::CT_Int16:
			if (autoInc)
			{
				sql->AppendCmdC(CSTR("smallserial"));
			}
			else
			{
				sql->AppendCmdC(CSTR("smallint"));
			}
			break;
		case DB::DBUtil::CT_Int32:
			if (autoInc)
			{
				sql->AppendCmdC(CSTR("serial"));
			}
			else
			{
				sql->AppendCmdC(CSTR("integer"));
			}
			break;
		case DB::DBUtil::CT_Int64:
			if (autoInc)
			{
				sql->AppendCmdC(CSTR("bigserial"));
			}
			else
			{
				sql->AppendCmdC(CSTR("bigint"));
			}
			break;
		case DB::DBUtil::CT_UInt16:
			if (autoInc)
			{
				sql->AppendCmdC(CSTR("smallserial"));
			}
			else
			{
				sql->AppendCmdC(CSTR("smallint"));
			}
			break;
		case DB::DBUtil::CT_UInt32:
			if (autoInc)
			{
				sql->AppendCmdC(CSTR("serial"));
			}
			else
			{
				sql->AppendCmdC(CSTR("integer"));
			}
			break;
		case DB::DBUtil::CT_UInt64:
			if (autoInc)
			{
				sql->AppendCmdC(CSTR("bigserial"));
			}
			else
			{
				sql->AppendCmdC(CSTR("bigint"));
			}
			break;
		case DB::DBUtil::CT_Binary:
			sql->AppendCmdC(CSTR("bytea"));
			break;
		case DB::DBUtil::CT_Vector:
			sql->AppendCmdC(CSTR("geometry"));
			break;
		case DB::DBUtil::CT_UUID:
			sql->AppendCmdC(CSTR("uuid"));
			break;
		case DB::DBUtil::CT_Unknown:
		default:
			////////////////////////////////////////
			break;
		}
		break;
	case DB::DBUtil::SQLType::Unknown:
	case DB::DBUtil::SQLType::Oracle:
	case DB::DBUtil::SQLType::WBEM:
	default:
		break;
	}
}

Bool DB::SQLGenerator::GenCreateTableCmd(DB::SQLBuilder *sql, Text::CString schemaName, Text::CString tableName, DB::TableDef *tabDef)
{
	DB::DBUtil::SQLType sqlType = sql->GetSQLType();
	UOSInt i;
	UOSInt j;
	UOSInt k;
	DB::ColDef *col;
	sql->AppendCmdC(CSTR("create table "));
	if (schemaName.leng > 0)
	{
		sql->AppendCol(schemaName.v);
		sql->AppendCmdC(CSTR("."));
	}
	sql->AppendCol(tableName.v);
	sql->AppendCmdC(CSTR(" ("));
	if (sqlType == DB::DBUtil::SQLType::Access || sqlType == DB::DBUtil::SQLType::MDBTools)
	{
		j = tabDef->GetColCnt();
		i = 0;
		while (i < j)
		{
			if (i > 0)
			{
				sql->AppendCmdC(CSTR(", "));
			}
			col = tabDef->GetCol(i++);
			AppendColDef(sqlType, sql, col);
		}
	}
	else if (sqlType == DB::DBUtil::SQLType::SQLite)
	{
		Bool hasAutoInc = false;
		j = tabDef->GetColCnt();
		i = 0;
		while (i < j)
		{
			col = tabDef->GetCol(i++);
			AppendColDef(sqlType, sql, col);
			if (col->IsAutoInc())
			{
				hasAutoInc = true;
			}
			if (i < j)
			{
				sql->AppendCmdC(CSTR(", "));
			}
		}
		if (!hasAutoInc)
		{
			i = 0;
			k = 0;
			sql->AppendCmdC(CSTR(", "));
			sql->AppendCmdC(CSTR("PRIMARY KEY ("));
			while (i < j)
			{
				col = tabDef->GetCol(i++);
				if (col->IsPK())
				{
					if (k > 0)
					{
						sql->AppendCmdC(CSTR(", "));
					}
					sql->AppendCol(col->GetColName()->v);
					k++;
				}
			}
			sql->AppendCmdC(CSTR(")"));
		}
	}
	else
	{
		j = tabDef->GetColCnt();
		i = 0;
		while (i < j)
		{
			col = tabDef->GetCol(i++);
			AppendColDef(sqlType, sql, col);
			if (i < j)
			{
				sql->AppendCmdC(CSTR(", "));
			}
		}
		i = 0;
		k = 0;
		while (i < j)
		{
			col = tabDef->GetCol(i++);
			if (col->IsPK())
			{
				if (k > 0)
				{
					sql->AppendCmdC(CSTR(", "));
				}
				else
				{
					sql->AppendCmdC(CSTR(", "));
					sql->AppendCmdC(CSTR("PRIMARY KEY ("));
				}
				sql->AppendCol(col->GetColName()->v);
				k++;
			}
		}
		if (k > 0)
		{
			sql->AppendCmdC(CSTR(")"));
		}
	}
	sql->AppendCmdC(CSTR(")"));
	if (sqlType == DB::DBUtil::SQLType::MySQL)
	{
		if (tabDef->GetEngine())
		{
			sql->AppendCmdC(CSTR(" ENGINE="));
			sql->AppendCmdC(tabDef->GetEngine()->ToCString());
		}
		if (tabDef->GetCharset())
		{
			sql->AppendCmdC(CSTR(" DEFAULT CHARSET="));
			sql->AppendCmdC(tabDef->GetCharset()->ToCString());
		}
	}
	return true;
}

Bool DB::SQLGenerator::GenInsertCmd(DB::SQLBuilder *sql, Text::CString schemaName, Text::CString tableName, DB::DBReader *r)
{
	UTF8Char tmpBuff[256];
	DB::DBUtil::ColType colType;
	UOSInt i;
	UOSInt j;

	Text::StringBuilderUTF8 sb;
	Data::DateTime dt;

	sql->AppendCmdC(CSTR("insert into "));
	if (schemaName.leng > 0)
	{
		sql->AppendCol(schemaName.v);
		sql->AppendCmdC(CSTR("."));
	}
	sql->AppendCol(tableName.v);
	sql->AppendCmdC(CSTR(" ("));
	j = r->ColCount();
	i = 1;
	r->GetName(0, tmpBuff);
	sql->AppendCol(tmpBuff);
	while (i < j)
	{
		r->GetName(i, tmpBuff);
		sql->AppendCmdC(CSTR(", "));
		sql->AppendCol(tmpBuff);
		i++;
	}
	sql->AppendCmdC(CSTR(") values ("));
	i = 0;
	while (i < j)
	{
		if (i > 0)
		{
			sql->AppendCmdC(CSTR(", "));
		}
		if (r->IsNull(i))
		{
			sql->AppendCmdC(CSTR("NULL"));
		}
		else
		{
			Math::Geometry::Vector2D *vec;
			UInt8 *binBuff;
			UOSInt colSize;
			colType = r->GetColType(i, &colSize);
			switch (colType)
			{
			case DB::DBUtil::CT_Bool:
				sql->AppendBool(r->GetBool(i));
				break;
			case DB::DBUtil::CT_DateTimeTZ:
			case DB::DBUtil::CT_DateTime:
			case DB::DBUtil::CT_Date:
				sql->AppendTS(r->GetTimestamp(i));
				break;
			case DB::DBUtil::CT_Int64:
				sql->AppendInt64(r->GetInt64(i));
				break;
			case DB::DBUtil::CT_Int32:
			case DB::DBUtil::CT_Int16:
				sql->AppendInt32(r->GetInt32(i));
				break;
			case DB::DBUtil::CT_UInt64:
				sql->AppendUInt64((UInt64)r->GetInt64(i));
				break;
			case DB::DBUtil::CT_UInt32:
			case DB::DBUtil::CT_UInt16:
			case DB::DBUtil::CT_Byte:
				sql->AppendUInt32((UInt32)r->GetInt32(i));
				break;
			case DB::DBUtil::CT_Double:
			case DB::DBUtil::CT_Float:
			case DB::DBUtil::CT_Decimal:
				sql->AppendDbl(r->GetDbl(i));
				break;
			case DB::DBUtil::CT_Vector:
				vec = r->GetVector(i);
				sql->AppendVector(vec);
				SDEL_CLASS(vec);
				break;
			case DB::DBUtil::CT_Binary:
				if (r->IsNull(i))
				{
					sql->AppendStrUTF8(0);
				}
				else
				{
					UOSInt sz = r->GetBinarySize(i);
					binBuff = MemAlloc(UInt8, sz);
					r->GetBinary(i, binBuff);
					sql->AppendBinary(binBuff, sz);
					MemFree(binBuff);
				}
				break;
			case DB::DBUtil::CT_UUID:
/*			{
				Data::UUID uuid;
				r->GetUUID(i, &uuid);
				sql->AppendUUID(&uuid);
			}*/
			case DB::DBUtil::CT_UTF8Char:
			case DB::DBUtil::CT_UTF16Char:
			case DB::DBUtil::CT_UTF32Char:
			case DB::DBUtil::CT_VarUTF8Char:
			case DB::DBUtil::CT_VarUTF16Char:
			case DB::DBUtil::CT_VarUTF32Char:
			case DB::DBUtil::CT_Unknown:
			default:
				sb.ClearStr();
				r->GetStr(i, &sb);
				sql->AppendStrC(sb.ToCString());
				break;
			}
		}
		i++;
	}
	sql->AppendCmdC(CSTR(")"));
	return true;
}

Bool DB::SQLGenerator::GenCreateDatabaseCmd(DB::SQLBuilder *sql, Text::CString databaseName)
{
	sql->AppendCmdC(CSTR("CREATE DATABASE "));
	sql->AppendCol(databaseName.v);
	return true;
}

Bool DB::SQLGenerator::GenDeleteDatabaseCmd(DB::SQLBuilder *sql, Text::CString databaseName)
{
	sql->AppendCmdC(CSTR("DROP DATABASE "));
	sql->AppendCol(databaseName.v);
	return true;
}

Bool DB::SQLGenerator::GenCreateSchemaCmd(DB::SQLBuilder *sql, Text::CString schemaName)
{
	sql->AppendCmdC(CSTR("CREATE SCHEMA "));
	sql->AppendCol(schemaName.v);
	return true;
}

Bool DB::SQLGenerator::GenDeleteSchemaCmd(DB::SQLBuilder *sql, Text::CString schemaName)
{
	sql->AppendCmdC(CSTR("DROP SCHEMA "));
	sql->AppendCol(schemaName.v);
	return true;
}

Bool DB::SQLGenerator::GenDropTableCmd(DB::SQLBuilder *sql, Text::CString tableName)
{
	sql->AppendCmdC(CSTR("drop table "));
	sql->AppendCol(tableName.v);
	return true;
}

Bool DB::SQLGenerator::GenDeleteTableCmd(DB::SQLBuilder *sql, Text::CString tableName)
{
	sql->AppendCmdC(CSTR("delete from "));
	sql->AppendCol(tableName.v);
	return true;
}

DB::SQLGenerator::PageStatus DB::SQLGenerator::GenSelectCmdPage(DB::SQLBuilder *sql, DB::TableDef *tabDef, DB::PageRequest *page)
{
	DB::SQLGenerator::PageStatus status;
	if (page)
	{
		status = PageStatus::NoPage;
	}
	else
	{
		status = PageStatus::Succ;
	}
	DB::ColDef *col;
	UOSInt i = 0;
	UOSInt j = tabDef->GetColCnt();
	sql->AppendCmdC(CSTR("select "));
	if (page && (sql->GetSQLType() == DB::DBUtil::SQLType::Access))
	{
		sql->AppendCmdC(CSTR("TOP "));
		sql->AppendInt32((Int32)((page->GetPageNum() + 1) * page->GetPageSize()));
		status = PageStatus::NoOffset;
	}
	while (i < j)
	{
		col = tabDef->GetCol(i);
		if (i > 0)
		{
			sql->AppendCmdC(CSTR(", "));
		}
		sql->AppendCol(col->GetColName()->v);
		i++;
	}
	sql->AppendCmdC(CSTR(" from "));
	sql->AppendTableName(tabDef);
	if (page)
	{
		Bool hasOrder = false;
		i = 1;
		j = page->GetSortingCount();
		if (j > 0)
		{
			hasOrder = true;
			sql->AppendCmdC(CSTR(" order by "));
			sql->AppendCol(page->GetSortColumn(0));
			if (page->IsSortDesc(0))
			{
				sql->AppendCmdC(CSTR(" desc"));
			}
			while (i < j)
			{
				sql->AppendCmdC(CSTR(", "));
				sql->AppendCol(page->GetSortColumn(i));
				if (page->IsSortDesc(i))
				{
					sql->AppendCmdC(CSTR(" desc"));
				}
				i++;
			}
		}

		if (sql->GetSQLType() == DB::DBUtil::SQLType::MySQL)
		{
			sql->AppendCmdC(CSTR(" LIMIT "));
			sql->AppendInt32((Int32)(page->GetPageNum() * page->GetPageSize()));
			sql->AppendCmdC(CSTR(", "));
			sql->AppendInt32((Int32)page->GetPageSize());
			status = PageStatus::Succ;
		}
		else if (sql->GetSQLType() == DB::DBUtil::SQLType::MSSQL)
		{
			if (!hasOrder)
			{
				i = 0;
				j = tabDef->GetColCnt();
				while (i < j)
				{
					col = tabDef->GetCol(i);
					if (col->IsPK())
					{
						if (hasOrder)
						{
							sql->AppendCmdC(CSTR(", "));
						}
						else
						{
							hasOrder = true;
							sql->AppendCmdC(CSTR(" order by "));
						}
						sql->AppendCol(col->GetColName()->v);
					}
					i++;
				}
			}
			if (hasOrder)
			{
				status = PageStatus::Succ;
				sql->AppendCmdC(CSTR(" offset "));
				sql->AppendInt32((Int32)(page->GetPageNum() * page->GetPageSize()));
				sql->AppendCmdC(CSTR(" row fetch next "));
				sql->AppendInt32((Int32)page->GetPageSize());
				sql->AppendCmdC(CSTR(" row only"));
			}
			else
			{
				status = PageStatus::NoPage;
			}
		}
	}
	return status;
}

UTF8Char *DB::SQLGenerator::GenInsertCmd(UTF8Char *sqlstr, DB::DBUtil::SQLType sqlType, Text::CString tableName, DB::DBReader *r)
{
	UTF8Char *currPtr;
	UTF8Char tmpBuff[256];
	UOSInt i;
	UOSInt j;

	currPtr = Text::StrConcatC(sqlstr, UTF8STRC("insert into "));
	currPtr = DB::DBUtil::SDBColUTF8(currPtr, tableName.v, sqlType);
	r->GetName(0, tmpBuff);
	currPtr = DB::DBUtil::SDBColUTF8(currPtr, tmpBuff, sqlType);
	j = r->ColCount();
	i = 1;
	while (i < j)
	{
		currPtr = Text::StrConcatC(currPtr, UTF8STRC(", "));
		r->GetName(i++, tmpBuff);
		currPtr = DB::DBUtil::SDBColUTF8(currPtr, tmpBuff, sqlType);
	}
	currPtr = Text::StrConcatC(currPtr, UTF8STRC(") values ("));
	if (r->IsNull(0))
	{
		currPtr = Text::StrConcatC(currPtr, UTF8STRC("NULL"));
	}
	else
	{
		r->GetStr(0, tmpBuff, sizeof(tmpBuff));
		currPtr = DB::DBUtil::SDBStrUTF8(currPtr, tmpBuff, sqlType);
	}
	i = 1;
	while (i < j)
	{
		currPtr = Text::StrConcatC(currPtr, UTF8STRC(", "));
		if (r->IsNull(i))
		{
			currPtr = Text::StrConcatC(currPtr, UTF8STRC("NULL"));
		}
		else
		{
			r->GetStr(i, tmpBuff, sizeof(tmpBuff));
			currPtr = DB::DBUtil::SDBStrUTF8(currPtr, tmpBuff, sqlType);
		}
		i++;
	}

	currPtr = Text::StrConcatC(currPtr, UTF8STRC(")"));
	return currPtr;
}