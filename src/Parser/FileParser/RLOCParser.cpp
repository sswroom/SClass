#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ByteTool.h"
#include "IO/Path.h"
#include "Map/GPSTrack.h"
#include "Net/SocketUtil.h"
#include "Parser/FileParser/RLOCParser.h"
#include "Text/MyString.h"
#include "Text/MyStringFloat.h"

typedef struct
{
	Int32 devId;
	Int32 devType;
	Int32 funcs;
	UInt32 status;
	UInt32 status2;
	Double other0;
	Double other1;
	Double other2;
	Double other3;
	UInt32 locTimeTS;
	UInt32 recvTimeTS;
	Int64 connId;
	Int32 gtime;
	Double gpsLat;
	Double gpsLon;
} ExtraInfo;

class RLOCExtraParser : public Map::GPSTrack::GPSExtraParser
{
public:
	RLOCExtraParser()
	{
	}

	virtual ~RLOCExtraParser()
	{
	}

	virtual UOSInt GetExtraCount(UnsafeArray<const UInt8> buff, UOSInt buffSize)
	{
		return 15;
	}

	virtual Bool GetExtraName(UnsafeArray<const UInt8> buff, UOSInt buffSize, UOSInt extIndex, NN<Text::StringBuilderUTF8> sb)
	{
		switch (extIndex)
		{
		case 0:
			sb->AppendC(UTF8STRC("DevID"));
			return true;
		case 1:
			sb->AppendC(UTF8STRC("DevType"));
			return true;
		case 2:
			sb->AppendC(UTF8STRC("Funcs"));
			return true;
		case 3:
			sb->AppendC(UTF8STRC("Status"));
			return true;
		case 4:
			sb->AppendC(UTF8STRC("Status2"));
			return true;
		case 5:
			sb->AppendC(UTF8STRC("Other0"));
			return true;
		case 6:
			sb->AppendC(UTF8STRC("Other1"));
			return true;
		case 7:
			sb->AppendC(UTF8STRC("Other2"));
			return true;
		case 8:
			sb->AppendC(UTF8STRC("Other3"));
			return true;
		case 9:
			sb->AppendC(UTF8STRC("LocTime"));
			return true;
		case 10:
			sb->AppendC(UTF8STRC("RecvTime"));
			return true;
		case 11:
			sb->AppendC(UTF8STRC("ConnId"));
			return true;
		case 12:
			sb->AppendC(UTF8STRC("GTime"));
			return true;
		case 13:
			sb->AppendC(UTF8STRC("GPS Lat"));
			return true;
		case 14:
			sb->AppendC(UTF8STRC("GPS Lon"));
			return true;
		}
		return false;
	}

	virtual Bool GetExtraValueStr(UnsafeArray<const UInt8> buff, UOSInt buffSize, UOSInt extIndex, NN<Text::StringBuilderUTF8> sb)
	{
		if (buffSize != sizeof(ExtraInfo))
			return false;
		UnsafeArray<const ExtraInfo> extInfo = UnsafeArray<const ExtraInfo>::ConvertFrom(buff);

		switch (extIndex)
		{
		case 0:
			sb->AppendI32(extInfo->devId);
			return true;
		case 1:
			sb->AppendI32(extInfo->devType);
			return true;
		case 2:
			sb->AppendI32(extInfo->funcs);
			return true;
		case 3:
			sb->AppendC(UTF8STRC("0x"));
			sb->AppendHex32(extInfo->status);
			return true;
		case 4:
			sb->AppendC(UTF8STRC("0x"));
			sb->AppendHex32(extInfo->status2);
			return true;
		case 5:
			Text::SBAppendF64(sb, extInfo->other0);
			return true;
		case 6:
			Text::SBAppendF64(sb, extInfo->other1);
			return true;
		case 7:
			Text::SBAppendF64(sb, extInfo->other2);
			return true;
		case 8:
			Text::SBAppendF64(sb, extInfo->other3);
			return true;
		case 9:
			sb->AppendTSNoZone(Data::Timestamp::FromEpochSec(extInfo->locTimeTS, Data::DateTimeUtil::GetLocalTzQhr()));
			return true;
		case 10:
			sb->AppendTSNoZone(Data::Timestamp::FromEpochSec(extInfo->recvTimeTS, Data::DateTimeUtil::GetLocalTzQhr()));
			return true;
		case 11:
			{
				UTF8Char sbuff[32];
				UnsafeArray<UTF8Char> sptr;
				sptr = Net::SocketUtil::GetIPv4Name(sbuff, (UInt32)(extInfo->connId >> 16), (UInt16)(extInfo->connId & 0xffff));
				sb->AppendC(sbuff, (UOSInt)(sptr - sbuff));
			}
			return true;
		case 12:
			sb->AppendI32(extInfo->gtime);
			return true;
		case 13:
			Text::SBAppendF64(sb, extInfo->gpsLat);
			return true;
		case 14:
			Text::SBAppendF64(sb, extInfo->gpsLon);
			return true;
		}
		return false;
	}
};


Parser::FileParser::RLOCParser::RLOCParser()
{
}

Parser::FileParser::RLOCParser::~RLOCParser()
{
}

