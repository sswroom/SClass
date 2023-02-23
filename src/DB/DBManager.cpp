#include "Stdafx.h"
#include "Crypto/Encrypt/AES256.h"
#include "Crypto/Hash/MD5.h"
#include "DB/DBManager.h"
#include "DB/MySQLConn.h"
#include "DB/ODBCConn.h"
#include "DB/OLEDBConn.h"
#include "DB/PostgreSQLConn.h"
#include "DB/SQLiteFile.h"
#include "IO/FileStream.h"
#include "IO/MemoryStream.h"
#include "Net/MySQLTCPClient.h"
#include "Text/MyStringW.h"
#include "Text/UTF8Reader.h"
#include "Text/UTF8Writer.h"
#include "Win32/WMIQuery.h"

#define DBPREFIX CSTR("DB:")
#define ENCKEY "sswr"
#define ENCKEYLEN (sizeof(ENCKEY) - 1)

Bool DB::DBManager::GetConnStr(DB::DBTool *db, Text::StringBuilderUTF8 *connStr)
{
	DB::DBConn *conn = db->GetConn();
	Text::String *s;
	switch (conn->GetConnType())
	{
	case DB::DBConn::CT_ODBC:
		{
			DB::ODBCConn *odbc = (DB::ODBCConn*)conn;
			if ((s = odbc->GetConnStr()) != 0)
			{
				connStr->AppendC(UTF8STRC("odbc:"));
				connStr->Append(s);
				return true;
			}
			else
			{
				s = odbc->GetConnDSN();
				connStr->AppendC(UTF8STRC("odbc:DSN="));
				connStr->Append(s);
				if ((s = odbc->GetConnUID()) != 0)
				{
					connStr->AppendC(UTF8STRC(";UID="));
					connStr->Append(s);
				}
				if ((s = odbc->GetConnPWD()) != 0)
				{
					connStr->AppendC(UTF8STRC(";PWD="));
					connStr->Append(s);
				}
				if ((s = odbc->GetConnSchema()) != 0)
				{
					connStr->AppendC(UTF8STRC(";Schema="));
					connStr->Append(s);
				}
				return true;
			}
		}
		break;
	case DB::DBConn::CT_MYSQL:
		{
			DB::MySQLConn *mysql = (DB::MySQLConn*)conn;
			connStr->AppendC(UTF8STRC("mysql:Server="));
			s = mysql->GetConnServer();
			connStr->Append(s);
			if ((s = mysql->GetConnDB()) != 0)
			{
				connStr->AppendC(UTF8STRC(";Database="));
				connStr->Append(s);
			}
			if ((s = mysql->GetConnUID()) != 0)
			{
				connStr->AppendC(UTF8STRC(";UID="));
				connStr->Append(s);
			}
			if ((s = mysql->GetConnPWD()) != 0)
			{
				connStr->AppendC(UTF8STRC(";PWD="));
				connStr->Append(s);
			}
			return true;
		}
		break;
	case DB::DBConn::CT_SQLITE:
		{
			DB::SQLiteFile *sqlite = (DB::SQLiteFile*)conn;
			connStr->AppendC(UTF8STRC("sqlite:File="));
			connStr->Append(sqlite->GetFileName());
			return true;
		}
		break;
	case DB::DBConn::CT_WMIQUERY:
		{
			Win32::WMIQuery *wmi = (Win32::WMIQuery*)conn;
			connStr->AppendC(UTF8STRC("wmi:ns="));
			const WChar *ns = wmi->GetNS();
			s = Text::String::NewNotNull(ns);
			connStr->Append(s);
			s->Release();
			return true;
		}
		break;
	case DB::DBConn::CT_OLEDB:
		{
			DB::OLEDBConn *oledb = (DB::OLEDBConn*)conn;
			connStr->AppendC(UTF8STRC("oledb:"));
			const WChar *cStr = oledb->GetConnStr();
			s = Text::String::NewNotNull(cStr);
			connStr->Append(s);
			s->Release();
			return true;
		}
		break;
	case DB::DBConn::CT_MYSQLTCP:
		{
			UTF8Char sbuff[128];
			UTF8Char *sptr;
			Net::MySQLTCPClient *mysql = (Net::MySQLTCPClient*)conn;
			connStr->AppendC(UTF8STRC("mysqltcp:Server="));
			sptr = Net::SocketUtil::GetAddrName(sbuff, mysql->GetConnAddr());
			connStr->AppendP(sbuff, sptr);
			connStr->AppendC(UTF8STRC(";Port="));
			connStr->AppendU16(mysql->GetConnPort());
			if ((s = mysql->GetConnDB()) != 0)
			{
				connStr->AppendC(UTF8STRC(";Database="));
				connStr->Append(s);
			}
			if ((s = mysql->GetConnUID()) != 0)
			{
				connStr->AppendC(UTF8STRC(";UID="));
				connStr->Append(s);
			}
			if ((s = mysql->GetConnPWD()) != 0)
			{
				connStr->AppendC(UTF8STRC(";PWD="));
				connStr->Append(s);
			}
			return true;
		}
		break;
	case DB::DBConn::CT_POSTGRESQL:
		{
			UTF8Char sbuff[128];
			UTF8Char *sptr;
			DB::PostgreSQLConn *psql = (DB::PostgreSQLConn*)conn;
			connStr->AppendC(UTF8STRC("postgresql:Server="));
			sptr = psql->GetConnServer()->ConcatTo(sbuff);
			connStr->AppendP(sbuff, sptr);
			connStr->AppendC(UTF8STRC(";Port="));
			connStr->AppendU16(psql->GetConnPort());
			if ((s = psql->GetConnDB()) != 0)
			{
				connStr->AppendC(UTF8STRC(";Database="));
				connStr->Append(s);
			}
			if ((s = psql->GetConnUID()) != 0)
			{
				connStr->AppendC(UTF8STRC(";UID="));
				connStr->Append(s);
			}
			if ((s = psql->GetConnPWD()) != 0)
			{
				connStr->AppendC(UTF8STRC(";PWD="));
				connStr->Append(s);
			}
			return true;
		}
		break;
	case DB::DBConn::CT_UNKNOWN:
		break;
	}
	return false;
}

