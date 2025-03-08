#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/StmData/MemoryDataRef.h"
#include "Media/ImageList.h"
#include "Media/JPEGDecoder.h"
#include "Media/JPEGFile.h"
#include "Parser/FileParser/JPGParser.h"

Parser::FileParser::JPGParser::JPGParser()
{
}

Parser::FileParser::JPGParser::~JPGParser()
{
}

Int32 Parser::FileParser::JPGParser::GetName()
{
	return *(Int32*)"JPGP";
}

void Parser::FileParser::JPGParser::PrepareSelector(NN<IO::FileSelector> selector, IO::ParserType t)
{
	if (t == IO::ParserType::Unknown || t == IO::ParserType::ImageList)
	{
		selector->AddFilter(CSTR("*.jpg"), CSTR("JPG Image"));
		selector->AddFilter(CSTR("*.jpeg"), CSTR("JPEG Image"));
	}
}

IO::ParserType Parser::FileParser::JPGParser::GetParserType()
{
	return IO::ParserType::ImageList;
}

Optional<IO::ParsedObject> Parser::FileParser::JPGParser::ParseFileHdr(NN<IO::StreamData> fd, Optional<IO::PackageFile> pkgFile, IO::ParserType targetType, Data::ByteArrayR hdr)
{
	if (hdr[0] != 0xff || hdr[1] != 0xd8 || fd->GetDataSize() >= 104857600)
	{
		return 0;
	}
	UOSInt fileLen = (UOSInt)fd->GetDataSize();
	UInt8 *buff = MemAlloc(UInt8, fileLen);
	if (fd->GetRealData(0, fileLen, Data::ByteArray(buff, fileLen)) != fileLen)
	{
		MemFree(buff);
		return 0;
	}
	
	NN<Media::StaticImage> img;
	Media::JPEGDecoder jpgDecoder;
	if (!jpgDecoder.DecodeImage(Data::ByteArrayR(buff, fileLen)).SetTo(img))
	{
		MemFree(buff);
		return 0;
	}
	NN<Media::ImageList> imgList;
	NEW_CLASSNN(imgList, Media::ImageList(fd->GetFullName()));
	imgList->AddImage(img, 0);
	{
		IO::StmData::MemoryDataRef fd(buff, fileLen);
		Media::JPEGFile::ParseJPEGHeader(fd, img, imgList, 0);
	}
	MemFree(buff);
	return imgList;
}
