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

void Parser::FileParser::MPGParser::PrepareSelector(IO::FileSelector *selector, IO::ParserType t)
{
	if (t == IO::ParserType::Unknown || t == IO::ParserType::MediaFile)
	{
		selector->AddFilter(CSTR("*.mpg"), CSTR("MPEG-1 System Stream File"));
		selector->AddFilter(CSTR("*.m2p"), CSTR("MPEG-2 Program Stream"));
		selector->AddFilter(CSTR("*.pss"), CSTR("PS System Stream"));
	}
}

IO::ParserType Parser::FileParser::MPGParser::GetParserType()
{
	return IO::ParserType::MediaFile;
}

IO::ParsedObject *Parser::FileParser::MPGParser::ParseFileHdr(IO::StreamData *fd, IO::PackageFile *pkgFile, IO::ParserType targetType, const UInt8 *hdr)
{
	IO::StreamData *concatFile = 0;

	if (ReadMInt32(&hdr[0]) != 0x000001ba)
		return 0;


	if ((hdr[4] & 0xf0) == 0x20) //MPG1
	{
		if (ReadMInt32(&hdr[12]) != 0x000001bb)
			return 0;

		////////////////////////////
		//return 0;
		Media::MediaFile *file = 0;
		NEW_CLASS(file, Media::MPGFile(fd));
		return file;
	}
	else if ((hdr[4] & 0xc0) == 0x40) //MPG2
	{
		Int32 i = (hdr[13] & 7);
		Int64 currOfst = 14 + i;
		if (ReadMInt32(&hdr[currOfst]) != 0x000001bb)
			return 0;

		if (fd->GetFullName()->EndsWithICase(UTF8STRC("_1.vob")))
		{
			UTF8Char sbuff[512];
			UTF8Char *sptr;
			UTF8Char *sptr2;
			if (fd->IsFullFile())
			{
				Int32 stmId;
				IO::StmData::ConcatStreamData *data;
				stmId = 2;
				NEW_CLASS(data, IO::StmData::ConcatStreamData(fd->GetFullName()));
				data->AddData(fd->GetPartialData(0, fd->GetDataSize()));
				
				NotNullPtr<Text::String> s = fd->GetFullFileName();
				sptr = Text::StrConcatC(sbuff, s->v, s->leng - 5);
				while (true)
				{
					sptr2 = Text::StrConcatC(Text::StrInt32(sptr, stmId), UTF8STRC(".vob"));
					NEW_CLASS(concatFile, IO::StmData::FileData(CSTRP(sbuff, sptr2), false));
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
				
				sptr = fd->GetShortName().ConcatTo(sbuff) - 5;
				while (true)
				{
					sptr2 = Text::StrConcatC(Text::StrInt32(sptr, stmId), UTF8STRC(".vob"));
					ind = pkgFile->GetItemIndex(CSTRP(sbuff, sptr2));
					if (ind == INVALID_INDEX)
					{
						break;
					}
					concatFile = pkgFile->GetItemStmDataNew(ind);
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
