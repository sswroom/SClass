#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ByteBuffer.h"
#include "IO/PackageFile.h"
#include "Parser/ObjParser/TPKXParser.h"
#include "Text/JSON.h"

Parser::ObjParser::TPKXParser::TPKXParser()
{
}

Parser::ObjParser::TPKXParser::~TPKXParser()
{
}

Int32 Parser::ObjParser::TPKXParser::GetName()
{
	return *(Int32*)"TPKX";
}

void Parser::ObjParser::TPKXParser::PrepareSelector(NN<IO::FileSelector> selector, IO::ParserType t)
{
	if (t == IO::ParserType::Unknown || t == IO::ParserType::MapLayer)
	{
		selector->AddFilter(CSTR("*.tpkx"), CSTR("TPKX File"));
	}
}

IO::ParserType Parser::ObjParser::TPKXParser::GetParserType()
{
	return IO::ParserType::MapLayer;
}

Optional<IO::ParsedObject> Parser::ObjParser::TPKXParser::ParseObject(NN<IO::ParsedObject> pobj, Optional<IO::PackageFile> pkgFile, IO::ParserType targetType)
{
	NN<IO::PackageFile> pkg;
	if (pobj->GetParserType() != IO::ParserType::PackageFile)
		return 0;
	NN<IO::StreamData> rootFD;
	pkg = NN<IO::PackageFile>::ConvertFrom(pobj);
	if (!pkg->GetItemStmDataNew(CSTR("root.json")).SetTo(rootFD))
	{
		return 0;
	}
	UOSInt rootLen = (UOSInt)rootFD->GetDataSize();
	if (rootLen < 2 && rootLen > 1048576)
	{
		rootFD.Delete();
		return 0;
	}
	Data::ByteBuffer rootBuff(rootLen + 1);
	if (rootFD->GetRealData(0, rootLen, rootBuff) != rootLen)
	{
		rootFD.Delete();
		return 0;
	}
	rootFD.Delete();
	rootBuff[rootLen] = 0;
	NN<Text::JSONBase> rootJson;
	if (!Text::JSONBase::ParseJSONStr(Text::CStringNN(rootBuff.Arr(), rootLen)).SetTo(rootJson))
	{
		return 0;
	}
	NN<Text::String> tileBundlesPath;
	Double xmin;
	Double ymin;
	Double xmax;
	Double ymax;
	Int32 tileWidth;
	Int32 tileHeight;
	Int32 minLOD;
	Int32 maxLOD;
	Double originX;
	Double originY;
	if (!rootJson->GetValueAsDouble(CSTR("fullExtent.xmin"), xmin) ||
		!rootJson->GetValueAsDouble(CSTR("fullExtent.ymin"), ymin) ||
		!rootJson->GetValueAsDouble(CSTR("fullExtent.xmax"), xmax) ||
		!rootJson->GetValueAsDouble(CSTR("fullExtent.ymax"), ymax) ||
		!rootJson->GetValueAsInt32(CSTR("tileInfo.cols"), tileWidth) ||
		!rootJson->GetValueAsInt32(CSTR("tileInfo.rows"), tileHeight) ||
		!rootJson->GetValueAsInt32(CSTR("minLOD"), minLOD) ||
		!rootJson->GetValueAsInt32(CSTR("maxLOD"), maxLOD) ||
		!rootJson->GetValueAsDouble(CSTR("tileInfo.origin.x"), originX) ||
		!rootJson->GetValueAsDouble(CSTR("tileInfo.origin.y"), originY) ||
		!rootJson->GetValueString(CSTR("tileBundlesPath")).SetTo(tileBundlesPath))
	{
		rootJson->EndUse();
		return 0;
	}
	///////////////////////
	rootJson->EndUse();
	return 0;
}
