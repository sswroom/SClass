#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ByteTool.h"
#include "IO/Path.h"
#include "Map/GPSTrack.h"
#include "Net/SocketUtil.h"
#include "Parser/FileParser/GLOCParser.h"
#include "Text/MyString.h"
#include "Text/MyStringFloat.h"

typedef struct
{
	UInt32 termId;
	Int32 devType;
	UInt32 recvTimeTS;
	UInt32 status;
	UInt32 status2;
	Int32 period;
	UInt16 mileageTZ;
	Double temperature1;
	Double temperature2;
	Double temperature3;
	Double rhLine;
	Double rhRef;
	Int32 mileageDiff;
	Int32 mileageDaily;
	Double fuelLev;
	UInt32 devIP;
	UInt16 devPort;
	UInt16 recType;
	UInt32 status4;
	Int32 adc5;
	Double adc6;
	Double adc7;
	Double adc8;
	UInt64 fileOfst;
} ExtraInfo;

class GLOCExtraParser : public Map::GPSTrack::GPSExtraParser
{
public:
	GLOCExtraParser()
	{
	}

	virtual ~GLOCExtraParser()
	{
	}

	virtual UOSInt GetExtraCount(UnsafeArray<const UInt8> buff, UOSInt buffSize)
	{
		return 24;
	}

	virtual Bool GetExtraName(UnsafeArray<const UInt8> buff, UOSInt buffSize, UOSInt extIndex, NN<Text::StringBuilderUTF8> sb)
	{
		switch (extIndex)
		{
		case 0:
			sb->AppendC(UTF8STRC("TermId"));
			return true;
		case 1:
			sb->AppendC(UTF8STRC("DevType"));
			return true;
		case 2:
			sb->AppendC(UTF8STRC("RecvTime"));
			return true;
		case 3:
			sb->AppendC(UTF8STRC("Status"));
			return true;
		case 4:
			sb->AppendC(UTF8STRC("Status2"));
			return true;
		case 5:
			sb->AppendC(UTF8STRC("Period"));
			return true;
		case 6:
			sb->AppendC(UTF8STRC("MileageTZ"));
			return true;
		case 7:
			sb->AppendC(UTF8STRC("Temperature1"));
			return true;
		case 8:
			sb->AppendC(UTF8STRC("Temperature2"));
			return true;
		case 9:
			sb->AppendC(UTF8STRC("Temperature3"));
			return true;
		case 10:
			sb->AppendC(UTF8STRC("RHLine"));
			return true;
		case 11:
			sb->AppendC(UTF8STRC("RHRef"));
			return true;
		case 12:
			sb->AppendC(UTF8STRC("MileageDiff"));
			return true;
		case 13:
			sb->AppendC(UTF8STRC("MileageDaily"));
			return true;
		case 14:
			sb->AppendC(UTF8STRC("FuelLev"));
			return true;
		case 15:
			sb->AppendC(UTF8STRC("DevIP"));
			return true;
		case 16:
			sb->AppendC(UTF8STRC("DevPort"));
			return true;
		case 17:
			sb->AppendC(UTF8STRC("RecType"));
			return true;
		case 18:
			sb->AppendC(UTF8STRC("Status4"));
			return true;
		case 19:
			sb->AppendC(UTF8STRC("adc5"));
			return true;
		case 20:
			sb->AppendC(UTF8STRC("adc6"));
			return true;
		case 21:
			sb->AppendC(UTF8STRC("adc7"));
			return true;
		case 22:
			sb->AppendC(UTF8STRC("adc8"));
			return true;
		case 23:
			sb->AppendC(UTF8STRC("FileOffset"));
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
			sb->AppendU32(extInfo->termId);
			return true;
		case 1:
			sb->AppendI32(extInfo->devType);
			return true;
		case 2:
			sb->AppendTSNoZone(Data::Timestamp::FromEpochSec(extInfo->recvTimeTS, Data::DateTimeUtil::GetLocalTzQhr()));
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
			sb->AppendI32(extInfo->period);
			return true;
		case 6:
			sb->AppendU16(extInfo->mileageTZ);
			return true;
		case 7:
			Text::SBAppendF64(sb, extInfo->temperature1);
			return true;
		case 8:
			Text::SBAppendF64(sb, extInfo->temperature2);
			return true;
		case 9:
			Text::SBAppendF64(sb, extInfo->temperature3);
			return true;
		case 10:
			Text::SBAppendF64(sb, extInfo->rhLine);
			return true;
		case 11:
			Text::SBAppendF64(sb, extInfo->rhRef);
			return true;
		case 12:
			sb->AppendI32(extInfo->mileageDiff);
			return true;
		case 13:
			sb->AppendI32(extInfo->mileageDaily);
			return true;
		case 14:
			Text::SBAppendF64(sb, extInfo->fuelLev);
			return true;
		case 15:
			{
				UTF8Char sbuff[32];
				UnsafeArray<UTF8Char> sptr;
				sptr = Net::SocketUtil::GetIPv4Name(sbuff, extInfo->devIP);
				sb->AppendC(sbuff, (UOSInt)(sptr - sbuff));
			}
			return true;
		case 16:
			sb->AppendU16(extInfo->devPort);
			return true;
		case 17:
			sb->AppendU16(extInfo->recType);
			return true;
		case 18:
			sb->AppendC(UTF8STRC("0x"));
			sb->AppendHex32(extInfo->status4);
			return true;
		case 19:
			sb->AppendI32(extInfo->adc5);
			return true;
		case 20:
			Text::SBAppendF64(sb, extInfo->adc6);
			return true;
		case 21:
			Text::SBAppendF64(sb, extInfo->adc7);
			return true;
		case 22:
			Text::SBAppendF64(sb, extInfo->adc8);
			return true;
		case 23:
			sb->AppendU64(extInfo->fileOfst);
			return true;
		}
		return false;
	}
};

