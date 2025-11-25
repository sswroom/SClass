#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ByteBuffer.h"
#include "Core/ByteTool_C.h"
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

void Parser::FileParser::LUTParser::PrepareSelector(NN<IO::FileSelector> selector, IO::ParserType t)
{
	if (t == IO::ParserType::Unknown || t == IO::ParserType::LUT)
	{
		selector->AddFilter(CSTR("*.3dlut"), CSTR("3D LUT File"));
	}
}

IO::ParserType Parser::FileParser::LUTParser::GetParserType()
{
	return IO::ParserType::LUT;
}

Optional<IO::ParsedObject> Parser::FileParser::LUTParser::ParseFileHdr(NN<IO::StreamData> fd, Optional<IO::PackageFile> pkgFile, IO::ParserType targetType, Data::ByteArrayR hdr)
{
	if (*(Int32*)&hdr[0] != *(Int32*)"3DLT" || ReadInt32(&hdr[4]) != 1 || ReadInt32(&hdr[84]) != 0)
	{
		return 0;
	}
	UInt32 inputBpp = ReadUInt32(&hdr[48]);
	UInt32 outputBpp = ReadUInt32(&hdr[64]);
	UInt32 paramOfst = ReadUInt32(&hdr[72]);
	UInt32 paramSize = ReadUInt32(&hdr[76]);
	UInt32 lutOfst = ReadUInt32(&hdr[80]);
	UInt32 lutSize = ReadUInt32(&hdr[92]);
	UInt32 inpLev = (UInt32)(1 << inputBpp);
//	Int32 outpLev = (1 << outputBpp);
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

	
	NEW_CLASS(lut, Media::LUT(3, inpLev, 3, fmt, fd->GetFullName()));
	Data::ByteBuffer paramBuff(paramSize + 1);
	paramBuff[paramSize] = 0;
	fd->GetRealData(paramOfst, paramSize, paramBuff);
	lut->SetRemark(Text::CString(paramBuff.Arr().Ptr(), paramSize));

	Data::ByteArray lutTable = lut->GetTableArray();
	fd->GetRealData(lutOfst, lutSize, lutTable);
	return lut;
}
