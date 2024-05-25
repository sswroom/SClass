#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ByteBuffer.h"
#include "Data/ByteTool.h"
#include "IO/StringLogFile.h"
#include "Parser/FileParser/EVTXParser.h"
#include <stdio.h>

#define VERBOSE

//https://github.com/libyal/libevtx/blob/main/documentation/Windows%20XML%20Event%20Log%20(EVTX).asciidoc

Parser::FileParser::EVTXParser::EVTXParser()
{
}

Parser::FileParser::EVTXParser::~EVTXParser()
{
}

Int32 Parser::FileParser::EVTXParser::GetName()
{
	return *(Int32*)"EVTX";
}

void Parser::FileParser::EVTXParser::PrepareSelector(NN<IO::FileSelector> selector, IO::ParserType t)
{
	if (t == IO::ParserType::Unknown || t == IO::ParserType::LogFile)
	{
		selector->AddFilter(CSTR("*.evtx"), CSTR("EVTX File"));
	}
}

IO::ParserType Parser::FileParser::EVTXParser::GetParserType()
{
	return IO::ParserType::LogFile;
}

Optional<IO::ParsedObject> Parser::FileParser::EVTXParser::ParseFileHdr(NN<IO::StreamData> fd, Optional<IO::PackageFile> pkgFile, IO::ParserType targetType, Data::ByteArrayR hdr)
{
	if (hdr.ReadNU64(0) != *(UInt64*)"ElfFile" || ReadUInt32(&hdr[32]) != 128 || ReadUInt16(&hdr[40]) != 4096)
	{
		return 0;
	}
	IO::StringLogFile *logFile;
	UInt64 ofst = 4096;
	UOSInt dataOfst;
	Data::Timestamp ts;
	Text::StringBuilderUTF8 sb;
	Text::StringBuilderUTF8 sbDetail;
	Data::ByteBuffer buff(65536);
	NEW_CLASS(logFile, IO::StringLogFile(fd->GetFullName()));

	while (fd->GetRealData(ofst, 65536, buff) == 65536)
	{
		if (*(UInt64*)&buff[0] == *(UInt64*)"ElfChnk")
		{
//			UInt64 firstEvtRecNum = ReadUInt64(&buff[8]);
//			UInt64 lastEvtRecNum = ReadUInt64(&buff[16]);
//			UInt64 firstEvtRecId = ReadUInt64(&buff[24]);
//			UInt64 lastEvtRecId = ReadUInt64(&buff[32]);
//			UInt32 hdrSize = ReadUInt32(&buff[40]);
			dataOfst = 512;
			UInt32 freeSpaceOfst = ReadUInt32(&buff[48]);
			while (dataOfst < freeSpaceOfst)
			{
				if (ReadUInt32(&buff[dataOfst]) != 0x2A2A)
				{
					break;
				}
				UInt32 recSize = ReadUInt32(&buff[dataOfst + 4]);
				UInt64 evtRecId = ReadUInt64(&buff[dataOfst + 8]);
				ts = Data::Timestamp::FromFILETIME(&buff[dataOfst + 16], Data::DateTimeUtil::GetLocalTzQhr());
				sb.ClearStr();
				sb.AppendC(UTF8STRC("RecordId = "));
				sb.AppendU64(evtRecId);
				sbDetail.ClearStr();
				ParseBinXML(buff.Ptr(), dataOfst + 24, dataOfst + recSize - 28, sbDetail, 0, 0);
				logFile->AddLog(ts, sb.ToCString(), sbDetail.ToCString());

				dataOfst += recSize;
			}
		}
		ofst += 65536;
	}
	return logFile;
}

