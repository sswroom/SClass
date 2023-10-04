#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ByteBuffer.h"
#include "Data/ByteTool.h"
#include "IO/BTScanLog.h"
#include "IO/FileStream.h"
#include "IO/StreamData.h"
#include "IO/FileAnalyse/PCapngFileAnalyse.h"
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

void Parser::FileParser::PCAPNGParser::PrepareSelector(NotNullPtr<IO::FileSelector> selector, IO::ParserType t)
{
	if (t == IO::ParserType::Unknown || t == IO::ParserType::EthernetAnalyzer)
	{
		selector->AddFilter(CSTR("*.pcapng"), CSTR("PCAPNG File"));
	}
}

IO::ParserType Parser::FileParser::PCAPNGParser::GetParserType()
{
	return IO::ParserType::EthernetAnalyzer;
}

IO::ParsedObject *Parser::FileParser::PCAPNGParser::ParseFileHdr(NotNullPtr<IO::StreamData> fd, IO::PackageFile *pkgFile, IO::ParserType targetType, const UInt8 *hdr)
{
	UInt32 maxSize = 65536;
	UInt64 currOfst;
	UInt64 fileSize = fd->GetDataSize();
	UInt32 packetType;
	UInt32 ifId;
	UInt16 linkType;
	UInt32 inclLen;
	UInt32 packetSize;
	Int8 timeResol;

	Bool isBTLink = false;
	IO::BTScanLog *scanLog;
	Net::EthernetAnalyzer *analyzer;
	Data::DateTime dt;

	if (ReadNInt32(hdr) != 0x0a0d0d0a)
	{
		return 0;
	}
	if (ReadInt32(&hdr[8]) == 0x1a2b3c4d)
	{
		Data::ArrayList<Int8> resolList;
		Data::ArrayList<UInt16> linkTypeList;

		NEW_CLASS(analyzer, Net::EthernetAnalyzer(0, Net::EthernetAnalyzer::AT_ALL, fd->GetFullFileName()));
		NEW_CLASS(scanLog, IO::BTScanLog(fd->GetFullFileName()));
		Data::ByteBuffer packetBuff(maxSize);
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
				fd->GetRealData(currOfst + 12, packetSize - 12, packetBuff.SubArray(12));
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
				if (ReadUInt16(&packetBuff[8]) == 201)
				{
					isBTLink = true;
				}
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
					i += 4 + (UOSInt)optLeng;
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
				if (linkType == 201)
				{
					if (inclLen > packetSize - 32)
					{
						break;
					}
					Int64 ts = (((Int64)ReadInt32(&packetBuff[12])) << 32) | ReadUInt32(&packetBuff[16]);
					IO::FileAnalyse::PCapngFileAnalyse::SetTime(&dt, ts, resolList.GetItem(0));
					scanLog->AddBTRAWPacket(dt.ToTicks(), &packetBuff[28], inclLen);
				}
				else
				{
					if (inclLen < 14 || inclLen > packetSize - 32)
					{
						break;
					}
					analyzer->PacketData(linkType, &packetBuff[28], inclLen);
				}
			}

			currOfst += packetSize;
		}
		if (isBTLink)
		{
			DEL_CLASS(analyzer);
			return scanLog;
		}
		else
		{
			DEL_CLASS(scanLog);
			return analyzer;
		}
	}
	else if (ReadMInt32(&hdr[8]) == 0x1a2b3c4d)
	{
		Data::ArrayList<Int8> resolList;
		Data::ArrayList<UInt16> linkTypeList;

		NEW_CLASS(analyzer, Net::EthernetAnalyzer(0, Net::EthernetAnalyzer::AT_ALL, fd->GetFullFileName()));
		NEW_CLASS(scanLog, IO::BTScanLog(fd->GetFullFileName()));
		Data::ByteBuffer packetBuff(maxSize);
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
				fd->GetRealData(currOfst + 12, packetSize - 12, packetBuff.SubArray(12));
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
				if (ReadMUInt16(&packetBuff[8]) == 201)
				{
					isBTLink = true;
				}
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
					i += 4 + (UOSInt)optLeng;
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
				if (linkType == 201)
				{
					if (inclLen > packetSize - 32)
					{
						break;
					}
					IO::FileAnalyse::PCapngFileAnalyse::SetTime(&dt, ReadMInt64(&packetBuff[12]), resolList.GetItem(0));
					scanLog->AddBTRAWPacket(dt.ToTicks(), &packetBuff[28], inclLen);
				}
				else
				{
					if (inclLen < 14 || inclLen > packetSize - 32)
					{
						break;
					}
					analyzer->PacketData(linkType, &packetBuff[28], inclLen);
				}
			}

			currOfst += packetSize;
		}
		if (isBTLink)
		{
			DEL_CLASS(analyzer);
			return scanLog;
		}
		else
		{
			DEL_CLASS(scanLog);
			return analyzer;
		}
	}
	return 0;
}
