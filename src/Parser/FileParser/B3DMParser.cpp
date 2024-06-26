#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ByteTool.h"
#include "IO/VirtualPackageFileFast.h"
#include "Parser/FileParser/B3DMParser.h"

Parser::FileParser::B3DMParser::B3DMParser()
{
}

Parser::FileParser::B3DMParser::~B3DMParser()
{
}

Int32 Parser::FileParser::B3DMParser::GetName()
{
	return *(Int32*)"B3DM";
}

void Parser::FileParser::B3DMParser::PrepareSelector(NN<IO::FileSelector> selector, IO::ParserType t)
{
	if (t == IO::ParserType::Unknown || t == IO::ParserType::PackageFile)
	{
		selector->AddFilter(CSTR("*.b3dm"), CSTR("Batched 3D Model"));
	}
}

IO::ParserType Parser::FileParser::B3DMParser::GetParserType()
{
	return IO::ParserType::PackageFile;
}

Optional<IO::ParsedObject> Parser::FileParser::B3DMParser::ParseFileHdr(NN<IO::StreamData> fd, Optional<IO::PackageFile> pkgFile, IO::ParserType targetType, Data::ByteArrayR hdr)
{
	UTF8Char sbuff[256];
	UnsafeArray<UTF8Char> sptr;
	if (ReadNInt32(&hdr[0]) != *(Int32*)"b3dm" || ReadUInt32(&hdr[8]) != fd->GetDataSize())
		return 0;

	UInt32 version = ReadUInt32(&hdr[4]);
	UInt32 featureTableJSONByteLength = ReadUInt32(&hdr[12]);
	UInt32 featureTableBinaryByteLength = ReadUInt32(&hdr[16]);
	UInt32 batchTableJSONByteLength = ReadUInt32(&hdr[20]);
	UInt32 batchTableBinaryByteLength = ReadUInt32(&hdr[24]);

	if (version != 1)
	{
		return 0;
	}
	UOSInt ofst = 28;
	if (featureTableJSONByteLength != 0)
	{
		return 0;
	}
	if (featureTableBinaryByteLength != 0)
	{
		return 0;
	}
	if (batchTableJSONByteLength != 0)
	{
		return 0;
	}
	if (batchTableBinaryByteLength != 0)
	{
		return 0;
	}

	IO::VirtualPackageFile *pf;
	NEW_CLASS(pf, IO::VirtualPackageFileFast(fd->GetFullName()));
	sptr = fd->GetShortName().OrEmpty().ConcatTo(sbuff);
	sptr = Text::StrConcatC(sptr, UTF8STRC(".glb"));
	pf->AddData(fd, ofst, fd->GetDataSize() - ofst, IO::PackFileItem::HeaderType::No, CSTRP(sbuff, sptr), 0, 0, 0, 0);

	return pf;
}
