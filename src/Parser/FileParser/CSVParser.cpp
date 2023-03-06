#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/DateTime.h"
#include "DB/CSVFile.h"
#include "IO/StreamDataStream.h"
#include "IO/StreamReader.h"
#include "Map/GPSTrack.h"
#include "Map/VectorLayer.h"
#include "Math/CoordinateSystemManager.h"
#include "Math/Geometry/Point.h"
#include "Parser/FileParser/CSVParser.h"
#include "Text/MyString.h"
#include "Text/MyStringFloat.h"

Parser::FileParser::CSVParser::CSVParser()
{
	this->codePage = 0;
}

Parser::FileParser::CSVParser::~CSVParser()
{
}

Int32 Parser::FileParser::CSVParser::GetName()
{
	return *(Int32*)"CSVP";
}

void Parser::FileParser::CSVParser::SetCodePage(UInt32 codePage)
{
	this->codePage = codePage;
}

void Parser::FileParser::CSVParser::PrepareSelector(IO::FileSelector *selector, IO::ParserType t)
{
	if (t == IO::ParserType::Unknown || t == IO::ParserType::MapLayer || t == IO::ParserType::ReadingDB)
	{
		selector->AddFilter(CSTR("*.csv"), CSTR("CSV File"));
	}
}

IO::ParserType Parser::FileParser::CSVParser::GetParserType()
{
	return IO::ParserType::Unknown;
}

