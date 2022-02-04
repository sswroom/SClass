#include "Stdafx.h"
#include "Crypto/Encrypt/AES256.h"
#include "Crypto/Hash/MD5.h"
#include "DB/DBManager.h"
#include "DB/MySQLConn.h"
#include "DB/ODBCConn.h"
#include "DB/OLEDBConn.h"
#include "DB/SQLiteFile.h"
#include "IO/FileStream.h"
#include "IO/MemoryStream.h"
#include "Net/MySQLTCPClient.h"
#include "Text/MyStringW.h"
#include "Text/UTF8Reader.h"
#include "Text/UTF8Writer.h"
#include "Win32/WMIQuery.h"

#define DBPREFIX ((const UTF8Char*)"DB:")
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
	case DB::DBConn::CT_UNKNOWN:
		break;
	}
	return false;
}

DB::DBTool *DB::DBManager::OpenConn(Text::String *connStr, IO::LogTool *log, Net::SocketFactory *sockf)
{
	return OpenConn(connStr->v, log, sockf);
}

DB::DBTool *DB::DBManager::OpenConn(const UTF8Char *connStr, IO::LogTool *log, Net::SocketFactory *sockf)
{
	DB::DBTool *db;
	UOSInt connStrLen = Text::StrCharCnt(connStr);
	if (Text::StrStartsWithC(connStr, connStrLen, UTF8STRC("odbc:")))
	{
		if (Text::StrStartsWithC(connStr + 5, connStrLen - 5, UTF8STRC("DSN=")))
		{
			Text::StringBuilderUTF8 sb;
			const UTF8Char *dsn = 0;
			const UTF8Char *uid = 0;
			const UTF8Char *pwd = 0;
			const UTF8Char *schema = 0;
			UOSInt cnt;
			sb.AppendC(connStr + 5, connStrLen - 5);
			Text::PString sarr[2];
			sarr[1] = sb;
			while (true)
			{
				cnt = Text::StrSplitP(sarr, 2, sarr[1], ';');
				if (sarr[0].StartsWithICase(UTF8STRC("DSN=")))
				{
					SDEL_TEXT(dsn);
					dsn = Text::StrCopyNewC(sarr[0].v + 4, sarr[0].leng - 4);
				}
				else if (sarr[0].StartsWithICase(UTF8STRC("UID=")))
				{
					SDEL_TEXT(uid);
					uid = Text::StrCopyNewC(sarr[0].v + 4, sarr[0].leng - 4);
				}
				else if (sarr[0].StartsWithICase(UTF8STRC("PWD=")))
				{
					SDEL_TEXT(pwd);
					pwd = Text::StrCopyNewC(sarr[0].v + 4, sarr[0].leng - 4);
				}
				else if (sarr[0].StartsWithICase(UTF8STRC("SCHEMA=")))
				{
					SDEL_TEXT(schema);
					schema = Text::StrCopyNewC(sarr[0].v + 7, sarr[0].leng - 7);
				}
				if (cnt != 2)
				{
					break;
				}
			}
			db = DB::ODBCConn::CreateDBTool(dsn, uid, pwd, schema, log, DBPREFIX);
			SDEL_TEXT(dsn);
			SDEL_TEXT(uid);
			SDEL_TEXT(pwd);
			SDEL_TEXT(schema);
			if (db)
			{
				return db;
			}
		}
		else
		{
			DB::ODBCConn *conn;
			NEW_CLASS(conn, DB::ODBCConn(connStr + 5, (const UTF8Char*)"ODBCConn", log));
			if (conn->GetConnError() == DB::ODBCConn::CE_NONE)
			{
				NEW_CLASS(db, DB::DBTool(conn, true, log, DBPREFIX));
				return db;
			}
			DEL_CLASS(conn);
		}
	}
	else if (Text::StrStartsWithC(connStr, connStrLen, UTF8STRC("mysql:")))
	{
		Text::StringBuilderUTF8 sb;
		const UTF8Char *server = 0;
		const UTF8Char *uid = 0;
		const UTF8Char *pwd = 0;
		const UTF8Char *schema = 0;
		UOSInt cnt;
		sb.AppendC(connStr + 6, connStrLen - 6);
		Text::PString sarr[2];
		sarr[1].v = sb.ToString();
		sarr[1].leng = sb.GetLength();
		while (true)
		{
			cnt = Text::StrSplitP(sarr, 2, sarr[1], ';');
			if (sarr[0].StartsWithICase(UTF8STRC("SERVER=")))
			{
				SDEL_TEXT(server);
				server = Text::StrCopyNewC(sarr[0].v + 7, sarr[0].leng - 7);
			}
			else if (sarr[0].StartsWithICase(UTF8STRC("UID=")))
			{
				SDEL_TEXT(uid);
				uid = Text::StrCopyNewC(sarr[0].v + 4, sarr[0].leng - 4);
			}
			else if (sarr[0].StartsWithICase(UTF8STRC("PWD=")))
			{
				SDEL_TEXT(pwd);
				pwd = Text::StrCopyNewC(sarr[0].v + 4, sarr[0].leng - 4);
			}
			else if (sarr[0].StartsWithICase(UTF8STRC("DATABASE=")))
			{
				SDEL_TEXT(schema);
				schema = Text::StrCopyNewC(sarr[0].v + 9, sarr[0].leng - 9);
			}
			if (cnt != 2)
			{
				break;
			}
		}
		db = DB::MySQLConn::CreateDBTool(sockf, server, schema, uid, pwd, log, DBPREFIX);
		SDEL_TEXT(server);
		SDEL_TEXT(uid);
		SDEL_TEXT(pwd);
		SDEL_TEXT(schema);
		if (db)
		{
			return db;
		}
	}
	else if (Text::StrStartsWithC(connStr, connStrLen, UTF8STRC("sqlite:")))
	{
		if (Text::StrStartsWithICaseC(connStr + 7, connStrLen - 7, UTF8STRC("FILE=")))
		{
			db = DB::SQLiteFile::CreateDBTool(connStr + 12, log, DBPREFIX);
			if (db)
			{
				return db;
			}
		}
	}
	else if (Text::StrStartsWithC(connStr, connStrLen, UTF8STRC("wmi:")))
	{
		if (Text::StrStartsWithICaseC(connStr + 4, connStrLen, UTF8STRC("NS=")))
		{
			const WChar *ns = Text::StrToWCharNew(connStr + 7);
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
	else if (Text::StrStartsWithC(connStr, connStrLen, UTF8STRC("oledb:")))
	{
		const WChar *cstr = Text::StrToWCharNew(connStr + 6);
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
	else if (Text::StrStartsWithC(connStr, connStrLen, UTF8STRC("mysqltcp:")))
	{
		Text::StringBuilderUTF8 sb;
		Net::SocketUtil::AddressInfo addr;
		UInt16 port = 0;
		const UTF8Char *uid = 0;
		const UTF8Char *pwd = 0;
		const UTF8Char *schema = 0;
		addr.addrType = Net::AddrType::Unknown;
		UOSInt cnt;
		sb.AppendC(connStr + 9, connStrLen - 9);
		Text::PString sarr[2];
		sarr[1].v = sb.ToString();
		sarr[1].leng = sb.GetLength();
		while (true)
		{
			cnt = Text::StrSplitP(sarr, 2, sarr[1], ';');
			if (sarr[0].StartsWithICase(UTF8STRC("SERVER=")))
			{
				Net::SocketUtil::GetIPAddr(sarr[0].v + 7, sarr[0].leng - 7, &addr);
			}
			else if (sarr[0].StartsWithICase(UTF8STRC("PORT=")))
			{
				Text::StrToUInt16(sarr[0].v + 5, &port);
			}
			else if (sarr[0].StartsWithICase(UTF8STRC("UID=")))
			{
				SDEL_TEXT(uid);
				uid = Text::StrCopyNewC(sarr[0].v + 4, sarr[0].leng - 4);
			}
			else if (sarr[0].StartsWithICase(UTF8STRC("PWD=")))
			{
				SDEL_TEXT(pwd);
				pwd = Text::StrCopyNewC(sarr[0].v + 4, sarr[0].leng - 4);
			}
			else if (sarr[0].StartsWithICase(UTF8STRC("DATABASE=")))
			{
				SDEL_TEXT(schema);
				schema = Text::StrCopyNewC(sarr[0].v + 9, sarr[0].leng - 9);
			}
			if (cnt != 2)
			{
				break;
			}
		}
		Net::MySQLTCPClient *cli;
		NEW_CLASS(cli, Net::MySQLTCPClient(sockf, &addr, port, uid, pwd, schema));
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
	return 0;
}

Bool DB::DBManager::StoreConn(Text::CString fileName, Data::ArrayList<DB::DBTool*> *dbList)
{
	IO::FileStream *fs;
	NEW_CLASS(fs, IO::FileStream(fileName, IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal));
	if (fs->IsError())
	{
		DEL_CLASS(fs);
		return false;
	}

	if (dbList->GetCount() > 0)
	{
		Text::StringBuilderUTF8 sb;
		DB::DBTool *db;
		UOSInt i;
		UOSInt j;
		i = 0;
		j = dbList->GetCount();
		while (i < j)
		{
			db = dbList->GetItem(i);
			if (GetConnStr(db, &sb))
			{
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

Bool DB::DBManager::RestoreConn(Text::CString fileName, Data::ArrayList<DB::DBTool*> *dbList, IO::LogTool *log, Net::SocketFactory *sockf)
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
		UTF8Char *sarr[2];
		fs->Read(fileBuff, (UOSInt)len);
		UInt8 keyBuff[32];
		UOSInt cnt;
		DB::DBTool *db;
		Crypto::Hash::MD5 md5;
		md5.Calc((const UInt8*)ENCKEY, ENCKEYLEN);
		md5.GetValue(keyBuff);
		md5.Clear();
		md5.Calc(keyBuff, 16);
		md5.GetValue(&keyBuff[16]);
		Crypto::Encrypt::AES256 aes(keyBuff);
		aes.Decrypt(fileBuff, (UOSInt)len, decBuff, 0);
		decBuff[(UOSInt)len] = 0;
		sarr[1] = decBuff;
		while (true)
		{
			cnt = Text::StrSplitLine(sarr, 2, sarr[1]);
			if (sarr[0][0])
			{
				db = OpenConn(sarr[0], log, sockf);
				if (db)
				{
					dbList->Add(db);
				}
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
