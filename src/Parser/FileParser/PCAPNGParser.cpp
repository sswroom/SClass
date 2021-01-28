#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ByteTool.h"
#include "IO/FileStream.h"
#include "IO/IStreamData.h"
#include "Net/EthernetAnalyzer.h"
#include "Parser/FileParser/PCAPNGParser.h"

Parser::FileParser::PCAPNGParser::PCAPNGParser()
{
}

Parser::FileParser::PCAPNGParser::~PCAPNGParser()
{
}

Int32 Parser::FileParser::PCAPNGParser::GetName()
{
	return *(Int32*)"PCAN";
}

void Parser::FileParser::PCAPNGParser::PrepareSelector(IO::IFileSelector *selector, IO::ParsedObject::ParserType t)
{
	if (t == IO::ParsedObject::PT_UNKNOWN || t == IO::ParsedObject::PT_ETHERNET_ANALYZER)
	{
		selector->AddFilter((const UTF8Char*)"*.pcapng", (const UTF8Char*)"PCAPNG File");
	}
}

IO::ParsedObject::ParserType Parser::FileParser::PCAPNGParser::GetParserType()
{
	return IO::ParsedObject::PT_ETHERNET_ANALYZER;
}

IO::ParsedObject *Parser::FileParser::PCAPNGParser::ParseFile(IO::IStreamData *fd, IO::PackageFile *pkgFile, IO::ParsedObject::ParserType targetType)
{
	UInt8 hdr[24];
	UInt8 *packetBuff;
	UInt32 maxSize = 65536;
	Int64 currOfst;
	Int64 fileSize = fd->GetDataSize();
	UInt32 packetType;
	UInt32 ifId;
	UInt16 linkType;
	UInt32 inclLen;
	UInt32 packetSize;
	Int8 timeResol;
	Net::EthernetAnalyzer *analyzer;

	if (fd->GetRealData(0, 24, hdr) != 24)
		return 0;
	if (ReadNInt32(hdr) != 0x0a0d0d0a)
	{
		return 0;
	}
	if (ReadInt32(&hdr[8]) == 0x1a2b3c4d)
	{
		Data::ArrayList<Int8> resolList;
		Data::ArrayList<UInt16> linkTypeList;

		NEW_CLASS(analyzer, Net::EthernetAnalyzer(0, Net::EthernetAnalyzer::AT_ALL, fd->GetFullFileName()));
		packetBuff = MemAlloc(UInt8, maxSize);
		currOfst = 0;
		while (currOfst + 12 < fileSize)
		{
			if (fd->GetRealData(currOfst, 12, packetBuff) != 12)
			{
				break;
			}
			packetSize = ReadUInt32(&packetBuff[4]);
			if (packetSize < 12 || packetSize + currOfst > fileSize)
			{
				break;
			}
			if (packetSize > 12)
			{
				fd->GetRealData(currOfst + 12, packetSize - 12, &packetBuff[12]);
			}
			packetType = ReadUInt32(&packetBuff[0]);
			if (packetType == 0x0a0d0d0a)
			{
				resolList.Clear();
				linkTypeList.Clear();
			}
			else if (packetType == 1)
			{
				UInt16 optCode;
				UInt16 optLeng;
				UOSInt i = 16;
				timeResol = 0;
				linkTypeList.Add(ReadUInt16(&packetBuff[8]));
				while (i < packetSize - 4)
				{
					optCode = ReadUInt16(&packetBuff[i]);
					optLeng = ReadUInt16(&packetBuff[i + 2]);
					if (i + 4 + optLeng > packetSize)
					{
						break;
					}
					if (optCode == 0)
					{
						break;
					}
					else if (optCode == 9)
					{
						timeResol = (Int8)packetBuff[i + 4];
						break;
					}
					i += 4 + optLeng;
					if (i & 3)
					{
						i += 4 - (i & 3);
					}
				}
				resolList.Add(timeResol);
			}
			else if (packetType == 6)
			{
				ifId = ReadUInt32(&packetBuff[8]);
				linkType = linkTypeList.GetItem(ifId);
				inclLen = ReadUInt32(&packetBuff[20]);
				if (inclLen < 14 || inclLen > packetSize - 32)
				{
					break;
				}
				analyzer->PacketData(linkType, &packetBuff[28], inclLen);
			}

			currOfst += packetSize;
		}
		MemFree(packetBuff);
		return analyzer;
	}
	else if (ReadMInt32(&hdr[8]) == 0x1a2b3c4d)
	{
		Data::ArrayList<Int8> resolList;
		Data::ArrayList<UInt16> linkTypeList;

		NEW_CLASS(analyzer, Net::EthernetAnalyzer(0, Net::EthernetAnalyzer::AT_ALL, fd->GetFullFileName()));
		packetBuff = MemAlloc(UInt8, maxSize);
		currOfst = 0;
		while (currOfst + 12 < fileSize)
		{
			if (fd->GetRealData(currOfst, 12, packetBuff) != 12)
			{
				break;
			}
			packetSize = ReadMUInt32(&packetBuff[4]);
			if (packetSize < 12 || packetSize + currOfst > fileSize)
			{
				break;
			}
			if (packetSize > 12)
			{
				fd->GetRealData(currOfst + 12, packetSize - 12, &packetBuff[12]);
			}
			packetType = ReadMUInt32(&packetBuff[0]);
			if (packetType == 0x0a0d0d0a)
			{
				resolList.Clear();
				linkTypeList.Clear();
			}
			else if (packetType == 1)
			{
				UInt16 optCode;
				UInt16 optLeng;
				UOSInt i = 16;
				timeResol = 0;
				linkTypeList.Add(ReadMUInt16(&packetBuff[8]));
				while (i < packetSize - 4)
				{
					optCode = ReadMUInt16(&packetBuff[i]);
					optLeng = ReadMUInt16(&packetBuff[i + 2]);
					if (i + 4 + optLeng > packetSize)
					{
						break;
					}
					if (optCode == 0)
					{
						break;
					}
					else if (optCode == 9)
					{
						timeResol = (Int8)packetBuff[i + 4];
						break;
					}
					i += 4 + optLeng;
					if (i & 3)
					{
						i += 4 - (i & 3);
					}
				}
				resolList.Add(timeResol);
			}
			else if (packetType == 6)
			{
				ifId = ReadMUInt32(&packetBuff[8]);
				linkType = linkTypeList.GetItem(ifId);
				inclLen = ReadMUInt32(&packetBuff[20]);
				if (inclLen < 14 || inclLen > packetSize - 32)
				{
					break;
				}
				analyzer->PacketData(linkType, &packetBuff[28], inclLen);
			}

			currOfst += packetSize;
		}
		MemFree(packetBuff);
		return analyzer;
	}
	return 0;
}
