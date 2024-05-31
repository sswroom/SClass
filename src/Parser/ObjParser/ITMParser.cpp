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

void Parser::ObjParser::ITMParser::SetParserList(Optional<Parser::ParserList> parsers)
{
	this->parsers = parsers;
}

void Parser::ObjParser::ITMParser::PrepareSelector(NN<IO::FileSelector> selector, IO::ParserType t)
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

Optional<IO::ParsedObject> Parser::ObjParser::ITMParser::ParseObject(NN<IO::ParsedObject> pobj, Optional<IO::PackageFile> pkgFile, IO::ParserType targetType)
{
	NN<Parser::ParserList> parsers;
	if (pobj->GetParserType() != IO::ParserType::PackageFile)
		return 0;
	if (!this->parsers.SetTo(parsers))
		return 0;
	NN<IO::PackageFile> pkg = NN<IO::PackageFile>::ConvertFrom(pobj);
	UTF8Char sbuff[512];
	UnsafeArray<UTF8Char> sptr;
	UOSInt i;
	UOSInt j;
	i = 0;
	j = pkg->GetCount();
	while (i < j)
	{
		if (pkg->GetItemName(sbuff, i).SetTo(sptr) && Text::StrEqualsC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("ituser.poi")))
		{
			if (pkg->GetItemType(i) == IO::PackageFile::PackObjectType::StreamData)
			{
				NN<IO::StreamData> fd;
				NN<IO::ParsedObject> pobj2;
				if (pkg->GetItemStmDataNew(i).SetTo(fd))
				{
					if (parsers->ParseFile(fd).SetTo(pobj2))
					{
						fd.Delete();
						if (pobj2->GetParserType() == IO::ParserType::ReadingDB)
						{
							pobj2->SetSourceName(pobj->GetSourceNameObj());
							return pobj2.Ptr();
						}
						pobj2.Delete();
					}
					else
					{
						fd.Delete();
					}
				}
			}
		}
		i++;
	}
	return 0;
}
