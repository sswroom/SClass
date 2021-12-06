#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ByteTool.h"
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

	virtual UOSInt GetExtraCount(const UInt8 *buff, UOSInt buffSize)
	{
		return 24;
	}

	virtual Bool GetExtraName(const UInt8 *buff, UOSInt buffSize, UOSInt extIndex, Text::StringBuilderUTF *sb)
	{
		switch (extIndex)
		{
		case 0:
			sb->Append((const UTF8Char*)"TermId");
			return true;
		case 1:
			sb->Append((const UTF8Char*)"DevType");
			return true;
		case 2:
			sb->Append((const UTF8Char*)"RecvTime");
			return true;
		case 3:
			sb->Append((const UTF8Char*)"Status");
			return true;
		case 4:
			sb->Append((const UTF8Char*)"Status2");
			return true;
		case 5:
			sb->Append((const UTF8Char*)"Period");
			return true;
		case 6:
			sb->Append((const UTF8Char*)"MileageTZ");
			return true;
		case 7:
			sb->Append((const UTF8Char*)"Temperature1");
			return true;
		case 8:
			sb->Append((const UTF8Char*)"Temperature2");
			return true;
		case 9:
			sb->Append((const UTF8Char*)"Temperature3");
			return true;
		case 10:
			sb->Append((const UTF8Char*)"RHLine");
			return true;
		case 11:
			sb->Append((const UTF8Char*)"RHRef");
			return true;
		case 12:
			sb->Append((const UTF8Char*)"MileageDiff");
			return true;
		case 13:
			sb->Append((const UTF8Char*)"MileageDaily");
			return true;
		case 14:
			sb->Append((const UTF8Char*)"FuelLev");
			return true;
		case 15:
			sb->Append((const UTF8Char*)"DevIP");
			return true;
		case 16:
			sb->Append((const UTF8Char*)"DevPort");
			return true;
		case 17:
			sb->Append((const UTF8Char*)"RecType");
			return true;
		case 18:
			sb->Append((const UTF8Char*)"Status4");
			return true;
		case 19:
			sb->Append((const UTF8Char*)"adc5");
			return true;
		case 20:
			sb->Append((const UTF8Char*)"adc6");
			return true;
		case 21:
			sb->Append((const UTF8Char*)"adc7");
			return true;
		case 22:
			sb->Append((const UTF8Char*)"adc8");
			return true;
		case 23:
			sb->Append((const UTF8Char*)"FileOffset");
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
			sb->AppendU32(extInfo->termId);
			return true;
		case 1:
			sb->AppendI32(extInfo->devType);
			return true;
		case 2:
			{
				Data::DateTime dt;
				dt.SetUnixTimestamp(extInfo->recvTimeTS);
				dt.ToLocalTime();
				sb->AppendDate(&dt);
			}
			return true;
		case 3:
			sb->Append((const UTF8Char*)"0x");
			sb->AppendHex32(extInfo->status);
			return true;
		case 4:
			sb->Append((const UTF8Char*)"0x");
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
				Net::SocketUtil::GetIPv4Name(sbuff, extInfo->devIP);
				sb->Append(sbuff);
			}
			return true;
		case 16:
			sb->AppendU16(extInfo->devPort);
			return true;
		case 17:
			sb->AppendU16(extInfo->recType);
			return true;
		case 18:
			sb->Append((const UTF8Char*)"0x");
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

void Parser::FileParser::GLOCParser::PrepareSelector(IO::IFileSelector *selector, IO::ParserType t)
{
	if (t == IO::ParserType::Unknown || t == IO::ParserType::MapLayer)
	{
		selector->AddFilter((const UTF8Char*)"*.dat", (const UTF8Char*)"GPS Location File");
	}
}

IO::ParserType Parser::FileParser::GLOCParser::GetParserType()
{
	return IO::ParserType::MapLayer;
}

IO::ParsedObject *Parser::FileParser::GLOCParser::ParseFile(IO::IStreamData *fd, IO::PackageFile *pkgFile, IO::ParserType targetType)
{
	Map::GPSTrack::GPSRecord rec;
	UInt8 buff[384];
	UTF8Char u8buff[256];
	const UTF8Char *sptr;
	UOSInt i;
	UInt64 currPos;
	UInt64 fileSize;
	Int64 devId;
	UInt32 idevId;
	sptr = fd->GetFullName()->v;
	i = Text::StrLastIndexOf(sptr, '\\');
	Text::StrConcat(u8buff, &sptr[i + 1]);
	if (!Text::StrStartsWithICase(u8buff, (const UTF8Char*)"GLOC"))
	{
		return 0;
	}
	i = Text::StrIndexOf(u8buff, (const UTF8Char*)"_");
	if (i == INVALID_INDEX)
	{
		i = Text::StrIndexOf(u8buff, (const UTF8Char*)".");
		if (i == INVALID_INDEX)
			return 0;
		u8buff[i] = 0;
		devId = Text::StrToInt64(&u8buff[4]);
	}
	else
	{
		u8buff[i] = 0;
		devId = Text::StrToInt64(&u8buff[4]);
	}
	if (devId == 0)
		return 0;
	fileSize = fd->GetDataSize();
	if (fileSize & 127)
		return 0;
	idevId = (UInt32)(devId & 0xffffffffLL);

	i = fd->GetRealData(0, 384, buff);
	if (*(UInt32*)&buff[0] != idevId || (i > 128 && *(UInt32*)&buff[128] != idevId) || (i > 256 && *(UInt32*)&buff[256] != idevId))
		return 0;

	Map::GPSTrack *track;
	Text::StrInt64(u8buff, devId);
	NEW_CLASS(track, Map::GPSTrack(fd->GetFullName()->v, true, 0, u8buff));
	track->SetTrackName(u8buff);
	GLOCExtraParser *parser;
	NEW_CLASS(parser, GLOCExtraParser());
	track->SetExtraParser(parser);
	currPos = 0;
	while (currPos < fileSize)
	{
		ExtraInfo extInfo;
		fd->GetRealData(currPos, 128, buff);
		extInfo.termId = ReadUInt32(&buff[0]);
		extInfo.devType = ReadInt32(&buff[4]);
		rec.utcTimeTicks = ReadUInt32(&buff[8]) * 1000LL;
		extInfo.recvTimeTS = ReadUInt32(&buff[12]);
		rec.lat = ReadInt32(&buff[16]) / 10000000.0;
		rec.lon = ReadInt32(&buff[20]) / 10000000.0;
		rec.speed = ReadUInt16(&buff[24]) * 0.1;
		rec.heading = ReadUInt16(&buff[26]) * 0.01;
		extInfo.status = ReadUInt32(&buff[28]);
		extInfo.status2 = ReadUInt32(&buff[32]);
		extInfo.period = ReadInt32(&buff[36]);
		rec.valid = (buff[32] & 1) == 0;
		rec.nSateUsed = buff[40];
		rec.nSateView = buff[41];
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
		i = track->AddRecord(&rec);
		track->SetExtraDataIndex(i, (UInt8*)&extInfo, sizeof(extInfo));
		currPos += 128;
	}
	return track;
}
