#include "Stdafx.h"
#include "Crypto/Encrypt/AES256.h"
#include "Crypto/Hash/MD5.h"
#include "Data/ByteBuffer.h"
#include "DB/DBManager.h"
#include "DB/MySQLConn.h"
#include "DB/ODBCConn.h"
#include "DB/OLEDBConn.h"
#include "DB/PostgreSQLConn.h"
#include "DB/SQLiteFile.h"
#include "DB/TDSConn.h"
#include "IO/FileStream.h"
#include "IO/MemoryStream.h"
#include "IO/Path.h"
#include "IO/StmData/FileData.h"
#include "Net/MySQLTCPClient.h"
#include "Text/MyStringW.h"
#include "Text/UTF8Reader.h"
#include "Text/UTF8Writer.h"
#include "Win32/WMIQuery.h"

#define DBPREFIX CSTR("DB:")
#define ENCKEY "sswr"
#define ENCKEYLEN (sizeof(ENCKEY) - 1)

Bool DB::DBManager::GetConnStr(NN<DB::DBTool> db, NN<Text::StringBuilderUTF8> connStr)
{
	NN<DB::DBConn> conn = db->GetDBConn();
	NN<Text::String> s;
	NN<Text::String> nns;
	switch (conn->GetConnType())
	{
	case DB::DBConn::CT_ODBC:
		{
			NN<DB::ODBCConn> odbc = NN<DB::ODBCConn>::ConvertFrom(conn);
			if (s.Set(odbc->GetConnStr()))
			{
				connStr->AppendC(UTF8STRC("odbc:"));
				connStr->Append(s);
				return true;
			}
			else if (odbc->GetConnDSN().SetTo(s))
			{
				connStr->AppendC(UTF8STRC("odbc:DSN="));
				connStr->Append(s);
				if (odbc->GetConnUID().SetTo(s))
				{
					connStr->AppendC(UTF8STRC(";UID="));
					connStr->Append(s);
				}
				if (odbc->GetConnPWD().SetTo(s))
				{
					connStr->AppendC(UTF8STRC(";PWD="));
					connStr->Append(s);
				}
				if (odbc->GetConnSchema().SetTo(s))
				{
					connStr->AppendC(UTF8STRC(";Schema="));
					connStr->Append(s);
				}
				return true;
			}
			return false;
		}
		break;
	case DB::DBConn::CT_MYSQL:
		{
			NN<DB::MySQLConn> mysql = NN<DB::MySQLConn>::ConvertFrom(conn);
			connStr->AppendC(UTF8STRC("mysql:Server="));
			connStr->Append(mysql->GetConnServer());
			if (mysql->GetConnDB().SetTo(s))
			{
				connStr->AppendC(UTF8STRC(";Database="));
				connStr->Append(s);
			}
			if (mysql->GetConnUID().SetTo(s))
			{
				connStr->AppendC(UTF8STRC(";UID="));
				connStr->Append(s);
			}
			if (mysql->GetConnPWD().SetTo(s))
			{
				connStr->AppendC(UTF8STRC(";PWD="));
				connStr->Append(s);
			}
			return true;
		}
		break;
	case DB::DBConn::CT_SQLITE:
		{
			NN<DB::SQLiteFile> sqlite = NN<DB::SQLiteFile>::ConvertFrom(conn);
			connStr->AppendC(UTF8STRC("sqlite:File="));
			connStr->Append(sqlite->GetFileName());
			return true;
		}
		break;
	case DB::DBConn::CT_WMIQUERY:
		{
			NN<Win32::WMIQuery> wmi = NN<Win32::WMIQuery>::ConvertFrom(conn);
			connStr->AppendC(UTF8STRC("wmi:ns="));
			const WChar *ns = wmi->GetNS();
			nns = Text::String::NewNotNull(ns);
			connStr->Append(nns);
			nns->Release();
			return true;
		}
		break;
	case DB::DBConn::CT_OLEDB:
		{
			NN<DB::OLEDBConn> oledb = NN<DB::OLEDBConn>::ConvertFrom(conn);
			connStr->AppendC(UTF8STRC("oledb:"));
			const WChar *cStr = oledb->GetConnStr();
			nns = Text::String::NewNotNull(cStr);
			connStr->Append(nns);
			nns->Release();
			return true;
		}
		break;
	case DB::DBConn::CT_MYSQLTCP:
		{
			UTF8Char sbuff[128];
			UTF8Char *sptr;
			NN<Net::MySQLTCPClient> mysql = NN<Net::MySQLTCPClient>::ConvertFrom(conn);
			connStr->AppendC(UTF8STRC("mysqltcp:Server="));
			sptr = Net::SocketUtil::GetAddrName(sbuff, mysql->GetConnAddr());
			connStr->AppendP(sbuff, sptr);
			connStr->AppendC(UTF8STRC(";Port="));
			connStr->AppendU16(mysql->GetConnPort());
			if (mysql->GetConnDB().SetTo(s))
			{
				connStr->AppendC(UTF8STRC(";Database="));
				connStr->Append(s);
			}
			connStr->AppendC(UTF8STRC(";UID="));
			connStr->Append(mysql->GetConnUID());

			connStr->AppendC(UTF8STRC(";PWD="));
			connStr->Append(mysql->GetConnPWD());
			return true;
		}
		break;
	case DB::DBConn::CT_POSTGRESQL:
		{
			UTF8Char sbuff[128];
			UTF8Char *sptr;
			NN<DB::PostgreSQLConn> psql = NN<DB::PostgreSQLConn>::ConvertFrom(conn);
			connStr->AppendC(UTF8STRC("postgresql:Server="));
			sptr = psql->GetConnServer()->ConcatTo(sbuff);
			connStr->AppendP(sbuff, sptr);
			connStr->AppendC(UTF8STRC(";Port="));
			connStr->AppendU16(psql->GetConnPort());

			connStr->AppendC(UTF8STRC(";Database="));
			connStr->Append(psql->GetConnDB());

			connStr->AppendC(UTF8STRC(";UID="));
			connStr->AppendOpt(psql->GetConnUID());

			connStr->AppendC(UTF8STRC(";PWD="));
			connStr->AppendOpt(psql->GetConnPWD());
			return true;
		}
		break;
	case DB::DBConn::CT_TDSCONN:
		{
			UTF8Char sbuff[128];
			UTF8Char *sptr;
			NN<DB::TDSConn> tds = NN<DB::TDSConn>::ConvertFrom(conn);
			connStr->AppendC(UTF8STRC("tds:Host="));
			sptr = tds->GetConnHost()->ConcatTo(sbuff);
			connStr->AppendP(sbuff, sptr);
			if (tds->GetConnDB().SetTo(s))
			{
				connStr->AppendC(UTF8STRC(";Database="));
				connStr->Append(s);
			}
			connStr->AppendC(UTF8STRC(";UID="));
			connStr->Append(tds->GetConnUID());

			connStr->AppendC(UTF8STRC(";PWD="));
			connStr->Append(tds->GetConnPWD());
			return true;
		}
		break;
	case DB::DBConn::CT_UNKNOWN:
		break;
	}
	return false;
}

