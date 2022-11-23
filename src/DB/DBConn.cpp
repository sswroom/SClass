#include "Stdafx.h"
#include "Data/DateTime.h"
#include "Data/FastStringMap.h"
#include "DB/DBConn.h"
#include "DB/DBReader.h"
#include "DB/TableDef.h"
#include "DB/SQL/CreateTableCommand.h"
#include "DB/SQL/SQLCommand.h"
#include "Text/MyString.h"
#include <stdio.h>

DB::DBConn::DBConn(Text::String *sourceName) : DB::ReadingDB(sourceName)
{
	this->lastDataError = DE_NO_ERROR;
}

DB::DBConn::DBConn(Text::CString sourceName) : DB::ReadingDB(sourceName)
{
	this->lastDataError = DE_NO_ERROR;
}

DB::DBConn::~DBConn()
{
}

DB::TableDef *DB::DBConn::GetTableDef(Text::CString schemaName, Text::CString tableName)
{
	UTF8Char buff[256];
	UTF8Char *ptr;
	switch (this->GetSQLType())
	{
	case DB::DBUtil::SQLType::MySQL:
	{
		OSInt i = 4;
		DB::DBReader *r = 0;
		ptr = Text::StrConcatC(buff, UTF8STRC("show table status where Name = "));
		ptr = DB::DBUtil::SDBStrUTF8(ptr, tableName.v, DB::DBUtil::SQLType::MySQL);

		while (i-- > 0 && r == 0)
		{
			r = this->ExecuteReader(CSTRP(buff, ptr));
		}
		if (r == 0)
			return 0;

		DB::TableDef *tab;

		if (r->ReadNext())
		{
			ptr = r->GetStr(0, buff, sizeof(buff));
			NEW_CLASS(tab, DB::TableDef(CSTRP(buff, ptr)));
			ptr = r->GetStr(1, buff, sizeof(buff));
			tab->SetEngine(CSTRP(buff, ptr));
			if (r->GetStr(17, buff, sizeof(buff)))
			{
				tab->SetComments(buff);
			}
			else
			{
				tab->SetComments(0);
			}
			if (r->GetStr(16, buff, sizeof(buff)))
			{
				tab->SetAttr(buff);
			}
			else
			{
				tab->SetAttr(0);
			}
			tab->SetSQLType(DB::DBUtil::SQLType::MySQL);
			tab->SetCharset(CSTR_NULL);
			this->CloseReader(r);
		}
		else
		{
			this->CloseReader(r);
			return 0;
		}
		DB::ColDef *col;
		ptr = Text::StrConcatC(buff, UTF8STRC("desc "));
		ptr = DB::DBUtil::SDBColUTF8(ptr, tableName.v, DB::DBUtil::SQLType::MySQL);
		r = this->ExecuteReader(CSTRP(buff, ptr));
		if (r)
		{
			while (r->ReadNext())
			{
				ptr = r->GetStr(0, buff, sizeof(buff));
				NEW_CLASS(col, DB::ColDef(CSTRP(buff, ptr)));
				ptr = r->GetStr(2, buff, sizeof(buff));
				col->SetNotNull(Text::StrEqualsICaseC(buff, (UOSInt)(ptr - buff), UTF8STRC("NO")));
				ptr = r->GetStr(3, buff, sizeof(buff));
				col->SetPK(Text::StrEqualsICaseC(buff, (UOSInt)(ptr - buff), UTF8STRC("PRI")));
				if ((ptr = r->GetStr(4, buff, sizeof(buff))) != 0)
				{
					col->SetDefVal(CSTRP(buff, ptr));
				}
				else
				{
					col->SetDefVal(CSTR_NULL);
				}
				if ((ptr = r->GetStr(5, buff, sizeof(buff))) != 0)
				{
					if (Text::StrEqualsC(buff, (UOSInt)(ptr - buff), UTF8STRC("auto_increment")))
					{
						col->SetAutoInc(true);
						col->SetAttr(CSTR_NULL);
					}
					else
					{
						col->SetAttr(CSTRP(buff, ptr));
					}
				}
				else
				{
					col->SetAttr(CSTR_NULL);
				}
				ptr = r->GetStr(1, buff, sizeof(buff));
				UOSInt colSize;
				col->SetNativeType(CSTRP(buff, ptr));
				col->SetColType(DB::DBUtil::ParseColType(DB::DBUtil::SQLType::MySQL, buff, &colSize));
				col->SetColSize(colSize);
				if (col->GetColType() == DB::DBUtil::CT_DateTime)
				{
					if (col->IsNotNull())
					{
						col->SetNotNull(false);
					}
				}
				tab->AddCol(col);
			}
			this->CloseReader(r);
		}
		return tab;
	}
	case DB::DBUtil::SQLType::Oracle:
	{
		return 0;
	}
	case DB::DBUtil::SQLType::MSSQL:
	{
		Int32 i = 4;
		DB::DBReader *r = 0;
		ptr = Text::StrConcatC(buff, UTF8STRC("exec sp_columns "));
		ptr = DB::DBUtil::SDBStrUTF8(ptr, tableName.v, DB::DBUtil::SQLType::MSSQL);
		if (schemaName.leng != 0)
		{
			ptr = Text::StrConcatC(ptr, UTF8STRC(", "));
			ptr = DB::DBUtil::SDBStrUTF8(ptr, schemaName.v, DB::DBUtil::SQLType::MSSQL);
		}
		while (i-- > 0 && r == 0)
		{
			r = this->ExecuteReader(CSTRP(buff, ptr));
		}
		if (r == 0)
			return 0;

		DB::TableDef *tab;
		NEW_CLASS(tab, DB::TableDef(tableName));
		tab->SetEngine(CSTR_NULL);
		tab->SetComments(0);
		tab->SetAttr(0);
		tab->SetCharset(CSTR_NULL);
		tab->SetSQLType(DB::DBUtil::SQLType::MSSQL);

		DB::ColDef *col;
		while (r->ReadNext())
		{
			ptr = r->GetStr(3, buff, sizeof(buff));
			NEW_CLASS(col, DB::ColDef(CSTRP(buff, ptr)));
			col->SetNotNull(!r->GetBool(10));
			col->SetPK(false);
			if ((ptr = r->GetStr(12, buff, sizeof(buff))) != 0)
			{
				if (*buff == '{')
				{
					ptr[-1] = 0;
					col->SetDefVal(CSTRP(&buff[1], ptr - 1));
				}
				else
				{
					col->SetDefVal(CSTRP(buff, ptr));
				}
			}
			UOSInt colSize = (UOSInt)r->GetInt32(6);
			ptr = r->GetStr(5, buff, sizeof(buff));
			if (Text::StrEndsWithC(buff, (UOSInt)(ptr - buff), UTF8STRC(" identity")))
			{
				col->SetAutoInc(true);
				ptr -= 9;
				*ptr = 0;
			}
			col->SetNativeType(CSTRP(buff, ptr));
			col->SetColType(DB::DBUtil::ParseColType(DB::DBUtil::SQLType::MSSQL, buff, &colSize));
			col->SetColSize(colSize);
			tab->AddCol(col);
		}
		this->CloseReader(r);

		Text::StringBuilderUTF8 sb;
		sb.AppendC(UTF8STRC("SELECT c.name AS column_name, i.name AS index_name, c.is_identity FROM sys.indexes i"));
		sb.AppendC(UTF8STRC(" inner join sys.index_columns ic  ON i.object_id = ic.object_id AND i.index_id = ic.index_id"));
		sb.AppendC(UTF8STRC(" inner join sys.columns c ON ic.object_id = c.object_id AND c.column_id = ic.column_id"));
		sb.AppendC(UTF8STRC(" WHERE i.is_primary_key = 1"));
		sb.AppendC(UTF8STRC(" and i.object_ID = OBJECT_ID('"));
		sb.Append(tableName);
		sb.AppendC(UTF8STRC("')"));
		r = 0;
		i = 4;
		while (i-- > 0 && r == 0)
		{
			r = this->ExecuteReader(sb.ToCString());
		}
		if (r == 0)
			return tab;

		UOSInt j;
		UOSInt k;
		while (r->ReadNext())
		{
			ptr = r->GetStr(0, buff, sizeof(buff));
			j = 0;
			k = tab->GetColCnt();
			while (j < k)
			{
				col = tab->GetCol(j);
				if (col->GetColName()->Equals(buff, (UOSInt)(ptr - buff)))
				{
					col->SetPK(true);
					break;
				}
				j++;
			}
		}
		this->CloseReader(r);

		return tab;
	}
	case DB::DBUtil::SQLType::Access:
	{
		return 0;
	}
	case DB::DBUtil::SQLType::SQLite:
	{
		DB::SQLBuilder sql(DB::DBUtil::SQLType::SQLite, this->GetTzQhr());
		sql.AppendCmdC(CSTR("select sql from sqlite_master where type='table' and name="));
		sql.AppendStrC(tableName);
		DB::DBReader *r = this->ExecuteReader(sql.ToCString());
		if (r == 0)
		{
			return 0;
		}
		Text::StringBuilderUTF8 sb;
		if (r->ReadNext())
		{
			r->GetStr(0, &sb);
		}
		this->CloseReader(r);

		DB::TableDef *tab = 0;
		DB::SQL::SQLCommand *cmd = DB::SQL::SQLCommand::Parse(sb.ToString(), DB::DBUtil::SQLType::SQLite);
		if (cmd)
		{
			if (cmd->GetCommandType() == DB::SQL::SQLCommand::CT_CREATE_TABLE)
			{
				tab = ((DB::SQL::CreateTableCommand*)cmd)->GetTableDef()->Clone();
			}
			DEL_CLASS(cmd);
		}
		return tab;
	}
	case DB::DBUtil::SQLType::PostgreSQL:
	{
		DB::SQLBuilder sql(DB::DBUtil::SQLType::PostgreSQL, this->GetTzQhr());
		sql.AppendCmdC(CSTR("select column_name, column_default, is_nullable, udt_name, character_maximum_length, datetime_precision from information_schema.columns where table_name="));
		sql.AppendStrC(tableName);
		sql.AppendCmdC(CSTR(" and table_schema = "));
		if (schemaName.leng == 0)
		{
			sql.AppendStrC(CSTR("public"));
		}
		else
		{
			sql.AppendStrC(schemaName);
		}
		sql.AppendCmdC(CSTR(" order by ordinal_position"));
		DB::DBReader *r = this->ExecuteReader(sql.ToCString());
		if (r == 0)
		{
			return 0;
		}
		Data::FastStringMap<DB::ColDef*> colMap;
		DB::TableDef *tab;
		DB::ColDef *col;
		Text::String *s;
		UOSInt sizeCol;
		NEW_CLASS(tab, DB::TableDef(tableName));
		while (r->ReadNext())
		{
			s = r->GetNewStr(0);
			NEW_CLASS(col, DB::ColDef(s));
			SDEL_STRING(s);
			s = r->GetNewStr(1);
			col->SetDefVal(s);
			SDEL_STRING(s);
			s = r->GetNewStr(2);
			col->SetNotNull((s != 0) && s->Equals(UTF8STRC("NO")));
			SDEL_STRING(s);
			s = r->GetNewStr(3);
			sizeCol = 4;
			if (s)
			{
				col->SetNativeType(s);
				//////////////////////////
				if (s->Equals(UTF8STRC("name")))
				{
					col->SetColType(DB::DBUtil::CT_VarUTF32Char);
					col->SetColSize(1024);
				}
				else if (s->Equals(UTF8STRC("timestamptz")))
				{
					col->SetColSize(6);
					col->SetColType(DB::DBUtil::CT_DateTimeTZ);
					sizeCol = 5;
				}
				else if (s->Equals(UTF8STRC("timestamp")))
				{
					col->SetColSize(6);
					col->SetColType(DB::DBUtil::CT_DateTime);
					sizeCol = 5;
				}
				else if (s->Equals(UTF8STRC("date")))
				{
					col->SetColType(DB::DBUtil::CT_Date);
					sizeCol = 5;
				}
				else if (s->Equals(UTF8STRC("char")))
				{
					col->SetColType(DB::DBUtil::CT_UTF32Char);
					col->SetColSize(1);
				}
				else if (s->Equals(UTF8STRC("bpchar")))
				{
					col->SetColType(DB::DBUtil::CT_UTF32Char);
				}
				else if (s->Equals(UTF8STRC("varchar")))
				{
					col->SetColType(DB::DBUtil::CT_VarUTF32Char);
					col->SetColSize(0x10000000);
				}
				else if (s->Equals(UTF8STRC("_char")))
				{
					col->SetColType(DB::DBUtil::CT_VarUTF32Char);
					col->SetColSize(1048576);
				}
				else if (s->Equals(UTF8STRC("bool")))
				{
					col->SetColType(DB::DBUtil::CT_Bool);
				}
				else if (s->Equals(UTF8STRC("int2")))
				{
					col->SetColType(DB::DBUtil::CT_Int16);
				}
				else if (s->Equals(UTF8STRC("int4")))
				{
					col->SetColType(DB::DBUtil::CT_Int32);
				}
				else if (s->Equals(UTF8STRC("int8")))
				{
					col->SetColType(DB::DBUtil::CT_Int64);
				}
				else if (s->Equals(UTF8STRC("smallserial")))
				{
					col->SetColType(DB::DBUtil::CT_Int16);
					col->SetAutoInc(true);
				}
				else if (s->Equals(UTF8STRC("serial")))
				{
					col->SetColType(DB::DBUtil::CT_Int32);
					col->SetAutoInc(true);
				}
				else if (s->Equals(UTF8STRC("bigserial")))
				{
					col->SetColType(DB::DBUtil::CT_Int64);
					col->SetAutoInc(true);
				}
				else if (s->Equals(UTF8STRC("text")))
				{
					col->SetColType(DB::DBUtil::CT_VarUTF32Char);
					col->SetColSize(0x10000000);
				}
				else if (s->Equals(UTF8STRC("uuid")))
				{
					col->SetColType(DB::DBUtil::CT_UUID);
				}
				else if (s->Equals(UTF8STRC("bytea")))
				{
					col->SetColType(DB::DBUtil::CT_Binary);
					col->SetColSize(1048576);
				}
				else if (s->Equals(UTF8STRC("_oid")))
				{
					col->SetColType(DB::DBUtil::CT_Binary);
					col->SetColSize(1048576);
				}
				else if (s->Equals(UTF8STRC("_int2")))
				{
					col->SetColType(DB::DBUtil::CT_Binary);
					col->SetColSize(1048576);
				}
				else if (s->Equals(UTF8STRC("oidvector")))
				{
					col->SetColType(DB::DBUtil::CT_Binary);
					col->SetColSize(1048576);
				}
				else if (s->Equals(UTF8STRC("int2vector")))
				{
					col->SetColType(DB::DBUtil::CT_Binary);
					col->SetColSize(1048576);
				}
				else if (s->Equals(UTF8STRC("jsonb"))) //////////////////////////////////
				{
					col->SetColType(DB::DBUtil::CT_VarUTF32Char);
					col->SetColSize(1048576);
				}
				else if (s->Equals(UTF8STRC("numeric")))
				{
					col->SetColType(DB::DBUtil::CT_Double);
				}
				else if (s->Equals(UTF8STRC("point")))
				{
					col->SetColType(DB::DBUtil::CT_Vector);
				}
				else if (s->Equals(UTF8STRC("float4")))
				{
					col->SetColType(DB::DBUtil::CT_Float);
				}
				else if (s->Equals(UTF8STRC("float8")))
				{
					col->SetColType(DB::DBUtil::CT_Double);
				}
				else if (s->Equals(UTF8STRC("geometry")))
				{
					col->SetColType(DB::DBUtil::CT_Vector);
				}
				else if (s->Equals(UTF8STRC("_float4")))
				{
					col->SetColType(DB::DBUtil::CT_Binary);
					col->SetColSize(1048576);
				}
				else if (s->Equals(UTF8STRC("oid")))
				{
					col->SetColType(DB::DBUtil::CT_Int32);
				}
				else if (s->Equals(UTF8STRC("xid")))
				{
					col->SetColType(DB::DBUtil::CT_Int32);
				}
				else if (s->Equals(UTF8STRC("regproc")))
				{
					col->SetColType(DB::DBUtil::CT_VarUTF32Char);
					col->SetColSize(256);
				}
				else if (s->Equals(UTF8STRC("anyarray")))
				{
					col->SetColType(DB::DBUtil::CT_VarUTF32Char);
					col->SetColSize(1048576);
				}
				else if (s->Equals(UTF8STRC("pg_node_tree")))
				{
					col->SetColType(DB::DBUtil::CT_VarUTF32Char);
					col->SetColSize(1048576);
				}
				else if (s->Equals(UTF8STRC("_aclitem")))
				{
					col->SetColType(DB::DBUtil::CT_VarUTF32Char);
					col->SetColSize(1048576);
				}
				else if (s->Equals(UTF8STRC("_text")))
				{
					col->SetColType(DB::DBUtil::CT_VarUTF32Char);
					col->SetColSize(0x10000000);
				}
				else if (s->Equals(UTF8STRC("citext")))
				{
					col->SetColType(DB::DBUtil::CT_VarUTF32Char);
					col->SetColSize(0x10000000);
				}
/*				else if (s->Equals(UTF8STRC("pg_ndistinct")))////////////////////
				{
					col->SetColType(DB::DBUtil::CT_VarUTF32Char);
					col->SetColSize(1048576);
				}
				else if (s->Equals(UTF8STRC("pg_dependencies")))////////////////////
				{
					col->SetColType(DB::DBUtil::CT_VarUTF32Char);
					col->SetColSize(1048576);
				}
				else if (s->Equals(UTF8STRC("pg_lsn")))////////////////////
				{
					col->SetColType(DB::DBUtil::CT_VarUTF32Char);
					col->SetColSize(1048576);
				}
				else if (s->Equals(UTF8STRC("pg_mcv_list")))////////////////////
				{
					col->SetColType(DB::DBUtil::CT_VarUTF32Char);
					col->SetColSize(1048576);
				}*/
				else if (s->Equals(UTF8STRC("_pg_statistic")))
				{
					col->SetColType(DB::DBUtil::CT_VarUTF32Char);
					col->SetColSize(1048576);
				}
				else
				{
					printf("ReadingDBTool.GetTableDef: PSQL Unknown type: %s\r\n", s->v);
					col->SetColType(DB::DBUtil::CT_Unknown);
				}
				s->Release();
			}
			if (!r->IsNull(sizeCol))
			{
				col->SetColSize((UOSInt)r->GetInt32(sizeCol));
			}
			tab->AddCol(col);
			colMap.Put(col->GetColName(), col);
		}
		this->CloseReader(r);

		sql.Clear();
		sql.AppendCmdC(CSTR("SELECT a.attname FROM pg_index i JOIN pg_attribute a ON a.attrelid = i.indrelid AND a.attnum = ANY(i.indkey) WHERE i.indrelid = "));
		if (schemaName.leng > 0)
		{
			Text::StringBuilderUTF8 sb;
			sb.Append(schemaName);
			sb.AppendUTF8Char('.');
			sb.Append(tableName);
			sql.AppendStrC(sb.ToCString());
		}
		else
		{
			sql.AppendStrC(tableName);
		}
		sql.AppendCmdC(CSTR("::regclass AND i.indisprimary"));
		r = this->ExecuteReader(sql.ToCString());
		if (r)
		{
			while (r->ReadNext())
			{
				ptr = r->GetStr(0, buff, 256);
				if (ptr)
				{
					col = colMap.GetC(CSTRP(buff, ptr));
					if (col)
					{
						col->SetPK(true);
					}
				}
			}
			this->CloseReader(r);
		}
		return tab;
	}
	case DB::DBUtil::SQLType::WBEM:
	case DB::DBUtil::SQLType::MDBTools:
	case DB::DBUtil::SQLType::Unknown:
	default:
	{
		return 0;
	}
	}
}

DB::DBConn::DataError DB::DBConn::GetLastDataError()
{
	return this->lastDataError;
}

Bool DB::DBConn::IsFullConn()
{
	return true;
}
