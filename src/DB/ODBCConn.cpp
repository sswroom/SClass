#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ByteTool.h"
#include "Data/DateTime.h"
#include "DB/DBConn.h"
#include "DB/DBTool.h"
#include "DB/ODBCConn.h"
#include "Math/Math.h"
#include "Math/MSGeography.h"
#include "Math/WKTWriter.h"
#include "Math/Geometry/Point.h"
#include "Net/MySQLUtil.h"
#include "Text/Encoding.h"
#include "Text/MyString.h"
#include "Text/MyStringFloat.h"
#include "Text/MyStringW.h"
#include "Text/StringBuilderUTF8.h"
#ifndef _WIN32_WCE

#include "Math/WKTReader.h"

#if defined(_WIN32) || defined(_WIN64) || defined(__CYGWIN__) || defined(__MINGW32__)
#include <windows.h>
#include "IO/Registry.h"
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
	UTF8Char buff[256];
	Int16 buffSize;
	SQLRETURN ret;
	ret = SQLGetInfoA(connHand, SQL_DRIVER_NAME, (Char*)buff, sizeof(buff), &buffSize);
	if (ret == SQL_SUCCESS || SQL_SUCCESS_WITH_INFO)
	{
		if (buffSize <= 0)
		{
			buff[0] = 0;
			buffSize = 0;
		}
	
		if (log->HasHandler())
		{
			Text::StringBuilderUTF8 sb;
			sb.AppendC(UTF8STRC("Driver is "));
			sb.AppendC((const UTF8Char*)buff, (UOSInt)buffSize);
			log->LogMessage(sb.ToCString(), IO::LogHandler::LogLevel::Action);
		}
//		Text::StrToLowerC(buff, buff);
		if (Text::StrStartsWithC(buff, (UOSInt)buffSize, UTF8STRC("myodbc")))
		{
			this->sqlType = DB::SQLType::MySQL;
		}
		else if (Text::StrStartsWithC(buff, (UOSInt)buffSize, UTF8STRC("libmyodbc")))
		{
			this->sqlType = DB::SQLType::MySQL;
		}
		else if (Text::StrStartsWithC(buff, (UOSInt)buffSize, UTF8STRC("SQLSVR")))
		{
			this->sqlType = DB::SQLType::MSSQL;
		}
		else if (Text::StrStartsWithC(buff, (UOSInt)buffSize, UTF8STRC("SQLSRV")))
		{
			this->sqlType = DB::SQLType::MSSQL;
		}
		else if (Text::StrStartsWithC(buff, (UOSInt)buffSize, UTF8STRC("SQORA")))
		{
			this->sqlType = DB::SQLType::Oracle;
		}
		else if (Text::StrStartsWithICaseC(buff, (UOSInt)buffSize, UTF8STRC("sqlncli")))
		{
			this->sqlType = DB::SQLType::MSSQL;
		}
		else if (Text::StrIndexOfC(buff, (UOSInt)buffSize, UTF8STRC("sqlite")) != INVALID_INDEX)
		{
			this->sqlType = DB::SQLType::SQLite;
		}
		else if (Text::StrIndexOfC(buff, (UOSInt)buffSize, UTF8STRC("odbcjt32")) != INVALID_INDEX)
		{
			this->sqlType = DB::SQLType::Access;
		}
		else if (Text::StrStartsWithC(buff, (UOSInt)buffSize, UTF8STRC("ACEODBC")))
		{
			this->sqlType = DB::SQLType::Access;
		}
		else if (Text::StrIndexOfC(buff, (UOSInt)buffSize, UTF8STRC("msodbcsql")) != INVALID_INDEX)
		{
			this->sqlType = DB::SQLType::MSSQL;
		}
		else if (Text::StrIndexOfC(buff, (UOSInt)buffSize, UTF8STRC("libtdsodbc")) != INVALID_INDEX)
		{
			this->sqlType = DB::SQLType::MSSQL;
		}
		else
		{
		}
	}
	if (this->sqlType == DB::SQLType::Unknown)
	{
		if (this->connStr->IndexOfICase(UTF8STRC("DRIVER=MDBTOOLS;")) != INVALID_INDEX)
		{
			this->sqlType = DB::SQLType::MDBTools;
		}
	}

	if (this->sqlType == DB::SQLType::MSSQL)
	{
		NotNullPtr<DB::DBReader> r;
		if (r.Set(this->ExecuteReader(CSTR("select getdate(), GETUTCDATE()"))))
		{
			r->ReadNext();
			Data::Timestamp ts1 = r->GetTimestamp(0);
			Data::Timestamp ts2 = r->GetTimestamp(1);
			this->CloseReader(r);
			this->tzQhr = (Int8)((ts1.inst.sec + 1 - ts2.inst.sec) / 900);
		}
	}
	else if (this->sqlType == DB::SQLType::MySQL)
	{
		NotNullPtr<DB::DBReader> r;
		if (r.Set(this->ExecuteReader(CSTR("SELECT VERSION()"))))
		{
			r->ReadNext();
			Text::String *s = r->GetNewStr(0);
			this->CloseReader(r);
			if (s)
			{
				this->axisAware = Net::MySQLUtil::IsAxisAware(s->ToCString());
				s->Release();
			}
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
			NotNullPtr<Text::String> s = Text::String::NewNotNull((const UTF16Char*)state);
			sb.Append(s);
			s->Release();
			sb.AppendC(UTF8STRC("]"));
			if (msgSize > 255)
			{
				SQLWCHAR *tmpBuff = MemAlloc(SQLWCHAR, (UInt16)(msgSize + 1));
				ret = SQLGetDiagRecW(SQL_HANDLE_DBC, hConn, 1, state, (SQLINTEGER*)&errCode, tmpBuff, (Int16)(msgSize + 1), &msgSize);
				s = Text::String::NewNotNull((const UTF16Char*)tmpBuff);
				sb.AppendC(s->v, s->leng);
				s->Release();
				MemFree(tmpBuff);				
			}
			else
			{
				s = Text::String::NewNotNull((const UTF16Char*)msg);
				sb.AppendC(s->v, s->leng);
				s->Release();
			}
		}
		this->lastErrorMsg = Text::String::New(sb.ToString(), sb.GetLength()).Ptr();

		SQLFreeHandle(SQL_HANDLE_DBC, hConn);
		SQLFreeHandle(SQL_HANDLE_ENV, hand);
		this->connErr = CE_CONNECT_ERR;
		return false;
	}
	this->sqlType = DB::SQLType::Unknown;
	envHand = hand;
	connHand = hConn;
	this->lastDataError = DE_NO_ERROR;

	UpdateConnInfo();

	if (schema && schema->leng > 0)
	{
		Text::StringBuilderUTF8 sb;
		sb.AppendC(UTF8STRC("use "));
		sb.Append(schema);
		this->ExecuteNonQuery(sb.ToCString());
	}
	return true;
}

