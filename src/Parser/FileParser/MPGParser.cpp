#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ByteTool.h"
#include "IO/PackageFile.h"
#include "IO/StmData/ConcatStreamData.h"
#include "IO/StmData/FileData.h"
#include "Media/MPGFile.h"
#include "Parser/FileParser/MPGParser.h"

Parser::FileParser::MPGParser::MPGParser()
{
}

Parser::FileParser::MPGParser::~MPGParser()
{
}

Int32 Parser::FileParser::MPGParser::GetName()
{
	return *(Int32*)"MPGP";
}

void Parser::FileParser::MPGParser::PrepareSelector(IO::IFileSelector *selector, IO::ParserType t)
{
	if (t == IO::ParserType::Unknown || t == IO::ParserType::MediaFile)
	{
		selector->AddFilter((const UTF8Char*)"*.mpg", (const UTF8Char*)"MPEG-1 System Stream File");
		selector->AddFilter((const UTF8Char*)"*.m2p", (const UTF8Char*)"MPEG-2 Program Stream");
		selector->AddFilter((const UTF8Char*)"*.pss", (const UTF8Char*)"PS System Stream");
	}
}

IO::ParserType Parser::FileParser::MPGParser::GetParserType()
{
	return IO::ParserType::MediaFile;
}

IO::ParsedObject *Parser::FileParser::MPGParser::ParseFile(IO::IStreamData *fd, IO::PackageFile *pkgFile, IO::ParserType targetType)
{
	IO::IStreamData *concatFile = 0;
	UInt8 buff[256];
//	return 0;

	if (fd->GetRealData(0, 128, buff) != 128)
		return 0;
	if (ReadMInt32(&buff[0]) != 0x000001ba)
		return 0;


	if ((buff[4] & 0xf0) == 0x20) //MPG1
	{
		if (ReadMInt32(&buff[12]) != 0x000001bb)
			return 0;

		////////////////////////////
		//return 0;
		Media::MediaFile *file = 0;
		NEW_CLASS(file, Media::MPGFile(fd));
		return file;
	}
	else if ((buff[4] & 0xc0) == 0x40) //MPG2
	{
		Int32 i = (buff[13] & 7);
		Int64 currOfst = 14 + i;
		if (ReadMInt32(&buff[currOfst]) != 0x000001bb)
			return 0;

		if (fd->GetFullName()->EndsWithICase((const UTF8Char*)"_1.vob"))
		{
			UTF8Char sbuff[512];
			UTF8Char *sptr;
			if (fd->IsFullFile())
			{
				Int32 stmId;
				IO::StmData::ConcatStreamData *data;
				stmId = 2;
				NEW_CLASS(data, IO::StmData::ConcatStreamData(fd->GetFullName()));
				data->AddData(fd->GetPartialData(0, fd->GetDataSize()));
				
				Text::String *s = fd->GetFullFileName();
				sptr = Text::StrConcatC(sbuff, s->v, s->leng - 5);
				while (true)
				{
					Text::StrConcat(Text::StrInt32(sptr, stmId), (const UTF8Char*)".vob");
					NEW_CLASS(concatFile, IO::StmData::FileData(sbuff, false));
					if (concatFile->GetDataSize() <= 0)
					{
						DEL_CLASS(concatFile);
						break;
					}
					data->AddData(concatFile);
					stmId++;
				}
				concatFile = data;
				fd = data;
			}
			else if (pkgFile)
			{
				Int32 stmId;
				UOSInt ind;
				IO::StmData::ConcatStreamData *data;
				stmId = 2;
				NEW_CLASS(data, IO::StmData::ConcatStreamData(fd->GetFullName()));
				data->AddData(fd->GetPartialData(0, fd->GetDataSize()));
				
				sptr = Text::StrConcat(sbuff, fd->GetShortName()) - 5;
				while (true)
				{
					Text::StrConcat(Text::StrInt32(sptr, stmId), (const UTF8Char*)".vob");
					ind = pkgFile->GetItemIndex(sbuff);
					if (ind == INVALID_INDEX)
					{
						break;
					}
					concatFile = pkgFile->GetItemStmData(ind);
					if (concatFile == 0)
					{
						break;
					}
					data->AddData(concatFile);
					stmId++;
				}
				concatFile = data;
				fd = data;
			}
		}

		Media::MediaFile *file = 0;
		NEW_CLASS(file, Media::MPGFile(fd));
		SDEL_CLASS(concatFile);
		return file;
	}
	else
	{
		return 0;
	}
}
