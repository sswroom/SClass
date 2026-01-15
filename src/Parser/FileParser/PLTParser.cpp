#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/DateTime.h"
#include "IO/StreamDataStream.h"
#include "Map/GPSTrack.h"
#include "Math/Math_C.h"
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

void Parser::FileParser::PLTParser::PrepareSelector(NN<IO::FileSelector> selector, IO::ParserType t)
{
	if (t == IO::ParserType::Unknown || t == IO::ParserType::MapLayer)
	{
		selector->AddFilter(CSTR("*.plt"), CSTR("OziExplorer Track Point File"));
	}
}

IO::ParserType Parser::FileParser::PLTParser::GetParserType()
{
	return IO::ParserType::MapLayer;
}

Optional<IO::ParsedObject> Parser::FileParser::PLTParser::ParseFileHdr(NN<IO::StreamData> fd, Optional<IO::PackageFile> pkgFile, IO::ParserType targetType, Data::ByteArrayR hdr)
{
	UTF8Char sbuff[1024];
	UnsafeArray<UTF8Char> sptr;
	UnsafeArray<UTF8Char> tmpArr[6];
	Map::GPSTrack *track = nullptr;
	Bool valid;

/*	UIntOS i = fd->GetFullName()->LastIndexOf('.');
	if (i == INVALID_INDEX)
		return 0;
	if (Text::StrCompareICase(&(fd->GetFullName())[i + 1], L"PLT") != 0)
		return 0;*/
	if (!Text::StrEqualsC(&hdr[0], 37, UTF8STRC("OziExplorer Track Point File Version ")))
		return nullptr;

	IO::StreamDataStream stm(fd);
	Text::UTF8Reader reader(stm);

	valid = true;
	if (!reader.ReadLine(sbuff, 1024).SetTo(sptr) || !Text::StrStartsWithC(sbuff, (UIntOS)(sptr - sbuff), UTF8STRC("OziExplorer Track Point File Version ")))
	{
		valid = false;
	}
	if (!reader.ReadLine(sbuff, 1024).SetTo(sptr) || !Text::StrEqualsC(sbuff, (UIntOS)(sptr - sbuff), UTF8STRC("WGS 84")))
	{
		valid = false;
	}
	if (!reader.ReadLine(sbuff, 1024).SetTo(sptr) || !Text::StrEqualsC(sbuff, (UIntOS)(sptr - sbuff), UTF8STRC("Altitude is in Feet")))
	{
		valid = false;
	}
	if (!reader.ReadLine(sbuff, 1024).SetTo(sptr) || !Text::StrStartsWithC(sbuff, (UIntOS)(sptr - sbuff), UTF8STRC("Reserved ")))
	{
		valid = false;
	}
	if (valid)
	{
		reader.ReadLine(sbuff, 1024);
		reader.ReadLine(sbuff, 1024);
		UIntOS cnt;

		NEW_CLASS(track, Map::GPSTrack(fd->GetFullName(), true, 65001, nullptr));
		while (reader.ReadLine(sbuff, 1024).NotNull())
		{
			cnt = Text::StrSplitTrim(tmpArr, 6, sbuff, ',');
			if (cnt == 6 || cnt == 5)
			{
				Double dval;
				Double tval;
				Data::DateTime dt(1899, 12, 30, 0, 0, 0, 0);
				Map::GPSTrack::GPSRecord3 rec;
				rec.pos.SetLat(Text::StrToDoubleOrNAN(tmpArr[0]));
				rec.pos.SetLon(Text::StrToDoubleOrNAN(tmpArr[1]));
				rec.heading = 0;
				rec.nSateUsed = 0;
				rec.nSateUsedGLO = 0;
				rec.nSateUsedGPS = 0;
				rec.nSateUsedSBAS = 0;
				rec.nSateViewGPS = 0;
				rec.nSateViewGLO = 0;
				rec.nSateViewGA = 0;
				rec.nSateViewQZSS = 0;
				rec.nSateViewBD = 0;
				rec.speed = 0;
				rec.valid = true;
				rec.altitude = Text::StrToDoubleOrNAN(tmpArr[3]) / 3.2808333333333333333333333333333;
				dval = Text::StrToDoubleOr(tmpArr[4], 0);
				tval = dval - (Int32)dval;
				dt.AddDay((Int32)dval);
				dt.AddMS(Double2Int32(tval * 86400000));
				rec.recTime = dt.ToInstant();
				track->AddRecord(rec);
			}
		}
	}
	return track;
}
