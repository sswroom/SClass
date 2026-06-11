#include "Stdafx.h"
#include "DB/ColDef.h"
#include "DB/DBMS.h"
#include "Sync/MutexUsage.h"
#include "Text/CharUtil.h"
#include "Text/MyString.h"
#include "Text/MyStringFloat.h"
#include "Text/MyStringW.h"
#include "Text/StringBuilderUTF8.h"

/*
select 1, 'abc', "abc", "abc""def", "abc\r\n\tdef", concat("abc", "def"), 1+1, 1 + concat('1', '2')
SET @ApplicationName='DBeaver 7.2.0 - Metadata'
select @ApplicationName


show engines;
Engine,Support,Comment,Transactions,XA,Savepoints
MRG_MyISAM,YES,"Collection of identical MyISAM tables",NO,NO,NO
CSV,YES,"Stores tables as CSV files",NO,NO,NO
MEMORY,YES,"Hash based, stored in memory, userful for temporary tables",NO,NO,NO
MyISAM,YES,"Non-transactional engine with good performance and small data footprint",NO,NO,NO
Aria,YES,"Crash-safe tables with MyISAM heritage",NO,NO,NO
InnoDB,DEFAULT,"Supports transactions, row level locking, foreign keys and encryption for tables",YES,YES,YES
PERFORMANCE_SCHEMA,YES,"Performance Schema",NO,NO,NO
SEQUENCE,YES,"Generated tables filed with sequential",YES,NO,YES

show charset:
Charset,Description,Default collation,Maxlen
big5,"Big5 Traditionl Chinese",big5_chinese_ci,2

show COLLATION:
Collation,Charset,Id,Default,Compiled,Sortlen
big5_chinese_ci,big5,1,Yes,Yes,1

show Variables:
Variablename,Value
alter_algorithm,DEFAULT

show Variables like '%_server':
Variablename,Value
character_set_server,utf8mb4
collation_server,utf8mb4_general_ci
*/

#define VERBOSE
#if defined(VERBOSE)
#include <stdio.h>
#endif

const Char *DB::DBMS::sysVarList[] = {
	"autocommit",
	"auto_increment_increment",
	"character_set_server",
	"collation_server",
	"lower_case_table_names",
	"max_allowed_packet",
	"sql_mode",
	"system_time_zone",
	"time_zone"
};

namespace DB
{
	class DBMSReader : public DB::DBReader
	{
	private:
		UIntOS colCount;
		IntOS rowChanged;
		Data::ArrayListArr<Optional<Text::String>> *rows;
		DB::DBUtil::ColType *colTypes;
		UnsafeArrayOpt<UnsafeArrayOpt<const UTF8Char>> colNames;
		IntOS rowIndex;
	public:
		DBMSReader(UIntOS colCount, IntOS rowChanged)
		{
			this->colCount = colCount;
			this->rowChanged = rowChanged;
			UnsafeArray<UnsafeArrayOpt<const UTF8Char>> nncolNames;
			if (this->rowChanged == -1)
			{
				NEW_CLASS(this->rows, Data::ArrayListArr<Optional<Text::String>>());
				this->colTypes = MemAlloc(DB::DBUtil::ColType, this->colCount);
				this->colNames = nncolNames = MemAllocArr(UnsafeArrayOpt<const UTF8Char>, this->colCount);
				UIntOS i;
				i = 0;
				while (i < this->colCount)
				{
					this->colTypes[i] = DB::DBUtil::CT_VarUTF8Char;
					nncolNames[i] = nullptr;
					i++;
				}
			}
			else
			{
				this->rows = 0;
				this->colTypes = 0;
				this->colNames = nullptr;
			}
			this->rowIndex = -1;
		}

		virtual ~DBMSReader()
		{
			if (this->rows)
			{
				UIntOS i;
				UIntOS j;
				i = this->rows->GetCount();
				while (i-- > 0)
				{
					UnsafeArray<Optional<Text::String>> row = this->rows->GetItemNoCheck(i);
					j = this->colCount;
					while (j-- > 0)
					{
						OPTSTR_DEL(row[j]);
					}
					MemFreeArr(row);
				}
				UnsafeArray<UnsafeArrayOpt<const UTF8Char>> nncolNames;
				if (this->colNames.SetTo(nncolNames))
				{
					i = this->colCount;
					while (i-- > 0)
					{
						SDEL_TEXT(nncolNames[i]);
					}
					MemFreeArr(nncolNames);
					this->colNames = nullptr;
				}
				MemFree(this->colTypes);
			}
		}

		void AddRow(UnsafeArray<Optional<Text::String>> row)
		{
			if (this->rows == 0)
			{
				return;
			}
			UnsafeArray<Optional<Text::String>> newRow = MemAllocArr(Optional<Text::String>, this->colCount);
			MemCopyNO(newRow.Ptr(), row.Ptr(), sizeof(Optional<Text::String>) * this->colCount);
			this->rows->Add(newRow);
		}

		void SetColumn(UIntOS colIndex, Text::CStringNN colName, DB::DBUtil::ColType colType)
		{
			UnsafeArray<UnsafeArrayOpt<const UTF8Char>> nncolNames;
			if (this->rows && this->colNames.SetTo(nncolNames) && colIndex < this->colCount)
			{
				SDEL_TEXT(nncolNames[colIndex]);
				nncolNames[colIndex] = Text::StrCopyNewC(colName.v, colName.leng).Ptr();
				this->colTypes[colIndex] = colType;
			}
		}

		virtual Bool ReadNext()
		{
			if (this->rows == 0)
			{
				return false;
			}
			if (this->rowIndex + 1 < (IntOS)this->rows->GetCount())
			{
				this->rowIndex++;
				return true;
			}
			return false;
		}

		virtual UIntOS ColCount()
		{
			return this->colCount;
		}

		virtual IntOS GetRowChanged()
		{
			return this->rowChanged;
		}

		virtual Int32 GetInt32(UIntOS colIndex)
		{
			if (this->rows == 0 || colIndex >= this->colCount)
				return 0;
			UnsafeArray<Optional<Text::String>> row;
			NN<Text::String> s;
			if (!this->rows->GetItem((UIntOS)this->rowIndex).SetTo(row) || !row[colIndex].SetTo(s))
				return 0;
			return s->ToInt32();
		}

		virtual Int64 GetInt64(UIntOS colIndex)
		{
			if (this->rows == 0 || colIndex >= this->colCount)
				return 0;
			UnsafeArray<Optional<Text::String>> row;
			NN<Text::String> s;
			if (!this->rows->GetItem((UIntOS)this->rowIndex).SetTo(row) || !row[colIndex].SetTo(s))
				return 0;
			return s->ToInt64();
		}

		virtual UnsafeArrayOpt<WChar> GetStr(UIntOS colIndex, UnsafeArray<WChar> buff)
		{
			if (this->rows == 0 || colIndex >= this->colCount)
				return nullptr;
			UnsafeArray<Optional<Text::String>> row;
			NN<Text::String> s;
			if (!this->rows->GetItem((UIntOS)this->rowIndex).SetTo(row) || !row[colIndex].SetTo(s))
				return nullptr;
			return Text::StrUTF8_WCharC(buff, s->v, s->leng, 0);
		}

		virtual Bool GetStr(UIntOS colIndex, NN<Text::StringBuilderUTF8> sb)
		{
			if (this->rows == 0 || colIndex >= this->colCount)
				return false;
			UnsafeArray<Optional<Text::String>> row;
			NN<Text::String> s;
			if (!this->rows->GetItem((UIntOS)this->rowIndex).SetTo(row) || !row[colIndex].SetTo(s))
				return false;
			sb->Append(s);
			return true;
		}

		virtual Optional<Text::String> GetNewStr(UIntOS colIndex)
		{
			if (this->rows == 0 || colIndex >= this->colCount)
				return nullptr;
			UnsafeArray<Optional<Text::String>> row;
			NN<Text::String> s;
			if (!this->rows->GetItem((UIntOS)this->rowIndex).SetTo(row) || !row[colIndex].SetTo(s))
				return nullptr;
			return s->Clone();
		}

		virtual UnsafeArrayOpt<UTF8Char> GetStr(UIntOS colIndex, UnsafeArray<UTF8Char> buff, UIntOS buffSize)
		{
			if (this->rows == 0 || colIndex >= this->colCount)
				return nullptr;
			UnsafeArray<Optional<Text::String>> row;
			NN<Text::String> s;
			if (!this->rows->GetItem((UIntOS)this->rowIndex).SetTo(row) || !row[colIndex].SetTo(s))
				return nullptr;
			return Text::StrConcatS(buff, s->v, buffSize);
		}

		virtual Data::Timestamp GetTimestamp(UIntOS colIndex)
		{
			if (this->rows == 0 || colIndex >= this->colCount)
				return Data::Timestamp(nullptr);
			UnsafeArray<Optional<Text::String>> row;
			NN<Text::String> s;
			if (!this->rows->GetItem((UIntOS)this->rowIndex).SetTo(row))
				 return Data::Timestamp(nullptr);
			if (!row[colIndex].SetTo(s))
				return Data::Timestamp(nullptr);
			return Data::Timestamp(s->ToCString(), 0);
		}

		virtual Double GetDblOrNAN(UIntOS colIndex)
		{
			if (this->rows == 0 || colIndex >= this->colCount)
				return NAN;
			UnsafeArray<Optional<Text::String>> row;
			NN<Text::String> s;
			if (!this->rows->GetItem((UIntOS)this->rowIndex).SetTo(row) || !row[colIndex].SetTo(s))
				return NAN;
			return s->ToDoubleOrNAN();
		}

		virtual Bool GetBool(UIntOS colIndex)
		{
			if (this->rows == 0 || colIndex >= this->colCount)
				return false;
			UnsafeArray<Optional<Text::String>> row;
			NN<Text::String> s;
			if (!this->rows->GetItem((UIntOS)this->rowIndex).SetTo(row) || !row[colIndex].SetTo(s))
				return false;
			return s->ToInt32() != 0;
		}

		virtual UIntOS GetBinarySize(UIntOS colIndex)
		{
			if (this->rows == 0 || colIndex >= this->colCount)
				return 0;
			UnsafeArray<Optional<Text::String>> row;
			NN<Text::String> s;
			if (!this->rows->GetItem((UIntOS)this->rowIndex).SetTo(row) || !row[colIndex].SetTo(s))
				return 0;
			return s->leng;
		}

		virtual UIntOS GetBinary(UIntOS colIndex, UnsafeArray<UInt8> buff)
		{
			if (this->rows == 0 || colIndex >= this->colCount)
				return 0;
			UnsafeArray<Optional<Text::String>> row;
			NN<Text::String> s;
			if (!this->rows->GetItem((UIntOS)this->rowIndex).SetTo(row) || !row[colIndex].SetTo(s))
				return 0;
			UIntOS cnt = s->leng;
			MemCopyNO(buff.Ptr(), s->v.Ptr(), cnt);
			return cnt;
		}

		virtual Optional<Math::Geometry::Vector2D> GetVector(UIntOS colIndex)
		{
			return nullptr;
		}

		virtual Bool GetUUID(UIntOS colIndex, NN<Data::UUID> uuid)
		{
			return false;
		}

		virtual Bool IsNull(UIntOS colIndex)
		{
			if (this->rows == 0 || colIndex >= this->colCount)
				return true;
			UnsafeArray<Optional<Text::String>> row;
			NN<Text::String> s;
			if (!this->rows->GetItem((UIntOS)this->rowIndex).SetTo(row) || !row[colIndex].SetTo(s))
				return true;
			return false;
		}

		virtual UnsafeArrayOpt<UTF8Char> GetName(UIntOS colIndex, UnsafeArray<UTF8Char> buff)
		{
			if (this->rowChanged != -1)
				return nullptr;
			if (colIndex >= this->colCount)
				return nullptr;
			UnsafeArray<UnsafeArrayOpt<const UTF8Char>> nncolNames;
			UnsafeArray<const UTF8Char> nns;
			if (this->colNames.SetTo(nncolNames) && nncolNames[colIndex].SetTo(nns))
			{
				return Text::StrConcat(buff, nns);
			}
			else
			{
				return Text::StrUIntOS(Text::StrConcatC(buff, UTF8STRC("column")), colIndex + 1);
			}

		}

