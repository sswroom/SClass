#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/DateTime.h"
#include "IO/StreamDataStream.h"
#include "Map/VectorLayer.h"
#include "Math/CoordinateSystemManager.h"
#include "Math/Math.h"
#include "Math/Geometry/PointZ.h"
#include "Parser/FileParser/WPTParser.h"
#include "Text/MyString.h"
#include "Text/MyStringFloat.h"
#include "Text/UTF8Reader.h"

Parser::FileParser::WPTParser::WPTParser()
{
}

Parser::FileParser::WPTParser::~WPTParser()
{
}

Int32 Parser::FileParser::WPTParser::GetName()
{
	return *(Int32*)"WPTP";
}

void Parser::FileParser::WPTParser::PrepareSelector(NN<IO::FileSelector> selector, IO::ParserType t)
{
	if (t == IO::ParserType::Unknown || t == IO::ParserType::MapLayer)
	{
		selector->AddFilter(CSTR("*.wpt"), CSTR("OziExplorer Waypoint File"));
	}
}

IO::ParserType Parser::FileParser::WPTParser::GetParserType()
{
	return IO::ParserType::MapLayer;
}

Optional<IO::ParsedObject> Parser::FileParser::WPTParser::ParseFileHdr(NN<IO::StreamData> fd, Optional<IO::PackageFile> pkgFile, IO::ParserType targetType, Data::ByteArrayR hdr)
{
	UTF8Char sbuff[1024];
	UnsafeArray<UTF8Char> sptr;
	UnsafeArray<UTF8Char> tmpArr[16];
	Map::VectorLayer *lyr = 0;
	NN<Math::Geometry::PointZ> pt;
	Bool valid;

	if (!fd->GetFullName()->EndsWithICase(UTF8STRC("WPT")))
		return 0;

	IO::StreamDataStream stm(fd);
	Text::UTF8Reader reader(stm);

	valid = true;
	if (!reader.ReadLine(sbuff, 1024).SetTo(sptr) || !Text::StrStartsWithC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("OziExplorer Waypoint File Version ")))
	{
		valid = false;
	}
	if (!reader.ReadLine(sbuff, 1024).SetTo(sptr) || !Text::StrEqualsC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("WGS 84")))
	{
		valid = false;
	}
	if (!reader.ReadLine(sbuff, 1024).SetTo(sptr) || !Text::StrStartsWithC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("Reserved ")))
	{
		valid = false;
	}
	if (valid)
	{
		UnsafeArrayOpt<const UTF8Char> colNames[] = {U8STR("Name"), U8STR("Description")};
		DB::DBUtil::ColType colTypes[] = {DB::DBUtil::CT_VarUTF8Char, DB::DBUtil::CT_VarUTF8Char};
		UOSInt colSizes[] = {14, 40};
		UOSInt colDPs[] = {0, 0};
		reader.ReadLine(sbuff, 1024);
		UnsafeArray<UTF8Char> cols[2];

		NEW_CLASS(lyr, Map::VectorLayer(Map::DRAW_LAYER_POINT, fd->GetFullName(), 2, colNames, Math::CoordinateSystemManager::CreateWGS84Csys(), colTypes, colSizes, colDPs, 0, 0));
		while (reader.ReadLine(sbuff, 1024).NotNull())
		{
			if (Text::StrSplitTrim(tmpArr, 16, sbuff, ',') == 16)
			{
				NEW_CLASSNN(pt, Math::Geometry::PointZ(4326, Text::StrToDouble(tmpArr[3]), Text::StrToDouble(tmpArr[2]), Text::StrToDouble(tmpArr[14]) / 3.2808333333333333333333333333333));
				cols[0] = tmpArr[1];
				cols[1] = tmpArr[10];
				lyr->AddVector(pt, UnsafeArray<UnsafeArrayOpt<const UTF8Char>>::ConvertFrom(UARR(cols)));
			}
		}
	}
	return lyr;
}
