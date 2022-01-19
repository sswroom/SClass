#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ByteTool.h"
#include "Data/DateTime.h"
#include "DB/DBConn.h"
#include "DB/ODBCConn.h"
#include "DB/DBTool.h"
#include "Math/Math.h"
#include "Math/MSGeography.h"
#include "Math/Point.h"
#include "Math/WKTWriter.h"
#include "Text/Encoding.h"
#include "Text/MyString.h"
#include "Text/MyStringFloat.h"
#include "Text/MyStringW.h"
#include "Text/StringBuilderUTF8.h"
#ifndef _WIN32_WCE

#if defined(_WIN32) || defined(_WIN64) || defined(__CYGWIN__) || defined(__MINGW32__)
#include <windows.h>
#else
#include "IO/IniFile.h"
#endif
#if defined(__CYGWIN__)
#define ULONG ULONG_TMP
#endif
#include <sqlext.h>

#include <stdio.h>

void DB::ODBCConn::UpdateConnInfo()
{
	Char buff[256];
	Int16 buffSize;
	SQLRETURN ret;
	ret = SQLGetInfoA(connHand, SQL_DRIVER_NAME, buff, sizeof(buff), &buffSize);
	if (ret == SQL_SUCCESS || SQL_SUCCESS_WITH_INFO)
	{
		if (buffSize <= 0)
		{
			buff[0] = 0;
			buffSize = 0;
		}
	
		if (log)
		{
			Text::StringBuilderUTF8 sb;
			sb.AppendC(UTF8STRC("Driver is "));
			sb.AppendC((const UTF8Char*)buff, (UOSInt)buffSize);
			log->LogMessageC(sb.ToString(), sb.GetLength(), IO::ILogHandler::LOG_LEVEL_ACTION);
		}
//		Text::StrToLowerC(buff, buff);
		if (Text::StrStartsWith(buff, "myodbc"))
		{
			this->svrType = DB::DBUtil::ServerType::MySQL;
		}
		else if (Text::StrStartsWith(buff, "libmyodbc"))
		{
			this->svrType = DB::DBUtil::ServerType::MySQL;
		}
		else if (Text::StrStartsWith(buff, "SQLSVR"))
		{
			this->svrType = DB::DBUtil::ServerType::MSSQL;
		}
		else if (Text::StrStartsWith(buff, "SQLSRV"))
		{
			this->svrType = DB::DBUtil::ServerType::MSSQL;
		}
		else if (Text::StrStartsWith(buff, "SQORA"))
		{
			this->svrType = DB::DBUtil::ServerType::Oracle;
		}
		else if (Text::StrStartsWithICase(buff, "sqlncli"))
		{
			this->svrType = DB::DBUtil::ServerType::MSSQL;
		}
		else if (Text::StrIndexOf(buff, "sqlite") != INVALID_INDEX)
		{
			this->svrType = DB::DBUtil::ServerType::SQLite;
		}
		else if (Text::StrIndexOf(buff, "odbcjt32") != INVALID_INDEX)
		{
			this->svrType = DB::DBUtil::ServerType::Access;
		}
		else if (Text::StrStartsWith(buff, "ACEODBC"))
		{
			this->svrType = DB::DBUtil::ServerType::Access;
		}
		else if (Text::StrIndexOf(buff, "msodbcsql") != INVALID_INDEX)
		{
			this->svrType = DB::DBUtil::ServerType::MSSQL;
		}
		else
		{
		}
	}
	if (this->svrType == DB::DBUtil::ServerType::Unknown)
	{
		if (this->connStr->IndexOfICase((const UTF8Char*)"DRIVER=MDBTOOLS;") != INVALID_INDEX)
		{
			this->svrType = DB::DBUtil::ServerType::MDBTools;
		}
	}

	if (this->svrType == DB::DBUtil::ServerType::MSSQL)
	{
		DB::DBReader *r = this->ExecuteReaderC(UTF8STRC("select getdate(), GETUTCDATE()"));
		if (r)
		{
			Data::DateTime dt1;
			Data::DateTime dt2;
			r->ReadNext();
			r->GetDate(0, &dt1);
			r->GetDate(1, &dt2);
			this->CloseReader(r);

			Text::StringBuilderUTF8 sb;
			sb.AppendDate(&dt1);
			sb.AppendC(UTF8STRC(", "));
			sb.AppendDate(&dt2);
			this->tzQhr = (Int8)(dt1.DiffMS(&dt2) / 900000);
		}
	}
}

Bool DB::ODBCConn::Connect(Text::String *dsn, Text::String *uid, Text::String *pwd, Text::String *schema)
{
	SQLHANDLE hand;
	SQLHANDLE hConn;
	SQLRETURN ret;
	int timeOut = 5;
	this->connErr = CE_NONE;
	this->envHand = 0;
	this->connHand = 0;
	SDEL_STRING(this->lastErrorMsg);
	ret = SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &hand);
	if (ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO)
	{
		this->connErr = CE_ALLOC_ENV;
		return false;
	}

	ret = SQLSetEnvAttr(hand, SQL_ATTR_ODBC_VERSION, (void*)SQL_OV_ODBC3, 0);
	if (ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO)
	{
		SQLFreeHandle(SQL_HANDLE_ENV, hand);
		this->connErr = CE_SET_ENV;
		return false;
	}
	ret = SQLAllocHandle(SQL_HANDLE_DBC, hand, &hConn);
	if (ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO)
	{
		SQLFreeHandle(SQL_HANDLE_ENV, hand);
		this->connErr = CE_ALLOC_DBC;
		return false;
	}
	ret = SQLSetConnectAttr(hConn, SQL_LOGIN_TIMEOUT, (SQLPOINTER)&timeOut, 0);
	if (ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO)
	{
		SQLFreeHandle(SQL_HANDLE_DBC, hConn);
		SQLFreeHandle(SQL_HANDLE_ENV, hand);
		this->connErr = CE_SET_DBC;
		return false;
	}

	SQLCHAR *uidPtr = (uid != 0 && uid->leng > 0)?uid->v:0;
	SQLCHAR *pwdPtr = (pwd != 0 && pwd->leng > 0)?pwd->v:0;
	if (uidPtr || pwdPtr)
	{
		ret = SQLConnectA(hConn, (SQLCHAR*)dsn->v, SQL_NTS, (SQLCHAR*)uidPtr, SQL_NTS, (SQLCHAR*)pwdPtr, SQL_NTS);
	}
	else
	{
		ret = SQLConnectA(hConn, (SQLCHAR*)dsn->v, SQL_NTS, NULL, 0, NULL, 0);
	}
	if (ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO)
	{
		Text::StringBuilderUTF8 sb;
		SQLWCHAR state[6];
		SQLWCHAR msg[256];
		Int16 msgSize;
		Int32 errCode;
		ret = SQLGetDiagRecW(SQL_HANDLE_DBC, hConn, 1, (SQLWCHAR*)state, (SQLINTEGER*)&errCode, msg, 256, &msgSize);
		if (ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO)
		{
			sb.AppendC(UTF8STRC("["));
			state[5] = 0;
			Text::String *s = Text::String::NewNotNull((const UTF16Char*)state);
			sb.Append(s);
			s->Release();
			sb.AppendC(UTF8STRC("]"));
			if (msgSize > 255)
			{
				SQLWCHAR *tmpBuff = MemAlloc(SQLWCHAR, (UInt16)(msgSize + 1));
				ret = SQLGetDiagRecW(SQL_HANDLE_DBC, hConn, 1, state, (SQLINTEGER*)&errCode, tmpBuff, (Int16)(msgSize + 1), &msgSize);
				s = Text::String::NewNotNull(tmpBuff);
				sb.AppendC(s->v, s->leng);
				s->Release();
				MemFree(tmpBuff);				
			}
			else
			{
				s = Text::String::NewNotNull(msg);
				sb.AppendC(s->v, s->leng);
				s->Release();
			}
		}
		this->lastErrorMsg = Text::String::New(sb.ToString(), sb.GetLength());

		SQLFreeHandle(SQL_HANDLE_DBC, hConn);
		SQLFreeHandle(SQL_HANDLE_ENV, hand);
		this->connErr = CE_CONNECT_ERR;
		return false;
	}
	this->svrType = DB::DBUtil::ServerType::Unknown;
	envHand = hand;
	connHand = hConn;
	this->lastDataError = DE_NO_ERROR;

	UpdateConnInfo();

	if (schema && schema->leng > 0)
	{
		Text::StringBuilderUTF8 sb;
		sb.AppendC(UTF8STRC("use "));
		sb.Append(schema);
		this->ExecuteNonQueryC(sb.ToString(), sb.GetLength());
	}
	return true;
}

Bool DB::ODBCConn::Connect(Text::String *connStr)
{
	SQLHANDLE hand;
	SQLHANDLE hConn;
	SQLRETURN ret;
	int timeOut = 5;
	SDEL_STRING(this->connStr);
	this->connStr = connStr->Clone();
	SDEL_STRING(this->lastErrorMsg);

	this->connErr = CE_NONE;
	ret = SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &hand);
	if (ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO)
	{
		this->connErr = CE_ALLOC_ENV;
		return false;
	}

	ret = SQLSetEnvAttr(hand, SQL_ATTR_ODBC_VERSION, (void*)SQL_OV_ODBC3, 0);
	if (ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO)
	{
		SQLFreeHandle(SQL_HANDLE_ENV, hand);
		this->connErr = CE_SET_ENV;
		return false;
	}
	ret = SQLAllocHandle(SQL_HANDLE_DBC, hand, &hConn);
	if (ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO)
	{
		SQLFreeHandle(SQL_HANDLE_ENV, hand);
		this->connErr = CE_ALLOC_DBC;
		return false;
	}
	ret = SQLSetConnectAttr(hConn, SQL_LOGIN_TIMEOUT, (SQLPOINTER)&timeOut, 0);
	if (ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO)
	{
		SQLFreeHandle(SQL_HANDLE_DBC, hConn);
		SQLFreeHandle(SQL_HANDLE_ENV, hand);
		this->connErr = CE_SET_DBC;
		return false;
	}