Bool DB::ODBCConn::Connect(NotNullPtr<Text::String> connStr)
{
	SQLHANDLE hand;
	SQLHANDLE hConn;
	SQLRETURN ret;
	int timeOut = 5;
	SDEL_STRING(this->connStr);
	this->connStr = connStr->Clone().Ptr();
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
	UOSInt outMaxSize = Text::StrUTF8_UTF16CntC(connStr->v, connStr->leng);
	SQLWCHAR *connBuff = MemAlloc(SQLWCHAR, outMaxSize + 2);
	SQLWCHAR *connEnd = (SQLWCHAR*)Text::StrUTF8_UTF16C((UTF16Char*)connBuff, connStr->v, connStr->leng, 0);
	connEnd[0] = 0;
	connEnd[1] = 0;
	outSize = 0;
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
			NotNullPtr<Text::String> s = Text::String::NewNotNull((const UTF16Char*)state);
			sb.Append(s);
			s->Release();
			sb.AppendC(UTF8STRC("]"));
			if (msgSize > 255)
			{
				SQLWCHAR *tmpBuff = MemAlloc(SQLWCHAR, (UInt16)(msgSize + 1));
				ret = SQLGetDiagRecW(SQL_HANDLE_DBC, hConn, 1, state, (SQLINTEGER*)&errCode, tmpBuff, (Int16)(msgSize + 1), &msgSize);
				s = Text::String::NewNotNull((const UTF16Char*)tmpBuff);
				sb.Append(s);
				s->Release();
				MemFree(tmpBuff);				
			}
			else
			{
				s = Text::String::NewNotNull((const UTF16Char*)msg);
				sb.Append(s);
				s->Release();
			}
		}
		this->lastErrorMsg = Text::String::New(sb.ToString(), sb.GetLength()).Ptr();
//		printf("ODBC Connect error: %s\r\n", sb.ToString());

		SQLFreeHandle(SQL_HANDLE_DBC, hConn);
		SQLFreeHandle(SQL_HANDLE_ENV, hand);
		this->connErr = CE_CONNECT_ERR;
		return false;
	}
	this->sqlType = DB::SQLType::Unknown;
	envHand = hand;
	connHand = hConn;
	this->lastDataError = DE_NO_ERROR;

	UpdateConnInfo();
	return true;
}

Bool DB::ODBCConn::Connect(Text::CString connStr)
{
	NotNullPtr<Text::String> s = Text::String::New(connStr);
	Bool ret = this->Connect(s);
	s->Release();
	return ret;
}

DB::ODBCConn::ODBCConn(Text::CStringNN sourceName, NotNullPtr<IO::LogTool> log) : DB::DBConn(sourceName)
{
	connHand = 0;
	envHand = 0;
	lastStmtHand = 0;
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
	this->axisAware = false;
}

DB::ODBCConn::ODBCConn(Text::CString connStr, Text::CStringNN sourceName, NotNullPtr<IO::LogTool> log) : DB::DBConn(sourceName)
{
	connHand = 0;
	envHand = 0;
	lastStmtHand = 0;
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
	this->axisAware = false;
	NotNullPtr<Text::String> s = Text::String::New(connStr);
	this->Connect(s);
	s->Release();
}

DB::ODBCConn::ODBCConn(NotNullPtr<Text::String> dsn, Text::String *uid, Text::String *pwd, Text::String *schema, NotNullPtr<IO::LogTool> log) : DB::DBConn(dsn)
{
	this->log = log;
	this->connStr = 0;
	this->connHand = 0;
	this->connErr = CE_NOT_CONNECT;
	this->lastErrorMsg = 0;
	this->envHand = 0;
	this->enableDebug = false;
	this->dsn = dsn->Clone().Ptr();
	this->uid = SCOPY_STRING(uid);
	this->pwd = SCOPY_STRING(pwd);
	this->schema = SCOPY_STRING(schema);
	lastStmtHand = 0;
	this->tzQhr = 0;
	this->forceTz = false;
	this->axisAware = false;
	this->Connect(this->dsn, this->uid, this->pwd, this->schema);
}

DB::ODBCConn::ODBCConn(Text::CStringNN dsn, Text::CString uid, Text::CString pwd, Text::CString schema, NotNullPtr<IO::LogTool> log) : DB::DBConn(dsn)
{
	this->log = log;
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
	this->axisAware = false;
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
}

DB::SQLType DB::ODBCConn::GetSQLType() const
{
	return sqlType;
}

Bool DB::ODBCConn::IsAxisAware() const
{
	return this->axisAware;
}

DB::DBConn::ConnType DB::ODBCConn::GetConnType() const
{
	return CT_ODBC;
}

Int8 DB::ODBCConn::GetTzQhr() const
{
	return this->tzQhr;
}

void DB::ODBCConn::GetConnName(NotNullPtr<Text::StringBuilderUTF8> sb)
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
			sb->AppendUTF8Char('/');
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

OSInt DB::ODBCConn::ExecuteNonQuery(Text::CStringNN sql)
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
	const WChar *wptr = Text::StrToWCharNew(sql.v);
	ret = SQLPrepareW(hStmt, (SQLWCHAR*)wptr, SQL_NTS);
	Text::StrDelNew(wptr);
	#else
	ret = SQLPrepareA(hStmt, (SQLCHAR*)sql.v, SQL_NTS);
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

