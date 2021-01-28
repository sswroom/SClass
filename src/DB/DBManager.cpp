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

Bool DB::DBManager::GetConnStr(DB::DBTool *db, Text::StringBuilderUTF *connStr)
{
	DB::DBConn *conn = db->GetConn();
	const UTF8Char *csptr;
	switch (conn->GetConnType())
	{
	case DB::DBConn::CT_ODBC:
		{
			DB::ODBCConn *odbc = (DB::ODBCConn*)conn;
			if ((csptr = odbc->GetConnStr()) != 0)
			{
				connStr->Append((const UTF8Char*)"odbc:");
				connStr->Append(csptr);
				return true;
			}
			else
			{
				csptr = odbc->GetConnDSN();
				connStr->Append((const UTF8Char*)"odbc:DSN=");
				connStr->Append(csptr);
				if ((csptr = odbc->GetConnUID()) != 0)
				{
					connStr->Append((const UTF8Char*)";UID=");
					connStr->Append(csptr);
				}
				if ((csptr = odbc->GetConnPWD()) != 0)
				{
					connStr->Append((const UTF8Char*)";PWD=");
					connStr->Append(csptr);
				}
				if ((csptr = odbc->GetConnSchema()) != 0)
				{
					connStr->Append((const UTF8Char*)";Schema=");
					connStr->Append(csptr);
				}
				return true;
			}
		}
		break;
	case DB::DBConn::CT_MYSQL:
		{
			DB::MySQLConn *mysql = (DB::MySQLConn*)conn;
			connStr->Append((const UTF8Char*)"mysql:Server=");
			csptr = mysql->GetConnServer();
			connStr->Append(csptr);
			if ((csptr = mysql->GetConnDB()) != 0)
			{
				connStr->Append((const UTF8Char*)";Database=");
				connStr->Append(csptr);
			}
			if ((csptr = mysql->GetConnUID()) != 0)
			{
				connStr->Append((const UTF8Char*)";UID=");
				connStr->Append(csptr);
			}
			if ((csptr = mysql->GetConnPWD()) != 0)
			{
				connStr->Append((const UTF8Char*)";PWD=");
				connStr->Append(csptr);
			}
			return true;
		}
		break;
	case DB::DBConn::CT_SQLITE:
		{
			DB::SQLiteFile *sqlite = (DB::SQLiteFile*)conn;
			connStr->Append((const UTF8Char*)"sqlite:File=");
			connStr->Append(sqlite->GetFileName());
			return true;
		}
		break;
	case DB::DBConn::CT_WMIQUERY:
		{
			Win32::WMIQuery *wmi = (Win32::WMIQuery*)conn;
			connStr->Append((const UTF8Char*)"wmi:ns=");
			const WChar *ns = wmi->GetNS();
			csptr = Text::StrToUTF8New(ns);
			connStr->Append(csptr);
			Text::StrDelNew(csptr);
			return true;
		}
		break;
	case DB::DBConn::CT_OLEDB:
		{
			DB::OLEDBConn *oledb = (DB::OLEDBConn*)conn;
			connStr->Append((const UTF8Char*)"oledb:");
			const WChar *cStr = oledb->GetConnStr();
			csptr = Text::StrToUTF8New(cStr);
			connStr->Append(csptr);
			Text::StrDelNew(csptr);
			return true;
		}
		break;
	case DB::DBConn::CT_MYSQLTCP:
		{
			UTF8Char sbuff[128];
			Net::MySQLTCPClient *mysql = (Net::MySQLTCPClient*)conn;
			connStr->Append((const UTF8Char*)"mysqltcp:Server=");
			Net::SocketUtil::GetAddrName(sbuff, mysql->GetConnAddr());
			connStr->Append(sbuff);
			connStr->Append((const UTF8Char*)";Port=");
			connStr->AppendU16(mysql->GetConnPort());
			if ((csptr = mysql->GetConnDB()) != 0)
			{
				connStr->Append((const UTF8Char*)";Database=");
				connStr->Append(csptr);
			}
			if ((csptr = mysql->GetConnUID()) != 0)
			{
				connStr->Append((const UTF8Char*)";UID=");
				connStr->Append(csptr);
			}
			if ((csptr = mysql->GetConnPWD()) != 0)
			{
				connStr->Append((const UTF8Char*)";PWD=");
				connStr->Append(csptr);
			}
			return true;
		}
		break;
	case DB::DBConn::CT_UNKNOWN:
		break;
	}
	return false;
}