Parser::FileParser::GLOCParser::GLOCParser()
{
}

Parser::FileParser::GLOCParser::~GLOCParser()
{
}

Int32 Parser::FileParser::GLOCParser::GetName()
{
	return *(Int32*)"GLOC";
}

void Parser::FileParser::GLOCParser::PrepareSelector(NN<IO::FileSelector> selector, IO::ParserType t)
{
	if (t == IO::ParserType::Unknown || t == IO::ParserType::MapLayer)
	{
		selector->AddFilter(CSTR("*.dat"), CSTR("GPS Location File"));
	}
}

IO::ParserType Parser::FileParser::GLOCParser::GetParserType()
{
	return IO::ParserType::MapLayer;
}

Optional<IO::ParsedObject> Parser::FileParser::GLOCParser::ParseFileHdr(NN<IO::StreamData> fd, Optional<IO::PackageFile> pkgFile, IO::ParserType targetType, Data::ByteArrayR hdr)
{
	Map::GPSTrack::GPSRecord3 rec;
	UInt8 buff[384];
	UTF8Char sbuff[256];
	UnsafeArray<const UTF8Char> sptr;
	UnsafeArray<UTF8Char> sptr2;
	UOSInt i;
	UInt64 currPos;
	UInt64 fileSize;
	Int64 devId;
	UInt32 idevId;
	NN<Text::String> name = fd->GetFullName();
	sptr = name->v;
	i = Text::StrLastIndexOfCharC(sptr, name->leng, IO::Path::PATH_SEPERATOR);
	sptr2 = Text::StrConcatC(sbuff, &sptr[i + 1], name->leng - i - 1);
	if (!Text::StrStartsWithICaseC(sbuff, (UOSInt)(sptr2 - sbuff), UTF8STRC("GLOC")))
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
		devId = Text::StrToInt64(&sbuff[4]);
	}
	else
	{
		sbuff[i] = 0;
		devId = Text::StrToInt64(&sbuff[4]);
	}
	if (devId == 0)
		return 0;
	fileSize = fd->GetDataSize();
	if (fileSize & 127)
		return 0;
	idevId = (UInt32)(devId & 0xffffffffLL);

	if (ReadUInt32(&hdr[0]) != idevId || (fileSize > 128 && ReadUInt32(&hdr[128]) != idevId) || (fileSize > 256 && ReadUInt32(&hdr[256]) != idevId))
		return 0;

	Map::GPSTrack *track;
	sptr = Text::StrInt64(sbuff, devId);
	NEW_CLASS(track, Map::GPSTrack(fd->GetFullName()->ToCString(), true, 0, CSTRP(sbuff, sptr)));
	track->SetTrackName(CSTRP(sbuff, sptr));
	GLOCExtraParser *parser;
	NEW_CLASS(parser, GLOCExtraParser());
	track->SetExtraParser(parser);
	currPos = 0;
	while (currPos < fileSize)
	{
		ExtraInfo extInfo;
		fd->GetRealData(currPos, 128, BYTEARR(buff));
		extInfo.termId = ReadUInt32(&buff[0]);
		extInfo.devType = ReadInt32(&buff[4]);
		rec.recTime = Data::TimeInstant(ReadUInt32(&buff[8]), 0);
		extInfo.recvTimeTS = ReadUInt32(&buff[12]);
		rec.pos = Math::Coord2DDbl(ReadInt32(&buff[20]) / 10000000.0,  ReadInt32(&buff[16]) / 10000000.0);
		rec.speed = ReadUInt16(&buff[24]) * 0.1;
		rec.heading = ReadUInt16(&buff[26]) * 0.01;
		extInfo.status = ReadUInt32(&buff[28]);
		extInfo.status2 = ReadUInt32(&buff[32]);
		extInfo.period = ReadInt32(&buff[36]);
		rec.valid = (buff[32] & 1) == 0;
		rec.nSateUsed = buff[40];
		rec.nSateUsedGPS = rec.nSateUsed;
		rec.nSateUsedGLO = 0;
		rec.nSateUsedSBAS = 0;
		rec.nSateViewGPS = buff[41];
		rec.nSateViewGLO = 0;
		rec.nSateViewGA = 0;
		rec.nSateViewQZSS = 0;
		rec.nSateViewBD = 0;
		extInfo.mileageTZ = ReadUInt16(&buff[42]);
		extInfo.temperature1 = ReadInt32(&buff[44]) * 0.01;
		extInfo.temperature2 = ReadInt32(&buff[48]) * 0.01;
		extInfo.temperature3 = ReadInt32(&buff[52]) * 0.01;
		extInfo.rhLine = ReadInt32(&buff[56]) * 0.0001;
		extInfo.rhRef = ReadInt32(&buff[60]) * 0.0001;
		extInfo.mileageDiff = ReadInt32(&buff[64]);
		extInfo.mileageDaily = ReadInt32(&buff[68]);
		extInfo.fuelLev = ReadInt32(&buff[72]) * 0.0001;
		extInfo.devIP = ReadNUInt32(&buff[76]);
		extInfo.devPort = ReadUInt16(&buff[80]);
		extInfo.recType = ReadUInt16(&buff[82]);
		rec.altitude = ReadInt32(&buff[84]) / 1000.0;
		extInfo.status4 = ReadUInt32(&buff[88]);
		extInfo.adc5 = ReadInt32(&buff[92]);
		extInfo.adc6 = ReadInt32(&buff[96]) * 0.001;
		extInfo.adc7 = ReadInt32(&buff[100]) * 0.001;
		extInfo.adc8 = ReadInt32(&buff[104]) * 0.001;
		extInfo.fileOfst = currPos;
		i = track->AddRecord(rec);
		track->SetExtraDataIndex(i, (UInt8*)&extInfo, sizeof(extInfo));
		currPos += 128;
	}
	track->SortRecords();
	return track;
}
