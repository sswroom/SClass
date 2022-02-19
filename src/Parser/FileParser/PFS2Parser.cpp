#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ByteTool.h"
#include "IO/PackageFile.h"
#include "Parser/FileParser/PFS2Parser.h"
#include "Text/Encoding.h"
#include "Text/MyString.h"

Parser::FileParser::PFS2Parser::PFS2Parser()
{
}

Parser::FileParser::PFS2Parser::~PFS2Parser()
{
}

Int32 Parser::FileParser::PFS2Parser::GetName()
{
	return *(Int32*)"PFS2";
}

void Parser::FileParser::PFS2Parser::PrepareSelector(IO::IFileSelector *selector, IO::ParserType t)
{
	if (t == IO::ParserType::Unknown || t == IO::ParserType::PackageFile)
	{
		selector->AddFilter(CSTR("*.pfs"), CSTR("PFS Package File"));
	}
}

IO::ParserType Parser::FileParser::PFS2Parser::GetParserType()
{
	return IO::ParserType::PackageFile;
}

typedef struct
{
	Char id[3];
	UInt32 hdrSize;
	UInt32 reserved;
	UInt32 recCnt;
} PF2Header;

IO::ParsedObject *Parser::FileParser::PFS2Parser::ParseFile(IO::IStreamData *fd, IO::PackageFile *pkgFile, IO::ParserType targetType)
{
	Text::String *fileName = fd->GetFullName();
	if (!fileName->EndsWithICase(UTF8STRC(".pfs")))
		return 0;

	UInt8 hdr[15];
	UInt32 hdrSize;
	UInt8 *records;
	UTF8Char sbuff[256];
	OSInt fileCnt = 0;
	UOSInt i;
	Text::Encoding enc(932);

	fd->GetRealData(0, 15, (UInt8*)&hdr);
	if (hdr[0] != 'p' || hdr[1] != 'f' || hdr[2] != '2')
	{
		return 0;
	}
	hdrSize = ReadUInt32(&hdr[3]);
	if (hdrSize > fd->GetDataSize() - 7)
		return 0;
	records = MemAlloc(UInt8, hdrSize - 8);
	fd->GetRealData(15, hdrSize - 8, records);
	IO::PackageFile *pf = 0;
	NEW_CLASS(pf, IO::PackageFile(fd->GetFullName()));
	i = 0;
	while (i < hdrSize - 8)
	{
		UInt32 fileNameSize = ReadUInt32(&records[i]);
		UInt32 fileOfst = ReadUInt32(&records[i + fileNameSize + 16]);
		UInt32 fileSize = ReadUInt32(&records[i + fileNameSize + 20]);
		enc.UTF8FromBytes(sbuff, &records[i + 4], fileNameSize, 0);
		pf->AddData(fd, fileOfst, fileSize, sbuff, 0);
		fileCnt++;
		i += fileNameSize + 24;
	}

	if (fileCnt != ReadInt32(&hdr[11]))
	{
		DEL_CLASS(pf);
		return 0;
	}
	return pf;
}