UOSInt Parser::FileParser::EVTXParser::ParseBinXML(UnsafeArray<const UInt8> chunk, UOSInt ofst, UOSInt endOfst, NN<Text::StringBuilderUTF8> sb, const UTF16Char *elementName, UOSInt nNameChar)
{
	UOSInt dataOfst;
	UInt32 dataSize;
	const UTF16Char *thisName;
	UInt16 nChar;
	while (ofst < endOfst)
	{
		UInt8 type = chunk[ofst];
		switch (type)
		{
		case 0x00:
#if defined(VERBOSE)
			printf("BinXML: %04x BinXmlTokenEOF\r\n", (UInt32)ofst);
#endif
			return ofst + 1;
		case 0x01:
		case 0x41:
		{
#if defined(VERBOSE)
			printf("BinXML: %04x BinXmlTokenOpenStartElementTag\r\n", (UInt32)ofst);
#endif
			//UInt16 depId = ReadUInt16(&chunk[ofst + 1]);
			dataSize = ReadUInt32(&chunk[ofst + 3]);
			ofst += 7;
			UOSInt dataEnd = ofst + dataSize;
			if (dataEnd > endOfst)
			{
				printf("BinXML: %04x BinXmlTokenOpenStartElementTag Parse error\r\n", (UInt32)ofst);
				return ofst;
			}
			dataOfst = ReadUInt32(&chunk[ofst]);
			sb->AppendUTF8Char('<');
			ofst += 4;
			if (dataOfst == ofst)
			{
				nChar = ReadUInt16(&chunk[ofst + 6]);
				thisName = (const UTF16Char*)&chunk[ofst + 8];
				ofst += 8 + (UOSInt)(nChar + 1) * 2;
			}
			else
			{
				nChar = ReadUInt16(&chunk[dataOfst + 6]);
				thisName = (const UTF16Char*)&chunk[dataOfst + 8];
			}
			sb->AppendUTF16(thisName, nChar);
			if (type == 0x41)
			{
				dataSize = ReadUInt32(&chunk[ofst]);
				ofst += 4;
				dataOfst = ParseBinXML(chunk, ofst, ofst + dataSize, sb, 0, 0);
				if (dataOfst == ofst + dataSize)
				{
					ofst = dataOfst;
					ofst = ParseBinXML(chunk, ofst, dataEnd, sb, thisName, nChar);
				}
			}
			else
			{
				ofst = ParseBinXML(chunk, ofst, dataEnd, sb, thisName, nChar);
			}
			ofst = dataEnd;
			break;
		}
		case 0x02:
#if defined(VERBOSE)
			printf("BinXML: %04x BinXmlTokenCloseStartElementTag\r\n", (UInt32)ofst);
#endif
			sb->AppendUTF8Char('>');
			ofst++;
			break;
		case 0x03:
#if defined(VERBOSE)
			printf("BinXML: %04x BinXmlTokenCloseEmptyElementTag\r\n", (UInt32)ofst);
#endif
			sb->AppendC(UTF8STRC("/>"));
			ofst++;
			break;
		case 0x04:
#if defined(VERBOSE)
			printf("BinXML: %04x BinXmlTokenEndElementTag\r\n", (UInt32)ofst);
#endif
			sb->AppendC(UTF8STRC("</"));
			if (elementName)
			{
				sb->AppendUTF16(elementName, nNameChar);
			}
			sb->AppendUTF8Char('>');
			ofst++;
			break;
		case 0x05:
#if defined(VERBOSE)
			printf("BinXML: %04x BinXmlTokenValue\r\n", (UInt32)ofst);
#endif
			nChar = ReadUInt16(&chunk[ofst + 2]);
			sb->AppendUTF16((const UTF16Char*)&chunk[ofst + 4], nChar);
			ofst += 4 + (UOSInt)nChar * 2;
			break;
		case 0x06:
		case 0x46:
		{
#if defined(VERBOSE)
			printf("BinXML: %04x BinXmlTokenAttribute\r\n", (UInt32)ofst);
#endif
			dataOfst = ReadUInt32(&chunk[ofst + 1]);
			ofst += 5;
			sb->AppendUTF8Char(' ');
			if (dataOfst == ofst)
			{
				UInt16 nChar = ReadUInt16(&chunk[ofst + 6]);
				sb->AppendUTF16((const UTF16Char*)&chunk[ofst + 8], nChar);
				ofst += 8 + (UOSInt)(nChar + 1) * 2;
			}
			else
			{
				UInt16 nChar = ReadUInt16(&chunk[dataOfst + 6]);
				sb->AppendUTF16((const UTF16Char*)&chunk[dataOfst + 8], nChar);
			}
			if (chunk[ofst] == 5 && chunk[ofst + 1] == 1)
			{
				UInt16 nChar = ReadUInt16(&chunk[ofst + 2]);
				sb->AppendUTF8Char('=');
				sb->AppendUTF8Char('"');
				sb->AppendUTF16((const UTF16Char*)&chunk[ofst + 4], nChar);
				sb->AppendUTF8Char('"');
				ofst += 4 + (UOSInt)nChar * 2;
			}
			else if (chunk[ofst] == 0x0e)
			{
				sb->AppendUTF8Char('=');
				sb->AppendUTF8Char('"');
				ofst = ParseBinXML(chunk, ofst, ofst + 4, sb, 0, 0);
				sb->AppendUTF8Char('"');
			}
			else
			{
				printf("BinXML: Unknown type in BinXmlTokenAttribute at %04x: %x\r\n", (UInt32)ofst, chunk[ofst]);
				return ofst;
			}
			break;
		}
		case 0x0c:
		{
#if defined(VERBOSE)
			printf("BinXML: %04x BinXmlTokenTemplateInstance\r\n", (UInt32)ofst);
#endif
			dataOfst = ReadUInt32(&chunk[ofst + 6]);
			ofst += 10;
			if (dataOfst == ofst)
			{
				ofst += 20;
			}
			dataSize = ReadUInt32(&chunk[ofst]);
			ofst += 4;
			dataOfst = ParseBinXML(chunk, ofst, ofst + dataSize - 1, sb, 0, 0);
			if (dataOfst != ofst + dataSize - 1)
			{
				printf("BinXML: %04x BinXmlTokenTemplateInstance parse error\r\n", (UInt32)ofst);
				return dataOfst;
			}
			ofst = dataOfst;
			break;
		}
		case 0x0e:
#if defined(VERBOSE)
			printf("BinXML: %04x BinXmlTokenOptionalSubstitution %d, Type = %d\r\n", (UInt32)ofst, ReadUInt16(&chunk[ofst + 1]), chunk[ofst + 3]);
#endif
			sb->AppendUTF8Char('?');
			ofst += 4;
			break;
		case 0x0f:
#if defined(VERBOSE)
			printf("BinXML: %04x BinXmlFragmentHeaderToken\r\n", (UInt32)ofst);
#endif
			ofst += 4;
			break;
		default:
			printf("BinXML: Unknown type at %04x: %x\r\n", (UInt32)ofst, chunk[ofst]);
			return ofst;
		}
	}
	return ofst;
}