DB::DBTool *DB::DBManager::OpenConn(const UTF8Char *connStr, IO::LogTool *log, Net::SocketFactory *sockf)
{
	DB::DBTool *db;
	if (Text::StrStartsWith(connStr, (const UTF8Char*)"odbc:"))
	{
		if (Text::StrStartsWith(connStr + 5, (const UTF8Char *)"DSN="))
		{
			Text::StringBuilderUTF8 sb;
			const UTF8Char *dsn = 0;
			const UTF8Char *uid = 0;
			const UTF8Char *pwd = 0;
			const UTF8Char *schema = 0;
			UOSInt cnt;
			sb.Append(connStr + 5);
			UTF8Char *sarr[2];
			sarr[1] = sb.ToString();
			while (true)
			{
				cnt = Text::StrSplit(sarr, 2, sarr[1], ';');
				if (Text::StrStartsWithICase(sarr[0], (const UTF8Char *)"DSN="))
				{
					SDEL_TEXT(dsn);
					dsn = Text::StrCopyNew(sarr[0] + 4);
				}
				else if (Text::StrStartsWithICase(sarr[0], (const UTF8Char*)"UID="))
				{
					SDEL_TEXT(uid);
					uid = Text::StrCopyNew(sarr[0] + 4);
				}
				else if (Text::StrStartsWithICase(sarr[0], (const UTF8Char*)"PWD="))
				{
					SDEL_TEXT(pwd);
					pwd = Text::StrCopyNew(sarr[0] + 4);
				}
				else if (Text::StrStartsWithICase(sarr[0], (const UTF8Char*)"SCHEMA="))
				{
					SDEL_TEXT(schema);
					schema = Text::StrCopyNew(sarr[0] + 7);
				}
				if (cnt != 2)
				{
					break;
				}
			}
			db = DB::ODBCConn::CreateDBTool(dsn, uid, pwd, schema, log, false, DBPREFIX);
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
				NEW_CLASS(db, DB::DBTool(conn, true, log, false, DBPREFIX));
				return db;
			}
			DEL_CLASS(conn);
		}
	}
	else if (Text::StrStartsWith(connStr, (const UTF8Char*)"mysql:"))
	{
		Text::StringBuilderUTF8 sb;
		const UTF8Char *server = 0;
		const UTF8Char *uid = 0;
		const UTF8Char *pwd = 0;
		const UTF8Char *schema = 0;
		UOSInt cnt;
		sb.Append(connStr + 6);
		UTF8Char *sarr[2];
		sarr[1] = sb.ToString();
		while (true)
		{
			cnt = Text::StrSplit(sarr, 2, sarr[1], ';');
			if (Text::StrStartsWithICase(sarr[0], (const UTF8Char *)"SERVER="))
			{
				SDEL_TEXT(server);
				server = Text::StrCopyNew(sarr[0] + 7);
			}
			else if (Text::StrStartsWithICase(sarr[0], (const UTF8Char*)"UID="))
			{
				SDEL_TEXT(uid);
				uid = Text::StrCopyNew(sarr[0] + 4);
			}
			else if (Text::StrStartsWithICase(sarr[0], (const UTF8Char*)"PWD="))
			{
				SDEL_TEXT(pwd);
				pwd = Text::StrCopyNew(sarr[0] + 4);
			}
			else if (Text::StrStartsWithICase(sarr[0], (const UTF8Char*)"DATABASE="))
			{
				SDEL_TEXT(schema);
				schema = Text::StrCopyNew(sarr[0] + 9);
			}
			if (cnt != 2)
			{
				break;
			}
		}
		db = DB::MySQLConn::CreateDBTool(sockf, server, schema, uid, pwd, log, false, DBPREFIX);
		SDEL_TEXT(server);
		SDEL_TEXT(uid);
		SDEL_TEXT(pwd);
		SDEL_TEXT(schema);
		if (db)
		{
			return db;
		}
	}
	else if (Text::StrStartsWith(connStr, (const UTF8Char*)"sqlite:"))
	{
		if (Text::StrStartsWithICase(connStr + 7, (const UTF8Char*)"FILE="))
		{
			db = DB::SQLiteFile::CreateDBTool(connStr + 12, log, false, DBPREFIX);
			if (db)
			{
				return db;
			}
		}
	}
	else if (Text::StrStartsWith(connStr, (const UTF8Char*)"wmi:"))
	{
		if (Text::StrStartsWithICase(connStr + 4, (const UTF8Char*)"NS="))
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
				NEW_CLASS(db, DB::DBTool(wmi, true, log, false, DBPREFIX));
				return db;
			}
		}
	}
	else if (Text::StrStartsWith(connStr, (const UTF8Char*)"oledb:"))
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
			NEW_CLASS(db, DB::DBTool(oledb, true, log, false, DBPREFIX));
			return db;
		}
	}
	else if (Text::StrStartsWith(connStr, (const UTF8Char*)"mysqltcp:"))
	{
		Text::StringBuilderUTF8 sb;
		Net::SocketUtil::AddressInfo addr;
		UInt16 port = 0;
		const UTF8Char *uid = 0;
		const UTF8Char *pwd = 0;
		const UTF8Char *schema = 0;
		addr.addrType = Net::SocketUtil::AT_UNKNOWN;
		UOSInt cnt;
		sb.Append(connStr + 9);
		UTF8Char *sarr[2];
		sarr[1] = sb.ToString();
		while (true)
		{
			cnt = Text::StrSplit(sarr, 2, sarr[1], ';');
			if (Text::StrStartsWithICase(sarr[0], (const UTF8Char *)"SERVER="))
			{
				Net::SocketUtil::GetIPAddr(sarr[0] + 7, &addr);
			}
			else if (Text::StrStartsWithICase(sarr[0], (const UTF8Char *)"PORT="))
			{
				Text::StrToUInt16(sarr[0] + 5, &port);
			}
			else if (Text::StrStartsWithICase(sarr[0], (const UTF8Char*)"UID="))
			{
				SDEL_TEXT(uid);
				uid = Text::StrCopyNew(sarr[0] + 4);
			}
			else if (Text::StrStartsWithICase(sarr[0], (const UTF8Char*)"PWD="))
			{
				SDEL_TEXT(pwd);
				pwd = Text::StrCopyNew(sarr[0] + 4);
			}
			else if (Text::StrStartsWithICase(sarr[0], (const UTF8Char*)"DATABASE="))
			{
				SDEL_TEXT(schema);
				schema = Text::StrCopyNew(sarr[0] + 9);
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
			NEW_CLASS(db, DB::DBTool(cli, true, log, false, DBPREFIX));
			return db;
		}
	}
	return 0;
}