Int32 Parser::FileParser::RLOCParser::GetName()
{
	return *(Int32*)"RLOC";
}

void Parser::FileParser::RLOCParser::PrepareSelector(NN<IO::FileSelector> selector, IO::ParserType t)
{
	if (t == IO::ParserType::Unknown || t == IO::ParserType::MapLayer)
	{
		selector->AddFilter(CSTR("*.dat"), CSTR("Rodsum Location File"));
	}
}

IO::ParserType Parser::FileParser::RLOCParser::GetParserType()
{
	return IO::ParserType::MapLayer;
}

Optional<IO::ParsedObject> Parser::FileParser::RLOCParser::ParseFileHdr(NN<IO::StreamData> fd, Optional<IO::PackageFile> pkgFile, IO::ParserType targetType, Data::ByteArrayR hdr)
{
	Map::GPSTrack::GPSRecord3 rec;
	UInt8 buff[384];
	UTF8Char sbuff[256];
	UnsafeArray<UTF8Char> sptr;
	UOSInt i;
	UOSInt currPos;
	UInt64 fileSize;
	Int32 devId;
	NN<Text::String> s = fd->GetFullName();
	i = s->LastIndexOf(IO::Path::PATH_SEPERATOR);
	sptr = Text::StrConcatC(sbuff, &s->v[i + 1], s->leng - i - 1);
	if (!Text::StrStartsWithICaseC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("LOC")))
	{
		return 0;
	}
	i = Text::StrIndexOfChar(sbuff, '_');
	if (i == INVALID_INDEX)
	{
		i = Text::StrIndexOfChar(sbuff, '.');
		if (i == INVALID_INDEX)
			return 0;
		sbuff[i] = 0;
		devId = Text::StrToInt32(&sbuff[3]);
	}
	else
	{
		sbuff[i] = 0;
		devId = Text::StrToInt32(&sbuff[3]);
	}
	if (devId == 0)
		return 0;
	fileSize = fd->GetDataSize();
	if (fileSize & 127)
		return 0;

	if (ReadInt32(&hdr[0]) != devId || ReadInt32(&hdr[128]) != devId || ReadInt32(&hdr[256]) != devId)
		return 0;

	Map::GPSTrack *track;
	sptr = Text::StrInt32(sbuff, devId);
	s = Text::String::New(sbuff, (UOSInt)(sptr - sbuff));
	NEW_CLASS(track, Map::GPSTrack(fd->GetFullName(), true, 0, s.Ptr()));
	track->SetTrackName(s->ToCString());
	s->Release();
	RLOCExtraParser *parser;
	NEW_CLASS(parser, RLOCExtraParser());
	track->SetExtraParser(parser);
	currPos = 0;
	while (currPos < fileSize)
	{
		ExtraInfo extInfo;

		fd->GetRealData(currPos, 128, BYTEARR(buff));
		extInfo.devId = ReadInt32(&buff[0]);
		extInfo.devType = ReadInt32(&buff[4]);
		rec.speed = ReadUInt16(&buff[8]) * 0.1;
		rec.heading = ReadUInt16(&buff[10]) * 0.01;
		rec.recTime = Data::TimeInstant(ReadInt32(&buff[12]), 0);
		extInfo.funcs = ReadInt24(&buff[16]);
		rec.nSateUsedGPS = buff[19];
		rec.pos.SetLat(ReadInt32(&buff[20]) / 200000.0);
		rec.pos.SetLon(ReadInt32(&buff[24]) / 200000.0);
		extInfo.status = ReadUInt32(&buff[28]);
		extInfo.status2 = ReadUInt32(&buff[32]);
		rec.nSateViewGPS = buff[36];
		rec.valid = buff[38] & 1;
		rec.nSateUsed = rec.nSateUsedGPS;
		rec.nSateUsedGLO = 0;
		rec.nSateUsedSBAS = 0;
		rec.nSateViewGLO = 0;
		rec.nSateViewGA = 0;
		rec.nSateViewQZSS = 0;
		rec.nSateViewBD = 0;
		extInfo.other0 = ReadUInt32(&buff[40]) * 0.001;
		extInfo.other1 = ReadUInt32(&buff[44]) * 0.001;
		extInfo.other2 = ReadInt16(&buff[48]) * 0.01;
		extInfo.other3 = ReadInt16(&buff[50]) * 0.01;
		extInfo.locTimeTS = ReadUInt32(&buff[52]);
		extInfo.recvTimeTS = ReadUInt32(&buff[56]);
		extInfo.connId = ReadInt64(&buff[64]);
		extInfo.gtime = ReadInt32(&buff[72]);
		extInfo.gpsLat = ReadInt32(&buff[76]) / 200000.0;
		extInfo.gpsLon = ReadInt32(&buff[80]) / 200000.0;
		if ((*(Int16*)&buff[16] & 0x22) == 0x22)
		{
			rec.altitude = (*(Int32*)&buff[44]) * 0.001;
		}
		else
		{
			rec.altitude = 0;
		}
		i = track->AddRecord(rec);
		track->SetExtraDataIndex(i, (const UInt8*)&extInfo, sizeof(extInfo));

		currPos += 128;
	}
	track->SortRecords();
	return track;
}
