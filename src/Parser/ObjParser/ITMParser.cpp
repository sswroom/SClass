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

void Parser::ObjParser::ITMParser::PrepareSelector(IO::IFileSelector *selector, IO::ParserType t)
{
	if (t == IO::ParserType::ReadingDB)
	{
		selector->AddFilter(CSTR("*.itm"), CSTR("Taiwan GPS Project File"));
	}
}

IO::ParserType Parser::ObjParser::ITMParser::GetParserType()
{
	return IO::ParserType::ReadingDB;
}

IO::ParsedObject *Parser::ObjParser::ITMParser::ParseObject(IO::ParsedObject *pobj, IO::PackageFile *pkgFile, IO::ParserType targetType)
{
	if (pobj->GetParserType() != IO::ParserType::PackageFile)
		return 0;
	if (this->parsers == 0)
		return 0;
	IO::PackageFile *pkg = (IO::PackageFile*)pobj;
	IO::ParserType pt;
	UTF8Char sbuff[512];
	UTF8Char *sptr;
	UOSInt i;
	UOSInt j;
	i = 0;
	j = pkg->GetCount();
	while (i < j)
	{
		sptr = pkg->GetItemName(sbuff, i);
		if (Text::StrEqualsC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("ituser.poi")))
		{
			if (pkg->GetItemType(i) == IO::PackageFile::PackObjectType::StreamData)
			{
				IO::IStreamData *fd = pkg->GetItemStmDataNew(i);
				IO::ParsedObject *pobj2 = parsers->ParseFile(fd, &pt);
				DEL_CLASS(fd);
				if (pobj2)
				{
					if (pt == IO::ParserType::ReadingDB)
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