//	printf("ODBC Connect: %s\r\n", connStr);
	SQLSMALLINT outSize;
	SQLWCHAR *connBuff = MemAlloc(SQLWCHAR, Text::StrUTF8_UTF16CntC(connStr->v, connStr->leng) + 2);
	SQLWCHAR *connEnd = Text::StrUTF8_UTF16C(connBuff, connStr->v, connStr->leng, 0);
	connEnd[0] = 0;
	connEnd[1] = 0;
	ret = SQLDriverConnectW(hConn, 0, connBuff, (SQLSMALLINT)(connEnd - connBuff), NULL, 0, &outSize, 0);
	MemFree(connBuff);

	if (ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO)
	{
		Text::StringBuilderUTF8 sb;
		SQLWCHAR state[6];
		SQLWCHAR msg[256];
		Int16 msgSize;
		Int32 errCode;
		ret = SQLGetDiagRecW(SQL_HANDLE_DBC, hConn, 1, (SQLWCHAR*)state, (SQLINTEGER*)&errCode, msg, 256, &msgSize);
		if (ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO)
		{
			sb.AppendC(UTF8STRC("["));
			state[5] = 0;
			Text::String *s = Text::String::NewNotNull((const UTF16Char*)state);
			sb.Append(s);
			s->Release();
			sb.AppendC(UTF8STRC("]"));
			if (msgSize > 255)
			{
				SQLWCHAR *tmpBuff = MemAlloc(SQLWCHAR, (UInt16)(msgSize + 1));
				ret = SQLGetDiagRecW(SQL_HANDLE_DBC, hConn, 1, state, (SQLINTEGER*)&errCode, tmpBuff, (Int16)(msgSize + 1), &msgSize);
				s = Text::String::NewNotNull(tmpBuff);
				sb.Append(s);
				s->Release();
				MemFree(tmpBuff);				
			}
			else
			{
				s = Text::String::NewNotNull(msg);
				sb.Append(s);
				s->Release();
			}
		}
		this->lastErrorMsg = Text::String::New(sb.ToString(), sb.GetLength());
//		printf("ODBC Connect error: %s\r\n", sb.ToString());

		SQLFreeHandle(SQL_HANDLE_DBC, hConn);
		SQLFreeHandle(SQL_HANDLE_ENV, hand);
		this->connErr = CE_CONNECT_ERR;
		return false;
	}
	this->svrType = DB::DBUtil::ServerType::Unknown;
	envHand = hand;
	connHand = hConn;
	this->lastDataError = DE_NO_ERROR;

	UpdateConnInfo();
	return true;
}

Bool DB::ODBCConn::Connect(const UTF8Char *connStr)
{
	Text::String *s = Text::String::NewNotNull(connStr);
	Bool ret = this->Connect(s);
	s->Release();
	return ret;
}

DB::ODBCConn::ODBCConn(const UTF8Char *sourceName, IO::LogTool *log) : DB::DBConn(sourceName)
{
	connHand = 0;
	envHand = 0;
	lastStmtHand = 0;
	this->tableNames = 0;
	this->log = log;
	this->connErr = CE_NOT_CONNECT;
	this->lastErrorMsg = 0;
	this->connStr = 0;
	this->connHand = 0;
	this->envHand = 0;
	this->dsn = 0;
	this->uid = 0;
	this->pwd = 0;
	this->schema = 0;
	this->enableDebug = false;
	this->tzQhr = 0;
	this->forceTz = false;
}

DB::ODBCConn::ODBCConn(const UTF8Char *connStr, const UTF8Char *sourceName, IO::LogTool *log) : DB::DBConn(sourceName)
{
	connHand = 0;
	envHand = 0;
	lastStmtHand = 0;
	this->tableNames = 0;
	this->log = log;
	this->connErr = CE_NOT_CONNECT;
	this->lastErrorMsg = 0;
	this->connStr = 0;
	this->connHand = 0;
	this->envHand = 0;
	this->dsn = 0;
	this->uid = 0;
	this->pwd = 0;
	this->schema = 0;
	this->enableDebug = false;
	this->tzQhr = 0;
	this->forceTz = false;
	Text::String *s = Text::String::NewNotNull(connStr);
	this->Connect(s);
	s->Release();
}

DB::ODBCConn::ODBCConn(Text::String *dsn, Text::String *uid, Text::String *pwd, Text::String *schema, IO::LogTool *log) : DB::DBConn(dsn)
{
	this->log = log;
	this->tableNames = 0;
	this->connStr = 0;
	this->connHand = 0;
	this->connErr = CE_NOT_CONNECT;
	this->lastErrorMsg = 0;
	this->envHand = 0;
	this->enableDebug = false;
	this->dsn = SCOPY_STRING(dsn);
	this->uid = SCOPY_STRING(uid);
	this->pwd = SCOPY_STRING(pwd);
	this->schema = SCOPY_STRING(schema);
	lastStmtHand = 0;
	this->tzQhr = 0;
	this->forceTz = false;
	this->Connect(this->dsn, this->uid, this->pwd, this->schema);
}

DB::ODBCConn::ODBCConn(const UTF8Char *dsn, const UTF8Char *uid, const UTF8Char *pwd, const UTF8Char *schema, IO::LogTool *log) : DB::DBConn(dsn)
{
	this->log = log;
	this->tableNames = 0;
	this->connStr = 0;
	this->connHand = 0;
	this->connErr = CE_NOT_CONNECT;
	this->lastErrorMsg = 0;
	this->envHand = 0;
	this->enableDebug = false;
	this->dsn = Text::String::NewOrNull(dsn);
	this->uid = Text::String::NewOrNull(uid);
	this->pwd = Text::String::NewOrNull(pwd);
	this->schema = Text::String::NewOrNull(schema);
	lastStmtHand = 0;
	this->tzQhr = 0;
	this->forceTz = false;
	this->Connect(this->dsn, this->uid, this->pwd, this->schema);
}

DB::ODBCConn::~ODBCConn()
{
	Close();
	SDEL_STRING(this->dsn);
	SDEL_STRING(this->uid);
	SDEL_STRING(this->pwd);
	SDEL_STRING(this->schema);
	SDEL_STRING(this->lastErrorMsg);
	SDEL_STRING(this->connStr);
	if (this->tableNames)
	{
		UOSInt i = this->tableNames->GetCount();
		while (i-- > 0)
		{
			Text::StrDelNew(this->tableNames->GetItem(i));
		}
		DEL_CLASS(this->tableNames);
		this->tableNames = 0;
	}
}

DB::DBUtil::ServerType DB::ODBCConn::GetSvrType()
{
	return svrType;
}

DB::DBConn::ConnType DB::ODBCConn::GetConnType()
{
	return CT_ODBC;
}

Int8 DB::ODBCConn::GetTzQhr()
{
	return this->tzQhr;
}

void DB::ODBCConn::GetConnName(Text::StringBuilderUTF *sb)
{
	sb->AppendC(UTF8STRC("ODBC:"));
	if (this->connStr)
	{
		sb->Append(this->connStr);
	}
	else if (this->dsn)
	{
		sb->Append(this->dsn);
		if (this->schema)
		{
			sb->AppendChar('/', 1);
			sb->Append(this->schema);
		}
	}
}

void DB::ODBCConn::Close()
{
	if (lastStmtHand)
	{
		SQLFreeHandle(SQL_HANDLE_STMT, lastStmtHand);
		lastStmtHand = 0;
	}
	if (connHand)
	{
		SQLDisconnect(connHand);
		SQLFreeHandle(SQL_HANDLE_DBC, connHand);
		connHand = 0;
	}
	if (envHand)
	{
		SQLFreeHandle(SQL_HANDLE_ENV, envHand);
		envHand = 0;
	}
}

void DB::ODBCConn::Dispose()
{
	delete this;
}

OSInt DB::ODBCConn::ExecuteNonQuery(const UTF8Char *sql)
{
	if (this->connHand == 0)
	{
		this->lastDataError = DE_NO_CONN;
		return -2;
	}
	if (lastStmtHand)
	{
		SQLFreeHandle(SQL_HANDLE_STMT, lastStmtHand);
		lastStmtHand = 0;
	}
	SDEL_STRING(this->lastErrorMsg);

	SQLHANDLE hStmt;
	SQLRETURN ret;
	OSInt rowCnt = -1;
	ret = SQLAllocHandle(SQL_HANDLE_STMT, (SQLHANDLE)connHand, &hStmt);
	if (ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO)
	{
		this->lastDataError = DE_INIT_SQL_ERROR;
		return -2;
	}

	#if _WCHAR_SIZE == 2
	const WChar *wptr = Text::StrToWCharNew(sql);
	ret = SQLPrepareW(hStmt, (SQLWCHAR*)wptr, SQL_NTS);
	Text::StrDelNew(wptr);
	#else
	ret = SQLPrepareA(hStmt, (SQLCHAR*)sql, SQL_NTS);
	#endif
	if (ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO)
	{
		this->lastStmtHand = hStmt;
		this->lastStmtState = 0;
		this->lastDataError = DE_EXEC_SQL_ERROR;
		return -2;
	}

	ret = SQLExecute(hStmt);
	if (ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO && ret != SQL_NO_DATA)
	{
//		this->LogSQLError(hStmt);
		this->lastStmtHand = hStmt;
		this->lastStmtState = 0;
		this->lastDataError = DE_EXEC_SQL_ERROR;
		return -2;
	}
	this->lastDataError = DE_NO_ERROR;

	ret = SQLRowCount(hStmt, (SQLLEN*)&rowCnt);

	ret = SQLCloseCursor(hStmt);
	SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
	return rowCnt;
}

/*OSInt DB::ODBCConn::ExecuteNonQuery(const WChar *sql)
{
	if (this->connHand == 0)
	{
		this->lastError = 3;
		return -2;
	}
	if (lastStmtHand)
	{
		SQLFreeHandle(SQL_HANDLE_STMT, lastStmtHand);
		lastStmtHand = 0;
	}
	SQLHANDLE hStmt;
	SQLRETURN ret;
	OSInt rowCnt = -1;
	ret = SQLAllocHandle(SQL_HANDLE_STMT, (SQLHANDLE)connHand, &hStmt);
	if (ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO)
	{
		this->lastError = 4;
		return -2;
	}

	#if _WCHAR_SIZE == 2
	ret = SQLPrepareW(hStmt, (SQLWCHAR*)sql, SQL_NTS);
	#else
	const UTF8Char *csptr = Text::StrToUTF8New(sql);
	ret = SQLPrepareA(hStmt, (SQLCHAR*)csptr, SQL_NTS);
	Text::StrDelNew(csptr);
	#endif
	if (ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO)
	{
		this->lastStmtHand = hStmt;
		this->lastStmtState = 0;
		this->lastError = 5;
		return -2;
	}

	ret = SQLExecute(hStmt);
	if (ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO && ret != SQL_NO_DATA)
	{
//		this->LogSQLError(hStmt);
		this->lastStmtHand = hStmt;
		this->lastStmtState = 0;
		this->lastError = 5;
		return -2;
	}

	ret = SQLRowCount(hStmt, (SQLLEN*)&rowCnt);

	ret = SQLCloseCursor(hStmt);
	SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
	return rowCnt;
}*/

