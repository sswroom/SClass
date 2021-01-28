#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ByteTool.h"
#include "Media/LUT.h"
#include "Parser/FileParser/LUTParser.h"

Parser::FileParser::LUTParser::LUTParser()
{
}

Parser::FileParser::LUTParser::~LUTParser()
{
}

Int32 Parser::FileParser::LUTParser::GetName()
{
	return *(Int32*)"LUTP";
}

void Parser::FileParser::LUTParser::PrepareSelector(IO::IFileSelector *selector, IO::ParsedObject::ParserType t)
{
	if (t == IO::ParsedObject::PT_UNKNOWN || t == IO::ParsedObject::PT_LUT)
	{
		selector->AddFilter((const UTF8Char*)"*.3dlut", (const UTF8Char*)"3D LUT File");
	}
}

IO::ParsedObject::ParserType Parser::FileParser::LUTParser::GetParserType()
{
	return IO::ParsedObject::PT_LUT;
}

IO::ParsedObject *Parser::FileParser::LUTParser::ParseFile(IO::IStreamData *fd, IO::PackageFile *pkgFile, IO::ParsedObject::ParserType targetType)
{
	UInt8 hdrBuff[96];
	fd->GetRealData(0, 96, hdrBuff);
	if (*(Int32*)&hdrBuff[0] != *(Int32*)"3DLT" || ReadInt32(&hdrBuff[4]) != 1 || ReadInt32(&hdrBuff[84]) != 0)
	{
		return 0;
	}
	Int32 inputBpp = ReadInt32(&hdrBuff[48]);
	Int32 outputBpp = ReadInt32(&hdrBuff[64]);
	Int32 paramOfst = ReadInt32(&hdrBuff[72]);
	Int32 paramSize = ReadInt32(&hdrBuff[76]);
	Int32 lutOfst = ReadInt32(&hdrBuff[80]);
	Int32 lutSize = ReadInt32(&hdrBuff[92]);
	Int32 inpLev = (1 << inputBpp);
//	Int32 outpLev = (1 << outputBpp);
	UInt8 *lutTable;
	if (inpLev * inpLev * inpLev * 3 * (outputBpp >> 3) != lutSize)
	{
		return 0;
	}
	Media::LUT::DataFormat fmt;
	Media::LUT *lut;
	if (outputBpp == 8)
	{
		fmt = Media::LUT::DF_UINT8;
	}
	else if (outputBpp == 16)
	{
		fmt = Media::LUT::DF_UINT16;
	}
	else if (outputBpp == 32)
	{
		fmt = Media::LUT::DF_SINGLE;
	}
	else
	{
		return 0;
	}

	UInt8 *paramBuff;
	NEW_CLASS(lut, Media::LUT(3, inpLev, 3, fmt, fd->GetFullName()));
	paramBuff = MemAlloc(UInt8, paramSize + 1);
	paramBuff[paramSize] = 0;
	fd->GetRealData(paramOfst, paramSize, paramBuff);
	MemFree(paramBuff);
	lut->SetRemark((UTF8Char*)paramBuff);

	lutTable = lut->GetTablePtr();
	fd->GetRealData(lutOfst, lutSize, lutTable);
	return lut;
}
