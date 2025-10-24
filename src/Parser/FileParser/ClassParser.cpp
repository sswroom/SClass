#include "Stdafx.h"
#include "MyMemory.h"
#include "Data/ByteBuffer.h"
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

void Parser::FileParser::ClassParser::PrepareSelector(NN<IO::FileSelector> selector, IO::ParserType t)
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

Optional<IO::ParsedObject> Parser::FileParser::ClassParser::ParseFileHdr(NN<IO::StreamData> fd, Optional<IO::PackageFile> pkgFile, IO::ParserType targetType, Data::ByteArrayR hdr)
{
	if (ReadMUInt32(&hdr[0]) != 0xCAFEBABE)
	{
		return 0;
	}
	UInt64 dsize = fd->GetDataSize();
	if (dsize < 26 || dsize > 1048576)
	{
		return 0;
	}
	Optional<IO::JavaClass> cls = 0;
	Data::ByteBuffer buff((UOSInt)dsize);
	if (fd->GetRealData(0, (UOSInt)dsize, buff) == dsize)
	{
		cls = IO::JavaClass::ParseBuff(fd->GetFullFileName(), buff);
	}
	return cls;
}