Optional<DB::ReadingDB> DB::DBManager::OpenConn(NN<Text::String> connStr, NN<IO::LogTool> log, NN<Net::SocketFactory> sockf, Optional<Parser::ParserList> parsers)
{
	return OpenConn(connStr->ToCString(), log, sockf, parsers);
}

Optional<DB::ReadingDB> DB::DBManager::OpenConn(Text::CStringNN connStr, NN<IO::LogTool> log, NN<Net::SocketFactory> sockf, Optional<Parser::ParserList> parsers)
{
	Optional<DB::DBTool> db;
	NN<DB::DBTool> nndb;
	if (connStr.StartsWith(UTF8STRC("odbc:")))
	{
		if (connStr.Substring(5).StartsWith(UTF8STRC("DSN=")))
		{
			Text::StringBuilderUTF8 sb;
			Text::String *dsn = 0;
			Text::String *uid = 0;
			Text::String *pwd = 0;
			Text::String *schema = 0;
			UOSInt cnt;
			sb.Append(connStr.Substring(5));
			Text::PString sarr[2];
			sarr[1] = sb;
			while (true)
			{
				cnt = Text::StrSplitP(sarr, 2, sarr[1], ';');
				if (sarr[0].StartsWithICase(UTF8STRC("DSN=")))
				{
					SDEL_STRING(dsn);
					dsn = Text::String::New(sarr[0].v + 4, sarr[0].leng - 4).Ptr();
				}
				else if (sarr[0].StartsWithICase(UTF8STRC("UID=")))
				{
					SDEL_STRING(uid);
					uid = Text::String::New(sarr[0].v + 4, sarr[0].leng - 4).Ptr();
				}
				else if (sarr[0].StartsWithICase(UTF8STRC("PWD=")))
				{
					SDEL_STRING(pwd);
					pwd = Text::String::New(sarr[0].v + 4, sarr[0].leng - 4).Ptr();
				}
				else if (sarr[0].StartsWithICase(UTF8STRC("SCHEMA=")))
				{
					SDEL_STRING(schema);
					schema = Text::String::New(sarr[0].v + 7, sarr[0].leng - 7).Ptr();
				}
				if (cnt != 2)
				{
					break;
				}
			}
			NN<Text::String> s;
			db = 0;
			if (s.Set(dsn))
			{
				db = DB::ODBCConn::CreateDBTool(s, uid, pwd, schema, log, DBPREFIX).OrNull();
			}
			SDEL_STRING(dsn);
			SDEL_STRING(uid);
			SDEL_STRING(pwd);
			SDEL_STRING(schema);
			if (db.NotNull())
			{
				return db;
			}
		}
		else
		{
			NN<DB::ODBCConn> conn;
			NEW_CLASSNN(conn, DB::ODBCConn(connStr.Substring(5), CSTR("ODBCConn"), log));
			if (conn->GetConnError() == DB::ODBCConn::CE_NONE)
			{
				NEW_CLASSNN(nndb, DB::DBTool(conn, true, log, DBPREFIX));
				return nndb;
			}
			conn.Delete();
		}
	}
	else if (connStr.StartsWith(UTF8STRC("mysql:")))
	{
		Text::StringBuilderUTF8 sb;
		NN<Text::String> nnserver;
		Text::String *server = 0;
		Text::String *uid = 0;
		Text::String *pwd = 0;
		Text::String *schema = 0;
		UOSInt cnt;
		sb.Append(connStr.Substring(6));
		Text::PString sarr[2];
		sarr[1] = sb;
		while (true)
		{
			cnt = Text::StrSplitP(sarr, 2, sarr[1], ';');
			if (sarr[0].StartsWithICase(UTF8STRC("SERVER=")))
			{
				SDEL_STRING(server);
				server = Text::String::New(sarr[0].v + 7, sarr[0].leng - 7).Ptr();
			}
			else if (sarr[0].StartsWithICase(UTF8STRC("UID=")))
			{
				SDEL_STRING(uid);
				uid = Text::String::New(sarr[0].v + 4, sarr[0].leng - 4).Ptr();
			}
			else if (sarr[0].StartsWithICase(UTF8STRC("PWD=")))
			{
				SDEL_STRING(pwd);
				pwd = Text::String::New(sarr[0].v + 4, sarr[0].leng - 4).Ptr();
			}
			else if (sarr[0].StartsWithICase(UTF8STRC("DATABASE=")))
			{
				SDEL_STRING(schema);
				schema = Text::String::New(sarr[0].v + 9, sarr[0].leng - 9).Ptr();
			}
			if (cnt != 2)
			{
				break;
			}
		}
		if (nnserver.Set(server))
		{
			db = DB::MySQLConn::CreateDBTool(sockf, nnserver, schema, uid, pwd, log, DBPREFIX);
		}
		else
		{
			db = 0;
		}
		
		SDEL_STRING(server);
		SDEL_STRING(uid);
		SDEL_STRING(pwd);
		SDEL_STRING(schema);
		if (db.NotNull())
		{
			return db;
		}
	}
	else if (connStr.StartsWith(UTF8STRC("sqlite:")))
	{
		if (connStr.Substring(7).StartsWithICase(UTF8STRC("FILE=")))
		{
			if (DB::SQLiteFile::CreateDBTool(connStr.Substring(12), log, DBPREFIX).SetTo(nndb))
			{
				return nndb;
			}
		}
	}
	else if (connStr.StartsWith(UTF8STRC("wmi:")))
	{
		if (connStr.Substring(4).StartsWithICase(UTF8STRC("NS=")))
		{
			const WChar *ns = Text::StrToWCharNew(connStr.v + 7);
			NN<Win32::WMIQuery> wmi;
			NEW_CLASSNN(wmi, Win32::WMIQuery(ns));
			if (wmi->IsError())
			{
				wmi.Delete();
			}
			else
			{
				NEW_CLASSNN(nndb, DB::DBTool(wmi, true, log, DBPREFIX));
				return nndb;
			}
		}
	}
	else if (connStr.StartsWith(UTF8STRC("oledb:")))
	{
		const WChar *cstr = Text::StrToWCharNew(connStr.v + 6);
		NN<DB::OLEDBConn> oledb;
		NEW_CLASSNN(oledb, DB::OLEDBConn(cstr, log));
		if (oledb->GetConnError() != DB::OLEDBConn::CE_NONE)
		{
			oledb.Delete();
		}
		else
		{
			NEW_CLASSNN(nndb, DB::DBTool(oledb, true, log, DBPREFIX));
			return nndb;
		}
	}
	else if (connStr.StartsWith(UTF8STRC("mysqltcp:")))
	{
		Text::StringBuilderUTF8 sb;
		Net::SocketUtil::AddressInfo addr;
		UInt16 port = 0;
		Text::String *uid = 0;
		Text::String *pwd = 0;
		Text::String *schema = 0;
		addr.addrType = Net::AddrType::Unknown;
		UOSInt cnt;
		sb.Append(connStr.Substring(9));
		Text::PString sarr[2];
		sarr[1] = sb;
		while (true)
		{
			cnt = Text::StrSplitP(sarr, 2, sarr[1], ';');
			if (sarr[0].StartsWithICase(UTF8STRC("SERVER=")))
			{
				Net::SocketUtil::SetAddrInfo(addr, sarr[0].ToCString().Substring(7));
			}
			else if (sarr[0].StartsWithICase(UTF8STRC("PORT=")))
			{
				Text::StrToUInt16(sarr[0].v + 5, port);
			}
			else if (sarr[0].StartsWithICase(UTF8STRC("UID=")))
			{
				SDEL_STRING(uid);
				uid = Text::String::New(sarr[0].v + 4, sarr[0].leng - 4).Ptr();
			}
			else if (sarr[0].StartsWithICase(UTF8STRC("PWD=")))
			{
				SDEL_STRING(pwd);
				pwd = Text::String::New(sarr[0].v + 4, sarr[0].leng - 4).Ptr();
			}
			else if (sarr[0].StartsWithICase(UTF8STRC("DATABASE=")))
			{
				SDEL_STRING(schema);
				schema = Text::String::New(sarr[0].v + 9, sarr[0].leng - 9).Ptr();
			}
			if (cnt != 2)
			{
				break;
			}
		}
		Net::MySQLTCPClient *cli = 0;
		NN<Net::MySQLTCPClient> nncli;
		NN<Text::String> uidStr;
		NN<Text::String> pwdStr;
		if (uidStr.Set(uid) && pwdStr.Set(pwd))
		{
			NEW_CLASS(cli, Net::MySQLTCPClient(sockf, addr, port, uidStr, pwdStr, schema));
		}
		SDEL_STRING(uid);
		SDEL_STRING(pwd);
		SDEL_STRING(schema);
		if (!nncli.Set(cli))
		{

		}
		else if (nncli->IsError())
		{
			nncli.Delete();
		}
		else
		{
			NEW_CLASSNN(nndb, DB::DBTool(nncli, true, log, DBPREFIX));
			return nndb;
		}
	}
	else if (connStr.StartsWith(UTF8STRC("postgresql:")))
	{
		Text::StringBuilderUTF8 sb;
		Text::String *server = 0;
		UInt16 port = 0;
		Text::String *uid = 0;
		Text::String *pwd = 0;
		Text::String *schema = 0;
		UOSInt cnt;
		sb.Append(connStr.Substring(11));
		Text::PString sarr[2];
		sarr[1] = sb;
		while (true)
		{
			cnt = Text::StrSplitP(sarr, 2, sarr[1], ';');
			if (sarr[0].StartsWithICase(UTF8STRC("SERVER=")))
			{
				SDEL_STRING(server);
				server = Text::String::New(sarr[0].v + 7, sarr[0].leng - 7).Ptr();
			}
			else if (sarr[0].StartsWithICase(UTF8STRC("PORT=")))
			{
				Text::StrToUInt16(sarr[0].v + 5, port);
			}
			else if (sarr[0].StartsWithICase(UTF8STRC("UID=")))
			{
				SDEL_STRING(uid);
				uid = Text::String::New(sarr[0].v + 4, sarr[0].leng - 4).Ptr();
			}
			else if (sarr[0].StartsWithICase(UTF8STRC("PWD=")))
			{
				SDEL_STRING(pwd);
				pwd = Text::String::New(sarr[0].v + 4, sarr[0].leng - 4).Ptr();
			}
			else if (sarr[0].StartsWithICase(UTF8STRC("DATABASE=")))
			{
				SDEL_STRING(schema);
				schema = Text::String::New(sarr[0].v + 9, sarr[0].leng - 9).Ptr();
			}
			if (cnt != 2)
			{
				break;
			}
		}
		DB::PostgreSQLConn *cli;
		NN<DB::PostgreSQLConn> nncli;
		NN<Text::String> serverStr;
		NN<Text::String> schemaStr;
		if (serverStr.Set(server) && schemaStr.Set(schema))
		{
			NEW_CLASS(cli, DB::PostgreSQLConn(serverStr, port, uid, pwd, schemaStr, log));
		}
		else
		{
			cli = 0;
		}
		SDEL_STRING(server);
		SDEL_STRING(uid);
		SDEL_STRING(pwd);
		SDEL_STRING(schema);
		if (!nncli.Set(cli))
		{

		}
		else if (nncli->IsConnError())
		{
			nncli.Delete();
		}
		else
		{
			NEW_CLASSNN(nndb, DB::DBTool(nncli, true, log, DBPREFIX));
			return nndb;
		}
	}
	else if (connStr.StartsWith(UTF8STRC("file:")))
	{
		NN<Parser::ParserList> nnparsers;
		if (parsers.SetTo(nnparsers))
		{
			IO::StmData::FileData fd(connStr.Substring(5), false);
			NN<DB::ReadingDB> rdb;
			if (Optional<DB::ReadingDB>::ConvertFrom(nnparsers->ParseFileType(fd, IO::ParserType::ReadingDB)).SetTo(rdb))
			{
				return rdb;
			}
		}
	}
	else if (connStr.StartsWith(UTF8STRC("tds:")))
	{
		Text::StringBuilderUTF8 sb;
		Text::String *server = 0;
		Text::String *uid = 0;
		Text::String *pwd = 0;
		Text::String *schema = 0;
		UOSInt cnt;
		sb.Append(connStr.Substring(4));
		Text::PString sarr[2];
		sarr[1] = sb;
		while (true)
		{
			cnt = Text::StrSplitP(sarr, 2, sarr[1], ';');
			if (sarr[0].StartsWithICase(UTF8STRC("HOST=")))
			{
				SDEL_STRING(server);
				server = Text::String::New(sarr[0].v + 5, sarr[0].leng - 5).Ptr();
			}
			else if (sarr[0].StartsWithICase(UTF8STRC("UID=")))
			{
				SDEL_STRING(uid);
				uid = Text::String::New(sarr[0].v + 4, sarr[0].leng - 4).Ptr();
			}
			else if (sarr[0].StartsWithICase(UTF8STRC("PWD=")))
			{
				SDEL_STRING(pwd);
				pwd = Text::String::New(sarr[0].v + 4, sarr[0].leng - 4).Ptr();
			}
			else if (sarr[0].StartsWithICase(UTF8STRC("DATABASE=")))
			{
				SDEL_STRING(schema);
				schema = Text::String::New(sarr[0].v + 9, sarr[0].leng - 9).Ptr();
			}
			if (cnt != 2)
			{
				break;
			}
		}
		DB::TDSConn *cli = 0;
		NN<DB::TDSConn> nncli;
		UInt16 port = 1433;
		if (server)
		{
			UOSInt i = server->IndexOf(':');
			if (i >= 0)
			{
				Text::StrToUInt16(&server->v[i + 1], port);
				server->TrimToLength(i);
			}
			NEW_CLASS(cli, DB::TDSConn(server->ToCString(), port, false, STR_CSTR(schema), STR_CSTR(uid), STR_CSTR(pwd), log, 0));
			if (!cli->IsConnected())
			{
				DEL_CLASS(cli);
				cli = 0;
			}
		}
		SDEL_STRING(server);
		SDEL_STRING(uid);
		SDEL_STRING(pwd);
		SDEL_STRING(schema);
		if (nncli.Set(cli))
		{
			NEW_CLASSNN(nndb, DB::DBTool(nncli, true, log, DBPREFIX));
			return nndb;
		}
	}
	return 0;
}

