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

void Parser::FileParser::ID3Parser::PrepareSelector(NN<IO::FileSelector> selector, IO::ParserType t)
{
	if (t == IO::ParserType::Unknown || t == IO::ParserType::MediaFile)
	{
		selector->AddFilter(CSTR("*.mp3"), CSTR("MP3(with ID3 Tag) File"));
	}
}

IO::ParserType Parser::FileParser::ID3Parser::GetParserType()
{
	return IO::ParserType::MediaFile;
}

IO::ParsedObject *Parser::FileParser::ID3Parser::ParseFileHdr(NN<IO::StreamData> fd, IO::PackageFile *pkgFile, IO::ParserType targetType, const UInt8 *hdr)
{
	UInt32 headerSize;
	if (hdr[0] != 'I' || hdr[1] != 'D' || hdr[2] != '3')
		return 0;
	
	headerSize = ReadUSInt32(&hdr[6]);
	Media::AudioBlockSource *src = 0;
	Media::MediaFile *vid;
	Media::BlockParser::MP3BlockParser mp3Parser;
	NN<IO::StreamData> data = fd->GetPartialData(headerSize + 10, fd->GetDataSize() - headerSize - 10);
	src = mp3Parser.ParseStreamData(data);
	data.Delete();
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

UInt32 Parser::FileParser::ID3Parser::ReadUSInt32(const UInt8 *buff)
{
	return (UInt32)((buff[0] << 21) | (buff[1] << 14) | (buff[2] << 7) | buff[3]);
}
