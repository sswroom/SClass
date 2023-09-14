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

void Parser::FileParser::WPTParser::PrepareSelector(IO::FileSelector *selector, IO::ParserType t)
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

IO::ParsedObject *Parser::FileParser::WPTParser::ParseFileHdr(NotNullPtr<IO::StreamData> fd, IO::PackageFile *pkgFile, IO::ParserType targetType, const UInt8 *hdr)
{
	UTF8Char sbuff[1024];
	UTF8Char *sptr;
	UTF8Char *tmpArr[16];
	Map::VectorLayer *lyr = 0;
	NotNullPtr<Math::Geometry::PointZ> pt;
	Bool valid;

	if (!fd->GetFullName()->EndsWithICase(UTF8STRC("WPT")))
		return 0;

	IO::StreamDataStream stm(fd);
	Text::UTF8Reader reader(stm);

	valid = true;
	sptr = reader.ReadLine(sbuff, 1024);
	if (sptr == 0 || !Text::StrStartsWithC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("OziExplorer Waypoint File Version ")))
	{
		valid = false;
	}
	sptr = reader.ReadLine(sbuff, 1024);
	if (sptr == 0 || !Text::StrEqualsC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("WGS 84")))
	{
		valid = false;
	}
	sptr = reader.ReadLine(sbuff, 1024);
	if (sptr == 0 || !Text::StrStartsWithC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("Reserved ")))
	{
		valid = false;
	}
	if (valid)
	{
		const UTF8Char *colNames[] = {(const UTF8Char*)"Name", (const UTF8Char*)"Description"};
		DB::DBUtil::ColType colTypes[] = {DB::DBUtil::CT_VarUTF8Char, DB::DBUtil::CT_VarUTF8Char};
		UOSInt colSizes[] = {14, 40};
		UOSInt colDPs[] = {0, 0};
		reader.ReadLine(sbuff, 1024);
		UTF8Char *cols[2];

		NEW_CLASS(lyr, Map::VectorLayer(Map::DRAW_LAYER_POINT, fd->GetFullName(), 2, colNames, Math::CoordinateSystemManager::CreateDefaultCsys(), colTypes, colSizes, colDPs, 0, 0));
		while (reader.ReadLine(sbuff, 1024))
		{
			if (Text::StrSplitTrim(tmpArr, 16, sbuff, ',') == 16)
			{
				NEW_CLASSNN(pt, Math::Geometry::PointZ(4326, Text::StrToDouble(tmpArr[3]), Text::StrToDouble(tmpArr[2]), Text::StrToDouble(tmpArr[14]) / 3.2808333333333333333333333333333));
				cols[0] = tmpArr[1];
				cols[1] = tmpArr[10];
				lyr->AddVector(pt, (const UTF8Char**)cols);
			}
		}
	}
	return lyr;
}
