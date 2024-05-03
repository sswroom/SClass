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
		UOSInt colCount;
		OSInt rowChanged;
		Data::ArrayList<Text::String **> *rows;
		DB::DBUtil::ColType *colTypes;
		const UTF8Char **colNames;
		OSInt rowIndex;
	public:
		DBMSReader(UOSInt colCount, OSInt rowChanged)
		{
			this->colCount = colCount;
			this->rowChanged = rowChanged;
			if (this->rowChanged == -1)
			{
				NEW_CLASS(this->rows, Data::ArrayList<Text::String**>());
				this->colTypes = MemAlloc(DB::DBUtil::ColType, this->colCount);
				this->colNames = MemAlloc(const UTF8Char*, this->colCount);
				UOSInt i;
				i = 0;
				while (i < this->colCount)
				{
					this->colTypes[i] = DB::DBUtil::CT_VarUTF8Char;
					this->colNames[i] = 0;
					i++;
				}
			}
			else
			{
				this->rows = 0;
				this->colTypes = 0;
				this->colNames = 0;
			}
			this->rowIndex = -1;
		}

		virtual ~DBMSReader()
		{
			if (this->rows)
			{
				UOSInt i;
				UOSInt j;
				Text::String **row;
				i = this->rows->GetCount();
				while (i-- > 0)
				{
					row = this->rows->GetItem(i);
					j = this->colCount;
					while (j-- > 0)
					{
						SDEL_STRING(row[j]);
					}
					MemFree(row);
				}
				i = this->colCount;
				while (i-- > 0)
				{
					SDEL_TEXT(this->colNames[i]);
				}
				MemFree(this->colNames);
				MemFree(this->colTypes);
			}
		}

		void AddRow(Text::String **row)
		{
			if (this->rows == 0)
			{
				return;
			}
			Text::String **newRow = MemAlloc(Text::String*, this->colCount);
			MemCopyNO(newRow, row, sizeof(Text::String*) * this->colCount);
			this->rows->Add(newRow);
		}

		void SetColumn(UOSInt colIndex, Text::CString colName, DB::DBUtil::ColType colType)
		{
			if (this->rows && colIndex < this->colCount)
			{
				SDEL_TEXT(this->colNames[colIndex]);
				this->colNames[colIndex] = Text::StrCopyNewC(colName.v, colName.leng).Ptr();
				this->colTypes[colIndex] = colType;
			}
		}

		virtual Bool ReadNext()
		{
			if (this->rows == 0)
			{
				return false;
			}
			if (this->rowIndex + 1 < (OSInt)this->rows->GetCount())
			{
				this->rowIndex++;
				return true;
			}
			return false;
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
			if (this->rows == 0 || colIndex >= this->colCount)
				return 0;
			Text::String **row = this->rows->GetItem((UOSInt)this->rowIndex);
			if (row == 0 || row[colIndex] == 0)
				return 0;
			return row[colIndex]->ToInt32();
		}

		virtual Int64 GetInt64(UOSInt colIndex)
		{
			if (this->rows == 0 || colIndex >= this->colCount)
				return 0;
			Text::String **row = this->rows->GetItem((UOSInt)this->rowIndex);
			if (row == 0 || row[colIndex] == 0)
				return 0;
			return row[colIndex]->ToInt64();
		}

		virtual WChar *GetStr(UOSInt colIndex, WChar *buff)
		{
			if (this->rows == 0 || colIndex >= this->colCount)
				return 0;
			Text::String **row = this->rows->GetItem((UOSInt)this->rowIndex);
			if (row == 0 || row[colIndex] == 0)
				return 0;
			return Text::StrUTF8_WCharC(buff, row[colIndex]->v, row[colIndex]->leng, 0);
		}

		virtual Bool GetStr(UOSInt colIndex, NN<Text::StringBuilderUTF8> sb)
		{
			if (this->rows == 0 || colIndex >= this->colCount)
				return false;
			Text::String **row = this->rows->GetItem((UOSInt)this->rowIndex);
			if (row == 0 || row[colIndex] == 0)
				return false;
			sb->Append(row[colIndex]);
			return true;
		}

		virtual Optional<Text::String> GetNewStr(UOSInt colIndex)
		{
			if (this->rows == 0 || colIndex >= this->colCount)
				return 0;
			Text::String **row = this->rows->GetItem((UOSInt)this->rowIndex);
			if (row == 0 || row[colIndex] == 0)
				return 0;
			return row[colIndex]->Clone();
		}

		virtual UTF8Char *GetStr(UOSInt colIndex, UTF8Char *buff, UOSInt buffSize)
		{
			if (this->rows == 0 || colIndex >= this->colCount)
				return 0;
			Text::String **row = this->rows->GetItem((UOSInt)this->rowIndex);
			if (row == 0 || row[colIndex] == 0)
				return 0;
			return Text::StrConcatS(buff, row[colIndex]->v, buffSize);
		}

		virtual Data::Timestamp GetTimestamp(UOSInt colIndex)
		{
			if (this->rows == 0 || colIndex >= this->colCount)
				return Data::Timestamp(0);
			Text::String **row = this->rows->GetItem((UOSInt)this->rowIndex);
			if (row == 0)
				return Data::Timestamp(0);
			if (row[colIndex] == 0)
				return Data::Timestamp(0);
			return Data::Timestamp(row[colIndex]->ToCString(), 0);
		}

		virtual Double GetDbl(UOSInt colIndex)
		{
			if (this->rows == 0 || colIndex >= this->colCount)
				return 0;
			Text::String **row = this->rows->GetItem((UOSInt)this->rowIndex);
			if (row == 0 || row[colIndex] == 0)
				return 0;
			return row[colIndex]->ToDouble();
		}

		virtual Bool GetBool(UOSInt colIndex)
		{
			if (this->rows == 0 || colIndex >= this->colCount)
				return false;
			Text::String **row = this->rows->GetItem((UOSInt)this->rowIndex);
			if (row == 0 || row[colIndex] == 0)
				return false;
			return row[colIndex]->ToInt32() != 0;
		}

		virtual UOSInt GetBinarySize(UOSInt colIndex)
		{
			if (this->rows == 0 || colIndex >= this->colCount)
				return 0;
			Text::String **row = this->rows->GetItem((UOSInt)this->rowIndex);
			if (row == 0 || row[colIndex] == 0)
				return 0;
			return row[colIndex]->leng;
		}

		virtual UOSInt GetBinary(UOSInt colIndex, UInt8 *buff)
		{
			if (this->rows == 0 || colIndex >= this->colCount)
				return 0;
			Text::String **row = this->rows->GetItem((UOSInt)this->rowIndex);
			if (row == 0 || row[colIndex] == 0)
				return 0;
			UOSInt cnt = row[colIndex]->leng;
			MemCopyNO(buff, row[colIndex]->v, cnt);
			return cnt;
		}

		virtual Optional<Math::Geometry::Vector2D> GetVector(UOSInt colIndex)
		{
			return 0;
		}

		virtual Bool GetUUID(UOSInt colIndex, NN<Data::UUID> uuid)
		{
			return false;
		}

		virtual Bool IsNull(UOSInt colIndex)
		{
			if (this->rows == 0 || colIndex >= this->colCount)
				return true;
			Text::String **row = this->rows->GetItem((UOSInt)this->rowIndex);
			if (row == 0 || row[colIndex] == 0)
				return true;
			return false;
		}

		virtual UTF8Char *GetName(UOSInt colIndex, UTF8Char *buff)
		{
			if (this->rowChanged != -1)
				return 0;
			if (colIndex >= this->colCount)
				return 0;
			if (this->colNames[colIndex])
			{
				return Text::StrConcat(buff, this->colNames[colIndex]);
			}
			else
			{
				return Text::StrUOSInt(Text::StrConcatC(buff, UTF8STRC("column")), colIndex + 1);
			}

		}

		virtual DB::DBUtil::ColType GetColType(UOSInt colIndex, OptOut<UOSInt> colSize)
		{
			if (this->rowChanged != -1)
				return DB::DBUtil::CT_Unknown;
			if (colIndex >= this->colCount)
				return DB::DBUtil::CT_Unknown;
			colSize.Set(256);
			return this->colTypes[colIndex];
		}

		virtual Bool GetColDef(UOSInt colIndex, NN<DB::ColDef> colDef)
		{
			UTF8Char sbuff[256];
			UTF8Char *sptr;
			if (this->rowChanged != -1)
				return false;
			if (colIndex >= this->colCount)
				return false;
			if (this->colNames[colIndex])
			{
				colDef->SetColName(NN<const UTF8Char>::FromPtr(this->colNames[colIndex]));
			}
			else
			{
				sptr = Text::StrUOSInt(Text::StrConcatC(sbuff, UTF8STRC("column")), colIndex + 1);
				colDef->SetColName(CSTRP(sbuff, sptr));
			}
			colDef->SetColType(this->colTypes[colIndex]);
			colDef->SetColSize(256);
			return true;
		}

		virtual void DelNewStr(const UTF8Char *s)
		{
			Text::StrDelNew(s);
		}
	};
}

