#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ArrayListNN.hpp"
#include "Data/FastMapObj.hpp"
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

void Parser::FileParser::SPREDParser::PrepareSelector(NN<IO::FileSelector> selector, IO::ParserType t)
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

Optional<IO::ParsedObject> Parser::FileParser::SPREDParser::ParseFileHdr(NN<IO::StreamData> fd, Optional<IO::PackageFile> pkgFile, IO::ParserType targetType, Data::ByteArrayR hdr)
{
	Data::ArrayListNN<Map::GPSTrack::GPSRecord3> *currDev = 0;
	Int32 currDevId = -1;
	Int32 devId;
	NN<Map::GPSTrack::GPSRecord3> rec;
	UInt8 buff[384];
	Bool error = false;
	UTF8Char sbuff[256];
	UnsafeArray<const UTF8Char> sptr;
	UIntOS i;
	UIntOS currPos;
	UIntOS readSize;
	UIntOS buffSize;
	UInt64 fileSize;
	Int32 cmdType;
	UInt32 cmdSize;
	if (hdr[0] != 'R' || hdr[1] != 'D')
	{
		return nullptr;
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
		return nullptr;
	}
	if (hdr[cmdSize] != 'R' || hdr[cmdSize + 1] != 'D')
	{
		return nullptr;
	}

	NN<Text::String> s = fd->GetFullName();
	i = Text::StrLastIndexOfCharC(s->v, s->leng, IO::Path::PATH_SEPERATOR);
	sptr = Text::StrConcatC(sbuff, &s->v[i + 1], s->leng - i - 1);
	if (!Text::StrStartsWithICaseC(sbuff, (UIntOS)(sptr - sbuff), UTF8STRC("RED")))
	{
		return nullptr;
	}
	i = Text::StrIndexOfChar(sbuff, '.');
	if (i == INVALID_INDEX)
		return nullptr;
	if (sbuff[i - 1] != 's' && sbuff[i - 1] != 'S')
		return nullptr;
	if (!Text::StrEqualsICaseC(&sbuff[i + 1], (UIntOS)(sptr - &sbuff[i + 1]), UTF8STRC("DAT")))
		return nullptr;

	fileSize = fd->GetDataSize();
	currPos = 0;

	Data::FastMapObj<Int32, Data::ArrayListNN<Map::GPSTrack::GPSRecord3>*> devRecs;
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
						NEW_CLASS(currDev, Data::ArrayListNN<Map::GPSTrack::GPSRecord3>());
						devRecs.Put(devId, currDev);
					}
					currDevId = devId;
				}
				rec = MemAllocANN(Map::GPSTrack::GPSRecord3);
				rec->pos.SetLat((*(Int32*)&buff[i + 8]) / 200000.0);
				rec->pos.SetLon((*(Int32*)&buff[i + 12]) / 200000.0);
				rec->speed = *(Int32*)&buff[i + 16] * 0.0001;
				rec->heading = *(UInt16*)&buff[i + 20] * 0.01;
				rec->recTime = Data::TimeInstant(ReadUInt32(&buff[i + 22]), 0);
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
		readSize = fd->GetRealData(currPos, 384 - buffSize, BYTEARR(buff).SubArray(buffSize));
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
			Data::ArrayIterator<NN<Map::GPSTrack::GPSRecord3>> it = currDev->Iterator();
			while (it.HasNext())
			{
				MemFreeA(it.Next().Ptr());
			}
			DEL_CLASS(currDev);
		}
		return nullptr;
	}

	Map::GPSTrack *track;
	NEW_CLASS(track, Map::GPSTrack(fd->GetFullName(), true, 0, nullptr));
	i = devRecs.GetCount();
	while (i-- > 0)
	{
		devId = devRecs.GetKey(i);
		sptr = Text::StrInt32(sbuff, devId);
		track->SetTrackName(CSTRP(sbuff, sptr));

		currDev = devRecs.Get(devId);
		Data::ArrayIterator<NN<Map::GPSTrack::GPSRecord3>> it = currDev->Iterator();
		while (it.HasNext())
		{
			track->AddRecord(rec = it.Next());
			MemFreeA(rec.Ptr());
		}
		DEL_CLASS(currDev);
		track->NewTrack();
	}
	return track;
}
