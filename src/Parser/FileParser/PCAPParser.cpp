#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ByteTool.h"
#include "IO/FileStream.h"
#include "IO/IStreamData.h"
#include "Net/EthernetAnalyzer.h"
#include "Parser/FileParser/PCAPParser.h"

Parser::FileParser::PCAPParser::PCAPParser()
{
}

Parser::FileParser::PCAPParser::~PCAPParser()
{
}

Int32 Parser::FileParser::PCAPParser::GetName()
{
	return *(Int32*)"PCAP";
}

void Parser::FileParser::PCAPParser::PrepareSelector(IO::IFileSelector *selector, IO::ParserType t)
{
	if (t == IO::ParserType::Unknown || t == IO::ParserType::EthernetAnalyzer)
	{
		selector->AddFilter((const UTF8Char*)"*.pcap", (const UTF8Char*)"PCAP File");
	}
}

IO::ParserType Parser::FileParser::PCAPParser::GetParserType()
{
	return IO::ParserType::EthernetAnalyzer;
}

IO::ParsedObject *Parser::FileParser::PCAPParser::ParseFile(IO::IStreamData *fd, IO::PackageFile *pkgFile, IO::ParserType targetType)
{
	UInt8 hdr[24];
	UInt8 *packetBuff;
	UInt32 maxSize = 65536;
	UInt64 currOfst;
	UInt64 fileSize = fd->GetDataSize();
	UInt32 inclLen;
	UInt32 origLen;
	UInt32 linkType;
	Net::EthernetAnalyzer *analyzer;

	if (fd->GetRealData(0, 24, hdr) != 24)
		return 0;
	if (ReadUInt32(hdr) == 0xa1b2c3d4)
	{
		linkType = ReadUInt32(&hdr[20]);
		NEW_CLASS(analyzer, Net::EthernetAnalyzer(0, Net::EthernetAnalyzer::AT_ALL, fd->GetFullFileName()));
		packetBuff = MemAlloc(UInt8, maxSize);
		currOfst = 24;
		while (currOfst + 16 < fileSize)
		{
			if (fd->GetRealData(currOfst, 16, hdr) != 16)
			{
				break;
			}
			inclLen = ReadUInt32(&hdr[8]);
			origLen = ReadUInt32(&hdr[12]);
			if (inclLen < 14 || inclLen > maxSize)
			{
				break;
			}
			if (fd->GetRealData(currOfst + 16, inclLen, packetBuff) != inclLen)
				break;
			analyzer->PacketData(linkType, packetBuff, origLen);
			currOfst += 16 + inclLen;
		}
		MemFree(packetBuff);
		return analyzer;
	}
	else if (ReadMUInt32(hdr) == 0xa1b2c3d4)
	{
		linkType = ReadMUInt32(&hdr[20]);
		NEW_CLASS(analyzer, Net::EthernetAnalyzer(0, Net::EthernetAnalyzer::AT_ALL, fd->GetFullFileName()));
		packetBuff = MemAlloc(UInt8, maxSize);
		currOfst = 24;
		while (currOfst + 16 < fileSize)
		{
			if (fd->GetRealData(currOfst, 16, hdr) != 16)
			{
				break;
			}
			inclLen = ReadMUInt32(&hdr[8]);
			origLen = ReadMUInt32(&hdr[12]);
			if (inclLen < 14 || inclLen > maxSize)
			{
				break;
			}
			if (fd->GetRealData(currOfst + 16, inclLen, packetBuff) != inclLen)
				break;
			analyzer->PacketData(linkType, packetBuff, origLen);
			currOfst += 16 + inclLen;
		}
		MemFree(packetBuff);
		return analyzer;
	}
	return 0;
}