void DB::DBManager::GetConnName(Text::CString connStr, NN<Text::StringBuilderUTF8> sbOut)
{
	if (connStr.StartsWith(UTF8STRC("odbc:")))
	{
		sbOut->AppendC(UTF8STRC("odbc:"));
		if (connStr.Substring(5).StartsWith(UTF8STRC("DSN=")))
		{
			Text::StringBuilderUTF8 sb;
			sb.Append(connStr.Substring(5));
			UOSInt cnt;
			Text::PString sarr[2];
			sarr[1] = sb;
			while (true)
			{
				cnt = Text::StrSplitP(sarr, 2, sarr[1], ';');
				if (sarr[0].StartsWithICase(UTF8STRC("DSN=")))
				{
					sbOut->Append(sarr[0].ToCString().Substring(4));
				}
				else if (sarr[0].StartsWithICase(UTF8STRC("SCHEMA=")))
				{
					sbOut->AppendC(UTF8STRC(", "));
					sbOut->Append(sarr[0].ToCString().Substring(7));
				}
				if (cnt != 2)
				{
					break;
				}
			}
		}
		else
		{
			sbOut->Append(connStr.Substring(5));
		}
	}
	else if (connStr.StartsWith(UTF8STRC("mysql:")))
	{
		sbOut->AppendC(UTF8STRC("mysql:"));
		Text::StringBuilderUTF8 sb;
		UOSInt cnt;
		sb.Append(connStr.Substring(6));
		Text::PString sarr[2];
		sarr[1] = sb;
		while (true)
		{
			cnt = Text::StrSplitP(sarr, 2, sarr[1], ';');
			if (sarr[0].StartsWithICase(UTF8STRC("SERVER=")))
			{
				sbOut->Append(sarr[0].ToCString().Substring(7));
			}
			else if (sarr[0].StartsWithICase(UTF8STRC("DATABASE=")))
			{
				sbOut->AppendC(UTF8STRC(", "));
				sbOut->Append(sarr[0].ToCString().Substring(9));
			}
			if (cnt != 2)
			{
				break;
			}
		}
	}
	else if (connStr.StartsWith(UTF8STRC("sqlite:")))
	{
		sbOut->AppendC(UTF8STRC("sqlite:"));
		if (connStr.Substring(7).StartsWithICase(UTF8STRC("FILE=")))
		{
			sbOut->Append(connStr.Substring(12));
		}
	}
	else if (connStr.StartsWith(UTF8STRC("wmi:")))
	{
		sbOut->AppendC(UTF8STRC("wmi:"));
		if (connStr.Substring(4).StartsWithICase(UTF8STRC("NS=")))
		{
			sbOut->Append(connStr.Substring(7));
		}
	}
	else if (connStr.StartsWith(UTF8STRC("oledb:")))
	{
		sbOut->Append(connStr);
	}
	else if (connStr.StartsWith(UTF8STRC("mysqltcp:")))
	{
		sbOut->AppendC(UTF8STRC("mysqltcp:"));
		Text::StringBuilderUTF8 sb;
		UOSInt cnt;
		sb.Append(connStr.Substring(9));
		Text::PString sarr[2];
		sarr[1] = sb;
		while (true)
		{
			cnt = Text::StrSplitP(sarr, 2, sarr[1], ';');
			if (sarr[0].StartsWithICase(UTF8STRC("SERVER=")))
			{
				sbOut->Append(sarr[0].ToCString().Substring(7));
			}
			else if (sarr[0].StartsWithICase(UTF8STRC("PORT=")))
			{
				sbOut->AppendUTF8Char(':');
				sbOut->Append(sarr[0].ToCString().Substring(5));
			}
			else if (sarr[0].StartsWithICase(UTF8STRC("DATABASE=")))
			{
				sbOut->AppendUTF8Char(',');
				sbOut->Append(sarr[0].ToCString().Substring(9));
			}
			if (cnt != 2)
			{
				break;
			}
		}
	}
	else if (connStr.StartsWith(UTF8STRC("postgresql:")))
	{
		sbOut->AppendC(UTF8STRC("pgsql:"));
		Text::StringBuilderUTF8 sb;
		UOSInt cnt;
		sb.Append(connStr.Substring(11));
		Text::PString sarr[2];
		sarr[1] = sb;
		while (true)
		{
			cnt = Text::StrSplitP(sarr, 2, sarr[1], ';');
			if (sarr[0].StartsWithICase(UTF8STRC("SERVER=")))
			{
				sbOut->Append(sarr[0].ToCString().Substring(7));
			}
			else if (sarr[0].StartsWithICase(UTF8STRC("PORT=")))
			{
				sbOut->AppendUTF8Char(':');
				sbOut->Append(sarr[0].ToCString().Substring(5));
			}
			else if (sarr[0].StartsWithICase(UTF8STRC("DATABASE=")))
			{
				sbOut->AppendUTF8Char(',');
				sbOut->Append(sarr[0].ToCString().Substring(9));
			}
			if (cnt != 2)
			{
				break;
			}
		}
	}
	else if (connStr.StartsWith(UTF8STRC("tds:")))
	{
		sbOut->AppendC(UTF8STRC("tds:"));
		Text::StringBuilderUTF8 sb;
		UOSInt cnt;
		sb.Append(connStr.Substring(4));
		Text::PString sarr[2];
		sarr[1] = sb;
		while (true)
		{
			cnt = Text::StrSplitP(sarr, 2, sarr[1], ';');
			if (sarr[0].StartsWithICase(UTF8STRC("HOST=")))
			{
				sbOut->Append(sarr[0].ToCString().Substring(5));
			}
			else if (sarr[0].StartsWithICase(UTF8STRC("DATABASE=")))
			{
				sbOut->AppendUTF8Char(',');
				sbOut->Append(sarr[0].ToCString().Substring(9));
			}
			if (cnt != 2)
			{
				break;
			}
		}
	}
	else if (connStr.StartsWith(UTF8STRC("file:")))
	{
		sbOut->AppendC(UTF8STRC("file:"));
		UOSInt i = connStr.LastIndexOf(IO::Path::PATH_SEPERATOR);
		sbOut->Append(connStr.Substring(i + 1));
	}
}

