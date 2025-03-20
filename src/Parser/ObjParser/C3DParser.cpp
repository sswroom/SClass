#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/PackageFile.h"
#include "Map/CesiumTile.h"
#include "Parser/ObjParser/C3DParser.h"

Parser::ObjParser::C3DParser::C3DParser()
{
	this->encFact = 0;
}

Parser::ObjParser::C3DParser::~C3DParser()
{
}

Int32 Parser::ObjParser::C3DParser::GetName()
{
	return *(Int32*)"C3DP";
}

void Parser::ObjParser::C3DParser::SetEncFactory(Optional<Text::EncodingFactory> encFact)
{
	this->encFact = encFact;
}

void Parser::ObjParser::C3DParser::PrepareSelector(NN<IO::FileSelector> selector, IO::ParserType t)
{
}

IO::ParserType Parser::ObjParser::C3DParser::GetParserType()
{
	return IO::ParserType::CesiumTile;
}

Optional<IO::ParsedObject> Parser::ObjParser::C3DParser::ParseObject(NN<IO::ParsedObject> pobj, Optional<IO::PackageFile> pkgFile, IO::ParserType targetType)
{
	if (pobj->GetParserType() == IO::ParserType::PackageFile)
	{
		NN<IO::PackageFile> pf = NN<IO::PackageFile>::ConvertFrom(pobj);
		Bool needRelease = false;
		if (pf->GetItemPack(CSTR("Scene"), needRelease).SetTo(pf))
		{
			NN<Map::CesiumTile> c3d;
			NEW_CLASSNN(c3d, Map::CesiumTile(pf, pobj->GetSourceNameObj(), this->encFact));
			if (needRelease)
			{
				pf.Delete();
			}
			if (!c3d->IsError())
			{
				return c3d;
			}
			c3d.Delete();
			return 0;
		}
	}
	return 0;
}
