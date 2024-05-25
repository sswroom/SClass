#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ByteBuffer.h"
#include "Data/ByteTool.h"
#include "IO/VirtualPackageFileFast.h"
#include "Parser/FileParser/TILParser.h"
#include "Text/Encoding.h"
#include "Text/MyString.h"

Parser::FileParser::TILParser::TILParser()
{
}

Parser::FileParser::TILParser::~TILParser()
{
}

Int32 Parser::FileParser::TILParser::GetName()
{
	return *(Int32*)"TILP";
}

void Parser::FileParser::TILParser::PrepareSelector(NN<IO::FileSelector> selector, IO::ParserType t)
{
	if (t == IO::ParserType::Unknown || t == IO::ParserType::PackageFile)
	{
		selector->AddFilter(CSTR("*.til"), CSTR("TimedImageList File"));
	}
}

IO::ParserType Parser::FileParser::TILParser::GetParserType()
{
	return IO::ParserType::PackageFile;
}

Optional<IO::ParsedObject> Parser::FileParser::TILParser::ParseFileHdr(NN<IO::StreamData> fd, Optional<IO::PackageFile> pkgFile, IO::ParserType targetType, Data::ByteArrayR hdr)
{
	UInt64 dirOfst;
	UInt64 fileSize;
	Int32 flags;
	UTF8Char fileName[256];
	UTF8Char *srcPtr;
	IO::VirtualPackageFile *pf;

	if (hdr[0] != 'S' || hdr[1] != 'T' || hdr[2] != 'i' || hdr[3] != 'l')
		return 0;
	
	Data::DateTime dt;
	NEW_CLASS(pf, IO::VirtualPackageFileFast(fd->GetFullName()));
	dirOfst = ReadUInt64(&hdr[8]);
	flags = ReadInt32(&hdr[4]);
	fileSize = fd->GetDataSize();
	if (flags & 2)
	{
		if (dirOfst < 16 || dirOfst > fileSize || ((fileSize - dirOfst) & 31) != 0)
		{
			DEL_CLASS(pf);
			return 0;
		}
		UOSInt indexSize = (UOSInt)(fileSize - dirOfst);
		UOSInt i;
		Data::ByteBuffer indexBuff(indexSize);
		fd->GetRealData(dirOfst, indexSize, indexBuff);
		i = 0;
		while (i < indexSize)
		{
			Int32 imgType;
			Int64 timeTicks = ReadInt64(&indexBuff[i]);
			dt.SetTicks(timeTicks);
			dt.ToLocalTime();
			srcPtr = dt.ToString(fileName, "yyyyMMdd HHmmss");
			imgType = ReadInt32(&indexBuff[i + 12]);
			if (imgType == 0)
			{
				srcPtr = Text::StrConcatC(srcPtr, UTF8STRC(".jpg"));
			}
			pf->AddData(fd, ReadUInt64(&indexBuff[i + 16]), ReadUInt64(&indexBuff[i + 24]), IO::PackFileItem::HeaderType::No, CSTRP(fileName, srcPtr), Data::Timestamp(timeTicks, 0), 0, 0, 0);
			i += 32;
		}
	}
	else if (flags & 1)
	{
		UInt8 indexBuff[32];
		dirOfst = 16;
		while (dirOfst < fileSize)
		{
			if (fd->GetRealData(dirOfst, 32, BYTEARR(indexBuff)) != 32)
				break;

			UInt64 fileOfst;
			Int32 imgType;
			Int64 timeTicks = ReadInt64(&indexBuff[0]);
			dt.SetTicks(timeTicks);
			dt.ToLocalTime();
			fileOfst = ReadUInt64(&indexBuff[16]);
			if (fileOfst != dirOfst + 32)
				break;
			srcPtr = dt.ToString(fileName, "yyyyMMdd HHmmss");
			imgType = ReadInt32(&indexBuff[12]);
			if (imgType == 0)
			{
				srcPtr = Text::StrConcatC(srcPtr, UTF8STRC(".jpg"));
			}
			pf->AddData(fd, fileOfst, ReadUInt64(&indexBuff[24]), IO::PackFileItem::HeaderType::No, CSTRP(fileName, srcPtr), Data::Timestamp(timeTicks, 0), 0, 0, 0);
		}
	}
	else
	{
		DEL_CLASS(pf);
		return 0;
	}
	return pf;
}
