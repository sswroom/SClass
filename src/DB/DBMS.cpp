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
		Data::ArrayList<const UTF8Char **> *rows;
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
				NEW_CLASS(this->rows, Data::ArrayList<const UTF8Char**>());
				this->colTypes = MemAlloc(DB::DBUtil::ColType, this->colCount);
				this->colNames = MemAlloc(const UTF8Char*, this->colCount);
				UOSInt i;
				i = 0;
				while (i < this->colCount)
				{
					this->colTypes[i] = DB::DBUtil::CT_VarChar;
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
				const UTF8Char **row;
				i = this->rows->GetCount();
				while (i-- > 0)
				{
					row = this->rows->GetItem(i);
					j = this->colCount;
					while (j-- > 0)
					{
						SDEL_TEXT(row[j]);
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

		void AddRow(const UTF8Char **row)
		{
			if (this->rows == 0)
			{
				return;
			}
			const UTF8Char **newRow = MemAlloc(const UTF8Char*, this->colCount);
			MemCopyNO(newRow, row, sizeof(const UTF8Char*) * this->colCount);
			this->rows->Add(newRow);
		}

		void SetColumn(UOSInt colIndex, const UTF8Char *colName, DB::DBUtil::ColType colType)
		{
			if (this->rows && colIndex < this->colCount)
			{
				SDEL_TEXT(this->colNames[colIndex]);
				this->colNames[colIndex] = Text::StrCopyNew(colName);
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
			const UTF8Char **row = this->rows->GetItem((UOSInt)this->rowIndex);
			if (row == 0 || row[colIndex] == 0)
				return 0;
			return Text::StrToInt32(row[colIndex]);
		}

		virtual Int64 GetInt64(UOSInt colIndex)
		{
			if (this->rows == 0 || colIndex >= this->colCount)
				return 0;
			const UTF8Char **row = this->rows->GetItem((UOSInt)this->rowIndex);
			if (row == 0 || row[colIndex] == 0)
				return 0;
			return Text::StrToInt64(row[colIndex]);
		}

		virtual WChar *GetStr(UOSInt colIndex, WChar *buff)
		{
			if (this->rows == 0 || colIndex >= this->colCount)
				return 0;
			const UTF8Char **row = this->rows->GetItem((UOSInt)this->rowIndex);
			if (row == 0 || row[colIndex] == 0)
				return 0;
			return Text::StrUTF8_WChar(buff, row[colIndex], -1, 0);
		}

		virtual Bool GetStr(UOSInt colIndex, Text::StringBuilderUTF *sb)
		{
			if (this->rows == 0 || colIndex >= this->colCount)
				return false;
			const UTF8Char **row = this->rows->GetItem((UOSInt)this->rowIndex);
			if (row == 0 || row[colIndex] == 0)
				return false;
			sb->Append(row[colIndex]);
			return true;
		}

		virtual const UTF8Char *GetNewStr(UOSInt colIndex)
		{
			if (this->rows == 0 || colIndex >= this->colCount)
				return 0;
			const UTF8Char **row = this->rows->GetItem((UOSInt)this->rowIndex);
			if (row == 0 || row[colIndex] == 0)
				return 0;
			return Text::StrCopyNew(row[colIndex]);
		}

		virtual UTF8Char *GetStr(UOSInt colIndex, UTF8Char *buff, UOSInt buffSize)
		{
			if (this->rows == 0 || colIndex >= this->colCount)
				return 0;
			const UTF8Char **row = this->rows->GetItem((UOSInt)this->rowIndex);
			if (row == 0 || row[colIndex] == 0)
				return 0;
			return Text::StrConcatS(buff, row[colIndex], buffSize);
		}

		virtual DateErrType GetDate(UOSInt colIndex, Data::DateTime *outVal)
		{
			if (this->rows == 0 || colIndex >= this->colCount)
				return DET_ERROR;
			const UTF8Char **row = this->rows->GetItem((UOSInt)this->rowIndex);
			if (row == 0)
				return DET_ERROR;
			if (row[colIndex] == 0)
				return DET_NULL;
			if (outVal->SetValue(row[colIndex]))
				return DET_OK;
			else
				return DET_ERROR;
		}

		virtual Double GetDbl(UOSInt colIndex)
		{
			if (this->rows == 0 || colIndex >= this->colCount)
				return 0;
			const UTF8Char **row = this->rows->GetItem((UOSInt)this->rowIndex);
			if (row == 0 || row[colIndex] == 0)
				return 0;
			return Text::StrToDouble(row[colIndex]);
		}

		virtual Bool GetBool(UOSInt colIndex)
		{
			if (this->rows == 0 || colIndex >= this->colCount)
				return false;
			const UTF8Char **row = this->rows->GetItem((UOSInt)this->rowIndex);
			if (row == 0 || row[colIndex] == 0)
				return false;
			return Text::StrToInt32(row[colIndex]) != 0;
		}

		virtual UOSInt GetBinarySize(UOSInt colIndex)
		{
			if (this->rows == 0 || colIndex >= this->colCount)
				return 0;
			const UTF8Char **row = this->rows->GetItem((UOSInt)this->rowIndex);
			if (row == 0 || row[colIndex] == 0)
				return 0;
			return Text::StrCharCnt(row[colIndex]);
		}

		virtual UOSInt GetBinary(UOSInt colIndex, UInt8 *buff)
		{
			if (this->rows == 0 || colIndex >= this->colCount)
				return 0;
			const UTF8Char **row = this->rows->GetItem((UOSInt)this->rowIndex);
			if (row == 0 || row[colIndex] == 0)
				return 0;
			UOSInt cnt = Text::StrCharCnt(row[colIndex]);
			MemCopyNO(buff, row[colIndex], cnt);
			return cnt;
		}

		virtual Math::Vector2D *GetVector(UOSInt colIndex)
		{
			return 0;
		}

		virtual Bool IsNull(UOSInt colIndex)
		{
			if (this->rows == 0 || colIndex >= this->colCount)
				return true;
			const UTF8Char **row = this->rows->GetItem((UOSInt)this->rowIndex);
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
				return Text::StrUOSInt(Text::StrConcat(buff, (const UTF8Char*)"column"), colIndex + 1);
			}

		}

		virtual DB::DBUtil::ColType GetColType(UOSInt colIndex, UOSInt *colSize)
		{
			if (this->rowChanged != -1)
				return DB::DBUtil::CT_Unknown;
			if (colIndex >= this->colCount)
				return DB::DBUtil::CT_Unknown;
			return this->colTypes[colIndex];
		}

		virtual Bool GetColDef(UOSInt colIndex, DB::ColDef *colDef)
		{
			UTF8Char sbuff[256];
			if (this->rowChanged != -1)
				return false;
			if (colIndex >= this->colCount)
				return false;
			if (this->colNames[colIndex])
			{
				colDef->SetColName(this->colNames[colIndex]);
			}
			else
			{
				Text::StrUOSInt(Text::StrConcat(sbuff, (const UTF8Char*)"column"), colIndex + 1);
				colDef->SetColName(sbuff);
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
				MemCopyNO(nameBuff, sql, sptr - sql);
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

Bool DB::DBMS::SysVarExist(DB::DBMS::SessionInfo *sess, const UTF8Char *varName, AccessType atype)
{
#if defined(VERBOSE)
	printf("SysVarExist: %s\r\n", varName);
#endif
	OSInt i = 0;
	OSInt j = sizeof(sysVarList) / sizeof(sysVarList[0]) - 1;
	OSInt k;
	OSInt l;
	while (i <= j)
	{
		k = (i + j) >> 1;
		l = Text::StrCompareICase((const UTF8Char*)sysVarList[k], varName);
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

const UTF8Char *DB::DBMS::SysVarGet(Text::StringBuilderUTF *sb, DB::DBMS::SessionInfo *sess, const UTF8Char *varName)
{
	Bool isGlobal = false;
	if (Text::StrStartsWithICase(varName, (const UTF8Char*)"GLOBAL."))
	{
		isGlobal = true;
		varName += 7;
	}

	if (Text::StrEqualsICase(varName, (const UTF8Char*)"autocommit"))
	{
		sb->Append(sess->autoCommit?(const UTF8Char*)"1":(const UTF8Char*)"0");
		return varName + 10;
	}
	else if (Text::StrEqualsICase(varName, (const UTF8Char*)"auto_increment_increment"))
	{
		sb->AppendI32(sess->autoIncInc);
		return varName + 24;
	}
	else if (Text::StrEqualsICase(varName, (const UTF8Char*)"character_set_server"))
	{
		sb->Append((const UTF8Char*)"utf8mb4");
		return varName + 20;
	}
	else if (Text::StrEqualsICase(varName, (const UTF8Char*)"collation_server"))
	{
		sb->Append((const UTF8Char*)"utf8mb4_0900_ai_ci");
		return varName + 16;
	}
	else if (Text::StrEqualsICase(varName, (const UTF8Char*)"lower_case_table_names"))
	{
		sb->AppendI32(1);
		return varName + 22;
	}
	else if (Text::StrEqualsICase(varName, (const UTF8Char*)"max_allowed_packet"))
	{
		sb->AppendU32(sess->params.clientMaxPacketSize);
		return varName + 18;
	}
	else if (Text::StrEqualsICase(varName, (const UTF8Char*)"sql_mode"))
	{
		Bool found = false;
		if (sess->sqlModes & SQLM_ALLOW_INVALID_DATES)
		{
			if (found) sb->Append((const UTF8Char*)", ");
			found = true;
			sb->Append((const UTF8Char*)"ALLOW_INVALID_DATES");
		}
		if (sess->sqlModes & SQLM_ANSI_QUOTES)
		{
			if (found) sb->Append((const UTF8Char*)", ");
			found = true;
			sb->Append((const UTF8Char*)"ANSI_QUOTES");
		}
		if (sess->sqlModes & SQLM_ERROR_FOR_DIVISION_BY_ZERO)
		{
			if (found) sb->Append((const UTF8Char*)", ");
			found = true;
			sb->Append((const UTF8Char*)"ERROR_FOR_DIVISION_BY_ZERO");
		}
		if (sess->sqlModes & SQLM_HIGH_NOT_PRECEDENCE)
		{
			if (found) sb->Append((const UTF8Char*)", ");
			found = true;
			sb->Append((const UTF8Char*)"HIGH_NOT_PRECEDENCE");
		}
		if (sess->sqlModes & SQLM_IGNORE_SPACE)
		{
			if (found) sb->Append((const UTF8Char*)", ");
			found = true;
			sb->Append((const UTF8Char*)"IGNORE_SPACE");
		}
		if (sess->sqlModes & SQLM_NO_AUTO_VALUE_ON_ZERO)
		{
			if (found) sb->Append((const UTF8Char*)", ");
			found = true;
			sb->Append((const UTF8Char*)"NO_AUTO_VALUE_ON_ZERO");
		}
		if (sess->sqlModes & SQLM_NO_BACKSLASH_ESCAPES)
		{
			if (found) sb->Append((const UTF8Char*)", ");
			found = true;
			sb->Append((const UTF8Char*)"NO_BACKSLASH_ESCAPES");
		}
		if (sess->sqlModes & SQLM_NO_DIR_IN_CREATE)
		{
			if (found) sb->Append((const UTF8Char*)", ");
			found = true;
			sb->Append((const UTF8Char*)"NO_DIR_IN_CREATE");
		}
		if (sess->sqlModes & SQLM_NO_ENGINE_SUBSTITUTION)
		{
			if (found) sb->Append((const UTF8Char*)", ");
			found = true;
			sb->Append((const UTF8Char*)"NO_ENGINE_SUBSTITUTION");
		}
		if (sess->sqlModes & SQLM_NO_UNSIGNED_SUBTRACTION)
		{
			if (found) sb->Append((const UTF8Char*)", ");
			found = true;
			sb->Append((const UTF8Char*)"NO_UNSIGNED_SUBTRACTION");
		}
		if (sess->sqlModes & SQLM_NO_ZERO_DATE)
		{
			if (found) sb->Append((const UTF8Char*)", ");
			found = true;
			sb->Append((const UTF8Char*)"NO_ZERO_DATE");
		}
		if (sess->sqlModes & SQLM_NO_ZERO_IN_DATE)
		{
			if (found) sb->Append((const UTF8Char*)", ");
			found = true;
			sb->Append((const UTF8Char*)"NO_ZERO_IN_DATE");
		}
		if (sess->sqlModes & SQLM_ONLY_FULL_GROUP_BY)
		{
			if (found) sb->Append((const UTF8Char*)", ");
			found = true;
			sb->Append((const UTF8Char*)"ONLY_FULL_GROUP_BY");
		}
		if (sess->sqlModes & SQLM_PAD_CHAR_TO_FULL_LENGTH)
		{
			if (found) sb->Append((const UTF8Char*)", ");
			found = true;
			sb->Append((const UTF8Char*)"PAD_CHAR_TO_FULL_LENGTH");
		}
		if (sess->sqlModes & SQLM_PIPES_AS_CONCAT)
		{
			if (found) sb->Append((const UTF8Char*)", ");
			found = true;
			sb->Append((const UTF8Char*)"PIPES_AS_CONCAT");
		}
		if (sess->sqlModes & SQLM_REAL_AS_FLOAT)
		{
			if (found) sb->Append((const UTF8Char*)", ");
			found = true;
			sb->Append((const UTF8Char*)"REAL_AS_FLOAT");
		}
		if (sess->sqlModes & SQLM_STRICT_ALL_TABLES)
		{
			if (found) sb->Append((const UTF8Char*)", ");
			found = true;
			sb->Append((const UTF8Char*)"STRICT_ALL_TABLES");
		}
		if (sess->sqlModes & SQLM_STRICT_TRANS_TABLES)
		{
			if (found) sb->Append((const UTF8Char*)", ");
			found = true;
			sb->Append((const UTF8Char*)"STRICT_TRANS_TABLES");
		}
		if (sess->sqlModes & SQLM_TIME_TRUNCATE_FRACTIONAL)
		{
			if (found) sb->Append((const UTF8Char*)", ");
			found = true;
			sb->Append((const UTF8Char*)"TIME_TRUNCATE_FRACTIONAL");
		}
		return varName + 8;
	}
	else if (Text::StrEqualsICase(varName, (const UTF8Char*)"system_time_zone") || Text::StrEqualsICase(varName, (const UTF8Char*)"time_zone"))
	{
		Data::DateTime dt;
		dt.SetCurrTime();
		Int32 tz = dt.GetTimeZoneQHR();
		if (tz > 0)
		{
			sb->AppendChar('+', 1);			
		}
		else
		{
			tz = -tz;
			sb->AppendChar('-', 1);
		}
		sb->AppendI32(tz >> 2);
		sb->AppendChar(':', 1);
		if (tz & 3)
		{
			sb->AppendI32((tz & 3) * 15);
		}
		else
		{
			sb->Append((const UTF8Char*)"00");
		}
		return varName + 16;
	}
	return 0;
}

void DB::DBMS::SysVarColumn(DB::DBMSReader *reader, UOSInt colIndex, const UTF8Char *varName, const UTF8Char *colName)
{
	UTF8Char sbuff[128];
	if (colName == 0)
	{
		Text::StrConcat(Text::StrConcat(sbuff, (const UTF8Char*)"@@"), varName);
		colName = sbuff;
	}
	
	if (Text::StrEqualsICase(varName, (const UTF8Char*)"autocommit"))
	{
		reader->SetColumn(colIndex, colName, DB::DBUtil::CT_Bool);
		return;
	}
	else if (Text::StrEqualsICase(varName, (const UTF8Char*)"auto_increment_increment"))
	{
		reader->SetColumn(colIndex, colName, DB::DBUtil::CT_Int32);
		return;
	}
	else if (Text::StrEqualsICase(varName, (const UTF8Char*)"character_set_server"))
	{
		reader->SetColumn(colIndex, colName, DB::DBUtil::CT_VarChar);
		return;
	}
	else if (Text::StrEqualsICase(varName, (const UTF8Char*)"collation_server"))
	{
		reader->SetColumn(colIndex, colName, DB::DBUtil::CT_VarChar);
		return;
	}
	else if (Text::StrEqualsICase(varName, (const UTF8Char*)"lower_case_table_names"))
	{
		reader->SetColumn(colIndex, colName, DB::DBUtil::CT_Int32);
		return;
	}
	else if (Text::StrEqualsICase(varName, (const UTF8Char*)"max_allowed_packet"))
	{
		reader->SetColumn(colIndex, colName, DB::DBUtil::CT_Int32);
		return;
	}
	else if (Text::StrEqualsICase(varName, (const UTF8Char*)"sql_mode"))
	{
		reader->SetColumn(colIndex, colName, DB::DBUtil::CT_VarChar);
		return;
	}
	else if (Text::StrEqualsICase(varName, (const UTF8Char*)"system_time_zone"))
	{
		reader->SetColumn(colIndex, colName, DB::DBUtil::CT_VarChar);
		return;
	}
	else if (Text::StrEqualsICase(varName, (const UTF8Char*)"time_zone"))
	{
		reader->SetColumn(colIndex, colName, DB::DBUtil::CT_VarChar);
		return;
	}
	reader->SetColumn(colIndex, colName, DB::DBUtil::CT_VarChar);
}

Bool DB::DBMS::SysVarSet(DB::DBMS::SessionInfo *sess, Bool isGlobal, const UTF8Char *varName, const UTF8Char *val)
{
	if (Text::StrEqualsICase(varName, (const UTF8Char*)"autocommit"))
	{
		Bool v;
		if (val == 0)
		{
			v = false;
		}
		else
		{
			v = Text::StrToInt32(val) != 0;
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
	else if (Text::StrEqualsICase(varName, (const UTF8Char*)"auto_increment_incremnt"))
	{
		Int32 v;
		if (val == 0)
		{
			v = 1;
		}
		else
		{
			v = Text::StrToInt32(val);
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
	else if (Text::StrEqualsICase(varName, (const UTF8Char*)"character_set_server"))
	{
	}
	else if (Text::StrEqualsICase(varName, (const UTF8Char*)"collation_server"))
	{
	}
	else if (Text::StrEqualsICase(varName, (const UTF8Char*)"lower_case_table_names"))
	{
	}
	else if (Text::StrEqualsICase(varName, (const UTF8Char*)"max_allowed_packet"))
	{
	}
	else if (Text::StrEqualsICase(varName, (const UTF8Char*)"sql_mode"))
	{
		SQLMODE sqlMode = (SQLMODE)0;
		Text::StringBuilderUTF8 sb;
		UTF8Char *sarr[2];
		UOSInt i;
		sb.Append(val);
		sarr[1] = sb.ToString();
		i = 2;
		while (i == 2)
		{
			i = Text::StrSplitTrim(sarr, 2, sarr[1], ',');
			if (Text::StrEqualsICase(sarr[0], (const UTF8Char*)"ALLOW_INVALID_DATES"))
			{
				sqlMode = (SQLMODE)(sqlMode | SQLM_ALLOW_INVALID_DATES);
			}
			if (Text::StrEqualsICase(sarr[0], (const UTF8Char*)"ANSI_QUOTES"))
			{
				sqlMode = (SQLMODE)(sqlMode | SQLM_ANSI_QUOTES);
			}
			if (Text::StrEqualsICase(sarr[0], (const UTF8Char*)"ERROR_FOR_DIVISION_BY_ZERO"))
			{
				sqlMode = (SQLMODE)(sqlMode | SQLM_ERROR_FOR_DIVISION_BY_ZERO);
			}
			if (Text::StrEqualsICase(sarr[0], (const UTF8Char*)"HIGH_NOT_PRECEDENCE"))
			{
				sqlMode = (SQLMODE)(sqlMode | SQLM_HIGH_NOT_PRECEDENCE);
			}
			if (Text::StrEqualsICase(sarr[0], (const UTF8Char*)"IGNORE_SPACE"))
			{
				sqlMode = (SQLMODE)(sqlMode | SQLM_IGNORE_SPACE);
			}
			if (Text::StrEqualsICase(sarr[0], (const UTF8Char*)"NO_AUTO_VALUE_ON_ZERO"))
			{
				sqlMode = (SQLMODE)(sqlMode | SQLM_NO_AUTO_VALUE_ON_ZERO);
			}
			if (Text::StrEqualsICase(sarr[0], (const UTF8Char*)"NO_BACKSLASH_ESCAPES"))
			{
				sqlMode = (SQLMODE)(sqlMode | SQLM_NO_BACKSLASH_ESCAPES);
			}
			if (Text::StrEqualsICase(sarr[0], (const UTF8Char*)"NO_DIR_IN_CREATE"))
			{
				sqlMode = (SQLMODE)(sqlMode | SQLM_NO_DIR_IN_CREATE);
			}
			if (Text::StrEqualsICase(sarr[0], (const UTF8Char*)"NO_ENGINE_SUBSTITUTION"))
			{
				sqlMode = (SQLMODE)(sqlMode | SQLM_NO_ENGINE_SUBSTITUTION);
			}
			if (Text::StrEqualsICase(sarr[0], (const UTF8Char*)"NO_UNSIGNED_SUBTRACTION"))
			{
				sqlMode = (SQLMODE)(sqlMode | SQLM_NO_UNSIGNED_SUBTRACTION);
			}
			if (Text::StrEqualsICase(sarr[0], (const UTF8Char*)"NO_ZERO_DATE"))
			{
				sqlMode = (SQLMODE)(sqlMode | SQLM_NO_ZERO_DATE);
			}
			if (Text::StrEqualsICase(sarr[0], (const UTF8Char*)"NO_ZERO_IN_DATE"))
			{
				sqlMode = (SQLMODE)(sqlMode | SQLM_NO_ZERO_IN_DATE);
			}
			if (Text::StrEqualsICase(sarr[0], (const UTF8Char*)"ONLY_FULL_GROUP_BY"))
			{
				sqlMode = (SQLMODE)(sqlMode | SQLM_ONLY_FULL_GROUP_BY);
			}
			if (Text::StrEqualsICase(sarr[0], (const UTF8Char*)"PAD_CHAR_TO_FULL_LENGTH"))
			{
				sqlMode = (SQLMODE)(sqlMode | SQLM_PAD_CHAR_TO_FULL_LENGTH);
			}
			if (Text::StrEqualsICase(sarr[0], (const UTF8Char*)"PIPES_AS_CONCAT"))
			{
				sqlMode = (SQLMODE)(sqlMode | SQLM_PIPES_AS_CONCAT);
			}
			if (Text::StrEqualsICase(sarr[0], (const UTF8Char*)"REAL_AS_FLOAT"))
			{
				sqlMode = (SQLMODE)(sqlMode | SQLM_REAL_AS_FLOAT);
			}
			if (Text::StrEqualsICase(sarr[0], (const UTF8Char*)"STRICT_ALL_TABLES"))
			{
				sqlMode = (SQLMODE)(sqlMode | SQLM_STRICT_ALL_TABLES);
			}
			if (Text::StrEqualsICase(sarr[0], (const UTF8Char*)"STRICT_TRANS_TABLES"))
			{
				sqlMode = (SQLMODE)(sqlMode | SQLM_STRICT_TRANS_TABLES);
			}
			if (Text::StrEqualsICase(sarr[0], (const UTF8Char*)"TIME_TRUNCATE_FRACTIONAL"))
			{
				sqlMode = (SQLMODE)(sqlMode | SQLM_TIME_TRUNCATE_FRACTIONAL);
			}
		}
		sess->sqlModes = sqlMode;
	}
	else if (Text::StrEqualsICase(varName, (const UTF8Char*)"system_time_zone"))
	{
	}
	else if (Text::StrEqualsICase(varName, (const UTF8Char*)"time_zone"))
	{
	}
	return false;

}

const UTF8Char *DB::DBMS::UserVarGet(Text::StringBuilderUTF *sb, DB::DBMS::SessionInfo *sess, const UTF8Char *varName)
{
	const UTF8Char *val = sess->userVars->Get(varName);
	UOSInt i = Text::StrCharCnt(varName);
	if (val)
	{
		sb->Append(val);
	}
	return varName + i;
}

void DB::DBMS::UserVarColumn(DB::DBMSReader *reader, UOSInt colIndex, const UTF8Char *varName, const UTF8Char *colName)
{
	UTF8Char sbuff[128];
	if (colName == 0)
	{
		Text::StrConcat(Text::StrConcat(sbuff, (const UTF8Char*)"@"), varName);
		colName = sbuff;
	}

	reader->SetColumn(colIndex, colName, DB::DBUtil::CT_VarChar);
}

Bool DB::DBMS::UserVarSet(DB::DBMS::SessionInfo *sess, const UTF8Char *varName, const UTF8Char *val)
{
	const UTF8Char *oldVal = sess->userVars->Remove(varName);
	if (oldVal)
	{
		Text::StrDelNew(oldVal);
	}
	if (val)
	{
		sess->userVars->Put(varName, Text::StrCopyNew(val));
	}
	return true;
}

const UTF8Char *DB::DBMS::Evals(const UTF8Char **valPtr, DB::DBMS::SessionInfo *sess, DB::DBMSReader *reader, UOSInt colIndex, const UTF8Char *colName, Bool *valid)
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
				sb2.AppendChar(c, 1);
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

				sb.Append((const UTF8Char*)"#HY000Unknown system variable '");
				sb.Append(val);
				sb.Append((const UTF8Char*)"'");
				SDEL_TEXT(sess->lastError);
				sess->lastError = Text::StrCopyNew(sb.ToString());
				return 0;
			}
		}
		*valPtr = val;
		if (reader)
		{
			this->SysVarColumn(reader, colIndex, sb2.ToString(), colName);
		}
		if (this->SysVarGet(&sb, sess, sb2.ToString()) == 0)
		{
			*valid = false;

			sb.Append((const UTF8Char*)"#HY000Unknown system variable '");
			sb.Append(sb2.ToString());
			sb.Append((const UTF8Char*)"'");
			SDEL_TEXT(sess->lastError);
			sess->lastError = Text::StrCopyNew(sb.ToString());
			return 0;
		}
		else
		{
			return Text::StrCopyNew(sb.ToString());
		}
	}
	else if (val[0] == '@')
	{
		if (reader)
		{
			this->UserVarColumn(reader, colIndex, val + 1, colName);
		}
		Text::StringBuilderUTF8 sb;
		if ((val = this->UserVarGet(&sb, sess, val + 1)) == 0)
		{
			return 0;
		}
		else
		{
			*valPtr = val;
			return Text::StrCopyNew(sb.ToString());
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
					sb.AppendChar('\'', 1);
				}
				else
				{
					*valPtr = val;
					if (reader)
						reader->SetColumn(colIndex, colName?colName:sb.ToString(), DB::DBUtil::CT_VarChar);
					return Text::StrCopyNew(sb.ToString());
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
					sb.AppendChar('\t', 1);
				}
				else if (*val == 'r')
				{
					val++;
					sb.AppendChar('\r', 1);
				}
				else if (*val == 'n')
				{
					val++;
					sb.AppendChar('\n', 1);
				}
				else if (*val == '\\')
				{
					val++;
					sb.AppendChar('\\', 1);
				}
				else if (*val == '\'')
				{
					val++;
					sb.AppendChar('\'', 1);
				}
				else if (*val == '\"')
				{
					val++;
					sb.AppendChar('\"', 1);
				}
				else
				{
					sb.AppendChar('\\', 1);	
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
					sb.AppendChar('\"', 1);
				}
				else
				{
					*valPtr = val;
					if (reader)
						reader->SetColumn(colIndex, colName?colName:sb.ToString(), DB::DBUtil::CT_VarChar);
					return Text::StrCopyNew(sb.ToString());
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
					sb.AppendChar('\t', 1);
				}
				else if (*val == 'r')
				{
					val++;
					sb.AppendChar('\r', 1);
				}
				else if (*val == 'n')
				{
					val++;
					sb.AppendChar('\n', 1);
				}
				else if (*val == '\\')
				{
					val++;
					sb.AppendChar('\\', 1);
				}
				else if (*val == '\'')
				{
					val++;
					sb.AppendChar('\'', 1);
				}
				else if (*val == '\"')
				{
					val++;
					sb.AppendChar('\"', 1);
				}
				else
				{
					sb.AppendChar('\\', 1);	
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
		const UTF8Char *val2;
		UTF8Char c;
		Bool isDbl = false;
		while (true)
		{
			c = *sptr++;
			if (c >= '0' && c <= '9')
			{
				sb.AppendChar(c, 1);
			}
			else if (c == '.')
			{
				isDbl = true;
				sb.AppendChar(c, 1);
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
				reader->SetColumn(colIndex, colName?colName:val, isDbl?DB::DBUtil::CT_Double:DB::DBUtil::CT_Int32);
			*valPtr = sptr;
			return Text::StrCopyNew(sb.ToString());
		}
		if (*sptr == '+')
		{
			sptr++;			
			while (Text::CharUtil::PtrIsWS(&sptr));
			val2 = this->Evals(&sptr, sess, 0, 0, 0, valid);
			if (val2 == 0)
			{
				*valid = false;
				return 0;
			}
			if (!isDbl)
			{
				Int32 iVal;
				if (Text::StrToInt32(val2, &iVal))
				{
					iVal += Text::StrToInt32(sb.ToString());
					sb.ClearStr();
					sb.AppendI32(iVal);
					if (reader)
						reader->SetColumn(colIndex, colName?colName:val, DB::DBUtil::CT_Int32);
					*valPtr = sptr;
					return Text::StrCopyNew(sb.ToString());
				}
			}
			Double dVal;
			if (Text::StrToDouble(val2, &dVal))
			{
				dVal += Text::StrToDouble(sb.ToString());
				sb.ClearStr();
				Text::SBAppendF64(&sb, dVal);
				if (reader)
					reader->SetColumn(colIndex, colName?colName:val, DB::DBUtil::CT_Double);
				*valPtr = sptr;
				return Text::StrCopyNew(sb.ToString());
			}
			else
			{
				////////////////////////////////
				*valid = false;
				return 0;
			}
		}
		else if (*sptr == '-')
		{
			sptr++;			
			while (Text::CharUtil::PtrIsWS(&sptr));
			val2 = this->Evals(&sptr, sess, 0, 0, 0, valid);
			if (val2 == 0)
			{
				*valid = false;
				return 0;
			}
			if (!isDbl)
			{
				Int32 iVal;
				if (Text::StrToInt32(val2, &iVal))
				{
					iVal = Text::StrToInt32(sb.ToString()) - iVal;
					sb.ClearStr();
					sb.AppendI32(iVal);
					if (reader)
						reader->SetColumn(colIndex, colName?colName:val, DB::DBUtil::CT_Int32);
					*valPtr = sptr;
					return Text::StrCopyNew(sb.ToString());
				}
			}
			Double dVal;
			if (Text::StrToDouble(val2, &dVal))
			{
				dVal = Text::StrToDouble(sb.ToString()) - dVal;
				sb.ClearStr();
				Text::SBAppendF64(&sb, dVal);
				if (reader)
					reader->SetColumn(colIndex, colName?colName:val, DB::DBUtil::CT_Double);
				*valPtr = sptr;
				return Text::StrCopyNew(sb.ToString());
			}
			else
			{
				*valid = false;
				////////////////////////////////
				return 0;
			}
		}
		else if (*sptr == '*')
		{
			sptr++;			
			while (Text::CharUtil::PtrIsWS(&sptr));
			val2 = this->Evals(&sptr, sess, 0, 0, 0, valid);
			if (val2 == 0)
			{
				*valid = false;
				return 0;
			}
			if (!isDbl)
			{
				Int32 iVal;
				if (Text::StrToInt32(val2, &iVal))
				{
					iVal = Text::StrToInt32(sb.ToString()) * iVal;
					sb.ClearStr();
					sb.AppendI32(iVal);
					if (reader)
						reader->SetColumn(colIndex, colName?colName:val, DB::DBUtil::CT_Int32);
					*valPtr = sptr;
					return Text::StrCopyNew(sb.ToString());
				}
			}
			Double dVal;
			if (Text::StrToDouble(val2, &dVal))
			{
				dVal = Text::StrToDouble(sb.ToString()) * dVal;
				sb.ClearStr();
				Text::SBAppendF64(&sb, dVal);
				if (reader)
					reader->SetColumn(colIndex, colName?colName:val, DB::DBUtil::CT_Double);
				*valPtr = sptr;
				return Text::StrCopyNew(sb.ToString());
			}
			else
			{
				*valid = false;
				////////////////////////////////
				return 0;
			}
		}
		else if (*sptr == '/')
		{
			sptr++;			
			while (Text::CharUtil::PtrIsWS(&sptr));
			val2 = this->Evals(&sptr, sess, 0, 0, 0, valid);
			if (val2 == 0)
			{
				*valid = false;
				return 0;
			}
			if (!isDbl)
			{
				Int32 iVal;
				if (Text::StrToInt32(val2, &iVal))
				{
					iVal = Text::StrToInt32(sb.ToString()) / iVal;
					sb.ClearStr();
					sb.AppendI32(iVal);
					if (reader)
						reader->SetColumn(colIndex, colName?colName:val, DB::DBUtil::CT_Int32);
					*valPtr = sptr;
					return Text::StrCopyNew(sb.ToString());
				}
			}
			Double dVal;
			if (Text::StrToDouble(val2, &dVal))
			{
				dVal = Text::StrToDouble(sb.ToString()) / dVal;
				sb.ClearStr();
				Text::SBAppendF64(&sb, dVal);
				if (reader)
					reader->SetColumn(colIndex, colName?colName:val, DB::DBUtil::CT_Double);
				*valPtr = sptr;
				return Text::StrCopyNew(sb.ToString());
			}
			else
			{
				*valid = false;
				////////////////////////////////
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
				sb.AppendChar(c, 1);
			}
			else if (c == '_' || c == '.')
			{
				sb.AppendChar(c, 1);
			}
			else if (c >= '0' && c <= '9')
			{
				sb.AppendChar(c, 1);
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
					if (sb.EqualsICase((const UTF8Char*)"CONCAT"))
					{
						sb.ClearStr();
						const UTF8Char *sVal;
						Bool v = true;
						sptr++;
						while (true)
						{
							while (Text::CharUtil::PtrIsWS(&sptr));
							sVal = this->Evals(&sptr, sess, 0, 0, 0, &v);
							if (!v)
							{
								*valid = false;
								return 0;
							}
							sb.Append(sVal);
							while (Text::CharUtil::PtrIsWS(&sptr));
							if (sptr[0] == ')')
							{
								if (reader)
									reader->SetColumn(colIndex, colName?colName:val, DB::DBUtil::CT_Double);
								*valPtr = sptr + 1;
								return Text::StrCopyNew(sb.ToString());
							}
							else if (sptr[0] == ',')
							{
								sptr++;
							}
							else
							{
								*valid = false;

								sb.ClearStr();
								sb.Append((const UTF8Char*)"#42S22Unknown column '");
								sb.Append(val);
								sb.Append((const UTF8Char*)"' in field list");
								SDEL_TEXT(sess->lastError);
								sess->lastError = Text::StrCopyNew(sb.ToString());

								return 0;
							}
						}
					}
					else if (sb.EqualsICase((const UTF8Char*)"DATABASE"))
					{
						sptr++;
						while (Text::CharUtil::PtrIsWS(&sptr));
						if (sptr[0] != ')')
						{
							*valid = false;

							sb.ClearStr();
							sb.Append((const UTF8Char*)"#42S22Unknown column '");
							sb.Append(sptr);
							sb.Append((const UTF8Char*)"' in field list");
							SDEL_TEXT(sess->lastError);
							sess->lastError = Text::StrCopyNew(sb.ToString());

							return 0;
						}
						else
						{
							*valPtr = sptr + 1;
							if (sess->database)
							{
								return Text::StrCopyNew(sess->database);
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
		sb.Append((const UTF8Char*)"#42S22Unknown column '");
		sb.Append(val);
		sb.Append((const UTF8Char*)"' in field list");
		SDEL_TEXT(sess->lastError);
		sess->lastError = Text::StrCopyNew(sb.ToString());

		return 0;
	}
}

DB::DBMS::DBMS(const UTF8Char *versionStr, IO::LogTool *log)
{
	NEW_CLASS(this->loginMap, Data::StringUTF8Map<DB::DBMS::LoginInfo*>());
	NEW_CLASS(this->loginSHA1, Crypto::Hash::SHA1());
	NEW_CLASS(this->loginMut, Sync::Mutex());
	NEW_CLASS(this->sessMut, Sync::Mutex());
	NEW_CLASS(this->sessMap, Data::Int32Map<DB::DBMS::SessionInfo*>());
	this->versionStr = Text::StrCopyNew(versionStr);
	this->log = log;
}

DB::DBMS::~DBMS()
{
	Text::StrDelNew(this->versionStr);
	Data::ArrayList<DB::DBMS::LoginInfo*> *loginList = this->loginMap->GetValues();
	DB::DBMS::LoginInfo *login;
	DB::DBMS::UserInfo *user;
	UOSInt i = loginList->GetCount();
	UOSInt j;
	while (i-- > 0)
	{
		login = loginList->GetItem(i);
		j = login->userList->GetCount();
		while (j-- > 0)
		{
			user = login->userList->GetItem(j);
			MemFree(user);
		}
		DEL_CLASS(login->userList);
		Text::StrDelNew(login->login);
		MemFree(login);
	}
	DEL_CLASS(this->loginMap);
	DEL_CLASS(this->loginSHA1);
	DEL_CLASS(this->loginMut);

	Data::ArrayList<DB::DBMS::SessionInfo*> *sessList;
	sessList = this->sessMap->GetValues();
	i = sessList->GetCount();
	while (i-- > 0)
	{
		this->SessDelete(sessList->GetItem(i));
	}
	DEL_CLASS(this->sessMap);
	DEL_CLASS(this->sessMut);
}

const UTF8Char *DB::DBMS::GetVersion()
{
	return this->versionStr;
}

IO::LogTool *DB::DBMS::GetLogTool()
{
	return this->log;
}

Bool DB::DBMS::UserAdd(Int32 userId, const UTF8Char *userName, const UTF8Char *password, const UTF8Char *host)
{
	DB::DBMS::LoginInfo *login;
	DB::DBMS::UserInfo *user;
	OSInt i;
	Bool succ;
	#if defined(VERBOSE)
	printf("UserAdd %s/%s@%s\r\n", userName, password, host);
	#endif
	Sync::MutexUsage mutUsage(this->loginMut);
	login = this->loginMap->Get(userName);
	if (login == 0)
	{
		login = MemAlloc(DB::DBMS::LoginInfo, 1);
		login->login = Text::StrCopyNew(userName);
		NEW_CLASS(login->userList, Data::ArrayList<DB::DBMS::UserInfo*>());
		this->loginMap->Put(userName, login);

		user = MemAlloc(DB::DBMS::UserInfo, 1);
		Text::StrConcat(user->host, host);
		this->loginSHA1->Clear();
		this->loginSHA1->Calc(password, Text::StrCharCnt(password));
		this->loginSHA1->GetValue(user->pwdSha1);
		user->userId = userId;
		login->userList->Add(user);
		succ = true;
	}
	else
	{
		succ = true;
		i = login->userList->GetCount();
		while (i-- > 0)
		{
			user = login->userList->GetItem(i);
			if (Text::StrEquals(user->host, host))
			{
				succ = false;
				break;
			}
		}
		if (succ)
		{
			user = MemAlloc(DB::DBMS::UserInfo, 1);
			Text::StrConcat(user->host, host);
			this->loginSHA1->Clear();
			this->loginSHA1->Calc(password, Text::StrCharCnt(password));
			this->loginSHA1->GetValue(user->pwdSha1);
			user->userId = userId;
			login->userList->Add(user);
		}
	}
	mutUsage.EndUse();
	return succ;
}

Int32 DB::DBMS::UserLoginMySQL(Int32 sessId, const UTF8Char *userName, const UInt8 *randomData, const UInt8 *passHash, const Net::SocketUtil::AddressInfo *addr, const DB::DBMS::SessionParam *param, const UTF8Char *database)
{
	DB::DBMS::LoginInfo *login;
	DB::DBMS::UserInfo *user;
	UInt8 hashBuff[20];
	UOSInt j;
	Int32 userId = 0;
	#if defined(VERBOSE)
	printf("mysql_native_password auth\r\n");
	#endif
	Sync::MutexUsage mutUsage(this->loginMut);
	login = this->loginMap->Get(userName);
	if (login)
	{
		UOSInt i = login->userList->GetCount();
		while (i-- > 0)
		{
			user = login->userList->GetItem(i);
			this->loginSHA1->Clear();
			this->loginSHA1->Calc(user->pwdSha1, 20);
			this->loginSHA1->GetValue(hashBuff);

			this->loginSHA1->Clear();
			this->loginSHA1->Calc(randomData, 20);
			this->loginSHA1->Calc(hashBuff, 20);
			this->loginSHA1->GetValue(hashBuff);
			
			j = 0;
			while (j < 20)
			{
				hashBuff[j] ^= user->pwdSha1[j];
				j++;
			}

			#if defined(VERBOSE)
			Text::StringBuilderUTF8 sb;
			sb.AppendHexBuff(hashBuff, 20, ' ', Text::LBT_NONE);
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
				DB::DBMS::SessionInfo *sess;
				Sync::MutexUsage mutUsage(this->sessMut);
				sess = this->sessMap->Get(sessId);
				if (sess == 0)
				{
					sess = MemAlloc(DB::DBMS::SessionInfo, 1);
					sess->sessId = sessId;
					sess->lastError = 0;
					sess->autoCommit = 1;
					sess->autoIncInc = 1;
					sess->sqlModes = (DB::DBMS::SQLMODE)(SQLM_ONLY_FULL_GROUP_BY | SQLM_STRICT_TRANS_TABLES | SQLM_NO_ZERO_IN_DATE | SQLM_NO_ZERO_DATE | SQLM_ERROR_FOR_DIVISION_BY_ZERO | SQLM_NO_ENGINE_SUBSTITUTION);
					sess->database = 0;
					if (database && database[0])
					{
						sess->database = Text::StrCopyNew(database);
					}
					MemCopyNO(&sess->params, param, sizeof(DB::DBMS::SessionParam));
					NEW_CLASS(sess->userVars, Data::StringUTF8Map<const UTF8Char*>());
					this->sessMap->Put(sessId, sess);
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

DB::DBReader *DB::DBMS::ExecuteReader(Int32 sessId, const UTF8Char *sql)
{
	const UTF8Char *sptr1;
	const UTF8Char *sptr2;
	const UTF8Char *sptr3;
	UOSInt i;
	UOSInt j;
	UTF8Char nameBuff[128];
	UTF8Char nameBuff2[128];
	DB::DBMS::SessionInfo *sess;
	sess = this->SessGet(sessId);
	if (sess == 0)
	{
		return 0;
	}
	sptr1 = sql;
	while (Text::CharUtil::PtrIsWS(&sptr1));
	if (Text::StrStartsWithICase(sptr1, (const UTF8Char*)"SELECT") && Text::CharUtil::IsWS(sptr1 + 6))
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
					SDEL_TEXT(col->name);
					SDEL_TEXT(col->asName);
					MemFree(col);
				}

				Text::StringBuilderUTF8 sb;
				sb.Append((const UTF8Char*)"#42000You have an error in your SQL syntax; check the manual that corresponds to your MySQL server version for the right syntax to use near '");
				sb.Append(sptr2);
				sb.Append((const UTF8Char*)"' at line 1");
				SDEL_TEXT(sess->lastError);
				sess->lastError = Text::StrCopyNew(sb.ToString());
				return 0;
			}
			while (Text::CharUtil::PtrIsWS(&sptr1));
			if (*sptr1 == ',' || *sptr1 == 0)
			{
				col = MemAlloc(DB::DBMS::SQLColumn, 1);
				col->name = Text::StrCopyNew(nameBuff);
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
			else if (Text::StrStartsWithICase(sptr1, (const UTF8Char*)"AS") && Text::CharUtil::IsWS(sptr1 + 2))
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
						SDEL_TEXT(col->name);
						SDEL_TEXT(col->asName);
						MemFree(col);
					}

					Text::StringBuilderUTF8 sb;
					sb.Append((const UTF8Char*)"#42000You have an error in your SQL syntax; check the manual that corresponds to your MySQL server version for the right syntax to use near '");
					sb.Append(sptr3);
					sb.Append((const UTF8Char*)"' at line 1");
					SDEL_TEXT(sess->lastError);
					sess->lastError = Text::StrCopyNew(sb.ToString());
					return 0;
				}
				while (Text::CharUtil::PtrIsWS(&sptr1));
				if (*sptr1 == ',' || *sptr1 == 0)
				{
					col = MemAlloc(DB::DBMS::SQLColumn, 1);
					col->name = Text::StrCopyNew(nameBuff);
					col->asName = Text::StrCopyNew(nameBuff2);
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
				else if (Text::StrStartsWithICase(sptr1, (const UTF8Char*)"FROM") && Text::CharUtil::IsWS(sptr1 + 4))
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
						SDEL_TEXT(col->name);
						SDEL_TEXT(col->asName);
						MemFree(col);
					}

					Text::StringBuilderUTF8 sb;
					sb.Append((const UTF8Char*)"#42000You have an error in your SQL syntax; check the manual that corresponds to your MySQL server version for the right syntax to use near '");
					sb.Append(sptr1);
					sb.Append((const UTF8Char*)"' at line 1");
					SDEL_TEXT(sess->lastError);
					sess->lastError = Text::StrCopyNew(sb.ToString());
					return 0;
				}
			}
			else if (Text::StrStartsWithICase(sptr1, (const UTF8Char*)"FROM") && Text::CharUtil::IsWS(sptr1 + 4))
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
						SDEL_TEXT(col->name);
						SDEL_TEXT(col->asName);
						MemFree(col);
					}

					Text::StringBuilderUTF8 sb;
					sb.Append((const UTF8Char*)"#42000You have an error in your SQL syntax; check the manual that corresponds to your MySQL server version for the right syntax to use near '");
					sb.Append(sptr3);
					sb.Append((const UTF8Char*)"' at line 1");
					SDEL_TEXT(sess->lastError);
					sess->lastError = Text::StrCopyNew(sb.ToString());
					return 0;
				}
				while (Text::CharUtil::PtrIsWS(&sptr1));
				if (*sptr1 == ',' || *sptr1 == 0)
				{
					col = MemAlloc(DB::DBMS::SQLColumn, 1);
					col->name = Text::StrCopyNew(nameBuff);
					col->asName = Text::StrCopyNew(nameBuff2);
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
				else if (Text::StrStartsWithICase(sptr1, (const UTF8Char*)"FROM") && Text::CharUtil::IsWS(sptr1 + 4))
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
						SDEL_TEXT(col->name);
						SDEL_TEXT(col->asName);
						MemFree(col);
					}

					Text::StringBuilderUTF8 sb;
					sb.Append((const UTF8Char*)"#42000You have an error in your SQL syntax; check the manual that corresponds to your MySQL server version for the right syntax to use near '");
					sb.Append(sptr1);
					sb.Append((const UTF8Char*)"' at line 1");
					SDEL_TEXT(sess->lastError);
					sess->lastError = Text::StrCopyNew(sb.ToString());
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
				SDEL_TEXT(col->name);
				SDEL_TEXT(col->asName);
				MemFree(col);
			}

			Text::StringBuilderUTF8 sb;
			sb.Append((const UTF8Char*)"#00000Unsupported syntax 'FROM'");
			SDEL_TEXT(sess->lastError);
			sess->lastError = Text::StrCopyNew(sb.ToString());

			return 0;
		}
		else
		{
			Bool valid = true;
			DB::DBMSReader *reader;
			Text::StringBuilderUTF8 sb;
			const UTF8Char **colVals;
			const UTF8Char *val;
			NEW_CLASS(reader, DB::DBMSReader(cols.GetCount(), -1));
			colVals = MemAlloc(const UTF8Char *, cols.GetCount());
			i = 0;
			j = cols.GetCount();
			while (i < j && valid)
			{
				col = cols.GetItem(i);
				val = col->name;
				#if defined(VERBOSE)
				printf("Column %d is %s\r\n", (int)i, val);
				#endif
				colVals[i] = this->Evals(&val, sess, reader, i, col->asName, &valid);

				SDEL_TEXT(col->name);
				SDEL_TEXT(col->asName);
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
				SDEL_TEXT(col->name);
				SDEL_TEXT(col->asName);
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
					SDEL_TEXT(colVals[i]);
				}
				MemFree(colVals);
				DEL_CLASS(reader);
				return 0;
			}
		}
	}
	else if (Text::StrStartsWithICase(sptr1, (const UTF8Char*)"SET") && Text::CharUtil::IsWS(sptr1 + 3))
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
						if (Text::StrEqualsICase(nameBuff, (const UTF8Char*)"GLOBAL"))
						{
							isGlobal = true;
							namePtr = nameBuff;
						}
						else if (Text::StrEqualsICase(nameBuff, (const UTF8Char*)"SESSION"))
						{
							namePtr = nameBuff;
						}
						else
						{
							i = cols.GetCount();
							while (i-- > 0)
							{
								col = cols.GetItem(i);
								SDEL_TEXT(col->name);
								SDEL_TEXT(col->asName);
								MemFree(col);
							}

							Text::StringBuilderUTF8 sb;
							sb.Append((const UTF8Char*)"#00000Unsupported syntax 'FROM'");
							SDEL_TEXT(sess->lastError);
							sess->lastError = Text::StrCopyNew(sb.ToString());
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
								SDEL_TEXT(col->name);
								SDEL_TEXT(col->asName);
								MemFree(col);
							}

							Text::StringBuilderUTF8 sb;
							sb.Append((const UTF8Char*)"#00000Unsupported syntax 'FROM'");
							SDEL_TEXT(sess->lastError);
							sess->lastError = Text::StrCopyNew(sb.ToString());
							return 0;
						}
					}
				}
				*namePtr = 0;
				if (this->SysVarExist(sess, nameBuff, isGlobal?(DB::DBMS::AT_SET_GLOBAL):(DB::DBMS::AT_SET_SESSION)))
				{
					while (Text::CharUtil::PtrIsWS(&sptr1));
					Bool valid = true;
					const UTF8Char *val = this->Evals(&sptr1, sess, 0, 0, 0, &valid);
					if (!valid)
					{
						i = cols.GetCount();
						while (i-- > 0)
						{
							col = cols.GetItem(i);
							SDEL_TEXT(col->name);
							SDEL_TEXT(col->asName);
							MemFree(col);
						}

						return 0;
					}
					nameBuff2[0] = isGlobal?'!':'#';
					Text::StrConcat(&nameBuff2[1], nameBuff);
					col = MemAlloc(DB::DBMS::SQLColumn, 1);
					col->name = Text::StrCopyNew(nameBuff2);
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
							if (col->name[0] == '!')
							{
								this->SysVarSet(sess, true, col->name + 1, col->asName);
							}
							else if (col->name[0] == '@')
							{
								this->UserVarSet(sess, col->name + 1,  col->asName);
							}
							else if (col->name[0] == '#')
							{
								this->SysVarSet(sess, false, col->name + 1, col->asName);
							}
							SDEL_TEXT(col->name);
							SDEL_TEXT(col->asName);
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
					sb.Append((const UTF8Char*)"#00000Unsupported syntax 'FROM'");
					SDEL_TEXT(sess->lastError);
					sess->lastError = Text::StrCopyNew(sb.ToString());
					return 0;
				}
			}
			else if (sptr1[0] == '@')
			{
				Text::StringBuilderUTF8 sb;
				Bool valid = true;
				UTF8Char c;
				sb.AppendChar('@', 1);

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
								sb.Append((const UTF8Char*)"#00000Unsupported syntax '");
								sb.Append(sptr1);
								sb.AppendChar('\'', 1);
								SDEL_TEXT(sess->lastError);
								sess->lastError = Text::StrCopyNew(sb.ToString());
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
								sb.Append((const UTF8Char*)"#00000Unsupported syntax '");
								sb.Append(sptr1);
								sb.AppendChar('\'', 1);
								SDEL_TEXT(sess->lastError);
								sess->lastError = Text::StrCopyNew(sb.ToString());
								break;
							}
							else if (c == 't')
							{
								sb.AppendChar('\t', 1);
							}
							else if (c == 'n')
							{
								sb.AppendChar('\n', 1);
							}
							else if (c == 'r')
							{
								sb.AppendChar('\r', 1);
							}
							else
							{
								sb.AppendChar(c, 1);
							}
						}
						else if (c == 0)
						{
							valid = false;

							sb.ClearStr();
							sb.Append((const UTF8Char*)"#00000Unsupported syntax '");
							sb.Append(sptr1);
							sb.AppendChar('\'', 1);
							SDEL_TEXT(sess->lastError);
							sess->lastError = Text::StrCopyNew(sb.ToString());
							break;

						}
						else
						{
							sb.AppendChar(c, 1);
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
								sb.Append((const UTF8Char*)"#00000Unsupported syntax '");
								sb.Append(sptr1);
								sb.AppendChar('\'', 1);
								SDEL_TEXT(sess->lastError);
								sess->lastError = Text::StrCopyNew(sb.ToString());
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
								sb.Append((const UTF8Char*)"#00000Unsupported syntax '");
								sb.Append(sptr1);
								sb.AppendChar('\'', 1);
								SDEL_TEXT(sess->lastError);
								sess->lastError = Text::StrCopyNew(sb.ToString());
								break;
							}
							else if (c == 't')
							{
								sb.AppendChar('\t', 1);
							}
							else if (c == 'n')
							{
								sb.AppendChar('\n', 1);
							}
							else if (c == 'r')
							{
								sb.AppendChar('\r', 1);
							}
							else
							{
								sb.AppendChar(c, 1);
							}
						}
						else if (c == 0)
						{
							valid = false;

							sb.ClearStr();
							sb.Append((const UTF8Char*)"#00000Unsupported syntax '");
							sb.Append(sptr1);
							sb.AppendChar('\'', 1);
							SDEL_TEXT(sess->lastError);
							sess->lastError = Text::StrCopyNew(sb.ToString());
							break;

						}
						else
						{
							sb.AppendChar(c, 1);
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
							sb.AppendChar(c, 1);
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
								sb.Append((const UTF8Char*)"#00000Unsupported syntax '");
								sb.Append(sptr1);
								sb.AppendChar('\'', 1);
								SDEL_TEXT(sess->lastError);
								sess->lastError = Text::StrCopyNew(sb.ToString());
								break;
							}
						}
					}
				}
				else
				{
					valid = false;

					sb.ClearStr();
					sb.Append((const UTF8Char*)"#00000Unsupported syntax '");
					sb.Append(sptr1);
					sb.AppendChar('\'', 1);
					SDEL_TEXT(sess->lastError);
					sess->lastError = Text::StrCopyNew(sb.ToString());
				}
				if (valid)
				{
					while (Text::CharUtil::PtrIsWS(&sptr1));
					const UTF8Char *val = this->Evals(&sptr1, sess, 0, 0, 0, &valid);
					if (!valid)
					{
						i = cols.GetCount();
						while (i-- > 0)
						{
							col = cols.GetItem(i);
							SDEL_TEXT(col->name);
							SDEL_TEXT(col->asName);
							MemFree(col);
						}


						sb.ClearStr();
						sb.Append((const UTF8Char*)"#00000Unsupported syntax '");
						sb.Append(sptr1);
						sb.AppendChar('\'', 1);
						SDEL_TEXT(sess->lastError);
						sess->lastError = Text::StrCopyNew(sb.ToString());
						return 0;
					}
					col = MemAlloc(DB::DBMS::SQLColumn, 1);
					col->name = Text::StrCopyNew(sb.ToString());
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
							if (col->name[0] == '!')
							{
								this->SysVarSet(sess, true, col->name + 1, col->asName);
							}
							else if (col->name[0] == '@')
							{
								this->UserVarSet(sess, col->name + 1,  col->asName);
							}
							else if (col->name[0] == '#')
							{
								this->SysVarSet(sess, false, col->name + 1, col->asName);
							}
							SDEL_TEXT(col->name);
							SDEL_TEXT(col->asName);
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
						SDEL_TEXT(col->name);
						SDEL_TEXT(col->asName);
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
								SDEL_TEXT(col->name);
								SDEL_TEXT(col->asName);
								MemFree(col);
							}

							Text::StringBuilderUTF8 sb;
							sb.Append((const UTF8Char*)"#00000Unsupported syntax 'FROM'");
							SDEL_TEXT(sess->lastError);
							sess->lastError = Text::StrCopyNew(sb.ToString());
							return 0;
						}
					}
				}
				*namePtr = 0;
				if (this->SysVarExist(sess, nameBuff + 1, DB::DBMS::AT_SET_SESSION))
				{
					while (Text::CharUtil::PtrIsWS(&sptr1));
					Bool valid = true;
					const UTF8Char *val = this->Evals(&sptr1, sess, 0, 0, 0, &valid);
					if (!valid)
					{
						i = cols.GetCount();
						while (i-- > 0)
						{
							col = cols.GetItem(i);
							SDEL_TEXT(col->name);
							SDEL_TEXT(col->asName);
							MemFree(col);
						}

						return 0;
					}
					col = MemAlloc(DB::DBMS::SQLColumn, 1);
					col->name = Text::StrCopyNew(nameBuff);
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
							if (col->name[0] == '!')
							{
								this->SysVarSet(sess, true, col->name + 1, col->asName);
							}
							else if (col->name[0] == '@')
							{
								this->UserVarSet(sess, col->name + 1,  col->asName);
							}
							else if (col->name[0] == '#')
							{
								this->SysVarSet(sess, false, col->name + 1, col->asName);
							}
							SDEL_TEXT(col->name);
							SDEL_TEXT(col->asName);
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
						SDEL_TEXT(col->name);
						SDEL_TEXT(col->asName);
						MemFree(col);
					}

					Text::StringBuilderUTF8 sb;
					sb.Append((const UTF8Char*)"#00000Unsupported syntax 'FROM'");
					SDEL_TEXT(sess->lastError);
					sess->lastError = Text::StrCopyNew(sb.ToString());
					return 0;
				}
			}
			else
			{
				i = cols.GetCount();
				while (i-- > 0)
				{
					col = cols.GetItem(i);
					SDEL_TEXT(col->name);
					SDEL_TEXT(col->asName);
					MemFree(col);
				}

				Text::StringBuilderUTF8 sb;
				sb.Append((const UTF8Char*)"#00000Unsupported syntax 'FROM'");
				SDEL_TEXT(sess->lastError);
				sess->lastError = Text::StrCopyNew(sb.ToString());
				return 0;
			}
		}
	}
	else if (Text::StrStartsWithICase(sptr1, (const UTF8Char*)"SHOW") && Text::CharUtil::IsWS(sptr1 + 4))
	{
		sptr1 += 4;
		while (Text::CharUtil::PtrIsWS(&sptr1));
		if (Text::StrStartsWithICase(sptr1, (const UTF8Char*)"ENGINES") && (Text::CharUtil::IsWS(sptr1 + 7) || sptr1[7] == 0))
		{
			////////////////////////
			Text::StringBuilderUTF8 sb;
			sb.Append((const UTF8Char*)"#42000You have an error in your SQL syntax; check the manual that corresponds to your MySQL server version for the right syntax to use near '");
			sb.Append(sptr1);
			sb.Append((const UTF8Char*)"' at line 1");
			SDEL_TEXT(sess->lastError);
			sess->lastError = Text::StrCopyNew(sb.ToString());
		}
		else if (Text::StrStartsWithICase(sptr1, (const UTF8Char*)"CHARSET") && (Text::CharUtil::IsWS(sptr1 + 7) || sptr1[7] == 0))
		{
			////////////////////////
			Text::StringBuilderUTF8 sb;
			sb.Append((const UTF8Char*)"#42000You have an error in your SQL syntax; check the manual that corresponds to your MySQL server version for the right syntax to use near '");
			sb.Append(sptr1);
			sb.Append((const UTF8Char*)"' at line 1");
			SDEL_TEXT(sess->lastError);
			sess->lastError = Text::StrCopyNew(sb.ToString());
		}
		else if (Text::StrStartsWithICase(sptr1, (const UTF8Char*)"COLLATION") && (Text::CharUtil::IsWS(sptr1 + 9) || sptr1[9] == 0))
		{
			////////////////////////
			Text::StringBuilderUTF8 sb;
			sb.Append((const UTF8Char*)"#42000You have an error in your SQL syntax; check the manual that corresponds to your MySQL server version for the right syntax to use near '");
			sb.Append(sptr1);
			sb.Append((const UTF8Char*)"' at line 1");
			SDEL_TEXT(sess->lastError);
			sess->lastError = Text::StrCopyNew(sb.ToString());
		}
		else if (Text::StrStartsWithICase(sptr1, (const UTF8Char*)"VARIABLES") && (Text::CharUtil::IsWS(sptr1 + 9) || sptr1[9] == 0))
		{
			sptr1 += 9;
			while (Text::CharUtil::PtrIsWS(&sptr1));
			
			if (sptr1[0] == 0)
			{
				DB::DBMSReader *reader;
				OSInt i;
				OSInt j;
				const UTF8Char *row[2];
				Text::StringBuilderUTF8 sb;
				NEW_CLASS(reader, DB::DBMSReader(2, -1));
				reader->SetColumn(0, (const UTF8Char*)"Variablename", DB::DBUtil::CT_VarChar);
				reader->SetColumn(1, (const UTF8Char*)"Value", DB::DBUtil::CT_VarChar);
				i = 0;
				j = sizeof(sysVarList) / sizeof(sysVarList[0]);
				while (i < j)
				{
					row[0] = Text::StrCopyNew((const UTF8Char*)sysVarList[i]);
					sb.ClearStr();
					SysVarGet(&sb, sess, row[0]);
					row[1] = Text::StrCopyNew(sb.ToString());
					reader->AddRow(row);
					i++;
				}
				return reader;
			}
			else if (Text::StrStartsWithICase(sptr1, (const UTF8Char*)"LIKE") && Text::CharUtil::IsWS(sptr1 + 4))
			{
				sptr1 += 4;
				while (Text::CharUtil::PtrIsWS(&sptr1));

				if (sptr1[0] == '\'')
				{
					Bool valid = true;
 					const UTF8Char *val = this->Evals(&sptr1, sess, 0, 0, 0, &valid);
					if (!valid)
					{
						return 0;
					}

					while (Text::CharUtil::PtrIsWS(&sptr1));
					if (sptr1[0] == 0 && val != 0)
					{
						DB::DBMSReader *reader;
						OSInt i;
						OSInt j;
						const UTF8Char *row[2];
						Text::StringBuilderUTF8 sb;
						NEW_CLASS(reader, DB::DBMSReader(2, -1));
						reader->SetColumn(0, (const UTF8Char*)"Variablename", DB::DBUtil::CT_VarChar);
						reader->SetColumn(1, (const UTF8Char*)"Value", DB::DBUtil::CT_VarChar);
						i = 0;
						j = sizeof(sysVarList) / sizeof(sysVarList[0]);
						while (i < j)
						{
							if (StrLike((const UTF8Char*)sysVarList[i], val))
							{
								sb.ClearStr();
								row[0] = Text::StrCopyNew((const UTF8Char*)sysVarList[i]);
								SysVarGet(&sb, sess, row[0]);
								row[1] = Text::StrCopyNew(sb.ToString());
								reader->AddRow(row);
							}
							i++;
						}
						SDEL_TEXT(val);
						return reader;

					}
					else
					{
						SDEL_TEXT(val);

						Text::StringBuilderUTF8 sb;
						sb.Append((const UTF8Char*)"#42000You have an error in your SQL syntax; check the manual that corresponds to your MySQL server version for the right syntax to use near '");
						sb.Append(sptr1);
						sb.Append((const UTF8Char*)"' at line 1");
						SDEL_TEXT(sess->lastError);
						sess->lastError = Text::StrCopyNew(sb.ToString());
						
						return 0;
					}
				}
				else
				{
					Text::StringBuilderUTF8 sb;
					sb.Append((const UTF8Char*)"#42000You have an error in your SQL syntax; check the manual that corresponds to your MySQL server version for the right syntax to use near '");
					sb.Append(sptr1);
					sb.Append((const UTF8Char*)"' at line 1");
					SDEL_TEXT(sess->lastError);
					sess->lastError = Text::StrCopyNew(sb.ToString());
				}
			}
			else
			{
				Text::StringBuilderUTF8 sb;
				sb.Append((const UTF8Char*)"#42000You have an error in your SQL syntax; check the manual that corresponds to your MySQL server version for the right syntax to use near '");
				sb.Append(sptr1);
				sb.Append((const UTF8Char*)"' at line 1");
				SDEL_TEXT(sess->lastError);
				sess->lastError = Text::StrCopyNew(sb.ToString());
			}
		}
		else
		{
			Text::StringBuilderUTF8 sb;
			sb.Append((const UTF8Char*)"#42000You have an error in your SQL syntax; check the manual that corresponds to your MySQL server version for the right syntax to use near '");
			sb.Append(sptr1);
			sb.Append((const UTF8Char*)"' at line 1");
			SDEL_TEXT(sess->lastError);
			sess->lastError = Text::StrCopyNew(sb.ToString());
		}
	}
	else
	{
		Text::StringBuilderUTF8 sb;
		sb.Append((const UTF8Char*)"#42000You have an error in your SQL syntax; check the manual that corresponds to your MySQL server version for the right syntax to use near '");
		sb.Append(sptr1);
		sb.Append((const UTF8Char*)"' at line 1");
		SDEL_TEXT(sess->lastError);
		sess->lastError = Text::StrCopyNew(sb.ToString());
	}
	return 0;
}

