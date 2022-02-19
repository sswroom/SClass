#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ArrayList.h"
#include "Data/Int32Map.h"
#include "IO/Path.h"
#include "Text/MyString.h"
#include "Map/GPSTrack.h"
#include "Parser/FileParser/SPREDParser.h"

Parser::FileParser::SPREDParser::SPREDParser()
{
}

Parser::FileParser::SPREDParser::~SPREDParser()
{
}

Int32 Parser::FileParser::SPREDParser::GetName()
{
	return *(Int32*)"SRED";
}

void Parser::FileParser::SPREDParser::PrepareSelector(IO::IFileSelector *selector, IO::ParserType t)
{
	if (t == IO::ParserType::Unknown || t == IO::ParserType::MapLayer)
	{
		selector->AddFilter(CSTR("RED*.dat"), CSTR("Skypower Redirect Data"));
	}
}

IO::ParserType Parser::FileParser::SPREDParser::GetParserType()
{
	return IO::ParserType::MapLayer;
}

IO::ParsedObject *Parser::FileParser::SPREDParser::ParseFile(IO::IStreamData *fd, IO::PackageFile *pkgFile, IO::ParserType targetType)
{
	Data::Int32Map<Data::ArrayList<Map::GPSTrack::GPSRecord2*>*> *devRecs = 0;
	Data::ArrayList<Map::GPSTrack::GPSRecord2*> *currDev = 0;
	Int32 currDevId = -1;
	Int32 devId;
	Map::GPSTrack::GPSRecord2 *rec;
	UInt8 buff[384];
	Bool error = false;
	UTF8Char u8buff[256];
	const UTF8Char *sptr;
	UOSInt i;
	UOSInt j;
	UOSInt k;
	UOSInt currPos;
	UOSInt readSize;
	UOSInt buffSize;
	UInt64 fileSize;
	Int32 cmdType;
	UInt32 cmdSize;
	Text::String *s = fd->GetFullName();
	i = Text::StrLastIndexOfCharC(s->v, s->leng, IO::Path::PATH_SEPERATOR);
	sptr = Text::StrConcatC(u8buff, &s->v[i + 1], s->leng - i - 1);
	if (!Text::StrStartsWithICaseC(u8buff, (UOSInt)(sptr - u8buff), UTF8STRC("RED")))
	{
		return 0;
	}
	i = Text::StrIndexOfChar(u8buff, '.');
	if (i == INVALID_INDEX)
		return 0;
	if (u8buff[i - 1] != 's' && u8buff[i - 1] != 'S')
		return 0;
	if (!Text::StrEqualsICaseC(&u8buff[i + 1], (UOSInt)(sptr - &u8buff[i + 1]), UTF8STRC("DAT")))
		return 0;

	fileSize = fd->GetDataSize();
	currPos = 0;
	buffSize = fd->GetRealData(0, 384, buff);

	if (buff[0] != 'R' || buff[1] != 'D')
	{
		return 0;
	}
	
	cmdType = *(UInt16*)&buff[2];
	if (cmdType == 0)
	{
		cmdSize = 82;
	}
	else if (cmdType == 1)
	{
		cmdSize = 4;
	}
	else if (cmdType == 2)
	{
		cmdSize = 20;
	}
	else
	{
		return 0;
	}
	if (buff[cmdSize] != 'R' || buff[cmdSize + 1] != 'D')
	{
		return 0;
	}
	NEW_CLASS(devRecs, Data::Int32Map<Data::ArrayList<Map::GPSTrack::GPSRecord2*>*>());
	currPos += buffSize;
	while (true)
	{
		i = 0;
		while (i <= buffSize - 4)
		{
			if (buff[i] != 'R' || buff[i + 1] != 'D')
			{
				error = true;
				break;
			}
			cmdType = *(UInt16*)&buff[i + 2];
			if (cmdType == 0)
			{
				cmdSize = 82;
			}
			else if (cmdType == 1)
			{
				cmdSize = 4;
			}
			else if (cmdType == 2)
			{
				cmdSize = 20;
			}
			else
			{
				error = true;
				break;
			}
			if (i + cmdSize > buffSize)
			{
				break;
			}
			if (cmdType == 0)
			{
				devId = *(Int32*)&buff[i + 4];
				if (currDevId != devId)
				{
					currDev = devRecs->Get(devId);
					if (currDev == 0)
					{
						NEW_CLASS(currDev, Data::ArrayList<Map::GPSTrack::GPSRecord2*>());
						devRecs->Put(devId, currDev);
					}
					currDevId = devId;
				}
				rec = MemAlloc(Map::GPSTrack::GPSRecord2, 1);
				rec->lat = (*(Int32*)&buff[i + 8]) / 200000.0;
				rec->lon = (*(Int32*)&buff[i + 12]) / 200000.0;
				rec->speed = *(Int32*)&buff[i + 16] * 0.0001;
				rec->heading = *(UInt16*)&buff[i + 20] * 0.01;
				rec->utcTimeTicks = 1000LL * *(UInt32*)&buff[i + 22];
				rec->altitude = *(Int32*)&buff[i + 26] * 0.01;
				rec->valid = (buff[i + 43] & 0x80) >> 7;
				UInt16 nSateUsed = (*(UInt16*)&buff[i + 42]) & 0x7fff;
				if (nSateUsed & 0x4000)
				{
					rec->nSateUsedGPS = (UInt8)(nSateUsed - 0x8000);
				}
				else
				{
					rec->nSateUsedGPS = (UInt8)nSateUsed;
				}
				rec->nSateUsed = rec->nSateUsedGPS;
				rec->nSateUsedGLO = 0;
				rec->nSateUsedSBAS = 0;
				rec->nSateViewGPS = 0;
				rec->nSateViewGLO = 0;
				rec->nSateViewGA = 0;
				rec->nSateViewQZSS = 0;
				rec->nSateViewBD = 0;
				currDev->Add(rec);

			}
			i += cmdSize;
		}
		if (error)
			break;
		if (i < buffSize)
		{
			MemCopyO(buff, &buff[i], buffSize - i);
			buffSize -= i;
		}
		else
		{
			buffSize = 0;
		}
		if (currPos >= fileSize)
			break;
		readSize = fd->GetRealData(currPos, 384 - buffSize, &buff[buffSize]);
		buffSize += readSize;
		currPos += readSize;
	}
	if (error)
	{
		Data::ArrayList<Data::ArrayList<Map::GPSTrack::GPSRecord2 *> *>*recs = devRecs->GetValues();
		i = recs->GetCount();
		while (i-- > 0)
		{
			currDev = recs->GetItem(i);
			j = currDev->GetCount();
			while (j-- > 0)
			{
				rec = currDev->GetItem(j);
				MemFree(rec);
			}
			DEL_CLASS(currDev);
		}
		DEL_CLASS(devRecs);
		return 0;
	}

	Map::GPSTrack *track;
	UTF8Char sbuff[12];
	NEW_CLASS(track, Map::GPSTrack(fd->GetFullName(), true, 0, 0));
	Data::SortableArrayListNative<Int32> *keys = devRecs->GetKeys();
	i = keys->GetCount();
	while (i-- > 0)
	{
		devId = keys->GetItem(i);
		sptr = Text::StrInt32(sbuff, devId);
		track->SetTrackName(CSTRP(sbuff, sptr));

		currDev = devRecs->Get(devId);
		j = 0;
		k = currDev->GetCount();
		while (j < k)
		{
			track->AddRecord(rec = currDev->GetItem(j));
			MemFree(rec);
			j++;
		}
		DEL_CLASS(currDev);
		track->NewTrack();
	}
	return track;
}
