#include "Stdafx.h"
#include "Crypto/Encrypt/AES256.h"
#include "Crypto/Hash/MD5.h"
#include "Data/ByteBuffer.h"
#include "DB/DBManager.h"
#include "DB/MySQLConn.h"
#include "DB/ODBCConn.h"
#include "DB/OLEDBConn.h"
#include "DB/PostgreSQLConn.h"
#include "DB/PostgreSQLTCPConn.h"
#include "DB/SQLiteFile.h"
#include "DB/TDSConn.h"
#include "IO/DirectoryPackage.h"
#include "IO/FileStream.h"
#include "IO/MemoryStream.h"
#include "IO/Path.h"
#include "IO/StmData/FileData.h"
#include "Map/ESRI/FileGDBDir.h"
#include "Net/MySQLTCPClient.h"
#include "Net/SSLEngineFactory.h"
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
	case DB::DBConn::ConnType::ODBC:
		{
			NN<DB::ODBCConn> odbc = NN<DB::ODBCConn>::ConvertFrom(conn);
			if (odbc->GetConnStr().SetTo(s))
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
	case DB::DBConn::ConnType::MySQL:
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
	case DB::DBConn::ConnType::SQLite:
		{
			NN<DB::SQLiteFile> sqlite = NN<DB::SQLiteFile>::ConvertFrom(conn);
			connStr->AppendC(UTF8STRC("sqlite:File="));
			connStr->Append(sqlite->GetFileName());
			return true;
		}
		break;
	case DB::DBConn::ConnType::WMIQuery:
		{
			NN<Win32::WMIQuery> wmi = NN<Win32::WMIQuery>::ConvertFrom(conn);
			connStr->AppendC(UTF8STRC("wmi:ns="));
			UnsafeArray<const WChar> ns = wmi->GetNS();
			nns = Text::String::NewNotNull(ns);
			connStr->Append(nns);
			nns->Release();
			return true;
		}
		break;
	case DB::DBConn::ConnType::OLEDB:
		{
			NN<DB::OLEDBConn> oledb = NN<DB::OLEDBConn>::ConvertFrom(conn);
			connStr->AppendC(UTF8STRC("oledb:"));
			UnsafeArray<const WChar> cStr = oledb->GetConnStr().Or(L"");
			nns = Text::String::NewNotNull(cStr);
			connStr->Append(nns);
			nns->Release();
			return true;
		}
		break;
	case DB::DBConn::ConnType::MySQLTCP:
		{
			UTF8Char sbuff[128];
			UnsafeArray<UTF8Char> sptr;
			NN<Net::MySQLTCPClient> mysql = NN<Net::MySQLTCPClient>::ConvertFrom(conn);
			connStr->AppendC(UTF8STRC("mysqltcp:Server="));
			sptr = Net::SocketUtil::GetAddrName(sbuff, mysql->GetConnAddr()).Or(sbuff);
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
	case DB::DBConn::ConnType::PostgreSQL:
		{
			UTF8Char sbuff[128];
			UnsafeArray<UTF8Char> sptr;
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
	case DB::DBConn::ConnType::PostgreSQLTCP:
		{
			UTF8Char sbuff[128];
			UnsafeArray<UTF8Char> sptr;
			NN<DB::PostgreSQLTCPConn> psql = NN<DB::PostgreSQLTCPConn>::ConvertFrom(conn);
			connStr->AppendC(UTF8STRC("postgresqltcp:Server="));
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
	case DB::DBConn::ConnType::TDSConn:
		{
			UTF8Char sbuff[128];
			UnsafeArray<UTF8Char> sptr;
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
	case DB::DBConn::ConnType::SQLEngine:
		{
			NN<Map::ESRI::FileGDBDir> fgdb = NN<Map::ESRI::FileGDBDir>::ConvertFrom(conn);
			connStr->AppendC(UTF8STRC("sqlengine:"));
			connStr->Append(fgdb->GetSourceNameObj());
			return true;
		}
		break;
	case DB::DBConn::ConnType::Unknown:
		break;
	}
	return false;
}

Optional<DB::ReadingDB> DB::DBManager::OpenConn(NN<Text::String> connStr, NN<IO::LogTool> log, NN<Net::TCPClientFactory> clif, Optional<Net::SSLEngine> ssl, Optional<Parser::ParserList> parsers)
{
	return OpenConn(connStr->ToCString(), log, clif, ssl, parsers);
}

Optional<DB::ReadingDB> DB::DBManager::OpenConn(Text::CStringNN connStr, NN<IO::LogTool> log, NN<Net::TCPClientFactory> clif, Optional<Net::SSLEngine> ssl, Optional<Parser::ParserList> parsers)
{
	Optional<DB::DBTool> db;
	NN<DB::DBTool> nndb;
	if (connStr.StartsWith(UTF8STRC("odbc:")))
	{
		if (connStr.Substring(5).StartsWith(UTF8STRC("DSN=")))
		{
			Text::StringBuilderUTF8 sb;
			Optional<Text::String> dsn = nullptr;
			Optional<Text::String> uid = nullptr;
			Optional<Text::String> pwd = nullptr;
			Optional<Text::String> schema = nullptr;
			UIntOS cnt;
			sb.Append(connStr.Substring(5));
			Text::PString sarr[2];
			sarr[1] = sb;
			while (true)
			{
				cnt = Text::StrSplitP(sarr, 2, sarr[1], ';');
				if (sarr[0].StartsWithICase(UTF8STRC("DSN=")))
				{
					OPTSTR_DEL(dsn);
					dsn = Text::String::New(sarr[0].v + 4, sarr[0].leng - 4);
				}
				else if (sarr[0].StartsWithICase(UTF8STRC("UID=")))
				{
					OPTSTR_DEL(uid);
					uid = Text::String::New(sarr[0].v + 4, sarr[0].leng - 4);
				}
				else if (sarr[0].StartsWithICase(UTF8STRC("PWD=")))
				{
					OPTSTR_DEL(pwd);
					pwd = Text::String::New(sarr[0].v + 4, sarr[0].leng - 4);
				}
				else if (sarr[0].StartsWithICase(UTF8STRC("SCHEMA=")))
				{
					OPTSTR_DEL(schema);
					schema = Text::String::New(sarr[0].v + 7, sarr[0].leng - 7);
				}
				if (cnt != 2)
				{
					break;
				}
			}
			NN<Text::String> s;
			db = nullptr;
			if (dsn.SetTo(s))
			{
				db = DB::ODBCConn::CreateDBTool(s, uid, pwd, schema, log, DBPREFIX).OrNull();
			}
			OPTSTR_DEL(dsn);
			OPTSTR_DEL(uid);
			OPTSTR_DEL(pwd);
			OPTSTR_DEL(schema);
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
		Optional<Text::String> server = nullptr;
		Optional<Text::String> uid = nullptr;
		NN<Text::String> nnuid;
		Optional<Text::String> pwd = nullptr;
		NN<Text::String> nnpwd;
		Optional<Text::String> schema = nullptr;
		UIntOS cnt;
		sb.Append(connStr.Substring(6));
		Text::PString sarr[2];
		sarr[1] = sb;
		while (true)
		{
			cnt = Text::StrSplitP(sarr, 2, sarr[1], ';');
			if (sarr[0].StartsWithICase(UTF8STRC("SERVER=")))
			{
				OPTSTR_DEL(server);
				server = Text::String::New(sarr[0].v + 7, sarr[0].leng - 7);
			}
			else if (sarr[0].StartsWithICase(UTF8STRC("UID=")))
			{
				OPTSTR_DEL(uid);
				uid = Text::String::New(sarr[0].v + 4, sarr[0].leng - 4);
			}
			else if (sarr[0].StartsWithICase(UTF8STRC("PWD=")))
			{
				OPTSTR_DEL(pwd);
				pwd = Text::String::New(sarr[0].v + 4, sarr[0].leng - 4);
			}
			else if (sarr[0].StartsWithICase(UTF8STRC("DATABASE=")))
			{
				OPTSTR_DEL(schema);
				schema = Text::String::New(sarr[0].v + 9, sarr[0].leng - 9);
			}
			if (cnt != 2)
			{
				break;
			}
		}
		if (server.SetTo(nnserver) && uid.SetTo(nnuid) && pwd.SetTo(nnpwd))
		{
			db = DB::MySQLConn::CreateDBTool(clif, nnserver, schema, nnuid, nnpwd, log, DBPREFIX);
		}
		else
		{
			db = nullptr;
		}
		
		OPTSTR_DEL(server);
		OPTSTR_DEL(uid);
		OPTSTR_DEL(pwd);
		OPTSTR_DEL(schema);
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
			UnsafeArray<const WChar> ns = Text::StrToWCharNew(connStr.v + 7);
			NN<Win32::WMIQuery> wmi;
			NEW_CLASSNN(wmi, Win32::WMIQuery(ns));
			Text::StrDelNew(ns);
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
		UnsafeArray<const WChar> cstr = Text::StrToWCharNew(connStr.v + 6);
		NN<DB::OLEDBConn> oledb;
		NEW_CLASSNN(oledb, DB::OLEDBConn(cstr, log));
		Text::StrDelNew(cstr);
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
		Optional<Text::String> uid = nullptr;
		Optional<Text::String> pwd = nullptr;
		Optional<Text::String> schema = nullptr;
		addr.addrType = Net::AddrType::Unknown;
		UIntOS cnt;
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
				OPTSTR_DEL(uid);
				uid = Text::String::New(sarr[0].v + 4, sarr[0].leng - 4);
			}
			else if (sarr[0].StartsWithICase(UTF8STRC("PWD=")))
			{
				OPTSTR_DEL(pwd);
				pwd = Text::String::New(sarr[0].v + 4, sarr[0].leng - 4);
			}
			else if (sarr[0].StartsWithICase(UTF8STRC("DATABASE=")))
			{
				OPTSTR_DEL(schema);
				schema = Text::String::New(sarr[0].v + 9, sarr[0].leng - 9);
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
		if (uid.SetTo(uidStr) && pwd.SetTo(pwdStr))
		{
			NEW_CLASS(cli, Net::MySQLTCPClient(clif, ssl, addr, port, uidStr, pwdStr, schema));
		}
		OPTSTR_DEL(uid);
		OPTSTR_DEL(pwd);
		OPTSTR_DEL(schema);
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
		Optional<Text::String> server = nullptr;
		UInt16 port = 0;
		Optional<Text::String> uid = nullptr;
		Optional<Text::String> pwd = nullptr;
		Optional<Text::String> schema = nullptr;
		UIntOS cnt;
		sb.Append(connStr.Substring(11));
		Text::PString sarr[2];
		sarr[1] = sb;
		while (true)
		{
			cnt = Text::StrSplitP(sarr, 2, sarr[1], ';');
			if (sarr[0].StartsWithICase(UTF8STRC("SERVER=")))
			{
				OPTSTR_DEL(server);
				server = Text::String::New(sarr[0].v + 7, sarr[0].leng - 7);
			}
			else if (sarr[0].StartsWithICase(UTF8STRC("PORT=")))
			{
				Text::StrToUInt16(sarr[0].v + 5, port);
			}
			else if (sarr[0].StartsWithICase(UTF8STRC("UID=")))
			{
				OPTSTR_DEL(uid);
				uid = Text::String::New(sarr[0].v + 4, sarr[0].leng - 4);
			}
			else if (sarr[0].StartsWithICase(UTF8STRC("PWD=")))
			{
				OPTSTR_DEL(pwd);
				pwd = Text::String::New(sarr[0].v + 4, sarr[0].leng - 4);
			}
			else if (sarr[0].StartsWithICase(UTF8STRC("DATABASE=")))
			{
				OPTSTR_DEL(schema);
				schema = Text::String::New(sarr[0].v + 9, sarr[0].leng - 9);
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
		if (server.SetTo(serverStr) && schema.SetTo(schemaStr))
		{
			NEW_CLASS(cli, DB::PostgreSQLConn(serverStr, port, uid, pwd, schemaStr, log));
		}
		else
		{
			cli = 0;
		}
		OPTSTR_DEL(server);
		OPTSTR_DEL(uid);
		OPTSTR_DEL(pwd);
		OPTSTR_DEL(schema);
		if (!nncli.Set(cli))
		{

		}
		else if (nncli->IsConnError())
		{
			nncli.Delete();
		}
		else
		{
			nncli->SetShowViews(true);
			NEW_CLASSNN(nndb, DB::DBTool(nncli, true, log, DBPREFIX));
			return nndb;
		}
	}
	else if (connStr.StartsWith(UTF8STRC("file:")))
	{
		Text::CStringNN fileName = connStr.Substring(5);
		NN<Parser::ParserList> nnparsers;
		if (parsers.SetTo(nnparsers))
		{
			NN<DB::ReadingDB> rdb;
			if (IO::Path::GetPathType(fileName) == IO::Path::PathType::Directory)
			{
				IO::DirectoryPackage dpkg(fileName);
				if (Optional<DB::ReadingDB>::ConvertFrom(nnparsers->ParseObjectType(dpkg, IO::ParserType::ReadingDB)).SetTo(rdb))
				{
					return rdb;
				}
			}
			else
			{
				IO::StmData::FileData fd(connStr.Substring(5), false);
				if (Optional<DB::ReadingDB>::ConvertFrom(nnparsers->ParseFileType(fd, IO::ParserType::ReadingDB)).SetTo(rdb))
				{
					return rdb;
				}
			}
		}
	}
	else if (connStr.StartsWith(UTF8STRC("tds:")))
	{
		Text::StringBuilderUTF8 sb;
		Optional<Text::String> server = nullptr;
		Optional<Text::String> uid = nullptr;
		Optional<Text::String> pwd = nullptr;
		Optional<Text::String> schema = nullptr;
		UIntOS cnt;
		sb.Append(connStr.Substring(4));
		Text::PString sarr[2];
		sarr[1] = sb;
		while (true)
		{
			cnt = Text::StrSplitP(sarr, 2, sarr[1], ';');
			if (sarr[0].StartsWithICase(UTF8STRC("HOST=")))
			{
				OPTSTR_DEL(server);
				server = Text::String::New(sarr[0].v + 5, sarr[0].leng - 5);
			}
			else if (sarr[0].StartsWithICase(UTF8STRC("UID=")))
			{
				OPTSTR_DEL(uid);
				uid = Text::String::New(sarr[0].v + 4, sarr[0].leng - 4);
			}
			else if (sarr[0].StartsWithICase(UTF8STRC("PWD=")))
			{
				OPTSTR_DEL(pwd);
				pwd = Text::String::New(sarr[0].v + 4, sarr[0].leng - 4);
			}
			else if (sarr[0].StartsWithICase(UTF8STRC("DATABASE=")))
			{
				OPTSTR_DEL(schema);
				schema = Text::String::New(sarr[0].v + 9, sarr[0].leng - 9);
			}
			if (cnt != 2)
			{
				break;
			}
		}
		DB::TDSConn *cli = 0;
		NN<DB::TDSConn> nncli;
		UInt16 port = 1433;
		NN<Text::String> serverStr;
		NN<Text::String> uidStr;
		NN<Text::String> pwdStr;
		if (server.SetTo(serverStr) && uid.SetTo(uidStr) && pwd.SetTo(pwdStr))
		{
			UIntOS i = serverStr->IndexOf(':');
			if (i >= 0)
			{
				Text::StrToUInt16(&serverStr->v[i + 1], port);
				serverStr->TrimToLength(i);
			}
			NEW_CLASS(cli, DB::TDSConn(serverStr->ToCString(), port, false, OPTSTR_CSTR(schema), uidStr->ToCString(), pwdStr->ToCString(), log, nullptr));
			if (!cli->IsConnected())
			{
				DEL_CLASS(cli);
				cli = 0;
			}
		}
		OPTSTR_DEL(server);
		OPTSTR_DEL(uid);
		OPTSTR_DEL(pwd);
		OPTSTR_DEL(schema);
		if (nncli.Set(cli))
		{
			NEW_CLASSNN(nndb, DB::DBTool(nncli, true, log, DBPREFIX));
			return nndb;
		}
	}
	return nullptr;
}

void DB::DBManager::GetConnName(Text::CStringNN connStr, NN<Text::StringBuilderUTF8> sbOut)
{
	if (connStr.StartsWith(UTF8STRC("odbc:")))
	{
		sbOut->AppendC(UTF8STRC("odbc:"));
		if (connStr.Substring(5).StartsWith(UTF8STRC("DSN=")))
		{
			Text::StringBuilderUTF8 sb;
			sb.Append(connStr.Substring(5));
			UIntOS cnt;
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
		UIntOS cnt;
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
		UIntOS cnt;
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
		UIntOS cnt;
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
		UIntOS cnt;
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
		UIntOS i = connStr.LastIndexOf(IO::Path::PATH_SEPERATOR);
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
		UIntOS i;
		UIntOS j;
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
		UIntOS outSize;
		Crypto::Hash::MD5 md5;
		md5.Calc((const UInt8*)ENCKEY, ENCKEYLEN);
		md5.GetValue(keyBuff);
		md5.Clear();
		md5.Calc(keyBuff, 16);
		md5.GetValue(&keyBuff[16]);
		Crypto::Encrypt::AES256 aes(keyBuff);
		UInt8 *outBuff = MemAlloc(UInt8, sb.GetCharCnt() + aes.GetEncBlockSize());
		outSize = aes.Encrypt(sb.ToString(), sb.GetCharCnt(), outBuff);
		fs->Write(Data::ByteArrayR(outBuff, outSize));
		MemFree(outBuff);
	}

	DEL_CLASS(fs);
	return true;
}

Bool DB::DBManager::RestoreConn(Text::CStringNN fileName, NN<Data::ArrayListNN<DB::DBManagerCtrl>> ctrlList, NN<IO::LogTool> log, NN<Net::TCPClientFactory> clif, Optional<Net::SSLEngine> ssl, Optional<Parser::ParserList> parsers)
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
		Data::ByteBuffer fileBuff((UIntOS)len);
		UInt8 *decBuff = MemAlloc(UInt8, (UIntOS)len + 1);
		Text::PString sarr[2];
		fs->Read(fileBuff);
		UInt8 keyBuff[32];
		UIntOS cnt;
		Crypto::Hash::MD5 md5;
		md5.Calc((const UInt8*)ENCKEY, ENCKEYLEN);
		md5.GetValue(keyBuff);
		md5.Clear();
		md5.Calc(keyBuff, 16);
		md5.GetValue(&keyBuff[16]);
		Crypto::Encrypt::AES256 aes(keyBuff);
		aes.Decrypt(fileBuff.Arr().Ptr(), (UIntOS)len, decBuff);
		decBuff[(UIntOS)len] = 0;
		while (len > 0 && decBuff[(UIntOS)len - 1] == 0)
		{
			len--;
		}
		sarr[1] = Text::PString(decBuff, (UIntOS)len);
		while (true)
		{
			cnt = Text::StrSplitLineP(sarr, 2, sarr[1]);
			if (sarr[0].leng > 0)
			{
				ctrlList->Add(DB::DBManagerCtrl::Create(sarr[0].ToCString(), log, clif, ssl, parsers));
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
