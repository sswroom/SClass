#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/PackageFile.h"
#include "Parser/ParserList.h"
#include "Parser/ObjParser/ITMParser.h"

Parser::ObjParser::ITMParser::ITMParser()
{
	this->parsers = 0;
}

Parser::ObjParser::ITMParser::~ITMParser()
{
}

Int32 Parser::ObjParser::ITMParser::GetName()
{
	return *(Int32*)"ITMP";
}

void Parser::ObjParser::ITMParser::SetParserList(Parser::ParserList *parsers)
{
	this->parsers = parsers;
}

void Parser::ObjParser::ITMParser::PrepareSelector(IO::IFileSelector *selector, IO::ParsedObject::ParserType t)
{
	if (t == IO::ParsedObject::PT_READINGDB_PARSER)
	{
		selector->AddFilter((const UTF8Char*)"*.itm", (const UTF8Char*)"Taiwan GPS Project File");
	}
}

IO::ParsedObject::ParserType Parser::ObjParser::ITMParser::GetParserType()
{
	return IO::ParsedObject::PT_READINGDB_PARSER;
}

IO::ParsedObject *Parser::ObjParser::ITMParser::ParseObject(IO::ParsedObject *pobj, IO::PackageFile *pkgFile, IO::ParsedObject::ParserType targetType)
{
	if (pobj->GetParserType() != IO::ParsedObject::PT_PACKAGE_PARSER)
		return 0;
	if (this->parsers == 0)
		return 0;
	IO::PackageFile *pkg = (IO::PackageFile*)pobj;
	IO::ParsedObject::ParserType pt;
	UTF8Char sbuff[512];
	UOSInt i;
	UOSInt j;
	i = 0;
	j = pkg->GetCount();
	while (i < j)
	{
		pkg->GetItemName(sbuff, i);
		if (Text::StrEquals(sbuff, (const UTF8Char*)"ituser.poi"))
		{
			if (pkg->GetItemType(i) == IO::PackageFile::POT_STREAMDATA)
			{
				IO::IStreamData *fd = pkg->GetItemStmData(i);
				IO::ParsedObject *pobj2 = parsers->ParseFile(fd, &pt);
				DEL_CLASS(fd);
				if (pobj2)
				{
					if (pt == IO::ParsedObject::PT_READINGDB_PARSER)
					{
						pobj2->SetSourceName(pobj->GetSourceNameObj());
						return pobj2;
					}
					DEL_CLASS(pobj2);
				}
			}
		}
		i++;
	}
	return 0;
}
