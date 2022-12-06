#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ArrayList.h"
#include "Data/FastMap.h"
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

IO::ParsedObject *Parser::FileParser::SPREDParser::ParseFileHdr(IO::IStreamData *fd, IO::PackageFile *pkgFile, IO::ParserType targetType, const UInt8 *hdr)
{
	Data::ArrayList<Map::GPSTrack::GPSRecord3*> *currDev = 0;
	Int32 currDevId = -1;
	Int32 devId;
	Map::GPSTrack::GPSRecord3 *rec;
	UInt8 buff[384];
	Bool error = false;
	UTF8Char sbuff[256];
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
	if (hdr[0] != 'R' || hdr[1] != 'D')
	{
		return 0;
	}
	
	cmdType = ReadUInt16(&hdr[2]);
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
	if (hdr[cmdSize] != 'R' || hdr[cmdSize + 1] != 'D')
	{
		return 0;
	}

	Text::String *s = fd->GetFullName();
	i = Text::StrLastIndexOfCharC(s->v, s->leng, IO::Path::PATH_SEPERATOR);
	sptr = Text::StrConcatC(sbuff, &s->v[i + 1], s->leng - i - 1);
	if (!Text::StrStartsWithICaseC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("RED")))
	{
		return 0;
	}
	i = Text::StrIndexOfChar(sbuff, '.');
	if (i == INVALID_INDEX)
		return 0;
	if (sbuff[i - 1] != 's' && sbuff[i - 1] != 'S')
		return 0;
	if (!Text::StrEqualsICaseC(&sbuff[i + 1], (UOSInt)(sptr - &sbuff[i + 1]), UTF8STRC("DAT")))
		return 0;

	fileSize = fd->GetDataSize();
	currPos = 0;

	Data::FastMap<Int32, Data::ArrayList<Map::GPSTrack::GPSRecord3*>*> devRecs;
	buffSize = 0;
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
					currDev = devRecs.Get(devId);
					if (currDev == 0)
					{
						NEW_CLASS(currDev, Data::ArrayList<Map::GPSTrack::GPSRecord3*>());
						devRecs.Put(devId, currDev);
					}
					currDevId = devId;
				}
				rec = MemAllocA(Map::GPSTrack::GPSRecord3, 1);
				rec->pos.lat = (*(Int32*)&buff[i + 8]) / 200000.0;
				rec->pos.lon = (*(Int32*)&buff[i + 12]) / 200000.0;
				rec->speed = *(Int32*)&buff[i + 16] * 0.0001;
				rec->heading = *(UInt16*)&buff[i + 20] * 0.01;
				rec->utcTimeTicks = 1000LL * *(UInt32*)&buff[i + 22];
				rec->altitude = *(Int32*)&buff[i + 26] * 0.01;
				rec->valid = ((buff[i + 43] & 0x80) >> 7) != 0;
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
		if (readSize == 0)
		{
			error = true;
			break;
		}
		buffSize += readSize;
		currPos += readSize;
	}
	if (error)
	{
		i = devRecs.GetCount();
		while (i-- > 0)
		{
			currDev = devRecs.GetItem(i);
			j = currDev->GetCount();
			while (j-- > 0)
			{
				rec = currDev->GetItem(j);
				MemFreeA(rec);
			}
			DEL_CLASS(currDev);
		}
		return 0;
	}

	Map::GPSTrack *track;
	NEW_CLASS(track, Map::GPSTrack(fd->GetFullName(), true, 0, 0));
	i = devRecs.GetCount();
	while (i-- > 0)
	{
		devId = devRecs.GetKey(i);
		sptr = Text::StrInt32(sbuff, devId);
		track->SetTrackName(CSTRP(sbuff, sptr));

		currDev = devRecs.Get(devId);
		j = 0;
		k = currDev->GetCount();
		while (j < k)
		{
			track->AddRecord(rec = currDev->GetItem(j));
			MemFreeA(rec);
			j++;
		}
		DEL_CLASS(currDev);
		track->NewTrack();
	}
	return track;
}