DB::DBReader *DB::ODBCConn::ExecuteReader(Text::CStringNN sql)
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
	const WChar *wptr = Text::StrToWCharNew(sql.v);
	ret = SQLPrepareW(hStmt, (SQLWCHAR*)wptr, SQL_NTS);
	Text::StrDelNew(wptr);
	#else
	ret = SQLPrepare(hStmt, (SQLCHAR*)sql.v, SQL_NTS);
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

void DB::ODBCConn::CloseReader(NotNullPtr<DB::DBReader> r)
{
	DB::ODBCReader *rdr = (DB::ODBCReader*)r.Ptr();
	DEL_CLASS(rdr);
}

void DB::ODBCConn::GetLastErrorMsg(NotNullPtr<Text::StringBuilderUTF8> str)
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
			NotNullPtr<Text::String> s = Text::String::NewNotNull((const UTF16Char*)state);
			str->Append(s);
			str->AppendC(UTF8STRC("]"));

			if (s->Equals(UTF8STRC("23000")))
				this->lastStmtState = 3;
			else if (s->Equals(UTF8STRC("42000")))
				this->lastStmtState = 3;
			else if (s->Equals(UTF8STRC("HY000")))
				this->lastStmtState = 3;
			else
				this->lastStmtState = 2;
			s->Release();

			if (msgSize > 255)
			{
				SQLWCHAR *tmpMsg = MemAlloc(SQLWCHAR, (UInt16)(msgSize + 1));
				ret = SQLGetDiagRecW(SQL_HANDLE_STMT, this->lastStmtHand, (SQLSMALLINT)recNumber, state, (SQLINTEGER*)&errCode, tmpMsg, (SQLSMALLINT)(msgSize + 1), &msgSize);
				s = Text::String::NewNotNull((const UTF16Char*)tmpMsg);
				str->Append(s);
				s->Release();
				MemFree(tmpMsg);
			}
			else
			{
				s = Text::String::NewNotNull((const UTF16Char*)msg);
				str->Append(s);
				s->Release();
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
		state[5] = 0;
		NotNullPtr<Text::String> s = Text::String::New((const UTF16Char*)state, 5);
		Bool ret = false;
		if (s->Equals(UTF8STRC("23000")))
			ret = true;
		else if (s->Equals(UTF8STRC("42000")))
			ret = true;
		else if (s->Equals(UTF8STRC("HY000")))
			ret = true;
		s->Release();
		return ret;
	}
	return false;
}

void DB::ODBCConn::Reconnect()
{
	Close();
	Int8 oldTzQhr = this->tzQhr;
	if (this->connStr)
	{
		NotNullPtr<Text::String> connStr = this->connStr->Clone();
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
		if (this->sqlType == DB::SQLType::MSSQL)
		{
			this->ExecuteNonQuery(CSTR("BEGIN TRANSACTION"));
		}
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

	if (this->sqlType == DB::SQLType::MSSQL)
	{
		this->ExecuteNonQuery(CSTR("COMMIT"));
	}
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

	if (this->sqlType == DB::SQLType::MSSQL)
	{
		this->ExecuteNonQuery(CSTR("ROLLBACK"));
	}
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
	if (this->sqlType == DB::SQLType::MySQL)
		return Text::StrConcatC(sqlstr, UTF8STRC("show Tables"));
	else if (this->sqlType == DB::SQLType::MSSQL)
		return Text::StrConcatC(sqlstr, UTF8STRC("select TABLE_NAME from user_tables"));
	else if (this->sqlType == DB::SQLType::Oracle)
		return Text::StrConcatC(sqlstr, UTF8STRC("select table_name from user_tables"));
	else if (this->sqlType == DB::SQLType::Access)
		return Text::StrConcatC(sqlstr, UTF8STRC("select name from MSysObjects where type = 1"));
	else if (this->sqlType == DB::SQLType::MDBTools)
		return Text::StrConcatC(sqlstr, UTF8STRC("select name from MSysObjects where type = 1"));
	else
		return Text::StrConcatC(sqlstr, UTF8STRC("show Tables"));
}

DB::DBReader *DB::ODBCConn::GetTablesInfo(Text::CString schemaName)
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

UOSInt DB::ODBCConn::QueryTableNames(Text::CString schemaName, NotNullPtr<Data::ArrayListNN<Text::String>> names)
{
	UTF8Char sbuff[256];
	UTF8Char *sptr;
//		ShowTablesCmd(sbuff);
//		DB::ReadingDB::DBReader *rdr = this->ExecuteReader(sbuff);
	UOSInt initCnt = names->GetCount();
	NotNullPtr<DB::DBReader> rdr;
	if (rdr.Set(this->GetTablesInfo(schemaName)))
	{
		sbuff[0] = 0;
		while (rdr->ReadNext())
		{
			sptr = rdr->GetStr(2, sbuff, sizeof(sbuff));
			if (sptr == 0)
			{
				sptr = sbuff;
			}
			if (Text::StrStartsWithC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("~sq_")))
			{

			}
			else
			{
				names->Add(Text::String::NewP(sbuff, sptr));
			}
		}
		this->CloseReader(rdr);
	}
	return names->GetCount() - initCnt;
}