DB::DBReader *DB::ODBCConn::ExecuteReader(const UTF8Char *sql)
{
	if (this->connHand == 0)
	{
		this->lastDataError = DE_NO_CONN;
		return 0;
	}
	if (lastStmtHand)
	{
		SQLFreeHandle(SQL_HANDLE_STMT, lastStmtHand);
		lastStmtHand = 0;
	}
	SDEL_STRING(this->lastErrorMsg);
	SQLHANDLE hStmt;
	SQLRETURN ret;
//	Int32 rowCnt = -1;
	ret = SQLAllocHandle(SQL_HANDLE_STMT, (SQLHANDLE)connHand, &hStmt);
	if (ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO)
	{
		this->lastDataError = DE_INIT_SQL_ERROR;
		return 0;
	}

	#if _WCHAR_SIZE == 2
	const WChar *wptr = Text::StrToWCharNew(sql);
	ret = SQLPrepareW(hStmt, (SQLWCHAR*)wptr, SQL_NTS);
	Text::StrDelNew(wptr);
	#else
	ret = SQLPrepare(hStmt, (SQLCHAR*)sql, SQL_NTS);
	#endif
	if (ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO)
	{
		this->lastStmtHand = hStmt;
		this->lastStmtState = 0;
		this->lastDataError = DE_EXEC_SQL_ERROR;
		return 0;
	}

	ret = SQLExecute(hStmt);
	if (ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO && ret != SQL_NO_DATA)
	{
//		this->LogSQLError(hStmt);
		this->lastStmtHand = hStmt;
		this->lastStmtState = 0;
		this->lastDataError = DE_EXEC_SQL_ERROR;
		return 0;
	}
	this->lastDataError = DE_NO_ERROR;

	DB::ODBCReader *r;
	NEW_CLASS(r, DB::ODBCReader(this, hStmt, this->enableDebug, this->tzQhr));
	return r;
}

/*DB::DBReader *DB::ODBCConn::ExecuteReader(const WChar *sql)
{
	if (this->connHand == 0)
	{
		this->lastError = 3;
		return 0;
	}
	if (lastStmtHand)
	{
		SQLFreeHandle(SQL_HANDLE_STMT, lastStmtHand);
		lastStmtHand = 0;
	}
	SQLHANDLE hStmt;
	SQLRETURN ret;
//	Int32 rowCnt = -1;
	ret = SQLAllocHandle(SQL_HANDLE_STMT, (SQLHANDLE)connHand, &hStmt);
	if (ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO)
	{
		this->lastError = 4;
		return 0;
	}

	#if _WCHAR_SIZE == 2
	ret = SQLPrepareW(hStmt, (SQLWCHAR*)sql, SQL_NTS);
	#else
	const UTF8Char *csptr = Text::StrToUTF8New(sql);
	ret = SQLPrepare(hStmt, (SQLCHAR*)csptr, SQL_NTS);
	Text::StrDelNew(csptr);
	#endif
	if (ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO)
	{
		this->lastStmtHand = hStmt;
		this->lastStmtState = 0;
		this->lastError = 5;
		return 0;
	}

	ret = SQLExecute(hStmt);
	if (ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO && ret != SQL_NO_DATA)
	{
//		this->LogSQLError(hStmt);
		this->lastStmtHand = hStmt;
		this->lastStmtState = 0;
		this->lastError = 5;
		return 0;
	}

	DB::ODBCReader *r;
	NEW_CLASS(r, DB::ODBCReader(this, hStmt, this->enableDebug));
	return r;
}*/

void DB::ODBCConn::CloseReader(DB::DBReader *r)
{
	if (r)
	{
		DB::ODBCReader *rdr = (DB::ODBCReader*)r;
		DEL_CLASS(rdr);
	}
}

void DB::ODBCConn::GetErrorMsg(Text::StringBuilderUTF *str)
{
	Int32 recNumber = 1;
	SQLWCHAR state[6];
	Int32 errCode;
	SQLWCHAR msg[256];
	Int16 msgSize;
	if (this->lastErrorMsg)
	{
		str->Append(this->lastErrorMsg);
		return;
	}
	if (this->lastStmtHand == 0)
		return;
	while (true)
	{
		SQLRETURN ret = SQLGetDiagRecW(SQL_HANDLE_STMT, this->lastStmtHand, (Int16)recNumber, state, (SQLINTEGER*)&errCode, msg, 256, &msgSize);
		if (ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO)
		{
			str->AppendC(UTF8STRC("["));
			state[5] = 0;
			const UTF8Char *csptr = Text::StrToUTF8New((const UTF16Char*)state);
			str->Append(csptr);
			Text::StrDelNew(csptr);
			str->AppendC(UTF8STRC("]"));

#if _WCHAR_SIZE == 4
			const UTF8Char *sptr = Text::StrToUTF8New(state);
			if (Text::StrEquals(sptr, (const UTF8Char*)"23000"))
				this->lastStmtState = 3;
			else if (Text::StrEquals(sptr, (const UTF8Char*)"42000"))
				this->lastStmtState = 3;
			else if (Text::StrEquals(sptr, (const UTF8Char*)"HY000"))
				this->lastStmtState = 3;
			else
				this->lastStmtState = 2;
			Text::StrDelNew(sptr);
#else
			if (Text::StrEquals((const WChar*)state, L"23000"))
				this->lastStmtState = 3;
			else if (Text::StrEquals((const WChar*)state, L"42000"))
				this->lastStmtState = 3;
			else if (Text::StrEquals((const WChar*)state, L"HY000"))
				this->lastStmtState = 3;
			else
				this->lastStmtState = 2;
#endif

			if (msgSize > 255)
			{
				SQLWCHAR *tmpMsg = MemAlloc(SQLWCHAR, (UInt16)(msgSize + 1));
				ret = SQLGetDiagRecW(SQL_HANDLE_STMT, this->lastStmtHand, (SQLSMALLINT)recNumber, state, (SQLINTEGER*)&errCode, tmpMsg, (SQLSMALLINT)(msgSize + 1), &msgSize);
				csptr = Text::StrToUTF8New((const UTF16Char*)tmpMsg);
				str->Append(csptr);
				Text::StrDelNew(csptr);
				MemFree(tmpMsg);
			}
			else
			{
				csptr = Text::StrToUTF8New((const UTF16Char*)msg);
				str->Append(csptr);
				Text::StrDelNew(csptr);
			}
			recNumber++;
		}
		else
		{
			break;
		}
	}
}

Bool DB::ODBCConn::IsLastDataError()
{
	Int32 recNumber = 1;
	SQLWCHAR state[6];
	SQLINTEGER errCode;
	SQLWCHAR msg[1024];
	SQLSMALLINT msgSize;
	if (this->lastStmtHand == 0)
		return false;
	if (this->lastStmtState & 2)
	{
		return (this->lastStmtState & 1) != 0;
	}

	SQLRETURN ret = SQLGetDiagRecW(SQL_HANDLE_STMT, this->lastStmtHand, (SQLSMALLINT)recNumber, state, (SQLINTEGER*)&errCode, msg, sizeof(msg) / sizeof(msg[0]), &msgSize);
	if (ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO)
	{
#if _WCHAR_SIZE == 4
		state[5] = 0;
		const UTF8Char *sptr = Text::StrToUTF8New(state);
		Bool ret = false;
		if (Text::StrEquals(sptr, (const UTF8Char*)"23000"))
			ret = true;
		else if (Text::StrEquals(sptr, (const UTF8Char*)"42000"))
			ret = true;
		else if (Text::StrEquals(sptr, (const UTF8Char*)"HY000"))
			ret = true;
		Text::StrDelNew(sptr);
		return ret;
#else
		state[5] = 0;
		if (Text::StrEquals((const WChar*)state, L"23000"))
			return true;
		if (Text::StrEquals((const WChar*)state, L"42000"))
			return true;
		if (Text::StrEquals((const WChar*)state, L"HY000"))
			return true;
#endif
	}
	return false;
}

void DB::ODBCConn::Reconnect()
{
	Close();
	Int8 oldTzQhr = this->tzQhr;
	if (this->connStr)
	{
		Text::String *connStr = this->connStr->Clone();
		Connect(connStr);
		connStr->Release();
	}
	else
	{
		Connect(this->dsn, this->uid, this->pwd, this->schema);
	}
	if (this->forceTz)
	{
		this->tzQhr = oldTzQhr;
	}
}

void *DB::ODBCConn::BeginTransaction()
{
	if (isTran)
		return 0;

	SQLUINTEGER mode;
	mode = SQL_AUTOCOMMIT_OFF;
	SQLRETURN ret = SQLSetConnectAttr(this->connHand, SQL_ATTR_AUTOCOMMIT, &mode, sizeof(mode));
	if (ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO)
	{
		isTran = true;
		return (void*)-1;
	}
	else
	{
		return 0;
	}
}

void DB::ODBCConn::Commit(void *tran)
{
	if (!isTran)
		return;

	SQLUINTEGER mode;
	isTran = false;
	mode = SQL_AUTOCOMMIT_ON;
	SQLEndTran(SQL_HANDLE_DBC, this->connHand, SQL_COMMIT);
	SQLSetConnectAttr(this->connHand, SQL_ATTR_AUTOCOMMIT, &mode, sizeof(mode));
}

void DB::ODBCConn::Rollback(void *tran)
{
	if (!isTran)
		return;

	SQLUINTEGER mode;
	isTran = false;
	mode = SQL_AUTOCOMMIT_ON;
	SQLEndTran(SQL_HANDLE_DBC, this->connHand, SQL_ROLLBACK);
	SQLSetConnectAttr(this->connHand, SQL_ATTR_AUTOCOMMIT, &mode, sizeof(mode));
}

DB::ODBCConn::ConnError DB::ODBCConn::GetConnError()
{
	return this->connErr;
}

void DB::ODBCConn::SetEnableDebug(Bool enableDebug)
{
	this->enableDebug = enableDebug;
}

void DB::ODBCConn::SetTraceFile(const WChar *fileName)
{
	SQLUINTEGER en = SQL_OPT_TRACE_ON;
	UInt8 buff[256];
	Text::Encoding enc;
	UOSInt i;
	SQLSetConnectAttr(this->connHand, SQL_ATTR_TRACE, &en, sizeof(en));
	i = enc.WToBytes(buff, fileName);
	SQLSetConnectAttr(this->connHand, SQL_ATTR_TRACEFILE, buff, (SQLINTEGER)(i - 1));
}