const UTF8Char *DB::DBMS::SQLParseName(UTF8Char *nameBuff, const UTF8Char *sql)
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
					return 0;
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
				const UTF8Char *sptr = sql;
				while (Text::CharUtil::PtrIsWS(&sptr));
				if (sptr[0] == '+' || sptr[0] == '-' || sptr[0] == '*' || sptr[0] == '/')
				{
					sptr++;
					nameBuff = Text::StrConcatC(nameBuff, sql, (UOSInt)(sptr - sql));
					sql = sptr;

					while (Text::CharUtil::PtrIsWS(&sptr));
					if (sptr > sql)
					{
						nameBuff = Text::StrConcatC(nameBuff, sql, (UOSInt)(sptr - sql));
						sql = sptr;
					}
					return SQLParseName(nameBuff, sql);
				}
				else
				{
					return 0;
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
				return 0;
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
				return 0;
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
				return 0;
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
				return 0;
			}
			*nameBuff++ = c;
		}
		else if (c == '*')
		{
			if (!isSep)
			{
				return 0;
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
				return 0;
			}
		}
		else if (c == 0 || c == ',')
		{
			if (isSep)
			{
				return 0;
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
			const UTF8Char *sptr = sql;
			while (Text::CharUtil::PtrIsWS(&sptr));
			if (sptr[0] == '(')
			{
				sptr++;
				MemCopyNO(nameBuff, sql, (UOSInt)(sptr - sql));
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
					sql = SQLParseName(nameBuff, sql);
					if (sql == 0)
						return 0;
					while (Text::CharUtil::PtrIsWS(&sql));
					MemCopyNO(nameBuff, sptr, (UOSInt)(sql - sptr));
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
						while (Text::CharUtil::PtrIsWS(&sql));
						if (sptr != sql)
						{
							MemCopyNO(nameBuff, sptr, (UOSInt)(sql - sptr));
							nameBuff += sql - sptr;
						}
					}
					else
					{
						return 0;
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
			return 0;
		}
	}
}

Bool DB::DBMS::StrLike(const UTF8Char *val, const UTF8Char *likeStr)
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
	printf("SysVarExist: %s\r\n", varName.v);
#endif
	OSInt i = 0;
	OSInt j = sizeof(sysVarList) / sizeof(sysVarList[0]) - 1;
	OSInt k;
	OSInt l;
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

void DB::DBMS::SysVarColumn(DB::DBMSReader *reader, UOSInt colIndex, const UTF8Char *varName, Text::CString colName)
{
	UTF8Char sbuff[128];
	UTF8Char *sptr;
	if (colName.leng == 0)
	{
		sptr = Text::StrConcat(Text::StrConcatC(sbuff, UTF8STRC("@@")), varName);
		colName = CSTRP(sbuff, sptr);
	}
	UOSInt varNameLen = Text::StrCharCnt(varName);
	
	if (Text::StrEqualsICaseC(varName, varNameLen, UTF8STRC("autocommit")))
	{
		reader->SetColumn(colIndex, colName, DB::DBUtil::CT_Bool);
		return;
	}
	else if (Text::StrEqualsICaseC(varName, varNameLen, UTF8STRC("auto_increment_increment")))
	{
		reader->SetColumn(colIndex, colName, DB::DBUtil::CT_Int32);
		return;
	}
	else if (Text::StrEqualsICaseC(varName, varNameLen, UTF8STRC("character_set_server")))
	{
		reader->SetColumn(colIndex, colName, DB::DBUtil::CT_VarUTF8Char);
		return;
	}
	else if (Text::StrEqualsICaseC(varName, varNameLen, UTF8STRC("collation_server")))
	{
		reader->SetColumn(colIndex, colName, DB::DBUtil::CT_VarUTF8Char);
		return;
	}
	else if (Text::StrEqualsICaseC(varName, varNameLen, UTF8STRC("lower_case_table_names")))
	{
		reader->SetColumn(colIndex, colName, DB::DBUtil::CT_Int32);
		return;
	}
	else if (Text::StrEqualsICaseC(varName, varNameLen, UTF8STRC("max_allowed_packet")))
	{
		reader->SetColumn(colIndex, colName, DB::DBUtil::CT_Int32);
		return;
	}
	else if (Text::StrEqualsICaseC(varName, varNameLen, UTF8STRC("sql_mode")))
	{
		reader->SetColumn(colIndex, colName, DB::DBUtil::CT_VarUTF8Char);
		return;
	}
	else if (Text::StrEqualsICaseC(varName, varNameLen, UTF8STRC("system_time_zone")))
	{
		reader->SetColumn(colIndex, colName, DB::DBUtil::CT_VarUTF8Char);
		return;
	}
	else if (Text::StrEqualsICaseC(varName, varNameLen, UTF8STRC("time_zone")))
	{
		reader->SetColumn(colIndex, colName, DB::DBUtil::CT_VarUTF8Char);
		return;
	}
	reader->SetColumn(colIndex, colName, DB::DBUtil::CT_VarUTF8Char);
}

