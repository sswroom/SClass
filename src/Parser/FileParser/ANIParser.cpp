#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ByteBuffer.h"
#include "Data/ByteTool.h"
#include "Parser/FileParser/ANIParser.h"
#include "Media/ImageList.h"
#include "Text/Encoding.h"

Parser::FileParser::ANIParser::ANIParser()
{
}

Parser::FileParser::ANIParser::~ANIParser()
{
}

Int32 Parser::FileParser::ANIParser::GetName()
{
	return *(Int32*)"ANIP";
}

void Parser::FileParser::ANIParser::PrepareSelector(IO::FileSelector *selector, IO::ParserType t)
{
	if (t == IO::ParserType::Unknown || t == IO::ParserType::ImageList)
	{
		selector->AddFilter(CSTR("*.ani"), CSTR("ANI File"));
	}
}

IO::ParserType Parser::FileParser::ANIParser::GetParserType()
{
	return IO::ParserType::ImageList;
}

IO::ParsedObject *Parser::FileParser::ANIParser::ParseFileHdr(IO::StreamData *fd, IO::PackageFile *pkgFile, IO::ParserType targetType, const UInt8 *hdr)
{
	UTF8Char sbuff[256];
	UInt8 riffHdr[24];
	UInt32 aniSize;
	Media::ImageList *imgList;
	UOSInt buffSize;
	UOSInt buffOfst;
	UInt32 tmp;
	UInt32 displayRate = 0;
	UInt32 nFrames = 0;
	Media::ImageList *currImage = 0;

	if (ReadNUInt32(&hdr[0]) != *(UInt32*)"RIFF" || ReadNUInt32(&hdr[8]) != *(UInt32*)"ACON")
	{
		return 0;
	}
	aniSize = ReadUInt32(&hdr[4]) + 8;
	UOSInt currOfst = 12;
	NEW_CLASS(imgList, Media::ImageList(fd->GetFullName()));

	while (currOfst < aniSize)
	{
		fd->GetRealData(currOfst, 12, BYTEARR(riffHdr));
		if (ReadNUInt32(&riffHdr[0]) == *(UInt32*)"LIST" && ReadNUInt32(&riffHdr[8]) == *(UInt32*)"INFO")
		{
			buffSize = ReadUInt32(&riffHdr[4]) - 4;
			Data::ByteBuffer buff(buffSize);
			buffOfst = 0;
			fd->GetRealData(currOfst + 12, buffSize, buff);
			while (buffOfst < buffSize)
			{
				tmp = *(UInt32*)&buff[buffOfst];
				if (tmp == *(UInt32*)"INAM")
				{
					Text::StrConcatC(sbuff, &buff[buffOfst + 8], *(UInt32*)&buff[buffOfst + 4]);
					imgList->SetImageName(sbuff);
				}
				else if (tmp == *(UInt32*)"IART")
				{
					Text::StrConcatC(sbuff, &buff[buffOfst + 8], *(UInt32*)&buff[buffOfst + 4]);
					imgList->SetAuthor(sbuff);
				}
				buffOfst += *(UInt32*)&buff[buffOfst + 4] + 8;
				if (buffOfst & 1)
				{
					buffOfst += 1;
				}
			}
		}
		else if (ReadNUInt32(&riffHdr[0]) == *(UInt32*)"anih")
		{
			buffSize = ReadUInt32(&riffHdr[4]);
			Data::ByteBuffer buff(buffSize);
			fd->GetRealData(currOfst + 8, buffSize, buff);
			displayRate = ReadUInt32(&buff[28]);
			nFrames = ReadUInt32(&buff[4]);
		}
		else if (ReadNUInt32(&riffHdr[0]) == *(UInt32*)"LIST" && ReadNUInt32(&riffHdr[8]) == *(UInt32*)"fram")
		{
			buffSize = ReadUInt32(&riffHdr[4]) - 4;
			Data::ByteBuffer buff(8);
			buffOfst = 0;
			while (buffOfst < buffSize)
			{
				fd->GetRealData(currOfst + 12 + buffOfst, 8, buff);
				tmp = *(UInt32*)&buff[0];
				if (tmp == *(UInt32*)"icon")
				{
					if (currImage)
					{
						UInt32 imgTime;
						Media::Image *img = currImage->GetImage(0, &imgTime)->Clone();
						imgList->AddImage(img, MulDivU32(displayRate, 1000, 60));
						DEL_CLASS(currImage);
					}
					IO::StreamData *data = fd->GetPartialData(currOfst + 20 + buffOfst, *(UInt32*)&buff[4]);
					currImage = (Media::ImageList *)this->icop.ParseFile(data, pkgFile, IO::ParserType::ImageList);
					DEL_CLASS(data);
				}

				buffOfst += ReadUInt32(&buff[4]) + 8;
				if (buffOfst & 1)
				{
					buffOfst += 1;
				}
			}

			if (currImage)
			{
				UInt32 imgTime;
				Media::Image *img = currImage->GetImage(0, &imgTime)->Clone();
				imgList->AddImage(img, MulDivU32(displayRate, 1000, 60));
				DEL_CLASS(currImage);
				currImage = 0;
			}
		}
		currOfst += ReadUInt32(&riffHdr[4]) + 8;
		if (currOfst & 1)
		{
			currOfst += 1;
		}
	}

	if (nFrames != imgList->GetCount())
	{
		DEL_CLASS(imgList);
		return 0;
	}

	return imgList;
}