UTF8Char *DB::ODBCConn::ShowTablesCmd(UTF8Char *sqlstr)
{
	if (this->svrType == DB::DBUtil::ServerType::MySQL)
		return Text::StrConcatC(sqlstr, UTF8STRC("show Tables"));
	else if (this->svrType == DB::DBUtil::ServerType::MSSQL)
		return Text::StrConcatC(sqlstr, UTF8STRC("select TABLE_NAME from user_tables"));
	else if (this->svrType == DB::DBUtil::ServerType::Oracle)
		return Text::StrConcatC(sqlstr, UTF8STRC("select table_name from user_tables"));
	else if (this->svrType == DB::DBUtil::ServerType::Access)
		return Text::StrConcatC(sqlstr, UTF8STRC("select name from MSysObjects where type = 1"));
	else if (this->svrType == DB::DBUtil::ServerType::MDBTools)
		return Text::StrConcatC(sqlstr, UTF8STRC("select name from MSysObjects where type = 1"));
	else
		return Text::StrConcatC(sqlstr, UTF8STRC("show Tables"));
}

DB::DBReader *DB::ODBCConn::GetTablesInfo()
{
	if (this->connHand == 0)
	{
		this->lastDataError = DE_CONN_ERROR;
		return 0;
	}
	if (lastStmtHand)
	{
		SQLFreeHandle(SQL_HANDLE_STMT, lastStmtHand);
		lastStmtHand = 0;
	}
	SQLHANDLE hStmt;
	SQLRETURN ret;
//	Int32 rowCnt = -1;
	ret = SQLAllocHandle(SQL_HANDLE_STMT, (SQLHANDLE)connHand, &hStmt);
	if (ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO)
	{
		this->lastDataError = DE_INIT_SQL_ERROR;
		return 0;
	}
	ret = SQLTablesW(hStmt, 0, 0, 0, 0, 0, 0, (SQLWCHAR*)L"TABLE", SQL_NTS );
	if (ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO && ret != SQL_NO_DATA)
	{
		this->lastStmtHand = hStmt;
		this->lastStmtState = 0;
		this->lastDataError = DE_EXEC_SQL_ERROR;
		return 0;
	}
	this->lastDataError = DE_NO_ERROR;

	DB::ODBCReader *r;
	NEW_CLASS(r, DB::ODBCReader(this, hStmt, this->enableDebug, this->tzQhr));
	return r;
}

UOSInt DB::ODBCConn::GetTableNames(Data::ArrayList<const UTF8Char*> *names)
{
	if (this->tableNames)
	{
	}
	else
	{
		WChar sbuff[256];
//		ShowTablesCmd(sbuff);
//		DB::ReadingDB::DBReader *rdr = this->ExecuteReader(sbuff);
		DB::DBReader *rdr = this->GetTablesInfo();
		NEW_CLASS(this->tableNames, Data::ArrayList<const UTF8Char*>());
		if (rdr)
		{
			sbuff[0] = 0;
			while (rdr->ReadNext())
			{
				rdr->GetStr(2, sbuff);
				if (Text::StrStartsWith(sbuff, L"~sq_"))
				{

				}
				else
				{
					this->tableNames->Add(Text::StrToUTF8New(sbuff));
				}
			}
			this->CloseReader(rdr);
		}
	}
	UOSInt i = 0;
	UOSInt j = this->tableNames->GetCount();
	while (i < j)
	{
		names->Add(this->tableNames->GetItem(i));
		i++;
	}
	//names->AddRange(this->tableNames);
	return this->tableNames->GetCount();
}

DB::DBReader *DB::ODBCConn::GetTableData(const UTF8Char *name, Data::ArrayList<Text::String*> *columnNames, UOSInt ofst, UOSInt maxCnt, const UTF8Char *ordering, Data::QueryConditions *condition)
{
	UTF8Char sbuff[512];
	UTF8Char *sptr;
	UOSInt i;
	UOSInt j;
	Text::StringBuilderUTF8 sb;
	sb.AppendC(UTF8STRC("select "));
	if (this->svrType == DB::DBUtil::ServerType::MSSQL || this->svrType == DB::DBUtil::ServerType::Access)
	{
		if (maxCnt > 0)
		{
			sb.AppendC(UTF8STRC("TOP "));
			sb.AppendUOSInt(maxCnt);
			sb.AppendChar(' ', 1);
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
			DB::DBUtil::SDBColUTF8(sbuff, columnNames->GetItem(i)->v, this->svrType);
			sb.Append(sbuff);
			i++;
		}
	}
	sb.AppendC(UTF8STRC(" from "));
	i = 0;
	while (true)
	{
		j = Text::StrIndexOf(&name[i], '.');
		if (j == INVALID_INDEX)
		{
			DB::DBUtil::SDBColUTF8(sbuff, &name[i], this->svrType);
			sb.Append(sbuff);
			break;
		}
		sptr = Text::StrConcatC(sbuff, &name[i], (UOSInt)j);
		DB::DBUtil::SDBColUTF8(sptr + 1, sbuff, this->svrType);
		sb.Append(sptr + 1);
		sb.AppendChar('.', 1);
		i += j + 1;
	}
	if (this->svrType == DB::DBUtil::ServerType::SQLite || this->svrType == DB::DBUtil::ServerType::MySQL)
	{
		if (maxCnt > 0)
		{
			sb.AppendC(UTF8STRC(" LIMIT "));
			sb.AppendUOSInt(maxCnt);
		}
	}
	return this->ExecuteReaderC(sb.ToString(), sb.GetLength());
}

void DB::ODBCConn::ShowSQLError(const UTF16Char *state, const UTF16Char *errMsg)
{
	Text::StringBuilderUTF8 sb;
	sb.AppendC(UTF8STRC("ODBC Error: ["));
	const UTF8Char *csptr = Text::StrToUTF8New(state);
	sb.Append(csptr);
	Text::StrDelNew(csptr);
	sb.AppendC(UTF8STRC("] "));
	csptr = Text::StrToUTF8New(errMsg);
	sb.Append(csptr);
	Text::StrDelNew(csptr);
	this->log->LogMessageC(sb.ToString(), sb.GetLength(), IO::ILogHandler::LOG_LEVEL_ERR_DETAIL);
}

void DB::ODBCConn::LogSQLError(void *hStmt)
{
	SQLWCHAR state[6];
	SQLWCHAR errorMsg[1024];
	if (this->log == 0)
		return;
	SQLINTEGER nativeError;
	SQLSMALLINT txtSize;
	SQLGetDiagRecW(SQL_HANDLE_STMT, (SQLHANDLE)hStmt, 1, state, &nativeError, errorMsg, sizeof(errorMsg) / sizeof(errorMsg[0]), &txtSize);
	this->ShowSQLError((const UTF16Char*)state, (const UTF16Char*)errorMsg);
}

Text::String *DB::ODBCConn::GetConnStr()
{
	return this->connStr;
}

Text::String *DB::ODBCConn::GetConnDSN()
{
	return this->dsn;
}

Text::String *DB::ODBCConn::GetConnUID()
{
	return this->uid;
}

Text::String *DB::ODBCConn::GetConnPWD()
{
	return this->pwd;
}

Text::String *DB::ODBCConn::GetConnSchema()
{
	return this->schema;
}

void DB::ODBCConn::ForceTz(Int8 tzQhr)
{
	this->forceTz = true;
	this->tzQhr = tzQhr;
}

UOSInt DB::ODBCConn::GetDriverList(Data::ArrayList<Text::String*> *driverList)
{
#if defined(WIN32) || defined(_WIN64) || defined(__CYGWIN__) || defined(__MINGW32__)
	return 0;
#else
	IO::ConfigFile *cfg = IO::IniFile::Parse((const UTF8Char*)"/etc/odbcinst.ini", 65001);
	if (cfg)
	{
		Data::ArrayList<Text::String*> cateList;
		UOSInt i = 0;
		UOSInt j = cfg->GetCateList(&cateList, false);
		while (i < j)
		{
			driverList->Add(cateList.GetItem(i)->Clone());
			i++;
		}
		DEL_CLASS(cfg);
		return j;
	}
	return 0;
#endif
}

IO::ConfigFile *DB::ODBCConn::GetDriverInfo(const UTF8Char *driverName, UOSInt nameLen)
{
#if defined(WIN32) || defined(_WIN64) || defined(__CYGWIN__) || defined(__MINGW32__)
	return 0;
#else
	IO::ConfigFile *cfg = IO::IniFile::Parse((const UTF8Char*)"/etc/odbcinst.ini", 65001);
	if (cfg)
	{
		IO::ConfigFile *cfgRet = cfg->CloneCate(driverName, nameLen);
		DEL_CLASS(cfg);
		return cfgRet;
	}
	return 0;
#endif
}

DB::DBTool *DB::ODBCConn::CreateDBTool(Text::String *dsn, Text::String *uid, Text::String *pwd, Text::String *schema, IO::LogTool *log, const UTF8Char *logPrefix)
{
	DB::ODBCConn *conn;
	DB::DBTool *db;
	NEW_CLASS(conn, DB::ODBCConn(dsn, uid, pwd, schema, log));
	if (conn->GetConnError() == CE_NONE)
	{
		NEW_CLASS(db, DB::DBTool(conn, true, log, logPrefix));
		return db;
	}
	else
	{
		DEL_CLASS(conn);
		return 0;
	}
}

DB::DBTool *DB::ODBCConn::CreateDBTool(const UTF8Char *dsn, const UTF8Char *uid, const UTF8Char *pwd, const UTF8Char *schema, IO::LogTool *log, const UTF8Char *logPrefix)
{
	DB::ODBCConn *conn;
	DB::DBTool *db;
	NEW_CLASS(conn, DB::ODBCConn(dsn, uid, pwd, schema, log));
	if (conn->GetConnError() == CE_NONE)
	{
		NEW_CLASS(db, DB::DBTool(conn, true, log, logPrefix));
		return db;
	}
	else
	{
		DEL_CLASS(conn);
		return 0;
	}
}

