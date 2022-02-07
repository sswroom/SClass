#include "Stdafx.h"
#include "MyMemory.h"
#include "DB/SQLiteFile.h"
#include "IO/FileStream.h"
#include "IO/Path.h"
#include "Parser/FileParser/SQLiteParser.h"
#include "Text/MyString.h"

Parser::FileParser::SQLiteParser::SQLiteParser()
{
}

Parser::FileParser::SQLiteParser::~SQLiteParser()
{
}

Int32 Parser::FileParser::SQLiteParser::GetName()
{
	return *(Int32*)"SQLI";
}

void Parser::FileParser::SQLiteParser::PrepareSelector(IO::IFileSelector *selector, IO::ParserType t)
{
	if (t == IO::ParserType::Unknown || t == IO::ParserType::ReadingDB)
	{
		selector->AddFilter((const UTF8Char*)"*.sqlite", (const UTF8Char*)"SQLite File");
	}
}

IO::ParserType Parser::FileParser::SQLiteParser::GetParserType()
{
	return IO::ParserType::ReadingDB;
}

IO::ParsedObject *Parser::FileParser::SQLiteParser::ParseFile(IO::IStreamData *fd, IO::PackageFile *pkgFile, IO::ParserType targetType)
{
	UInt8 hdr[32];
	fd->GetRealData(0, 32, hdr);
	if (!Text::StrStartsWithC(hdr, 32, UTF8STRC("SQLite format 3")))
		return 0;
	if (fd->IsFullFile())
	{
		DB::SQLiteFile *pf;
		NEW_CLASS(pf, DB::SQLiteFile(fd->GetFullFileName()));
		pf->SetSourceName(fd->GetFullName());
		return pf;
	}
	else
	{
		UTF8Char sbuff[512];
		UTF8Char *sptr;
		Data::DateTime t;
		IO::FileStream *fs;
		sptr = IO::Path::GetProcessFileName(sbuff);
		sptr = IO::Path::AppendPathC(sbuff, sptr, UTF8STRC("temp"));
		IO::Path::CreateDirectory(sbuff);
		*sptr++ = IO::Path::PATH_SEPERATOR;
		t.SetCurrTimeUTC();
		sptr = Text::StrHexVal64(sptr, (UInt64)t.ToTicks());
		*sptr++ = '_';
		sptr = fd->GetShortName().ConcatTo(sptr);

		Bool valid = false;
		UInt64 currOfst = 0;
		UOSInt readSize;
		UInt8 *buff;
		buff = MemAlloc(UInt8, 1048576);
		NEW_CLASS(fs, IO::FileStream(CSTRP(sbuff, sptr), IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal));
		while (true)
		{
			readSize = fd->GetRealData(currOfst, 1048576, buff);
			if (readSize <= 0)
				break;
			if (fs->Write(buff, readSize) != readSize)
			{
				valid = false;
			}
			currOfst += readSize;
			if (readSize < 1048576)
				break;
		}
		DEL_CLASS(fs);

		if (valid)
		{
			DB::SQLiteFile *pf;
			NEW_CLASS(pf, DB::SQLiteFile(CSTRP(sbuff, sptr)));
			pf->SetDeleteOnClose(true);
			pf->SetSourceName(fd->GetFullName());
			return pf;
		}
		else
		{
			IO::Path::DeleteFile(sbuff);
			return 0;
		}
	}
}
