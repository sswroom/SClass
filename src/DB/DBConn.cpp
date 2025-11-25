#include "Stdafx.h"
#include "Data/DateTime.h"
#include "Data/FastStringMap.hpp"
#include "DB/DBConn.h"
#include "DB/DBReader.h"
#include "DB/TableDef.h"
#include "DB/SQL/CreateTableCommand.h"
#include "DB/SQL/SQLCommand.h"
#include "Text/MyString.h"
#include <stdio.h>

DB::DBConn::DBConn(NN<Text::String> sourceName) : DB::ReadingDB(sourceName)
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

Optional<DB::TableDef> DB::DBConn::GetTableDef(Text::CString schemaName, Text::CStringNN tableName)
{
	UTF8Char buff[256];
	UnsafeArray<UTF8Char> ptr;
	Text::CStringNN nns;
	switch (this->GetSQLType())
	{
	case DB::SQLType::MySQL:
	{
		OSInt i = 4;
		Optional<DB::DBReader> tmpr = 0;
		NN<DB::DBReader> r;
		ptr = Text::StrConcatC(buff, UTF8STRC("show table status where Name = "));
		ptr = DB::DBUtil::SDBStrUTF8(ptr, tableName.v, DB::SQLType::MySQL);

		while (i-- > 0 && tmpr.IsNull())
		{
			tmpr = this->ExecuteReader(CSTRP(buff, ptr));
		}
		if (!tmpr.SetTo(r))
			return 0;

		DB::TableDef *tab;

		if (r->ReadNext())
		{
			ptr = r->GetStr(0, buff, sizeof(buff)).Or(buff);
			NEW_CLASS(tab, DB::TableDef(nullptr, CSTRP(buff, ptr)));
			if (r->GetStr(1, buff, sizeof(buff)).SetTo(ptr))
			{
				tab->SetEngine(CSTRP(buff, ptr));
			}
			if (r->GetStr(17, buff, sizeof(buff)).NotNull())
			{
				tab->SetComments(buff);
			}
			else
			{
				tab->SetComments(0);
			}
			if (r->GetStr(16, buff, sizeof(buff)).NotNull())
			{
				tab->SetAttr(buff);
			}
			else
			{
				tab->SetAttr(0);
			}
			tab->SetSQLType(DB::SQLType::MySQL);
			tab->SetCharset(nullptr);
			this->CloseReader(r);
		}
		else
		{
			this->CloseReader(r);
			return 0;
		}
		NN<DB::ColDef> col;
		ptr = Text::StrConcatC(buff, UTF8STRC("desc "));
		ptr = DB::DBUtil::SDBColUTF8(ptr, tableName.v, DB::SQLType::MySQL);
		if (this->ExecuteReader(CSTRP(buff, ptr)).SetTo(r))
		{
			while (r->ReadNext())
			{
				ptr = r->GetStr(0, buff, sizeof(buff)).Or(buff);
				NEW_CLASSNN(col, DB::ColDef(CSTRP(buff, ptr)));
				ptr = r->GetStr(2, buff, sizeof(buff)).Or(buff);
				col->SetNotNull(Text::StrEqualsICaseC(buff, (UOSInt)(ptr - buff), UTF8STRC("NO")));
				ptr = r->GetStr(3, buff, sizeof(buff)).Or(buff);
				col->SetPK(Text::StrEqualsICaseC(buff, (UOSInt)(ptr - buff), UTF8STRC("PRI")));
				if (r->GetStr(4, buff, sizeof(buff)).SetTo(ptr))
				{
					col->SetDefVal(CSTRP(buff, ptr));
				}
				else
				{
					col->SetDefVal(Text::CString(nullptr));
				}
				if (r->GetStr(5, buff, sizeof(buff)).SetTo(ptr))
				{
					if (Text::StrEqualsC(buff, (UOSInt)(ptr - buff), UTF8STRC("auto_increment")))
					{
						col->SetAutoInc(DB::ColDef::AutoIncType::Default, 1, 1);
						col->SetAttr(Text::CString(nullptr));
					}
					else
					{
						col->SetAttr(CSTRP(buff, ptr));
					}
				}
				else
				{
					col->SetAttr(Text::CString(nullptr));
				}
				ptr = r->GetStr(1, buff, sizeof(buff)).Or(buff);
				UOSInt colSize = 0;
				UOSInt colDP = 0;
				col->SetNativeType(CSTRP(buff, ptr));
				col->SetColType(DB::DBUtil::ParseColType(DB::SQLType::MySQL, buff, colSize, colDP));
				col->SetColSize(colSize);
				col->SetColDP(colDP);
				if (col->GetColType() == DB::DBUtil::CT_DateTime)
				{
					if (col->IsNotNull())
					{
						col->SetNotNull(false);
					}
					NN<Text::String> defVal;
					if (col->GetDefVal().SetTo(defVal) && defVal->StartsWith(UTF8STRC("0000-00-00 00:00:00")))
					{
						col->SetDefVal(Text::CString(nullptr));
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
		Optional<DB::DBReader> tmpr = 0;
		NN<DB::DBReader> r;
		ptr = Text::StrConcatC(buff, UTF8STRC("exec sp_columns "));
		ptr = DB::DBUtil::SDBStrUTF8(ptr, tableName.v, DB::SQLType::MSSQL);
		if (schemaName.leng != 0)
		{
			ptr = Text::StrConcatC(ptr, UTF8STRC(", "));
			ptr = DB::DBUtil::SDBStrUTF8(ptr, schemaName.v, DB::SQLType::MSSQL);
		}
		while (i-- > 0 && tmpr.IsNull())
		{
			tmpr = this->ExecuteReader(CSTRP(buff, ptr));
		}
		if (!tmpr.SetTo(r))
			return 0;

		DB::TableDef *tab;
		NEW_CLASS(tab, DB::TableDef(schemaName, tableName));
		tab->SetEngine(nullptr);
		tab->SetComments(0);
		tab->SetAttr(0);
		tab->SetCharset(nullptr);
		tab->SetSQLType(DB::SQLType::MSSQL);

		NN<DB::ColDef> col;
		while (r->ReadNext())
		{
			ptr = r->GetStr(3, buff, sizeof(buff)).Or(buff);
			NEW_CLASSNN(col, DB::ColDef(CSTRP(buff, ptr)));
			col->SetNotNull(!r->GetBool(10));
			col->SetPK(false);
			if (r->GetStr(12, buff, sizeof(buff)).SetTo(ptr))
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
			ptr = r->GetStr(5, buff, sizeof(buff)).Or(buff);
			if (Text::StrEndsWithC(buff, (UOSInt)(ptr - buff), UTF8STRC(" identity")))
			{
				col->SetAutoInc(DB::ColDef::AutoIncType::Default, 1, 1);
				ptr -= 9;
				*ptr = 0;
			}
			col->SetNativeType(CSTRP(buff, ptr));
			col->SetColType(DB::DBUtil::ParseColType(DB::SQLType::MSSQL, buff, colSize, colDP));
			col->SetColSize(colSize);
			col->SetColDP(colDP);
			tab->AddCol(col);
		}
		this->CloseReader(r);

		Text::StringBuilderUTF8 sb;
		sb.AppendC(UTF8STRC("SELECT c.name AS column_name, i.name AS index_name, c.is_identity, IDENT_CURRENT('"));
		sb.AppendOpt(tableName);
		sb.AppendC(UTF8STRC("'), IDENT_INCR('"));
		sb.AppendOpt(tableName);
		sb.AppendC(UTF8STRC("') FROM sys.indexes i"));
		sb.AppendC(UTF8STRC(" inner join sys.index_columns ic  ON i.object_id = ic.object_id AND i.index_id = ic.index_id"));
		sb.AppendC(UTF8STRC(" inner join sys.columns c ON ic.object_id = c.object_id AND c.column_id = ic.column_id"));
		sb.AppendC(UTF8STRC(" WHERE i.is_primary_key = 1"));
		sb.AppendC(UTF8STRC(" and i.object_ID = OBJECT_ID('"));
		sb.AppendOpt(tableName);
		sb.AppendC(UTF8STRC("')"));
		tmpr = 0;
		i = 4;
		while (i-- > 0 && tmpr.IsNull())
		{
			tmpr = this->ExecuteReader(sb.ToCString());
		}
		if (!tmpr.SetTo(r))
			return tab;

		Data::ArrayIterator<NN<DB::ColDef>> it;
		while (r->ReadNext())
		{
			ptr = r->GetStr(0, buff, sizeof(buff)).Or(buff);
			it = tab->ColIterator();
			while (it.HasNext())
			{
				NN<DB::ColDef> col = it.Next();
				if (col->GetColName()->Equals(buff, (UOSInt)(ptr - buff)))
				{
					col->SetPK(true);
					if (col->IsAutoInc())
					{
						Int64 lastIndex = 0;
						Int64 step = 1;
						if (!r->IsNull(3))
							lastIndex = r->GetInt64(3);
						if (!r->IsNull(4))
							step = r->GetInt64(4);
						col->SetAutoInc(DB::ColDef::AutoIncType::Default, lastIndex + step, step);
					}
					break;
				}
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
		NN<DB::DBReader> r;
		if (!this->ExecuteReader(sql.ToCString()).SetTo(r))
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
		NN<DB::SQL::SQLCommand> cmd;
		if (DB::SQL::SQLCommand::Parse(sb.ToString(), DB::SQLType::SQLite).SetTo(cmd))
		{
			if (cmd->GetCommandType() == DB::SQL::SQLCommand::CT_CREATE_TABLE)
			{
				tab = NN<DB::SQL::CreateTableCommand>::ConvertFrom(cmd)->GetTableDef()->Clone().Ptr();
			}
			cmd.Delete();
		}
		if (tab)
		{
			sql.Clear();
			sql.AppendCmdC(CSTR("select srs_id from gpkg_contents where table_name="));
			sql.AppendStrC(tableName);
			if (this->ExecuteReader(sql.ToCString()).SetTo(r))
			{
				if (r->ReadNext())
				{
					Int32 srid = r->GetInt32(0);
					NN<DB::ColDef> col;
					UOSInt i = tab->GetColCnt();
					while (i-- > 0)
					{
						if (tab->GetCol(i).SetTo(col))
						{
							if (col->GetColType() == DB::DBUtil::CT_Vector)
							{
								col->SetGeometrySRID((UInt32)srid);
							}
						}
					}
				}
				this->CloseReader(r);
			}

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
		NN<DB::DBReader> r;
		if (!this->ExecuteReader(sql.ToCString()).SetTo(r))
		{
			return 0;
		}
		Data::FastStringMap<DB::ColDef*> colMap;
		DB::TableDef *tab;
		NN<DB::ColDef> col;
		Bool hasGeometry = false;
		Optional<Text::String> geometrySchema = 0;
		Optional<Text::String> ops;
		NN<Text::String> s;
		UOSInt sizeCol;
		NEW_CLASS(tab, DB::TableDef(schemaName, tableName));
		while (r->ReadNext())
		{
			ops = r->GetNewStr(0);
			NEW_CLASSNN(col, DB::ColDef(Text::String::OrEmpty(ops)));
			OPTSTR_DEL(ops);
			ops = r->GetNewStr(1);
			col->SetDefVal(ops);
			OPTSTR_DEL(ops);
			ops = r->GetNewStr(2);
			col->SetNotNull(ops.SetTo(s) && s->Equals(UTF8STRC("NO")));
			OPTSTR_DEL(ops);
			ops = r->GetNewStr(6);
			if (!ops.SetTo(s) || !s->Equals(UTF8STRC("YES")))
			{
				OPTSTR_DEL(ops);
				col->SetAutoIncNone();
			}
			else
			{
				OPTSTR_DEL(ops);
				ops = r->GetNewStr(7);
				if (ops.SetTo(s) && s->Equals(UTF8STRC("ALWAYS")))
				{
					col->SetAutoInc(DB::ColDef::AutoIncType::Always, r->GetInt64(8), r->GetInt64(9));
				}
				else
				{
					col->SetAutoInc(DB::ColDef::AutoIncType::Default, r->GetInt64(8), r->GetInt64(9));
				}
				OPTSTR_DEL(ops);
			}
			sizeCol = 4;
			if (r->GetNewStr(3).SetTo(s))
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
				else if (s->Equals(UTF8STRC("bit")))
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
					if (geometrySchema.IsNull())
					{
						geometrySchema = r->GetNewStr(10);
					}
				}
				else if (s->Equals(UTF8STRC("st_geometry")))
				{
					col->SetColType(DB::DBUtil::CT_Vector);
					col->SetColDP(0);
					col->SetColSize(0);
					hasGeometry = true;
					if (geometrySchema.IsNull())
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
					printf("ReadingDBTool.GetTableDef: PSQL Unknown type: %s\r\n", s->v.Ptr());
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
		if (schemaName.SetTo(nns) && nns.leng > 0)
		{
			Text::StringBuilderUTF8 sb;
			sb.Append(nns);
			sb.AppendUTF8Char('.');
			sb.AppendOpt(tableName);
			sql.AppendStrC(sb.ToCString());
		}
		else
		{
			sql.AppendStrC(tableName);
		}
		sql.AppendCmdC(CSTR("::regclass AND i.indisprimary"));
		if (this->ExecuteReader(sql.ToCString()).SetTo(r))
		{
			while (r->ReadNext())
			{
				if (r->GetStr(0, buff, 256).SetTo(ptr))
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
		if (hasGeometry && geometrySchema.SetTo(s))
		{
			if (s->Equals(CSTR("sde")))
			{
				sql.Clear();
				sql.AppendCmdC(CSTR("SELECT f_geometry_column, coord_dimension, srid FROM "));
				sql.AppendCol(s->v);
				sql.AppendCmdC(CSTR(".sde_geometry_columns where f_table_name = "));
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
			}
			else
			{
				sql.Clear();
				sql.AppendCmdC(CSTR("SELECT f_geometry_column, coord_dimension, srid, type FROM "));
				sql.AppendCol(s->v);
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
			}
			if (this->ExecuteReader(sql.ToCString()).SetTo(r))
			{
				while (r->ReadNext())
				{
					Optional<Text::String> colName = r->GetNewStr(0);
					Int32 dimension = r->GetInt32(1);
					Int32 srid = r->GetInt32(2);
					Optional<Text::String> t = r->GetNewStr(3);
					DB::ColDef *col = 0;
					NN<Text::String> s;
					if (colName.SetTo(s))
						col = colMap.GetNN(s);
					if (col)
					{
						col->SetColDP((UInt32)srid);
						if (!t.SetTo(s) || s->Equals(UTF8STRC("GEOMETRY")))
						{
							if (dimension == 3)
								col->SetColSize((UOSInt)DB::ColDef::GeometryType::AnyZ);
							else if (dimension == 4)
								col->SetColSize((UOSInt)DB::ColDef::GeometryType::AnyZM);
							else
								col->SetColSize((UOSInt)DB::ColDef::GeometryType::Any);
						}
						else if (s->Equals(UTF8STRC("POINT")))
						{
							if (dimension == 3)
								col->SetColSize((UOSInt)DB::ColDef::GeometryType::PointZ);
							else if (dimension == 4)
								col->SetColSize((UOSInt)DB::ColDef::GeometryType::PointZM);
							else
								col->SetColSize((UOSInt)DB::ColDef::GeometryType::Point);
						}
						else if (s->Equals(UTF8STRC("PATH")))
						{
							if (dimension == 3)
								col->SetColSize((UOSInt)DB::ColDef::GeometryType::PathZ);
							else if (dimension == 4)
								col->SetColSize((UOSInt)DB::ColDef::GeometryType::PathZM);
							else
								col->SetColSize((UOSInt)DB::ColDef::GeometryType::Path);
						}
						else if (s->Equals(UTF8STRC("POLYGON")))
						{
							if (dimension == 3)
								col->SetColSize((UOSInt)DB::ColDef::GeometryType::PolygonZ);
							else if (dimension == 4)
								col->SetColSize((UOSInt)DB::ColDef::GeometryType::PolygonZM);
							else
								col->SetColSize((UOSInt)DB::ColDef::GeometryType::Polygon);
						}
						else if (s->Equals(UTF8STRC("MULTIPOLYGON")))
						{
							if (dimension == 3)
								col->SetColSize((UOSInt)DB::ColDef::GeometryType::MultiPolygonZ);
							else if (dimension == 4)
								col->SetColSize((UOSInt)DB::ColDef::GeometryType::MultiPolygonZM);
							else
								col->SetColSize((UOSInt)DB::ColDef::GeometryType::MultiPolygon);
						}
						else
						{
							printf("DBConn Postgresql: Unsupported type %s\r\n", s->v.Ptr());
							if (dimension == 3)
								col->SetColSize((UOSInt)DB::ColDef::GeometryType::AnyZ);
							else if (dimension == 4)
								col->SetColSize((UOSInt)DB::ColDef::GeometryType::AnyZM);
							else
								col->SetColSize((UOSInt)DB::ColDef::GeometryType::Any);
						}
					}
					OPTSTR_DEL(colName);
					OPTSTR_DEL(t);
				}
				this->CloseReader(r);
			}
			else
			{
				Text::StringBuilderUTF8 sb;
				this->GetLastErrorMsg(sb);
				printf("DBConn Postgresql: %s\r\n", sb.ToPtr());
			}
		}
		OPTSTR_DEL(geometrySchema);
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
