#include "Stdafx.h"
#include "MyMemory.h"
#include "Parser/FileParser/ID3Parser.h"
#include "Text/Encoding.h"
#include "Text/MyString.h"
#include "Media/MediaFile.h"
#include "Media/AudioBlockSource.h"
#include "Media/BlockParser/MP3BlockParser.h"

Parser::FileParser::ID3Parser::ID3Parser()
{
}

Parser::FileParser::ID3Parser::~ID3Parser()
{
}

Int32 Parser::FileParser::ID3Parser::GetName()
{
	return *(Int32*)"ID3P";
}

void Parser::FileParser::ID3Parser::SetCodePage(UInt32 codePage)
{
	this->codePage = codePage;
}

void Parser::FileParser::ID3Parser::PrepareSelector(IO::IFileSelector *selector, IO::ParsedObject::ParserType t)
{
	if (t == IO::ParsedObject::PT_UNKNOWN || t == IO::ParsedObject::PT_VIDEO_PARSER)
	{
		selector->AddFilter((const UTF8Char*)"*.mp3", (const UTF8Char*)"MP3(with ID3 Tag) File");
	}
}

IO::ParsedObject::ParserType Parser::FileParser::ID3Parser::GetParserType()
{
	return IO::ParsedObject::PT_VIDEO_PARSER;
}

IO::ParsedObject *Parser::FileParser::ID3Parser::ParseFile(IO::IStreamData *fd, IO::PackageFile *pkgFile, IO::ParsedObject::ParserType targetType)
{
	UInt8 buff[256];
	UInt32 headerSize;
	fd->GetRealData(0, 32, buff);
	if (buff[0] != 'I' || buff[1] != 'D' || buff[2] != '3')
		return 0;
	
	headerSize = ReadUSInt32(&buff[6]);
	Media::AudioBlockSource *src = 0;
	Media::MediaFile *vid;
	Media::BlockParser::MP3BlockParser mp3Parser;
	IO::IStreamData *data = fd->GetPartialData(headerSize + 10, fd->GetDataSize() - headerSize - 10);
	src = mp3Parser.ParseStreamData(data);
	DEL_CLASS(data);
	if (src)
	{
		NEW_CLASS(vid, Media::MediaFile(fd->GetFullName()));
		vid->AddSource(src, 0);
		return vid;
	}
	else
	{
		return 0;
	}
}

UInt32 Parser::FileParser::ID3Parser::ReadUSInt32(UInt8 *buff)
{
	return (UInt32)((buff[0] << 21) | (buff[1] << 14) | (buff[2] << 7) | buff[3]);
}
