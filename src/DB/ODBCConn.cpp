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

#if defined(WIN32) || defined(_WIN64) || defined(__CYGWIN__) || defined(__MINGW32__)
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
		if (log)
		{
			Text::StringBuilderUTF8 sb;
			sb.Append((const UTF8Char*)"Driver is ");
			sb.Append((const UTF8Char*)buff);
			log->LogMessage(sb.ToString(), IO::ILogHandler::LOG_LEVEL_ACTION);
		}
//		Text::StrToLower(buff);
		if (Text::StrStartsWith(buff, "myodbc"))
		{
			this->svrType = DB::DBUtil::SVR_TYPE_MYSQL;
		}
		else if (Text::StrStartsWith(buff, "libmyodbc"))
		{
			this->svrType = DB::DBUtil::SVR_TYPE_MYSQL;
		}
		else if (Text::StrStartsWith(buff, "SQLSVR"))
		{
			this->svrType = DB::DBUtil::SVR_TYPE_MSSQL;
		}
		else if (Text::StrStartsWith(buff, "SQLSRV"))
		{
			this->svrType = DB::DBUtil::SVR_TYPE_MSSQL;
		}
		else if (Text::StrStartsWith(buff, "SQORA"))
		{
			this->svrType = DB::DBUtil::SVR_TYPE_ORACLE;
		}
		else if (Text::StrStartsWithICase(buff, "sqlncli"))
		{
			this->svrType = DB::DBUtil::SVR_TYPE_MSSQL;
		}
		else if (Text::StrIndexOf(buff, "sqlite") >= 0)
		{
			this->svrType = DB::DBUtil::SVR_TYPE_SQLITE;
		}
		else if (Text::StrIndexOf(buff, "odbcjt32") >= 0)
		{
			this->svrType = DB::DBUtil::SVR_TYPE_ACCESS;
		}
		else if (Text::StrStartsWith(buff, "ACEODBC"))
		{
			this->svrType = DB::DBUtil::SVR_TYPE_ACCESS;
		}
		else if (Text::StrIndexOf(buff, "msodbcsql") >= 0)
		{
			this->svrType = DB::DBUtil::SVR_TYPE_MSSQL;
		}
		else
		{
		}
	}

	if (this->svrType == DB::DBUtil::SVR_TYPE_MSSQL)
	{
		DB::DBReader *r = this->ExecuteReader((const UTF8Char*)"select getdate(), GETUTCDATE()");
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
			sb.Append((const UTF8Char*)", ");
			sb.AppendDate(&dt2);
			this->tzQhr = dt1.DiffMS(&dt2) / 900000;
		}
	}
}

Bool DB::ODBCConn::Connect(const UTF8Char *dsn, const UTF8Char *uid, const UTF8Char *pwd, const UTF8Char *schema)
{
	SQLHANDLE hand;
	SQLHANDLE hConn;
	SQLRETURN ret;
	int timeOut = 5;
	this->connErr = CE_NONE;
	this->envHand = 0;
	this->connHand = 0;
	SDEL_TEXT(this->lastErrorMsg);
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

	if (uid || pwd)
	{
		ret = SQLConnectA(hConn, (SQLCHAR*)dsn, SQL_NTS, (SQLCHAR*)uid, SQL_NTS, (SQLCHAR*)pwd, SQL_NTS);
	}
	else
	{
		ret = SQLConnectA(hConn, (SQLCHAR*)dsn, SQL_NTS, NULL, 0, NULL, 0);
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
			sb.Append((const UTF8Char*)"[");
			state[5] = 0;
			const UTF8Char *csptr = Text::StrToUTF8New((const UTF16Char*)state);
			sb.Append(csptr);
			Text::StrDelNew(csptr);
			sb.Append((const UTF8Char*)"]");
			if (msgSize > 255)
			{
				SQLWCHAR *tmpBuff = MemAlloc(SQLWCHAR, msgSize + 1);
				ret = SQLGetDiagRecW(SQL_HANDLE_DBC, hConn, 1, state, (SQLINTEGER*)&errCode, tmpBuff, msgSize + 1, &msgSize);
				csptr = Text::StrToUTF8New(tmpBuff);
				sb.Append(csptr);
				Text::StrDelNew(csptr);
				MemFree(tmpBuff);				
			}
			else
			{
				csptr = Text::StrToUTF8New(msg);
				sb.Append(csptr);
				Text::StrDelNew(csptr);
			}
		}
		this->lastErrorMsg = Text::StrCopyNew(sb.ToString());

		SQLFreeHandle(SQL_HANDLE_DBC, hConn);
		SQLFreeHandle(SQL_HANDLE_ENV, hand);
		this->connErr = CE_CONNECT_ERR;
		return false;
	}
	this->svrType = DB::DBUtil::SVR_TYPE_UNKNOWN;
	envHand = hand;
	connHand = hConn;
	this->lastDataError = DE_NO_ERROR;

	UpdateConnInfo();

	if (schema)
	{
		Text::StringBuilderUTF8 sb;
		sb.Append((const UTF8Char*)"use ");
		sb.Append(schema);
		this->ExecuteNonQuery(sb.ToString());
	}
	return true;
}

