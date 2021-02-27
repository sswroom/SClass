#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ByteTool.h"
#include "Parser/FileParser/ANIParser.h"
#include "Media/ImageList.h"
#include "Text/Encoding.h"

Parser::FileParser::ANIParser::ANIParser()
{
	NEW_CLASS(icop, Parser::FileParser::ICOParser());
}

Parser::FileParser::ANIParser::~ANIParser()
{
	DEL_CLASS(icop);
}

Int32 Parser::FileParser::ANIParser::GetName()
{
	return *(Int32*)"ANIP";
}

void Parser::FileParser::ANIParser::PrepareSelector(IO::IFileSelector *selector, IO::ParsedObject::ParserType t)
{
	if (t == IO::ParsedObject::PT_UNKNOWN || t == IO::ParsedObject::PT_IMAGE_LIST_PARSER)
	{
		selector->AddFilter((const UTF8Char*)"*.ani", (const UTF8Char*)"ANI File");
	}
}

IO::ParsedObject::ParserType Parser::FileParser::ANIParser::GetParserType()
{
	return IO::ParsedObject::PT_IMAGE_LIST_PARSER;
}

IO::ParsedObject *Parser::FileParser::ANIParser::ParseFile(IO::IStreamData *fd, IO::PackageFile *pkgFile, IO::ParsedObject::ParserType targetType)
{
	UInt32 hdr[6];
	UTF8Char sbuff[256];
	UInt32 aniSize;
	Media::ImageList *imgList;
	UInt8 *buff;
	UOSInt buffSize;
	UOSInt buffOfst;
	UInt32 tmp;
	Int32 displayRate = 0;
	UInt32 nFrames = 0;
	Media::ImageList *currImage = 0;

	fd->GetRealData(0, 24, (UInt8*)hdr);
	if (hdr[0] != *(UInt32*)"RIFF" || hdr[2] != *(UInt32*)"ACON")
	{
		return 0;
	}
	aniSize = hdr[1] + 8;
	UOSInt currOfst = 12;
	NEW_CLASS(imgList, Media::ImageList(fd->GetFullName()));

	while (currOfst < aniSize)
	{
		fd->GetRealData(currOfst, 12, (UInt8*)hdr);
		if (hdr[0] == *(UInt32*)"LIST" && hdr[2] == *(UInt32*)"INFO")
		{
			buffSize = hdr[1] - 4;
			buff = MemAlloc(UInt8, buffSize);
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
			MemFree(buff);
		}
		else if (hdr[0] == *(UInt32*)"anih")
		{
			buffSize = hdr[1];
			buff = MemAlloc(UInt8, buffSize);
			fd->GetRealData(currOfst + 8, buffSize, buff);
			displayRate = ReadInt32(&buff[28]);
			nFrames = ReadUInt32(&buff[4]);
			MemFree(buff);
		}
		else if (hdr[0] == *(UInt32*)"LIST" && hdr[2] == *(UInt32*)"fram")
		{
			buffSize = hdr[1] - 4;
			buff = MemAlloc(UInt8, 8);
			buffOfst = 0;
			while (buffOfst < buffSize)
			{
				fd->GetRealData(currOfst + 12 + buffOfst, 8, buff);
				tmp = *(UInt32*)&buff[0];
				if (tmp == *(UInt32*)"icon")
				{
					if (currImage)
					{
						Int32 imgTime;
						Media::Image *img = currImage->GetImage(0, &imgTime)->Clone();
						imgList->AddImage(img, MulDiv32(displayRate, 1000, 60));
						DEL_CLASS(currImage);
					}
					IO::IStreamData *data = fd->GetPartialData(currOfst + 20 + buffOfst, *(UInt32*)&buff[4]);
					currImage = (Media::ImageList *)this->icop->ParseFile(data, pkgFile, IO::ParsedObject::PT_IMAGE_LIST_PARSER);
					DEL_CLASS(data);
				}

				buffOfst += ReadUInt32(&buff[4]) + 8;
				if (buffOfst & 1)
				{
					buffOfst += 1;
				}
			}
			MemFree(buff);

			if (currImage)
			{
				Int32 imgTime;
				Media::Image *img = currImage->GetImage(0, &imgTime)->Clone();
				imgList->AddImage(img, MulDiv32(displayRate, 1000, 60));
				DEL_CLASS(currImage);
				currImage = 0;
			}
		}
		currOfst += hdr[1] + 8;
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