DB::DBReader *DB::ODBCConn::QueryTableData(Text::CString schemaName, Text::CString tableName, Data::ArrayListNN<Text::String> *columnNames, UOSInt ofst, UOSInt maxCnt, Text::CString ordering, Data::QueryConditions *condition)
{
	UTF8Char sbuff[512];
	UTF8Char *sptr;
	UOSInt i;
	UOSInt j;
	Text::StringBuilderUTF8 sb;
	sb.AppendC(UTF8STRC("select "));
	if (this->sqlType == DB::SQLType::MSSQL || this->sqlType == DB::SQLType::Access)
	{
		if (maxCnt > 0)
		{
			sb.AppendC(UTF8STRC("TOP "));
			sb.AppendUOSInt(maxCnt);
			sb.AppendUTF8Char(' ');
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
			sptr = DB::DBUtil::SDBColUTF8(sbuff, columnNames->GetItem(i)->v, this->sqlType);
			sb.AppendC(sbuff, (UOSInt)(sptr - sbuff));
			i++;
		}
	}
	sb.AppendC(UTF8STRC(" from "));
	if (schemaName.leng > 0 && DB::DBUtil::HasSchema(this->sqlType))
	{
		sptr = DB::DBUtil::SDBColUTF8(sbuff, schemaName.v, this->sqlType);
		sb.AppendP(sbuff, sptr);
		sb.AppendUTF8Char('.');
	}
	sptr = DB::DBUtil::SDBColUTF8(sbuff, tableName.v, this->sqlType);
	sb.AppendP(sbuff, sptr);
	if (this->sqlType == DB::SQLType::SQLite || this->sqlType == DB::SQLType::MySQL)
	{
		if (maxCnt > 0)
		{
			sb.AppendC(UTF8STRC(" LIMIT "));
			sb.AppendUOSInt(maxCnt);
		}
	}
	return this->ExecuteReader(sb.ToCString());
}

void DB::ODBCConn::ShowSQLError(const UTF16Char *state, const UTF16Char *errMsg)
{
	Text::StringBuilderUTF8 sb;
	sb.AppendC(UTF8STRC("ODBC Error: ["));
	NotNullPtr<Text::String> s = Text::String::NewNotNull(state);
	sb.Append(s);
	s->Release();
	sb.AppendC(UTF8STRC("] "));
	s = Text::String::NewNotNull(errMsg);
	sb.Append(s);
	s->Release();
	this->log->LogMessage(sb.ToCString(), IO::LogHandler::LogLevel::ErrorDetail);
}