		virtual DB::DBUtil::ColType GetColType(UIntOS colIndex, OptOut<UIntOS> colSize)
		{
			if (this->rowChanged != -1)
				return DB::DBUtil::CT_Unknown;
			if (colIndex >= this->colCount)
				return DB::DBUtil::CT_Unknown;
			colSize.Set(256);
			return this->colTypes[colIndex];
		}

		virtual Bool GetColDef(UIntOS colIndex, NN<DB::ColDef> colDef)
		{
			UTF8Char sbuff[256];
			UnsafeArray<UTF8Char> sptr;
			UnsafeArray<UnsafeArrayOpt<const UTF8Char>> nncolNames;
			UnsafeArray<const UTF8Char> nns;
			if (this->rowChanged != -1)
				return false;
			if (colIndex >= this->colCount)
				return false;
			if (this->colNames.SetTo(nncolNames) && nncolNames[colIndex].SetTo(nns))
			{
				colDef->SetColName(nns);
			}
			else
			{
				sptr = Text::StrUIntOS(Text::StrConcatC(sbuff, UTF8STRC("column")), colIndex + 1);
				colDef->SetColName(CSTRP(sbuff, sptr));
			}
			colDef->SetColType(this->colTypes[colIndex]);
			colDef->SetColSize(256);
			return true;
		}

		virtual void DelNewStr(UnsafeArray<const UTF8Char> s)
		{
			Text::StrDelNew(s);
		}
	};
}

UnsafeArrayOpt<const UTF8Char> DB::DBMS::SQLParseName(UnsafeArray<UTF8Char> nameBuff, UnsafeArray<const UTF8Char> sql)
{
	Int32 varType = 0;
	Bool isSep = true;
	if (sql[0] >= '0' && sql[0] <= '9')
	{
		Bool hasDot = false;
		*nameBuff++ = *sql++;
		while (true)
		{
			if (sql[0] == '.')
			{
				if (hasDot)
					return nullptr;
				hasDot = true;
				*nameBuff++ = '.';
			}
			else if (sql[0] >= '0' && sql[0] <= '9')
			{
				*nameBuff++ = *sql++;
			}
			else if (sql[0] == 0 || sql[0] == ',')
			{
				*nameBuff = 0;
				return sql;
			}
			else
			{
				UnsafeArray<const UTF8Char> sptr = sql;
				while (Text::CharUtil::PtrIsWS(sptr));
				if (sptr[0] == '+' || sptr[0] == '-' || sptr[0] == '*' || sptr[0] == '/')
				{
					sptr++;
					nameBuff = Text::StrConcatC(nameBuff, sql, (UIntOS)(sptr - sql));
					sql = sptr;

					while (Text::CharUtil::PtrIsWS(sptr));
					if (sptr > sql)
					{
						nameBuff = Text::StrConcatC(nameBuff, sql, (UIntOS)(sptr - sql));
						sql = sptr;
					}
					return SQLParseName(nameBuff, sql);
				}
				else
				{
					return nullptr;
				}
			}
		}
	}
	else if (sql[0] == '\'')
	{
		UTF8Char c;
		*nameBuff++ = *sql++;
		while (true)
		{
			c = *sql++;
			if (c == '\'')
			{
				*nameBuff++ = c;
				c = *sql;
				if (c == '\'')
				{
					*nameBuff++ = c;
					sql++;
				}
				else if (c == ' ' || c == 0 || c == '\t' || c == '\r' || c == '\n' || c == ',')
				{
					*nameBuff = 0;
					return sql;
				}
				else
				{
					*nameBuff = 0;
					return sql;
				}
			}
			else if (c == '\\')
			{
				*nameBuff++ = c;
				c = *sql;
				if (c == 0)
				{
					*nameBuff = 0;
					return sql;
				}
				else
				{
					*nameBuff++ = c;
					sql++;
				}
			}
			else if (c == 0)
			{
				return nullptr;
			}
			else
			{
				*nameBuff++ = c;
			}
		}
	}
	else if (sql[0] == '"')
	{
		UTF8Char c;
		*nameBuff++ = *sql++;
		while (true)
		{
			c = *sql++;
			if (c == '\"')
			{
				*nameBuff++ = c;
				c = *sql;
				if (c == '\"')
				{
					*nameBuff++ = c;
					sql++;
				}
				else if (c == ' ' || c == 0 || c == '\t' || c == '\r' || c == '\n' || c == ',')
				{
					*nameBuff = 0;
					return sql;
				}
				else
				{
					*nameBuff = 0;
					return sql;
				}
			}
			else if (c == '\\')
			{
				*nameBuff++ = c;
				c = *sql;
				if (c == 0)
				{
					*nameBuff = 0;
					return sql;
				}
				else
				{
					*nameBuff++ = c;
					sql++;
				}
			}
			else if (c == 0)
			{
				return nullptr;
			}
			else
			{
				*nameBuff++ = c;
			}
		}
	}
	else if (sql[0] == '@' && sql[1] == '@')
	{
		nameBuff[0] = '@';
		nameBuff[1] = '@';
		nameBuff += 2;
		sql += 2;
		varType = 2;
	}
	else if (sql[0] == '@')
	{
		nameBuff[0] = '@';
		nameBuff += 1;
		sql += 1;
		varType = 1;
		isSep = false;
	}
	while (true)
	{
		UTF8Char c = *sql++;
		if (c == '.')
		{
			if (varType == 1 || isSep)
			{
				return nullptr;
			}
			*nameBuff++ = c;
			isSep = true;
		}
		else if ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || c == '_')
		{
			*nameBuff++ = c;
			isSep = false;
		}
		else if (c >= '0' && c <= '9')
		{
			if (isSep)
			{
				return nullptr;
			}
			*nameBuff++ = c;
		}
		else if (c == '*')
		{
			if (!isSep)
			{
				return nullptr;
			}
			*nameBuff++ = c;
			c = *sql++;
			if (c == ' ' || c == 0 || c == '\t' || c == '\r' || c == '\n' || c == ',')
			{
				*nameBuff = 0;
				return sql - 1;
			}
			else
			{
				return nullptr;
			}
		}
		else if (c == 0 || c == ',')
		{
			if (isSep)
			{
				return nullptr;
			}
			else
			{
				*nameBuff = 0;
				return sql - 1;
			}
		}
		else if (c == ' '  || c == '\t' || c == '\r' || c == '\n' || c == '(')
		{
			sql--;
			UnsafeArray<const UTF8Char> sptr = sql;
			while (Text::CharUtil::PtrIsWS(sptr));
			if (sptr[0] == '(')
			{
				sptr++;
				MemCopyNO(nameBuff.Ptr(), sql.Ptr(), (UIntOS)(sptr - sql));
				nameBuff += sptr - sql;
				sql = sptr;
				if (sql[0] == ')')
				{
					*nameBuff++ = ')';
					*nameBuff = 0;
					return sql + 1;
				}
				while (true)
				{
					sptr = sql;
					if (!SQLParseName(nameBuff, sql).SetTo(sql))
						return nullptr;
					while (Text::CharUtil::PtrIsWS(sql));
					MemCopyNO(nameBuff.Ptr(), sptr.Ptr(), (UIntOS)(sql - sptr));
					nameBuff += sql - sptr;
					if (sql[0] == ')')
					{
						*nameBuff++ = ')';
						*nameBuff = 0;
						return sql + 1;
					}
					else if (sql[0] == ',')
					{
						*nameBuff++ = ',';
						sql++;
						sptr = sql;
						while (Text::CharUtil::PtrIsWS(sql));
						if (sptr != sql)
						{
							MemCopyNO(nameBuff.Ptr(), sptr.Ptr(), (UIntOS)(sql - sptr));
							nameBuff += sql - sptr;
						}
					}
					else
					{
						return nullptr;
					}
				}
				
			}
			else
			{
				*nameBuff = 0;
				return sql;
			}
		}
		else
		{
			return nullptr;
		}
	}
}

Bool DB::DBMS::StrLike(UnsafeArray<const UTF8Char> val, UnsafeArray<const UTF8Char> likeStr)
{
	if (Text::StrEquals(val, likeStr))
	{
		return true;
	}
	while (true)
	{
		if (likeStr[0] == '_')
		{
			if (val[0] == 0)
			{
				return false;
			}
			likeStr++;
			val++;
		}
		else if (likeStr[0] == '%')
		{
			if (likeStr[1] == 0)
			{
				return true;
			}

			likeStr++;
			while (true)
			{
				if (val[0] == 0)
				{
					return false;
				}
				else if (StrLike(val, likeStr))
				{
					return true;
				}
				val++;
			}
		}
		else if (val[0] != likeStr[0])
		{
			return false;
		}
		else if (val[0] == 0)
		{
			return true;
		}
		else
		{
			likeStr++;
			val++;
		}
		
	}
}

Bool DB::DBMS::SysVarExist(NN<DB::DBMS::SessionInfo> sess, Text::CStringNN varName, AccessType atype)
{
#if defined(VERBOSE)
	printf("SysVarExist: %s\r\n", varName.v.Ptr());
#endif
	IntOS i = 0;
	IntOS j = sizeof(sysVarList) / sizeof(sysVarList[0]) - 1;
	IntOS k;
	IntOS l;
	while (i <= j)
	{
		k = (i + j) >> 1;
		l = Text::StrCompareICase((const UTF8Char*)sysVarList[k], varName.v);
		if (l > 0)
		{
			j = k - 1;
		}
		else if (l < 0)
		{
			i = k + 1;
		}
		else
		{
			return true;
		}
	}
	return false;
}