DB::ODBCReader::ODBCReader(DB::ODBCConn *conn, void *hStmt, Bool enableDebug, Int8 tzQhr)
{
	this->conn = conn;
	this->hStmt = hStmt;
	this->enableDebug = enableDebug;
	this->rowChanged = -1;
	this->tzQhr = tzQhr;

	UOSInt i;
	Int16 cnt;
	SQLRETURN ret = SQLNumResultCols((SQLHANDLE)this->hStmt, &cnt);
	if (ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO)
	{
		this->colCnt = (UInt16)cnt;
	}
	else
	{
		this->colCnt = 0;
	}
	SQLLEN len = -1;
	ret = SQLRowCount((SQLHSTMT)this->hStmt, (SQLLEN*)&len);
	this->rowChanged = len;
	this->colDatas = MemAlloc(DB::ODBCReader::ColumnData, this->colCnt);

	i = 0;
	while (i < this->colCnt)
	{
		Int16 nameLen;
		Int16 dataType;
		UOSInt cSize;
		Int16 decimalDigit;
		Int16 notNull;
		SQLWCHAR buff[10];
		SQLDescribeColW((SQLHANDLE)this->hStmt, (SQLUSMALLINT)(i + 1), buff, 10, &nameLen, &dataType, (SQLULEN*)&cSize, &decimalDigit, &notNull);
		this->colDatas[i].odbcType = dataType;
		this->colDatas[i].colType = ODBCType2DBType(dataType, cSize & 0xffff);
		this->colDatas[i].colSize = (cSize & 0xffff);
		this->colDatas[i].isNull = true;

		switch (this->colDatas[i].colType)
		{
		case DB::DBUtil::CT_Char:
		case DB::DBUtil::CT_VarChar:
		case DB::DBUtil::CT_NChar:
		case DB::DBUtil::CT_NVarChar:
		case DB::DBUtil::CT_UUID:
			NEW_CLASS(this->colDatas[i].colData, Text::StringBuilderUTF8());
			break;
		case DB::DBUtil::CT_Double:
		case DB::DBUtil::CT_Float:
			break;
		case DB::DBUtil::CT_Int16:
		case DB::DBUtil::CT_UInt16:
		case DB::DBUtil::CT_Int32:
		case DB::DBUtil::CT_UInt32:
		case DB::DBUtil::CT_Byte:
		case DB::DBUtil::CT_Int64:
		case DB::DBUtil::CT_UInt64:
			break;
		case DB::DBUtil::CT_Bool:
			break;
		case DB::DBUtil::CT_DateTime:
		case DB::DBUtil::CT_DateTime2:
			NEW_CLASS(this->colDatas[i].colData, Data::DateTime());
			break;
		case DB::DBUtil::CT_Vector:
		case DB::DBUtil::CT_Binary:
			this->colDatas[i].colData = 0;
			break;
		case DB::DBUtil::CT_Unknown:
		default:
			break;
		}
		i++;
	}
}

DB::ODBCReader::~ODBCReader()
{
	SQLHANDLE hStmt = (SQLHANDLE)this->hStmt;
	SQLCloseCursor(hStmt);
	SQLFreeHandle(SQL_HANDLE_STMT, hStmt);

	UOSInt i;
	Text::StringBuilderUTF8 *sb;
	Data::DateTime *dt;
	i = this->colCnt;
	while (i-- > 0)
	{
		switch (this->colDatas[i].colType)
		{
		case DB::DBUtil::CT_Char:
		case DB::DBUtil::CT_VarChar:
		case DB::DBUtil::CT_NChar:
		case DB::DBUtil::CT_NVarChar:
		case DB::DBUtil::CT_UUID:
			sb = (Text::StringBuilderUTF8*)this->colDatas[i].colData;
			DEL_CLASS(sb);
			break;
		case DB::DBUtil::CT_Double:
		case DB::DBUtil::CT_Float:
			break;
		case DB::DBUtil::CT_Int16:
		case DB::DBUtil::CT_UInt16:
		case DB::DBUtil::CT_Int32:
		case DB::DBUtil::CT_UInt32:
		case DB::DBUtil::CT_Byte:
		case DB::DBUtil::CT_Int64:
		case DB::DBUtil::CT_UInt64:
			break;
		case DB::DBUtil::CT_Bool:
			break;
		case DB::DBUtil::CT_DateTime:
		case DB::DBUtil::CT_DateTime2:
			dt = (Data::DateTime*)this->colDatas[i].colData;
			DEL_CLASS(dt);
			break;
		case DB::DBUtil::CT_Vector:
		case DB::DBUtil::CT_Binary:
			if (this->colDatas[i].colData)
				MemFree(this->colDatas[i].colData);
			break;
		case DB::DBUtil::CT_Unknown:
		default:
			break;
		}
	}
	MemFree(this->colDatas);
}

Bool DB::ODBCReader::ReadNext()
{
	if (this->enableDebug)
	{
//		printf("ODBCReader.ReadNext 1\r\n");
	}
	SQLRETURN ret = SQLFetch((SQLHANDLE)this->hStmt);
	if (ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO)
	{
		UOSInt i;
		if (this->enableDebug)
		{
//			printf("ODBCReader.ReadNext 2\r\n");
		}
		i = 0;
		while (i < this->colCnt)
		{
			Text::StringBuilderUTF8 *sb;
			Data::DateTime *dt;
			UTF16Char *sptr;
			SQLLEN len;
			SQLRETURN ret;

			if (this->enableDebug)
			{
//				printf("ODBCReader.ReadNext Col %d\r\n", i);
			}
			switch (this->colDatas[i].colType)
			{
			case DB::DBUtil::CT_NChar:
			case DB::DBUtil::CT_NVarChar:
			case DB::DBUtil::CT_Char:
			case DB::DBUtil::CT_VarChar:
			case DB::DBUtil::CT_UUID:
				{
					len = 256;
					sb = (Text::StringBuilderUTF8*)this->colDatas[i].colData;
					sb->ClearStr();

#if !defined(_WIN32) && !defined(_WIN64)
					if (this->colDatas[i].odbcType == SQL_CHAR || this->colDatas[i].odbcType == SQL_VARCHAR || this->colDatas[i].odbcType == SQL_LONGVARCHAR)
					{
						UTF8Char sbuff[256];
						ret = SQLGetData((SQLHANDLE)this->hStmt, (SQLUSMALLINT)(i + 1), SQL_C_CHAR, sbuff, 256, &len);
						if (ret == SQL_SUCCESS_WITH_INFO || ret == SQL_SUCCESS)
						{
							if (len == SQL_NULL_DATA)
							{
								this->colDatas[i].isNull = true;
							}
							else if (len == 0)
							{
								this->colDatas[i].isNull = false;
							}
							else if (len > 0 && len <= 510)
							{
								if (ret == SQL_SUCCESS_WITH_INFO)
								{
	//								wprintf(L"ODBCReader: Char Error, len = %d, v = %ls\r\n", len, sb->GetEndPtr());
									this->conn->LogSQLError(this->hStmt);
								}
								sb->AppendC(sbuff, (UOSInt)len);
								this->colDatas[i].isNull = false;
							}
							else
							{
								if (len == -4) //SQL_NO_TOTAL
								{
									len = 2048;
								}
								sb->AllocLeng((UOSInt)len);
								UTF8Char *endPtr = sb->GetEndPtr();
								ret = SQLGetData((SQLHANDLE)this->hStmt, (SQLUSMALLINT)(i + 1), SQL_C_CHAR, endPtr, len + 1, &len);
								if (ret == SQL_SUCCESS_WITH_INFO || ret == SQL_ERROR)
								{
	//								wprintf(L"ODBCReader: Char Error, len = %d, v = %ls\r\n", len, sb->GetEndPtr());
									this->conn->LogSQLError(this->hStmt);
								}
								if (ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO)
								{
									endPtr = &endPtr[len];
									*endPtr = 0;
									sb->SetEndPtr(endPtr);
									this->colDatas[i].isNull = false;
								}
								else
								{
									sb->ClearStr();
									this->colDatas[i].isNull = true;
								}
							}
						}
						else
						{
							this->colDatas[i].isNull = true;
						}
					}
					else
#endif					
					{
						UTF16Char wbuff[256];
						ret = SQLGetData((SQLHANDLE)this->hStmt, (SQLUSMALLINT)(i + 1), SQL_C_WCHAR, wbuff, 512, &len);
						if (ret == SQL_SUCCESS_WITH_INFO || ret == SQL_SUCCESS)
						{
							if (len == SQL_NULL_DATA)
							{
								this->colDatas[i].isNull = true;
							}
							else if (len == 0)
							{
								this->colDatas[i].isNull = false;
							}
							else if (len > 0 && len <= 510)
							{
								if (ret == SQL_SUCCESS_WITH_INFO)
								{
	//								wprintf(L"ODBCReader: Char Error, len = %d, v = %ls\r\n", len, sb->GetEndPtr());
									this->conn->LogSQLError(this->hStmt);
								}
								UOSInt cnt = Text::StrUTF16_UTF8CntC(wbuff, (UOSInt)len >> 1);
								sb->AllocLeng(cnt);
								UTF8Char *endPtr = Text::StrUTF16_UTF8C(sb->GetEndPtr(), wbuff, (UOSInt)len >> 1);
								*endPtr = 0;
								sb->SetEndPtr(endPtr);
								this->colDatas[i].isNull = false;
							}
							else
							{
								UOSInt cnt = Text::StrUTF16_UTF8CntC(wbuff, 255);
								sb->AllocLeng(cnt);
								UTF8Char *endPtr = Text::StrUTF16_UTF8C(sb->GetEndPtr(), wbuff, 255);
								*endPtr = 0;
								sb->SetEndPtr(endPtr);

								if (len == -4) //SQL_NO_TOTAL
								{
									len = 2048;
								}
								sptr = MemAlloc(UTF16Char, (UOSInt)((len >> 1) + 1));
								ret = SQLGetData((SQLHANDLE)this->hStmt, (SQLUSMALLINT)(i + 1), SQL_C_WCHAR, sptr, len + 2, &len);
								if (ret == SQL_SUCCESS_WITH_INFO || ret == SQL_ERROR)
								{
	//								wprintf(L"ODBCReader: Char Error, len = %d, v = %ls\r\n", len, sb->GetEndPtr());
									this->conn->LogSQLError(this->hStmt);
								}
								if (ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO)
								{
									cnt = Text::StrUTF16_UTF8CntC(sptr, (UOSInt)len >> 1);
									sb->AllocLeng(cnt);
									endPtr = Text::StrUTF16_UTF8C(sb->GetEndPtr(), sptr, (UOSInt)len >> 1);
									*endPtr = 0;
									sb->SetEndPtr(endPtr);
									this->colDatas[i].isNull = false;
								}
								else
								{
									this->colDatas[i].isNull = true;
								}
								MemFree(sptr);
							}
						}
						else
						{
							if (ret == SQL_ERROR)
							{
								this->conn->LogSQLError(this->hStmt);
							}
							this->colDatas[i].isNull = true;
						}
					}
				}
				break;
			case DB::DBUtil::CT_Double:
			case DB::DBUtil::CT_Float:
				{
					SQLDOUBLE dbl;
					ret = SQLGetData((SQLHANDLE)this->hStmt, (SQLUSMALLINT)(i + 1), SQL_C_DOUBLE, &dbl, sizeof(dbl), &len);
					if (ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO || ret == SQL_NO_DATA)
					{
						if (len == SQL_NULL_DATA)
						{
							this->colDatas[i].isNull = true;
						}
						else
						{
							this->colDatas[i].isNull = false;
							*(Double*)&this->colDatas[i].dataVal = dbl;
						}
					}
					else
					{
						this->colDatas[i].isNull = true;
					}
				}
				break;
			case DB::DBUtil::CT_Int16:
			case DB::DBUtil::CT_UInt16:
			case DB::DBUtil::CT_UInt32:
			case DB::DBUtil::CT_Int32:
			case DB::DBUtil::CT_Byte:
			case DB::DBUtil::CT_Bool:
				{
					SQLINTEGER val;
					ret = SQLGetData((SQLHANDLE)this->hStmt, (SQLUSMALLINT)(i + 1), SQL_C_SLONG, &val, sizeof(val), &len);
					if (ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO || ret == SQL_NO_DATA)
					{
						if (len == SQL_NULL_DATA)
						{
							this->colDatas[i].isNull = true;
						}
						else
						{
							this->colDatas[i].isNull = false;
							this->colDatas[i].dataVal = val;
						}
					}
					else
					{
						this->colDatas[i].isNull = true;
					}
				}
				break;
			case DB::DBUtil::CT_UInt64:
			case DB::DBUtil::CT_Int64:
				{
					SQLBIGINT val;
					ret = SQLGetData((SQLHANDLE)this->hStmt, (SQLUSMALLINT)(i + 1), SQL_C_SBIGINT, &val, sizeof(val), &len);
					if (ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO || ret == SQL_NO_DATA)
					{
						if (len == SQL_NULL_DATA)
						{
							this->colDatas[i].isNull = true;
						}
						else
						{
							this->colDatas[i].isNull = false;
							this->colDatas[i].dataVal = val;
						}
					}
					else
					{
						this->colDatas[i].isNull = true;
					}
				}
				break;
			case DB::DBUtil::CT_DateTime:
			case DB::DBUtil::CT_DateTime2:
				{
					TIMESTAMP_STRUCT ts;
					dt = (Data::DateTime*)this->colDatas[i].colData;
					ret = SQLGetData((SQLHANDLE)this->hStmt, (SQLUSMALLINT)(i + 1), SQL_C_TIMESTAMP, &ts, sizeof(ts), &len);
					if (ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO || ret == SQL_NO_DATA)
					{
						if (len == SQL_NULL_DATA)
						{
							this->colDatas[i].isNull = true;
						}
						else
						{
							this->colDatas[i].isNull = false;
							UInt16 ms;
							ms = (UInt16)(ts.fraction / 1000000);
							dt->SetValueNoFix((UInt16)ts.year, (UInt8)ts.month, (UInt8)ts.day, (UInt8)ts.hour, (UInt8)ts.minute, (UInt8)ts.second, ms, this->tzQhr);
						}
					}
					else
					{
						this->colDatas[i].isNull = true;
					}
				}
				break;
			case DB::DBUtil::CT_Vector:
			case DB::DBUtil::CT_Binary:
				{
					if (this->colDatas[i].colData)
					{
						MemFree(this->colDatas[i].colData);
						this->colDatas[i].colData = 0;
					}
					ret = SQLGetData((SQLHANDLE)this->hStmt, (SQLUSMALLINT)(i + 1), SQL_C_BINARY, &this->colDatas[i].dataVal, 0, &len);
					if (ret == SQL_SUCCESS_WITH_INFO || ret == SQL_SUCCESS)
					{
						if (len == SQL_NULL_DATA)
						{
							this->colDatas[i].isNull = true;
						}
						else
						{
							this->colDatas[i].dataVal = len;
							this->colDatas[i].colData = MemAlloc(UInt8, (UOSInt)len);
							ret = SQLGetData((SQLHANDLE)this->hStmt, (SQLUSMALLINT)(i + 1), SQL_C_BINARY, this->colDatas[i].colData, len, &len);
							if (ret == SQL_SUCCESS_WITH_INFO || ret == SQL_SUCCESS)
							{
								this->colDatas[i].isNull = false;
							}
							else
							{
								this->colDatas[i].isNull = true;
							}
						}
					}
					else
					{
						this->colDatas[i].isNull = true;
					}
				}
				break;
			case DB::DBUtil::CT_Unknown:
			default:
				this->colDatas[i].isNull = true;
				break;
			}

			i++;
		}
		if (this->enableDebug)
		{
	//		printf("ODBCReader.ReadNext 3\r\n");
		}
		return true;
	}
	else if (ret == SQL_NO_DATA)
	{
		return false;
	}
	if (this->enableDebug)
	{
//		printf("ODBCReader.ReadNext 4 ret = %d\r\n", ret);
	}
	return false;

}

