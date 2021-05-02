#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/DateTime.h"
#include "IO/StreamDataStream.h"
#include "IO/StreamReader.h"
#include "Map/GPSTrack.h"
#include "Math/Math.h"
#include "Parser/FileParser/PLTParser.h"
#include "Text/MyString.h"
#include "Text/MyStringFloat.h"

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

void Parser::FileParser::PLTParser::PrepareSelector(IO::IFileSelector *selector, IO::ParsedObject::ParserType t)
{
	if (t == IO::ParsedObject::PT_UNKNOWN || t == IO::ParsedObject::PT_MAP_LAYER_PARSER)
	{
		selector->AddFilter((const UTF8Char*)"*.plt", (const UTF8Char*)"OziExplorer Track Point File");
	}
}

IO::ParsedObject::ParserType Parser::FileParser::PLTParser::GetParserType()
{
	return IO::ParsedObject::PT_MAP_LAYER_PARSER;
}

IO::ParsedObject *Parser::FileParser::PLTParser::ParseFile(IO::IStreamData *fd, IO::PackageFile *pkgFile, IO::ParsedObject::ParserType targetType)
{
	UInt8 buff[40];
	UTF8Char sbuff[1024];
	UTF8Char *tmpArr[6];
	Map::GPSTrack *track = 0;
	Bool valid;

/*	OSInt i = Text::StrLastIndexOf(fd->GetFullName(), '.');
	if (i < 0)
		return 0;
	if (Text::StrCompareICase(&(fd->GetFullName())[i + 1], L"PLT") != 0)
		return 0;*/
	fd->GetRealData(0, 37, buff);
	buff[37] = 0;
	if (Text::StrCompareICase((Char*)buff, "OziExplorer Track Point File Version ") != 0)
		return 0;

	IO::StreamDataStream *stm;
	IO::StreamReader *reader;
	NEW_CLASS(stm, IO::StreamDataStream(fd));
	NEW_CLASS(reader, IO::StreamReader(stm, 65001));

	valid = true;
	reader->ReadLine(sbuff, 1024);
	if (!Text::StrStartsWith(sbuff, (const UTF8Char*)"OziExplorer Track Point File Version "))
	{
		valid = false;
	}
	reader->ReadLine(sbuff, 1024);
	if (Text::StrCompare(sbuff, (const UTF8Char*)"WGS 84") != 0)
	{
		valid = false;
	}
	reader->ReadLine(sbuff, 1024);
	if (Text::StrCompare(sbuff, (const UTF8Char*)"Altitude is in Feet") != 0)
	{
		valid = false;
	}
	reader->ReadLine(sbuff, 1024);
	if (!Text::StrStartsWith(sbuff, (const UTF8Char*)"Reserved "))
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
				dt.AddMS(Math::Double2Int32(tval * 86400000));
				rec.utcTimeTicks = dt.ToTicks();
				track->AddRecord(&rec);
			}
		}
	}
	DEL_CLASS(reader);
	DEL_CLASS(stm);
	return track;
}
