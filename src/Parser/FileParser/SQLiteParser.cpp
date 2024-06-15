#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ArrayListStrFast.h"
#include "Data/ArrayListStrFastNN.h"
#include "Data/ByteBuffer.h"
#include "Data/Sort/ArtificialQuickSort.h"
#include "DB/SQLiteFile.h"
#include "IO/FileStream.h"
#include "IO/Path.h"
#include "Map/GeoPackage.h"
#include "Parser/FileParser/SQLiteParser.h"
#include "Text/MyString.h"
#include "Text/StringComparatorFastNN.h"

#include <stdio.h>

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

void Parser::FileParser::SQLiteParser::PrepareSelector(NN<IO::FileSelector> selector, IO::ParserType t)
{
	if (t == IO::ParserType::Unknown || t == IO::ParserType::ReadingDB)
	{
		selector->AddFilter(CSTR("*.sqlite"), CSTR("SQLite File"));
	}
}

IO::ParserType Parser::FileParser::SQLiteParser::GetParserType()
{
	return IO::ParserType::ReadingDB;
}

Optional<IO::ParsedObject> Parser::FileParser::SQLiteParser::ParseFileHdr(NN<IO::StreamData> fd, Optional<IO::PackageFile> pkgFile, IO::ParserType targetType, Data::ByteArrayR hdr)
{
	if (!Text::StrStartsWithC(&hdr[0], 32, UTF8STRC("SQLite format 3")))
		return 0;
	if (fd->IsFullFile())
	{
		DB::SQLiteFile *pf;
		NEW_CLASS(pf, DB::SQLiteFile(fd->GetFullFileName()));
		pf->SetSourceName(fd->GetFullName());
		if (targetType == IO::ParserType::MapLayer || targetType == IO::ParserType::Unknown)
		{
			return ParseAsMap(pf);
		}
		return pf;
	}
	else
	{
		UTF8Char sbuff[512];
		UnsafeArray<UTF8Char> sptr;
		sptr = IO::Path::GetProcessFileName(sbuff).Or(sbuff);
		sptr = IO::Path::AppendPath(sbuff, sptr, CSTR("temp"));
		IO::Path::CreateDirectory(CSTRP(sbuff, sptr));
		*sptr++ = IO::Path::PATH_SEPERATOR;
		sptr = Text::StrHexVal64(sptr, (UInt64)Data::DateTimeUtil::GetCurrTimeMillis());
		*sptr++ = '_';
		sptr = fd->GetShortName().OrEmpty().ConcatTo(sptr);

		Bool valid = false;
		UInt64 currOfst = 0;
		UOSInt readSize;
		{
			Data::ByteBuffer buff(1048576);
			IO::FileStream fs(CSTRP(sbuff, sptr), IO::FileMode::Create, IO::FileShare::DenyNone, IO::FileStream::BufferType::Normal);
			while (true)
			{
				readSize = fd->GetRealData(currOfst, 1048576, buff);
				if (readSize <= 0)
					break;
				if (fs.Write(buff.WithSize(readSize)) != readSize)
				{
					valid = false;
				}
				currOfst += readSize;
				if (readSize < 1048576)
					break;
			}
		}

		if (valid)
		{
			DB::SQLiteFile *pf;
			NEW_CLASS(pf, DB::SQLiteFile(CSTRP(sbuff, sptr)));
			pf->SetDeleteOnClose(true);
			pf->SetSourceName(fd->GetFullName());
			if (targetType == IO::ParserType::MapLayer || targetType == IO::ParserType::Unknown)
			{
				return ParseAsMap(pf);
			}
			return pf;
		}
		else
		{
			IO::Path::DeleteFile(sbuff);
			return 0;
		}
	}
}

Optional<IO::ParsedObject> Parser::FileParser::SQLiteParser::ParseAsMap(DB::DBConn *conn)
{
	Data::ArrayListStringNN tableNames;
	conn->QueryTableNames(CSTR_NULL, tableNames);
	Data::Sort::ArtificialQuickSort::Sort(&tableNames, NN<Data::Comparator<NN<Text::String>>>(tableNames));
	if (tableNames.SortedIndexOfC(CSTR("gpkg_spatial_ref_sys")) < 0 ||
		tableNames.SortedIndexOfC(CSTR("gpkg_contents")) < 0 ||
		tableNames.SortedIndexOfC(CSTR("gpkg_geometry_columns")) < 0 ||
		tableNames.SortedIndexOfC(CSTR("gpkg_tile_matrix_set")) < 0 ||
		tableNames.SortedIndexOfC(CSTR("gpkg_tile_matrix")) < 0)
	{
		tableNames.FreeAll();
		return conn;
	}
	
	tableNames.FreeAll();

	Map::GeoPackage *gpkg;
	NEW_CLASS(gpkg, Map::GeoPackage(conn));
	NN<IO::ParsedObject> pobj = gpkg->CreateLayerCollection();
	gpkg->Release();
	return pobj;
}