Bool DB::DBManager::StoreConn(const UTF8Char *fileName, Data::ArrayList<DB::DBTool*> *dbList)
{
	IO::FileStream *fs;
	NEW_CLASS(fs, IO::FileStream(fileName, IO::FileStream::FILE_MODE_CREATE, IO::FileStream::FILE_SHARE_DENY_NONE, IO::FileStream::BT_NORMAL));
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
				sb.Append((const UTF8Char*)"\r\n");
			}
			i++;
		}
		
		UInt8 keyBuff[32];
		OSInt outSize;
		Crypto::Hash::MD5 md5;
		md5.Calc((const UInt8*)ENCKEY, ENCKEYLEN);
		md5.GetValue(keyBuff);
		md5.Clear();
		md5.Calc(keyBuff, 16);
		md5.GetValue(&keyBuff[16]);
		Crypto::Encrypt::AES256 aes(keyBuff);
		UInt8 *outBuff = MemAlloc(UInt8, sb.GetCharCnt());
		outSize = aes.Encrypt(sb.ToString(), sb.GetCharCnt(), outBuff, 0);
		fs->Write(outBuff, outSize);
		MemFree(outBuff);
	}

	DEL_CLASS(fs);
	return true;
}

Bool DB::DBManager::RestoreConn(const UTF8Char *fileName, Data::ArrayList<DB::DBTool*> *dbList, IO::LogTool *log, Net::SocketFactory *sockf)
{
	IO::FileStream *fs;
	NEW_CLASS(fs, IO::FileStream(fileName, IO::FileStream::FILE_MODE_READONLY, IO::FileStream::FILE_SHARE_DENY_NONE, IO::FileStream::BT_NORMAL));
	if (fs->IsError())
	{
		DEL_CLASS(fs);
		return false;
	}

	Int64 len = fs->GetLength();
	if (len > 0 && len < 65536)
	{
		UInt8 *fileBuff = MemAlloc(UInt8, (OSInt)len);
		UInt8 *decBuff = MemAlloc(UInt8, (OSInt)len + 1);
		UTF8Char *sarr[2];
		fs->Read(fileBuff, (OSInt)len);
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
		aes.Decrypt(fileBuff, (OSInt)len, decBuff, 0);
		decBuff[(OSInt)len] = 0;
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
