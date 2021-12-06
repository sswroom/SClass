#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ByteTool.h"
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

	virtual Bool GetExtraName(const UInt8 *buff, UOSInt buffSize, UOSInt extIndex, Text::StringBuilderUTF *sb)
	{
		if (extIndex < 18)
		{
			switch (extIndex)
			{
			case 0:
				sb->Append((const UTF8Char*)"RecvTime");
				return true;
			case 1:
				sb->Append((const UTF8Char*)"Status1");
				return true;
			case 2:
				sb->Append((const UTF8Char*)"Status2");
				return true;
			case 3:
				sb->Append((const UTF8Char*)"Status3");
				return true;
			case 4:
				sb->Append((const UTF8Char*)"ReportingCode");
				return true;
			case 5:
				sb->Append((const UTF8Char*)"AnalogCnt");
				return true;
			case 6:
				sb->Append((const UTF8Char*)"RemoteAddr");
				return true;
			case 7:
				sb->Append((const UTF8Char*)"ProcTime");
				return true;
			case 8:
				sb->Append((const UTF8Char*)"ExtraSize");
				return true;
			case 9:
				sb->Append((const UTF8Char*)"HandlerType");
				return true;
			case 10:
				sb->Append((const UTF8Char*)"MCC");
				return true;
			case 11:
				sb->Append((const UTF8Char*)"MNC");
				return true;
			case 12:
				sb->Append((const UTF8Char*)"LAC");
				return true;
			case 13:
				sb->Append((const UTF8Char*)"CellId");
				return true;
			case 14:
				sb->Append((const UTF8Char*)"SignalStrength");
				return true;
			case 15:
				sb->Append((const UTF8Char*)"DOP");
				return true;
			case 16:
				sb->Append((const UTF8Char*)"IDType");
				return true;
			case 17:
				sb->Append((const UTF8Char*)"DriverId");
				return true;
			}
		}
		else
		{
			if (extIndex & 1)
			{
				sb->Append((const UTF8Char*)"AnalogVal");
				sb->AppendUOSInt((extIndex - 8) >> 1);
				return true;
			}
			else
			{
				sb->Append((const UTF8Char*)"AnalogId");
				sb->AppendUOSInt((extIndex - 8) >> 1);
				return true;
			}
		}
		return false;
	}

	virtual Bool GetExtraValueStr(const UInt8 *buff, UOSInt buffSize, UOSInt extIndex, Text::StringBuilderUTF *sb)
	{
		UTF8Char sbuff[32];
		if (extIndex < 18)
		{
			switch (extIndex)
			{
			case 0:
				{
					Data::DateTime dt;
					dt.SetTicks(ReadInt64(&buff[0]));
					dt.ToLocalTime();
					sb->AppendDate(&dt);
				}
				return true;
			case 1:
				sb->Append((const UTF8Char*)"0x");
				sb->AppendHex32(ReadUInt32(&buff[8]));
				return true;
			case 2:
				sb->Append((const UTF8Char*)"0x");
				sb->AppendHex32(ReadUInt32(&buff[12]));
				return true;
			case 3:
				sb->Append((const UTF8Char*)"0x");
				sb->AppendHex32(ReadUInt32(&buff[16]));
				return true;
			case 4:
				sb->AppendI32(ReadInt32(&buff[20]));
				return true;
			case 5:
				sb->AppendI32(ReadInt32(&buff[24]));
				return true;
			case 6:
				Net::SocketUtil::GetIPv4Name(sbuff, ReadUInt32(&buff[28]), ReadUInt16(&buff[32]));
				sb->Append(sbuff);
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
				sb->Append(&buff[70]);
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
				sb->Append((const UTF8Char*)"0x");
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

void Parser::FileParser::SMDLParser::PrepareSelector(IO::IFileSelector *selector, IO::ParserType t)
{
	if (t == IO::ParserType::Unknown || t == IO::ParserType::MapLayer)
	{
		selector->AddFilter((const UTF8Char*)"*.loc", (const UTF8Char*)"Location File");
	}
}

IO::ParserType Parser::FileParser::SMDLParser::GetParserType()
{
	return IO::ParserType::MapLayer;
}

IO::ParsedObject *Parser::FileParser::SMDLParser::ParseFile(IO::IStreamData *fd, IO::PackageFile *pkgFile, IO::ParserType targetType)
{
	Map::GPSTrack::GPSRecord rec;
	UInt8 buff[384];
	UTF8Char sbuff[256];
	const UTF8Char *sptr;
	UOSInt i;
	UOSInt j;
	UOSInt ui;
	UOSInt currPos;
	UInt64 fileSize;
	Int32 t;
	Int32 fileT;
	sptr = fd->GetFullName()->v;
	i = Text::StrLastIndexOf(sptr, '\\');
	Text::StrConcat(sbuff, &sptr[i + 1]);
	j = Text::StrIndexOf(sbuff, (const UTF8Char*)".loc");
	if (j != 8)
	{
		return 0;
	}
	sbuff[j] = 0;
	t = Text::StrToInt32(sbuff);
	fileSize = fd->GetDataSize();
	if (fd->GetRealData(0, 252, buff) != 252)
		return 0;
	Data::DateTime dt;
	dt.SetTicks(ReadInt64(&buff[0]));
	fileT = dt.GetYear() * 10000 + dt.GetMonth() * 100 + dt.GetDay();
	if (fileT != t)
	{
		return 0;
	}

	Map::GPSTrack *track;
	Text::String *s = Text::String::New(sbuff);
	NEW_CLASS(track, Map::GPSTrack(fd->GetFullName(), true, 0, s));
	s->Release();
	track->SetTrackName(sbuff);
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
		fd->GetRealData(currPos, 96, buff);
		rec.utcTimeTicks = ReadInt64(&buff[0]);
		recvTimeTick = ReadInt64(&buff[8]);
		rec.lat = ReadDouble(&buff[16]);
		rec.lon = ReadDouble(&buff[24]);
		rec.altitude = ReadDouble(&buff[32]);
		rec.speed = ReadDouble(&buff[40]);
		rec.heading = ReadDouble(&buff[48]);
		rec.nSateUsed = ReadInt32(&buff[56]);
		rec.nSateView = 0;
		status1 = ReadInt32(&buff[60]);
		status2 = ReadInt32(&buff[64]);
		status3 = ReadInt32(&buff[68]);
		reportingCode = ReadInt32(&buff[72]);
		analogCnt = ReadUInt32(&buff[76]);
		remoteIP = ReadUInt32(&buff[80]);
		remotePort = ReadUInt16(&buff[84]);
		procTime = ReadInt32(&buff[88]);
		extraSize = ReadUInt32(&buff[92]);
		rec.valid = status1 & 2;
		ui = track->AddRecord(&rec);
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
			fd->GetRealData(currPos, 16 * analogCnt, &buff[94]);
			currPos += 16 * analogCnt;
			fd->GetRealData(currPos, extraSize, &buff[42]);
			currPos += extraSize;
			track->SetExtraDataIndex(ui, buff, 94 + 16 * analogCnt);
		}
		else if (analogCnt == 0)
		{
			fd->GetRealData(currPos, extraSize, &buff[42]);
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
