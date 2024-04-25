#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ByteTool.h"
#include "IO/Path.h"
#include "Map/GPSTrack.h"
#include "Net/SocketUtil.h"
#include "Parser/FileParser/SMDLParser.h"
#include "Text/MyString.h"
#include "Text/MyStringFloat.h"

class SMDLExtraParser : public Map::GPSTrack::GPSExtraParser
{
public:
	SMDLExtraParser()
	{
	}

	virtual ~SMDLExtraParser()
	{
	}

	virtual UOSInt GetExtraCount(const UInt8 *buff, UOSInt buffSize)
	{
		UOSInt analogCnt = ReadUInt32(&buff[24]);
		return 18 + 2 * analogCnt;
	}

	virtual Bool GetExtraName(const UInt8 *buff, UOSInt buffSize, UOSInt extIndex, NN<Text::StringBuilderUTF8> sb)
	{
		if (extIndex < 18)
		{
			switch (extIndex)
			{
			case 0:
				sb->AppendC(UTF8STRC("RecvTime"));
				return true;
			case 1:
				sb->AppendC(UTF8STRC("Status1"));
				return true;
			case 2:
				sb->AppendC(UTF8STRC("Status2"));
				return true;
			case 3:
				sb->AppendC(UTF8STRC("Status3"));
				return true;
			case 4:
				sb->AppendC(UTF8STRC("ReportingCode"));
				return true;
			case 5:
				sb->AppendC(UTF8STRC("AnalogCnt"));
				return true;
			case 6:
				sb->AppendC(UTF8STRC("RemoteAddr"));
				return true;
			case 7:
				sb->AppendC(UTF8STRC("ProcTime"));
				return true;
			case 8:
				sb->AppendC(UTF8STRC("ExtraSize"));
				return true;
			case 9:
				sb->AppendC(UTF8STRC("HandlerType"));
				return true;
			case 10:
				sb->AppendC(UTF8STRC("MCC"));
				return true;
			case 11:
				sb->AppendC(UTF8STRC("MNC"));
				return true;
			case 12:
				sb->AppendC(UTF8STRC("LAC"));
				return true;
			case 13:
				sb->AppendC(UTF8STRC("CellId"));
				return true;
			case 14:
				sb->AppendC(UTF8STRC("SignalStrength"));
				return true;
			case 15:
				sb->AppendC(UTF8STRC("DOP"));
				return true;
			case 16:
				sb->AppendC(UTF8STRC("IDType"));
				return true;
			case 17:
				sb->AppendC(UTF8STRC("DriverId"));
				return true;
			}
		}
		else
		{
			if (extIndex & 1)
			{
				sb->AppendC(UTF8STRC("AnalogVal"));
				sb->AppendUOSInt((extIndex - 8) >> 1);
				return true;
			}
			else
			{
				sb->AppendC(UTF8STRC("AnalogId"));
				sb->AppendUOSInt((extIndex - 8) >> 1);
				return true;
			}
		}
		return false;
	}

	virtual Bool GetExtraValueStr(const UInt8 *buff, UOSInt buffSize, UOSInt extIndex, NN<Text::StringBuilderUTF8> sb)
	{
		UTF8Char sbuff[32];
		UTF8Char *sptr;
		if (extIndex < 18)
		{
			switch (extIndex)
			{
			case 0:
				sb->AppendTSNoZone(Data::Timestamp(ReadInt64(&buff[0]), Data::DateTimeUtil::GetLocalTzQhr()));
				return true;
			case 1:
				sb->AppendC(UTF8STRC("0x"));
				sb->AppendHex32(ReadUInt32(&buff[8]));
				return true;
			case 2:
				sb->AppendC(UTF8STRC("0x"));
				sb->AppendHex32(ReadUInt32(&buff[12]));
				return true;
			case 3:
				sb->AppendC(UTF8STRC("0x"));
				sb->AppendHex32(ReadUInt32(&buff[16]));
				return true;
			case 4:
				sb->AppendI32(ReadInt32(&buff[20]));
				return true;
			case 5:
				sb->AppendI32(ReadInt32(&buff[24]));
				return true;
			case 6:
				sptr = Net::SocketUtil::GetIPv4Name(sbuff, ReadUInt32(&buff[28]), ReadUInt16(&buff[32]));
				sb->AppendC(sbuff, (UOSInt)(sptr - sbuff));
				return true;
			case 7:
				sb->AppendI32(ReadInt32(&buff[34]));
				return true;
			case 8:
				sb->AppendI32(ReadInt32(&buff[38]));
				return true;
			case 9:
				sb->AppendU16(ReadUInt16(&buff[42]));
				return true;
			case 10:
				sb->AppendU16(ReadUInt16(&buff[44]));
				return true;
			case 11:
				sb->AppendU16(ReadUInt16(&buff[46]));
				return true;
			case 12:
				sb->AppendU16(ReadUInt16(&buff[48]));
				return true;
			case 13:
				sb->AppendI32(ReadInt32(&buff[50]));
				return true;
			case 14:
				sb->AppendI32(ReadInt32(&buff[54]));
				return true;
			case 15:
				Text::SBAppendF64(sb, ReadDouble(&buff[58]));
				return true;
			case 16:
				sb->AppendI32(ReadInt32(&buff[66]));
				return true;
			case 17:
				sb->AppendSlow(&buff[70]);
				return true;
			}
		}
		else
		{
			if (extIndex & 1)
			{
				Text::SBAppendF64(sb, ReadDouble(&buff[94 + (16 * ((extIndex - 18) >> 1))]));
				return true;
			}
			else
			{
				sb->AppendC(UTF8STRC("0x"));
				sb->AppendHex64(ReadUInt64(&buff[94 + (16 * ((extIndex - 18) >> 1)) + 8]));
				return true;
			}
		}
		return false;
	}
};

