#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ByteTool.h"
#include "IO/Path.h"
#include "Map/GPSTrack.h"
#include "Net/SocketUtil.h"
#include "Parser/FileParser/SLOCParser.h"
#include "Text/MyString.h"
#include "Text/MyStringFloat.h"

typedef struct
{
	Int64 termId;
	UInt32 status;
	UInt32 inStatus;
	UInt32 outStatus;
	UInt8 fixMode;
	UInt8 gpsFix;
	Double pdop;
	Double hdop;
	Double vdop;
	Int32 adc1;
	Int32 adc2;
	Int32 devType;
	Double temper1;
	Double temper2;
	Double temper3;
	UInt32 cliIP;
	UInt16 cliPort;
	UInt32 recvTimeTS;
} ExtraInfo;

class SLOCExtraParser : public Map::GPSTrack::GPSExtraParser
{
public:
	SLOCExtraParser()
	{
	}

	virtual ~SLOCExtraParser()
	{
	}

	virtual UOSInt GetExtraCount(const UInt8 *buff, UOSInt buffSize)
	{
		return 18;
	}

	virtual Bool GetExtraName(const UInt8 *buff, UOSInt buffSize, UOSInt extIndex, NotNullPtr<Text::StringBuilderUTF8> sb)
	{
		switch (extIndex)
		{
		case 0:
			sb->AppendC(UTF8STRC("TermId"));
			return true;
		case 1:
			sb->AppendC(UTF8STRC("Status"));
			return true;
		case 2:
			sb->AppendC(UTF8STRC("InStatus"));
			return true;
		case 3:
			sb->AppendC(UTF8STRC("OutStatus"));
			return true;
		case 4:
			sb->AppendC(UTF8STRC("FixMode"));
			return true;
		case 5:
			sb->AppendC(UTF8STRC("GPSFix"));
			return true;
		case 6:
			sb->AppendC(UTF8STRC("PDOP"));
			return true;
		case 7:
			sb->AppendC(UTF8STRC("HDOP"));
			return true;
		case 8:
			sb->AppendC(UTF8STRC("VDOP"));
			return true;
		case 9:
			sb->AppendC(UTF8STRC("ADC1"));
			return true;
		case 10:
			sb->AppendC(UTF8STRC("ADC2"));
			return true;
		case 11:
			sb->AppendC(UTF8STRC("DevType"));
			return true;
		case 12:
			sb->AppendC(UTF8STRC("Temper1"));
			return true;
		case 13:
			sb->AppendC(UTF8STRC("Temper2"));
			return true;
		case 14:
			sb->AppendC(UTF8STRC("Temper3"));
			return true;
		case 15:
			sb->AppendC(UTF8STRC("CliIP"));
			return true;
		case 16:
			sb->AppendC(UTF8STRC("CliPort"));
			return true;
		case 17:
			sb->AppendC(UTF8STRC("RecvTime"));
			return true;
		}
		return false;
	}

	virtual Bool GetExtraValueStr(const UInt8 *buff, UOSInt buffSize, UOSInt extIndex, NotNullPtr<Text::StringBuilderUTF8> sb)
	{
		if (buffSize != sizeof(ExtraInfo))
			return false;
		const ExtraInfo *extInfo = (const ExtraInfo*)buff;

		switch (extIndex)
		{
		case 0:
			sb->AppendI64(extInfo->termId);
			return true;
		case 1:
			sb->AppendC(UTF8STRC("0x"));
			sb->AppendHex32(extInfo->status);
			return true;
		case 2:
			sb->AppendC(UTF8STRC("0x"));
			sb->AppendHex32(extInfo->inStatus);
			return true;
		case 3:
			sb->AppendC(UTF8STRC("0x"));
			sb->AppendHex32(extInfo->outStatus);
			return true;
		case 4:
			sb->AppendU16(extInfo->fixMode);
			return true;
		case 5:
			sb->AppendU16(extInfo->gpsFix);
			return true;
		case 6:
			Text::SBAppendF64(sb, extInfo->pdop);
			return true;
		case 7:
			Text::SBAppendF64(sb, extInfo->hdop);
			return true;
		case 8:
			Text::SBAppendF64(sb, extInfo->vdop);
			return true;
		case 9:
			sb->AppendI32(extInfo->adc1);
			return true;
		case 10:
			sb->AppendI32(extInfo->adc2);
			return true;
		case 11:
			sb->AppendI32(extInfo->devType);
			return true;
		case 12:
			Text::SBAppendF64(sb, extInfo->temper1);
			return true;
		case 13:
			Text::SBAppendF64(sb, extInfo->temper2);
			return true;
		case 14:
			Text::SBAppendF64(sb, extInfo->temper3);
			return true;
		case 15:
			{
				UTF8Char sbuff[32];
				UTF8Char *sptr;
				sptr = Net::SocketUtil::GetIPv4Name(sbuff, extInfo->cliIP);
				sb->AppendC(sbuff, (UOSInt)(sptr - sbuff));
			}
			return true;
		case 16:
			sb->AppendU16(extInfo->cliPort);
			return true;
		case 17:
			{
				Data::DateTime dt;
				dt.SetUnixTimestamp(extInfo->recvTimeTS);
				dt.ToLocalTime();
				sb->AppendDate(&dt);
			}
			return true;
		}
		return false;
	}
};