UOSInt DB::ODBCReader::ColCount()
{
	return this->colCnt;
}

OSInt DB::ODBCReader::GetRowChanged()
{
	return this->rowChanged;
}

Int32 DB::ODBCReader::GetInt32(UOSInt colIndex)
{
	if (colIndex >= this->colCnt)
		return 0;
	if (this->colDatas[colIndex].isNull)
		return 0;
	switch (this->colDatas[colIndex].colType)
	{
	case DB::DBUtil::CT_Char:
	case DB::DBUtil::CT_VarChar:
	case DB::DBUtil::CT_NChar:
	case DB::DBUtil::CT_NVarChar:
	case DB::DBUtil::CT_UUID:
		return Text::StrToInt32(((Text::StringBuilderUTF8*)this->colDatas[colIndex].colData)->ToString());
	case DB::DBUtil::CT_Double:
	case DB::DBUtil::CT_Float:
		return Double2Int32(*(Double*)&this->colDatas[colIndex].dataVal);
	case DB::DBUtil::CT_Int16:
	case DB::DBUtil::CT_UInt16:
	case DB::DBUtil::CT_Int32:
	case DB::DBUtil::CT_UInt32:
	case DB::DBUtil::CT_Byte:
	case DB::DBUtil::CT_Int64:
	case DB::DBUtil::CT_UInt64:
	case DB::DBUtil::CT_Bool:
		return (Int32)this->colDatas[colIndex].dataVal;
	case DB::DBUtil::CT_DateTime:
	case DB::DBUtil::CT_DateTime2:
		return 0;
	case DB::DBUtil::CT_Vector:
	case DB::DBUtil::CT_Binary:
		return 0;
	case DB::DBUtil::CT_Unknown:
	default:
		return 0;
	}
	return 0;
}

Int64 DB::ODBCReader::GetInt64(UOSInt colIndex)
{
	if (colIndex >= this->colCnt)
		return 0;
	if (this->colDatas[colIndex].isNull)
		return 0;
	switch (this->colDatas[colIndex].colType)
	{
	case DB::DBUtil::CT_Char:
	case DB::DBUtil::CT_VarChar:
	case DB::DBUtil::CT_NChar:
	case DB::DBUtil::CT_NVarChar:
	case DB::DBUtil::CT_UUID:
		return Text::StrToInt64(((Text::StringBuilderUTF8*)this->colDatas[colIndex].colData)->ToString());
	case DB::DBUtil::CT_Double:
	case DB::DBUtil::CT_Float:
		return (Int64)(*(Double*)&this->colDatas[colIndex].dataVal);
	case DB::DBUtil::CT_Int16:
	case DB::DBUtil::CT_UInt16:
	case DB::DBUtil::CT_Int32:
	case DB::DBUtil::CT_UInt32:
	case DB::DBUtil::CT_Byte:
	case DB::DBUtil::CT_Int64:
	case DB::DBUtil::CT_UInt64:
	case DB::DBUtil::CT_Bool:
		return this->colDatas[colIndex].dataVal;
	case DB::DBUtil::CT_DateTime:
	case DB::DBUtil::CT_DateTime2:
		return 0;
	case DB::DBUtil::CT_Vector:
	case DB::DBUtil::CT_Binary:
		return 0;
	case DB::DBUtil::CT_Unknown:
	default:
		break;
	}
	return 0;
}

WChar *DB::ODBCReader::GetStr(UOSInt colIndex, WChar *buff)
{
	UTF8Char u8buff[64];
	if (colIndex >= this->colCnt)
		return 0;
	if (this->colDatas[colIndex].isNull)
		return 0;
	switch (this->colDatas[colIndex].colType)
	{
	case DB::DBUtil::CT_Char:
	case DB::DBUtil::CT_VarChar:
	case DB::DBUtil::CT_NChar:
	case DB::DBUtil::CT_NVarChar:
	case DB::DBUtil::CT_UUID:
		return Text::StrUTF8_WChar(buff, ((Text::StringBuilderUTF8*)this->colDatas[colIndex].colData)->ToString(), 0);
	case DB::DBUtil::CT_Double:
	case DB::DBUtil::CT_Float:
		return Text::StrDouble(buff, *(Double*)&this->colDatas[colIndex].dataVal);
	case DB::DBUtil::CT_Int16:
	case DB::DBUtil::CT_UInt16:
	case DB::DBUtil::CT_Int32:
	case DB::DBUtil::CT_UInt32:
	case DB::DBUtil::CT_Byte:
	case DB::DBUtil::CT_Int64:
	case DB::DBUtil::CT_UInt64:
	case DB::DBUtil::CT_Bool:
		return Text::StrInt64(buff, this->colDatas[colIndex].dataVal);
	case DB::DBUtil::CT_DateTime:
	case DB::DBUtil::CT_DateTime2:
		((Data::DateTime*)this->colDatas[colIndex].colData)->ToString(u8buff);
		return Text::StrUTF8_WChar(buff, u8buff, 0);
	case DB::DBUtil::CT_Binary:
		return 0;
	case DB::DBUtil::CT_Vector:
		{
			Math::Vector2D *vec = this->GetVector(colIndex);
			if (vec)
			{
				//Math::WKTWriter::GenerateWKT();
				DEL_CLASS(vec);
			}
		}
		return 0;
	case DB::DBUtil::CT_Unknown:
	default:
		return 0;
	}
	return 0;
}

