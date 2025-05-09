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

void Parser::FileParser::CSVParser::PrepareSelector(NN<IO::FileSelector> selector, IO::ParserType t)
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

Optional<IO::ParsedObject> Parser::FileParser::CSVParser::ParseFileHdr(NN<IO::StreamData> fd, Optional<IO::PackageFile> pkgFile, IO::ParserType targetType, Data::ByteArrayR hdr)
{
	UTF8Char sbuff[1024];
	UTF8Char sbuff2[64];
	UnsafeArray<UTF8Char> sptr2;
	Text::PString tmpArr[2];
	UnsafeArray<UnsafeArray<const UTF8Char>> tmpcArr2;
	Text::PString *tmpArr2;
	UOSInt colCnt;
	UOSInt currCol;

	if (!fd->GetFullFileName()->EndsWithICase(UTF8STRC(".CSV")))
		return 0;

	UOSInt i;
	IO::StreamDataStream stm(fd);
	IO::StreamReader reader(stm, this->codePage);

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

	UInt32 gloggerMatches = 0;

	Data::ArrayListStrUTF8 colNames;
	reader.ReadLine(sbuff, 1024);
	colCnt = Text::StrCSVSplitP(tmpArr, 2, sbuff);
	currCol = 0;
	while (true)
	{
		colNames.Add(UnsafeArray<const UTF8Char>(tmpArr[0].v));
		
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
		else if (tmpArr[0].Equals(UTF8STRC("1")))
		{
			if (currCol == 0)
				gloggerMatches |= 1;
		}
		else if (tmpArr[0].Equals(UTF8STRC("0.0")))
		{
			if (currCol == 2)
				gloggerMatches |= 2;
			else if (currCol == 5)
				gloggerMatches |= 4;
		}
		else if (tmpArr[0].leng == 0)
		{
			if (currCol == 8 && colCnt == 1)
				gloggerMatches |= 8;
		}
		currCol += 1;
		if (colCnt < 2)
			break;
		colCnt = Text::StrCSVSplitP(tmpArr, 2, tmpArr[1].v);
	}

	if (gloggerMatches == 15)
	{
		//id, track start, time passed, lat, lon, distance travelled, speed (knot), altitude, empty
		stm.SeekFromBeginning(0);
		Map::GPSTrack *track;
		Map::GPSTrack::GPSRecord3 rec;
		Text::PString sarr[10];
		reader.ReadLine(sbuff, 1024);
		if (Text::StrCSVSplitP(sarr, 10, sbuff) == 9)
		{
			Data::Timestamp startTime = Data::Timestamp::FromStr(sarr[1].ToCString(), Data::DateTimeUtil::GetLocalTzQhr());
			Text::String *lastTime = 0;
			NEW_CLASS(track, Map::GPSTrack(fd->GetFullName(), altCol != INVALID_INDEX, this->codePage, 0));
			track->SetTrackName(fd->GetShortName());
			
			while (true)
			{
				if (lastTime == 0 || !lastTime->Equals(sarr[1]))
				{
					SDEL_STRING(lastTime);
					lastTime = Text::String::New(sarr[1].ToCString()).Ptr();
					rec.recTime = Data::Timestamp::FromStr(lastTime->ToCString(), Data::DateTimeUtil::GetLocalTzQhr()).inst;
				}
				else
				{
					rec.recTime = startTime.AddSecondDbl(sarr[2].ToDoubleOr(0)).inst;
				}
				rec.pos = Math::Coord2DDbl(sarr[4].ToDoubleOrNAN(), sarr[3].ToDoubleOrNAN());
				rec.valid = true;
				rec.altitude = sarr[7].ToDoubleOr(0);
				rec.speed = sarr[6].ToDoubleOr(0);
				rec.heading = 0;
				rec.nSateUsedGPS = 0;
				rec.nSateViewGPS = 0;
				rec.nSateUsed = 0;
				rec.nSateUsedGLO = 0;
				rec.nSateUsedSBAS = 0;
				rec.nSateViewGLO = 0;
				rec.nSateViewGA = 0;
				rec.nSateViewQZSS = 0;
				rec.nSateViewBD = 0;
				track->AddRecord(rec);
				if (reader.ReadLine(sbuff, 1024) == 0)
				{
					break;
				}
				if (Text::StrCSVSplitP(sarr, 10, sbuff) != 9)
				{
					break;
				}
			}
			SDEL_STRING(lastTime);
			return track;		
		}
		else
		{
			return 0;
		}
	}
	else if (((dateCol != INVALID_INDEX && timeCol != INVALID_INDEX) || (dtCol != INVALID_INDEX)) && latCol != INVALID_INDEX && lonCol != INVALID_INDEX)
	{
		Map::GPSTrack *track;
		Map::GPSTrack::GPSRecord3 rec;
		Data::DateTime dt;
		NEW_CLASS(track, Map::GPSTrack(fd->GetFullName(), altCol != INVALID_INDEX, this->codePage, 0));
		track->SetTrackName(fd->GetShortName());
		
		tmpArr2 = MemAlloc(Text::PString, currCol + 1);
		while (reader.ReadLine(sbuff, 1024).NotNull())
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
				rec.recTime = dt.ToInstant();
				rec.pos = Math::Coord2DDbl(Text::StrToDoubleOrNAN(tmpArr2[lonCol].v), Text::StrToDoubleOrNAN(tmpArr2[latCol].v));
				if (latDirCol != INVALID_INDEX)
				{
					if (tmpArr2[latDirCol].v[0] == 'S')
						rec.pos.SetLat(-rec.pos.GetLat());
				}
				if (lonDirCol != INVALID_INDEX)
				{
					if (tmpArr2[lonDirCol].v[0] == 'W')
						rec.pos.SetLon(-rec.pos.GetLon());
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
					rec.altitude = Text::StrToDoubleOr(tmpArr2[altCol].v, 0);
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
					rec.speed = Text::StrToDoubleOr(tmpArr2[speedCol].v, 0) / 1.852;
				}
				else
				{
					rec.speed = 0;
				}
				if (headingCol != INVALID_INDEX)
				{
					rec.heading = Text::StrToDoubleOr(tmpArr2[headingCol].v, 0);
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
				track->AddRecord(rec);
			}
		}		

		MemFree(tmpArr2);
		return track;
	}
	else if (latCol != INVALID_INDEX && lonCol != INVALID_INDEX)
	{
		Map::VectorLayer *lyr;
		NN<Math::Geometry::Point> pt;
		UOSInt i;
		UOSInt nameCol = 0;

		tmpcArr2 = MemAllocArr(UnsafeArray<const UTF8Char>, currCol + 1);
		i = currCol;
		while (i-- > 0)
		{
			tmpcArr2[i] = colNames.GetItem(i).Or(U8STR(""));
			if (Text::StrEndsWithICase(tmpcArr2[i], (const UTF8Char*)"NAME") == 0)
			{
				nameCol = i;
			}
		}
		NN<Math::CoordinateSystem> csys;
		NEW_CLASS(lyr, Map::VectorLayer(Map::DRAW_LAYER_POINT, fd->GetFullName(), currCol, UnsafeArray<UnsafeArrayOpt<const UTF8Char>>::ConvertFrom(tmpcArr2), csys = Math::CoordinateSystemManager::CreateWGS84Csys(), nameCol, 0));
		
		UnsafeArray<UnsafeArray<UTF8Char>> tmpUArr2 = UnsafeArray<UnsafeArray<UTF8Char>>::ConvertFrom(tmpcArr2);
		while (reader.ReadLine(sbuff, 1024).NotNull())
		{
			if ((UOSInt)currCol == Text::StrCSVSplit(tmpUArr2, currCol + 1, sbuff))
			{
				NEW_CLASSNN(pt, Math::Geometry::Point(csys->GetSRID(), Text::StrToDoubleOrNAN(tmpUArr2[lonCol]), Text::StrToDoubleOrNAN(tmpUArr2[latCol])));
				lyr->AddVector2(pt, UnsafeArray<UnsafeArrayOpt<const UTF8Char>>::ConvertFrom(tmpUArr2));
			}
		}

		MemFreeArr(tmpcArr2);
		return lyr;
	}
	else
	{
		if (colNames.GetCount() > 1)
		{
			if (fd->IsFullFile())
			{
				DB::CSVFile *csv;
				NEW_CLASS(csv, DB::CSVFile(fd->GetFullFileName(), this->codePage));
				return csv;
			}
			else
			{
				DB::CSVFile *csv;
				NEW_CLASS(csv, DB::CSVFile(fd, this->codePage));
				return csv;
			}
		}
		else
		{
			return 0;
		}
	}
}
