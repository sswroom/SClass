#include "Stdafx.h"
#include "DB/DBTool.h"
#include "DB/MDBFileConn.h"
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

DB::MDBFileConn::MDBFileConn(Text::CStringNN fileName, NN<IO::LogTool> log, UInt32 codePage, const WChar *uid, const WChar *pwd) : DB::ODBCConn(fileName, log)
{
	Text::StringBuilderUTF8 sb;
	sb.AppendC(UTF8STRC("Driver={Microsoft Access Driver (*.mdb)};Dbq="));
	sb.Append(fileName);
	if (codePage != 0)
	{
		UTF8Char sbuff[16];
		UnsafeArray<UTF8Char> sptr = Text::EncodingFactory::GetInternetName(sbuff, codePage);
		sb.AppendC(UTF8STRC(";CharSet="));
		sb.AppendC(sbuff, (UOSInt)(sptr - sbuff));

/*		Text::Locale::LocaleEntry *loc = Text::Locale::GetLocaleEntryByCodePage(codePage);
		if (loc)
		{
//			sb.AppendC(UTF8STRC(";Locale Identifier="));
//			sb.Append(loc->lcid);
		}*/
	}
	if (uid != 0 || pwd != 0)
	{
		sb.AppendC(UTF8STRC(";Uid="));
		if (uid)
		{
			NN<Text::String> s = Text::String::NewNotNull(uid);
			sb.Append(s);
			s->Release();
		}
		sb.AppendC(UTF8STRC(";Pwd="));
		if (pwd)
		{
			NN<Text::String> s = Text::String::NewNotNull(pwd);
			sb.Append(s);
			s->Release();
		}
	}
	if (!Connect(sb.ToCString()))
	{
		sb.ClearStr();
		sb.AppendC(UTF8STRC("Driver={Microsoft Access Driver (*.mdb, *.accdb)};Dbq="));
		sb.Append(fileName);
		if (codePage != 0)
		{
			UTF8Char sbuff[16];
			UnsafeArray<UTF8Char> sptr = Text::EncodingFactory::GetInternetName(sbuff, codePage);
			sb.AppendC(UTF8STRC(";CharSet="));
			sb.AppendC(sbuff, (UOSInt)(sptr - sbuff));

	/*		Text::Locale::LocaleEntry *loc = Text::Locale::GetLocaleEntryByCodePage(codePage);
			if (loc)
			{
	//			sb.AppendC(UTF8STRC(";Locale Identifier="));
	//			sb.Append(loc->lcid);
			}*/
		}
		if (uid != 0 || pwd != 0)
		{
			sb.AppendC(UTF8STRC(";Uid="));
			if (uid)
			{
				NN<Text::String> s = Text::String::NewNotNull(uid);
				sb.Append(s);
				s->Release();
			}
			sb.AppendC(UTF8STRC(";Pwd="));
			if (pwd)
			{
				NN<Text::String> s = Text::String::NewNotNull(pwd);
				sb.Append(s);
				s->Release();
			}
		}
		if (!Connect(sb.ToCString()))
		{
			sb.ClearStr();
			sb.AppendC(UTF8STRC("Driver=MDBTools;DBQ=\""));
			sb.Append(fileName);
			sb.AppendC(UTF8STRC("\""));
			if (codePage != 0)
			{
				UTF8Char sbuff[16];
				UnsafeArray<UTF8Char> sptr = Text::EncodingFactory::GetInternetName(sbuff, codePage);
				sb.AppendC(UTF8STRC(";CharSet="));
				sb.AppendC(sbuff, (UOSInt)(sptr - sbuff));

		/*		Text::Locale::LocaleEntry *loc = Text::Locale::GetLocaleEntryByCodePage(codePage);
				if (loc)
				{
		//			sb.AppendC(UTF8STRC(";Locale Identifier="));
		//			sb.Append(loc->lcid);
				}*/
			}
			if (uid != 0 || pwd != 0)
			{
				sb.AppendC(UTF8STRC(";Uid="));
				if (uid)
				{
					NN<Text::String> s = Text::String::NewNotNull(uid);
					sb.Append(s);
					s->Release();
				}
				sb.AppendC(UTF8STRC(";Pwd="));
				if (pwd)
				{
					NN<Text::String> s = Text::String::NewNotNull(pwd);
					sb.Append(s);
					s->Release();
				}
			}
			Connect(sb.ToCString());
		}
	}
}

