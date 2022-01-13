#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/DateTime.h"
#include "DB/CSVFile.h"
#include "IO/StreamDataStream.h"
#include "IO/StreamReader.h"
#include "Map/GPSTrack.h"
#include "Map/VectorLayer.h"
#include "Math/CoordinateSystemManager.h"
#include "Math/Point.h"
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

void Parser::FileParser::CSVParser::PrepareSelector(IO::IFileSelector *selector, IO::ParserType t)
{
	if (t == IO::ParserType::Unknown || t == IO::ParserType::MapLayer || t == IO::ParserType::ReadingDB)
	{
		selector->AddFilter((const UTF8Char*)"*.csv", (const UTF8Char*)"CSV File");
	}
}

IO::ParserType Parser::FileParser::CSVParser::GetParserType()
{
	return IO::ParserType::Unknown;
}

IO::ParsedObject *Parser::FileParser::CSVParser::ParseFile(IO::IStreamData *fd, IO::PackageFile *pkgFile, IO::ParserType targetType)
{
	UTF8Char sbuff[1024];
	UTF8Char sbuff2[64];
	UTF8Char *tmpArr[2];
	const UTF8Char **tmpcArr2;
	UTF8Char **tmpArr2;
	UOSInt colCnt;
	UOSInt currCol;
	Data::ArrayListStrUTF8 *colNames;

	if (!fd->GetFullFileName()->EndsWithICase(UTF8STRC(".CSV")))
		return 0;

	UOSInt i;
	IO::StreamDataStream *stm;
	IO::StreamReader *reader;
	NEW_CLASS(stm, IO::StreamDataStream(fd));
	NEW_CLASS(reader, IO::StreamReader(stm, this->codePage));

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

	NEW_CLASS(colNames, Data::ArrayListStrUTF8());
	reader->ReadLine(sbuff, 1024);
	colCnt = Text::StrCSVSplit(tmpArr, 2, sbuff);
	currCol = 0;
	while (true)
	{
		colNames->Add(tmpArr[0]);
		
		if (Text::StrCompareICase(tmpArr[0], (const UTF8Char*)"UTC DATE") == 0)
		{
			dateCol = currCol;
		}
		else if (Text::StrCompareICase(tmpArr[0], (const UTF8Char*)"UTC TIME") == 0)
		{
			timeCol = currCol;
		}
		else if (Text::StrCompareICase(tmpArr[0], (const UTF8Char*)"TS") == 0)
		{
			dtCol = currCol;
		}
		else if (Text::StrCompareICase(tmpArr[0], (const UTF8Char*)"VALID") == 0)
		{
			validCol = currCol;
		}
		else if (Text::StrCompareICase(tmpArr[0], (const UTF8Char*)"LATITUDE") == 0)
		{
			latCol = currCol;
		}
		else if (Text::StrCompareICase(tmpArr[0], (const UTF8Char*)"LONGITUDE") == 0)
		{
			lonCol = currCol;
		}
		else if (Text::StrCompareICase(tmpArr[0], (const UTF8Char*)"MAPX") == 0)
		{
			lonCol = currCol;
		}
		else if (Text::StrCompareICase(tmpArr[0], (const UTF8Char*)"MAPY") == 0)
		{
			latCol = currCol;
		}
		else if (Text::StrCompareICase(tmpArr[0], (const UTF8Char*)"RESULTX") == 0)
		{
			lonCol = currCol;
		}
		else if (Text::StrCompareICase(tmpArr[0], (const UTF8Char*)"RESULTY") == 0)
		{
			latCol = currCol;
		}
		else if (Text::StrCompareICase(tmpArr[0], (const UTF8Char*)"LAT") == 0)
		{
			latCol = currCol;
		}
		else if (Text::StrCompareICase(tmpArr[0], (const UTF8Char*)"LON") == 0)
		{
			lonCol = currCol;
		}
		else if (Text::StrCompareICase(tmpArr[0], (const UTF8Char*)"LNG") == 0)
		{
			lonCol = currCol;
		}
		else if (Text::StrCompareICase(tmpArr[0], (const UTF8Char*)"N/S") == 0)
		{
			latDirCol = currCol;
		}
		else if (Text::StrCompareICase(tmpArr[0], (const UTF8Char*)"E/W") == 0)
		{
			lonDirCol = currCol;
		}
		else if (Text::StrCompareICase(tmpArr[0], (const UTF8Char*)"HEIGHT") == 0)
		{
			altCol = currCol;
		}
		else if (Text::StrCompareICase(tmpArr[0], (const UTF8Char*)"SPEED") == 0)
		{
			speedCol = currCol;
		}
		else if (Text::StrCompareICase(tmpArr[0], (const UTF8Char*)"HEADING") == 0)
		{
			headingCol = currCol;
		}
		else if (Text::StrCompareICase(tmpArr[0], (const UTF8Char*)"NSAT(USED/VIEW)") == 0)
		{
			nSateCol = currCol;
		}
		currCol += 1;
		if (colCnt < 2)
			break;
		colCnt = Text::StrCSVSplit(tmpArr, 2, tmpArr[1]);
	}

	if (((dateCol != INVALID_INDEX && timeCol != INVALID_INDEX) || (dtCol != INVALID_INDEX)) && latCol != INVALID_INDEX && lonCol != INVALID_INDEX)
	{
		Map::GPSTrack *track;
		Map::GPSTrack::GPSRecord rec;
		Data::DateTime dt;
		NEW_CLASS(track, Map::GPSTrack(fd->GetFullName(), altCol != INVALID_INDEX, this->codePage, 0));
		track->SetTrackName(fd->GetShortName());
		
		tmpArr2 = MemAlloc(UTF8Char*, currCol + 1);
		while (reader->ReadLine(sbuff, 1024))
		{
			if ((UOSInt)currCol == Text::StrCSVSplit(tmpArr2, currCol + 1, sbuff))
			{
				if (dtCol != INVALID_INDEX)
				{
					dt.SetValue(tmpArr2[dtCol]);
				}
				else
				{
					Text::StrConcat(Text::StrConcatC(Text::StrConcat(sbuff2, tmpArr2[dateCol]), UTF8STRC(" ")), tmpArr2[timeCol]);
					dt.SetValue(sbuff2);
				}
				rec.utcTimeTicks = dt.ToTicks();
				rec.lat = Text::StrToDouble(tmpArr2[latCol]);
				rec.lon = Text::StrToDouble(tmpArr2[lonCol]);
				if (latDirCol != INVALID_INDEX)
				{
					if (tmpArr2[latDirCol][0] == 'S')
						rec.lat = -rec.lat;
				}
				if (lonDirCol != INVALID_INDEX)
				{
					if (tmpArr2[lonDirCol][0] == 'W')
						rec.lon = -rec.lon;
				}
				if (validCol != INVALID_INDEX)
				{
					if (Text::StrCompareICase(tmpArr2[validCol], (const UTF8Char*)"DGPS"))
					{
						rec.valid = 2;
					}
					else if (Text::StrCompareICase(tmpArr2[validCol], (const UTF8Char*)"SPS"))
					{
						rec.valid = 1;
					}
					else
					{
						rec.valid = 0;
					}
				}
				else
				{
					rec.valid = 0;
				}
				if (altCol != INVALID_INDEX)
				{
					i = Text::StrIndexOf(tmpArr2[altCol], (const UTF8Char*)" ");
					if (i != INVALID_INDEX)
					{
						tmpArr2[altCol][i] = 0;
					}
					rec.altitude = Text::StrToDouble(tmpArr2[altCol]);
				}
				else
				{
					rec.altitude = 0;
				}
				if (speedCol != INVALID_INDEX)
				{
					i = Text::StrIndexOf(tmpArr2[speedCol], (const UTF8Char*)" ");
					if (i != INVALID_INDEX)
					{
						tmpArr2[speedCol][i] = 0;
					}
					rec.speed = Text::StrToDouble(tmpArr2[speedCol]) / 1.852;
				}
				else
				{
					rec.speed = 0;
				}
				if (headingCol != INVALID_INDEX)
				{
					rec.heading = Text::StrToDouble(tmpArr2[headingCol]);
				}
				else
				{
					rec.heading = 0;
				}
				if (nSateCol != INVALID_INDEX)
				{
					i = Text::StrIndexOf(tmpArr2[nSateCol], (const UTF8Char*)"/");
					if (i != INVALID_INDEX)
					{
						tmpArr2[nSateCol][i] = 0;
						Text::StrTrim(tmpArr2[nSateCol]);
						Text::StrTrim(&tmpArr2[nSateCol][i + 1]);
						rec.nSateUsed = Text::StrToInt32(tmpArr2[nSateCol]);
						rec.nSateView = Text::StrToInt32(&tmpArr2[nSateCol][i + 1]);
					}
					else
					{
						rec.nSateUsed = 0;
						rec.nSateView = 0;
					}
				}
				else
				{
					rec.nSateUsed = 0;
					rec.nSateView = 0;
				}
				track->AddRecord(&rec);
			}
		}		

		MemFree(tmpArr2);
		DEL_CLASS(reader);
		DEL_CLASS(stm);
		DEL_CLASS(colNames);
		return track;
	}
	else if (latCol != INVALID_INDEX && lonCol != INVALID_INDEX)
	{
		Map::VectorLayer *lyr;
		Math::Point *pt;
		UOSInt i;
		UOSInt nameCol = 0;

		tmpcArr2 = MemAlloc(const UTF8Char*, currCol + 1);
		i = currCol;
		while (i-- > 0)
		{
			tmpcArr2[i] = colNames->GetItem(i);
			if (Text::StrEndsWithICase(tmpcArr2[i], (const UTF8Char*)"NAME") == 0)
			{
				nameCol = i;
			}
		}
		Math::CoordinateSystem *csys;
		NEW_CLASS(lyr, Map::VectorLayer(Map::DRAW_LAYER_POINT, fd->GetFullName(), currCol, tmpcArr2, csys = Math::CoordinateSystemManager::CreateGeogCoordinateSystemDefName(Math::CoordinateSystemManager::GCST_WGS84), nameCol, 0));
		
		tmpArr2 = (UTF8Char**)tmpcArr2;
		while (reader->ReadLine(sbuff, 1024))
		{
			if ((UOSInt)currCol == Text::StrCSVSplit(tmpArr2, currCol + 1, sbuff))
			{
				NEW_CLASS(pt, Math::Point(csys->GetSRID(), Text::StrToDouble(tmpArr2[lonCol]), Text::StrToDouble(tmpArr2[latCol])));
				lyr->AddVector(pt, (const UTF8Char**)tmpArr2);
			}
		}		

		MemFree(tmpcArr2);
		DEL_CLASS(reader);
		DEL_CLASS(stm);
		DEL_CLASS(colNames);
		return lyr;
	}
	else
	{
		if (colNames->GetCount() > 1 && fd->IsFullFile())
		{
			DEL_CLASS(reader);
			DEL_CLASS(stm);
			DEL_CLASS(colNames);
			DB::CSVFile *csv;
			NEW_CLASS(csv, DB::CSVFile(fd->GetFullFileName(), this->codePage));
			return csv;
		}
		else
		{
			DEL_CLASS(reader);
			DEL_CLASS(stm);
			DEL_CLASS(colNames);
			return 0;
		}
	}
}
