#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ByteTool.h"
#include "Data/StringUTF8Map.h"
#include "IO/PackageFile.h"
#include "IO/Path.h"
#include "Parser/FileParser/RAR5Parser.h"

const UInt8 *Parser::FileParser::RAR5Parser::ReadVInt(const UInt8 *buffPtr, UInt64 *val)
{
	UInt64 v = 0;
	OSInt sh = 0;
	UInt8 b;
	while (true)
	{
		b = *buffPtr++;
		v |= ((UInt64)(b & 0x7f)) << sh;
		if ((b & 0x80) == 0)
		{
			break;
		}
		sh += 7;
	}
	*val = v;
	return buffPtr;
}

Parser::FileParser::RAR5Parser::RAR5Parser()
{
}

Parser::FileParser::RAR5Parser::~RAR5Parser()
{
}

Int32 Parser::FileParser::RAR5Parser::GetName()
{
	return *(Int32*)"RAR5";
}

void Parser::FileParser::RAR5Parser::PrepareSelector(NotNullPtr<IO::FileSelector> selector, IO::ParserType t)
{
	if (t == IO::ParserType::Unknown || t == IO::ParserType::PackageFile)
	{
		selector->AddFilter(CSTR("*.rar"), CSTR("RAR File"));
	}
}

IO::ParserType Parser::FileParser::RAR5Parser::GetParserType()
{
	return IO::ParserType::PackageFile;
}

IO::ParsedObject *Parser::FileParser::RAR5Parser::ParseFileHdr(NotNullPtr<IO::StreamData> fd, IO::PackageFile *pkgFile, IO::ParserType targetType, const UInt8 *hdr)
{
	UTF8Char sbuff[512];
	UTF8Char *sptr;
	UInt8 buff[512];
	const UInt8 *buffPtr;
	const UInt8 *nextPtr;
	const UInt8 *extraEnd;
	UInt64 iVal;
	UInt64 headerFlags;
	UInt64 currOfst = 8;
	UInt64 endOfst = fd->GetDataSize();
	UInt32 headerSize;
	UInt64 extraSize;
	UInt64 dataSize;
	UInt32 headerType;

	if (ReadInt32(&hdr[0]) != 0x21726152 || ReadInt32(&hdr[4]) != 0x0001071A)
	{
		return 0;
	}
	IO::PackageFile *pf;
	Data::DateTime dt;
	dt.ToLocalTime();
	NEW_CLASS(pf, IO::PackageFile(fd->GetFullName()));

	while (currOfst + 8 <= endOfst)
	{
		fd->GetRealData(currOfst, 512, BYTEARR(buff));
		buffPtr = buff + 4;
		buffPtr = ReadVInt(buffPtr, &iVal);
		if (iVal >= 1048576 * 2)
		{
			DEL_CLASS(pf);
			return 0;
		}
		headerSize = (UInt32)(iVal + (UOSInt)(buffPtr - buff));
		buffPtr = ReadVInt(buffPtr, &iVal);
		if (iVal > 5)
		{
			DEL_CLASS(pf);
			return 0;
		}
		headerType = (UInt32)iVal;
		buffPtr = ReadVInt(buffPtr, &iVal);
		headerFlags = iVal;
		extraSize = 0;
		if (headerFlags & 1)
		{
			buffPtr = ReadVInt(buffPtr, &extraSize); //extraSize;
		}
		if (headerFlags & 2)
		{
			buffPtr = ReadVInt(buffPtr, &iVal);
			dataSize = iVal;
		}
		else
		{
			dataSize = 0;
		}

		if (headerType == 2)
		{
			UInt64 compInfo;
			UInt64 unpackedSize;
			UInt32 dataCRC = 0;
			buffPtr = ReadVInt(buffPtr, &headerFlags);
			buffPtr = ReadVInt(buffPtr, &unpackedSize);
			buffPtr = ReadVInt(buffPtr, &iVal); //Attributes
			dt.SetTicks(0);
			if (headerFlags & 2)
			{
				dt.SetUnixTimestamp(ReadUInt32(buffPtr));
				buffPtr += 4;
			}
			if (headerFlags & 4)
			{
				dataCRC = ReadUInt32(buffPtr);
				buffPtr += 4;
			}
			buffPtr = ReadVInt(buffPtr, &compInfo);
			buffPtr = ReadVInt(buffPtr, &iVal); //host OS
			buffPtr = ReadVInt(buffPtr, &iVal); //name length
			sptr = Text::StrConcatC(sbuff, buffPtr, (UOSInt)iVal); //name
			buffPtr += iVal; //name

			extraEnd = buffPtr + (OSInt)extraSize;
			if (extraEnd <= buff + headerSize)
			{
				while (buffPtr < extraEnd)
				{
					buffPtr = ReadVInt(buffPtr, &extraSize);
					nextPtr = buffPtr + extraSize;
					if (nextPtr > extraEnd)
					{
						break;
					}
					buffPtr = ReadVInt(buffPtr, &iVal);
					if (iVal == 1)
					{
					}
					else if (iVal == 2)
					{
					}
					else if (iVal == 3)
					{
						buffPtr = ReadVInt(buffPtr, &headerFlags);
						if (headerFlags & 2)
						{
							if (headerFlags & 1)
							{
								dt.SetUnixTimestamp(ReadUInt32(buffPtr));
								buffPtr += 4;
							}
							else
							{
								dt.SetValueFILETIME((void*)buffPtr);
								buffPtr += 8;
							}
						}
					}
					else if (iVal == 4)
					{
					}
					else if (iVal == 5)
					{
					}
					else if (iVal == 6)
					{
					}
					else if (iVal == 7)
					{
					}
					buffPtr = nextPtr;
				}
			}

	/*		sb->AppendC(UTF8STRC("\r\nCompression version = "));
			sb->AppendU32(iVal & 0x3f);
			sb->AppendC(UTF8STRC("\r\nCompression Solid Flag = "));
			sb->AppendU32((iVal & 0x40) >> 6);
			sb->AppendC(UTF8STRC("\r\nCompression method = "));
			sb->AppendU32((iVal & 0x380) >> 7);
			sb->AppendC(UTF8STRC("\r\nCompression dir size = "));
			sb->AppendU32((iVal & 0x3c00) >> 10);*/
			if (((compInfo & 0x380) >> 7) == 0)
			{
				pf->AddData(fd, currOfst + headerSize, dataSize, CSTRP(sbuff, sptr), Data::Timestamp(dt.ToInstant(), dt.GetTimeZoneQHR()), 0, 0);
			}
			else
			{
				IO::PackFileItem::CompressInfo cinfo;
				cinfo.checkMethod = Crypto::Hash::HashType::CRC32R_IEEE;
				WriteMUInt32(cinfo.checkBytes, dataCRC);
				cinfo.compExtras = 0;
				cinfo.compExtraSize = 0;
				cinfo.compFlags = (Int32)compInfo;
				cinfo.compMethod = Data::Compress::Decompressor::CM_UNKNOWN;
				cinfo.decSize = unpackedSize;
				pf->AddCompData(fd, currOfst + headerSize, dataSize, &cinfo, CSTRP(sbuff, sptr), Data::Timestamp(dt.ToInstant(), dt.GetTimeZoneQHR()), 0, 0);
			}
		}
		currOfst += headerSize + dataSize;
	}

	return pf;
}