void DB::DBMS::CloseReader(DB::DBReader *r)
{

}

UTF8Char *DB::DBMS::GetErrMessage(Int32 sessId, UTF8Char *msgBuff)
{
	DB::DBMS::SessionInfo *sess;
	Sync::MutexUsage mutUsage(this->sessMut);
	sess = this->sessMap->Get(sessId);
	if (sess && sess->lastError)
	{
		msgBuff = Text::StrConcat(msgBuff, sess->lastError);
	}
	mutUsage.EndUse();
	return msgBuff;
}

DB::DBMS::SessionInfo *DB::DBMS::SessGet(Int32 sessId)
{
	DB::DBMS::SessionInfo *sess;
	Sync::MutexUsage mutUsage(this->sessMut);
	sess = this->sessMap->Get(sessId);
	mutUsage.EndUse();
	return sess;
}

void DB::DBMS::SessEnd(Int32 sessId)
{
	DB::DBMS::SessionInfo *sess;
	if (sessId == 0)
	{
		return;
	}
	Sync::MutexUsage mutUsage(this->sessMut);
	sess = this->sessMap->Remove(sessId);
	mutUsage.EndUse();
	if (sess)
	{
		this->SessDelete(sess);
	}
}

void DB::DBMS::SessDelete(DB::DBMS::SessionInfo *sess)
{
	Data::ArrayList<const UTF8Char*> *varList = sess->userVars->GetValues();
	UOSInt i = varList->GetCount();
	const UTF8Char *var;
	while (i-- > 0)
	{
		var = varList->GetItem(i);
		SDEL_TEXT(var);
	}
	DEL_CLASS(sess->userVars);
	SDEL_TEXT(sess->lastError);
	SDEL_TEXT(sess->database);
	MemFree(sess);
}
