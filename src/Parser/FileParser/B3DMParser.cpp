#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ByteTool.h"
#include "IO/PackageFile.h"
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

void Parser::FileParser::B3DMParser::PrepareSelector(IO::IFileSelector *selector, IO::ParserType t)
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

IO::ParsedObject *Parser::FileParser::B3DMParser::ParseFile(IO::IStreamData *fd, IO::PackageFile *pkgFile, IO::ParserType targetType)
{
	UInt8 hdrBuff[28];
	UTF8Char sbuff[256];
	UTF8Char *sptr;
	if (fd->GetRealData(0, 28, hdrBuff) != 28)
		return 0;
	if (ReadNInt32(&hdrBuff[0]) != *(Int32*)"b3dm" || ReadUInt32(&hdrBuff[8]) != fd->GetDataSize())
		return 0;

	UInt32 version = ReadUInt32(&hdrBuff[4]);
	UInt32 featureTableJSONByteLength = ReadUInt32(&hdrBuff[12]);
	UInt32 featureTableBinaryByteLength = ReadUInt32(&hdrBuff[16]);
	UInt32 batchTableJSONByteLength = ReadUInt32(&hdrBuff[20]);
	UInt32 batchTableBinaryByteLength = ReadUInt32(&hdrBuff[24]);

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

	IO::PackageFile *pf;
	NEW_CLASS(pf, IO::PackageFile(fd->GetFullName()));
	sptr = fd->GetShortName().ConcatTo(sbuff);
	sptr = Text::StrConcatC(sptr, UTF8STRC(".glb"));
	pf->AddData(fd, ofst, fd->GetDataSize() - ofst, CSTRP(sbuff, sptr), 0);

	return pf;
}
