#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ByteTool.h"
#include "IO/PackageFile.h"
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

void Parser::FileParser::TILParser::PrepareSelector(IO::IFileSelector *selector, IO::ParsedObject::ParserType t)
{
	if (t == IO::ParsedObject::PT_UNKNOWN || t == IO::ParsedObject::PT_PACKAGE_PARSER)
	{
		selector->AddFilter((const UTF8Char*)"*.til", (const UTF8Char*)"TimedImageList File");
	}
}

IO::ParsedObject::ParserType Parser::FileParser::TILParser::GetParserType()
{
	return IO::ParsedObject::PT_PACKAGE_PARSER;
}

IO::ParsedObject *Parser::FileParser::TILParser::ParseFile(IO::IStreamData *fd, IO::PackageFile *pkgFile, IO::ParsedObject::ParserType targetType)
{
	UInt8 hdrBuff[16];
	UInt64 dirOfst;
	UInt64 fileSize;
	Int32 flags;
	UTF8Char fileName[256];
	UTF8Char *srcPtr;
	IO::PackageFile *pf;

	if (fd->GetRealData(0, 16, hdrBuff) != 16)
		return 0;
	if (hdrBuff[0] != 'S' || hdrBuff[1] != 'T' || hdrBuff[2] != 'i' || hdrBuff[3] != 'l')
		return 0;
	
	Data::DateTime dt;
	NEW_CLASS(pf, IO::PackageFile(fd->GetFullName()));
	dirOfst = ReadUInt64(&hdrBuff[8]);
	flags = ReadInt32(&hdrBuff[4]);
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
		UInt8 *indexBuff = MemAlloc(UInt8, indexSize);
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
				srcPtr = Text::StrConcat(srcPtr, (const UTF8Char*)".jpg");
			}
			pf->AddData(fd, ReadUInt64(&indexBuff[i + 16]), ReadUInt64(&indexBuff[i + 24]), fileName, timeTicks);
			i += 32;
		}
		MemFree(indexBuff);
	}
	else if (flags & 1)
	{
		UInt8 indexBuff[32];
		dirOfst = 16;
		while (dirOfst < fileSize)
		{
			if (fd->GetRealData(dirOfst, 32, indexBuff) != 32)
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
				srcPtr = Text::StrConcat(srcPtr, (const UTF8Char*)".jpg");
			}
			pf->AddData(fd, fileOfst, ReadUInt64(&indexBuff[24]), fileName, timeTicks);
		}
	}
	else
	{
		DEL_CLASS(pf);
		return 0;
	}
	return pf;
}