Parser::FileParser::SLOCParser::SLOCParser()
{
}

Parser::FileParser::SLOCParser::~SLOCParser()
{
}

Int32 Parser::FileParser::SLOCParser::GetName()
{
	return *(Int32*)"SLOC";
}

void Parser::FileParser::SLOCParser::PrepareSelector(IO::FileSelector *selector, IO::ParserType t)
{
	if (t == IO::ParserType::Unknown || t == IO::ParserType::MapLayer)
	{
		selector->AddFilter(CSTR("*.dat"), CSTR("SP Location File"));
	}
}

IO::ParserType Parser::FileParser::SLOCParser::GetParserType()
{
	return IO::ParserType::MapLayer;
}

IO::ParsedObject *Parser::FileParser::SLOCParser::ParseFileHdr(NotNullPtr<IO::StreamData> fd, IO::PackageFile *pkgFile, IO::ParserType targetType, const UInt8 *hdr)
{
	Map::GPSTrack::GPSRecord3 rec;
	UInt8 buff[384];
	UTF8Char sbuff[256];
	const UTF8Char *sptr;
	UOSInt i;
	UOSInt currPos;
	UInt64 fileSize;
	Int64 devId;
	NotNullPtr<Text::String> s = fd->GetFullName();
	i = Text::StrLastIndexOfCharC(s->v, s->leng, IO::Path::PATH_SEPERATOR);
	sptr = Text::StrConcatC(sbuff, &s->v[i + 1], s->leng - i - 1);
	if (!Text::StrStartsWithICaseC(sbuff, (UOSInt)(sptr - sbuff), UTF8STRC("LOC")))
	{
		return 0;
	}
	i = Text::StrIndexOfChar(sbuff, '_');
	if (i == INVALID_INDEX)
		return 0;
	sbuff[i] = 0;
	devId = Text::StrToInt64(&sbuff[3]);
	if (devId == 0)
		return 0;
	fileSize = fd->GetDataSize();
	if ((fileSize % 84) != 0)
		return 0;

	if (fileSize < 252)
		return 0;
	if (ReadInt64(&hdr[0]) != devId || ReadInt64(&hdr[84]) != devId || ReadInt64(&hdr[168]) != devId)
		return 0;

	Map::GPSTrack *track;
	sptr = Text::StrInt64(sbuff, devId);
	s = Text::String::New(sbuff, (UOSInt)(sptr - sbuff));
	NEW_CLASS(track, Map::GPSTrack(fd->GetFullName(), true, 0, s.Ptr()));
	track->SetTrackName(s->ToCString());
	s->Release();
	SLOCExtraParser *parser;
	NEW_CLASS(parser, SLOCExtraParser());
	track->SetExtraParser(parser);
	currPos = 0;
	while (currPos < fileSize)
	{
		ExtraInfo extInfo;
		fd->GetRealData(currPos, 84, BYTEARR(buff));
		extInfo.termId = ReadInt64(&buff[0]);
		rec.pos.SetLat(ReadInt32(&buff[8]) / 200000.0);
		rec.pos.SetLon(ReadInt32(&buff[12]) / 200000.0);
		rec.speed = ReadUInt16(&buff[16]) * 0.01;
		rec.heading = ReadUInt16(&buff[18]) * 0.01;
		rec.recTime = Data::TimeInstant(ReadUInt32(&buff[20]), 0);
		rec.altitude = ReadInt16(&buff[24]);
		extInfo.status = ReadUInt32(&buff[26]);
		extInfo.inStatus = ReadUInt32(&buff[30]);
		extInfo.outStatus = ReadUInt32(&buff[34]);
		rec.nSateUsedGPS = buff[38];
		rec.nSateUsed = buff[38];
		rec.nSateUsedGLO = 0;
		rec.nSateUsedSBAS = 0;
		rec.nSateViewGPS = 0;
		rec.nSateViewGA = 0;
		rec.nSateViewGLO = 0;
		rec.nSateViewQZSS = 0;
		rec.nSateViewBD = 0;
		rec.valid = buff[39] & 1;
		extInfo.fixMode = buff[40];
		extInfo.gpsFix = buff[41];
		extInfo.pdop = ReadInt16(&buff[42]) * 0.1;
		extInfo.hdop = ReadInt16(&buff[44]) * 0.1;
		extInfo.vdop = ReadInt16(&buff[46]) * 0.1;
		extInfo.adc1 = ReadInt32(&buff[48]);
		extInfo.adc2 = ReadInt32(&buff[52]);
		extInfo.devType = ReadInt32(&buff[56]);
		extInfo.temper1 = ReadInt32(&buff[60]) * 0.1;
		extInfo.temper2 = ReadInt32(&buff[64]) * 0.1;
		extInfo.temper3 = ReadInt32(&buff[68]) * 0.1;
		extInfo.cliIP = ReadNUInt32(&buff[72]);
		extInfo.cliPort = ReadUInt16(&buff[76]);
		extInfo.recvTimeTS = ReadUInt32(&buff[78]);
		i = track->AddRecord(rec);
		track->SetExtraDataIndex(i, (UInt8*)&extInfo, sizeof(extInfo));
		currPos += 84;
		
	}
	return track;
}