IO::ParsedObject *Parser::FileParser::CSVParser::ParseFileHdr(IO::StreamData *fd, IO::PackageFile *pkgFile, IO::ParserType targetType, const UInt8 *hdr)
{
	UTF8Char sbuff[1024];
	UTF8Char sbuff2[64];
	UTF8Char *sptr2;
	Text::PString tmpArr[2];
	const UTF8Char **tmpcArr2;
	Text::PString *tmpArr2;
	UOSInt colCnt;
	UOSInt currCol;

	if (!fd->GetFullFileName()->EndsWithICase(UTF8STRC(".CSV")))
		return 0;

	UOSInt i;
	IO::StreamDataStream stm(fd);
	IO::StreamReader reader(&stm, this->codePage);

	UOSInt dateCol = INVALID_INDEX;
	UOSInt timeCol = INVALID_INDEX;
	UOSInt dtCol = INVALID_INDEX;
	UOSInt validCol = INVALID_INDEX;
	UOSInt latCol = INVALID_INDEX;
	UOSInt lonCol = INVALID_INDEX;
	UOSInt latDirCol = INVALID_INDEX;
	UOSInt lonDirCol = INVALID_INDEX;
	UOSInt altCol = INVALID_INDEX;
	UOSInt speedCol = INVALID_INDEX;
	UOSInt headingCol = INVALID_INDEX;
	UOSInt nSateCol = INVALID_INDEX;

	Data::ArrayListStrUTF8 colNames;
	reader.ReadLine(sbuff, 1024);
	colCnt = Text::StrCSVSplitP(tmpArr, 2, sbuff);
	currCol = 0;
	while (true)
	{
		colNames.Add(tmpArr[0].v);
		
		if (tmpArr[0].EqualsICase(UTF8STRC("UTC DATE")))
		{
			dateCol = currCol;
		}
		else if (tmpArr[0].EqualsICase(UTF8STRC("UTC TIME")))
		{
			timeCol = currCol;
		}
		else if (tmpArr[0].EqualsICase(UTF8STRC("TS")))
		{
			dtCol = currCol;
		}
		else if (tmpArr[0].EqualsICase(UTF8STRC("VALID")))
		{
			validCol = currCol;
		}
		else if (tmpArr[0].EqualsICase(UTF8STRC("LATITUDE")))
		{
			latCol = currCol;
		}
		else if (tmpArr[0].EqualsICase(UTF8STRC("LONGITUDE")))
		{
			lonCol = currCol;
		}
		else if (tmpArr[0].EqualsICase(UTF8STRC("MAPX")))
		{
			lonCol = currCol;
		}
		else if (tmpArr[0].EqualsICase(UTF8STRC("MAPY")))
		{
			latCol = currCol;
		}
		else if (tmpArr[0].EqualsICase(UTF8STRC("RESULTX")))
		{
			lonCol = currCol;
		}
		else if (tmpArr[0].EqualsICase(UTF8STRC("RESULTY")))
		{
			latCol = currCol;
		}
		else if (tmpArr[0].EqualsICase(UTF8STRC("LAT")))
		{
			latCol = currCol;
		}
		else if (tmpArr[0].EqualsICase(UTF8STRC("LON")))
		{
			lonCol = currCol;
		}
		else if (tmpArr[0].EqualsICase(UTF8STRC("LNG")))
		{
			lonCol = currCol;
		}
		else if (tmpArr[0].EqualsICase(UTF8STRC("N/S")))
		{
			latDirCol = currCol;
		}
		else if (tmpArr[0].EqualsICase(UTF8STRC("E/W")))
		{
			lonDirCol = currCol;
		}
		else if (tmpArr[0].EqualsICase(UTF8STRC("HEIGHT")))
		{
			altCol = currCol;
		}
		else if (tmpArr[0].EqualsICase(UTF8STRC("SPEED")))
		{
			speedCol = currCol;
		}
		else if (tmpArr[0].EqualsICase(UTF8STRC("HEADING")))
		{
			headingCol = currCol;
		}
		else if (tmpArr[0].EqualsICase(UTF8STRC("NSAT(USED/VIEW)")))
		{
			nSateCol = currCol;
		}
		currCol += 1;
		if (colCnt < 2)
			break;
		colCnt = Text::StrCSVSplitP(tmpArr, 2, tmpArr[1].v);
	}

	if (((dateCol != INVALID_INDEX && timeCol != INVALID_INDEX) || (dtCol != INVALID_INDEX)) && latCol != INVALID_INDEX && lonCol != INVALID_INDEX)
	{
		Map::GPSTrack *track;
		Map::GPSTrack::GPSRecord3 rec;
		Data::DateTime dt;
		NEW_CLASS(track, Map::GPSTrack(fd->GetFullName(), altCol != INVALID_INDEX, this->codePage, 0));
		track->SetTrackName(fd->GetShortName());
		
		tmpArr2 = MemAlloc(Text::PString, currCol + 1);
		while (reader.ReadLine(sbuff, 1024))
		{
			if ((UOSInt)currCol == Text::StrCSVSplitP(tmpArr2, currCol + 1, sbuff))
			{
				if (dtCol != INVALID_INDEX)
				{
					dt.SetValue(tmpArr2[dtCol].ToCString());
				}
				else
				{
					sptr2 = Text::StrConcatC(Text::StrConcatC(Text::StrConcatC(sbuff2, tmpArr2[dateCol].v, tmpArr2[dateCol].leng), UTF8STRC(" ")), tmpArr2[timeCol].v, tmpArr2[timeCol].leng);
					dt.SetValue(CSTRP(sbuff2, sptr2));
				}
				rec.utcTimeTicks = dt.ToTicks();
				rec.pos = Math::Coord2DDbl(Text::StrToDouble(tmpArr2[lonCol].v), Text::StrToDouble(tmpArr2[latCol].v));
				if (latDirCol != INVALID_INDEX)
				{
					if (tmpArr2[latDirCol].v[0] == 'S')
						rec.pos.lat = -rec.pos.lat;
				}
				if (lonDirCol != INVALID_INDEX)
				{
					if (tmpArr2[lonDirCol].v[0] == 'W')
						rec.pos.lon = -rec.pos.lon;
				}
				if (validCol != INVALID_INDEX)
				{
					if (Text::StrEqualsICaseC(tmpArr2[validCol].v, tmpArr2[validCol].leng, UTF8STRC("DGPS")))
					{
						rec.valid = true;
					}
					else if (Text::StrEqualsICaseC(tmpArr2[validCol].v, tmpArr2[validCol].leng, UTF8STRC("SPS")))
					{
						rec.valid = true;
					}
					else
					{
						rec.valid = false;
					}
				}
				else
				{
					rec.valid = false;
				}
				if (altCol != INVALID_INDEX)
				{
					i = tmpArr2[altCol].IndexOf(' ');
					if (i != INVALID_INDEX)
					{
						tmpArr2[altCol].TrimToLength(i);
					}
					rec.altitude = Text::StrToDouble(tmpArr2[altCol].v);
				}
				else
				{
					rec.altitude = 0;
				}
				if (speedCol != INVALID_INDEX)
				{
					i = tmpArr2[speedCol].IndexOf(' ');
					if (i != INVALID_INDEX)
					{
						tmpArr2[speedCol].TrimToLength(i);
					}
					rec.speed = Text::StrToDouble(tmpArr2[speedCol].v) / 1.852;
				}
				else
				{
					rec.speed = 0;
				}
				if (headingCol != INVALID_INDEX)
				{
					rec.heading = Text::StrToDouble(tmpArr2[headingCol].v);
				}
				else
				{
					rec.heading = 0;
				}
				if (nSateCol != INVALID_INDEX)
				{
					i = tmpArr2[nSateCol].IndexOf('/');
					if (i != INVALID_INDEX)
					{
						tmpArr2[nSateCol].v[i] = 0;
						Text::StrTrimC(tmpArr2[nSateCol].v, i);
						Text::StrTrimC(&tmpArr2[nSateCol].v[i + 1], tmpArr2[nSateCol].leng - i - 1);
						rec.nSateUsedGPS = (UInt8)Text::StrToUInt32(tmpArr2[nSateCol].v);
						rec.nSateViewGPS = (UInt8)Text::StrToUInt32(&tmpArr2[nSateCol].v[i + 1]);
					}
					else
					{
						rec.nSateUsedGPS = 0;
						rec.nSateViewGPS = 0;
					}
				}
				else
				{
					rec.nSateUsedGPS = 0;
					rec.nSateViewGPS = 0;
				}
				rec.nSateUsed = rec.nSateUsedGPS;
				rec.nSateUsedGLO = 0;
				rec.nSateUsedSBAS = 0;
				rec.nSateViewGLO = 0;
				rec.nSateViewGA = 0;
				rec.nSateViewQZSS = 0;
				rec.nSateViewBD = 0;
				track->AddRecord(&rec);
			}
		}		

		MemFree(tmpArr2);
		return track;
	}
	else if (latCol != INVALID_INDEX && lonCol != INVALID_INDEX)
	{
		Map::VectorLayer *lyr;
		Math::Geometry::Point *pt;
		UOSInt i;
		UOSInt nameCol = 0;

		tmpcArr2 = MemAlloc(const UTF8Char*, currCol + 1);
		i = currCol;
		while (i-- > 0)
		{
			tmpcArr2[i] = colNames.GetItem(i);
			if (Text::StrEndsWithICase(tmpcArr2[i], (const UTF8Char*)"NAME") == 0)
			{
				nameCol = i;
			}
		}
		Math::CoordinateSystem *csys;
		NEW_CLASS(lyr, Map::VectorLayer(Map::DRAW_LAYER_POINT, fd->GetFullName(), currCol, tmpcArr2, csys = Math::CoordinateSystemManager::CreateGeogCoordinateSystemDefName(Math::CoordinateSystemManager::GCST_WGS84), nameCol, 0));
		
		UTF8Char **tmpUArr2 = (UTF8Char**)tmpcArr2;
		while (reader.ReadLine(sbuff, 1024))
		{
			if ((UOSInt)currCol == Text::StrCSVSplit(tmpUArr2, currCol + 1, sbuff))
			{
				NEW_CLASS(pt, Math::Geometry::Point(csys->GetSRID(), Text::StrToDouble(tmpUArr2[lonCol]), Text::StrToDouble(tmpUArr2[latCol])));
				lyr->AddVector(pt, (const UTF8Char**)tmpUArr2);
			}
		}

		MemFree(tmpcArr2);
		return lyr;
	}
	else
	{
		if (colNames.GetCount() > 1 && fd->IsFullFile())
		{			
			DB::CSVFile *csv;
			NEW_CLASS(csv, DB::CSVFile(fd->GetFullFileName(), this->codePage));
			return csv;
		}
		else
		{
			return 0;
		}
	}
}
