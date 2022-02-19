#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ByteTool.h"
#include "IO/JavaClass.h"
#include "Parser/FileParser/ClassParser.h"

Parser::FileParser::ClassParser::ClassParser()
{
}

Parser::FileParser::ClassParser::~ClassParser()
{
}

Int32 Parser::FileParser::ClassParser::GetName()
{
	return *(Int32*)"JCLS";
}

void Parser::FileParser::ClassParser::PrepareSelector(IO::IFileSelector *selector, IO::ParserType t)
{
	if (t == IO::ParserType::Unknown || t == IO::ParserType::JavaClass)
	{
		selector->AddFilter(CSTR("*.class"), CSTR("Class File"));
	}
}

IO::ParserType Parser::FileParser::ClassParser::GetParserType()
{
	return IO::ParserType::JavaClass;
}

IO::ParsedObject *Parser::FileParser::ClassParser::ParseFile(IO::IStreamData *fd, IO::PackageFile *pkgFile, IO::ParserType targetType)
{
	UInt8 hdr[64];
	if (fd->GetRealData(0, 64, hdr) != 64)
	{
		return 0;
	}
	if (ReadMUInt32(&hdr[0]) != 0xCAFEBABE)
	{
		return 0;
	}
	UInt64 dsize = fd->GetDataSize();
	if (dsize < 26 || dsize > 1048576)
	{
		return 0;
	}
	IO::JavaClass *cls = 0;
	UInt8 *buff = MemAlloc(UInt8, (UOSInt)dsize);
	if (fd->GetRealData(0, (UOSInt)dsize, buff) == dsize)
	{
		cls = IO::JavaClass::ParseBuff(fd->GetFullFileName(), buff, (UOSInt)dsize);
	}
	MemFree(buff);
	return cls;
}
