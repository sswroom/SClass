#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/DateTime.h"
#include "IO/StreamDataStream.h"
#include "Map/GPSTrack.h"
#include "Math/Math.h"
#include "Parser/FileParser/PLTParser.h"
#include "Text/MyString.h"
#include "Text/MyStringFloat.h"
#include "Text/UTF8Reader.h"

Parser::FileParser::PLTParser::PLTParser()
{
}

Parser::FileParser::PLTParser::~PLTParser()
{
}

Int32 Parser::FileParser::PLTParser::GetName()
{
	return *(Int32*)"PLTP";
}

void Parser::FileParser::PLTParser::PrepareSelector(IO::IFileSelector *selector, IO::ParserType t)
{
	if (t == IO::ParserType::Unknown || t == IO::ParserType::MapLayer)
	{
		selector->AddFilter((const UTF8Char*)"*.plt", (const UTF8Char*)"OziExplorer Track Point File");
	}
}

IO::ParserType Parser::FileParser::PLTParser::GetParserType()
{
	return IO::ParserType::MapLayer;
}

IO::ParsedObject *Parser::FileParser::PLTParser::ParseFile(IO::IStreamData *fd, IO::PackageFile *pkgFile, IO::ParserType targetType)
{
	UInt8 buff[40];
	UTF8Char sbuff[1024];
	UTF8Char *sptr;
	UTF8Char *tmpArr[6];
	Map::GPSTrack *track = 0;
	Bool valid;

/*	UOSInt i = fd->GetFullName()->LastIndexOf('.');
	if (i == INVALID_INDEX)
		return 0;
	if (Text::StrCompareICase(&(fd->GetFullName())[i + 1], L"PLT") != 0)
		return 0;*/
	fd->GetRealData(0, 37, buff);
	buff[37] = 0;
	if (!Text::StrEqualsICaseC(buff, 37, UTF8STRC("OziExplorer Track Point File Version ")))
		return 0;

	IO::StreamDataStream *stm;
	Text::UTF8Reader *reader;
	NEW_CLASS(stm, IO::StreamDataStream(fd));
	NEW_CLASS(reader, Text::UTF8Reader(stm));

	valid = true;
	sptr = reader->ReadLine(sbuff, 1024);
	if (sptr == 0 || !Text::StrStartsWithC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("OziExplorer Track Point File Version ")))
	{
		valid = false;
	}
	sptr = reader->ReadLine(sbuff, 1024);
	if (sptr == 0 || !Text::StrEqualsC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("WGS 84")))
	{
		valid = false;
	}
	sptr = reader->ReadLine(sbuff, 1024);
	if (sptr == 0 || !Text::StrEqualsC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("Altitude is in Feet")))
	{
		valid = false;
	}
	sptr = reader->ReadLine(sbuff, 1024);
	if (sptr == 0 || !Text::StrStartsWithC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("Reserved ")))
	{
		valid = false;
	}
	if (valid)
	{
		reader->ReadLine(sbuff, 1024);
		reader->ReadLine(sbuff, 1024);
		UOSInt cnt;

		NEW_CLASS(track, Map::GPSTrack(fd->GetFullName(), true, 65001, 0));
		while (reader->ReadLine(sbuff, 1024))
		{
			cnt = Text::StrSplitTrim(tmpArr, 6, sbuff, ',');
			if (cnt == 6 || cnt == 5)
			{
				Double dval;
				Double tval;
				Data::DateTime dt(1899, 12, 30, 0, 0, 0, 0);
				Map::GPSTrack::GPSRecord rec;
				rec.lat = Text::StrToDouble(tmpArr[0]);
				rec.lon = Text::StrToDouble(tmpArr[1]);
				rec.heading = 0;
				rec.nSateUsed = 0;
				rec.nSateView = 0;
				rec.speed = 0;
				rec.valid = true;
				rec.altitude = Text::StrToDouble(tmpArr[3]) / 3.2808333333333333333333333333333;
				dval = Text::StrToDouble(tmpArr[4]);
				tval = dval - (Int32)dval;
				dt.AddDay((Int32)dval);
				dt.AddMS(Double2Int32(tval * 86400000));
				rec.utcTimeTicks = dt.ToTicks();
				track->AddRecord(&rec);
			}
		}
	}
	DEL_CLASS(reader);
	DEL_CLASS(stm);
	return track;
}