Bool DB::ODBCConn::Connect(const UTF8Char *connStr)
{
	SQLHANDLE hand;
	SQLHANDLE hConn;
	SQLRETURN ret;
	int timeOut = 5;
	if (this->connStr)
	{
		Text::StrDelNew(this->connStr);
	}
	this->connStr = Text::StrCopyNew(connStr);
	SDEL_TEXT(this->lastErrorMsg);

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
	SQLWCHAR *connBuff = MemAlloc(SQLWCHAR, Text::StrUTF8_UTF16Cnt(connStr, -1) + 1);
	SQLWCHAR *connEnd = Text::StrUTF8_UTF16(connBuff, connStr, -1, 0);
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
			sb.Append((const UTF8Char*)"[");
			state[5] = 0;
			const UTF8Char *csptr = Text::StrToUTF8New((const UTF16Char*)state);
			sb.Append(csptr);
			Text::StrDelNew(csptr);
			sb.Append((const UTF8Char*)"]");
			if (msgSize > 255)
			{
				SQLWCHAR *tmpBuff = MemAlloc(SQLWCHAR, msgSize + 1);
				ret = SQLGetDiagRecW(SQL_HANDLE_DBC, hConn, 1, state, (SQLINTEGER*)&errCode, tmpBuff, msgSize + 1, &msgSize);
				csptr = Text::StrToUTF8New(tmpBuff);
				sb.Append(csptr);
				Text::StrDelNew(csptr);
				MemFree(tmpBuff);				
			}
			else
			{
				csptr = Text::StrToUTF8New(msg);
				sb.Append(csptr);
				Text::StrDelNew(csptr);
			}
		}
		this->lastErrorMsg = Text::StrCopyNew(sb.ToString());
//		printf("ODBC Connect error: %s\r\n", sb.ToString());

		SQLFreeHandle(SQL_HANDLE_DBC, hConn);
		SQLFreeHandle(SQL_HANDLE_ENV, hand);
		this->connErr = CE_CONNECT_ERR;
		return false;
	}
	this->svrType = DB::DBUtil::SVR_TYPE_UNKNOWN;
	envHand = hand;
	connHand = hConn;
	this->lastDataError = DE_NO_ERROR;

	UpdateConnInfo();
	return true;
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
	this->Connect(connStr);
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
	if (dsn)
		this->dsn = Text::StrCopyNew(dsn);
	else
		this->dsn = 0;
	if (uid)
		this->uid = Text::StrCopyNew(uid);
	else
		this->uid = 0;
	if (pwd)
		this->pwd = Text::StrCopyNew(pwd);
	else
		this->pwd = 0;
	if (schema)
		this->schema = Text::StrCopyNew(schema);
	else
		this->schema = 0;
	lastStmtHand = 0;
	this->tzQhr = 0;
	this->Connect(this->dsn, this->uid, this->pwd, this->schema);
}

