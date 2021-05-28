#include "Stdafx.h"
#include "DB/DBTool.h"
#include "DB/MDBFile.h"
#include "Text/Encoding.h"
#include "Text/EncodingFactory.h"
#include "Text/Locale.h"
#include "Text/MyStringW.h"
#include "Text/StringBuilderUTF8.h"

#ifndef _WIN32_WCE
#if defined(_WIN32) || defined(__CYGWIN__) || defined(__MINGW32__)
#include <windows.h>
#endif
#if defined(__CYGWIN__)
#define ULONG ULONG_Tmp
#endif
#include <odbcinst.h>

DB::MDBFile::MDBFile(const UTF8Char *fileName, IO::LogTool *log, Int32 codePage, const WChar *uid, const WChar *pwd) : DB::ODBCConn(fileName, log)
{
	Text::StringBuilderUTF8 sb;
	sb.Append((const UTF8Char*)"Driver={Microsoft Access Driver (*.mdb)};Dbq=");
	sb.Append(fileName);
	if (codePage != 0)
	{
		UTF8Char sbuff[16];
		if (Text::EncodingFactory::GetInternetName(sbuff, codePage))
		{
			sb.Append((const UTF8Char*)";CharSet=");
			sb.Append(sbuff);
		}

/*		Text::Locale::LocaleEntry *loc = Text::Locale::GetLocaleEntryByCodePage(codePage);
		if (loc)
		{
//			sb.Append((const UTF8Char*)";Locale Identifier=");
//			sb.Append(loc->lcid);
		}*/
	}
	if (uid != 0 || pwd != 0)
	{
		sb.Append((const UTF8Char*)";Uid=");
		if (uid)
		{
			const UTF8Char *csptr = Text::StrToUTF8New(uid);
			sb.Append(csptr);
			Text::StrDelNew(csptr);
		}
		sb.Append((const UTF8Char*)";Pwd=");
		if (pwd)
		{
			const UTF8Char *csptr = Text::StrToUTF8New(pwd);
			sb.Append(csptr);
			Text::StrDelNew(csptr);
		}
	}
	if (!Connect(sb.ToString()))
	{
		sb.ClearStr();
		sb.Append((const UTF8Char*)"Driver={Microsoft Access Driver (*.mdb, *.accdb)};Dbq=");
		sb.Append(fileName);
		if (codePage != 0)
		{
			UTF8Char sbuff[16];
			if (Text::EncodingFactory::GetInternetName(sbuff, codePage))
			{
				sb.Append((const UTF8Char*)";CharSet=");
				sb.Append(sbuff);
			}

	/*		Text::Locale::LocaleEntry *loc = Text::Locale::GetLocaleEntryByCodePage(codePage);
			if (loc)
			{
	//			sb.Append((const UTF8Char*)";Locale Identifier=");
	//			sb.Append(loc->lcid);
			}*/
		}
		if (uid != 0 || pwd != 0)
		{
			sb.Append((const UTF8Char*)";Uid=");
			if (uid)
			{
				const UTF8Char *csptr = Text::StrToUTF8New(uid);
				sb.Append(csptr);
				Text::StrDelNew(csptr);
			}
			sb.Append((const UTF8Char*)";Pwd=");
			if (pwd)
			{
				const UTF8Char *csptr = Text::StrToUTF8New(pwd);
				sb.Append(csptr);
				Text::StrDelNew(csptr);
			}
		}
		if (!Connect(sb.ToString()))
		{
			sb.ClearStr();
			sb.Append((const UTF8Char*)"Driver=MDBTools;DBQ=\"");
			sb.Append(fileName);
			sb.Append((const UTF8Char*)"\"");
			if (codePage != 0)
			{
				UTF8Char sbuff[16];
				if (Text::EncodingFactory::GetInternetName(sbuff, codePage))
				{
					sb.Append((const UTF8Char*)";CharSet=");
					sb.Append(sbuff);
				}

		/*		Text::Locale::LocaleEntry *loc = Text::Locale::GetLocaleEntryByCodePage(codePage);
				if (loc)
				{
		//			sb.Append((const UTF8Char*)";Locale Identifier=");
		//			sb.Append(loc->lcid);
				}*/
			}
			if (uid != 0 || pwd != 0)
			{
				sb.Append((const UTF8Char*)";Uid=");
				if (uid)
				{
					const UTF8Char *csptr = Text::StrToUTF8New(uid);
					sb.Append(csptr);
					Text::StrDelNew(csptr);
				}
				sb.Append((const UTF8Char*)";Pwd=");
				if (pwd)
				{
					const UTF8Char *csptr = Text::StrToUTF8New(pwd);
					sb.Append(csptr);
					Text::StrDelNew(csptr);
				}
			}
			Connect(sb.ToString());
		}
	}
}

