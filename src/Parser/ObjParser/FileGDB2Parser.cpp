#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/PackageFile.h"
#include "Map/ESRI/FileGDBDir.h"
#include "Parser/ObjParser/FileGDB2Parser.h"

Parser::ObjParser::FileGDB2Parser::FileGDB2Parser()
{
}

Parser::ObjParser::FileGDB2Parser::~FileGDB2Parser()
{
}

Int32 Parser::ObjParser::FileGDB2Parser::GetName()
{
	return *(Int32*)"FGDB";
}

void Parser::ObjParser::FileGDB2Parser::PrepareSelector(IO::IFileSelector *selector, IO::ParserType t)
{
	if (t == IO::ParserType::ReadingDB)
	{
		selector->AddFilter(CSTR("*.gdb"), CSTR("File Geodatabase File"));
	}
}

IO::ParserType Parser::ObjParser::FileGDB2Parser::GetParserType()
{
	return IO::ParserType::ReadingDB;
}

IO::ParsedObject *Parser::ObjParser::FileGDB2Parser::ParseObject(IO::ParsedObject *pobj, IO::PackageFile *pkgFile, IO::ParserType targetType)
{
	if (pobj->GetParserType() != IO::ParserType::PackageFile)
		return 0;
	IO::PackageFile *pkg = (IO::PackageFile*)pobj;
	UOSInt index = pkg->GetItemIndex(CSTR("a00000001.gdbtable"));
	if (index == INVALID_INDEX)
	{
		return 0;
	}
	Map::ESRI::FileGDBDir *fgdb = Map::ESRI::FileGDBDir::OpenDir(pkg);
	if (fgdb == 0)
	{
		return 0;
	}
	return fgdb;
}