Bool DB::DBMS::SysVarGet(NN<Text::StringBuilderUTF8> sb, NN<DB::DBMS::SessionInfo> sess, Text::CStringNN varName)
{
//	Bool isGlobal = false;
	if (varName.EqualsICase(UTF8STRC("GLOBAL.")))
	{
		varName = varName.Substring(7);
//		isGlobal = true;
	}

	if (varName.EqualsICase(UTF8STRC("autocommit")))
	{
		sb->AppendChar(sess->autoCommit?'1':'0', 1);
		return true;
	}
	else if (varName.EqualsICase(UTF8STRC("auto_increment_increment")))
	{
		sb->AppendI32(sess->autoIncInc);
		return true;
	}
	else if (varName.EqualsICase(UTF8STRC("character_set_server")))
	{
		sb->AppendC(UTF8STRC("utf8mb4"));
		return true;
	}
	else if (varName.EqualsICase(UTF8STRC("collation_server")))
	{
		sb->AppendC(UTF8STRC("utf8mb4_0900_ai_ci"));
		return true;
	}
	else if (varName.EqualsICase(UTF8STRC("lower_case_table_names")))
	{
		sb->AppendI32(1);
		return true;
	}
	else if (varName.EqualsICase(UTF8STRC("max_allowed_packet")))
	{
		sb->AppendU32(sess->params.clientMaxPacketSize);
		return true;
	}
	else if (varName.EqualsICase(UTF8STRC("sql_mode")))
	{
		Bool found = false;
		if (sess->sqlModes & SQLM_ALLOW_INVALID_DATES)
		{
			if (found) sb->AppendC(UTF8STRC(", "));
			found = true;
			sb->AppendC(UTF8STRC("ALLOW_INVALID_DATES"));
		}
		if (sess->sqlModes & SQLM_ANSI_QUOTES)
		{
			if (found) sb->AppendC(UTF8STRC(", "));
			found = true;
			sb->AppendC(UTF8STRC("ANSI_QUOTES"));
		}
		if (sess->sqlModes & SQLM_ERROR_FOR_DIVISION_BY_ZERO)
		{
			if (found) sb->AppendC(UTF8STRC(", "));
			found = true;
			sb->AppendC(UTF8STRC("ERROR_FOR_DIVISION_BY_ZERO"));
		}
		if (sess->sqlModes & SQLM_HIGH_NOT_PRECEDENCE)
		{
			if (found) sb->AppendC(UTF8STRC(", "));
			found = true;
			sb->AppendC(UTF8STRC("HIGH_NOT_PRECEDENCE"));
		}
		if (sess->sqlModes & SQLM_IGNORE_SPACE)
		{
			if (found) sb->AppendC(UTF8STRC(", "));
			found = true;
			sb->AppendC(UTF8STRC("IGNORE_SPACE"));
		}
		if (sess->sqlModes & SQLM_NO_AUTO_VALUE_ON_ZERO)
		{
			if (found) sb->AppendC(UTF8STRC(", "));
			found = true;
			sb->AppendC(UTF8STRC("NO_AUTO_VALUE_ON_ZERO"));
		}
		if (sess->sqlModes & SQLM_NO_BACKSLASH_ESCAPES)
		{
			if (found) sb->AppendC(UTF8STRC(", "));
			found = true;
			sb->AppendC(UTF8STRC("NO_BACKSLASH_ESCAPES"));
		}
		if (sess->sqlModes & SQLM_NO_DIR_IN_CREATE)
		{
			if (found) sb->AppendC(UTF8STRC(", "));
			found = true;
			sb->AppendC(UTF8STRC("NO_DIR_IN_CREATE"));
		}
		if (sess->sqlModes & SQLM_NO_ENGINE_SUBSTITUTION)
		{
			if (found) sb->AppendC(UTF8STRC(", "));
			found = true;
			sb->AppendC(UTF8STRC("NO_ENGINE_SUBSTITUTION"));
		}
		if (sess->sqlModes & SQLM_NO_UNSIGNED_SUBTRACTION)
		{
			if (found) sb->AppendC(UTF8STRC(", "));
			found = true;
			sb->AppendC(UTF8STRC("NO_UNSIGNED_SUBTRACTION"));
		}
		if (sess->sqlModes & SQLM_NO_ZERO_DATE)
		{
			if (found) sb->AppendC(UTF8STRC(", "));
			found = true;
			sb->AppendC(UTF8STRC("NO_ZERO_DATE"));
		}
		if (sess->sqlModes & SQLM_NO_ZERO_IN_DATE)
		{
			if (found) sb->AppendC(UTF8STRC(", "));
			found = true;
			sb->AppendC(UTF8STRC("NO_ZERO_IN_DATE"));
		}
		if (sess->sqlModes & SQLM_ONLY_FULL_GROUP_BY)
		{
			if (found) sb->AppendC(UTF8STRC(", "));
			found = true;
			sb->AppendC(UTF8STRC("ONLY_FULL_GROUP_BY"));
		}
		if (sess->sqlModes & SQLM_PAD_CHAR_TO_FULL_LENGTH)
		{
			if (found) sb->AppendC(UTF8STRC(", "));
			found = true;
			sb->AppendC(UTF8STRC("PAD_CHAR_TO_FULL_LENGTH"));
		}
		if (sess->sqlModes & SQLM_PIPES_AS_CONCAT)
		{
			if (found) sb->AppendC(UTF8STRC(", "));
			found = true;
			sb->AppendC(UTF8STRC("PIPES_AS_CONCAT"));
		}
		if (sess->sqlModes & SQLM_REAL_AS_FLOAT)
		{
			if (found) sb->AppendC(UTF8STRC(", "));
			found = true;
			sb->AppendC(UTF8STRC("REAL_AS_FLOAT"));
		}
		if (sess->sqlModes & SQLM_STRICT_ALL_TABLES)
		{
			if (found) sb->AppendC(UTF8STRC(", "));
			found = true;
			sb->AppendC(UTF8STRC("STRICT_ALL_TABLES"));
		}
		if (sess->sqlModes & SQLM_STRICT_TRANS_TABLES)
		{
			if (found) sb->AppendC(UTF8STRC(", "));
			found = true;
			sb->AppendC(UTF8STRC("STRICT_TRANS_TABLES"));
		}
		if (sess->sqlModes & SQLM_TIME_TRUNCATE_FRACTIONAL)
		{
			if (found) sb->AppendC(UTF8STRC(", "));
			found = true;
			sb->AppendC(UTF8STRC("TIME_TRUNCATE_FRACTIONAL"));
		}
		return true;
	}
	else if (varName.EqualsICase(UTF8STRC("system_time_zone")) || varName.EqualsICase(UTF8STRC("time_zone")))
	{
		Data::DateTime dt;
		dt.SetCurrTime();
		Int32 tz = dt.GetTimeZoneQHR();
		if (tz > 0)
		{
			sb->AppendUTF8Char('+');
		}
		else
		{
			tz = -tz;
			sb->AppendUTF8Char('-');
		}
		sb->AppendI32(tz >> 2);
		sb->AppendUTF8Char(':');
		if (tz & 3)
		{
			sb->AppendI32((tz & 3) * 15);
		}
		else
		{
			sb->AppendC(UTF8STRC("00"));
		}
		return true;
	}
	return false;
}

void DB::DBMS::SysVarColumn(DB::DBMSReader *reader, UIntOS colIndex, UnsafeArray<const UTF8Char> varName, Text::CString colName)
{
	UTF8Char sbuff[128];
	UnsafeArray<UTF8Char> sptr;
	Text::CStringNN nncolName;
	if (!colName.SetTo(nncolName) || nncolName.leng == 0)
	{
		sptr = Text::StrConcat(Text::StrConcatC(sbuff, UTF8STRC("@@")), varName);
		nncolName = CSTRP(sbuff, sptr);
	}
	UIntOS varNameLen = Text::StrCharCnt(varName);
	
	if (Text::StrEqualsICaseC(varName, varNameLen, UTF8STRC("autocommit")))
	{
		reader->SetColumn(colIndex, nncolName, DB::DBUtil::CT_Bool);
		return;
	}
	else if (Text::StrEqualsICaseC(varName, varNameLen, UTF8STRC("auto_increment_increment")))
	{
		reader->SetColumn(colIndex, nncolName, DB::DBUtil::CT_Int32);
		return;
	}
	else if (Text::StrEqualsICaseC(varName, varNameLen, UTF8STRC("character_set_server")))
	{
		reader->SetColumn(colIndex, nncolName, DB::DBUtil::CT_VarUTF8Char);
		return;
	}
	else if (Text::StrEqualsICaseC(varName, varNameLen, UTF8STRC("collation_server")))
	{
		reader->SetColumn(colIndex, nncolName, DB::DBUtil::CT_VarUTF8Char);
		return;
	}
	else if (Text::StrEqualsICaseC(varName, varNameLen, UTF8STRC("lower_case_table_names")))
	{
		reader->SetColumn(colIndex, nncolName, DB::DBUtil::CT_Int32);
		return;
	}
	else if (Text::StrEqualsICaseC(varName, varNameLen, UTF8STRC("max_allowed_packet")))
	{
		reader->SetColumn(colIndex, nncolName, DB::DBUtil::CT_Int32);
		return;
	}
	else if (Text::StrEqualsICaseC(varName, varNameLen, UTF8STRC("sql_mode")))
	{
		reader->SetColumn(colIndex, nncolName, DB::DBUtil::CT_VarUTF8Char);
		return;
	}
	else if (Text::StrEqualsICaseC(varName, varNameLen, UTF8STRC("system_time_zone")))
	{
		reader->SetColumn(colIndex, nncolName, DB::DBUtil::CT_VarUTF8Char);
		return;
	}
	else if (Text::StrEqualsICaseC(varName, varNameLen, UTF8STRC("time_zone")))
	{
		reader->SetColumn(colIndex, nncolName, DB::DBUtil::CT_VarUTF8Char);
		return;
	}
	reader->SetColumn(colIndex, nncolName, DB::DBUtil::CT_VarUTF8Char);
}

Bool DB::DBMS::SysVarSet(NN<DB::DBMS::SessionInfo> sess, Bool isGlobal, Text::CStringNN varName, Optional<Text::String> val)
{
	NN<Text::String> nnval;
	if (varName.EqualsICase(UTF8STRC("autocommit")))
	{
		Bool v;
		if (!val.SetTo(nnval))
		{
			v = false;
		}
		else
		{
			v = nnval->ToInt32() != 0;
		}
		if (isGlobal)
		{
			//this->autoCommit = v;
		}
		else
		{
			sess->autoCommit = v;
		}
		return true;
	}
	else if (varName.EqualsICase(UTF8STRC("auto_increment_incremnt")))
	{
		Int32 v;
		if (!val.SetTo(nnval))
		{
			v = 1;
		}
		else
		{
			v = nnval->ToInt32();
		}
		if (v <= 0)
		{
			v = 1;
		}
		else if (v > 65535)
		{
			v = 65535;
		}

		if (isGlobal)
		{
			//this->autoIncInc = v;
		}
		else
		{
			sess->autoIncInc = v;
		}
		return true;
	}
	else if (varName.EqualsICase(UTF8STRC("character_set_server")))
	{
	}
	else if (varName.EqualsICase(UTF8STRC("collation_server")))
	{
	}
	else if (varName.EqualsICase(UTF8STRC("lower_case_table_names")))
	{
	}
	else if (varName.EqualsICase(UTF8STRC("max_allowed_packet")))
	{
	}
	else if (varName.EqualsICase(UTF8STRC("sql_mode")))
	{
		SQLMODE sqlMode = (SQLMODE)0;
		Text::StringBuilderUTF8 sb;
		Text::PString sarr[2];
		UIntOS i;
		sb.AppendOpt(val);
		sarr[1] = sb;
		i = 2;
		while (i == 2)
		{
			i = Text::StrSplitTrimP(sarr, 2, sarr[1], ',');
			if (Text::StrEqualsICaseC(sarr[0].v, sarr[0].leng, UTF8STRC("ALLOW_INVALID_DATES")))
			{
				sqlMode = (SQLMODE)(sqlMode | SQLM_ALLOW_INVALID_DATES);
			}
			if (Text::StrEqualsICaseC(sarr[0].v, sarr[0].leng, UTF8STRC("ANSI_QUOTES")))
			{
				sqlMode = (SQLMODE)(sqlMode | SQLM_ANSI_QUOTES);
			}
			if (Text::StrEqualsICaseC(sarr[0].v, sarr[0].leng, UTF8STRC("ERROR_FOR_DIVISION_BY_ZERO")))
			{
				sqlMode = (SQLMODE)(sqlMode | SQLM_ERROR_FOR_DIVISION_BY_ZERO);
			}
			if (Text::StrEqualsICaseC(sarr[0].v, sarr[0].leng, UTF8STRC("HIGH_NOT_PRECEDENCE")))
			{
				sqlMode = (SQLMODE)(sqlMode | SQLM_HIGH_NOT_PRECEDENCE);
			}
			if (Text::StrEqualsICaseC(sarr[0].v, sarr[0].leng, UTF8STRC("IGNORE_SPACE")))
			{
				sqlMode = (SQLMODE)(sqlMode | SQLM_IGNORE_SPACE);
			}
			if (Text::StrEqualsICaseC(sarr[0].v, sarr[0].leng, UTF8STRC("NO_AUTO_VALUE_ON_ZERO")))
			{
				sqlMode = (SQLMODE)(sqlMode | SQLM_NO_AUTO_VALUE_ON_ZERO);
			}
			if (Text::StrEqualsICaseC(sarr[0].v, sarr[0].leng, UTF8STRC("NO_BACKSLASH_ESCAPES")))
			{
				sqlMode = (SQLMODE)(sqlMode | SQLM_NO_BACKSLASH_ESCAPES);
			}
			if (Text::StrEqualsICaseC(sarr[0].v, sarr[0].leng, UTF8STRC("NO_DIR_IN_CREATE")))
			{
				sqlMode = (SQLMODE)(sqlMode | SQLM_NO_DIR_IN_CREATE);
			}
			if (Text::StrEqualsICaseC(sarr[0].v, sarr[0].leng, UTF8STRC("NO_ENGINE_SUBSTITUTION")))
			{
				sqlMode = (SQLMODE)(sqlMode | SQLM_NO_ENGINE_SUBSTITUTION);
			}
			if (Text::StrEqualsICaseC(sarr[0].v, sarr[0].leng, UTF8STRC("NO_UNSIGNED_SUBTRACTION")))
			{
				sqlMode = (SQLMODE)(sqlMode | SQLM_NO_UNSIGNED_SUBTRACTION);
			}
			if (Text::StrEqualsICaseC(sarr[0].v, sarr[0].leng, UTF8STRC("NO_ZERO_DATE")))
			{
				sqlMode = (SQLMODE)(sqlMode | SQLM_NO_ZERO_DATE);
			}
			if (Text::StrEqualsICaseC(sarr[0].v, sarr[0].leng, UTF8STRC("NO_ZERO_IN_DATE")))
			{
				sqlMode = (SQLMODE)(sqlMode | SQLM_NO_ZERO_IN_DATE);
			}
			if (Text::StrEqualsICaseC(sarr[0].v, sarr[0].leng, UTF8STRC("ONLY_FULL_GROUP_BY")))
			{
				sqlMode = (SQLMODE)(sqlMode | SQLM_ONLY_FULL_GROUP_BY);
			}
			if (Text::StrEqualsICaseC(sarr[0].v, sarr[0].leng, UTF8STRC("PAD_CHAR_TO_FULL_LENGTH")))
			{
				sqlMode = (SQLMODE)(sqlMode | SQLM_PAD_CHAR_TO_FULL_LENGTH);
			}
			if (Text::StrEqualsICaseC(sarr[0].v, sarr[0].leng, UTF8STRC("PIPES_AS_CONCAT")))
			{
				sqlMode = (SQLMODE)(sqlMode | SQLM_PIPES_AS_CONCAT);
			}
			if (Text::StrEqualsICaseC(sarr[0].v, sarr[0].leng, UTF8STRC("REAL_AS_FLOAT")))
			{
				sqlMode = (SQLMODE)(sqlMode | SQLM_REAL_AS_FLOAT);
			}
			if (Text::StrEqualsICaseC(sarr[0].v, sarr[0].leng, UTF8STRC("STRICT_ALL_TABLES")))
			{
				sqlMode = (SQLMODE)(sqlMode | SQLM_STRICT_ALL_TABLES);
			}
			if (Text::StrEqualsICaseC(sarr[0].v, sarr[0].leng, UTF8STRC("STRICT_TRANS_TABLES")))
			{
				sqlMode = (SQLMODE)(sqlMode | SQLM_STRICT_TRANS_TABLES);
			}
			if (Text::StrEqualsICaseC(sarr[0].v, sarr[0].leng, UTF8STRC("TIME_TRUNCATE_FRACTIONAL")))
			{
				sqlMode = (SQLMODE)(sqlMode | SQLM_TIME_TRUNCATE_FRACTIONAL);
			}
		}
		sess->sqlModes = sqlMode;
	}
	else if (varName.EqualsICase(UTF8STRC("system_time_zone")))
	{
	}
	else if (varName.EqualsICase(UTF8STRC("time_zone")))
	{
	}
	return false;

}