Bool DB::MDBFile::CreateMDBFile(const UTF8Char *fileName)
{
#if _WCHAR_SIZE == 4
	BOOL fCreated;
	Char sbuff[512];
	Char *sptr;
	sptr = Text::StrConcat(sbuff, "CREATE_DB=");
	sptr = Text::StrConcat(sptr, (const Char*)fileName);
	sptr = Text::StrConcat(sptr, " General");
	sptr[1] = 0;
	fCreated = SQLConfigDataSource(0, ODBC_ADD_DSN, "Microsoft Access Driver (*.mdb)", sbuff);
	if (fCreated == 0)
	{
		sptr = Text::StrConcat(sbuff, "CREATE_DBV4=");
		sptr = Text::StrConcat(sptr, (const Char*)fileName);
		sptr = Text::StrConcat(sptr, " General");
		sptr[1] = 0;
		fCreated = SQLConfigDataSource(0, ODBC_ADD_DSN, "Microsoft Access Driver (*.mdb, *.accdb)", sbuff);
		if (fCreated == 0)
		{
			sptr = Text::StrConcat(sbuff, "CREATE_DB=");
			sptr = Text::StrConcat(sptr, (const Char*)fileName);
			sptr = Text::StrConcat(sptr, " General");
			sptr[1] = 0;
			fCreated = SQLConfigDataSource(0, ODBC_ADD_DSN, "MDBTools", sbuff);
		}
	}
	return fCreated != 0;
#else
	BOOL fCreated;
	UTF16Char sbuff[512];
	UTF16Char *sptr;
	sptr = Text::StrUTF8_UTF16(sbuff, (const UTF8Char*)"CREATE_DB=", 0);
	sptr = Text::StrUTF8_UTF16(sptr, fileName, 0);
	sptr = Text::StrUTF8_UTF16(sptr, (const UTF8Char*)" General");
	sptr[1] = 0;
	fCreated = SQLConfigDataSourceW(0, ODBC_ADD_DSN, L"Microsoft Access Driver (*.mdb)", sbuff);
	if (fCreated == 0)
	{
		sptr = Text::StrUTF8_UTF16(sbuff, (const UTF8Char*)"CREATE_DBV4=", 0);
		sptr = Text::StrUTF8_UTF16(sptr, fileName, 0);
		sptr = Text::StrUTF8_UTF16(sptr, (const UTF8Char*)" General");
		sptr[1] = 0;
		fCreated = SQLConfigDataSourceW(0, ODBC_ADD_DSN, L"Microsoft Access Driver (*.mdb, *.accdb)", sbuff);
	}
	return fCreated != 0;
#endif
}

DB::DBTool *DB::MDBFile::CreateDBTool(const UTF8Char *fileName, IO::LogTool *log, const UTF8Char *logPrefix)
{
	DB::MDBFile *conn;
	DB::DBTool *db;
	NEW_CLASS(conn, DB::MDBFile(fileName, log, 0, 0, 0));
	if (conn->GetConnError() == DB::ODBCConn::CE_NONE)
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
#endif