DB::ODBCConn::~ODBCConn()
{
	Close();
	SDEL_TEXT(this->dsn);
	SDEL_TEXT(this->uid);
	SDEL_TEXT(this->pwd);
	SDEL_TEXT(this->schema);
	SDEL_TEXT(this->lastErrorMsg);
	if (this->connStr)
		Text::StrDelNew(this->connStr);
	this->connStr = 0;
	if (this->tableNames)
	{
		OSInt i = this->tableNames->GetCount();
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

Int32 DB::ODBCConn::GetTzQhr()
{
	return this->tzQhr;
}

void DB::ODBCConn::GetConnName(Text::StringBuilderUTF *sb)
{
	sb->Append((const UTF8Char *)"ODBC:");
	if (this->connStr)
	{
		sb->Append(this->connStr);
	}
	else
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
	SDEL_TEXT(this->lastErrorMsg);

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
	SDEL_TEXT(this->lastErrorMsg);
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
		SQLRETURN ret = SQLGetDiagRecW(SQL_HANDLE_STMT, this->lastStmtHand, recNumber, state, (SQLINTEGER*)&errCode, msg, 256, &msgSize);
		if (ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO)
		{
			str->Append((const UTF8Char*)"[");
			state[5] = 0;
			const UTF8Char *csptr = Text::StrToUTF8New((const UTF16Char*)state);
			str->Append(csptr);
			Text::StrDelNew(csptr);
			str->Append((const UTF8Char*)"]");

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
				SQLWCHAR *tmpMsg = MemAlloc(SQLWCHAR, msgSize + 1);
				ret = SQLGetDiagRecW(SQL_HANDLE_STMT, this->lastStmtHand, recNumber, state, (SQLINTEGER*)&errCode, tmpMsg, msgSize + 1, &msgSize);
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

	SQLRETURN ret = SQLGetDiagRecW(SQL_HANDLE_STMT, this->lastStmtHand, recNumber, state, (SQLINTEGER*)&errCode, msg, sizeof(msg) / sizeof(msg[0]), &msgSize);
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
	if (this->connStr)
	{
		const UTF8Char *connStr = Text::StrCopyNew(this->connStr);
		Connect(connStr);
		Text::StrDelNew(connStr);
	}
	else
	{
		Connect(this->dsn, this->uid, this->pwd, this->schema);
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
	OSInt i;
	SQLSetConnectAttr(this->connHand, SQL_ATTR_TRACE, &en, sizeof(en));
	i = enc.WToBytes(buff, fileName, -1);
	SQLSetConnectAttr(this->connHand, SQL_ATTR_TRACEFILE, buff, (SQLINTEGER)(i - 1));
}

UTF8Char *DB::ODBCConn::ShowTablesCmd(UTF8Char *sqlstr)
{
	if (this->svrType == DB::DBUtil::SVR_TYPE_MYSQL)
		return Text::StrConcat(sqlstr, (const UTF8Char*)"show Tables");
	else if (this->svrType == DB::DBUtil::SVR_TYPE_MSSQL)
		return Text::StrConcat(sqlstr, (const UTF8Char*)"select TABLE_NAME from user_tables");
	else if (this->svrType == DB::DBUtil::SVR_TYPE_ORACLE)
		return Text::StrConcat(sqlstr, (const UTF8Char*)"select table_name from user_tables");
	else if (this->svrType == DB::DBUtil::SVR_TYPE_ACCESS)
		return Text::StrConcat(sqlstr, (const UTF8Char*)"select name from MSysObjects where type = 1");
	else
		return Text::StrConcat(sqlstr, (const UTF8Char*)"show Tables");
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
				this->tableNames->Add(Text::StrToUTF8New(sbuff));
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

DB::DBReader *DB::ODBCConn::GetTableData(const UTF8Char *name, UOSInt maxCnt, void *ordering, void *condition)
{
	UTF8Char sbuff[512];
	UTF8Char *sptr;
	Text::StringBuilderUTF8 sb;
	sb.Append((const UTF8Char*)"select ");
	if (this->svrType == DB::DBUtil::SVR_TYPE_MSSQL || this->svrType == DB::DBUtil::SVR_TYPE_ACCESS)
	{
		if (maxCnt > 0)
		{
			sb.Append((const UTF8Char*)"TOP ");
			sb.AppendOSInt(maxCnt);
			sb.AppendChar(' ', 1);
		}
	}
	sb.Append((const UTF8Char*)"* from ");
	OSInt i = 0;
	OSInt j;
	while (true)
	{
		j = Text::StrIndexOf(&name[i], '.');
		if (j < 0)
		{
			DB::DBUtil::SDBColUTF8(sbuff, &name[i], this->svrType);
			sb.Append(sbuff);
			break;
		}
		sptr = Text::StrConcatC(sbuff, &name[i], j);
		DB::DBUtil::SDBColUTF8(sptr + 1, sbuff, this->svrType);
		sb.Append(sptr + 1);
		sb.AppendChar('.', 1);
		i += j + 1;
	}
	if (this->svrType == DB::DBUtil::SVR_TYPE_SQLITE || this->svrType == DB::DBUtil::SVR_TYPE_MYSQL)
	{
		if (maxCnt > 0)
		{
			sb.Append((const UTF8Char*)" LIMIT ");
			sb.AppendOSInt(maxCnt);
		}
	}
	return this->ExecuteReader(sb.ToString());
}

void DB::ODBCConn::ShowSQLError(const UTF16Char *state, const UTF16Char *errMsg)
{
	Text::StringBuilderUTF8 sb;
	sb.Append((const UTF8Char*)"ODBC Error: [");
	const UTF8Char *csptr = Text::StrToUTF8New(state);
	sb.Append(csptr);
	Text::StrDelNew(csptr);
	sb.Append((const UTF8Char*)"] ");
	csptr = Text::StrToUTF8New(errMsg);
	sb.Append(csptr);
	Text::StrDelNew(csptr);
	this->log->LogMessage(sb.ToString(), IO::ILogHandler::LOG_LEVEL_ERR_DETAIL);
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

const UTF8Char *DB::ODBCConn::GetConnStr()
{
	return this->connStr;
}

const UTF8Char *DB::ODBCConn::GetConnDSN()
{
	return this->dsn;
}

const UTF8Char *DB::ODBCConn::GetConnUID()
{
	return this->uid;
}

const UTF8Char *DB::ODBCConn::GetConnPWD()
{
	return this->pwd;
}

const UTF8Char *DB::ODBCConn::GetConnSchema()
{
	return this->schema;
}

UOSInt DB::ODBCConn::GetDriverList(Data::ArrayList<const UTF8Char*> *driverList)
{
#if defined(WIN32) || defined(_WIN64) || defined(__CYGWIN__) || defined(__MINGW32__)
	return 0;
#else
	IO::ConfigFile *cfg = IO::IniFile::Parse((const UTF8Char*)"/etc/odbcinst.ini", 65001);
	if (cfg)
	{
		Data::ArrayList<const UTF8Char*> cateList;
		UOSInt i = 0;
		UOSInt j = cfg->GetCateList(&cateList);
		while (i < j)
		{
			driverList->Add(Text::StrCopyNew(cateList.GetItem(i)));
			i++;
		}
		DEL_CLASS(cfg);
		return j;
	}
	return 0;
#endif
}

IO::ConfigFile *DB::ODBCConn::GetDriverInfo(const UTF8Char *driverName)
{
#if defined(WIN32) || defined(_WIN64) || defined(__CYGWIN__) || defined(__MINGW32__)
	return 0;
#else
	IO::ConfigFile *cfg = IO::IniFile::Parse((const UTF8Char*)"/etc/odbcinst.ini", 65001);
	if (cfg)
	{
		IO::ConfigFile *cfgRet = cfg->CloneCate(driverName);
		DEL_CLASS(cfg);
		return cfgRet;
	}
	return 0;
#endif
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

DB::ODBCReader::ODBCReader(DB::ODBCConn *conn, void *hStmt, Bool enableDebug, Int32 tzQhr)
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
		this->colCnt = cnt;
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
		SQLDescribeColW((SQLHANDLE)this->hStmt, (SQLUSMALLINT)i + 1, buff, 10, &nameLen, &dataType, (SQLULEN*)&cSize, &decimalDigit, &notNull);
		this->colDatas[i].odbcType = dataType;
		this->colDatas[i].colType = ODBCType2DBType(dataType, cSize & 0xffff);
		this->colDatas[i].isNull = true;

		switch (this->colDatas[i].colType)
		{
		case DB::DBUtil::CT_Char:
		case DB::DBUtil::CT_VarChar:
			NEW_CLASS(this->colDatas[i].colData, Text::StringBuilderUTF8());
			break;
		case DB::DBUtil::CT_Double:
		case DB::DBUtil::CT_Float:
			break;
		case DB::DBUtil::CT_Int16:
		case DB::DBUtil::CT_UInt32:
		case DB::DBUtil::CT_Int32:
		case DB::DBUtil::CT_Byte:
		case DB::DBUtil::CT_Int64:
			break;
		case DB::DBUtil::CT_Bool:
			break;
		case DB::DBUtil::CT_DateTime:
			NEW_CLASS(this->colDatas[i].colData, Data::DateTime());
			break;
		case DB::DBUtil::CT_Vector:
		case DB::DBUtil::CT_Binary:
			this->colDatas[i].colData = 0;
			break;
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
			sb = (Text::StringBuilderUTF8*)this->colDatas[i].colData;
			DEL_CLASS(sb);
			break;
		case DB::DBUtil::CT_Double:
		case DB::DBUtil::CT_Float:
			break;
		case DB::DBUtil::CT_Int16:
		case DB::DBUtil::CT_UInt32:
		case DB::DBUtil::CT_Int32:
		case DB::DBUtil::CT_Byte:
		case DB::DBUtil::CT_Int64:
			break;
		case DB::DBUtil::CT_Bool:
			break;
		case DB::DBUtil::CT_DateTime:
			dt = (Data::DateTime*)this->colDatas[i].colData;
			DEL_CLASS(dt);
			break;
		case DB::DBUtil::CT_Binary:
			if (this->colDatas[i].colData)
				MemFree(this->colDatas[i].colData);
			break;
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
			WChar sbuff[1];
#if !defined(_WIN32) && !defined(_WIN64)
			UTF8Char *u8ptr;
#endif
			UTF16Char *sptr;
			SQLLEN len;
			SQLRETURN ret;

			if (this->enableDebug)
			{
//				printf("ODBCReader.ReadNext Col %d\r\n", i);
			}
			switch (this->colDatas[i].colType)
			{
			case DB::DBUtil::CT_Char:
			case DB::DBUtil::CT_VarChar:
				{
					len = 256;
					sb = (Text::StringBuilderUTF8*)this->colDatas[i].colData;
					sb->ClearStr();

#if !defined(_WIN32) && !defined(_WIN64)
					if (this->colDatas[i].odbcType == SQL_CHAR || this->colDatas[i].odbcType == SQL_VARCHAR || this->colDatas[i].odbcType == SQL_LONGVARCHAR)
					{
						ret = SQLGetData((SQLHANDLE)this->hStmt, (SQLUSMALLINT)(i + 1), SQL_C_CHAR, sbuff, 0, &len);
						if (ret == SQL_SUCCESS_WITH_INFO || ret == SQL_SUCCESS)
						{
							if (len == SQL_NULL_DATA)
							{
								this->colDatas[i].isNull = true;
							}
							else
							{
								if (len == -4) //SQL_NO_TOTAL
								{
									len = 2048;
								}
								u8ptr = MemAlloc(UTF8Char, len + 1);
								ret = SQLGetData((SQLHANDLE)this->hStmt, (SQLUSMALLINT)(i + 1), SQL_C_CHAR, u8ptr, len + 1, &len);
								if (ret == SQL_SUCCESS_WITH_INFO || ret == SQL_ERROR)
								{
	//								wprintf(L"ODBCReader: Char Error, len = %d, v = %ls\r\n", len, sb->GetEndPtr());
									this->conn->LogSQLError(this->hStmt);
								}
								if (ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO)
								{
									sb->AppendC(u8ptr, len);
									this->colDatas[i].isNull = false;
								}
								else
								{
									this->colDatas[i].isNull = true;
								}
								MemFree(u8ptr);
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
						ret = SQLGetData((SQLHANDLE)this->hStmt, (SQLUSMALLINT)i + 1, SQL_C_WCHAR, sbuff, 0, &len);
						if (ret == SQL_SUCCESS_WITH_INFO || ret == SQL_SUCCESS)
						{
							if (len == SQL_NULL_DATA)
							{
								this->colDatas[i].isNull = true;
							}
							else
							{
								if (len == -4) //SQL_NO_TOTAL
								{
									len = 2048;
								}
								sptr = MemAlloc(UTF16Char, (len >> 1) + 1);
								ret = SQLGetData((SQLHANDLE)this->hStmt, (SQLUSMALLINT)i + 1, SQL_C_WCHAR, sptr, len + 2, &len);
								if (ret == SQL_SUCCESS_WITH_INFO || ret == SQL_ERROR)
								{
	//								wprintf(L"ODBCReader: Char Error, len = %d, v = %ls\r\n", len, sb->GetEndPtr());
									this->conn->LogSQLError(this->hStmt);
								}
								if (ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO)
								{
									const UTF16Char *wptr = Text::StrCopyNewC(sptr, (len >> 1));
									const UTF8Char *csptr = Text::StrToUTF8New(wptr);
									sb->Append(csptr);
									Text::StrDelNew(csptr);
									Text::StrDelNew(wptr);
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
							this->colDatas[i].isNull = true;
						}
					}
				}
				break;
			case DB::DBUtil::CT_Double:
			case DB::DBUtil::CT_Float:
				{
					SQLDOUBLE dbl;
					ret = SQLGetData((SQLHANDLE)this->hStmt, (SQLUSMALLINT)i + 1, SQL_C_DOUBLE, &dbl, sizeof(dbl), &len);
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
			case DB::DBUtil::CT_UInt32:
			case DB::DBUtil::CT_Int32:
			case DB::DBUtil::CT_Byte:
			case DB::DBUtil::CT_Bool:
				{
					SQLINTEGER val;
					ret = SQLGetData((SQLHANDLE)this->hStmt, (SQLUSMALLINT)i + 1, SQL_C_SLONG, &val, sizeof(val), &len);
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
			case DB::DBUtil::CT_Int64:
				{
					SQLBIGINT val;
					ret = SQLGetData((SQLHANDLE)this->hStmt, (SQLUSMALLINT)i + 1, SQL_C_SBIGINT, &val, sizeof(val), &len);
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
				{
					TIMESTAMP_STRUCT ts;
					dt = (Data::DateTime*)this->colDatas[i].colData;
					ret = SQLGetData((SQLHANDLE)this->hStmt, (SQLUSMALLINT)i + 1, SQL_C_TIMESTAMP, &ts, sizeof(ts), &len);
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
							ms = ts.fraction / 1000000;
							dt->SetValue((UInt16)ts.year, (UInt8)ts.month, (UInt8)ts.day, (UInt8)ts.hour, (UInt8)ts.minute, (UInt8)ts.second, ms, this->tzQhr);
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
					ret = SQLGetData((SQLHANDLE)this->hStmt, (SQLUSMALLINT)i + 1, SQL_C_BINARY, &this->colDatas[i].dataVal, 0, &len);
					if (ret == SQL_SUCCESS_WITH_INFO || ret == SQL_SUCCESS)
					{
						if (len == SQL_NULL_DATA)
						{
							this->colDatas[i].isNull = true;
						}
						else
						{
							this->colDatas[i].dataVal = len;
							this->colDatas[i].colData = MemAlloc(UInt8, len);
							ret = SQLGetData((SQLHANDLE)this->hStmt, (SQLUSMALLINT)i + 1, SQL_C_BINARY, this->colDatas[i].colData, len, &len);
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
		return Text::StrToInt32(((Text::StringBuilderUTF8*)this->colDatas[colIndex].colData)->ToString());
	case DB::DBUtil::CT_Double:
	case DB::DBUtil::CT_Float:
		return Math::Double2Int32(*(Double*)&this->colDatas[colIndex].dataVal);
	case DB::DBUtil::CT_Int16:
	case DB::DBUtil::CT_UInt32:
	case DB::DBUtil::CT_Int32:
	case DB::DBUtil::CT_Byte:
	case DB::DBUtil::CT_Int64:
	case DB::DBUtil::CT_Bool:
		return (Int32)this->colDatas[colIndex].dataVal;
	case DB::DBUtil::CT_DateTime:
		return 0;
	case DB::DBUtil::CT_Binary:
		return 0;
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
		return Text::StrToInt64(((Text::StringBuilderUTF8*)this->colDatas[colIndex].colData)->ToString());
	case DB::DBUtil::CT_Double:
	case DB::DBUtil::CT_Float:
		return (Int64)(*(Double*)&this->colDatas[colIndex].dataVal);
	case DB::DBUtil::CT_Int16:
	case DB::DBUtil::CT_UInt32:
	case DB::DBUtil::CT_Int32:
	case DB::DBUtil::CT_Byte:
	case DB::DBUtil::CT_Int64:
	case DB::DBUtil::CT_Bool:
		return this->colDatas[colIndex].dataVal;
	case DB::DBUtil::CT_DateTime:
		return 0;
	case DB::DBUtil::CT_Binary:
		return 0;
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
		return Text::StrUTF8_WChar(buff, ((Text::StringBuilderUTF8*)this->colDatas[colIndex].colData)->ToString(), -1, 0);
	case DB::DBUtil::CT_Double:
	case DB::DBUtil::CT_Float:
		return Text::StrDouble(buff, *(Double*)&this->colDatas[colIndex].dataVal);
	case DB::DBUtil::CT_Int16:
	case DB::DBUtil::CT_UInt32:
	case DB::DBUtil::CT_Int32:
	case DB::DBUtil::CT_Byte:
	case DB::DBUtil::CT_Int64:
	case DB::DBUtil::CT_Bool:
		return Text::StrInt64(buff, this->colDatas[colIndex].dataVal);
	case DB::DBUtil::CT_DateTime:
		((Data::DateTime*)this->colDatas[colIndex].colData)->ToString(u8buff);
		return Text::StrUTF8_WChar(buff, u8buff, -1, 0);
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
	case DB::DBUtil::CT_Char:
	case DB::DBUtil::CT_VarChar:
		sb->Append(((Text::StringBuilderUTF8*)this->colDatas[colIndex].colData)->ToString());
		return true;
	case DB::DBUtil::CT_Double:
	case DB::DBUtil::CT_Float:
		Text::SBAppendF64(sb, *(Double*)&this->colDatas[colIndex].dataVal);
		return true;
	case DB::DBUtil::CT_Int16:
	case DB::DBUtil::CT_UInt32:
	case DB::DBUtil::CT_Int32:
	case DB::DBUtil::CT_Byte:
	case DB::DBUtil::CT_Int64:
	case DB::DBUtil::CT_Bool:
		sb->AppendI64(this->colDatas[colIndex].dataVal);
		return true;
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
	default:
		return false;
	}
	return 0;
}

const UTF8Char *DB::ODBCReader::GetNewStr(UOSInt colIndex)
{
	UTF8Char sbuff[32];
	if (colIndex >= this->colCnt)
		return 0;
	if (this->colDatas[colIndex].isNull)
		return 0;
	switch (this->colDatas[colIndex].colType)
	{
	case DB::DBUtil::CT_Char:
	case DB::DBUtil::CT_VarChar:
		return Text::StrCopyNew(((Text::StringBuilderUTF8*)this->colDatas[colIndex].colData)->ToString());
	case DB::DBUtil::CT_Double:
	case DB::DBUtil::CT_Float:
		Text::StrDouble(sbuff, *(Double*)&this->colDatas[colIndex].dataVal);
		return Text::StrCopyNew(sbuff);
	case DB::DBUtil::CT_Int16:
	case DB::DBUtil::CT_UInt32:
	case DB::DBUtil::CT_Int32:
	case DB::DBUtil::CT_Byte:
	case DB::DBUtil::CT_Int64:
	case DB::DBUtil::CT_Bool:
		Text::StrInt64(sbuff, this->colDatas[colIndex].dataVal);
		return Text::StrCopyNew(sbuff);
	case DB::DBUtil::CT_DateTime:
		((Data::DateTime*)this->colDatas[colIndex].colData)->ToString(sbuff);
		return Text::StrCopyNew(sbuff);
	case DB::DBUtil::CT_Binary:
		return 0;
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
	case DB::DBUtil::CT_Char:
	case DB::DBUtil::CT_VarChar:
		return Text::StrConcatS(buff, ((Text::StringBuilderUTF8*)this->colDatas[colIndex].colData)->ToString(), buffSize);
	case DB::DBUtil::CT_Double:
	case DB::DBUtil::CT_Float:
		return Text::StrDouble(buff, *(Double*)&this->colDatas[colIndex].dataVal);
	case DB::DBUtil::CT_Int16:
	case DB::DBUtil::CT_UInt32:
	case DB::DBUtil::CT_Int32:
	case DB::DBUtil::CT_Byte:
	case DB::DBUtil::CT_Int64:
	case DB::DBUtil::CT_Bool:
		return Text::StrInt64(buff, this->colDatas[colIndex].dataVal);
	case DB::DBUtil::CT_DateTime:
		return ((Data::DateTime*)this->colDatas[colIndex].colData)->ToString(buff);
	case DB::DBUtil::CT_Binary:
		return 0;
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
	case DB::DBUtil::CT_Char:
	case DB::DBUtil::CT_VarChar:
		outVal->SetValue(((Text::StringBuilderUTF8*)this->colDatas[colIndex].colData)->ToString());
		return DB::DBReader::DET_OK;
	case DB::DBUtil::CT_Double:
	case DB::DBUtil::CT_Float:
		return DB::DBReader::DET_ERROR;
	case DB::DBUtil::CT_Int16:
	case DB::DBUtil::CT_UInt32:
	case DB::DBUtil::CT_Int32:
	case DB::DBUtil::CT_Byte:
	case DB::DBUtil::CT_Int64:
	case DB::DBUtil::CT_Bool:
		return DB::DBReader::DET_ERROR;
	case DB::DBUtil::CT_DateTime:
		outVal->SetValue((Data::DateTime*)this->colDatas[colIndex].colData);
		return DB::DBReader::DET_OK;
	case DB::DBUtil::CT_Binary:
		return DB::DBReader::DET_ERROR;
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
	case DB::DBUtil::CT_Char:
	case DB::DBUtil::CT_VarChar:
		return Text::StrToDouble(((Text::StringBuilderUTF8*)this->colDatas[colIndex].colData)->ToString());
	case DB::DBUtil::CT_Double:
	case DB::DBUtil::CT_Float:
		return *(Double*)&this->colDatas[colIndex].dataVal;
	case DB::DBUtil::CT_Int16:
	case DB::DBUtil::CT_UInt32:
	case DB::DBUtil::CT_Int32:
	case DB::DBUtil::CT_Byte:
	case DB::DBUtil::CT_Int64:
	case DB::DBUtil::CT_Bool:
		return (Double)this->colDatas[colIndex].dataVal;
	case DB::DBUtil::CT_DateTime:
		return 0;
	case DB::DBUtil::CT_Binary:
		return 0;
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
	case DB::DBUtil::CT_Char:
	case DB::DBUtil::CT_VarChar:
		return Text::StrToInt32(((Text::StringBuilderUTF8*)this->colDatas[colIndex].colData)->ToString()) != 0;
	case DB::DBUtil::CT_Double:
	case DB::DBUtil::CT_Float:
		return (*(Double*)&this->colDatas[colIndex].dataVal) != 0;
	case DB::DBUtil::CT_Int16:
	case DB::DBUtil::CT_UInt32:
	case DB::DBUtil::CT_Int32:
	case DB::DBUtil::CT_Byte:
	case DB::DBUtil::CT_Int64:
	case DB::DBUtil::CT_Bool:
		return this->colDatas[colIndex].dataVal != 0;
	case DB::DBUtil::CT_DateTime:
		return false;
	case DB::DBUtil::CT_Binary:
		return false;
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
	case DB::DBUtil::CT_Char:
	case DB::DBUtil::CT_VarChar:
		return 0;
	case DB::DBUtil::CT_Double:
	case DB::DBUtil::CT_Float:
		return 0;
	case DB::DBUtil::CT_Int16:
	case DB::DBUtil::CT_UInt32:
	case DB::DBUtil::CT_Int32:
	case DB::DBUtil::CT_Byte:
	case DB::DBUtil::CT_Int64:
	case DB::DBUtil::CT_Bool:
		return 0;
	case DB::DBUtil::CT_DateTime:
		return 0;
	case DB::DBUtil::CT_Binary:
		return (OSInt)this->colDatas[colIndex].dataVal;
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
	case DB::DBUtil::CT_Char:
	case DB::DBUtil::CT_VarChar:
		return 0;
	case DB::DBUtil::CT_Double:
	case DB::DBUtil::CT_Float:
		return 0;
	case DB::DBUtil::CT_Int16:
	case DB::DBUtil::CT_UInt32:
	case DB::DBUtil::CT_Int32:
	case DB::DBUtil::CT_Byte:
	case DB::DBUtil::CT_Int64:
	case DB::DBUtil::CT_Bool:
		return 0;
	case DB::DBUtil::CT_DateTime:
		return 0;
	case DB::DBUtil::CT_Binary:
		MemCopyNO(buff, this->colDatas[colIndex].colData, (OSInt)this->colDatas[colIndex].dataVal);
		return (OSInt)this->colDatas[colIndex].dataVal;
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
	if (this->conn->GetSvrType() == DB::DBUtil::SVR_TYPE_MSSQL)
	{
		if (this->colDatas[colIndex].colType == DB::DBUtil::CT_Binary || this->colDatas[colIndex].colType == DB::DBUtil::CT_Vector)
		{
			OSInt dataSize = (OSInt)this->colDatas[colIndex].dataVal;
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

UTF8Char *DB::ODBCReader::GetName(UOSInt colIndex, UTF8Char *buff)
{
	Int16 nameLen = 0;
	Int16 dataType;
	UOSInt colSize;
	Int16 decimalDigit;
	Int16 notNull;
	SQLWCHAR sbuff[256];
	SQLDescribeColW((SQLHANDLE)this->hStmt, (SQLUSMALLINT)colIndex + 1, sbuff, 512, &nameLen, &dataType, (SQLULEN*)&colSize, &decimalDigit, &notNull);
	return Text::StrUTF16_UTF8(buff, sbuff, -1);
}

Bool DB::ODBCReader::IsNull(UOSInt colIndex)
{
	if (colIndex >= this->colCnt)
		return true;
	return this->colDatas[colIndex].isNull;
}

DB::DBUtil::ColType DB::ODBCReader::GetColType(UOSInt colIndex, UOSInt *colSize)
{
	Int16 nameLen;
	Int16 dataType;
	UOSInt cSize;
	Int16 decimalDigit;
	Int16 notNull;
	SQLWCHAR buff[10];
	SQLDescribeColW((SQLHANDLE)this->hStmt, (SQLUSMALLINT)colIndex + 1, buff, 10, &nameLen, &dataType, (SQLULEN*)&cSize, &decimalDigit, &notNull);
	if (colSize)
	{
		*colSize = cSize;
	}
	return ODBCType2DBType(dataType, cSize & 0xffff);
}

Bool DB::ODBCReader::GetColDef(UOSInt colIndex, DB::ColDef *colDef)
{
	SQLWCHAR sbuff[256];
	Int16 nameLen = 0;
	Int16 dataType;
	UOSInt colSize;
	Int16 decimalDigit;
	Int16 notNull;

	SQLRETURN ret = SQLDescribeColW((SQLHANDLE)this->hStmt, (SQLUSMALLINT)colIndex + 1, sbuff, 512, &nameLen, &dataType, (SQLULEN*)&colSize, &decimalDigit, &notNull);
	if (ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO)
	{
		return false;
	}

	const UTF8Char *csptr = Text::StrToUTF8New((const UTF16Char*)sbuff);
	colDef->SetColName(csptr);
	Text::StrDelNew(csptr);
	colDef->SetColType(this->ODBCType2DBType(dataType, colSize));
	colDef->SetColSize(colSize);
	colDef->SetColDP(decimalDigit);
	colDef->SetNotNull(notNull == SQL_NO_NULLS);
	colDef->SetPK(false);
	colDef->SetAutoInc(false);
	colDef->SetDefVal(0);
	colDef->SetAttr(0);
	return true;
}

void DB::ODBCReader::DelNewStr(const UTF8Char *s)
{
	if (s)
	{
		Text::StrDelNew(s);
	}
}

DB::DBUtil::ColType DB::ODBCReader::ODBCType2DBType(Int16 odbcType, UOSInt colSize)
{
	if (odbcType == SQL_CHAR)
	{
		return DB::DBUtil::CT_Char;
	}
	else if (odbcType == SQL_VARCHAR)
	{
		return DB::DBUtil::CT_VarChar;
	}
	else if (odbcType == SQL_LONGVARCHAR)
	{
		return DB::DBUtil::CT_VarChar;
	}
	else if (odbcType == SQL_WCHAR)
	{
		return DB::DBUtil::CT_Char;
	}
	else if (odbcType == SQL_WVARCHAR)
	{
		return DB::DBUtil::CT_VarChar;
	}
	else if (odbcType == SQL_WLONGVARCHAR)
	{
		return DB::DBUtil::CT_VarChar;
	}
	else if (odbcType == SQL_DECIMAL)
	{
		return DB::DBUtil::CT_Double;
	}
	else if (odbcType == SQL_NUMERIC)
	{
		return DB::DBUtil::CT_Double;
	}
	else if (odbcType == SQL_SMALLINT)
	{
		return DB::DBUtil::CT_Int16;
	}
	else if (odbcType == SQL_INTEGER)
	{
		if (this->conn->GetSvrType() == DB::DBUtil::SVR_TYPE_MYSQL && colSize == 10)
		{
			return DB::DBUtil::CT_UInt32;
		}
		else
		{
			return DB::DBUtil::CT_Int32;
		}
	}
	else if (odbcType == SQL_REAL)
	{
		return DB::DBUtil::CT_Float;
	}
	else if (odbcType == SQL_FLOAT)
	{
		return DB::DBUtil::CT_Double;
	}
	else if (odbcType == SQL_DOUBLE)
	{
		return DB::DBUtil::CT_Double;
	}
	else if (odbcType == SQL_BIT)
	{
		return DB::DBUtil::CT_Bool;
	}
	else if (odbcType == SQL_TINYINT)
	{
		return DB::DBUtil::CT_Byte;
	}
	else if (odbcType == SQL_BIGINT)
	{
		return DB::DBUtil::CT_Int64;
	}
	else if (odbcType == SQL_TYPE_TIMESTAMP)
	{
		return DB::DBUtil::CT_DateTime;
	}
	else if (odbcType == SQL_LONGVARBINARY)
	{
		return DB::DBUtil::CT_Binary;
	}
	else if (odbcType == -151)
	{
		return DB::DBUtil::CT_Vector;
	}
	else
	{
		return DB::DBUtil::CT_VarChar;
	}
}
#endif