Bool DB::DBMS::UserVarGet(NN<Text::StringBuilderUTF8> sb, NN<DB::DBMS::SessionInfo> sess, Text::CStringNN varName)
{
	NN<Text::String> val;
	if (sess->userVars.GetC(varName).SetTo(val))
	{
		sb->Append(val);
		return true;
	}
	return false;
}

void DB::DBMS::UserVarColumn(DB::DBMSReader *reader, UIntOS colIndex, UnsafeArray<const UTF8Char> varName, Text::CString colName)
{
	UTF8Char sbuff[128];
	UnsafeArray<UTF8Char> sptr;
	Text::CStringNN nncolName;
	if (!colName.SetTo(nncolName) || nncolName.leng == 0)
	{
		sptr = Text::StrConcat(Text::StrConcatC(sbuff, UTF8STRC("@")), varName);
		nncolName = CSTRP(sbuff, sptr);
	}

	reader->SetColumn(colIndex, nncolName, DB::DBUtil::CT_VarUTF8Char);
}

Bool DB::DBMS::UserVarSet(NN<DB::DBMS::SessionInfo> sess, Text::CStringNN varName, Optional<Text::String> val)
{
	NN<Text::String> nnval;
	if (val.SetTo(nnval))
	{
		if (sess->userVars.PutC(varName, nnval->Clone()).SetTo(nnval))
			nnval->Release();
	}
	else
	{
		if (sess->userVars.RemoveC(varName).SetTo(nnval))
			nnval->Release();
	}
	return true;
}

Optional<Text::String> DB::DBMS::Evals(InOutParam<UnsafeArray<const UTF8Char>> valPtr, NN<DB::DBMS::SessionInfo> sess, DB::DBMSReader *reader, UIntOS colIndex, Text::CString colName, OutParam<Bool> valid)
{
	UnsafeArray<const UTF8Char> val = valPtr.Get();
	if (Text::StrStartsWith(val, U8STR("@@")))
	{
		Text::StringBuilderUTF8 sb;
		Text::StringBuilderUTF8 sb2;
		UTF8Char c;
		val += 2;
		while (true)
		{
			c = *val++;
			if (c == 0 || c == ',')
			{
				val--;
				break;
			}
			else if ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || (c >= '0' && c <= '9') || c == '_' || c == '.')
			{
				sb2.AppendUTF8Char(c);
			}
			else
			{
				val--;
				while (Text::CharUtil::PtrIsWS(val));
				if (val[0] == 0 || val[0] == ',')
				{
					break;
				}

				valid.Set(false);

				sb.AppendC(UTF8STRC("#HY000Unknown system variable '"));
				sb.AppendSlow(val);
				sb.AppendC(UTF8STRC("'"));
				OPTSTR_DEL(sess->lastError);
				sess->lastError = Text::String::New(sb.ToCString());
				return nullptr;
			}
		}
		valPtr.Set(val);
		if (reader)
		{
			this->SysVarColumn(reader, colIndex, sb2.ToString(), colName);
		}
		if (this->SysVarGet(sb, sess, sb2.ToCString()) == 0)
		{
			valid.Set(false);

			sb.AppendC(UTF8STRC("#HY000Unknown system variable '"));
			sb.AppendC(sb2.ToString(), sb2.GetLength());
			sb.AppendC(UTF8STRC("'"));
			OPTSTR_DEL(sess->lastError);
			sess->lastError = Text::String::New(sb.ToCString());
			return nullptr;
		}
		else
		{
			return Text::String::New(sb.ToString(), sb.GetLength());
		}
	}
	else if (val[0] == '@')
	{
		if (reader)
		{
			this->UserVarColumn(reader, colIndex, val + 1, colName);
		}
		Text::StringBuilderUTF8 sb;
		Text::CStringNN s = Text::CStringNN::FromPtr(val + 1);
		if (!this->UserVarGet(sb, sess, s))
		{
			return nullptr;
		}
		else
		{
			valPtr.Set(s.GetEndPtr());
			return Text::String::New(sb.ToString(), sb.GetLength());
		}
	}
	else if (val[0] == '\'')
	{
		Text::StringBuilderUTF8 sb;
		val++;
		UTF8Char c;
		while (true)
		{
			c = *val++;
			if (c == '\'')
			{
				if (*val == '\'')
				{
					val++;
					sb.AppendUTF8Char('\'');
				}
				else
				{
					valPtr.Set(val);
					Text::CStringNN nncolName;
					if (reader)
						reader->SetColumn(colIndex, (colName.SetTo(nncolName) && nncolName.leng > 0)?nncolName:sb.ToCString(), DB::DBUtil::CT_VarUTF8Char);
					return Text::String::New(sb.ToString(), sb.GetLength());
				}
			}
			else if (c == 0)
			{
				valid.Set(false);
				return nullptr;
			}
			else if (c == '\\')
			{
				if (*val == 't')
				{
					val++;
					sb.AppendUTF8Char('\t');
				}
				else if (*val == 'r')
				{
					val++;
					sb.AppendUTF8Char('\r');
				}
				else if (*val == 'n')
				{
					val++;
					sb.AppendUTF8Char('\n');
				}
				else if (*val == '\\')
				{
					val++;
					sb.AppendUTF8Char('\\');
				}
				else if (*val == '\'')
				{
					val++;
					sb.AppendUTF8Char('\'');
				}
				else if (*val == '\"')
				{
					val++;
					sb.AppendUTF8Char('\"');
				}
				else
				{
					sb.AppendUTF8Char('\\');	
				}
			}
			else
			{
				sb.AppendC(&val[-1], 1);
			}
		}
	}
	else if (val[0] == '"')
	{
		Text::StringBuilderUTF8 sb;
		val++;
		UTF8Char c;
		while (true)
		{
			c = *val++;
			if (c == '\"')
			{
				if (*val == '\"')
				{
					val++;
					sb.AppendUTF8Char('\"');
				}
				else
				{
					Text::CStringNN nncolName;
					valPtr.Set(val);
					if (reader)
						reader->SetColumn(colIndex, (colName.SetTo(nncolName) && nncolName.leng > 0)?nncolName:sb.ToCString(), DB::DBUtil::CT_VarUTF8Char);
					return Text::String::New(sb.ToString(), sb.GetLength());
				}
			}
			else if (c == 0)
			{
				valid.Set(false);
				return nullptr;
			}
			else if (c == '\\')
			{
				if (*val == 't')
				{
					val++;
					sb.AppendUTF8Char('\t');
				}
				else if (*val == 'r')
				{
					val++;
					sb.AppendUTF8Char('\r');
				}
				else if (*val == 'n')
				{
					val++;
					sb.AppendUTF8Char('\n');
				}
				else if (*val == '\\')
				{
					val++;
					sb.AppendUTF8Char('\\');
				}
				else if (*val == '\'')
				{
					val++;
					sb.AppendUTF8Char('\'');
				}
				else if (*val == '\"')
				{
					val++;
					sb.AppendUTF8Char('\"');
				}
				else
				{
					sb.AppendUTF8Char('\\');
				}
			}
			else
			{
				sb.AppendC(&val[-1], 1);
			}
		}
	}
	else if (val[0] >= '0' && val[0] <= '9')
	{
		Text::StringBuilderUTF8 sb;
		UnsafeArray<const UTF8Char> sptr = val;
		NN<Text::String> val2;
		UTF8Char c;
		Bool isDbl = false;
		while (true)
		{
			c = *sptr++;
			if (c >= '0' && c <= '9')
			{
				sb.AppendUTF8Char(c);
			}
			else if (c == '.')
			{
				isDbl = true;
				sb.AppendUTF8Char(c);
			}
			else
			{
				break;
			}
		}

		sptr--;
		while (Text::CharUtil::PtrIsWS(sptr));
		if (*sptr == 0 || *sptr == ',')
		{
			Text::CStringNN nncolName;
			if (reader)
				reader->SetColumn(colIndex, (colName.SetTo(nncolName) && nncolName.leng > 0)?nncolName:Text::CStringNN::FromPtr(val), isDbl?DB::DBUtil::CT_Double:DB::DBUtil::CT_Int32);
			valPtr.Set(sptr);
			return Text::String::New(sb.ToString(), sb.GetLength());
		}
		if (*sptr == '+')
		{
			sptr++;			
			while (Text::CharUtil::PtrIsWS(sptr));
			if (!this->Evals(sptr, sess, 0, 0, nullptr, valid).SetTo(val2))
			{
				valid.Set(false);
				return nullptr;
			}
			if (!isDbl)
			{
				Int32 iVal;
				if (val2->ToInt32(iVal))
				{
					iVal += Text::StrToInt32(sb.ToString());
					sb.ClearStr();
					sb.AppendI32(iVal);
					Text::CStringNN nncolName;
					if (reader)
						reader->SetColumn(colIndex, (colName.SetTo(nncolName) && nncolName.leng > 0)?nncolName:Text::CStringNN::FromPtr(val), DB::DBUtil::CT_Int32);
					valPtr.Set(sptr);
					val2->Release();
					return Text::String::New(sb.ToString(), sb.GetLength());
				}
			}
			Double dVal;
			if (val2->ToDouble(dVal))
			{
				dVal += sb.ToDoubleOr(0);
				sb.ClearStr();
				sb.AppendDouble(dVal);
				Text::CStringNN nncolName;
				if (reader)
					reader->SetColumn(colIndex, (colName.SetTo(nncolName) && nncolName.leng > 0)?nncolName:Text::CStringNN::FromPtr(val), DB::DBUtil::CT_Double);
				valPtr.Set(sptr);
				val2->Release();
				return Text::String::New(sb.ToString(), sb.GetLength());
			}
			else
			{
				////////////////////////////////
				valid.Set(false);
				val2->Release();
				return nullptr;
			}
		}
		else if (*sptr == '-')
		{
			sptr++;			
			while (Text::CharUtil::PtrIsWS(sptr));
			if (!this->Evals(sptr, sess, 0, 0, nullptr, valid).SetTo(val2))
			{
				valid.Set(false);
				return nullptr;
			}
			if (!isDbl)
			{
				Int32 iVal;
				if (val2->ToInt32(iVal))
				{
					iVal = Text::StrToInt32(sb.ToString()) - iVal;
					sb.ClearStr();
					sb.AppendI32(iVal);
					Text::CStringNN nncolName;
					if (reader)
						reader->SetColumn(colIndex, (colName.SetTo(nncolName) && nncolName.leng > 0)?nncolName:Text::CStringNN::FromPtr(val), DB::DBUtil::CT_Int32);
					valPtr.Set(sptr);
					val2->Release();
					return Text::String::New(sb.ToString(), sb.GetLength());
				}
			}
			Double dVal;
			if (val2->ToDouble(dVal))
			{
				dVal = sb.ToDoubleOr(0) - dVal;
				sb.ClearStr();
				sb.AppendDouble(dVal);
				Text::CStringNN nncolName;
				if (reader)
					reader->SetColumn(colIndex, (colName.SetTo(nncolName) && nncolName.leng > 0)?nncolName:Text::CStringNN::FromPtr(val), DB::DBUtil::CT_Double);
				valPtr.Set(sptr);
				val2->Release();
				return Text::String::New(sb.ToString(), sb.GetLength());
			}
			else
			{
				valid.Set(false);
				val2->Release();
				////////////////////////////////
				return nullptr;
			}
		}
		else if (*sptr == '*')
		{
			sptr++;			
			while (Text::CharUtil::PtrIsWS(sptr));
			if (!this->Evals(sptr, sess, 0, 0, nullptr, valid).SetTo(val2))
			{
				valid.Set(false);
				return nullptr;
			}
			if (!isDbl)
			{
				Int32 iVal;
				if (val2->ToInt32(iVal))
				{
					iVal = Text::StrToInt32(sb.ToString()) * iVal;
					sb.ClearStr();
					sb.AppendI32(iVal);
					Text::CStringNN nncolName;
					if (reader)
						reader->SetColumn(colIndex, (colName.SetTo(nncolName) && nncolName.leng > 0)?nncolName:Text::CStringNN::FromPtr(val), DB::DBUtil::CT_Int32);
					valPtr.Set(sptr);
					val2->Release();
					return Text::String::New(sb.ToString(), sb.GetLength());
				}
			}
			Double dVal;
			if (val2->ToDouble(dVal))
			{
				dVal = sb.ToDoubleOrNAN() * dVal;
				sb.ClearStr();
				sb.AppendDouble(dVal);
				Text::CStringNN nncolName;
				if (reader)
					reader->SetColumn(colIndex, (colName.SetTo(nncolName) && nncolName.leng > 0)?nncolName:Text::CStringNN::FromPtr(val), DB::DBUtil::CT_Double);
				valPtr.Set(sptr);
				val2->Release();
				return Text::String::New(sb.ToString(), sb.GetLength());
			}
			else
			{
				valid.Set(false);
				val2->Release();
				////////////////////////////////
				return nullptr;
			}
		}
		else if (*sptr == '/')
		{
			sptr++;			
			while (Text::CharUtil::PtrIsWS(sptr));
			if (!this->Evals(sptr, sess, 0, 0, nullptr, valid).SetTo(val2))
			{
				valid.Set(false);
				return nullptr;
			}
			if (!isDbl)
			{
				Int32 iVal;
				if (val2->ToInt32(iVal))
				{
					iVal = Text::StrToInt32(sb.ToString()) / iVal;
					sb.ClearStr();
					sb.AppendI32(iVal);
					Text::CStringNN nncolName;
					if (reader)
						reader->SetColumn(colIndex, (colName.SetTo(nncolName) && nncolName.leng > 0)?nncolName:Text::CStringNN::FromPtr(val), DB::DBUtil::CT_Int32);
					valPtr.Set(sptr);
					val2->Release();
					return Text::String::New(sb.ToString(), sb.GetLength());
				}
			}
			Double dVal;
			if (val2->ToDouble(dVal))
			{
				dVal = sb.ToDoubleOrNAN() / dVal;
				sb.ClearStr();
				sb.AppendDouble(dVal);
				Text::CStringNN nncolName;
				if (reader)
					reader->SetColumn(colIndex, (colName.SetTo(nncolName) && nncolName.leng > 0)?nncolName:Text::CStringNN::FromPtr(val), DB::DBUtil::CT_Double);
				valPtr.Set(sptr);
				val2->Release();
				return Text::String::New(sb.ToString(), sb.GetLength());
			}
			else
			{
				valid.Set(false);
				////////////////////////////////
				val2->Release();
				return nullptr;
			}
		}
		else
		{
			valid.Set(false);
			return nullptr;
		}
	}
	else
	{
		Text::StringBuilderUTF8 sb;
		UnsafeArray<const UTF8Char> sptr = val;
		UTF8Char c;
		while (true)
		{
			c = *sptr++;
			if ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z'))
			{
				sb.AppendUTF8Char(c);
			}
			else if (c == '_' || c == '.')
			{
				sb.AppendUTF8Char(c);
			}
			else if (c >= '0' && c <= '9')
			{
				sb.AppendUTF8Char(c);
			}
			else if (c == 0)
			{
				break;
			}
			else
			{
				sptr--;
				while (Text::CharUtil::PtrIsWS(sptr));
				if (sptr[0] == '(')
				{
					if (sb.EqualsICase(UTF8STRC("CONCAT")))
					{
						sb.ClearStr();
						NN<Text::String> sVal;
						Bool v = true;
						sptr++;
						while (true)
						{
							while (Text::CharUtil::PtrIsWS(sptr));
							if (!this->Evals(sptr, sess, 0, 0, nullptr, v).SetTo(sVal) || !v)
							{
								valid.Set(false);
								return nullptr;
							}
							sb.Append(sVal);
							sVal->Release();
							while (Text::CharUtil::PtrIsWS(sptr));
							if (sptr[0] == ')')
							{
								Text::CStringNN nncolName;
								if (reader)
									reader->SetColumn(colIndex, (colName.SetTo(nncolName) && nncolName.leng > 0)?nncolName:Text::CStringNN::FromPtr(val), DB::DBUtil::CT_Double);
								valPtr.Set(sptr + 1);
								return Text::String::New(sb.ToString(), sb.GetLength());
							}
							else if (sptr[0] == ',')
							{
								sptr++;
							}
							else
							{
								valid.Set(false);

								sb.ClearStr();
								sb.AppendC(UTF8STRC("#42S22Unknown column '"));
								sb.AppendSlow(val);
								sb.AppendC(UTF8STRC("' in field list"));
								OPTSTR_DEL(sess->lastError);
								sess->lastError = Text::String::New(sb.ToCString());

								return nullptr;
							}
						}
					}
					else if (sb.EqualsICase(UTF8STRC("DATABASE")))
					{
						sptr++;
						while (Text::CharUtil::PtrIsWS(sptr));
						if (sptr[0] != ')')
						{
							valid.Set(false);

							sb.ClearStr();
							sb.AppendC(UTF8STRC("#42S22Unknown column '"));
							sb.AppendSlow(sptr);
							sb.AppendC(UTF8STRC("' in field list"));
							OPTSTR_DEL(sess->lastError);
							sess->lastError = Text::String::New(sb.ToCString());

							return nullptr;
						}
						else
						{
							valPtr.Set(sptr + 1);
							return Text::String::CopyOrNull(sess->database);
						}
					}
					else
					{
						break;
					}
				}
				else
				{
					break;
				}
			}
		}
		valid.Set(false);

		sb.ClearStr();
		sb.AppendC(UTF8STRC("#42S22Unknown column '"));
		sb.AppendSlow(val);
		sb.AppendC(UTF8STRC("' in field list"));
		OPTSTR_DEL(sess->lastError);
		sess->lastError = Text::String::New(sb.ToCString());

		return nullptr;
	}
}