Parser::FileParser::SMDLParser::SMDLParser()
{
}

Parser::FileParser::SMDLParser::~SMDLParser()
{
}

Int32 Parser::FileParser::SMDLParser::GetName()
{
	return *(Int32*)"SMDL";
}

void Parser::FileParser::SMDLParser::PrepareSelector(NN<IO::FileSelector> selector, IO::ParserType t)
{
	if (t == IO::ParserType::Unknown || t == IO::ParserType::MapLayer)
	{
		selector->AddFilter(CSTR("*.loc"), CSTR("Location File"));
	}
}

IO::ParserType Parser::FileParser::SMDLParser::GetParserType()
{
	return IO::ParserType::MapLayer;
}

IO::ParsedObject *Parser::FileParser::SMDLParser::ParseFileHdr(NN<IO::StreamData> fd, IO::PackageFile *pkgFile, IO::ParserType targetType, const UInt8 *hdr)
{
	Map::GPSTrack::GPSRecord3 rec;
	UInt8 buff[384];
	UTF8Char sbuff[256];
	UTF8Char *sptr;
	NN<Text::String> s;
	UOSInt i;
	UOSInt ui;
	UOSInt currPos;
	UInt64 fileSize;
	Int32 t;
	Int32 fileT;
	s = fd->GetFullName();
	if (!s->EndsWith(UTF8STRC(".loc")))
	{
		return 0;
	}
	sptr = fd->GetFullName()->v;
	i = s->LastIndexOf(IO::Path::PATH_SEPERATOR);
	sptr = Text::StrConcat(sbuff, &s->v[i + 1]);
	sptr[-4] = 0;
	sptr -= 4;
	t = Text::StrToInt32(sbuff);
	fileSize = fd->GetDataSize();
	if (fd->GetRealData(0, 252, BYTEARR(buff)) != 252)
		return 0;
	Data::DateTime dt;
	dt.SetTicks(ReadInt64(&buff[0]));
	fileT = dt.GetYear() * 10000 + dt.GetMonth() * 100 + dt.GetDay();
	if (fileT != t)
	{
		return 0;
	}

	Map::GPSTrack *track;
	s = Text::String::NewP(sbuff, sptr);
	NEW_CLASS(track, Map::GPSTrack(fd->GetFullName(), true, 0, s.Ptr()));
	track->SetTrackName(s->ToCString());
	s->Release();
	SMDLExtraParser *parser;
	NEW_CLASS(parser, SMDLExtraParser());
	track->SetExtraParser(parser);
	currPos = 0;
	while (currPos < fileSize)
	{
		Int64 recvTimeTick;
		Int32 status1;
		Int32 status2;
		Int32 status3;
		Int32 reportingCode;
		UInt32 analogCnt;
		UInt32 remoteIP;
		UInt16 remotePort;
		Int32 procTime;
		UInt32 extraSize;
		fd->GetRealData(currPos, 96, BYTEARR(buff));
		rec.recTime = Data::TimeInstant::FromTicks(ReadInt64(&buff[0]));
		recvTimeTick = ReadInt64(&buff[8]);
		rec.pos = Math::Coord2DDbl(ReadDouble(&buff[24]), ReadDouble(&buff[16]));
		rec.altitude = ReadDouble(&buff[32]);
		rec.speed = ReadDouble(&buff[40]);
		rec.heading = ReadDouble(&buff[48]);
		rec.nSateUsedGPS = (UInt8)ReadInt32(&buff[56]);
		rec.nSateUsed = rec.nSateUsedGPS;
		rec.nSateUsedGLO = 0;
		rec.nSateUsedSBAS = 0;
		rec.nSateViewGPS = 0;
		rec.nSateViewGLO = 0;
		rec.nSateViewGA = 0;
		rec.nSateViewQZSS = 0;
		rec.nSateViewBD = 0;
		status1 = ReadInt32(&buff[60]);
		status2 = ReadInt32(&buff[64]);
		status3 = ReadInt32(&buff[68]);
		reportingCode = ReadInt32(&buff[72]);
		analogCnt = ReadUInt32(&buff[76]);
		remoteIP = ReadUInt32(&buff[80]);
		remotePort = ReadUInt16(&buff[84]);
		procTime = ReadInt32(&buff[88]);
		extraSize = ReadUInt32(&buff[92]);
		rec.valid = (status1 & 2) != 0;
		ui = track->AddRecord(rec);
		currPos += 96;
		WriteInt64(&buff[0], recvTimeTick);
		WriteInt32(&buff[8], status1);
		WriteInt32(&buff[12], status2);
		WriteInt32(&buff[16], status3);
		WriteInt32(&buff[20], reportingCode);
		WriteUInt32(&buff[24], analogCnt);
		WriteUInt32(&buff[28], remoteIP);
		WriteInt16(&buff[32], remotePort);
		WriteInt32(&buff[34], procTime);
		WriteUInt32(&buff[38], extraSize);
		if (analogCnt > 0 && analogCnt <= 32)
		{
			fd->GetRealData(currPos, 16 * analogCnt, BYTEARR(buff) + 94);
			currPos += 16 * analogCnt;
			fd->GetRealData(currPos, extraSize, BYTEARR(buff) + 42);
			currPos += extraSize;
			track->SetExtraDataIndex(ui, buff, 94 + 16 * analogCnt);
		}
		else if (analogCnt == 0)
		{
			fd->GetRealData(currPos, extraSize, BYTEARR(buff) + 42);
			currPos += extraSize;
			track->SetExtraDataIndex(ui, buff, 42 + extraSize);
		}
		else
		{
			break;
		}
	}
	return track;
}