Bool DB::MDBFileConn::CreateMDBFile(Text::CStringNN fileName)
{
#if _WCHAR_SIZE == 4
	BOOL fCreated;
	UTF8Char sbuff[512];
	UnsafeArray<UTF8Char> sptr;
	sptr = Text::StrConcatC(sbuff, UTF8STRC("CREATE_DB="));
	sptr = fileName.ConcatTo(sptr);
	sptr = Text::StrConcatC(sptr, UTF8STRC(" General"));
	sptr[1] = 0;
	fCreated = SQLConfigDataSource(0, ODBC_ADD_DSN, "Microsoft Access Driver (*.mdb)", (Char*)sbuff);
	if (fCreated == 0)
	{
		sptr = Text::StrConcatC(sbuff, UTF8STRC("CREATE_DBV4="));
		sptr = fileName.ConcatTo(sptr);
		sptr = Text::StrConcatC(sptr, UTF8STRC(" General"));
		sptr[1] = 0;
		fCreated = SQLConfigDataSource(0, ODBC_ADD_DSN, "Microsoft Access Driver (*.mdb, *.accdb)", (Char*)sbuff);
		if (fCreated == 0)
		{
			sptr = Text::StrConcatC(sbuff, UTF8STRC("CREATE_DB="));
			sptr = fileName.ConcatTo(sptr);
			sptr = Text::StrConcatC(sptr, UTF8STRC(" General"));
			sptr[1] = 0;
			fCreated = SQLConfigDataSource(0, ODBC_ADD_DSN, "MDBTools", (Char*)sbuff);
		}
	}
	return fCreated != 0;
#else
	BOOL fCreated;
	UTF16Char sbuff[512];
	UnsafeArray<UTF16Char> sptr;
	sptr = Text::StrUTF8_UTF16(sbuff, (const UTF8Char*)"CREATE_DB=", 0);
	sptr = Text::StrUTF8_UTF16(sptr, fileName.v, 0);
	sptr = Text::StrUTF8_UTF16(sptr, (const UTF8Char*)" General");
	sptr[1] = 0;
	fCreated = SQLConfigDataSourceW(0, ODBC_ADD_DSN, L"Microsoft Access Driver (*.mdb)", sbuff);
	if (fCreated == 0)
	{
		sptr = Text::StrUTF8_UTF16(sbuff, (const UTF8Char*)"CREATE_DBV4=", 0);
		sptr = Text::StrUTF8_UTF16(sptr, fileName.v, 0);
		sptr = Text::StrUTF8_UTF16(sptr, (const UTF8Char*)" General");
		sptr[1] = 0;
		fCreated = SQLConfigDataSourceW(0, ODBC_ADD_DSN, L"Microsoft Access Driver (*.mdb, *.accdb)", sbuff);
	}
	return fCreated != 0;
#endif
}

Optional<DB::DBTool> DB::MDBFileConn::CreateDBTool(NN<Text::String> fileName, NN<IO::LogTool> log, Text::CString logPrefix)
{
	NN<DB::MDBFileConn> conn;
	DB::DBTool *db;
	NEW_CLASSNN(conn, DB::MDBFileConn(fileName->ToCString(), log, 0, 0, 0));
	if (conn->GetConnError() == DB::ODBCConn::CE_NONE)
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

Optional<DB::DBTool> DB::MDBFileConn::CreateDBTool(Text::CStringNN fileName, NN<IO::LogTool> log, Text::CString logPrefix)
{
	NN<DB::MDBFileConn> conn;
	DB::DBTool *db;
	NEW_CLASSNN(conn, DB::MDBFileConn(fileName, log, 0, 0, 0));
	if (conn->GetConnError() == DB::ODBCConn::CE_NONE)
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
#endif