DB::DBMS::DBMS(Text::CStringNN versionStr, NN<IO::LogTool> log)
{
	this->versionStr = Text::String::New(versionStr);
	this->log = log;
}

DB::DBMS::~DBMS()
{
	this->versionStr->Release();
	NN<DB::DBMS::LoginInfo> login;
	NN<DB::DBMS::UserInfo> user;
	UIntOS i = this->loginMap.GetCount();
	UIntOS j;
	while (i-- > 0)
	{
		login = this->loginMap.GetItemNoCheck(i);
		j = login->userList.GetCount();
		while (j-- > 0)
		{
			user = login->userList.GetItemNoCheck(j);
			MemFreeNN(user);
		}
		login->login->Release();
		login.Delete();
	}

	i = this->sessMap.GetCount();
	while (i-- > 0)
	{
		this->SessDelete(this->sessMap.GetItemNoCheck(i));
	}
}

NN<Text::String> DB::DBMS::GetVersion() const
{
	return this->versionStr;
}

NN<IO::LogTool> DB::DBMS::GetLogTool()
{
	return this->log;
}

Bool DB::DBMS::UserAdd(Int32 userId, Text::CStringNN userName, Text::CStringNN password, Text::CStringNN host)
{
	NN<DB::DBMS::LoginInfo> login;
	NN<DB::DBMS::UserInfo> user;
	UIntOS i;
	Bool succ;
	#if defined(VERBOSE)
	printf("UserAdd %s/%s@%s\r\n", userName.v.Ptr(), password.v.Ptr(), host.v.Ptr());
	#endif
	Sync::MutexUsage mutUsage(this->loginMut);
	if (!this->loginMap.GetC(userName).SetTo(login))
	{
		NEW_CLASSNN(login, DB::DBMS::LoginInfo());
		login->login = Text::String::New(userName);
		this->loginMap.PutC(userName, login);

		user = MemAllocNN(DB::DBMS::UserInfo);
		user->hostLen = (UIntOS)(host.ConcatTo(user->host) - user->host);
		this->loginSHA1.Clear();
		this->loginSHA1.Calc(password.v, password.leng);
		this->loginSHA1.GetValue(user->pwdSha1);
		user->userId = userId;
		login->userList.Add(user);
		succ = true;
	}
	else
	{
		succ = true;
		i = login->userList.GetCount();
		while (i-- > 0)
		{
			user = login->userList.GetItemNoCheck(i);
			if (host.Equals(user->host, user->hostLen))
			{
				succ = false;
				break;
			}
		}
		if (succ)
		{
			user = MemAllocNN(DB::DBMS::UserInfo);
			user->hostLen = (UIntOS)(host.ConcatTo(user->host) - user->host);
			this->loginSHA1.Clear();
			this->loginSHA1.Calc(password.v, password.leng);
			this->loginSHA1.GetValue(user->pwdSha1);
			user->userId = userId;
			login->userList.Add(user);
		}
	}
	mutUsage.EndUse();
	return succ;
}

Int32 DB::DBMS::UserLoginMySQL(Int32 sessId, Text::CStringNN userName, UnsafeArray<const UInt8> randomData, UnsafeArray<const UInt8> passHash, NN<const Net::SocketUtil::AddressInfo> addr, const DB::DBMS::SessionParam *param, UnsafeArrayOpt<const UTF8Char> database)
{
	NN<DB::DBMS::LoginInfo> login;
	NN<DB::DBMS::UserInfo> user;
	UInt8 hashBuff[20];
	UIntOS j;
	Int32 userId = 0;
	#if defined(VERBOSE)
	printf("mysql_native_password auth\r\n");
	#endif
	Sync::MutexUsage mutUsage(this->loginMut);
	if (this->loginMap.GetC(userName).SetTo(login))
	{
		UIntOS i = login->userList.GetCount();
		while (i-- > 0)
		{
			user = login->userList.GetItemNoCheck(i);
			this->loginSHA1.Clear();
			this->loginSHA1.Calc(user->pwdSha1, 20);
			this->loginSHA1.GetValue(hashBuff);

			this->loginSHA1.Clear();
			this->loginSHA1.Calc(randomData, 20);
			this->loginSHA1.Calc(hashBuff, 20);
			this->loginSHA1.GetValue(hashBuff);
			
			j = 0;
			while (j < 20)
			{
				hashBuff[j] = (UInt8)(hashBuff[j] ^ user->pwdSha1[j]);
				j++;
			}

			#if defined(VERBOSE)
			Text::StringBuilderUTF8 sb;
			sb.AppendHexBuff(hashBuff, 20, ' ', Text::LineBreakType::None);
			printf("Password Hash = %s\r\n", sb.ToPtr());
			#endif
			userId = user->userId;
			j = 20;
			while (j-- > 0)
			{
				if (passHash[j] != hashBuff[j])
				{
					userId = 0;
					break;
				}
			}

			if (userId != 0)
			{
				NN<DB::DBMS::SessionInfo> sess;
				Sync::MutexUsage mutUsage(this->sessMut);
				if (!this->sessMap.Get(sessId).SetTo(sess))
				{
					NEW_CLASSNN(sess, DB::DBMS::SessionInfo());
					sess->sessId = sessId;
					sess->lastError = nullptr;
					sess->autoCommit = 1;
					sess->autoIncInc = 1;
					sess->sqlModes = (DB::DBMS::SQLMODE)(SQLM_ONLY_FULL_GROUP_BY | SQLM_STRICT_TRANS_TABLES | SQLM_NO_ZERO_IN_DATE | SQLM_NO_ZERO_DATE | SQLM_ERROR_FOR_DIVISION_BY_ZERO | SQLM_NO_ENGINE_SUBSTITUTION);
					sess->database = nullptr;
					UnsafeArray<const UTF8Char> nndatabase;
					if (database.SetTo(nndatabase) && nndatabase[0])
					{
						sess->database = Text::String::NewNotNullSlow(nndatabase);
					}
					MemCopyNO(&sess->params, param, sizeof(DB::DBMS::SessionParam));
					this->sessMap.Put(sessId, sess);
				}
				sess->user = user;
				mutUsage.EndUse();
				break;
			}
		}
	}
	else
	{
		#if defined(VERBOSE)
		printf("Login not found\r\n");
		#endif
	}
	mutUsage.EndUse();
	return userId;
}