Bool DB::DBMS::SysVarSet(NN<DB::DBMS::SessionInfo> sess, Bool isGlobal, Text::CStringNN varName, Text::String *val)
{
	if (varName.EqualsICase(UTF8STRC("autocommit")))
	{
		Bool v;
		if (val == 0)
		{
			v = false;
		}
		else
		{
			v = val->ToInt32() != 0;
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
		if (val == 0)
		{
			v = 1;
		}
		else
		{
			v = val->ToInt32();
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
		UOSInt i;
		sb.Append(val);
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
	if (sess->userVars.Get(varName).SetTo(val))
	{
		sb->Append(val);
		return true;
	}
	return false;
}

void DB::DBMS::UserVarColumn(DB::DBMSReader *reader, UOSInt colIndex, const UTF8Char *varName, Text::CString colName)
{
	UTF8Char sbuff[128];
	UTF8Char *sptr;
	if (colName.leng == 0)
	{
		sptr = Text::StrConcat(Text::StrConcatC(sbuff, UTF8STRC("@")), varName);
		colName = CSTRP(sbuff, sptr);
	}

	reader->SetColumn(colIndex, colName, DB::DBUtil::CT_VarUTF8Char);
}

Bool DB::DBMS::UserVarSet(NN<DB::DBMS::SessionInfo> sess, Text::CStringNN varName, Optional<Text::String> val)
{
	NN<Text::String> nnval;
	if (val.SetTo(nnval))
	{
		if (sess->userVars.Put(varName, nnval->Clone()).SetTo(nnval))
			nnval->Release();
	}
	else
	{
		if (sess->userVars.Remove(varName).SetTo(nnval))
			nnval->Release();
	}
	return true;
}

Text::String *DB::DBMS::Evals(const UTF8Char **valPtr, NN<DB::DBMS::SessionInfo> sess, DB::DBMSReader *reader, UOSInt colIndex, Text::CString colName, Bool *valid)
{
	const UTF8Char *val = *valPtr;
	if (Text::StrStartsWith(val, (const UTF8Char*)"@@"))
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
				while (Text::CharUtil::PtrIsWS(&val));
				if (val[0] == 0 || val[0] == ',')
				{
					break;
				}

				*valid = false;

				sb.AppendC(UTF8STRC("#HY000Unknown system variable '"));
				sb.AppendSlow(val);
				sb.AppendC(UTF8STRC("'"));
				SDEL_STRING(sess->lastError);
				sess->lastError = Text::String::New(sb.ToCString()).Ptr();
				return 0;
			}
		}
		*valPtr = val;
		if (reader)
		{
			this->SysVarColumn(reader, colIndex, sb2.ToString(), colName);
		}
		if (this->SysVarGet(sb, sess, sb2.ToCString()) == 0)
		{
			*valid = false;

			sb.AppendC(UTF8STRC("#HY000Unknown system variable '"));
			sb.AppendC(sb2.ToString(), sb2.GetLength());
			sb.AppendC(UTF8STRC("'"));
			SDEL_STRING(sess->lastError);
			sess->lastError = Text::String::New(sb.ToCString()).Ptr();
			return 0;
		}
		else
		{
			return Text::String::New(sb.ToString(), sb.GetLength()).Ptr();
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
			return 0;
		}
		else
		{
			*valPtr = s.GetEndPtr();
			return Text::String::New(sb.ToString(), sb.GetLength()).Ptr();
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
					*valPtr = val;
					if (reader)
						reader->SetColumn(colIndex, (colName.leng > 0)?colName:sb.ToCString(), DB::DBUtil::CT_VarUTF8Char);
					return Text::String::New(sb.ToString(), sb.GetLength()).Ptr();
				}
			}
			else if (c == 0)
			{
				*valid = false;
				return 0;
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
					*valPtr = val;
					if (reader)
						reader->SetColumn(colIndex, (colName.leng > 0)?colName:sb.ToCString(), DB::DBUtil::CT_VarUTF8Char);
					return Text::String::New(sb.ToString(), sb.GetLength()).Ptr();
				}
			}
			else if (c == 0)
			{
				*valid = false;
				return 0;
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
		const UTF8Char *sptr = val;
		Text::String *val2;
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
		while (Text::CharUtil::PtrIsWS(&sptr));
		if (*sptr == 0 || *sptr == ',')
		{
			if (reader)
				reader->SetColumn(colIndex, (colName.leng > 0)?colName:Text::CString::FromPtr(val), isDbl?DB::DBUtil::CT_Double:DB::DBUtil::CT_Int32);
			*valPtr = sptr;
			return Text::String::New(sb.ToString(), sb.GetLength()).Ptr();
		}
		if (*sptr == '+')
		{
			sptr++;			
			while (Text::CharUtil::PtrIsWS(&sptr));
			val2 = this->Evals(&sptr, sess, 0, 0, CSTR_NULL, valid);
			if (val2 == 0)
			{
				*valid = false;
				return 0;
			}
			if (!isDbl)
			{
				Int32 iVal;
				if (val2->ToInt32(iVal))
				{
					iVal += Text::StrToInt32(sb.ToString());
					sb.ClearStr();
					sb.AppendI32(iVal);
					if (reader)
						reader->SetColumn(colIndex, (colName.leng > 0)?colName:Text::CString::FromPtr(val), DB::DBUtil::CT_Int32);
					*valPtr = sptr;
					val2->Release();
					return Text::String::New(sb.ToString(), sb.GetLength()).Ptr();
				}
			}
			Double dVal;
			if (val2->ToDouble(dVal))
			{
				dVal += Text::StrToDouble(sb.ToString());
				sb.ClearStr();
				sb.AppendDouble(dVal);
				if (reader)
					reader->SetColumn(colIndex, (colName.leng > 0)?colName:Text::CString::FromPtr(val), DB::DBUtil::CT_Double);
				*valPtr = sptr;
				val2->Release();
				return Text::String::New(sb.ToString(), sb.GetLength()).Ptr();
			}
			else
			{
				////////////////////////////////
				*valid = false;
				val2->Release();
				return 0;
			}
		}
		else if (*sptr == '-')
		{
			sptr++;			
			while (Text::CharUtil::PtrIsWS(&sptr));
			val2 = this->Evals(&sptr, sess, 0, 0, CSTR_NULL, valid);
			if (val2 == 0)
			{
				*valid = false;
				return 0;
			}
			if (!isDbl)
			{
				Int32 iVal;
				if (val2->ToInt32(iVal))
				{
					iVal = Text::StrToInt32(sb.ToString()) - iVal;
					sb.ClearStr();
					sb.AppendI32(iVal);
					if (reader)
						reader->SetColumn(colIndex, (colName.leng > 0)?colName:Text::CString::FromPtr(val), DB::DBUtil::CT_Int32);
					*valPtr = sptr;
					val2->Release();
					return Text::String::New(sb.ToString(), sb.GetLength()).Ptr();
				}
			}
			Double dVal;
			if (val2->ToDouble(dVal))
			{
				dVal = Text::StrToDouble(sb.ToString()) - dVal;
				sb.ClearStr();
				sb.AppendDouble(dVal);
				if (reader)
					reader->SetColumn(colIndex, (colName.leng > 0)?colName:Text::CString::FromPtr(val), DB::DBUtil::CT_Double);
				*valPtr = sptr;
				val2->Release();
				return Text::String::New(sb.ToString(), sb.GetLength()).Ptr();
			}
			else
			{
				*valid = false;
				val2->Release();
				////////////////////////////////
				return 0;
			}
		}
		else if (*sptr == '*')
		{
			sptr++;			
			while (Text::CharUtil::PtrIsWS(&sptr));
			val2 = this->Evals(&sptr, sess, 0, 0, CSTR_NULL, valid);
			if (val2 == 0)
			{
				*valid = false;
				return 0;
			}
			if (!isDbl)
			{
				Int32 iVal;
				if (val2->ToInt32(iVal))
				{
					iVal = Text::StrToInt32(sb.ToString()) * iVal;
					sb.ClearStr();
					sb.AppendI32(iVal);
					if (reader)
						reader->SetColumn(colIndex, (colName.leng > 0)?colName:Text::CString::FromPtr(val), DB::DBUtil::CT_Int32);
					*valPtr = sptr;
					val2->Release();
					return Text::String::New(sb.ToString(), sb.GetLength()).Ptr();
				}
			}
			Double dVal;
			if (val2->ToDouble(dVal))
			{
				dVal = Text::StrToDouble(sb.ToString()) * dVal;
				sb.ClearStr();
				sb.AppendDouble(dVal);
				if (reader)
					reader->SetColumn(colIndex, (colName.leng > 0)?colName:Text::CString::FromPtr(val), DB::DBUtil::CT_Double);
				*valPtr = sptr;
				val2->Release();
				return Text::String::New(sb.ToString(), sb.GetLength()).Ptr();
			}
			else
			{
				*valid = false;
				val2->Release();
				////////////////////////////////
				return 0;
			}
		}
		else if (*sptr == '/')
		{
			sptr++;			
			while (Text::CharUtil::PtrIsWS(&sptr));
			val2 = this->Evals(&sptr, sess, 0, 0, CSTR_NULL, valid);
			if (val2 == 0)
			{
				*valid = false;
				return 0;
			}
			if (!isDbl)
			{
				Int32 iVal;
				if (val2->ToInt32(iVal))
				{
					iVal = Text::StrToInt32(sb.ToString()) / iVal;
					sb.ClearStr();
					sb.AppendI32(iVal);
					if (reader)
						reader->SetColumn(colIndex, (colName.leng > 0)?colName:Text::CString::FromPtr(val), DB::DBUtil::CT_Int32);
					*valPtr = sptr;
					val2->Release();
					return Text::String::New(sb.ToString(), sb.GetLength()).Ptr();
				}
			}
			Double dVal;
			if (val2->ToDouble(dVal))
			{
				dVal = Text::StrToDouble(sb.ToString()) / dVal;
				sb.ClearStr();
				sb.AppendDouble(dVal);
				if (reader)
					reader->SetColumn(colIndex, (colName.leng > 0)?colName:Text::CString::FromPtr(val), DB::DBUtil::CT_Double);
				*valPtr = sptr;
				val2->Release();
				return Text::String::New(sb.ToString(), sb.GetLength()).Ptr();
			}
			else
			{
				*valid = false;
				////////////////////////////////
				val2->Release();
				return 0;
			}
		}
		else
		{
			*valid = false;
			return 0;
		}
	}
	else
	{
		Text::StringBuilderUTF8 sb;
		const UTF8Char *sptr = val;
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
				while (Text::CharUtil::PtrIsWS(&sptr));
				if (sptr[0] == '(')
				{
					if (sb.EqualsICase(UTF8STRC("CONCAT")))
					{
						sb.ClearStr();
						Text::String *sVal;
						Bool v = true;
						sptr++;
						while (true)
						{
							while (Text::CharUtil::PtrIsWS(&sptr));
							sVal = this->Evals(&sptr, sess, 0, 0, CSTR_NULL, &v);
							if (!v)
							{
								*valid = false;
								return 0;
							}
							sb.Append(sVal);
							sVal->Release();
							while (Text::CharUtil::PtrIsWS(&sptr));
							if (sptr[0] == ')')
							{
								if (reader)
									reader->SetColumn(colIndex, (colName.leng > 0)?colName:Text::CString::FromPtr(val), DB::DBUtil::CT_Double);
								*valPtr = sptr + 1;
								return Text::String::New(sb.ToString(), sb.GetLength()).Ptr();
							}
							else if (sptr[0] == ',')
							{
								sptr++;
							}
							else
							{
								*valid = false;

								sb.ClearStr();
								sb.AppendC(UTF8STRC("#42S22Unknown column '"));
								sb.AppendSlow(val);
								sb.AppendC(UTF8STRC("' in field list"));
								SDEL_STRING(sess->lastError);
								sess->lastError = Text::String::New(sb.ToCString()).Ptr();

								return 0;
							}
						}
					}
					else if (sb.EqualsICase(UTF8STRC("DATABASE")))
					{
						sptr++;
						while (Text::CharUtil::PtrIsWS(&sptr));
						if (sptr[0] != ')')
						{
							*valid = false;

							sb.ClearStr();
							sb.AppendC(UTF8STRC("#42S22Unknown column '"));
							sb.AppendSlow(sptr);
							sb.AppendC(UTF8STRC("' in field list"));
							SDEL_STRING(sess->lastError);
							sess->lastError = Text::String::New(sb.ToCString()).Ptr();

							return 0;
						}
						else
						{
							*valPtr = sptr + 1;
							if (sess->database)
							{
								return sess->database->Clone().Ptr();
							}
							else
							{
								return 0;
							}
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
		*valid = false;

		sb.ClearStr();
		sb.AppendC(UTF8STRC("#42S22Unknown column '"));
		sb.AppendSlow(val);
		sb.AppendC(UTF8STRC("' in field list"));
		SDEL_STRING(sess->lastError);
		sess->lastError = Text::String::New(sb.ToCString()).Ptr();

		return 0;
	}
}

DB::DBMS::DBMS(Text::CString versionStr, NN<IO::LogTool> log)
{
	this->versionStr = Text::String::New(versionStr);
	this->log = log;
}

DB::DBMS::~DBMS()
{
	this->versionStr->Release();
	NN<DB::DBMS::LoginInfo> login;
	NN<DB::DBMS::UserInfo> user;
	UOSInt i = this->loginMap.GetCount();
	UOSInt j;
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

Bool DB::DBMS::UserAdd(Int32 userId, Text::CStringNN userName, Text::CString password, Text::CString host)
{
	NN<DB::DBMS::LoginInfo> login;
	NN<DB::DBMS::UserInfo> user;
	UOSInt i;
	Bool succ;
	#if defined(VERBOSE)
	printf("UserAdd %s/%s@%s\r\n", userName.v, password.v, host.v);
	#endif
	Sync::MutexUsage mutUsage(this->loginMut);
	if (!this->loginMap.GetC(userName).SetTo(login))
	{
		NEW_CLASSNN(login, DB::DBMS::LoginInfo());
		login->login = Text::String::New(userName);
		this->loginMap.PutC(userName, login);

		user = MemAllocNN(DB::DBMS::UserInfo);
		user->hostLen = (UOSInt)(host.ConcatTo(user->host) - user->host);
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
			user->hostLen = (UOSInt)(host.ConcatTo(user->host) - user->host);
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

Int32 DB::DBMS::UserLoginMySQL(Int32 sessId, Text::CStringNN userName, const UInt8 *randomData, const UInt8 *passHash, NN<const Net::SocketUtil::AddressInfo> addr, const DB::DBMS::SessionParam *param, const UTF8Char *database)
{
	NN<DB::DBMS::LoginInfo> login;
	NN<DB::DBMS::UserInfo> user;
	UInt8 hashBuff[20];
	UOSInt j;
	Int32 userId = 0;
	#if defined(VERBOSE)
	printf("mysql_native_password auth\r\n");
	#endif
	Sync::MutexUsage mutUsage(this->loginMut);
	if (this->loginMap.GetC(userName).SetTo(login))
	{
		UOSInt i = login->userList.GetCount();
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
				hashBuff[j] ^= user->pwdSha1[j];
				j++;
			}

			#if defined(VERBOSE)
			Text::StringBuilderUTF8 sb;
			sb.AppendHexBuff(hashBuff, 20, ' ', Text::LineBreakType::None);
			printf("Password Hash = %s\r\n", sb.ToString());
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
					sess->lastError = 0;
					sess->autoCommit = 1;
					sess->autoIncInc = 1;
					sess->sqlModes = (DB::DBMS::SQLMODE)(SQLM_ONLY_FULL_GROUP_BY | SQLM_STRICT_TRANS_TABLES | SQLM_NO_ZERO_IN_DATE | SQLM_NO_ZERO_DATE | SQLM_ERROR_FOR_DIVISION_BY_ZERO | SQLM_NO_ENGINE_SUBSTITUTION);
					sess->database = 0;
					if (database && database[0])
					{
						sess->database = Text::String::NewNotNullSlow(database).Ptr();
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

DB::DBReader *DB::DBMS::ExecuteReader(Int32 sessId, const UTF8Char *sql, UOSInt sqlLen)
{
	const UTF8Char *sptr1;
	const UTF8Char *sptr2;
	const UTF8Char *sptr3;
	const UTF8Char *sqlEnd = sql + sqlLen;
	UOSInt i;
	UOSInt j;
	UTF8Char nameBuff[128];
	UTF8Char nameBuff2[128];
	NN<DB::DBMS::SessionInfo> sess;
	if (!this->SessGet(sessId).SetTo(sess))
	{
		return 0;
	}
	sptr1 = sql;
	while (Text::CharUtil::PtrIsWS(&sptr1));
	if (Text::StrStartsWithICaseC(sptr1, (UOSInt)(sqlEnd - sptr1), UTF8STRC("SELECT")) && Text::CharUtil::IsWS(sptr1 + 6))
	{
		sptr1 += 6;
		Data::ArrayList<DB::DBMS::SQLColumn*> cols;
		DB::DBMS::SQLColumn *col;
		Bool hasFrom = false;

		while (true)
		{
			while (Text::CharUtil::PtrIsWS(&sptr1));
			sptr2 = sptr1;
			if ((sptr1 = SQLParseName(nameBuff, sptr1)) == 0)
			{
				i = cols.GetCount();
				while (i-- > 0)
				{
					col = cols.GetItem(i);
					col->name->Release();
					SDEL_STRING(col->asName);
					MemFree(col);
				}

				Text::StringBuilderUTF8 sb;
				sb.AppendC(UTF8STRC("#42000You have an error in your SQL syntax; check the manual that corresponds to your MySQL server version for the right syntax to use near '"));
				sb.AppendC(sptr2, (UOSInt)(sqlEnd - sptr2));
				sb.AppendC(UTF8STRC("' at line 1"));
				SDEL_STRING(sess->lastError);
				sess->lastError = Text::String::New(sb.ToCString()).Ptr();
				return 0;
			}
			while (Text::CharUtil::PtrIsWS(&sptr1));
			if (*sptr1 == ',' || *sptr1 == 0)
			{
				col = MemAlloc(DB::DBMS::SQLColumn, 1);
				col->name = Text::String::NewNotNullSlow(nameBuff);
				col->asName = 0;
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
			else if (Text::StrStartsWithICaseC(sptr1, (UOSInt)(sqlEnd - sptr1), UTF8STRC("AS")) && Text::CharUtil::IsWS(sptr1 + 2))
			{
				sptr1 += 2;
				while (Text::CharUtil::PtrIsWS(&sptr1));
				sptr3 = sptr1;
				if ((sptr1 = SQLParseName(nameBuff2, sptr1)) == 0)
				{
					i = cols.GetCount();
					while (i-- > 0)
					{
						col = cols.GetItem(i);
						col->name->Release();
						SDEL_STRING(col->asName);
						MemFree(col);
					}

					Text::StringBuilderUTF8 sb;
					sb.AppendC(UTF8STRC("#42000You have an error in your SQL syntax; check the manual that corresponds to your MySQL server version for the right syntax to use near '"));
					sb.AppendC(sptr3, (UOSInt)(sqlEnd - sptr3));
					sb.AppendC(UTF8STRC("' at line 1"));
					SDEL_STRING(sess->lastError);
					sess->lastError = Text::String::New(sb.ToCString()).Ptr();
					return 0;
				}
				while (Text::CharUtil::PtrIsWS(&sptr1));
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
				else if (Text::StrStartsWithICaseC(sptr1, (UOSInt)(sqlEnd - sptr1), UTF8STRC("FROM")) && Text::CharUtil::IsWS(sptr1 + 4))
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
						SDEL_STRING(col->asName);
						MemFree(col);
					}

					Text::StringBuilderUTF8 sb;
					sb.AppendC(UTF8STRC("#42000You have an error in your SQL syntax; check the manual that corresponds to your MySQL server version for the right syntax to use near '"));
					sb.AppendC(sptr1, (UOSInt)(sqlEnd - sptr1));
					sb.AppendC(UTF8STRC("' at line 1"));
					SDEL_STRING(sess->lastError);
					sess->lastError = Text::String::New(sb.ToCString()).Ptr();
					return 0;
				}
			}
			else if (Text::StrStartsWithICaseC(sptr1, (UOSInt)(sqlEnd - sptr1), UTF8STRC("FROM")) && Text::CharUtil::IsWS(sptr1 + 4))
			{
				hasFrom = true;
				sptr1 += 4;
				break;
			}
			else
			{
				sptr3 = sptr1;
				if ((sptr1 = SQLParseName(nameBuff2, sptr1)) == 0)
				{
					i = cols.GetCount();
					while (i-- > 0)
					{
						col = cols.GetItem(i);
						col->name->Release();
						SDEL_STRING(col->asName);
						MemFree(col);
					}

					Text::StringBuilderUTF8 sb;
					sb.AppendC(UTF8STRC("#42000You have an error in your SQL syntax; check the manual that corresponds to your MySQL server version for the right syntax to use near '"));
					sb.AppendSlow(sptr3);
					sb.AppendC(UTF8STRC("' at line 1"));
					SDEL_STRING(sess->lastError);
					sess->lastError = Text::String::New(sb.ToCString()).Ptr();
					return 0;
				}
				while (Text::CharUtil::PtrIsWS(&sptr1));
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
				else if (Text::StrStartsWithICaseC(sptr1, (UOSInt)(sqlEnd - sptr1), UTF8STRC("FROM")) && Text::CharUtil::IsWS(sptr1 + 4))
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
						SDEL_STRING(col->asName);
						MemFree(col);
					}

					Text::StringBuilderUTF8 sb;
					sb.AppendC(UTF8STRC("#42000You have an error in your SQL syntax; check the manual that corresponds to your MySQL server version for the right syntax to use near '"));
					sb.AppendC(sptr1, (UOSInt)(sqlEnd - sptr1));
					sb.AppendC(UTF8STRC("' at line 1"));
					SDEL_STRING(sess->lastError);
					sess->lastError = Text::String::New(sb.ToCString()).Ptr();
					return 0;
				}
			}
		}

		if (hasFrom)
		{
			while (Text::CharUtil::PtrIsWS(&sptr1));
			//////////////////////////////
			i = cols.GetCount();
			while (i-- > 0)
			{
				col = cols.GetItem(i);
				col->name->Release();
				SDEL_STRING(col->asName);
				MemFree(col);
			}

			Text::StringBuilderUTF8 sb;
			sb.AppendC(UTF8STRC("#00000Unsupported syntax 'FROM'"));
			SDEL_STRING(sess->lastError);
			sess->lastError = Text::String::New(sb.ToCString()).Ptr();

			return 0;
		}
		else
		{
			Bool valid = true;
			DB::DBMSReader *reader;
			Text::StringBuilderUTF8 sb;
			Text::String **colVals;
			const UTF8Char *val;
			NEW_CLASS(reader, DB::DBMSReader(cols.GetCount(), -1));
			colVals = MemAlloc(Text::String *, cols.GetCount());
			i = 0;
			j = cols.GetCount();
			while (i < j && valid)
			{
				col = cols.GetItem(i);
				val = col->name->v;
				#if defined(VERBOSE)
				printf("Column %d is %s\r\n", (int)i, val);
				#endif
				colVals[i] = this->Evals(&val, sess, reader, i, col->asName->ToCString(), &valid);

				col->name->Release();
				SDEL_STRING(col->asName);
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
				colVals[i] = 0;
				col->name->Release();
				SDEL_STRING(col->asName);
				MemFree(col);
				i++;
			}

			if (valid)
			{
				reader->AddRow(colVals);
				MemFree(colVals);

				return reader;
			}
			else
			{
				i = j;
				while (i-- > 0)
				{
					SDEL_STRING(colVals[i]);
				}
				MemFree(colVals);
				DEL_CLASS(reader);
				return 0;
			}
		}
	}
	else if (Text::StrStartsWithICaseC(sptr1, (UOSInt)(sqlEnd - sptr1), UTF8STRC("SET")) && Text::CharUtil::IsWS(sptr1 + 3))
	{
		sptr1 += 3;

		Data::ArrayList<DB::DBMS::SQLColumn*> cols;
		DB::DBMS::SQLColumn *col;
		UTF8Char c;
		UTF8Char *namePtr;

		while (true)
		{
			while (Text::CharUtil::PtrIsWS(&sptr1));
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
						if (Text::StrEqualsICaseC(nameBuff, (UOSInt)(namePtr - nameBuff), UTF8STRC("GLOBAL")))
						{
							isGlobal = true;
							namePtr = nameBuff;
						}
						else if (Text::StrEqualsICaseC(nameBuff, (UOSInt)(namePtr - nameBuff), UTF8STRC("SESSION")))
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
								SDEL_STRING(col->asName);
								MemFree(col);
							}

							Text::StringBuilderUTF8 sb;
							sb.AppendC(UTF8STRC("#00000Unsupported syntax 'FROM'"));
							SDEL_STRING(sess->lastError);
							sess->lastError = Text::String::New(sb.ToCString()).Ptr();
							return 0;
						}
					}
					else if ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || (c >= '0' && c <= '9'))
					{
						*namePtr++ = c;
					}
					else
					{
						sptr1--;
						while (Text::CharUtil::PtrIsWS(&sptr1));
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
								SDEL_STRING(col->asName);
								MemFree(col);
							}

							Text::StringBuilderUTF8 sb;
							sb.AppendC(UTF8STRC("#00000Unsupported syntax 'FROM'"));
							SDEL_STRING(sess->lastError);
							sess->lastError = Text::String::New(sb.ToCString()).Ptr();
							return 0;
						}
					}
				}
				*namePtr = 0;
				if (this->SysVarExist(sess, CSTRP(nameBuff, namePtr), isGlobal?(DB::DBMS::AT_SET_GLOBAL):(DB::DBMS::AT_SET_SESSION)))
				{
					while (Text::CharUtil::PtrIsWS(&sptr1));
					Bool valid = true;
					Text::String *val = this->Evals(&sptr1, sess, 0, 0, CSTR_NULL, &valid);
					if (!valid)
					{
						i = cols.GetCount();
						while (i-- > 0)
						{
							col = cols.GetItem(i);
							col->name->Release();
							SDEL_STRING(col->asName);
							MemFree(col);
						}
						SDEL_STRING(val);
						return 0;
					}
					nameBuff2[0] = isGlobal?'!':'#';
					Text::StrConcat(&nameBuff2[1], nameBuff);
					col = MemAlloc(DB::DBMS::SQLColumn, 1);
					col->name = Text::String::NewNotNullSlow(nameBuff2);
					col->asName = val;
					cols.Add(col);

					while (Text::CharUtil::PtrIsWS(&sptr1));
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
							SDEL_STRING(col->asName);
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
					SDEL_STRING(sess->lastError);
					sess->lastError = Text::String::New(sb.ToCString()).Ptr();
					return 0;
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
							while (Text::CharUtil::PtrIsWS(&sptr1));
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
								SDEL_STRING(sess->lastError);
								sess->lastError = Text::String::New(sb.ToCString()).Ptr();
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
								SDEL_STRING(sess->lastError);
								sess->lastError = Text::String::New(sb.ToCString()).Ptr();
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
							SDEL_STRING(sess->lastError);
							sess->lastError = Text::String::New(sb.ToCString()).Ptr();
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
							while (Text::CharUtil::PtrIsWS(&sptr1));
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
								SDEL_STRING(sess->lastError);
								sess->lastError = Text::String::New(sb.ToCString()).Ptr();
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
								SDEL_STRING(sess->lastError);
								sess->lastError = Text::String::New(sb.ToCString()).Ptr();
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
							SDEL_STRING(sess->lastError);
							sess->lastError = Text::String::New(sb.ToCString()).Ptr();
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
							while (Text::CharUtil::PtrIsWS(&sptr1));
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
								SDEL_STRING(sess->lastError);
								sess->lastError = Text::String::New(sb.ToCString()).Ptr();
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
					SDEL_STRING(sess->lastError);
					sess->lastError = Text::String::New(sb.ToCString()).Ptr();
				}
				if (valid)
				{
					while (Text::CharUtil::PtrIsWS(&sptr1));
					Text::String *val = this->Evals(&sptr1, sess, 0, 0, CSTR_NULL, &valid);
					if (!valid)
					{
						i = cols.GetCount();
						while (i-- > 0)
						{
							col = cols.GetItem(i);
							col->name->Release();
							SDEL_STRING(col->asName);
							MemFree(col);
						}
						SDEL_STRING(val);


						sb.ClearStr();
						sb.AppendC(UTF8STRC("#00000Unsupported syntax '"));
						sb.AppendSlow(sptr1);
						sb.AppendUTF8Char('\'');
						SDEL_STRING(sess->lastError);
						sess->lastError = Text::String::New(sb.ToCString()).Ptr();
						return 0;
					}
					col = MemAlloc(DB::DBMS::SQLColumn, 1);
					col->name = Text::String::New(sb.ToCString());
					col->asName = val;
					cols.Add(col);


					while (Text::CharUtil::PtrIsWS(&sptr1));
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
							SDEL_STRING(col->asName);
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
						SDEL_STRING(col->asName);
						MemFree(col);
					}
					return 0;
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
						while (Text::CharUtil::PtrIsWS(&sptr1));
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
								SDEL_STRING(col->asName);
								MemFree(col);
							}

							Text::StringBuilderUTF8 sb;
							sb.AppendC(UTF8STRC("#00000Unsupported syntax 'FROM'"));
							SDEL_STRING(sess->lastError);
							sess->lastError = Text::String::New(sb.ToCString()).Ptr();
							return 0;
						}
					}
				}
				*namePtr = 0;
				if (this->SysVarExist(sess, CSTRP(nameBuff + 1, namePtr), DB::DBMS::AT_SET_SESSION))
				{
					while (Text::CharUtil::PtrIsWS(&sptr1));
					Bool valid = true;
					Text::String *val = this->Evals(&sptr1, sess, 0, 0, CSTR_NULL, &valid);
					if (!valid)
					{
						i = cols.GetCount();
						while (i-- > 0)
						{
							col = cols.GetItem(i);
							col->name->Release();
							SDEL_STRING(col->asName);
							MemFree(col);
						}
						SDEL_STRING(val);

						return 0;
					}
					col = MemAlloc(DB::DBMS::SQLColumn, 1);
					col->name = Text::String::NewP(nameBuff, namePtr);
					col->asName = val;
					cols.Add(col);

					while (Text::CharUtil::PtrIsWS(&sptr1));
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
							SDEL_STRING(col->asName);
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
						SDEL_STRING(col->asName);
						MemFree(col);
					}

					Text::StringBuilderUTF8 sb;
					sb.AppendC(UTF8STRC("#00000Unsupported syntax 'FROM'"));
					SDEL_STRING(sess->lastError);
					sess->lastError = Text::String::New(sb.ToCString()).Ptr();
					return 0;
				}
			}
			else
			{
				i = cols.GetCount();
				while (i-- > 0)
				{
					col = cols.GetItem(i);
					col->name->Release();
					SDEL_STRING(col->asName);
					MemFree(col);
				}

				Text::StringBuilderUTF8 sb;
				sb.AppendC(UTF8STRC("#00000Unsupported syntax 'FROM'"));
				SDEL_STRING(sess->lastError);
				sess->lastError = Text::String::New(sb.ToCString()).Ptr();
				return 0;
			}
		}
	}
	else if (Text::StrStartsWithICaseC(sptr1, (UOSInt)(sqlEnd - sptr1), UTF8STRC("SHOW")) && Text::CharUtil::IsWS(sptr1 + 4))
	{
		sptr1 += 4;
		while (Text::CharUtil::PtrIsWS(&sptr1));
		if (Text::StrStartsWithICaseC(sptr1, (UOSInt)(sqlEnd - sptr1), UTF8STRC("ENGINES")) && (Text::CharUtil::IsWS(sptr1 + 7) || sptr1[7] == 0))
		{
			////////////////////////
			Text::StringBuilderUTF8 sb;
			sb.AppendC(UTF8STRC("#42000You have an error in your SQL syntax; check the manual that corresponds to your MySQL server version for the right syntax to use near '"));
			sb.AppendSlow(sptr1);
			sb.AppendC(UTF8STRC("' at line 1"));
			SDEL_STRING(sess->lastError);
			sess->lastError = Text::String::New(sb.ToCString()).Ptr();
		}
		else if (Text::StrStartsWithICaseC(sptr1, (UOSInt)(sqlEnd - sptr1), UTF8STRC("CHARSET")) && (Text::CharUtil::IsWS(sptr1 + 7) || sptr1[7] == 0))
		{
			////////////////////////
			Text::StringBuilderUTF8 sb;
			sb.AppendC(UTF8STRC("#42000You have an error in your SQL syntax; check the manual that corresponds to your MySQL server version for the right syntax to use near '"));
			sb.AppendSlow(sptr1);
			sb.AppendC(UTF8STRC("' at line 1"));
			SDEL_STRING(sess->lastError);
			sess->lastError = Text::String::New(sb.ToCString()).Ptr();
		}
		else if (Text::StrStartsWithICaseC(sptr1, (UOSInt)(sqlEnd - sptr1), UTF8STRC("COLLATION")) && (Text::CharUtil::IsWS(sptr1 + 9) || sptr1[9] == 0))
		{
			////////////////////////
			Text::StringBuilderUTF8 sb;
			sb.AppendC(UTF8STRC("#42000You have an error in your SQL syntax; check the manual that corresponds to your MySQL server version for the right syntax to use near '"));
			sb.AppendSlow(sptr1);
			sb.AppendC(UTF8STRC("' at line 1"));
			SDEL_STRING(sess->lastError);
			sess->lastError = Text::String::New(sb.ToCString()).Ptr();
		}
		else if (Text::StrStartsWithICaseC(sptr1, (UOSInt)(sqlEnd - sptr1), UTF8STRC("VARIABLES")) && (Text::CharUtil::IsWS(sptr1 + 9) || sptr1[9] == 0))
		{
			sptr1 += 9;
			while (Text::CharUtil::PtrIsWS(&sptr1));
			
			if (sptr1[0] == 0)
			{
				DB::DBMSReader *reader;
				OSInt i;
				OSInt j;
				Text::String *row[2];
				Text::StringBuilderUTF8 sb;
				NEW_CLASS(reader, DB::DBMSReader(2, -1));
				reader->SetColumn(0, CSTR("Variablename"), DB::DBUtil::CT_VarUTF8Char);
				reader->SetColumn(1, CSTR("Value"), DB::DBUtil::CT_VarUTF8Char);
				i = 0;
				j = sizeof(sysVarList) / sizeof(sysVarList[0]);
				while (i < j)
				{
					row[0] = Text::String::NewNotNullSlow((const UTF8Char*)sysVarList[i]).Ptr();
					sb.ClearStr();
					SysVarGet(sb, sess, row[0]->ToCString());
					row[1] = Text::String::New(sb.ToString(), sb.GetLength()).Ptr();
					reader->AddRow(row);
					i++;
				}
				return reader;
			}
			else if (Text::StrStartsWithICaseC(sptr1, (UOSInt)(sqlEnd - sptr1), UTF8STRC("LIKE")) && Text::CharUtil::IsWS(sptr1 + 4))
			{
				sptr1 += 4;
				while (Text::CharUtil::PtrIsWS(&sptr1));

				if (sptr1[0] == '\'')
				{
					Bool valid = true;
 					Text::String *val = this->Evals(&sptr1, sess, 0, 0, CSTR_NULL, &valid);
					if (!valid)
					{
						SDEL_STRING(val);
						return 0;
					}

					while (Text::CharUtil::PtrIsWS(&sptr1));
					if (sptr1[0] == 0 && val != 0)
					{
						DB::DBMSReader *reader;
						OSInt i;
						OSInt j;
						Text::String *row[2];
						Text::StringBuilderUTF8 sb;
						NEW_CLASS(reader, DB::DBMSReader(2, -1));
						reader->SetColumn(0, CSTR("Variablename"), DB::DBUtil::CT_VarUTF8Char);
						reader->SetColumn(1, CSTR("Value"), DB::DBUtil::CT_VarUTF8Char);
						i = 0;
						j = sizeof(sysVarList) / sizeof(sysVarList[0]);
						while (i < j)
						{
							if (StrLike((const UTF8Char*)sysVarList[i], val->v))
							{
								sb.ClearStr();
								row[0] = Text::String::NewNotNullSlow((const UTF8Char*)sysVarList[i]).Ptr();
								SysVarGet(sb, sess, row[0]->ToCString());
								row[1] = Text::String::New(sb.ToString(), sb.GetLength()).Ptr();
								reader->AddRow(row);
							}
							i++;
						}
						SDEL_STRING(val);
						return reader;

					}
					else
					{
						SDEL_STRING(val);

						Text::StringBuilderUTF8 sb;
						sb.AppendC(UTF8STRC("#42000You have an error in your SQL syntax; check the manual that corresponds to your MySQL server version for the right syntax to use near '"));
						sb.AppendSlow(sptr1);
						sb.AppendC(UTF8STRC("' at line 1"));
						SDEL_STRING(sess->lastError);
						sess->lastError = Text::String::New(sb.ToCString()).Ptr();
						
						return 0;
					}
				}
				else
				{
					Text::StringBuilderUTF8 sb;
					sb.AppendC(UTF8STRC("#42000You have an error in your SQL syntax; check the manual that corresponds to your MySQL server version for the right syntax to use near '"));
					sb.AppendSlow(sptr1);
					sb.AppendC(UTF8STRC("' at line 1"));
					SDEL_STRING(sess->lastError);
					sess->lastError = Text::String::New(sb.ToCString()).Ptr();
				}
			}
			else
			{
				Text::StringBuilderUTF8 sb;
				sb.AppendC(UTF8STRC("#42000You have an error in your SQL syntax; check the manual that corresponds to your MySQL server version for the right syntax to use near '"));
				sb.AppendSlow(sptr1);
				sb.AppendC(UTF8STRC("' at line 1"));
				SDEL_STRING(sess->lastError);
				sess->lastError = Text::String::New(sb.ToCString()).Ptr();
			}
		}
		else
		{
			Text::StringBuilderUTF8 sb;
			sb.AppendC(UTF8STRC("#42000You have an error in your SQL syntax; check the manual that corresponds to your MySQL server version for the right syntax to use near '"));
			sb.AppendSlow(sptr1);
			sb.AppendC(UTF8STRC("' at line 1"));
			SDEL_STRING(sess->lastError);
			sess->lastError = Text::String::New(sb.ToCString()).Ptr();
		}
	}
	else
	{
		Text::StringBuilderUTF8 sb;
		sb.AppendC(UTF8STRC("#42000You have an error in your SQL syntax; check the manual that corresponds to your MySQL server version for the right syntax to use near '"));
		sb.AppendSlow(sptr1);
		sb.AppendC(UTF8STRC("' at line 1"));
		SDEL_STRING(sess->lastError);
		sess->lastError = Text::String::New(sb.ToCString()).Ptr();
	}
	return 0;
}

void DB::DBMS::CloseReader(DB::DBReader *r)
{

}

UTF8Char *DB::DBMS::GetErrMessage(Int32 sessId, UTF8Char *msgBuff)
{
	NN<DB::DBMS::SessionInfo> sess;
	Sync::MutexUsage mutUsage(this->sessMut);
	if (this->sessMap.Get(sessId).SetTo(sess) && sess->lastError)
	{
		msgBuff = sess->lastError->ConcatTo(msgBuff);
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
	UOSInt i = varList->GetCount();
	NN<Text::String> var;
	while (i-- > 0)
	{
		var = varList->GetItemNoCheck(i);
		var->Release();
	}
	SDEL_STRING(sess->lastError);
	SDEL_STRING(sess->database);
	sess.Delete();
}
