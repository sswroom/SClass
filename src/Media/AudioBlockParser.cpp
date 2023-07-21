#include "Stdafx.h"
#include "MyMemory.h"
#include "Media/AudioBlockParser.h"
#include "Media/BlockParser/AC3BlockParser.h"
#include "Media/BlockParser/MP2BlockParser.h"
#include "Media/BlockParser/MP3BlockParser.h"

Media::AudioBlockParser::AudioBlockParser()
{
}

Media::AudioBlockParser::~AudioBlockParser()
{
}

Media::AudioBlockParser *Media::AudioBlockParser::CreateParser(NotNullPtr<IO::StreamData> stmData)
{
	UInt8 buff[256];
	Media::AudioBlockParser *parser;

	stmData->GetRealData(0, 256, BYTEARR(buff));
	if (buff[0] == 0x0b && buff[1] == 0x77) //AC3
	{
		NEW_CLASS(parser, Media::BlockParser::AC3BlockParser());
		return parser;
	}
	else if (buff[0] == 0xff && (buff[1] & 0xfe) == 0xfa) //MP3
	{
		NEW_CLASS(parser, Media::BlockParser::MP3BlockParser());
		return parser;
	}
	else if (buff[0] == 0xff && (buff[1] & 0xfe) == 0xfc) //MP2
	{
		NEW_CLASS(parser, Media::BlockParser::MP2BlockParser());
		return parser;
	}
	return 0;
}