Bool DB::ODBCReader::GetStr(UOSInt colIndex, Text::StringBuilderUTF *sb)
{
	if (colIndex >= this->colCnt)
		return false;
	if (this->colDatas[colIndex].isNull)
		return false;
	switch (this->colDatas[colIndex].colType)
	{
	case DB::DBUtil::CT_NChar:
	case DB::DBUtil::CT_NVarChar:
	case DB::DBUtil::CT_Char:
	case DB::DBUtil::CT_VarChar:
	case DB::DBUtil::CT_UUID:
		sb->Append(((Text::StringBuilderUTF8*)this->colDatas[colIndex].colData)->ToString());
		return true;
	case DB::DBUtil::CT_Double:
	case DB::DBUtil::CT_Float:
		Text::SBAppendF64(sb, *(Double*)&this->colDatas[colIndex].dataVal);
		return true;
	case DB::DBUtil::CT_Int16:
	case DB::DBUtil::CT_Int32:
	case DB::DBUtil::CT_Byte:
	case DB::DBUtil::CT_Int64:
	case DB::DBUtil::CT_Bool:
		sb->AppendI64(this->colDatas[colIndex].dataVal);
		return true;
	case DB::DBUtil::CT_UInt16:
		sb->AppendU16((UInt16)this->colDatas[colIndex].dataVal);
		return true;
	case DB::DBUtil::CT_UInt32:
		sb->AppendU32((UInt32)this->colDatas[colIndex].dataVal);
		return true;
	case DB::DBUtil::CT_UInt64:
		sb->AppendU64((UInt64)this->colDatas[colIndex].dataVal);
		return true;
	case DB::DBUtil::CT_DateTime2:
	case DB::DBUtil::CT_DateTime:
		sb->AppendDate((Data::DateTime*)this->colDatas[colIndex].colData);
		return true;
	case DB::DBUtil::CT_Binary:
		return false;
	case DB::DBUtil::CT_Vector:
		{
			Math::Vector2D *vec = this->GetVector(colIndex);
			if (vec)
			{
				Math::WKTWriter wkt;
				wkt.GenerateWKT(sb, vec);
				DEL_CLASS(vec);
			}
		}
		return 0;
	case DB::DBUtil::CT_Unknown:
	default:
		return false;
	}
	return 0;
}

Text::String *DB::ODBCReader::GetNewStr(UOSInt colIndex)
{
	UTF8Char sbuff[32];
	UTF8Char *sptr;
	if (colIndex >= this->colCnt)
		return 0;
	if (this->colDatas[colIndex].isNull)
		return 0;
	switch (this->colDatas[colIndex].colType)
	{
	case DB::DBUtil::CT_Char:
	case DB::DBUtil::CT_VarChar:
	case DB::DBUtil::CT_NChar:
	case DB::DBUtil::CT_NVarChar:
	case DB::DBUtil::CT_UUID:
		{
			Text::StringBuilderUTF8 *sb = (Text::StringBuilderUTF8*)this->colDatas[colIndex].colData;
			return Text::String::New(sb->ToString(), sb->GetLength());
		}
	case DB::DBUtil::CT_Double:
	case DB::DBUtil::CT_Float:
		sptr = Text::StrDouble(sbuff, *(Double*)&this->colDatas[colIndex].dataVal);
		return Text::String::New(sbuff, (UOSInt)(sptr - sbuff));
	case DB::DBUtil::CT_Int16:
	case DB::DBUtil::CT_Int32:
	case DB::DBUtil::CT_Byte:
	case DB::DBUtil::CT_Int64:
	case DB::DBUtil::CT_Bool:
		sptr = Text::StrInt64(sbuff, this->colDatas[colIndex].dataVal);
		return Text::String::New(sbuff, (UOSInt)(sptr - sbuff));
	case DB::DBUtil::CT_UInt64:
		sptr = Text::StrUInt64(sbuff, (UInt64)this->colDatas[colIndex].dataVal);
		return Text::String::New(sbuff, (UOSInt)(sptr - sbuff));
	case DB::DBUtil::CT_UInt32:
		sptr = Text::StrUInt32(sbuff, (UInt32)this->colDatas[colIndex].dataVal);
		return Text::String::New(sbuff, (UOSInt)(sptr - sbuff));
	case DB::DBUtil::CT_UInt16:
		sptr = Text::StrUInt16(sbuff, (UInt16)this->colDatas[colIndex].dataVal);
		return Text::String::New(sbuff, (UOSInt)(sptr - sbuff));
	case DB::DBUtil::CT_DateTime:
	case DB::DBUtil::CT_DateTime2:
		sptr = ((Data::DateTime*)this->colDatas[colIndex].colData)->ToString(sbuff);
		return Text::String::New(sbuff, (UOSInt)(sptr - sbuff));
	case DB::DBUtil::CT_Binary:
		return 0;
	case DB::DBUtil::CT_Vector:
		{
			Math::Vector2D *vec = this->GetVector(colIndex);
			if (vec)
			{
				Text::StringBuilderUTF8 sb;
				Math::WKTWriter wkt;
				wkt.GenerateWKT(&sb, vec);
				DEL_CLASS(vec);
				return Text::String::New(sb.ToString(), sb.GetLength());
			}
		}
		return 0;
	case DB::DBUtil::CT_Unknown:
	default:
		return 0;
	}
	return 0;
}

UTF8Char *DB::ODBCReader::GetStr(UOSInt colIndex, UTF8Char *buff, UOSInt buffSize)
{
	if (colIndex >= this->colCnt)
		return 0;
	if (this->colDatas[colIndex].isNull)
		return 0;
	switch (this->colDatas[colIndex].colType)
	{
	case DB::DBUtil::CT_NChar:
	case DB::DBUtil::CT_NVarChar:
	case DB::DBUtil::CT_Char:
	case DB::DBUtil::CT_VarChar:
	case DB::DBUtil::CT_UUID:
		return Text::StrConcatS(buff, ((Text::StringBuilderUTF8*)this->colDatas[colIndex].colData)->ToString(), buffSize);
	case DB::DBUtil::CT_Double:
	case DB::DBUtil::CT_Float:
		return Text::StrDouble(buff, *(Double*)&this->colDatas[colIndex].dataVal);
	case DB::DBUtil::CT_Int16:
	case DB::DBUtil::CT_Int32:
	case DB::DBUtil::CT_Byte:
	case DB::DBUtil::CT_Int64:
	case DB::DBUtil::CT_Bool:
		return Text::StrInt64(buff, this->colDatas[colIndex].dataVal);
	case DB::DBUtil::CT_UInt32:
		return Text::StrUInt32(buff, (UInt32)this->colDatas[colIndex].dataVal);
	case DB::DBUtil::CT_UInt64:
		return Text::StrUInt64(buff, (UInt64)this->colDatas[colIndex].dataVal);
	case DB::DBUtil::CT_UInt16:
		return Text::StrUInt16(buff, (UInt16)this->colDatas[colIndex].dataVal);
	case DB::DBUtil::CT_DateTime:
	case DB::DBUtil::CT_DateTime2:
		return ((Data::DateTime*)this->colDatas[colIndex].colData)->ToString(buff);
	case DB::DBUtil::CT_Binary:
		return 0;
	case DB::DBUtil::CT_Vector:
		{
			Math::Vector2D *vec = this->GetVector(colIndex);
			if (vec)
			{
				Text::StringBuilderUTF8 sb;
				Math::WKTWriter wkt;
				wkt.GenerateWKT(&sb, vec);
				DEL_CLASS(vec);
				return Text::StrConcatC(buff, sb.ToString(), sb.GetLength());
			}
		}
		return 0;
	case DB::DBUtil::CT_Unknown:
	default:
		return 0;
	}
	return 0;
}

DB::DBReader::DateErrType DB::ODBCReader::GetDate(UOSInt colIndex, Data::DateTime *outVal)
{
	if (colIndex >= this->colCnt)
		return DB::DBReader::DET_ERROR;
	if (this->colDatas[colIndex].isNull)
		return DB::DBReader::DET_NULL;
	switch (this->colDatas[colIndex].colType)
	{
	case DB::DBUtil::CT_NChar:
	case DB::DBUtil::CT_NVarChar:
	case DB::DBUtil::CT_Char:
	case DB::DBUtil::CT_VarChar:
	case DB::DBUtil::CT_UUID:
		{
			Text::StringBuilderUTF8 *sb = (Text::StringBuilderUTF8*)this->colDatas[colIndex].colData;
			outVal->SetValue(sb->ToString(), sb->GetLength());
		}
		return DB::DBReader::DET_OK;
	case DB::DBUtil::CT_Double:
	case DB::DBUtil::CT_Float:
		return DB::DBReader::DET_ERROR;
	case DB::DBUtil::CT_UInt16:
	case DB::DBUtil::CT_Int16:
	case DB::DBUtil::CT_UInt32:
	case DB::DBUtil::CT_Int32:
	case DB::DBUtil::CT_UInt64:
	case DB::DBUtil::CT_Int64:
	case DB::DBUtil::CT_Byte:
	case DB::DBUtil::CT_Bool:
		return DB::DBReader::DET_ERROR;
	case DB::DBUtil::CT_DateTime2:
	case DB::DBUtil::CT_DateTime:
		outVal->SetValue((Data::DateTime*)this->colDatas[colIndex].colData);
		return DB::DBReader::DET_OK;
	case DB::DBUtil::CT_Vector:
	case DB::DBUtil::CT_Binary:
		return DB::DBReader::DET_ERROR;
	case DB::DBUtil::CT_Unknown:
	default:
		return DB::DBReader::DET_ERROR;
	}
	return DB::DBReader::DET_ERROR;
}

Double DB::ODBCReader::GetDbl(UOSInt colIndex)
{
	if (colIndex >= this->colCnt)
		return 0;
	if (this->colDatas[colIndex].isNull)
		return 0;
	switch (this->colDatas[colIndex].colType)
	{
	case DB::DBUtil::CT_NChar:
	case DB::DBUtil::CT_NVarChar:
	case DB::DBUtil::CT_Char:
	case DB::DBUtil::CT_VarChar:
	case DB::DBUtil::CT_UUID:
		return Text::StrToDouble(((Text::StringBuilderUTF8*)this->colDatas[colIndex].colData)->ToString());
	case DB::DBUtil::CT_Double:
	case DB::DBUtil::CT_Float:
		return *(Double*)&this->colDatas[colIndex].dataVal;
	case DB::DBUtil::CT_Int16:
	case DB::DBUtil::CT_Int32:
	case DB::DBUtil::CT_Byte:
	case DB::DBUtil::CT_Int64:
	case DB::DBUtil::CT_Bool:
		return (Double)this->colDatas[colIndex].dataVal;
	case DB::DBUtil::CT_UInt16:
		return (Double)(UInt16)this->colDatas[colIndex].dataVal;
	case DB::DBUtil::CT_UInt32:
		return (Double)(UInt32)this->colDatas[colIndex].dataVal;
	case DB::DBUtil::CT_UInt64:
		return (Double)(UInt64)this->colDatas[colIndex].dataVal;
	case DB::DBUtil::CT_DateTime:
	case DB::DBUtil::CT_DateTime2:
		return 0;
	case DB::DBUtil::CT_Vector:
	case DB::DBUtil::CT_Binary:
		return 0;
	case DB::DBUtil::CT_Unknown:
	default:
		return 0;
	}
	return 0;
}