Bool DB::DBManager::StoreConn(Text::CStringNN fileName, NN<Data::ArrayListNN<DB::DBManagerCtrl>> ctrlList)
{
	IO::FileStream *fs;
	NEW_CLASS(fs, IO::FileStream(fileName, IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal));
	if (fs->IsError())
	{
		DEL_CLASS(fs);
		return false;
	}

	if (ctrlList->GetCount() > 0)
	{
		Text::StringBuilderUTF8 sb;
		NN<DB::DBManagerCtrl> ctrl;
		NN<Text::String> connStr;
		UOSInt i;
		UOSInt j;
		i = 0;
		j = ctrlList->GetCount();
		while (i < j)
		{
			ctrl = ctrlList->GetItemNoCheck(i);
			if (ctrl->GetConnStr().SetTo(connStr))
			{
				sb.Append(connStr);
				sb.AppendC(UTF8STRC("\r\n"));
			}
			i++;
		}
		
		UInt8 keyBuff[32];
		UOSInt outSize;
		Crypto::Hash::MD5 md5;
		md5.Calc((const UInt8*)ENCKEY, ENCKEYLEN);
		md5.GetValue(keyBuff);
		md5.Clear();
		md5.Calc(keyBuff, 16);
		md5.GetValue(&keyBuff[16]);
		Crypto::Encrypt::AES256 aes(keyBuff);
		UInt8 *outBuff = MemAlloc(UInt8, sb.GetCharCnt() + aes.GetEncBlockSize());
		outSize = aes.Encrypt(sb.ToString(), sb.GetCharCnt(), outBuff);
		fs->Write(outBuff, outSize);
		MemFree(outBuff);
	}

	DEL_CLASS(fs);
	return true;
}

