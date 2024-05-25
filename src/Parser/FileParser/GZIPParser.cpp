#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ByteTool.h"
#include "IO/VirtualPackageFileFast.h"
#include "Parser/FileParser/GZIPParser.h"
#include "Text/Encoding.h"
#include "Text/MyString.h"

Parser::FileParser::GZIPParser::GZIPParser()
{
}

Parser::FileParser::GZIPParser::~GZIPParser()
{
}

Int32 Parser::FileParser::GZIPParser::GetName()
{
	return *(Int32*)"GZIP";
}

void Parser::FileParser::GZIPParser::PrepareSelector(NN<IO::FileSelector> selector, IO::ParserType t)
{
	if (t == IO::ParserType::Unknown || t == IO::ParserType::PackageFile)
	{
		selector->AddFilter(CSTR("*.gz"), CSTR("GZIP File"));
	}
}

IO::ParserType Parser::FileParser::GZIPParser::GetParserType()
{
	return IO::ParserType::PackageFile;
}

Optional<IO::ParsedObject> Parser::FileParser::GZIPParser::ParseFileHdr(NN<IO::StreamData> fd, Optional<IO::PackageFile> pkgFile, IO::ParserType targetType, Data::ByteArrayR hdr)
{
	UInt8 footer[8];
	UTF8Char sbuff[512];
	UOSInt byteConv = 0;
	UInt64 fileLeng;
	UTF8Char *sptr;

	if (hdr[0] != 0x1f || hdr[1] != 0x8b || hdr[2] != 8)
	{
		return 0;
	}
	sbuff[0] = 0;
	if (hdr[3] & 1)
		return 0;
	if (hdr[3] & 2)
		return 0;
	if (hdr[3] & 4)
		return 0;
	if (hdr[3] & 8)
	{
		sptr = Text::StrConcat(sbuff, &hdr[10]);
		byteConv = (UOSInt)(sptr - sbuff);
		if (byteConv >= 247)
			return 0;
		
	}
	else
	{
		sptr = fd->GetShortName().ConcatTo(sbuff);
		if ((sptr - sbuff) > 3 && Text::StrEqualsC(&sptr[-3], 3, UTF8STRC(".gz")))
		{
			sptr[-3] = 0;
			sptr -= 3;
		}
		else
		{
			sptr[0] = 0;
		}
	}
	fileLeng = fd->GetDataSize();
	fd->GetRealData(fileLeng - 8, 8, BYTEARR(footer));

	IO::VirtualPackageFile *pf;
	IO::PackFileItem::CompressInfo cinfo;

	cinfo.decSize = *(UInt32*)&footer[4];
	cinfo.compMethod = Data::Compress::Decompressor::CM_DEFLATE;
	cinfo.checkMethod = Crypto::Hash::HashType::CRC32R_IEEE;
	cinfo.compFlags = 0;
	cinfo.compExtraSize = 0;
	cinfo.compExtras = 0;
	*(Int32*)cinfo.checkBytes = *(Int32*)footer;
	NEW_CLASS(pf, IO::VirtualPackageFileFast(fd->GetFullName()));
	pf->AddCompData(fd, 10 + byteConv, fileLeng - 18 - byteConv, IO::PackFileItem::HeaderType::No, &cinfo, CSTRP(sbuff, sptr), Data::Timestamp(ReadUInt32(&hdr[4]) * 1000LL, 0), 0, 0, 0);

	return pf;
}