Bool DB::ODBCReader::GetBool(UOSInt colIndex)
{
	if (colIndex >= this->colCnt)
		return false;
	if (this->colDatas[colIndex].isNull)
		return false;
	switch (this->colDatas[colIndex].colType)
	{
	case DB::DBUtil::CT_NChar:
	case DB::DBUtil::CT_NVarChar:
	case DB::DBUtil::CT_Char:
	case DB::DBUtil::CT_VarChar:
	case DB::DBUtil::CT_UUID:
		return Text::StrToInt32(((Text::StringBuilderUTF8*)this->colDatas[colIndex].colData)->ToString()) != 0;
	case DB::DBUtil::CT_Double:
	case DB::DBUtil::CT_Float:
		return (*(Double*)&this->colDatas[colIndex].dataVal) != 0;
	case DB::DBUtil::CT_UInt16:
	case DB::DBUtil::CT_Int16:
	case DB::DBUtil::CT_UInt32:
	case DB::DBUtil::CT_Int32:
	case DB::DBUtil::CT_Int64:
	case DB::DBUtil::CT_UInt64:
	case DB::DBUtil::CT_Byte:
	case DB::DBUtil::CT_Bool:
		return this->colDatas[colIndex].dataVal != 0;
	case DB::DBUtil::CT_DateTime:
	case DB::DBUtil::CT_DateTime2:
		return false;
	case DB::DBUtil::CT_Vector:
	case DB::DBUtil::CT_Binary:
		return false;
	case DB::DBUtil::CT_Unknown:
	default:
		return false;
	}
	return false;
}

UOSInt DB::ODBCReader::GetBinarySize(UOSInt colIndex)
{
	if (colIndex >= this->colCnt)
		return 0;
	if (this->colDatas[colIndex].isNull)
		return 0;
	switch (this->colDatas[colIndex].colType)
	{
	case DB::DBUtil::CT_NChar:
	case DB::DBUtil::CT_NVarChar:
	case DB::DBUtil::CT_Char:
	case DB::DBUtil::CT_VarChar:
	case DB::DBUtil::CT_UUID:
		return 0;
	case DB::DBUtil::CT_Double:
	case DB::DBUtil::CT_Float:
		return 0;
	case DB::DBUtil::CT_Int16:
	case DB::DBUtil::CT_UInt16:
	case DB::DBUtil::CT_Int32:
	case DB::DBUtil::CT_UInt32:
	case DB::DBUtil::CT_Int64:
	case DB::DBUtil::CT_UInt64:
	case DB::DBUtil::CT_Byte:
	case DB::DBUtil::CT_Bool:
		return 0;
	case DB::DBUtil::CT_DateTime:
	case DB::DBUtil::CT_DateTime2:
		return 0;
	case DB::DBUtil::CT_Vector:
	case DB::DBUtil::CT_Binary:
		return (UOSInt)this->colDatas[colIndex].dataVal;
	case DB::DBUtil::CT_Unknown:
	default:
		return 0;
	}
	return 0;
}

UOSInt DB::ODBCReader::GetBinary(UOSInt colIndex, UInt8 *buff)
{
	if (colIndex >= this->colCnt)
		return 0;
	if (this->colDatas[colIndex].isNull)
		return 0;
	switch (this->colDatas[colIndex].colType)
	{
	case DB::DBUtil::CT_NChar:
	case DB::DBUtil::CT_NVarChar:
	case DB::DBUtil::CT_Char:
	case DB::DBUtil::CT_VarChar:
	case DB::DBUtil::CT_UUID:
		return 0;
	case DB::DBUtil::CT_Double:
	case DB::DBUtil::CT_Float:
		return 0;
	case DB::DBUtil::CT_Int16:
	case DB::DBUtil::CT_UInt16:
	case DB::DBUtil::CT_Int32:
	case DB::DBUtil::CT_UInt32:
	case DB::DBUtil::CT_Int64:
	case DB::DBUtil::CT_UInt64:
	case DB::DBUtil::CT_Byte:
	case DB::DBUtil::CT_Bool:
		return 0;
	case DB::DBUtil::CT_DateTime:
	case DB::DBUtil::CT_DateTime2:
		return 0;
	case DB::DBUtil::CT_Vector:
	case DB::DBUtil::CT_Binary:
		MemCopyNO(buff, this->colDatas[colIndex].colData, (UOSInt)this->colDatas[colIndex].dataVal);
		return (UOSInt)(UInt64)this->colDatas[colIndex].dataVal;
	case DB::DBUtil::CT_Unknown:
	default:
		return 0;
	}
	return 0;
}

Math::Vector2D *DB::ODBCReader::GetVector(UOSInt colIndex)
{
	if (colIndex >= this->colCnt)
		return 0;
	if (this->colDatas[colIndex].isNull)
		return 0;
	if (this->conn->GetSvrType() == DB::DBUtil::ServerType::MSSQL)
	{
		if (this->colDatas[colIndex].colType == DB::DBUtil::CT_Binary || this->colDatas[colIndex].colType == DB::DBUtil::CT_Vector)
		{
			UOSInt dataSize = (UOSInt)this->colDatas[colIndex].dataVal;
			UInt8 *buffPtr = (UInt8*)this->colDatas[colIndex].colData;
			UInt32 srId;
			return Math::MSGeography::ParseBinary(buffPtr, dataSize, &srId);
		}
		else
		{
			return 0;
		}
	}
	return 0;
}

Bool DB::ODBCReader::GetUUID(UOSInt colIndex, Data::UUID *uuid)
{
	return false;
}

UTF8Char *DB::ODBCReader::GetName(UOSInt colIndex, UTF8Char *buff)
{
	Int16 nameLen = 0;
	Int16 dataType;
	UOSInt colSize;
	Int16 decimalDigit;
	Int16 notNull;
	SQLWCHAR sbuff[256];
	SQLDescribeColW((SQLHANDLE)this->hStmt, (SQLUSMALLINT)(colIndex + 1), sbuff, 512, &nameLen, &dataType, (SQLULEN*)&colSize, &decimalDigit, &notNull);
	return Text::StrUTF16_UTF8(buff, sbuff);
}

Bool DB::ODBCReader::IsNull(UOSInt colIndex)
{
	if (colIndex >= this->colCnt)
		return true;
	return this->colDatas[colIndex].isNull;
}

DB::DBUtil::ColType DB::ODBCReader::GetColType(UOSInt colIndex, UOSInt *colSize)
{
	if (colIndex >= this->colCnt)
		return DB::DBUtil::CT_Unknown;
	if (colSize)
	{
		*colSize = this->colDatas[colIndex].colSize;
	}
	return this->colDatas[colIndex].colType;
}

Bool DB::ODBCReader::GetColDef(UOSInt colIndex, DB::ColDef *colDef)
{
	SQLWCHAR sbuff[256];
	Int16 nameLen = 0;
	Int16 dataType;
	UOSInt colSize;
	Int16 decimalDigit;
	Int16 notNull;

	SQLRETURN ret = SQLDescribeColW((SQLHANDLE)this->hStmt, (SQLUSMALLINT)(colIndex + 1), sbuff, 512, &nameLen, &dataType, (SQLULEN*)&colSize, &decimalDigit, &notNull);
	if (ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO)
	{
		return false;
	}

	const UTF8Char *csptr = Text::StrToUTF8New((const UTF16Char*)sbuff);
	colDef->SetColName(csptr);
	Text::StrDelNew(csptr);
	colDef->SetColType(this->ODBCType2DBType(dataType, colSize));
	colDef->SetColSize(colSize);
	colDef->SetColDP((UInt16)decimalDigit);
	colDef->SetNotNull(notNull == SQL_NO_NULLS);
	colDef->SetPK(false);
	colDef->SetAutoInc(false);
	colDef->SetDefVal((const UTF8Char*)0);
	colDef->SetAttr((const UTF8Char*)0);
	return true;
}

DB::DBUtil::ColType DB::ODBCReader::ODBCType2DBType(Int16 odbcType, UOSInt colSize)
{
	switch (odbcType)
	{
	case SQL_CHAR:
		return DB::DBUtil::CT_Char;
	case SQL_VARCHAR:
		return DB::DBUtil::CT_VarChar;
	case SQL_LONGVARCHAR:
		return DB::DBUtil::CT_VarChar;
	case SQL_WCHAR:
		return DB::DBUtil::CT_Char;
	case SQL_WVARCHAR:
		return DB::DBUtil::CT_VarChar;
	case SQL_WLONGVARCHAR:
		return DB::DBUtil::CT_VarChar;
	case SQL_DECIMAL:
		return DB::DBUtil::CT_Double;
	case SQL_NUMERIC:
		return DB::DBUtil::CT_Double;
	case SQL_SMALLINT:
		return DB::DBUtil::CT_Int16;
	case SQL_INTEGER:
		if (this->conn->GetSvrType() == DB::DBUtil::ServerType::MySQL && colSize == 10)
		{
			return DB::DBUtil::CT_UInt32;
		}
		else
		{
			return DB::DBUtil::CT_Int32;
		}
	case SQL_REAL:
		return DB::DBUtil::CT_Float;
	case SQL_FLOAT:
		return DB::DBUtil::CT_Double;
	case SQL_DOUBLE:
		return DB::DBUtil::CT_Double;
	case SQL_BIT:
		return DB::DBUtil::CT_Bool;
	case SQL_TINYINT:
		return DB::DBUtil::CT_Byte;
	case SQL_BIGINT:
		return DB::DBUtil::CT_Int64;
	case SQL_TYPE_TIMESTAMP:
		return DB::DBUtil::CT_DateTime;
	case SQL_VARBINARY:
		return DB::DBUtil::CT_Binary;
	case SQL_LONGVARBINARY:
		return DB::DBUtil::CT_Binary;
	case -151:
		return DB::DBUtil::CT_Vector;
	case 91:
		return DB::DBUtil::CT_DateTime; //MSSQL Date
	default:
		return DB::DBUtil::CT_VarChar;
	}
}
#endif