Optional<DB::DBReader> DB::DBMS::ExecuteReader(Int32 sessId, UnsafeArray<const UTF8Char> sql, UIntOS sqlLen)
{
	UnsafeArray<const UTF8Char> sptr1;
	UnsafeArray<const UTF8Char> sptr2;
	UnsafeArray<const UTF8Char> sptr3;
	UnsafeArray<const UTF8Char> sqlEnd = sql + sqlLen;
	UIntOS i;
	UIntOS j;
	UTF8Char nameBuff[128];
	UTF8Char nameBuff2[128];
	NN<DB::DBMS::SessionInfo> sess;
	if (!this->SessGet(sessId).SetTo(sess))
	{
		return nullptr;
	}
	sptr1 = sql;
	while (Text::CharUtil::PtrIsWS(sptr1));
	if (Text::StrStartsWithICaseC(sptr1, (UIntOS)(sqlEnd - sptr1), UTF8STRC("SELECT")) && Text::CharUtil::IsWS(sptr1 + 6))
	{
		sptr1 += 6;
		Data::ArrayListObj<DB::DBMS::SQLColumn*> cols;
		DB::DBMS::SQLColumn *col;
		Bool hasFrom = false;

		while (true)
		{
			while (Text::CharUtil::PtrIsWS(sptr1));
			sptr2 = sptr1;
			if (!SQLParseName(nameBuff, sptr1).SetTo(sptr1))
			{
				i = cols.GetCount();
				while (i-- > 0)
				{
					col = cols.GetItem(i);
					col->name->Release();
					OPTSTR_DEL(col->asName);
					MemFree(col);
				}

				Text::StringBuilderUTF8 sb;
				sb.AppendC(UTF8STRC("#42000You have an error in your SQL syntax; check the manual that corresponds to your MySQL server version for the right syntax to use near '"));
				sb.AppendC(sptr2, (UIntOS)(sqlEnd - sptr2));
				sb.AppendC(UTF8STRC("' at line 1"));
				OPTSTR_DEL(sess->lastError);
				sess->lastError = Text::String::New(sb.ToCString()).Ptr();
				return nullptr;
			}
			while (Text::CharUtil::PtrIsWS(sptr1));
			if (*sptr1 == ',' || *sptr1 == 0)
			{
				col = MemAlloc(DB::DBMS::SQLColumn, 1);
				col->name = Text::String::NewNotNullSlow(nameBuff);
				col->asName = nullptr;
				col->sqlPtr = sptr2;
				cols.Add(col);
				if (*sptr1 == 0)
				{
					break;
				}
				else
				{
					sptr1++;
				}
			}
			else if (Text::StrStartsWithICaseC(sptr1, (UIntOS)(sqlEnd - sptr1), UTF8STRC("AS")) && Text::CharUtil::IsWS(sptr1 + 2))
			{
				sptr1 += 2;
				while (Text::CharUtil::PtrIsWS(sptr1));
				sptr3 = sptr1;
				if (!SQLParseName(nameBuff2, sptr1).SetTo(sptr1))
				{
					i = cols.GetCount();
					while (i-- > 0)
					{
						col = cols.GetItem(i);
						col->name->Release();
						OPTSTR_DEL(col->asName);
						MemFree(col);
					}

					Text::StringBuilderUTF8 sb;
					sb.AppendC(UTF8STRC("#42000You have an error in your SQL syntax; check the manual that corresponds to your MySQL server version for the right syntax to use near '"));
					sb.AppendC(sptr3, (UIntOS)(sqlEnd - sptr3));
					sb.AppendC(UTF8STRC("' at line 1"));
					OPTSTR_DEL(sess->lastError);
					sess->lastError = Text::String::New(sb.ToCString()).Ptr();
					return nullptr;
				}
				while (Text::CharUtil::PtrIsWS(sptr1));
				if (*sptr1 == ',' || *sptr1 == 0)
				{
					col = MemAlloc(DB::DBMS::SQLColumn, 1);
					col->name = Text::String::NewNotNullSlow(nameBuff);
					col->asName = Text::String::NewNotNullSlow(nameBuff2).Ptr();
					col->sqlPtr = sptr2;
					cols.Add(col);
					if (*sptr1 == 0)
					{
						break;
					}
					else
					{
						sptr1++;
					}
				}
				else if (Text::StrStartsWithICaseC(sptr1, (UIntOS)(sqlEnd - sptr1), UTF8STRC("FROM")) && Text::CharUtil::IsWS(sptr1 + 4))
				{
					hasFrom = true;
					sptr1 += 4;
					break;
				}
				else
				{
					i = cols.GetCount();
					while (i-- > 0)
					{
						col = cols.GetItem(i);
						col->name->Release();
						OPTSTR_DEL(col->asName);
						MemFree(col);
					}

					Text::StringBuilderUTF8 sb;
					sb.AppendC(UTF8STRC("#42000You have an error in your SQL syntax; check the manual that corresponds to your MySQL server version for the right syntax to use near '"));
					sb.AppendC(sptr1, (UIntOS)(sqlEnd - sptr1));
					sb.AppendC(UTF8STRC("' at line 1"));
					OPTSTR_DEL(sess->lastError);
					sess->lastError = Text::String::New(sb.ToCString()).Ptr();
					return nullptr;
				}
			}
			else if (Text::StrStartsWithICaseC(sptr1, (UIntOS)(sqlEnd - sptr1), UTF8STRC("FROM")) && Text::CharUtil::IsWS(sptr1 + 4))
			{
				hasFrom = true;
				sptr1 += 4;
				break;
			}
			else
			{
				sptr3 = sptr1;
				if (!SQLParseName(nameBuff2, sptr1).SetTo(sptr1))
				{
					i = cols.GetCount();
					while (i-- > 0)
					{
						col = cols.GetItem(i);
						col->name->Release();
						OPTSTR_DEL(col->asName);
						MemFree(col);
					}

					Text::StringBuilderUTF8 sb;
					sb.AppendC(UTF8STRC("#42000You have an error in your SQL syntax; check the manual that corresponds to your MySQL server version for the right syntax to use near '"));
					sb.AppendSlow(sptr3);
					sb.AppendC(UTF8STRC("' at line 1"));
					OPTSTR_DEL(sess->lastError);
					sess->lastError = Text::String::New(sb.ToCString()).Ptr();
					return nullptr;
				}
				while (Text::CharUtil::PtrIsWS(sptr1));
				if (*sptr1 == ',' || *sptr1 == 0)
				{
					col = MemAlloc(DB::DBMS::SQLColumn, 1);
					col->name = Text::String::NewNotNullSlow(nameBuff);
					col->asName = Text::String::NewNotNullSlow(nameBuff2).Ptr();
					col->sqlPtr = sptr2;
					cols.Add(col);
					if (*sptr1 == 0)
					{
						break;
					}
					else
					{
						sptr1++;
					}
				}
				else if (Text::StrStartsWithICaseC(sptr1, (UIntOS)(sqlEnd - sptr1), UTF8STRC("FROM")) && Text::CharUtil::IsWS(sptr1 + 4))
				{
					hasFrom = true;
					sptr1 += 4;
					break;
				}
				else
				{
					i = cols.GetCount();
					while (i-- > 0)
					{
						col = cols.GetItem(i);
						col->name->Release();
						OPTSTR_DEL(col->asName);
						MemFree(col);
					}

					Text::StringBuilderUTF8 sb;
					sb.AppendC(UTF8STRC("#42000You have an error in your SQL syntax; check the manual that corresponds to your MySQL server version for the right syntax to use near '"));
					sb.AppendC(sptr1, (UIntOS)(sqlEnd - sptr1));
					sb.AppendC(UTF8STRC("' at line 1"));
					OPTSTR_DEL(sess->lastError);
					sess->lastError = Text::String::New(sb.ToCString()).Ptr();
					return nullptr;
				}
			}
		}

		if (hasFrom)
		{
			while (Text::CharUtil::PtrIsWS(sptr1));
			//////////////////////////////
			i = cols.GetCount();
			while (i-- > 0)
			{
				col = cols.GetItem(i);
				col->name->Release();
				OPTSTR_DEL(col->asName);
				MemFree(col);
			}

			Text::StringBuilderUTF8 sb;
			sb.AppendC(UTF8STRC("#00000Unsupported syntax 'FROM'"));
			OPTSTR_DEL(sess->lastError);
			sess->lastError = Text::String::New(sb.ToCString());

			return nullptr;
		}
		else
		{
			Bool valid = true;
			DB::DBMSReader *reader;
			Text::StringBuilderUTF8 sb;
			UnsafeArray<Optional<Text::String>> colVals;
			UnsafeArray<const UTF8Char> val;
			NEW_CLASS(reader, DB::DBMSReader(cols.GetCount(), -1));
			colVals = MemAllocArr(Optional<Text::String>, cols.GetCount());
			i = 0;
			j = cols.GetCount();
			while (i < j && valid)
			{
				col = cols.GetItem(i);
				val = col->name->v;
				#if defined(VERBOSE)
				printf("Column %d is %s\r\n", (int)i, val.Ptr());
				#endif
				colVals[i] = this->Evals(val, sess, reader, i, OPTSTR_CSTR(col->asName), valid);

				col->name->Release();
				OPTSTR_DEL(col->asName);
				MemFree(col);
				i++;

				if (!valid)
				{
					break;
				}
			}
			while (i < j)
			{
				col = cols.GetItem(i);
				colVals[i] = nullptr;
				col->name->Release();
				OPTSTR_DEL(col->asName);
				MemFree(col);
				i++;
			}

			if (valid)
			{
				reader->AddRow(colVals);
				MemFreeArr(colVals);

				return reader;
			}
			else
			{
				i = j;
				while (i-- > 0)
				{
					OPTSTR_DEL(colVals[i]);
				}
				MemFreeArr(colVals);
				DEL_CLASS(reader);
				return nullptr;
			}
		}
	}
	else if (Text::StrStartsWithICaseC(sptr1, (UIntOS)(sqlEnd - sptr1), UTF8STRC("SET")) && Text::CharUtil::IsWS(sptr1 + 3))
	{
		sptr1 += 3;

		Data::ArrayListObj<DB::DBMS::SQLColumn*> cols;
		DB::DBMS::SQLColumn *col;
		UTF8Char c;
		UnsafeArray<UTF8Char> namePtr;

		while (true)
		{
			while (Text::CharUtil::PtrIsWS(sptr1));
			sptr2 = sptr1;
			if (sptr1[0] == '@' && sptr1[1] == '@')
			{
				Bool isGlobal = false;
				namePtr = nameBuff;
				sptr1 += 2;
				while (true)
				{
					c = *sptr1++;
					if (c == '.')
					{
						*namePtr = 0;
						if (Text::StrEqualsICaseC(nameBuff, (UIntOS)(namePtr - nameBuff), UTF8STRC("GLOBAL")))
						{
							isGlobal = true;
							namePtr = nameBuff;
						}
						else if (Text::StrEqualsICaseC(nameBuff, (UIntOS)(namePtr - nameBuff), UTF8STRC("SESSION")))
						{
							namePtr = nameBuff;
						}
						else
						{
							i = cols.GetCount();
							while (i-- > 0)
							{
								col = cols.GetItem(i);
								col->name->Release();
								OPTSTR_DEL(col->asName);
								MemFree(col);
							}

							Text::StringBuilderUTF8 sb;
							sb.AppendC(UTF8STRC("#00000Unsupported syntax 'FROM'"));
							OPTSTR_DEL(sess->lastError);
							sess->lastError = Text::String::New(sb.ToCString());
							return nullptr;
						}
					}
					else if ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || (c >= '0' && c <= '9'))
					{
						*namePtr++ = c;
					}
					else
					{
						sptr1--;
						while (Text::CharUtil::PtrIsWS(sptr1));
						if (sptr1[0] == '=' || (sptr1[0] == ':' && sptr1[1] == '='))
						{
							if (sptr1[0] == '=')
							{
								sptr1++;
							}
							else
							{
								sptr1 += 2;
							}
							break;
						}
						else
						{
							i = cols.GetCount();
							while (i-- > 0)
							{
								col = cols.GetItem(i);
								col->name->Release();
								OPTSTR_DEL(col->asName);
								MemFree(col);
							}

							Text::StringBuilderUTF8 sb;
							sb.AppendC(UTF8STRC("#00000Unsupported syntax 'FROM'"));
							OPTSTR_DEL(sess->lastError);
							sess->lastError = Text::String::New(sb.ToCString());
							return nullptr;
						}
					}
				}
				*namePtr = 0;
				if (this->SysVarExist(sess, CSTRP(nameBuff, namePtr), isGlobal?(DB::DBMS::AT_SET_GLOBAL):(DB::DBMS::AT_SET_SESSION)))
				{
					while (Text::CharUtil::PtrIsWS(sptr1));
					Bool valid = true;
					NN<Text::String> val;
					Optional<Text::String> optval;
					if (!(optval = this->Evals(sptr1, sess, 0, 0, nullptr, valid)).SetTo(val) || !valid)
					{
						i = cols.GetCount();
						while (i-- > 0)
						{
							col = cols.GetItem(i);
							col->name->Release();
							OPTSTR_DEL(col->asName);
							MemFree(col);
						}
						OPTSTR_DEL(optval);
						return nullptr;
					}
					nameBuff2[0] = isGlobal?'!':'#';
					Text::StrConcat(&nameBuff2[1], nameBuff);
					col = MemAlloc(DB::DBMS::SQLColumn, 1);
					col->name = Text::String::NewNotNullSlow(nameBuff2);
					col->asName = val;
					cols.Add(col);

					while (Text::CharUtil::PtrIsWS(sptr1));
					if (sptr1[0] == ',')
					{
						sptr1++;
					}
					else if (sptr1[0] == 0)
					{
						i = cols.GetCount();
						while (i-- > 0)
						{
							col = cols.GetItem(i);
							if (col->name->v[0] == '!')
							{
								this->SysVarSet(sess, true, col->name->ToCString().Substring(1), col->asName);
							}
							else if (col->name->v[0] == '@')
							{
								this->UserVarSet(sess, col->name->ToCString().Substring(1), col->asName);
							}
							else if (col->name->v[0] == '#')
							{
								this->SysVarSet(sess, false, col->name->ToCString().Substring(1), col->asName);
							}
							col->name->Release();
							OPTSTR_DEL(col->asName);
							MemFree(col);
						}

						DB::DBMSReader *reader;
						NEW_CLASS(reader, DB::DBMSReader(0, 0));
						return reader;
					}
				}
				else
				{
					Text::StringBuilderUTF8 sb;
					sb.AppendC(UTF8STRC("#00000Unsupported syntax 'FROM'"));
					OPTSTR_DEL(sess->lastError);
					sess->lastError = Text::String::New(sb.ToCString());
					return nullptr;
				}
			}
			else if (sptr1[0] == '@')
			{
				Text::StringBuilderUTF8 sb;
				Bool valid = true;
				UTF8Char c;
				sb.AppendUTF8Char('@');

				if (sptr1[1] == '\'')
				{
					sptr1 += 2;
					while (true)
					{
						c = *sptr1++;
						if (c == '\'')
						{
							while (Text::CharUtil::PtrIsWS(sptr1));
							if (sptr1[0] == '=')
							{
								sptr1++;
								break;
							}
							else if (sptr1[0] == ':' && sptr1[1] == '=')
							{
								sptr1 += 2;
								break;
							}
							else
							{
								valid = false;

								sb.ClearStr();
								sb.AppendC(UTF8STRC("#00000Unsupported syntax '"));
								sb.AppendSlow(sptr1);
								sb.AppendUTF8Char('\'');
								OPTSTR_DEL(sess->lastError);
								sess->lastError = Text::String::New(sb.ToCString());
								break;
							}
						}
						else if (c == '\\')
						{
							c = *sptr1++;
							if (c == 0)
							{
								valid = false;

								sb.ClearStr();
								sb.AppendC(UTF8STRC("#00000Unsupported syntax '"));
								sb.AppendSlow(sptr1);
								sb.AppendUTF8Char('\'');
								OPTSTR_DEL(sess->lastError);
								sess->lastError = Text::String::New(sb.ToCString());
								break;
							}
							else if (c == 't')
							{
								sb.AppendUTF8Char('\t');
							}
							else if (c == 'n')
							{
								sb.AppendUTF8Char('\n');
							}
							else if (c == 'r')
							{
								sb.AppendUTF8Char('\r');
							}
							else
							{
								sb.AppendUTF8Char(c);
							}
						}
						else if (c == 0)
						{
							valid = false;

							sb.ClearStr();
							sb.AppendC(UTF8STRC("#00000Unsupported syntax '"));
							sb.AppendSlow(sptr1);
							sb.AppendUTF8Char('\'');
							OPTSTR_DEL(sess->lastError);
							sess->lastError = Text::String::New(sb.ToCString());
							break;

						}
						else
						{
							sb.AppendUTF8Char(c);
						}
					}
				}
				else if (sptr1[1] == '"')
				{
					sptr1 += 2;
					while (true)
					{
						c = *sptr1++;
						if (c == '\"')
						{
							while (Text::CharUtil::PtrIsWS(sptr1));
							if (sptr1[0] == '=')
							{
								sptr1++;
								break;
							}
							else if (sptr1[0] == ':' && sptr1[1] == '=')
							{
								sptr1 += 2;
								break;
							}
							else
							{
								valid = false;

								sb.ClearStr();
								sb.AppendC(UTF8STRC("#00000Unsupported syntax '"));
								sb.AppendSlow(sptr1);
								sb.AppendUTF8Char('\'');
								OPTSTR_DEL(sess->lastError);
								sess->lastError = Text::String::New(sb.ToCString());
								break;
							}
						}
						else if (c == '\\')
						{
							c = *sptr1++;
							if (c == 0)
							{
								valid = false;

								sb.ClearStr();
								sb.AppendC(UTF8STRC("#00000Unsupported syntax '"));
								sb.AppendSlow(sptr1);
								sb.AppendUTF8Char('\'');
								OPTSTR_DEL(sess->lastError);
								sess->lastError = Text::String::New(sb.ToCString());
								break;
							}
							else if (c == 't')
							{
								sb.AppendUTF8Char('\t');
							}
							else if (c == 'n')
							{
								sb.AppendUTF8Char('\n');
							}
							else if (c == 'r')
							{
								sb.AppendUTF8Char('\r');
							}
							else
							{
								sb.AppendUTF8Char(c);
							}
						}
						else if (c == 0)
						{
							valid = false;

							sb.ClearStr();
							sb.AppendC(UTF8STRC("#00000Unsupported syntax '"));
							sb.AppendSlow(sptr1);
							sb.AppendUTF8Char('\'');
							OPTSTR_DEL(sess->lastError);
							sess->lastError = Text::String::New(sb.ToCString());
							break;

						}
						else
						{
							sb.AppendUTF8Char(c);
						}
					}
				}
				else if ((sptr1[1] >= 'A' && sptr1[1] <= 'Z') || (sptr1[1] >= 'a' && sptr1[1] <= 'z') || (sptr1[1] >= '0' && sptr1[1] <= '9') || sptr1[1] == '.' || sptr1[1] == '_' || sptr1[1] == '$')
				{
					sptr1++;
					while (true)
					{
						c = *sptr1++;
						if ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || (c >= '0' && c <= '9') || c == '.' || c == '_' || c == '$')
						{
							sb.AppendUTF8Char(c);
						}
						else
						{
							sptr1--;
							while (Text::CharUtil::PtrIsWS(sptr1));
							if (sptr1[0] == '=')
							{
								sptr1++;
								break;
							}
							else if (sptr1[0] == ':' && sptr1[1] == '=')
							{
								sptr1 += 2;
								break;
							}
							else
							{
								valid = false;

								sb.ClearStr();
								sb.AppendC(UTF8STRC("#00000Unsupported syntax '"));
								sb.AppendSlow(sptr1);
								sb.AppendUTF8Char('\'');
								OPTSTR_DEL(sess->lastError);
								sess->lastError = Text::String::New(sb.ToCString());
								break;
							}
						}
					}
				}
				else
				{
					valid = false;

					sb.ClearStr();
					sb.AppendC(UTF8STRC("#00000Unsupported syntax '"));
					sb.AppendSlow(sptr1);
					sb.AppendUTF8Char('\'');
					OPTSTR_DEL(sess->lastError);
					sess->lastError = Text::String::New(sb.ToCString());
				}
				if (valid)
				{
					while (Text::CharUtil::PtrIsWS(sptr1));
					Optional<Text::String> val = this->Evals(sptr1, sess, 0, 0, nullptr, valid);
					if (!valid)
					{
						i = cols.GetCount();
						while (i-- > 0)
						{
							col = cols.GetItem(i);
							col->name->Release();
							OPTSTR_DEL(col->asName);
							MemFree(col);
						}
						OPTSTR_DEL(val);


						sb.ClearStr();
						sb.AppendC(UTF8STRC("#00000Unsupported syntax '"));
						sb.AppendSlow(sptr1);
						sb.AppendUTF8Char('\'');
						OPTSTR_DEL(sess->lastError);
						sess->lastError = Text::String::New(sb.ToCString());
						return nullptr;
					}
					col = MemAlloc(DB::DBMS::SQLColumn, 1);
					col->name = Text::String::New(sb.ToCString());
					col->asName = val;
					cols.Add(col);


					while (Text::CharUtil::PtrIsWS(sptr1));
					if (sptr1[0] == ',')
					{
						sptr1++;
					}
					else if (sptr1[0] == 0)
					{
						i = cols.GetCount();
						while (i-- > 0)
						{
							col = cols.GetItem(i);
							if (col->name->v[0] == '!')
							{
								this->SysVarSet(sess, true, col->name->ToCString().Substring(1), col->asName);
							}
							else if (col->name->v[0] == '@')
							{
								this->UserVarSet(sess, col->name->ToCString().Substring(1),  col->asName);
							}
							else if (col->name->v[0] == '#')
							{
								this->SysVarSet(sess, false, col->name->ToCString().Substring(1), col->asName);
							}
							col->name->Release();
							OPTSTR_DEL(col->asName);
							MemFree(col);
						}

						DB::DBMSReader *reader;
						NEW_CLASS(reader, DB::DBMSReader(0, 0));
						return reader;
					}
				}
				else
				{
					i = cols.GetCount();
					while (i-- > 0)
					{
						col = cols.GetItem(i);
						col->name->Release();
						OPTSTR_DEL(col->asName);
						MemFree(col);
					}
					return nullptr;
				}
			}
			else if ((sptr1[0] >= 'A' && sptr1[0] <= 'Z') || (sptr1[0] >='a' && sptr1[0] <= 'z'))
			{
				nameBuff[0] = '#';
				namePtr = nameBuff + 1;

				while (true)
				{
					c = *sptr1++;
					if ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || (c >= '0' && c <= '9') || c == '_')
					{
						*namePtr++ = c;
					}
					else
					{
						sptr1--;
						while (Text::CharUtil::PtrIsWS(sptr1));
						if (sptr1[0] == '=' || (sptr1[0] == ':' && sptr1[1] == '='))
						{
							if (sptr1[0] == '=')
							{
								sptr1++;
							}
							else
							{
								sptr1 += 2;
							}
							break;
						}
						else
						{
							i = cols.GetCount();
							while (i-- > 0)
							{
								col = cols.GetItem(i);
								col->name->Release();
								OPTSTR_DEL(col->asName);
								MemFree(col);
							}

							Text::StringBuilderUTF8 sb;
							sb.AppendC(UTF8STRC("#00000Unsupported syntax 'FROM'"));
							OPTSTR_DEL(sess->lastError);
							sess->lastError = Text::String::New(sb.ToCString()).Ptr();
							return nullptr;
						}
					}
				}
				*namePtr = 0;
				if (this->SysVarExist(sess, CSTRP(nameBuff + 1, namePtr), DB::DBMS::AT_SET_SESSION))
				{
					while (Text::CharUtil::PtrIsWS(sptr1));
					Bool valid = true;
					Optional<Text::String> val = this->Evals(sptr1, sess, 0, 0, nullptr, valid);
					if (!valid)
					{
						i = cols.GetCount();
						while (i-- > 0)
						{
							col = cols.GetItem(i);
							col->name->Release();
							OPTSTR_DEL(col->asName);
							MemFree(col);
						}
						OPTSTR_DEL(val);

						return nullptr;
					}
					col = MemAlloc(DB::DBMS::SQLColumn, 1);
					col->name = Text::String::NewP(nameBuff, namePtr);
					col->asName = val;
					cols.Add(col);

					while (Text::CharUtil::PtrIsWS(sptr1));
					if (sptr1[0] == ',')
					{
						sptr1++;
					}
					else if (sptr1[0] == 0)
					{
						i = cols.GetCount();
						while (i-- > 0)
						{
							col = cols.GetItem(i);
							if (col->name->v[0] == '!')
							{
								this->SysVarSet(sess, true, col->name->ToCString().Substring(1), col->asName);
							}
							else if (col->name->v[0] == '@')
							{
								this->UserVarSet(sess, col->name->ToCString().Substring(1),  col->asName);
							}
							else if (col->name->v[0] == '#')
							{
								this->SysVarSet(sess, false, col->name->ToCString().Substring(1), col->asName);
							}
							col->name->Release();
							OPTSTR_DEL(col->asName);
							MemFree(col);
						}

						DB::DBMSReader *reader;
						NEW_CLASS(reader, DB::DBMSReader(0, 0));
						return reader;
					}
				}
				else
				{
					i = cols.GetCount();
					while (i-- > 0)
					{
						col = cols.GetItem(i);
						col->name->Release();
						OPTSTR_DEL(col->asName);
						MemFree(col);
					}

					Text::StringBuilderUTF8 sb;
					sb.AppendC(UTF8STRC("#00000Unsupported syntax 'FROM'"));
					OPTSTR_DEL(sess->lastError);
					sess->lastError = Text::String::New(sb.ToCString());
					return nullptr;
				}
			}
			else
			{
				i = cols.GetCount();
				while (i-- > 0)
				{
					col = cols.GetItem(i);
					col->name->Release();
					OPTSTR_DEL(col->asName);
					MemFree(col);
				}

				Text::StringBuilderUTF8 sb;
				sb.AppendC(UTF8STRC("#00000Unsupported syntax 'FROM'"));
				OPTSTR_DEL(sess->lastError);
				sess->lastError = Text::String::New(sb.ToCString());
				return nullptr;
			}
		}
	}
	else if (Text::StrStartsWithICaseC(sptr1, (UIntOS)(sqlEnd - sptr1), UTF8STRC("SHOW")) && Text::CharUtil::IsWS(sptr1 + 4))
	{
		sptr1 += 4;
		while (Text::CharUtil::PtrIsWS(sptr1));
		if (Text::StrStartsWithICaseC(sptr1, (UIntOS)(sqlEnd - sptr1), UTF8STRC("ENGINES")) && (Text::CharUtil::IsWS(sptr1 + 7) || sptr1[7] == 0))
		{
			////////////////////////
			Text::StringBuilderUTF8 sb;
			sb.AppendC(UTF8STRC("#42000You have an error in your SQL syntax; check the manual that corresponds to your MySQL server version for the right syntax to use near '"));
			sb.AppendSlow(sptr1);
			sb.AppendC(UTF8STRC("' at line 1"));
			OPTSTR_DEL(sess->lastError);
			sess->lastError = Text::String::New(sb.ToCString());
		}
		else if (Text::StrStartsWithICaseC(sptr1, (UIntOS)(sqlEnd - sptr1), UTF8STRC("CHARSET")) && (Text::CharUtil::IsWS(sptr1 + 7) || sptr1[7] == 0))
		{
			////////////////////////
			Text::StringBuilderUTF8 sb;
			sb.AppendC(UTF8STRC("#42000You have an error in your SQL syntax; check the manual that corresponds to your MySQL server version for the right syntax to use near '"));
			sb.AppendSlow(sptr1);
			sb.AppendC(UTF8STRC("' at line 1"));
			OPTSTR_DEL(sess->lastError);
			sess->lastError = Text::String::New(sb.ToCString());
		}
		else if (Text::StrStartsWithICaseC(sptr1, (UIntOS)(sqlEnd - sptr1), UTF8STRC("COLLATION")) && (Text::CharUtil::IsWS(sptr1 + 9) || sptr1[9] == 0))
		{
			////////////////////////
			Text::StringBuilderUTF8 sb;
			sb.AppendC(UTF8STRC("#42000You have an error in your SQL syntax; check the manual that corresponds to your MySQL server version for the right syntax to use near '"));
			sb.AppendSlow(sptr1);
			sb.AppendC(UTF8STRC("' at line 1"));
			OPTSTR_DEL(sess->lastError);
			sess->lastError = Text::String::New(sb.ToCString());
		}
		else if (Text::StrStartsWithICaseC(sptr1, (UIntOS)(sqlEnd - sptr1), UTF8STRC("VARIABLES")) && (Text::CharUtil::IsWS(sptr1 + 9) || sptr1[9] == 0))
		{
			sptr1 += 9;
			while (Text::CharUtil::PtrIsWS(sptr1));
			
			if (sptr1[0] == 0)
			{
				DB::DBMSReader *reader;
				IntOS i;
				IntOS j;
				Optional<Text::String> row[2];
				NN<Text::String> s;
				Text::StringBuilderUTF8 sb;
				NEW_CLASS(reader, DB::DBMSReader(2, -1));
				reader->SetColumn(0, CSTR("Variablename"), DB::DBUtil::CT_VarUTF8Char);
				reader->SetColumn(1, CSTR("Value"), DB::DBUtil::CT_VarUTF8Char);
				i = 0;
				j = sizeof(sysVarList) / sizeof(sysVarList[0]);
				while (i < j)
				{
					row[0] = s = Text::String::NewNotNullSlow((const UTF8Char*)sysVarList[i]);
					sb.ClearStr();
					SysVarGet(sb, sess, s->ToCString());
					row[1] = Text::String::New(sb.ToString(), sb.GetLength());
					reader->AddRow(row);
					i++;
				}
				return reader;
			}
			else if (Text::StrStartsWithICaseC(sptr1, (UIntOS)(sqlEnd - sptr1), UTF8STRC("LIKE")) && Text::CharUtil::IsWS(sptr1 + 4))
			{
				sptr1 += 4;
				while (Text::CharUtil::PtrIsWS(sptr1));

				if (sptr1[0] == '\'')
				{
					Bool valid = true;
 					Optional<Text::String> val = this->Evals(sptr1, sess, 0, 0, nullptr, valid);
					if (!valid)
					{
						OPTSTR_DEL(val);
						return nullptr;
					}

					while (Text::CharUtil::PtrIsWS(sptr1));
					NN<Text::String> s;
					if (sptr1[0] == 0 && val.SetTo(s))
					{
						DB::DBMSReader *reader;
						IntOS i;
						IntOS j;
						Optional<Text::String> row[2];
						Text::StringBuilderUTF8 sb;
						NEW_CLASS(reader, DB::DBMSReader(2, -1));
						reader->SetColumn(0, CSTR("Variablename"), DB::DBUtil::CT_VarUTF8Char);
						reader->SetColumn(1, CSTR("Value"), DB::DBUtil::CT_VarUTF8Char);
						i = 0;
						j = sizeof(sysVarList) / sizeof(sysVarList[0]);
						while (i < j)
						{
							if (StrLike((const UTF8Char*)sysVarList[i], s->v))
							{
								sb.ClearStr();
								row[0] = s = Text::String::NewNotNullSlow((const UTF8Char*)sysVarList[i]);
								SysVarGet(sb, sess, s->ToCString());
								row[1] = Text::String::New(sb.ToString(), sb.GetLength());
								reader->AddRow(row);
							}
							i++;
						}
						OPTSTR_DEL(val);
						return reader;

					}
					else
					{
						OPTSTR_DEL(val);

						Text::StringBuilderUTF8 sb;
						sb.AppendC(UTF8STRC("#42000You have an error in your SQL syntax; check the manual that corresponds to your MySQL server version for the right syntax to use near '"));
						sb.AppendSlow(sptr1);
						sb.AppendC(UTF8STRC("' at line 1"));
						OPTSTR_DEL(sess->lastError);
						sess->lastError = Text::String::New(sb.ToCString());
						
						return nullptr;
					}
				}
				else
				{
					Text::StringBuilderUTF8 sb;
					sb.AppendC(UTF8STRC("#42000You have an error in your SQL syntax; check the manual that corresponds to your MySQL server version for the right syntax to use near '"));
					sb.AppendSlow(sptr1);
					sb.AppendC(UTF8STRC("' at line 1"));
					OPTSTR_DEL(sess->lastError);
					sess->lastError = Text::String::New(sb.ToCString());
				}
			}
			else
			{
				Text::StringBuilderUTF8 sb;
				sb.AppendC(UTF8STRC("#42000You have an error in your SQL syntax; check the manual that corresponds to your MySQL server version for the right syntax to use near '"));
				sb.AppendSlow(sptr1);
				sb.AppendC(UTF8STRC("' at line 1"));
				OPTSTR_DEL(sess->lastError);
				sess->lastError = Text::String::New(sb.ToCString());
			}
		}
		else
		{
			Text::StringBuilderUTF8 sb;
			sb.AppendC(UTF8STRC("#42000You have an error in your SQL syntax; check the manual that corresponds to your MySQL server version for the right syntax to use near '"));
			sb.AppendSlow(sptr1);
			sb.AppendC(UTF8STRC("' at line 1"));
			OPTSTR_DEL(sess->lastError);
			sess->lastError = Text::String::New(sb.ToCString());
		}
	}
	else
	{
		Text::StringBuilderUTF8 sb;
		sb.AppendC(UTF8STRC("#42000You have an error in your SQL syntax; check the manual that corresponds to your MySQL server version for the right syntax to use near '"));
		sb.AppendSlow(sptr1);
		sb.AppendC(UTF8STRC("' at line 1"));
		OPTSTR_DEL(sess->lastError);
		sess->lastError = Text::String::New(sb.ToCString());
	}
	return nullptr;
}

