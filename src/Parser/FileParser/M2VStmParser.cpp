#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ByteTool.h"
#include "Media/M2VFile.h"
#include "Media/MPEGVideoParser.h"
#include "Parser/FileParser/M2VStmParser.h"

Parser::FileParser::M2VStmParser::M2VStmParser()
{
}

Parser::FileParser::M2VStmParser::~M2VStmParser()
{
}

Int32 Parser::FileParser::M2VStmParser::GetName()
{
	return *(Int32*)"M2VS";
}

void Parser::FileParser::M2VStmParser::PrepareSelector(NN<IO::FileSelector> selector, IO::ParserType t)
{
	if (t == IO::ParserType::Unknown || t == IO::ParserType::MediaFile)
	{
		selector->AddFilter(CSTR("*.m2v"), CSTR("MPEG-2 Video Elemental File"));
	}
}

IO::ParserType Parser::FileParser::M2VStmParser::GetParserType()
{
	return IO::ParserType::MediaFile;
}

IO::ParsedObject *Parser::FileParser::M2VStmParser::ParseFileHdr(NN<IO::StreamData> fd, IO::PackageFile *pkgFile, IO::ParserType targetType, const UInt8 *hdr)
{
	if (ReadMInt32(hdr) != 0x1b3)
		return 0;
	UInt8 tmpBuff[1024];
	UOSInt readSize = fd->GetRealData(0, 1024, BYTEARR(tmpBuff));
	UInt32 frameRateNorm;
	UInt32 frameRateDenorm;
	UInt64 bitRate;
	Media::FrameInfo info;
	if (!Media::MPEGVideoParser::GetFrameInfo(tmpBuff, readSize, info, frameRateNorm, frameRateDenorm, &bitRate, false))
		return 0;

	Media::MediaFile *file;
	NEW_CLASS(file, Media::M2VFile(fd));
	return file;
}
