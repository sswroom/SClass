#include "Stdafx.h"
#include "MyMemory.h"
#include "Core/ByteTool_C.h"
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

void Parser::FileParser::MPGParser::PrepareSelector(NN<IO::FileSelector> selector, IO::ParserType t)
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

Optional<IO::ParsedObject> Parser::FileParser::MPGParser::ParseFileHdr(NN<IO::StreamData> fd, Optional<IO::PackageFile> pkgFile, IO::ParserType targetType, Data::ByteArrayR hdr)
{
	IO::StreamData *concatFile = 0;
	NN<IO::PackageFile> nnpkgFile;

	if (ReadMInt32(&hdr[0]) != 0x000001ba)
		return nullptr;


	if ((hdr[4] & 0xf0) == 0x20) //MPG1
	{
		if (ReadMInt32(&hdr[12]) != 0x000001bb)
			return nullptr;

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
		if (ReadMInt32(&hdr[(IntOS)currOfst]) != 0x000001bb)
			return nullptr;

		if (fd->GetFullName()->EndsWithICase(UTF8STRC("_1.vob")))
		{
			UTF8Char sbuff[512];
			UnsafeArray<UTF8Char> sptr;
			UnsafeArray<UTF8Char> sptr2;
			if (fd->IsFullFile())
			{
				Int32 stmId;
				NN<IO::StmData::ConcatStreamData> data;
				stmId = 2;
				NEW_CLASSNN(data, IO::StmData::ConcatStreamData(fd->GetFullName()));
				data->AddData(fd->GetPartialData(0, fd->GetDataSize()));
				
				NN<Text::String> s = fd->GetFullFileName();
				sptr = Text::StrConcatC(sbuff, s->v, s->leng - 5);
				while (true)
				{
					sptr2 = Text::StrConcatC(Text::StrInt32(sptr, stmId), UTF8STRC(".vob"));
					NEW_CLASSNN(fd, IO::StmData::FileData(CSTRP(sbuff, sptr2), false));
					if (fd->GetDataSize() <= 0)
					{
						fd.Delete();
						break;
					}
					data->AddData(fd);
					stmId++;
				}
				concatFile = data.Ptr();
				fd = data;
			}
			else if (pkgFile.SetTo(nnpkgFile))
			{
				Int32 stmId;
				UIntOS ind;
				NN<IO::StmData::ConcatStreamData> data;
				stmId = 2;
				NEW_CLASSNN(data, IO::StmData::ConcatStreamData(fd->GetFullName()));
				data->AddData(fd->GetPartialData(0, fd->GetDataSize()));
				
				sptr = fd->GetShortName().OrEmpty().ConcatTo(sbuff) - 5;
				while (true)
				{
					sptr2 = Text::StrConcatC(Text::StrInt32(sptr, stmId), UTF8STRC(".vob"));
					ind = nnpkgFile->GetItemIndex(CSTRP(sbuff, sptr2));
					if (ind == INVALID_INDEX)
					{
						break;
					}
					if (!nnpkgFile->GetItemStmDataNew(ind).SetTo(fd))
					{
						break;
					}
					data->AddData(fd);
					stmId++;
				}
				concatFile = data.Ptr();
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
		return nullptr;
	}
}