DB::DBTool *DB::DBManager::OpenConn(Text::String *connStr, IO::LogTool *log, Net::SocketFactory *sockf)
{
	return OpenConn(connStr->ToCString(), log, sockf);
}

DB::DBTool *DB::DBManager::OpenConn(Text::CString connStr, IO::LogTool *log, Net::SocketFactory *sockf)
{
	DB::DBTool *db;
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
					dsn = Text::String::New(sarr[0].v + 4, sarr[0].leng - 4);
				}
				else if (sarr[0].StartsWithICase(UTF8STRC("UID=")))
				{
					SDEL_STRING(uid);
					uid = Text::String::New(sarr[0].v + 4, sarr[0].leng - 4);
				}
				else if (sarr[0].StartsWithICase(UTF8STRC("PWD=")))
				{
					SDEL_STRING(pwd);
					pwd = Text::String::New(sarr[0].v + 4, sarr[0].leng - 4);
				}
				else if (sarr[0].StartsWithICase(UTF8STRC("SCHEMA=")))
				{
					SDEL_STRING(schema);
					schema = Text::String::New(sarr[0].v + 7, sarr[0].leng - 7);
				}
				if (cnt != 2)
				{
					break;
				}
			}
			db = DB::ODBCConn::CreateDBTool(dsn, uid, pwd, schema, log, DBPREFIX);
			SDEL_STRING(dsn);
			SDEL_STRING(uid);
			SDEL_STRING(pwd);
			SDEL_STRING(schema);
			if (db)
			{
				return db;
			}
		}
		else
		{
			DB::ODBCConn *conn;
			NEW_CLASS(conn, DB::ODBCConn(connStr.Substring(5), CSTR("ODBCConn"), log));
			if (conn->GetConnError() == DB::ODBCConn::CE_NONE)
			{
				NEW_CLASS(db, DB::DBTool(conn, true, log, DBPREFIX));
				return db;
			}
			DEL_CLASS(conn);
		}
	}
	else if (connStr.StartsWith(UTF8STRC("mysql:")))
	{
		Text::StringBuilderUTF8 sb;
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
				server = Text::String::New(sarr[0].v + 7, sarr[0].leng - 7);
			}
			else if (sarr[0].StartsWithICase(UTF8STRC("UID=")))
			{
				SDEL_STRING(uid);
				uid = Text::String::New(sarr[0].v + 4, sarr[0].leng - 4);
			}
			else if (sarr[0].StartsWithICase(UTF8STRC("PWD=")))
			{
				SDEL_STRING(pwd);
				pwd = Text::String::New(sarr[0].v + 4, sarr[0].leng - 4);
			}
			else if (sarr[0].StartsWithICase(UTF8STRC("DATABASE=")))
			{
				SDEL_STRING(schema);
				schema = Text::String::New(sarr[0].v + 9, sarr[0].leng - 9);
			}
			if (cnt != 2)
			{
				break;
			}
		}
		db = DB::MySQLConn::CreateDBTool(sockf, server, schema, uid, pwd, log, DBPREFIX);
		SDEL_STRING(server);
		SDEL_STRING(uid);
		SDEL_STRING(pwd);
		SDEL_STRING(schema);
		if (db)
		{
			return db;
		}
	}
	else if (connStr.StartsWith(UTF8STRC("sqlite:")))
	{
		if (connStr.Substring(7).StartsWithICase(UTF8STRC("FILE=")))
		{
			db = DB::SQLiteFile::CreateDBTool(connStr.Substring(12), log, DBPREFIX);
			if (db)
			{
				return db;
			}
		}
	}
	else if (connStr.StartsWith(UTF8STRC("wmi:")))
	{
		if (connStr.Substring(4).StartsWithICase(UTF8STRC("NS=")))
		{
			const WChar *ns = Text::StrToWCharNew(connStr.v + 7);
			Win32::WMIQuery *wmi;
			NEW_CLASS(wmi, Win32::WMIQuery(ns));
			if (wmi->IsError())
			{
				DEL_CLASS(wmi);
			}
			else
			{
				NEW_CLASS(db, DB::DBTool(wmi, true, log, DBPREFIX));
				return db;
			}
		}
	}
	else if (connStr.StartsWith(UTF8STRC("oledb:")))
	{
		const WChar *cstr = Text::StrToWCharNew(connStr.v + 6);
		DB::OLEDBConn *oledb;
		NEW_CLASS(oledb, DB::OLEDBConn(cstr, log));
		if (oledb->GetConnError() != DB::OLEDBConn::CE_NONE)
		{
			DEL_CLASS(oledb);
		}
		else
		{
			NEW_CLASS(db, DB::DBTool(oledb, true, log, DBPREFIX));
			return db;
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
				Net::SocketUtil::GetIPAddr(sarr[0].ToCString().Substring(7), &addr);
			}
			else if (sarr[0].StartsWithICase(UTF8STRC("PORT=")))
			{
				Text::StrToUInt16(sarr[0].v + 5, &port);
			}
			else if (sarr[0].StartsWithICase(UTF8STRC("UID=")))
			{
				SDEL_STRING(uid);
				uid = Text::String::New(sarr[0].v + 4, sarr[0].leng - 4);
			}
			else if (sarr[0].StartsWithICase(UTF8STRC("PWD=")))
			{
				SDEL_STRING(pwd);
				pwd = Text::String::New(sarr[0].v + 4, sarr[0].leng - 4);
			}
			else if (sarr[0].StartsWithICase(UTF8STRC("DATABASE=")))
			{
				SDEL_STRING(schema);
				schema = Text::String::New(sarr[0].v + 9, sarr[0].leng - 9);
			}
			if (cnt != 2)
			{
				break;
			}
		}
		Net::MySQLTCPClient *cli;
		NEW_CLASS(cli, Net::MySQLTCPClient(sockf, &addr, port, uid, pwd, schema));
		SDEL_STRING(uid);
		SDEL_STRING(pwd);
		SDEL_STRING(schema);
		if (cli->IsError())
		{
			DEL_CLASS(cli);
		}
		else
		{
			NEW_CLASS(db, DB::DBTool(cli, true, log, DBPREFIX));
			return db;
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
				server = Text::String::New(sarr[0].v + 7, sarr[0].leng - 7);
			}
			else if (sarr[0].StartsWithICase(UTF8STRC("PORT=")))
			{
				Text::StrToUInt16(sarr[0].v + 5, &port);
			}
			else if (sarr[0].StartsWithICase(UTF8STRC("UID=")))
			{
				SDEL_STRING(uid);
				uid = Text::String::New(sarr[0].v + 4, sarr[0].leng - 4);
			}
			else if (sarr[0].StartsWithICase(UTF8STRC("PWD=")))
			{
				SDEL_STRING(pwd);
				pwd = Text::String::New(sarr[0].v + 4, sarr[0].leng - 4);
			}
			else if (sarr[0].StartsWithICase(UTF8STRC("DATABASE=")))
			{
				SDEL_STRING(schema);
				schema = Text::String::New(sarr[0].v + 9, sarr[0].leng - 9);
			}
			if (cnt != 2)
			{
				break;
			}
		}
		DB::PostgreSQLConn *cli;
		NEW_CLASS(cli, DB::PostgreSQLConn(server, port, uid, pwd, schema, log));
		SDEL_STRING(server);
		SDEL_STRING(uid);
		SDEL_STRING(pwd);
		SDEL_STRING(schema);
		if (cli->IsConnError())
		{
			DEL_CLASS(cli);
		}
		else
		{
			NEW_CLASS(db, DB::DBTool(cli, true, log, DBPREFIX));
			return db;
		}
	}
	return 0;
}

