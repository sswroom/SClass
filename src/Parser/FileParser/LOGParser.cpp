#include "Stdafx.h"
#include "MyMemory.h"
#include "IO/Path.h"
#include "IO/StreamDataStream.h"
#include "IO/StreamReader.h"
#include "IO/UDPLog.h"
#include "Parser/FileParser/LOGParser.h"
#include "Text/MyString.h"

Parser::FileParser::LOGParser::LOGParser()
{
	this->codePage = 0;
}

Parser::FileParser::LOGParser::~LOGParser()
{
}

Int32 Parser::FileParser::LOGParser::GetName()
{
	return *(Int32*)"LOGP";
}

void Parser::FileParser::LOGParser::SetCodePage(UInt32 codePage)
{
	this->codePage = codePage;
}

void Parser::FileParser::LOGParser::PrepareSelector(NN<IO::FileSelector> selector, IO::ParserType t)
{
	if (t == IO::ParserType::Unknown || t == IO::ParserType::LogFile)
	{
		selector->AddFilter(CSTR("*.log"), CSTR("Log File"));
	}
}

IO::ParserType Parser::FileParser::LOGParser::GetParserType()
{
	return IO::ParserType::LogFile;
}

Optional<IO::ParsedObject> Parser::FileParser::LOGParser::ParseFileHdr(NN<IO::StreamData> fd, Optional<IO::PackageFile> pkgFile, IO::ParserType targetType, Data::ByteArrayR hdr)
{
	UTF8Char sbuff[512];
	UnsafeArray<UTF8Char> sptr;
//	WChar baseDir[256];
//	WChar wbuff2[256];
//	WChar wbuff3[256];
//	WChar *filePath;
//	WChar *fileName;
//	WChar *sarr[10];
	UOSInt i;
//	OSInt j;
//	OSInt k;
	sptr = fd->GetFullName()->ConcatTo(sbuff);
	i = Text::StrLastIndexOfCharC(sbuff, (UOSInt)(sptr - sbuff), '.');
	if (i == INVALID_INDEX || !Text::StrEqualsICaseC(&sbuff[i], (UOSInt)(sptr - &sbuff[i]), UTF8STRC(".LOG")))
	{
		return nullptr;
	}
	IO::StreamDataStream stm(fd);
	IO::StreamReader reader(stm, this->codePage);
	if (!reader.ReadLine(sbuff, 255).SetTo(sptr))
	{
		return nullptr;
	}
	UOSInt strLen = (UOSInt)(sptr - sbuff);
	if (strLen >= 43 && sbuff[2] == ':' && sbuff[5] == ':' && sbuff[8] == ' ' && Text::StrEquals(&sbuff[9], (const UTF8Char*)"MSG start UDP server successfully!"))
	{
		IO::UDPLog *log = 0;
/*		NEW_CLASS(log, IO::LogFile(fd->GetFullFileName()));
		log->*/
		//////////////////////////////
		return log;
	}
	return nullptr;
}