void DB::DBMS::CloseReader(NN<DB::DBReader> r)
{

}

UnsafeArray<UTF8Char> DB::DBMS::GetErrMessage(Int32 sessId, UnsafeArray<UTF8Char> msgBuff)
{
	NN<DB::DBMS::SessionInfo> sess;
	NN<Text::String> lastError;
	Sync::MutexUsage mutUsage(this->sessMut);
	if (this->sessMap.Get(sessId).SetTo(sess) && sess->lastError.SetTo(lastError))
	{
		msgBuff = lastError->ConcatTo(msgBuff);
	}
	mutUsage.EndUse();
	return msgBuff;
}

Optional<DB::DBMS::SessionInfo> DB::DBMS::SessGet(Int32 sessId)
{
	Sync::MutexUsage mutUsage(this->sessMut);
	return this->sessMap.Get(sessId);
}

void DB::DBMS::SessEnd(Int32 sessId)
{
	NN<DB::DBMS::SessionInfo> sess;
	if (sessId == 0)
	{
		return;
	}
	Sync::MutexUsage mutUsage(this->sessMut);
	if (this->sessMap.Remove(sessId).SetTo(sess))
	{
		mutUsage.EndUse();
		this->SessDelete(sess);
	}
}

void DB::DBMS::SessDelete(NN<DB::DBMS::SessionInfo> sess)
{
	NN<const Data::ArrayListNN<Text::String>> varList = sess->userVars.GetValues();
	UIntOS i = varList->GetCount();
	NN<Text::String> var;
	while (i-- > 0)
	{
		var = varList->GetItemNoCheck(i);
		var->Release();
	}
	OPTSTR_DEL(sess->lastError);
	OPTSTR_DEL(sess->database);
	sess.Delete();
}
