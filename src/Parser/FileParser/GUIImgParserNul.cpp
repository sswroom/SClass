#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/FileStream.h"
#include "IO/Path.h"
#include "IO/StreamDataStream.h"
#include "IO/StreamReader.h"
#include "Map/VectorLayer.h"
#include "Media/ImageCopy.h"
#include "Media/ImageList.h"
#include "Media/JPEGFile.h"
#include "Media/SharedImage.h"
#include "Media/StaticImage.h"
#include "Parser/FileParser/GUIImgParser.h"
#include "Text/MyString.h"

Parser::FileParser::GUIImgParser::GUIImgParser()
{
}

Parser::FileParser::GUIImgParser::~GUIImgParser()
{
}

Int32 Parser::FileParser::GUIImgParser::GetName()
{
	return *(Int32*)"GUII";
}

void Parser::FileParser::GUIImgParser::SetParserList(Parser::ParserList *parsers)
{
	this->parsers = parsers;
}

void Parser::FileParser::GUIImgParser::PrepareSelector(NotNullPtr<IO::FileSelector> selector, IO::ParserType t)
{
}

IO::ParserType Parser::FileParser::GUIImgParser::GetParserType()
{
	return IO::ParserType::ImageList;
}

IO::ParsedObject *Parser::FileParser::GUIImgParser::ParseFileHdr(NotNullPtr<IO::StreamData> fd, IO::PackageFile *pkgFile, IO::ParserType targetType, const UInt8 *hdr)
{
	return 0;
}

