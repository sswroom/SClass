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

void Parser::FileParser::M2VStmParser::PrepareSelector(IO::IFileSelector *selector, IO::ParsedObject::ParserType t)
{
	if (t == IO::ParsedObject::PT_UNKNOWN || t == IO::ParsedObject::PT_VIDEO_PARSER)
	{
		selector->AddFilter((const UTF8Char*)"*.m2v", (const UTF8Char*)"MPEG-2 Video Elemental File");
	}
}

IO::ParsedObject::ParserType Parser::FileParser::M2VStmParser::GetParserType()
{
	return IO::ParsedObject::PT_VIDEO_PARSER;
}

IO::ParsedObject *Parser::FileParser::M2VStmParser::ParseFile(IO::IStreamData *fd, IO::PackageFile *pkgFile, IO::ParsedObject::ParserType targetType)
{
	UInt8 tmpBuff[1024];
	UOSInt readSize = fd->GetRealData(0, 1024, tmpBuff);
	Media::FrameInfo info;
	Int32 frameRateNorm;
	Int32 frameRateDenorm;
	UInt64 bitRate;
	if (!Media::MPEGVideoParser::GetFrameInfo(tmpBuff, readSize, &info, &frameRateNorm, &frameRateDenorm, &bitRate, false))
		return 0;

	Media::MediaFile *file;
	NEW_CLASS(file, Media::M2VFile(fd));
	return file;
}