void DB::ODBCConn::LogSQLError(void *hStmt)
{
	SQLWCHAR state[6];
	SQLWCHAR errorMsg[1024];
	if (!this->log->HasHandler())
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

UOSInt DB::ODBCConn::GetDriverList(Data::ArrayListNN<Text::String> *driverList)
{
#if defined(WIN32) || defined(_WIN64) || defined(__CYGWIN__) || defined(__MINGW32__)
	WChar wbuff[512];
	IO::Registry* reg = IO::Registry::OpenLocalSoftware(L"ODBC\\ODBCINST.INI\\ODBC Drivers");
	UOSInt i = 0;
	if (reg)
	{
		while (reg->GetName(wbuff, i))
		{
			driverList->Add(Text::String::NewNotNull(wbuff));
			i++;
		}
		IO::Registry::CloseRegistry(reg);
	}
	return i;
#else
	IO::ConfigFile *cfg = IO::IniFile::Parse(CSTR("/etc/odbcinst.ini"), 65001);
	if (cfg)
	{
		Data::ArrayListNN<Text::String> cateList;
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

IO::ConfigFile *DB::ODBCConn::GetDriverInfo(Text::CString driverName)
{
#if defined(WIN32) || defined(_WIN64) || defined(__CYGWIN__) || defined(__MINGW32__)
	return 0;
#else
	IO::ConfigFile *cfg = IO::IniFile::Parse(CSTR("/etc/odbcinst.ini"), 65001);
	if (cfg)
	{
		IO::ConfigFile *cfgRet = cfg->CloneCate(driverName);
		DEL_CLASS(cfg);
		return cfgRet;
	}
	return 0;
#endif
}

DB::DBTool *DB::ODBCConn::CreateDBTool(NotNullPtr<Text::String> dsn, Text::String *uid, Text::String *pwd, Text::String *schema, NotNullPtr<IO::LogTool> log, Text::CString logPrefix)
{
	NotNullPtr<DB::ODBCConn> conn;
	DB::DBTool *db;
	NEW_CLASSNN(conn, DB::ODBCConn(dsn, uid, pwd, schema, log));
	if (conn->GetConnError() == CE_NONE)
	{
		NEW_CLASS(db, DB::DBTool(conn, true, log, logPrefix));
		return db;
	}
	else
	{
		conn.Delete();
		return 0;
	}
}

DB::DBTool *DB::ODBCConn::CreateDBTool(Text::CStringNN dsn, Text::CString uid, Text::CString pwd, Text::CString schema, NotNullPtr<IO::LogTool> log, Text::CString logPrefix)
{
	NotNullPtr<DB::ODBCConn> conn;
	DB::DBTool *db;
	NEW_CLASSNN(conn, DB::ODBCConn(dsn, uid, pwd, schema, log));
	if (conn->GetConnError() == CE_NONE)
	{
		NEW_CLASS(db, DB::DBTool(conn, true, log, logPrefix));
		return db;
	}
	else
	{
		conn.Delete();
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
		case DB::DBUtil::CT_UTF8Char:
		case DB::DBUtil::CT_UTF16Char:
		case DB::DBUtil::CT_UTF32Char:
		case DB::DBUtil::CT_VarUTF8Char:
		case DB::DBUtil::CT_VarUTF16Char:
		case DB::DBUtil::CT_VarUTF32Char:
		case DB::DBUtil::CT_UUID:
			NEW_CLASS(this->colDatas[i].colData, Text::StringBuilderUTF8());
			break;
		case DB::DBUtil::CT_Double:
		case DB::DBUtil::CT_Float:
		case DB::DBUtil::CT_Decimal:
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
		case DB::DBUtil::CT_DateTimeTZ:
		case DB::DBUtil::CT_DateTime:
		case DB::DBUtil::CT_Date:
			this->colDatas[i].colData = MemAlloc(Data::Timestamp, 1);
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
	Data::Timestamp *ts;
	i = this->colCnt;
	while (i-- > 0)
	{
		switch (this->colDatas[i].colType)
		{
		case DB::DBUtil::CT_UTF8Char:
		case DB::DBUtil::CT_UTF16Char:
		case DB::DBUtil::CT_UTF32Char:
		case DB::DBUtil::CT_VarUTF8Char:
		case DB::DBUtil::CT_VarUTF16Char:
		case DB::DBUtil::CT_VarUTF32Char:
		case DB::DBUtil::CT_UUID:
			sb = (Text::StringBuilderUTF8*)this->colDatas[i].colData;
			DEL_CLASS(sb);
			break;
		case DB::DBUtil::CT_Double:
		case DB::DBUtil::CT_Float:
		case DB::DBUtil::CT_Decimal:
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
		case DB::DBUtil::CT_DateTimeTZ:
		case DB::DBUtil::CT_DateTime:
		case DB::DBUtil::CT_Date:
			ts = (Data::Timestamp*)this->colDatas[i].colData;
			MemFree(ts);
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
			Data::Timestamp *dt;
			UTF16Char *sptr;
			SQLLEN len;
			SQLRETURN ret;

			if (this->enableDebug)
			{
//				printf("ODBCReader.ReadNext Col %d\r\n", i);
			}
			switch (this->colDatas[i].colType)
			{
			case DB::DBUtil::CT_UTF8Char:
			case DB::DBUtil::CT_UTF16Char:
			case DB::DBUtil::CT_UTF32Char:
			case DB::DBUtil::CT_VarUTF8Char:
			case DB::DBUtil::CT_VarUTF16Char:
			case DB::DBUtil::CT_VarUTF32Char:
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
									if (len == -4) //SQL_NO_TOTAL
									{
										len = (SQLLEN)Text::StrCharCnt(sptr) * 2;
									}
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
			case DB::DBUtil::CT_Decimal:
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
							this->colDatas[i].isNull = true;
						}
						else if (len > 0 && len <= 255)
						{
							if (ret == SQL_SUCCESS_WITH_INFO)
							{
	//								wprintf(L"ODBCReader: Char Error, len = %d, v = %ls\r\n", len, sb->GetEndPtr());
								this->conn->LogSQLError(this->hStmt);
							}
							sbuff[len] = 0;
							*(Double*)&this->colDatas[i].dataVal = Text::StrToDouble(sbuff);
							this->colDatas[i].isNull = false;
						}
						else
						{
							this->colDatas[i].isNull = true;
						}
					}
					else
					{
						this->colDatas[i].isNull = true;
					}
				}
				break;
			case DB::DBUtil::CT_Int32:
			case DB::DBUtil::CT_Int16:
			case DB::DBUtil::CT_UInt16:
			case DB::DBUtil::CT_UInt32:
			case DB::DBUtil::CT_Byte:
			case DB::DBUtil::CT_Bool:
				this->colDatas[i].isNull = true;
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
			case DB::DBUtil::CT_DateTimeTZ:
			case DB::DBUtil::CT_DateTime:
			case DB::DBUtil::CT_Date:
				{
					TIMESTAMP_STRUCT ts;
					dt = (Data::Timestamp*)this->colDatas[i].colData;
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
							Data::DateTimeUtil::TimeValue tval;
							tval.year = (UInt16)ts.year;
							tval.month = (UInt8)ts.month;
							tval.day = (UInt8)ts.day;
							tval.hour = (UInt8)ts.hour;
							tval.minute = (UInt8)ts.minute;
							tval.second = (UInt8)ts.second;
							*dt = Data::Timestamp(Data::TimeInstant(Data::DateTimeUtil::TimeValue2Secs(tval, this->tzQhr), ts.fraction), this->tzQhr);
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
	case DB::DBUtil::CT_UTF8Char:
	case DB::DBUtil::CT_UTF16Char:
	case DB::DBUtil::CT_UTF32Char:
	case DB::DBUtil::CT_VarUTF8Char:
	case DB::DBUtil::CT_VarUTF16Char:
	case DB::DBUtil::CT_VarUTF32Char:
	case DB::DBUtil::CT_UUID:
		return Text::StrToInt32(((Text::StringBuilderUTF8*)this->colDatas[colIndex].colData)->ToString());
	case DB::DBUtil::CT_Double:
	case DB::DBUtil::CT_Float:
	case DB::DBUtil::CT_Decimal:
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
	case DB::DBUtil::CT_DateTimeTZ:
	case DB::DBUtil::CT_DateTime:
	case DB::DBUtil::CT_Date:
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
	case DB::DBUtil::CT_UTF8Char:
	case DB::DBUtil::CT_UTF16Char:
	case DB::DBUtil::CT_UTF32Char:
	case DB::DBUtil::CT_VarUTF8Char:
	case DB::DBUtil::CT_VarUTF16Char:
	case DB::DBUtil::CT_VarUTF32Char:
	case DB::DBUtil::CT_UUID:
		return Text::StrToInt64(((Text::StringBuilderUTF8*)this->colDatas[colIndex].colData)->ToString());
	case DB::DBUtil::CT_Double:
	case DB::DBUtil::CT_Float:
	case DB::DBUtil::CT_Decimal:
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
	case DB::DBUtil::CT_DateTimeTZ:
	case DB::DBUtil::CT_DateTime:
	case DB::DBUtil::CT_Date:
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
	UTF8Char sbuff[64];
	if (colIndex >= this->colCnt)
		return 0;
	if (this->colDatas[colIndex].isNull)
		return 0;
	switch (this->colDatas[colIndex].colType)
	{
	case DB::DBUtil::CT_UTF8Char:
	case DB::DBUtil::CT_UTF16Char:
	case DB::DBUtil::CT_UTF32Char:
	case DB::DBUtil::CT_VarUTF8Char:
	case DB::DBUtil::CT_VarUTF16Char:
	case DB::DBUtil::CT_VarUTF32Char:
	case DB::DBUtil::CT_UUID:
		return Text::StrUTF8_WChar(buff, ((Text::StringBuilderUTF8*)this->colDatas[colIndex].colData)->ToString(), 0);
	case DB::DBUtil::CT_Double:
	case DB::DBUtil::CT_Float:
	case DB::DBUtil::CT_Decimal:
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
	case DB::DBUtil::CT_DateTimeTZ:
	case DB::DBUtil::CT_DateTime:
	case DB::DBUtil::CT_Date:
		((Data::Timestamp*)this->colDatas[colIndex].colData)->ToString(sbuff);
		return Text::StrUTF8_WChar(buff, sbuff, 0);
	case DB::DBUtil::CT_Binary:
		return 0;
	case DB::DBUtil::CT_Vector:
		{
			Math::Geometry::Vector2D *vec = this->GetVector(colIndex);
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

Bool DB::ODBCReader::GetStr(UOSInt colIndex, NotNullPtr<Text::StringBuilderUTF8> sb)
{
	if (colIndex >= this->colCnt)
		return false;
	if (this->colDatas[colIndex].isNull)
		return false;
	switch (this->colDatas[colIndex].colType)
	{
	case DB::DBUtil::CT_UTF8Char:
	case DB::DBUtil::CT_UTF16Char:
	case DB::DBUtil::CT_UTF32Char:
	case DB::DBUtil::CT_VarUTF8Char:
	case DB::DBUtil::CT_VarUTF16Char:
	case DB::DBUtil::CT_VarUTF32Char:
	case DB::DBUtil::CT_UUID:
		sb->Append((Text::StringBuilderUTF8*)this->colDatas[colIndex].colData);
		return true;
	case DB::DBUtil::CT_Double:
	case DB::DBUtil::CT_Float:
	case DB::DBUtil::CT_Decimal:
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
	case DB::DBUtil::CT_Date:
	case DB::DBUtil::CT_DateTime:
	case DB::DBUtil::CT_DateTimeTZ:
		sb->AppendTS(*(Data::Timestamp*)this->colDatas[colIndex].colData);
		return true;
	case DB::DBUtil::CT_Binary:
		return false;
	case DB::DBUtil::CT_Vector:
		{
			NotNullPtr<Math::Geometry::Vector2D> vec;
			if (vec.Set(this->GetVector(colIndex)))
			{
				Math::WKTWriter wkt;
				wkt.ToText(sb, vec);
				vec.Delete();
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
	case DB::DBUtil::CT_UTF8Char:
	case DB::DBUtil::CT_UTF16Char:
	case DB::DBUtil::CT_UTF32Char:
	case DB::DBUtil::CT_VarUTF8Char:
	case DB::DBUtil::CT_VarUTF16Char:
	case DB::DBUtil::CT_VarUTF32Char:
	case DB::DBUtil::CT_UUID:
		{
			Text::StringBuilderUTF8 *sb = (Text::StringBuilderUTF8*)this->colDatas[colIndex].colData;
			return Text::String::New(sb->ToString(), sb->GetLength()).Ptr();
		}
	case DB::DBUtil::CT_Double:
	case DB::DBUtil::CT_Float:
	case DB::DBUtil::CT_Decimal:
		sptr = Text::StrDouble(sbuff, *(Double*)&this->colDatas[colIndex].dataVal);
		return Text::String::New(sbuff, (UOSInt)(sptr - sbuff)).Ptr();
	case DB::DBUtil::CT_Int16:
	case DB::DBUtil::CT_Int32:
	case DB::DBUtil::CT_Byte:
	case DB::DBUtil::CT_Int64:
	case DB::DBUtil::CT_Bool:
		sptr = Text::StrInt64(sbuff, this->colDatas[colIndex].dataVal);
		return Text::String::New(sbuff, (UOSInt)(sptr - sbuff)).Ptr();
	case DB::DBUtil::CT_UInt64:
		sptr = Text::StrUInt64(sbuff, (UInt64)this->colDatas[colIndex].dataVal);
		return Text::String::New(sbuff, (UOSInt)(sptr - sbuff)).Ptr();
	case DB::DBUtil::CT_UInt32:
		sptr = Text::StrUInt32(sbuff, (UInt32)this->colDatas[colIndex].dataVal);
		return Text::String::New(sbuff, (UOSInt)(sptr - sbuff)).Ptr();
	case DB::DBUtil::CT_UInt16:
		sptr = Text::StrUInt16(sbuff, (UInt16)this->colDatas[colIndex].dataVal);
		return Text::String::New(sbuff, (UOSInt)(sptr - sbuff)).Ptr();
	case DB::DBUtil::CT_DateTimeTZ:
	case DB::DBUtil::CT_DateTime:
	case DB::DBUtil::CT_Date:
		sptr = ((Data::Timestamp*)this->colDatas[colIndex].colData)->ToString(sbuff);
		return Text::String::New(sbuff, (UOSInt)(sptr - sbuff)).Ptr();
	case DB::DBUtil::CT_Binary:
		return 0;
	case DB::DBUtil::CT_Vector:
		{
			NotNullPtr<Math::Geometry::Vector2D> vec;
			if (vec.Set(this->GetVector(colIndex)))
			{
				Text::StringBuilderUTF8 sb;
				Math::WKTWriter wkt;
				wkt.ToText(sb, vec);
				vec.Delete();
				return Text::String::New(sb.ToString(), sb.GetLength()).Ptr();
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
	case DB::DBUtil::CT_UTF8Char:
	case DB::DBUtil::CT_UTF16Char:
	case DB::DBUtil::CT_UTF32Char:
	case DB::DBUtil::CT_VarUTF8Char:
	case DB::DBUtil::CT_VarUTF16Char:
	case DB::DBUtil::CT_VarUTF32Char:
	case DB::DBUtil::CT_UUID:
		return Text::StrConcatS(buff, ((Text::StringBuilderUTF8*)this->colDatas[colIndex].colData)->ToString(), buffSize);
	case DB::DBUtil::CT_Double:
	case DB::DBUtil::CT_Float:
	case DB::DBUtil::CT_Decimal:
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
	case DB::DBUtil::CT_DateTimeTZ:
	case DB::DBUtil::CT_DateTime:
	case DB::DBUtil::CT_Date:
		return ((Data::Timestamp*)this->colDatas[colIndex].colData)->ToString(buff);
	case DB::DBUtil::CT_Binary:
		return 0;
	case DB::DBUtil::CT_Vector:
		{
			NotNullPtr<Math::Geometry::Vector2D> vec;
			if (vec.Set(this->GetVector(colIndex)))
			{
				Text::StringBuilderUTF8 sb;
				Math::WKTWriter wkt;
				wkt.ToText(sb, vec);
				vec.Delete();
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

Data::Timestamp DB::ODBCReader::GetTimestamp(UOSInt colIndex)
{
	if (colIndex >= this->colCnt)
		return Data::Timestamp(0);
	if (this->colDatas[colIndex].isNull)
		return Data::Timestamp(0);
	switch (this->colDatas[colIndex].colType)
	{
	case DB::DBUtil::CT_UTF8Char:
	case DB::DBUtil::CT_UTF16Char:
	case DB::DBUtil::CT_UTF32Char:
	case DB::DBUtil::CT_VarUTF8Char:
	case DB::DBUtil::CT_VarUTF16Char:
	case DB::DBUtil::CT_VarUTF32Char:
	case DB::DBUtil::CT_UUID:
		{
			Text::StringBuilderUTF8 *sb = (Text::StringBuilderUTF8*)this->colDatas[colIndex].colData;
			return Data::Timestamp(sb->ToCString(), this->tzQhr);
		}
	case DB::DBUtil::CT_Double:
	case DB::DBUtil::CT_Float:
	case DB::DBUtil::CT_Decimal:
		return Data::Timestamp(0);
	case DB::DBUtil::CT_UInt16:
	case DB::DBUtil::CT_Int16:
	case DB::DBUtil::CT_UInt32:
	case DB::DBUtil::CT_Int32:
	case DB::DBUtil::CT_UInt64:
	case DB::DBUtil::CT_Int64:
	case DB::DBUtil::CT_Byte:
	case DB::DBUtil::CT_Bool:
		return Data::Timestamp(0);
	case DB::DBUtil::CT_Date:
	case DB::DBUtil::CT_DateTime:
	case DB::DBUtil::CT_DateTimeTZ:
		return *(Data::Timestamp*)this->colDatas[colIndex].colData;
	case DB::DBUtil::CT_Vector:
	case DB::DBUtil::CT_Binary:
		return Data::Timestamp(0);
	case DB::DBUtil::CT_Unknown:
	default:
		return Data::Timestamp(0);
	}
	return Data::Timestamp(0);
}

Double DB::ODBCReader::GetDbl(UOSInt colIndex)
{
	if (colIndex >= this->colCnt)
		return 0;
	if (this->colDatas[colIndex].isNull)
		return 0;
	switch (this->colDatas[colIndex].colType)
	{
	case DB::DBUtil::CT_UTF8Char:
	case DB::DBUtil::CT_UTF16Char:
	case DB::DBUtil::CT_UTF32Char:
	case DB::DBUtil::CT_VarUTF8Char:
	case DB::DBUtil::CT_VarUTF16Char:
	case DB::DBUtil::CT_VarUTF32Char:
	case DB::DBUtil::CT_UUID:
		return Text::StrToDouble(((Text::StringBuilderUTF8*)this->colDatas[colIndex].colData)->ToString());
	case DB::DBUtil::CT_Double:
	case DB::DBUtil::CT_Float:
	case DB::DBUtil::CT_Decimal:
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
	case DB::DBUtil::CT_DateTimeTZ:
	case DB::DBUtil::CT_DateTime:
	case DB::DBUtil::CT_Date:
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
	case DB::DBUtil::CT_UTF8Char:
	case DB::DBUtil::CT_UTF16Char:
	case DB::DBUtil::CT_UTF32Char:
	case DB::DBUtil::CT_VarUTF8Char:
	case DB::DBUtil::CT_VarUTF16Char:
	case DB::DBUtil::CT_VarUTF32Char:
	case DB::DBUtil::CT_UUID:
		return Text::StrToInt32(((Text::StringBuilderUTF8*)this->colDatas[colIndex].colData)->ToString()) != 0;
	case DB::DBUtil::CT_Double:
	case DB::DBUtil::CT_Float:
	case DB::DBUtil::CT_Decimal:
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
	case DB::DBUtil::CT_DateTimeTZ:
	case DB::DBUtil::CT_DateTime:
	case DB::DBUtil::CT_Date:
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
	case DB::DBUtil::CT_UTF8Char:
	case DB::DBUtil::CT_UTF16Char:
	case DB::DBUtil::CT_UTF32Char:
	case DB::DBUtil::CT_VarUTF8Char:
	case DB::DBUtil::CT_VarUTF16Char:
	case DB::DBUtil::CT_VarUTF32Char:
	case DB::DBUtil::CT_UUID:
		return 0;
	case DB::DBUtil::CT_Double:
	case DB::DBUtil::CT_Float:
	case DB::DBUtil::CT_Decimal:
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
	case DB::DBUtil::CT_DateTimeTZ:
	case DB::DBUtil::CT_DateTime:
	case DB::DBUtil::CT_Date:
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
	case DB::DBUtil::CT_UTF8Char:
	case DB::DBUtil::CT_UTF16Char:
	case DB::DBUtil::CT_UTF32Char:
	case DB::DBUtil::CT_VarUTF8Char:
	case DB::DBUtil::CT_VarUTF16Char:
	case DB::DBUtil::CT_VarUTF32Char:
	case DB::DBUtil::CT_UUID:
		return 0;
	case DB::DBUtil::CT_Double:
	case DB::DBUtil::CT_Float:
	case DB::DBUtil::CT_Decimal:
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
	case DB::DBUtil::CT_DateTimeTZ:
	case DB::DBUtil::CT_DateTime:
	case DB::DBUtil::CT_Date:
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

Math::Geometry::Vector2D *DB::ODBCReader::GetVector(UOSInt colIndex)
{
	if (colIndex >= this->colCnt)
		return 0;
	if (this->colDatas[colIndex].isNull)
		return 0;
	if (this->conn->GetSQLType() == DB::SQLType::MSSQL)
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

Bool DB::ODBCReader::GetUUID(UOSInt colIndex, NotNullPtr<Data::UUID> uuid)
{
	return false;
}

Bool DB::ODBCReader::GetVariItem(UOSInt colIndex, NotNullPtr<Data::VariItem> item)
{
	if (colIndex >= this->colCnt)
		return false;
	if (this->colDatas[colIndex].isNull)
	{
		item->SetNull();
		return true;
	}
	Text::StringBuilderUTF8 *sb;
	switch (this->colDatas[colIndex].colType)
	{
	case DB::DBUtil::CT_UTF8Char:
	case DB::DBUtil::CT_UTF16Char:
	case DB::DBUtil::CT_UTF32Char:
	case DB::DBUtil::CT_VarUTF8Char:
	case DB::DBUtil::CT_VarUTF16Char:
	case DB::DBUtil::CT_VarUTF32Char:
	case DB::DBUtil::CT_UUID:
		sb = (Text::StringBuilderUTF8*)this->colDatas[colIndex].colData;
		item->SetStr(sb->v, sb->leng);
		return true;
	case DB::DBUtil::CT_Double:
	case DB::DBUtil::CT_Decimal:
		item->SetF64(*(Double*)&this->colDatas[colIndex].dataVal);
		return true;
	case DB::DBUtil::CT_Float:
		item->SetF32((Single)*(Double*)&this->colDatas[colIndex].dataVal);
		return true;
	case DB::DBUtil::CT_Int16:
		item->SetI16((Int16)this->colDatas[colIndex].dataVal);
		return true;
	case DB::DBUtil::CT_UInt16:
		item->SetU16((UInt16)this->colDatas[colIndex].dataVal);
		return true;
	case DB::DBUtil::CT_Int32:
		item->SetI32((Int32)this->colDatas[colIndex].dataVal);
		return true;
	case DB::DBUtil::CT_UInt32:
		item->SetU32((UInt32)this->colDatas[colIndex].dataVal);
		return true;
	case DB::DBUtil::CT_Byte:
		item->SetU8((UInt8)this->colDatas[colIndex].dataVal);
		return true;
	case DB::DBUtil::CT_Int64:
		item->SetI64(this->colDatas[colIndex].dataVal);
		return true;
	case DB::DBUtil::CT_UInt64:
		item->SetU64((UInt64)this->colDatas[colIndex].dataVal);
		return true;
	case DB::DBUtil::CT_Bool:
		item->SetBool(this->colDatas[colIndex].dataVal != 0);
		return true;
	case DB::DBUtil::CT_DateTimeTZ:
	case DB::DBUtil::CT_DateTime:
	case DB::DBUtil::CT_Date:
		item->SetDate(*(Data::Timestamp*)this->colDatas[colIndex].colData);
		return true;
	case DB::DBUtil::CT_Vector:
		if (this->conn->GetSQLType() == DB::SQLType::MSSQL)
		{
			UOSInt dataSize = (UOSInt)this->colDatas[colIndex].dataVal;
			UInt8 *buffPtr = (UInt8*)this->colDatas[colIndex].colData;
			UInt32 srId;
			NotNullPtr<Math::Geometry::Vector2D> vec;
			if (vec.Set(Math::MSGeography::ParseBinary(buffPtr, dataSize, &srId)))
			{
				item->SetVectorDirect(vec);
				return true;
			}
			item->SetNull();
			return false;
		}
		return false;
	case DB::DBUtil::CT_Binary:
		item->SetByteArr((UInt8*)this->colDatas[colIndex].colData, (UOSInt)this->colDatas[colIndex].dataVal);
		return true;
	case DB::DBUtil::CT_Unknown:
	default:
		return false;
	}
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
	return Text::StrUTF16_UTF8(buff, (const UTF16Char*)sbuff);
}

Bool DB::ODBCReader::IsNull(UOSInt colIndex)
{
	if (colIndex >= this->colCnt)
		return true;
	return this->colDatas[colIndex].isNull;
}

DB::DBUtil::ColType DB::ODBCReader::GetColType(UOSInt colIndex, OptOut<UOSInt> colSize)
{
	if (colIndex >= this->colCnt)
		return DB::DBUtil::CT_Unknown;
	colSize.Set(this->colDatas[colIndex].colSize);
	return this->colDatas[colIndex].colType;
}

Bool DB::ODBCReader::GetColDef(UOSInt colIndex, NotNullPtr<DB::ColDef> colDef)
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

	NotNullPtr<Text::String> s = Text::String::NewNotNull((const UTF16Char*)sbuff);
	colDef->SetColName(s);
	s->Release();
	colDef->SetColType(this->ODBCType2DBType(dataType, colSize));
	colDef->SetColSize(colSize);
	colDef->SetColDP((UInt16)decimalDigit);
	colDef->SetNotNull(notNull == SQL_NO_NULLS);
	colDef->SetPK(false);
	colDef->SetAutoIncNone();
	colDef->SetDefVal(CSTR_NULL);
	colDef->SetAttr(CSTR_NULL);
	return true;
}

DB::DBUtil::ColType DB::ODBCReader::ODBCType2DBType(Int16 odbcType, UOSInt colSize)
{
	switch (odbcType)
	{
	case SQL_CHAR:
		return DB::DBUtil::CT_UTF8Char;
	case SQL_VARCHAR:
		return DB::DBUtil::CT_VarUTF8Char;
	case SQL_LONGVARCHAR:
		return DB::DBUtil::CT_VarUTF8Char;
	case SQL_WCHAR:
		return DB::DBUtil::CT_UTF16Char;
	case SQL_WVARCHAR:
		return DB::DBUtil::CT_VarUTF16Char;
	case SQL_WLONGVARCHAR:
		return DB::DBUtil::CT_VarUTF16Char;
	case SQL_DECIMAL:
		return DB::DBUtil::CT_Double;
	case SQL_NUMERIC:
		return DB::DBUtil::CT_Double;
	case SQL_SMALLINT:
		return DB::DBUtil::CT_Int16;
	case SQL_INTEGER:
		if (this->conn->GetSQLType() == DB::SQLType::MySQL)// && colSize == 10)
		{
			return DB::DBUtil::CT_Int32;
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
		return DB::DBUtil::CT_VarUTF8Char;
	}
}
#endif
