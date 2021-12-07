#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ByteTool.h"
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

	virtual Bool GetExtraName(const UInt8 *buff, UOSInt buffSize, UOSInt extIndex, Text::StringBuilderUTF *sb)
	{
		switch (extIndex)
		{
		case 0:
			sb->Append((const UTF8Char*)"TermId");
			return true;
		case 1:
			sb->Append((const UTF8Char*)"Status");
			return true;
		case 2:
			sb->Append((const UTF8Char*)"InStatus");
			return true;
		case 3:
			sb->Append((const UTF8Char*)"OutStatus");
			return true;
		case 4:
			sb->Append((const UTF8Char*)"FixMode");
			return true;
		case 5:
			sb->Append((const UTF8Char*)"GPSFix");
			return true;
		case 6:
			sb->Append((const UTF8Char*)"PDOP");
			return true;
		case 7:
			sb->Append((const UTF8Char*)"HDOP");
			return true;
		case 8:
			sb->Append((const UTF8Char*)"VDOP");
			return true;
		case 9:
			sb->Append((const UTF8Char*)"ADC1");
			return true;
		case 10:
			sb->Append((const UTF8Char*)"ADC2");
			return true;
		case 11:
			sb->Append((const UTF8Char*)"DevType");
			return true;
		case 12:
			sb->Append((const UTF8Char*)"Temper1");
			return true;
		case 13:
			sb->Append((const UTF8Char*)"Temper2");
			return true;
		case 14:
			sb->Append((const UTF8Char*)"Temper3");
			return true;
		case 15:
			sb->Append((const UTF8Char*)"CliIP");
			return true;
		case 16:
			sb->Append((const UTF8Char*)"CliPort");
			return true;
		case 17:
			sb->Append((const UTF8Char*)"RecvTime");
			return true;
		}
		return false;
	}

	virtual Bool GetExtraValueStr(const UInt8 *buff, UOSInt buffSize, UOSInt extIndex, Text::StringBuilderUTF *sb)
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
			sb->Append((const UTF8Char*)"0x");
			sb->AppendHex32(extInfo->status);
			return true;
		case 2:
			sb->Append((const UTF8Char*)"0x");
			sb->AppendHex32(extInfo->inStatus);
			return true;
		case 3:
			sb->Append((const UTF8Char*)"0x");
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
				Net::SocketUtil::GetIPv4Name(sbuff, extInfo->cliIP);
				sb->Append(sbuff);
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

void Parser::FileParser::SLOCParser::PrepareSelector(IO::IFileSelector *selector, IO::ParserType t)
{
	if (t == IO::ParserType::Unknown || t == IO::ParserType::MapLayer)
	{
		selector->AddFilter((const UTF8Char*)"*.dat", (const UTF8Char*)"SP Location File");
	}
}

IO::ParserType Parser::FileParser::SLOCParser::GetParserType()
{
	return IO::ParserType::MapLayer;
}

IO::ParsedObject *Parser::FileParser::SLOCParser::ParseFile(IO::IStreamData *fd, IO::PackageFile *pkgFile, IO::ParserType targetType)
{
	Map::GPSTrack::GPSRecord rec;
	UInt8 buff[384];
	UTF8Char sbuff[256];
	const UTF8Char *sptr;
	UOSInt i;
	UOSInt currPos;
	UInt64 fileSize;
	Int64 devId;
	sptr = fd->GetFullName()->v;
	i = Text::StrLastIndexOf(sptr, '\\');
	Text::StrConcat(sbuff, &sptr[i + 1]);
	if (!Text::StrStartsWithICase(sbuff, (const UTF8Char*)"LOC"))
	{
		return 0;
	}
	i = Text::StrIndexOf(sbuff, (const UTF8Char*)"_");
	if (i == INVALID_INDEX)
		return 0;
	sbuff[i] = 0;
	devId = Text::StrToInt64(&sbuff[3]);
	if (devId == 0)
		return 0;
	fileSize = fd->GetDataSize();
	if ((fileSize % 84) != 0)
		return 0;

	if (fd->GetRealData(0, 252, buff) != 252)
		return 0;
	if (*(Int64*)&buff[0] != devId || *(Int64*)&buff[84] != devId || *(Int64*)&buff[168] != devId)
		return 0;

	Map::GPSTrack *track;
	Text::StrInt64(sbuff, devId);
	Text::String *s = Text::String::NewNotNull(sbuff);
	NEW_CLASS(track, Map::GPSTrack(fd->GetFullName(), true, 0, s));
	s->Release();
	track->SetTrackName(sbuff);
	SLOCExtraParser *parser;
	NEW_CLASS(parser, SLOCExtraParser());
	track->SetExtraParser(parser);
	currPos = 0;
	while (currPos < fileSize)
	{
		ExtraInfo extInfo;
		fd->GetRealData(currPos, 84, buff);
		extInfo.termId = ReadInt64(&buff[0]);
		rec.lat = ReadInt32(&buff[8]) / 200000.0;
		rec.lon = ReadInt32(&buff[12]) / 200000.0;
		rec.speed = ReadUInt16(&buff[16]) * 0.01;
		rec.heading = ReadUInt16(&buff[18]) * 0.01;
		rec.utcTimeTicks = ReadUInt32(&buff[20]) * 1000LL;
		rec.altitude = ReadInt16(&buff[24]);
		extInfo.status = ReadUInt32(&buff[26]);
		extInfo.inStatus = ReadUInt32(&buff[30]);
		extInfo.outStatus = ReadUInt32(&buff[34]);
		rec.nSateUsed = buff[38];
		rec.nSateView = 0;
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
		i = track->AddRecord(&rec);
		track->SetExtraDataIndex(i, (UInt8*)&extInfo, sizeof(extInfo));
		currPos += 84;
		
	}
	return track;
}
