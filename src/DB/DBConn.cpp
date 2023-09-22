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

DB::DBConn::DBConn(NotNullPtr<Text::String> sourceName) : DB::ReadingDB(sourceName)
{
	this->lastDataError = DE_NO_ERROR;
}

DB::DBConn::DBConn(Text::CStringNN sourceName) : DB::ReadingDB(sourceName)
{
	this->lastDataError = DE_NO_ERROR;
}

DB::DBConn::~DBConn()
{
}

Bool DB::DBConn::IsAxisAware() const
{
	return false;
}

DB::TableDef *DB::DBConn::GetTableDef(Text::CString schemaName, Text::CString tableName)
{
	UTF8Char buff[256];
	UTF8Char *ptr;
	switch (this->GetSQLType())
	{
	case DB::SQLType::MySQL:
	{
		OSInt i = 4;
		DB::DBReader *tmpr = 0;
		NotNullPtr<DB::DBReader> r;
		ptr = Text::StrConcatC(buff, UTF8STRC("show table status where Name = "));
		ptr = DB::DBUtil::SDBStrUTF8(ptr, tableName.v, DB::SQLType::MySQL);

		while (i-- > 0 && tmpr == 0)
		{
			tmpr = this->ExecuteReader(CSTRP(buff, ptr));
		}
		if (!r.Set(tmpr))
			return 0;

		DB::TableDef *tab;

		if (r->ReadNext())
		{
			ptr = r->GetStr(0, buff, sizeof(buff));
			NEW_CLASS(tab, DB::TableDef(CSTR_NULL, CSTRP(buff, ptr)));
			ptr = r->GetStr(1, buff, sizeof(buff));
			if (ptr)
			{
				tab->SetEngine(CSTRP(buff, ptr));
			}
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
			tab->SetSQLType(DB::SQLType::MySQL);
			tab->SetCharset(CSTR_NULL);
			this->CloseReader(r);
		}
		else
		{
			this->CloseReader(r);
			return 0;
		}
		NotNullPtr<DB::ColDef> col;
		ptr = Text::StrConcatC(buff, UTF8STRC("desc "));
		ptr = DB::DBUtil::SDBColUTF8(ptr, tableName.v, DB::SQLType::MySQL);
		if (r.Set(this->ExecuteReader(CSTRP(buff, ptr))))
		{
			while (r->ReadNext())
			{
				ptr = r->GetStr(0, buff, sizeof(buff));
				NEW_CLASSNN(col, DB::ColDef(CSTRP(buff, ptr)));
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
						col->SetAutoInc(DB::ColDef::AutoIncType::Default, 1, 1);
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
				UOSInt colSize = 0;
				UOSInt colDP = 0;
				col->SetNativeType(CSTRP(buff, ptr));
				col->SetColType(DB::DBUtil::ParseColType(DB::SQLType::MySQL, buff, &colSize, &colDP));
				col->SetColSize(colSize);
				col->SetColDP(colDP);
				if (col->GetColType() == DB::DBUtil::CT_DateTime)
				{
					if (col->IsNotNull())
					{
						col->SetNotNull(false);
					}
					Text::String *defVal = col->GetDefVal();
					if (defVal && defVal->StartsWith(UTF8STRC("0000-00-00 00:00:00")))
					{
						col->SetDefVal(0);
					}
				}
				tab->AddCol(col);
			}
			this->CloseReader(r);
		}
		return tab;
	}
	case DB::SQLType::Oracle:
	{
		return 0;
	}
	case DB::SQLType::MSSQL:
	{
		Int32 i = 4;
		DB::DBReader *tmpr = 0;
		NotNullPtr<DB::DBReader> r;
		ptr = Text::StrConcatC(buff, UTF8STRC("exec sp_columns "));
		ptr = DB::DBUtil::SDBStrUTF8(ptr, tableName.v, DB::SQLType::MSSQL);
		if (schemaName.leng != 0)
		{
			ptr = Text::StrConcatC(ptr, UTF8STRC(", "));
			ptr = DB::DBUtil::SDBStrUTF8(ptr, schemaName.v, DB::SQLType::MSSQL);
		}
		while (i-- > 0 && tmpr == 0)
		{
			tmpr = this->ExecuteReader(CSTRP(buff, ptr));
		}
		if (!r.Set(tmpr))
			return 0;

		DB::TableDef *tab;
		NEW_CLASS(tab, DB::TableDef(schemaName, tableName));
		tab->SetEngine(CSTR_NULL);
		tab->SetComments(0);
		tab->SetAttr(0);
		tab->SetCharset(CSTR_NULL);
		tab->SetSQLType(DB::SQLType::MSSQL);

		NotNullPtr<DB::ColDef> col;
		while (r->ReadNext())
		{
			ptr = r->GetStr(3, buff, sizeof(buff));
			NEW_CLASSNN(col, DB::ColDef(CSTRP(buff, ptr)));
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
			UOSInt colDP = (UOSInt)r->GetInt32(8);
			ptr = r->GetStr(5, buff, sizeof(buff));
			if (Text::StrEndsWithC(buff, (UOSInt)(ptr - buff), UTF8STRC(" identity")))
			{
				col->SetAutoInc(DB::ColDef::AutoIncType::Default, 1, 1);
				ptr -= 9;
				*ptr = 0;
			}
			col->SetNativeType(CSTRP(buff, ptr));
			col->SetColType(DB::DBUtil::ParseColType(DB::SQLType::MSSQL, buff, &colSize, &colDP));
			col->SetColSize(colSize);
			col->SetColDP(colDP);
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
		tmpr = 0;
		i = 4;
		while (i-- > 0 && tmpr == 0)
		{
			tmpr = this->ExecuteReader(sb.ToCString());
		}
		if (!r.Set(tmpr))
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
				DB::ColDef *col = tab->GetCol(j);
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
	case DB::SQLType::Access:
	{
		return 0;
	}
	case DB::SQLType::SQLite:
	{
		DB::SQLBuilder sql(DB::SQLType::SQLite, false, this->GetTzQhr());
		sql.AppendCmdC(CSTR("select sql from sqlite_master where type='table' and name="));
		sql.AppendStrC(tableName);
		NotNullPtr<DB::DBReader> r;
		if (!r.Set(this->ExecuteReader(sql.ToCString())))
		{
			return 0;
		}
		Text::StringBuilderUTF8 sb;
		if (r->ReadNext())
		{
			r->GetStr(0, sb);
		}
		this->CloseReader(r);

		DB::TableDef *tab = 0;
		DB::SQL::SQLCommand *cmd = DB::SQL::SQLCommand::Parse(sb.ToString(), DB::SQLType::SQLite);
		if (cmd)
		{
			if (cmd->GetCommandType() == DB::SQL::SQLCommand::CT_CREATE_TABLE)
			{
				tab = ((DB::SQL::CreateTableCommand*)cmd)->GetTableDef()->Clone().Ptr();
			}
			DEL_CLASS(cmd);
		}
		return tab;
	}
	case DB::SQLType::PostgreSQL:
	{
		DB::SQLBuilder sql(DB::SQLType::PostgreSQL, false, this->GetTzQhr());
		sql.AppendCmdC(CSTR("select column_name, column_default, is_nullable, udt_name, character_maximum_length, datetime_precision, is_identity, identity_generation, identity_start, identity_increment, udt_schema from information_schema.columns where table_name="));
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
		NotNullPtr<DB::DBReader> r;
		if (!r.Set(this->ExecuteReader(sql.ToCString())))
		{
			return 0;
		}
		Data::FastStringMap<DB::ColDef*> colMap;
		DB::TableDef *tab;
		NotNullPtr<DB::ColDef> col;
		Bool hasGeometry = false;
		Text::String *geometrySchema = 0;
		Text::String *s;
		UOSInt sizeCol;
		NEW_CLASS(tab, DB::TableDef(schemaName, tableName));
		while (r->ReadNext())
		{
			s = r->GetNewStr(0);
			NEW_CLASSNN(col, DB::ColDef(Text::String::OrEmpty(s)));
			SDEL_STRING(s);
			s = r->GetNewStr(1);
			col->SetDefVal(s);
			SDEL_STRING(s);
			s = r->GetNewStr(2);
			col->SetNotNull((s != 0) && s->Equals(UTF8STRC("NO")));
			SDEL_STRING(s);
			s = r->GetNewStr(6);
			if (s == 0 || !s->Equals(UTF8STRC("YES")))
			{
				SDEL_STRING(s);
				col->SetAutoIncNone();
			}
			else
			{
				SDEL_STRING(s);
				s = r->GetNewStr(7);
				if (s && s->Equals(UTF8STRC("ALWAYS")))
				{
					col->SetAutoInc(DB::ColDef::AutoIncType::Always, r->GetInt64(8), r->GetInt64(9));
				}
				else
				{
					col->SetAutoInc(DB::ColDef::AutoIncType::Default, r->GetInt64(8), r->GetInt64(9));
				}
				SDEL_STRING(s);
			}
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
					col->SetAutoInc(DB::ColDef::AutoIncType::Default, 1, 1);
				}
				else if (s->Equals(UTF8STRC("serial")))
				{
					col->SetColType(DB::DBUtil::CT_Int32);
					col->SetAutoInc(DB::ColDef::AutoIncType::Default, 1, 1);
				}
				else if (s->Equals(UTF8STRC("bigserial")))
				{
					col->SetColType(DB::DBUtil::CT_Int64);
					col->SetAutoInc(DB::ColDef::AutoIncType::Default, 1, 1);
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
					col->SetColDP(0);
					col->SetColSize(0);
					hasGeometry = true;
					if (geometrySchema == 0)
					{
						geometrySchema = r->GetNewStr(10);
					}
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
			colMap.PutNN(col->GetColName(), col.Ptr());
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
		if (r.Set(this->ExecuteReader(sql.ToCString())))
		{
			while (r->ReadNext())
			{
				ptr = r->GetStr(0, buff, 256);
				if (ptr)
				{
					DB::ColDef *col = colMap.GetC(CSTRP(buff, ptr));
					if (col)
					{
						col->SetPK(true);
					}
				}
			}
			this->CloseReader(r);
		}
		if (hasGeometry)
		{
			sql.Clear();
			sql.AppendCmdC(CSTR("SELECT f_geometry_column, coord_dimension, srid, type FROM "));
			sql.AppendCol(geometrySchema->v);
			sql.AppendCmdC(CSTR(".geometry_columns where f_table_name = "));
			sql.AppendStrC(tableName);
			sql.AppendCmdC(CSTR(" and f_table_schema = "));
			if (schemaName.leng == 0)
			{
				sql.AppendStrC(CSTR("public"));
			}
			else
			{
				sql.AppendStrC(schemaName);
			}
			if (r.Set(this->ExecuteReader(sql.ToCString())))
			{
				while (r->ReadNext())
				{
					Text::String *colName = r->GetNewStr(0);
					Int32 dimension = r->GetInt32(1);
					Int32 srid = r->GetInt32(2);
					Text::String *t = r->GetNewStr(3);
					DB::ColDef *col = colMap.Get(colName);
					if (col)
					{
						col->SetColDP((UInt32)srid);
						if (t == 0 || t->Equals(UTF8STRC("GEOMETRY")))
						{
							if (dimension == 3)
								col->SetColSize((UOSInt)DB::ColDef::GeometryType::AnyZ);
							else if (dimension == 4)
								col->SetColSize((UOSInt)DB::ColDef::GeometryType::AnyZM);
							else
								col->SetColSize((UOSInt)DB::ColDef::GeometryType::Any);
						}
						else if (t->Equals(UTF8STRC("POINT")))
						{
							if (dimension == 3)
								col->SetColSize((UOSInt)DB::ColDef::GeometryType::PointZ);
							else if (dimension == 4)
								col->SetColSize((UOSInt)DB::ColDef::GeometryType::PointZM);
							else
								col->SetColSize((UOSInt)DB::ColDef::GeometryType::Point);
						}
						else if (t->Equals(UTF8STRC("PATH")))
						{
							if (dimension == 3)
								col->SetColSize((UOSInt)DB::ColDef::GeometryType::PathZ);
							else if (dimension == 4)
								col->SetColSize((UOSInt)DB::ColDef::GeometryType::PathZM);
							else
								col->SetColSize((UOSInt)DB::ColDef::GeometryType::Path);
						}
						else if (t->Equals(UTF8STRC("POLYGON")))
						{
							if (dimension == 3)
								col->SetColSize((UOSInt)DB::ColDef::GeometryType::PolygonZ);
							else if (dimension == 4)
								col->SetColSize((UOSInt)DB::ColDef::GeometryType::PolygonZM);
							else
								col->SetColSize((UOSInt)DB::ColDef::GeometryType::Polygon);
						}
						else
						{
							printf("DBConn Postgresql: Unsupported type %s\r\n", t->v);
							if (dimension == 3)
								col->SetColSize((UOSInt)DB::ColDef::GeometryType::AnyZ);
							else if (dimension == 4)
								col->SetColSize((UOSInt)DB::ColDef::GeometryType::AnyZM);
							else
								col->SetColSize((UOSInt)DB::ColDef::GeometryType::Any);
						}
					}
					SDEL_STRING(colName);
					SDEL_STRING(t);
				}
				this->CloseReader(r);
			}
			else
			{
				Text::StringBuilderUTF8 sb;
				this->GetLastErrorMsg(sb);
				printf("DBConn Postgresql: %s\r\n", sb.ToString());
			}
		}
		SDEL_STRING(geometrySchema);
		return tab;
	}
	case DB::SQLType::WBEM:
	case DB::SQLType::MDBTools:
	case DB::SQLType::Unknown:
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

Bool DB::DBConn::IsFullConn() const
{
	return true;
}