void DB::DBManager::GetConnName(Text::CString connStr, Text::StringBuilderUTF8 *sbOut)
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
}

Bool DB::DBManager::StoreConn(Text::CString fileName, Data::ArrayList<DB::DBManagerCtrl*> *ctrlList)
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
		DB::DBManagerCtrl *ctrl;
		Text::String *connStr;
		UOSInt i;
		UOSInt j;
		i = 0;
		j = ctrlList->GetCount();
		while (i < j)
		{
			ctrl = ctrlList->GetItem(i);
			connStr = ctrl->GetConnStr();
			if (connStr)
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
		outSize = aes.Encrypt(sb.ToString(), sb.GetCharCnt(), outBuff, 0);
		fs->Write(outBuff, outSize);
		MemFree(outBuff);
	}

	DEL_CLASS(fs);
	return true;
}

Bool DB::DBManager::RestoreConn(Text::CString fileName, Data::ArrayList<DB::DBManagerCtrl*> *ctrlList, IO::LogTool *log, Net::SocketFactory *sockf)
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
		UInt8 *fileBuff = MemAlloc(UInt8, (UOSInt)len);
		UInt8 *decBuff = MemAlloc(UInt8, (UOSInt)len + 1);
		Text::PString sarr[2];
		fs->Read(fileBuff, (UOSInt)len);
		UInt8 keyBuff[32];
		UOSInt cnt;
		Crypto::Hash::MD5 md5;
		md5.Calc((const UInt8*)ENCKEY, ENCKEYLEN);
		md5.GetValue(keyBuff);
		md5.Clear();
		md5.Calc(keyBuff, 16);
		md5.GetValue(&keyBuff[16]);
		Crypto::Encrypt::AES256 aes(keyBuff);
		aes.Decrypt(fileBuff, (UOSInt)len, decBuff, 0);
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
				ctrlList->Add(DB::DBManagerCtrl::Create(sarr[0].ToCString(), log, sockf));
			}
			if (cnt != 2)
			{
				break;
			}
		}
		MemFree(decBuff);
		MemFree(fileBuff);
	}
	DEL_CLASS(fs);
	return true;
}