Bool DB::DBManager::RestoreConn(Text::CStringNN fileName, NN<Data::ArrayListNN<DB::DBManagerCtrl>> ctrlList, NN<IO::LogTool> log, NN<Net::SocketFactory> sockf, Optional<Parser::ParserList> parsers)
{
	IO::FileStream *fs;
	NEW_CLASS(fs, IO::FileStream(fileName, IO::FileMode::ReadOnly, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal));
	if (fs->IsError())
	{
		DEL_CLASS(fs);
		return false;
	}

	UInt64 len = fs->GetLength();
	if (len > 0 && len < 65536)
	{
		Data::ByteBuffer fileBuff((UOSInt)len);
		UInt8 *decBuff = MemAlloc(UInt8, (UOSInt)len + 1);
		Text::PString sarr[2];
		fs->Read(fileBuff);
		UInt8 keyBuff[32];
		UOSInt cnt;
		Crypto::Hash::MD5 md5;
		md5.Calc((const UInt8*)ENCKEY, ENCKEYLEN);
		md5.GetValue(keyBuff);
		md5.Clear();
		md5.Calc(keyBuff, 16);
		md5.GetValue(&keyBuff[16]);
		Crypto::Encrypt::AES256 aes(keyBuff);
		aes.Decrypt(fileBuff.Ptr().Ptr(), (UOSInt)len, decBuff);
		decBuff[(UOSInt)len] = 0;
		while (len > 0 && decBuff[(UOSInt)len - 1] == 0)
		{
			len--;
		}
		sarr[1] = Text::PString(decBuff, (UOSInt)len);
		while (true)
		{
			cnt = Text::StrSplitLineP(sarr, 2, sarr[1]);
			if (sarr[0].leng > 0)
			{
				ctrlList->Add(DB::DBManagerCtrl::Create(sarr[0].ToCString(), log, sockf, parsers));
			}
			if (cnt != 2)
			{
				break;
			}
		}
		MemFree(decBuff);
	}
	DEL_CLASS(fs);
	return true;
}
